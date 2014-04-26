#region License
/* UnXNB - Quick and dirty XNB decompressor ganked from FNA
 * Copyright 2009-2014 Ethan Lee and the MonoGame Team
 *
 * Released under the Microsoft Public License.
 * See unxnb.LICENSE for details.
 */
#endregion

#region Using Statements
using System;
using System.IO;
using Microsoft.Xna.Framework.Content;
#endregion

public class UnXNB
{
	public static void Main(string[] args)
	{
		if (args.Length < 1)
		{
			Console.WriteLine("Usage: UnXNB.exe effectFile.xnb");
		}
		for (int i = 0; i < args.Length; i += 1)
		{
			MemoryStream decompressedStream;
			using (Stream fileIn = File.OpenRead(args[i]))
			using (BinaryReader reader = new BinaryReader(fileIn))
			{
				if (	reader.ReadByte() != 'X' ||
					reader.ReadByte() != 'N' ||
					reader.ReadByte() != 'B' ||
					reader.ReadByte() != 'w'	)
				{
					Console.WriteLine("Not an XNB file!");
					continue;
				}
				byte version = reader.ReadByte();
				if (version != 5 && version != 4)
				{
					Console.WriteLine("XNB version too old");
					continue;
				}
				if ((reader.ReadByte() & 0x80) == 0)
				{
					Console.WriteLine("XNB not compressed!");
					continue;
				}
				int compressedSize = reader.ReadInt32();
				int decompressedSize = reader.ReadInt32();
				int decodedBytes = 0;
				long startPos = fileIn.Position;
				long pos = startPos;
				LzxDecoder decoder = new LzxDecoder(16);
				decompressedStream = new MemoryStream(decompressedSize);
				while (pos - startPos < compressedSize)
				{
					int hi = reader.ReadByte();
					int lo = reader.ReadByte();
					int blockSize = (hi << 8) | lo;
					int frameSize = 0x8000;
					if (hi == 0xFF)
					{
						hi = lo;
						lo = reader.ReadByte();
						frameSize = (hi << 8) | lo;
						hi = reader.ReadByte();
						lo = reader.ReadByte();
						blockSize = (hi << 8) | lo;
						pos += 5;
					}
					else
					{
						pos += 2;
					}
					if (blockSize == 0 || frameSize == 0)
					{
						break;
					}
					decoder.Decompress(
						fileIn,
						blockSize,
						decompressedStream,
						frameSize
					);
					pos += blockSize;
					decodedBytes += frameSize;
					fileIn.Seek(pos, SeekOrigin.Begin);
				}
				if (decompressedStream.Position != decompressedSize)
				{
					Console.WriteLine("Decompression failure!");
					decompressedStream.Close();
					continue;
				}
			}

			// Write to file, cutting out initial XNA crap
			decompressedStream.Seek(0, SeekOrigin.Begin);
			using (BinaryReader trim = new BinaryReader(decompressedStream))
			using (FileStream fileOut = new FileStream(
				Path.GetFileNameWithoutExtension(args[i]) + ".fxb",
				FileMode.Create
			)) {
				trim.ReadByte();	// 7-bit encoded int
				trim.ReadString();	// Type info
				trim.ReadUInt32();	// Version?
				trim.ReadByte();	// 7-bit encoded int
				trim.ReadByte();	// 7-bit encoded int
				decompressedStream.CopyTo(fileOut);
			}
			decompressedStream.Close();
		}
	}
}
