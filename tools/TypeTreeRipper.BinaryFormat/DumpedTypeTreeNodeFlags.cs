namespace TypeTreeRipper.BinaryFormat;

[Flags]
public enum DumpedTypeTreeNodeFlags : uint
{
	IsArray = 1 << 0,
	IsManagedReference = 1 << 1,
	IsManagedReferenceRegistry = 1 << 2,
	IsArrayOfRefs = 1 << 3
}
