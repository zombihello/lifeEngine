using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace lifeBuildTool
{
    class lifeBuildTool
    {
        static int Main( string[] InArguments )
        {
            bool        success = true;

            try
            {
                // Change the working directory to be the Development/Source folder.
                Directory.SetCurrentDirectory( Path.Combine( Path.GetDirectoryName( Assembly.GetExecutingAssembly().Location ), "..\\..\\..\\Source" ) );

                LEBuildTarget       buildTarget = new LEBuildTarget( InArguments );
                success = buildTarget.Build();
            }
            catch ( BuildException InException )
            {
                // Treat our (expected) BuildExceptions as nice messages, not the full callstack dump
                Logging.WriteLine( string.Format( "{0}", InException.Message ) );
                success = false;
            }
            catch ( Exception InException )
            {
                Logging.WriteLine( string.Format( "{0}", InException ) );
                success = false;
            }

            return success ? 0 : 1;
        }
    }
}
