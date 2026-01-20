using System.Buffers;
using System.Buffers.Binary;
using System.Diagnostics;
using System.Text;
using AssetRipper.HashAlgorithms;
using AssetRipper.Primitives;
using TypeTreeRipper.BinaryFormat;

namespace TypeTreeRipper.Converter.Features.StructDump;

public static class StructDumpBinaryOutput
{
    public static void Output(TypeTreeBinary binary, string outputPath)
    {
        using var fs = File.OpenWrite(outputPath);
        using var writer = new BinaryWriter(fs);
        Output(binary, writer);
    }

    private static void Output(TypeTreeBinary binary, BinaryWriter writer)
    {
        var typesByTypeId = binary.TypeTrees.ToDictionary(x => x.RTTI.PersistentTypeId);

        var revision = new UnityVersion(binary.Header.MajorRevision, binary.Header.MinorRevision,
            binary.Header.PatchRevision);

        writer.WriteNullTerminatedString(revision.ToString());

        writer.Write((byte)7); // RuntimePlatform.WindowsEditor
        writer.Write((byte)1); // HasTypeTrees

        var typeCount = 0;
        var typeCountPosition = writer.BaseStream.Position;
        writer.BaseStream.Position += 4; // Will be filled in later

        var oldTypeHashBuffer = (stackalloc byte[16]);
        foreach (var type in binary.TypeTrees.OrderBy(x => x.RTTI.PersistentTypeId))
        {
            // NOTE: This logic does not make sense to me.
            // Why does this use the persistent type ID of the first non-abstract base type?
            // I think this just never gets hit in practice (i.e. always fails out), but the ancient tools all do this.

            var nonAbstractBaseType = type;
            while (nonAbstractBaseType.RTTI.Flags.HasFlag(DumpedTypeTreeRTTIFlags.IsAbstract)
                   && nonAbstractBaseType.RTTI.BasePersistentTypeId != -1)
            {
                nonAbstractBaseType = typesByTypeId[nonAbstractBaseType.RTTI.BasePersistentTypeId];
            }

            if (nonAbstractBaseType.RTTI.Flags.HasFlag(DumpedTypeTreeRTTIFlags.IsAbstract))
                continue;

            writer.Write(nonAbstractBaseType.RTTI.PersistentTypeId);

            // The ancient tools do a check for negative persistent type IDs here, but this check never succeeds
            // as they were only ever part of MonoBehavior type ids to signal special handling
            // our type IDs are always positive
            nonAbstractBaseType.CalculateOldTypeHash(oldTypeHashBuffer);
            writer.Write(oldTypeHashBuffer);

            writer.Write(nonAbstractBaseType.Nodes.Count);

            var stringBufferBuilder = new StringBufferBuilder();
            var stringBufferSizePosition = writer.BaseStream.Position;
            writer.BaseStream.Position += 4; // Will be filled in later

            foreach (var node in nonAbstractBaseType.Nodes)
            {
                writer.Write(node.Version);
                writer.Write(node.Level);
                writer.Write((byte)node.Flags);
                writer.Write(stringBufferBuilder.AddString(node.Type));
                writer.Write(stringBufferBuilder.AddString(node.Name));
                writer.Write(node.ByteSize);
                writer.Write(node.Index);
                writer.Write((uint)node.MetaFlags);
            }

            stringBufferBuilder.Write(writer);

            var nodeEndPosition = writer.BaseStream.Position;
            writer.BaseStream.Position = stringBufferSizePosition;
            writer.Write(stringBufferBuilder.Length);

            writer.BaseStream.Position = nodeEndPosition;

            typeCount++;
        }

        var writerEndPosition = writer.BaseStream.Position;

        writer.BaseStream.Position = typeCountPosition;
        writer.Write(typeCount);

        writer.BaseStream.Position = writerEndPosition;
    }

    private ref struct StringBufferBuilder()
    {
        private readonly Dictionary<string, int> _containedStrings = [];
        private readonly List<string> _stringBufferData = [];

        public int Length { get; private set; } = 0;

        public int AddString(string str)
        {
            if (_containedStrings.TryGetValue(str, out var existingOffset))
            {
                return existingOffset;
            }

            var offset = Length;
            _containedStrings[str] = offset;
            _stringBufferData.Add(str);
            Length = offset + Encoding.UTF8.GetByteCount(str) + 1;

            return offset;
        }

        public void Write(BinaryWriter writer)
        {
            foreach (var value in _stringBufferData)
            {
                writer.WriteNullTerminatedString(value);
            }
        }
    }

    extension(DumpedTypeTree typeTree)
    {
        private void CalculateOldTypeHash(Span<byte> destination)
        {
            Debug.Assert(destination.Length == 16);

            var ctx = new MD4();

            foreach (var node in typeTree.Nodes)
            {
                ctx.TransformString(node.Type);
                ctx.TransformString(node.Name);
                ctx.TransformUInt32((uint)node.ByteSize);
                ctx.TransformUInt32((uint)node.Flags);
                ctx.TransformUInt32((uint)node.Version);
                ctx.TransformUInt32((uint)(node.MetaFlags & DumpedTypeTreeNodeMetaFlags.DebugProperty));
            }

            ctx.ComputeHash([], destination);
        }
    }

    extension(BinaryWriter writer)
    {
        private void WriteNullTerminatedString(string value)
        {
            writer.Write(Encoding.UTF8.GetBytes(value));
            writer.Write(byte.MinValue);
        }
    }

    extension(MD4 md4)
    {
        private void TransformString(string value)
        {
            var data = ArrayPool<byte>.Shared.Rent(Encoding.UTF8.GetByteCount(value));
            var byteCount = Encoding.UTF8.GetBytes(value, data);
            md4.TransformBlock(data, 0, byteCount, null, 0);
            ArrayPool<byte>.Shared.Return(data);
        }

        private void TransformUInt32(uint value)
        {
            var data = ArrayPool<byte>.Shared.Rent(sizeof(uint));
            BinaryPrimitives.WriteUInt32LittleEndian(data, value);
            md4.TransformBlock(data, 0, sizeof(uint), null, 0);
            ArrayPool<byte>.Shared.Return(data);
        }
    }
}
