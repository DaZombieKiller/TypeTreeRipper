namespace TypeTreeRipper.BinaryFormat;

public class DumpedTypeTreeRTTI(BinaryReader reader)
{
	public string ClassName { get; } = reader.ReadLengthPrefixedString();
	public string ClassNamespace { get; } = reader.ReadLengthPrefixedString();
	public string Module { get; } = reader.ReadLengthPrefixedString();
	public int PersistentTypeId { get; } = reader.ReadInt32();
	public int Size { get; } = reader.ReadInt32();

	public DumpedTypeTreeRTTIFlags Flags { get; } = (DumpedTypeTreeRTTIFlags)reader.ReadUInt32();

	public int BasePersistentTypeId { get; } = reader.ReadInt32();

	public uint DerivedFromTypeIndex { get; } = reader.ReadUInt32();
	public uint DerivedFromDescendantCount { get; } = reader.ReadUInt32();

	public int GetValueHash()
	{
		var hashCode = new HashCode();
		hashCode.Add(ClassName);
		hashCode.Add(ClassNamespace);
		hashCode.Add(Module);
		hashCode.Add(PersistentTypeId);
		hashCode.Add(Size);
		hashCode.Add(Flags);
		hashCode.Add(BasePersistentTypeId);
		hashCode.Add(DerivedFromTypeIndex);
		hashCode.Add(DerivedFromDescendantCount);
		return hashCode.ToHashCode();
	}
}