namespace TypeTreeRipper.BinaryFormat;

public class TypeTreeBinary
{
	public const ulong ExpectedMagic = 0x4545525445505954; // 'TYPETREE', little-endian
	public const uint ExpectedVersion = 1;

	public DumpedTypeTreeHeader Header { get; }
	public List<DumpedTypeTree> TypeTrees { get; }

	public TypeTreeBinary(BinaryReader reader)
	{
		Header = new DumpedTypeTreeHeader(reader);

		if (Header.Magic != ExpectedMagic)
		{
			throw new InvalidDataException($"Invalid magic number: {Header.Magic:X16}");
		}

		if (Header.Version != ExpectedVersion)
		{
			throw new InvalidDataException($"Unsupported version: {Header.Version}");
		}

		var count = reader.ReadUInt32();
		TypeTrees = new List<DumpedTypeTree>(checked((int)count));
		for (int i = 0; i < count; i++)
		{
			TypeTrees.Add(new DumpedTypeTree(reader));
		}
	}

	public static TypeTreeBinary FromFile(string filePath)
	{
		using var fs = File.OpenRead(filePath);
		return FromStream(fs);
	}

	public static TypeTreeBinary FromStream(Stream stream)
	{
		using var reader = new BinaryReader(stream);
		return new TypeTreeBinary(reader);
	}
}
