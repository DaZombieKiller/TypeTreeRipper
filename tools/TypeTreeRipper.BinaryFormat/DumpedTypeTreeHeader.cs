namespace TypeTreeRipper.BinaryFormat;

public class DumpedTypeTreeHeader(BinaryReader reader)
{
	public ulong Magic { get; } = reader.ReadUInt64();
	public uint Version { get; } = reader.ReadUInt32();

	public ushort MajorRevision { get; } = reader.ReadUInt16();
	public byte MinorRevision { get; } = reader.ReadByte();
	public byte PatchRevision { get; } = reader.ReadByte();

	public string Variant { get; } = reader.ReadLengthPrefixedString();
}
