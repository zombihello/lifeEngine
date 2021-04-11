using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lifeBuildTool
{
    class LEBuildTestbedGame : LEBuildProject
    {
        public void SetUpProjectEnvironment( CPPEnvironment InOutCPPEnvironment, List< LEProjectDesc > InOutDepencyProjects, List< LEProjectDesc > InOutProjects )
        {
            InOutDepencyProjects.Add( new LEProjectDesc( "Engine/Core/Core.vcxproj" ) );
            InOutCPPEnvironment.includePaths.Add( "Engine/Core/Include" );
        }

        public string GetName()
        {
            return "TestbedGame";
        }
    }
}
