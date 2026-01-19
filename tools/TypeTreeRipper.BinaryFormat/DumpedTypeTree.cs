namespace TypeTreeRipper.BinaryFormat;

public class DumpedTypeTree
{
	public DumpedTypeTreeRTTI RTTI { get; }
	public TransferInstructionFlags TransferFlags { get; }
	public List<DumpedTypeTreeNode> Nodes { get; }

	public bool IsReleaseTree => TransferFlags.HasFlag(TransferInstructionFlags.SerializeGameRelease);

	public DumpedTypeTree(BinaryReader reader)
	{
		RTTI = new DumpedTypeTreeRTTI(reader);
		TransferFlags = (TransferInstructionFlags)reader.ReadUInt64();

		var count = reader.ReadUInt32();
		Nodes = new List<DumpedTypeTreeNode>(checked((int)count));
		for (int i = 0; i < count; i++)
		{
			Nodes.Add(new DumpedTypeTreeNode(reader));
		}
	}

	public int GetValueHash()
	{
		var hashCode = new HashCode();

		hashCode.Add(RTTI.GetValueHash());
		hashCode.Add(TransferFlags);
		
		foreach (var node in Nodes)
		{
			hashCode.Add(node.GetValueHash());
		}

		return hashCode.ToHashCode();
	}
}
