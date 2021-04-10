using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lifeBuildTool
{
    class Logging
    {
        public static void WriteLine( string InMessage )
        {
            Console.WriteLine( InMessage );
            if ( System.Diagnostics.Debugger.IsAttached )
            {
                System.Diagnostics.Trace.WriteLine( InMessage );
            }
        }
    }
}
