using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace lifeBuildTool
{
    class ResponseFile
    {
		/// <summary>
		/// Creates a file from a list of strings; each string is placed on a line in the file
		/// </summary>
		public static string Create( List<string> InLines )
		{
			return Create( Path.GetTempFileName(), InLines );
		}

		/// <summary>
		/// Creates a file from a list of strings; each string is placed on a line in the file
		/// </summary>
		public static string Create( string InTempFileName, List<string> InLines )
		{
			Directory.CreateDirectory( Path.GetDirectoryName( InTempFileName ) );
			using ( FileStream ResponseFileStream = new FileStream( InTempFileName, FileMode.Create, FileAccess.Write ) )
			{
				using ( StreamWriter StreamWriter = new StreamWriter( ResponseFileStream ) )
				{
					foreach ( string Line in InLines )
						StreamWriter.WriteLine( Line );
				}
			}

			return InTempFileName;
		}
	}
}
