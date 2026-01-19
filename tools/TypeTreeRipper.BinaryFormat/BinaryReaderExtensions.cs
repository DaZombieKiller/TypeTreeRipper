using System.Text;

namespace TypeTreeRipper.BinaryFormat;

public static class BinaryReaderExtensions
{
	extension(BinaryReader reader)
	{
		public string ReadLengthPrefixedString()
		{
			var size = reader.ReadUInt32();
			var bytes = reader.ReadBytes(checked((int)size));
			return Encoding.UTF8.GetString(bytes);
		}
	}
}
