using AssetRipper.Primitives;
using AssetRipper.Tpk;
using AssetRipper.Tpk.TypeTrees;
using TypeTreeRipper.BinaryFormat;
using VersionClassPair = System.Collections.Generic.KeyValuePair<
    AssetRipper.Primitives.UnityVersion,
    AssetRipper.Tpk.TypeTrees.TpkUnityClass?>;

namespace TypeTreeRipper.Converter.Features.Tpk;

public static class TpkOutput
{
    public static void Output(string inputFolder, string outputPath, bool emitAllCompressions)
    {
        var orderedBinaries = GetOrderedBinariesByVersion(inputFolder);
        var tpkBlob = Create(orderedBinaries);

        if (emitAllCompressions)
        {
            WriteTpk(tpkBlob, TpkCompressionType.None, Path.Join(outputPath, "uncompressed.tpk"));
            WriteTpk(tpkBlob, TpkCompressionType.Lz4, Path.Join(outputPath, "lz4.tpk"));
            WriteTpk(tpkBlob, TpkCompressionType.Lzma, Path.Join(outputPath, "lzma.tpk"));
            WriteTpk(tpkBlob, TpkCompressionType.Brotli, Path.Join(outputPath, "brotli.tpk"));
        }
        else
        {
            WriteTpk(tpkBlob, TpkCompressionType.None, outputPath);
        }
    }

    private static void WriteTpk(TpkDataBlob blob, TpkCompressionType compressionType, string outputPath)
    {
        TpkFile.FromBlob(blob, compressionType).WriteToFile(outputPath);
        Console.WriteLine($"TPK with {compressionType} compression saved to {outputPath}");
    }

    private static IEnumerable<(UnityVersion, TypeTreeBinary)> GetOrderedBinariesByVersion(string directoryPath)
    {
        var info = new DirectoryInfo(directoryPath);
        if (!info.Exists)
        {
            throw new ArgumentException(null, nameof(directoryPath));
        }

        var files = new Dictionary<UnityVersion, List<string>>();
        foreach (var directory in info.GetDirectories())
        {
            UnityVersion version = default;

            foreach (var file in directory.GetFiles())
            {
                if (file.Extension == ".ttbin")
                {
                    if (version == default)
                    {
                        version = UnityVersion.Parse(directory.Name);
                    }

                    if (files.TryGetValue(version, out var fileList))
                    {
                        fileList.Add(file.FullName);
                    }
                    else
                    {
                        files[version] = [file.FullName];
                    }
                }
            }
        }

        var orderedVersions = files.Select(pair => pair.Key).ToList();
        orderedVersions.Sort();

        return orderedVersions.SelectMany(version => files[version].Select(x => (version, TypeTreeBinary.FromFile(x))));
    }

    private static TpkTypeTreeBlob Create(IEnumerable<(UnityVersion, TypeTreeBinary)> typeTreeBinariesOrderedByUnityVersion)
    {
        var blob = new TpkTypeTreeBlob();
        blob.CommonString.Add(UnityVersion.MinVersion, 0);

        var latestUnityClassesDumped = new Dictionary<int, int>();
        var classDictionary = new Dictionary<int, TpkClassInformation>();

        var versionClasses = new Dictionary<UnityVersion, Dictionary<int, TpkUnityClass>>();

        foreach (var (fullVersion, typeTreeBinary) in typeTreeBinariesOrderedByUnityVersion)
        {
            if (!versionClasses.TryGetValue(fullVersion, out var classesByTypeId))
            {
                versionClasses[fullVersion] = classesByTypeId = new Dictionary<int, TpkUnityClass>();
            }

            Console.WriteLine(fullVersion);
            blob.Versions.Add(fullVersion);

            var conversionContext = new TpkConversionContext(typeTreeBinary);
            foreach (var typeTree in typeTreeBinary.TypeTrees)
            {
                var currentTypeTreeValueHash = typeTree.GetValueHash();
                if (!latestUnityClassesDumped.TryGetValue(typeTree.RTTI.PersistentTypeId, out var cachedHashCode)
                    || cachedHashCode != currentTypeTreeValueHash)
                {
                    latestUnityClassesDumped[typeTree.RTTI.PersistentTypeId] = currentTypeTreeValueHash;
                    if (!classDictionary.TryGetValue(typeTree.RTTI.PersistentTypeId, out var tpkClassInformation))
                    {
                        tpkClassInformation = new TpkClassInformation(typeTree.RTTI.PersistentTypeId);
                        classDictionary.Add(typeTree.RTTI.PersistentTypeId, tpkClassInformation);
                    }

                    if (classesByTypeId.TryGetValue(typeTree.RTTI.PersistentTypeId, out var tpkUnityClass))
                    {
                        TpkConversionContext.Merge(tpkUnityClass, typeTree, blob.StringBuffer, blob.NodeBuffer);
                    }
                    else
                    {
                        tpkUnityClass = conversionContext.Convert(typeTree, blob.StringBuffer, blob.NodeBuffer);
                        classesByTypeId[typeTree.RTTI.PersistentTypeId] = tpkUnityClass;
                    }

                    tpkClassInformation.Classes.Add(new VersionClassPair(fullVersion, tpkUnityClass));
                }
            }

            var typeIds = typeTreeBinary.TypeTrees.Select(c => c.RTTI.PersistentTypeId).ToList();
            foreach (var unusedId in classDictionary.Keys.Where(id => !typeIds.Contains(id)))
            {
                if (latestUnityClassesDumped.Remove(unusedId))
                {
                    classDictionary[unusedId].Classes.Add(new VersionClassPair(fullVersion, null));
                }
            }
        }

        PostProcessCreatedBlob(blob, classDictionary);

        return blob;
    }

    private static void PostProcessCreatedBlob(TpkTypeTreeBlob blob, Dictionary<int, TpkClassInformation> classDictionary)
    {
        foreach (var tpkClassInfo in classDictionary.Values)
        {
            var pairs = tpkClassInfo.Classes.ToArray();
            var previousClass = pairs[0].Value;

            for (var i = 1; i < pairs.Length; i++)
            {
                var pair = pairs[i];

                if (pair.Value == previousClass)
                {
                    tpkClassInfo.Classes.Remove(pair);
                }
                else
                {
                    previousClass = pair.Value;
                }
            }
        }

        blob.ClassInformation.AddRange(classDictionary.Values);
        blob.CreationTime = DateTime.Now.ToUniversalTime();
    }
}
