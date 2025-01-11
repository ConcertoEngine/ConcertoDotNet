using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace DotNetLib
{
	public static class Lib
	{
		private static int s_CallCount = 1;

		[StructLayout(LayoutKind.Sequential)]
		public struct LibArgs
		{
			public IntPtr Message;
			public int Number;
		}

		public static int Hello(IntPtr arg, int sizeOfArgs)
		{
			if (sizeOfArgs < Marshal.SizeOf(typeof(LibArgs)))
			{
				Debugger.Break();
				Console.WriteLine("Invalid argument sizeof");
				return 1;
			}

			LibArgs libArgs = Marshal.PtrToStructure<LibArgs>(arg);

			if (libArgs.Message == IntPtr.Zero)
			{
				Debugger.Break();
				Console.WriteLine("Message is null");
				return 1;
			}

			Console.WriteLine($"Hello, world! from {nameof(Lib)} [count: {s_CallCount++}]");
			PrintLibArgs(libArgs);
			return 0;
		}

		public static int UpdateIntVectorWithoutCopy(IntPtr pointer, int lenght)
		{
			if (pointer == IntPtr.Zero)
			{
				Debugger.Break();
				Console.WriteLine("Pointer is null");
				return 1;
			}

			unsafe
			{
				Span<Int32> vector = new(pointer.ToPointer(), lenght);
				for (Int32 i = 0; i < vector.Length; i++)
				{
					vector[i] = i * 2;
				}
			}
			return 0;
		}

		private static void PrintLibArgs(LibArgs libArgs)
		{
			string message = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
				? Marshal.PtrToStringUni(libArgs.Message)
				: Marshal.PtrToStringUTF8(libArgs.Message);

			Console.WriteLine($"-- message: {message}");
			Console.WriteLine($"-- number: {libArgs.Number}");
		}
	}
}