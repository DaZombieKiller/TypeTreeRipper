using TypeTreeRipper.BinaryFormat;

namespace TypeTreeRipper.Converter.Features.StructDump;

public static class StructDumpTextOutput
{
    public static void Output(TypeTreeBinary binary, string outputPath)
    {
        using var output = File.OpenWrite(outputPath);
        using var writer = new StreamWriter(output);
        Output(binary, writer);
    }

    private static void Output(TypeTreeBinary binary, StreamWriter writer)
    {
        var typesByTypeId = binary.TypeTrees.ToDictionary(x => x.RTTI.PersistentTypeId);

        foreach (var type in binary.TypeTrees.OrderBy(x => x.RTTI.PersistentTypeId))
        {
            writer.Write($"\n// classID{{{type.RTTI.PersistentTypeId}}}: ");

            var inheritanceCurrentType = type;
            while (true)
            {
                writer.Write(inheritanceCurrentType.RTTI.ClassName);
                if (inheritanceCurrentType.RTTI.BasePersistentTypeId == -1)
                    break;

                writer.Write(" <- ");
                inheritanceCurrentType = typesByTypeId[inheritanceCurrentType.RTTI.BasePersistentTypeId];
            }

            writer.WriteLine();

            var abstractCurrentType = type;
            while (abstractCurrentType.RTTI.Flags.HasFlag(DumpedTypeTreeRTTIFlags.IsAbstract))
            {
                writer.WriteLine($"// {abstractCurrentType.RTTI.ClassName} is abstract");
                if (abstractCurrentType.RTTI.BasePersistentTypeId == -1)
                    break;

                abstractCurrentType = typesByTypeId[abstractCurrentType.RTTI.BasePersistentTypeId];
            }

            foreach (var node in abstractCurrentType.Nodes)
                Output(node, writer);
        }
    }

    private static void Output(DumpedTypeTreeNode node, StreamWriter writer)
    {
        for (int i = 0; i < node.Level; i++)
            writer.Write("\t");

        writer.WriteLine($"{node.Type} {node.Name} " +
                     $"// ByteSize{{{node.ByteSize:x}}}, " +
                     $"Index{{{node.Index:x}}}, " +
                     $"Version{{{node.Version:x}}}, " +
                     $"IsArray{{{(uint)node.Flags:x}}}, " +
                     $"MetaFlag{{{(uint)node.MetaFlags:x}}}");
    }
}
