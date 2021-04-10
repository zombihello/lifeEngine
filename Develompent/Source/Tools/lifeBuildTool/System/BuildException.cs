using System;
using System.Collections.Generic;
using System.Text;

namespace lifeBuildTool
{
    class BuildException : Exception
    {
        public BuildException( string MessageFormat, params Object[] MessageObjects ) :
            base( string.Format( MessageFormat, MessageObjects ) )
        {
        }
    }
}
