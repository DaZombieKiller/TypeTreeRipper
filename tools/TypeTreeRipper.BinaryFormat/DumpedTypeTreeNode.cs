namespace TypeTreeRipper.BinaryFormat;

public class DumpedTypeTreeNode(BinaryReader reader)
{
	public string Type { get; } = reader.ReadLengthPrefixedString();
	public string Name { get; } = reader.ReadLengthPrefixedString();

	public DumpedTypeTreeNodeFlags Flags { get; } = (DumpedTypeTreeNodeFlags)reader.ReadUInt32();
	public int ByteSize { get; } = reader.ReadInt32();
	public int Index { get; } = reader.ReadInt32();

	public short Version { get; } = reader.ReadInt16();
	public byte Level { get; } = reader.ReadByte();

	public DumpedTypeTreeNodeMetaFlags MetaFlags { get; } = (DumpedTypeTreeNodeMetaFlags)reader.ReadUInt32();
	public ulong RefTypeHash { get; } = reader.ReadUInt64();

	public int GetValueHash()
	{
		var hashCode = new HashCode();
		hashCode.Add(Type);
		hashCode.Add(Name);
		hashCode.Add(Flags);
		hashCode.Add(ByteSize);
		hashCode.Add(Index);
		hashCode.Add(Version);
		hashCode.Add(Level);
		hashCode.Add(MetaFlags);
		hashCode.Add(RefTypeHash);
		return hashCode.ToHashCode();
	}
}