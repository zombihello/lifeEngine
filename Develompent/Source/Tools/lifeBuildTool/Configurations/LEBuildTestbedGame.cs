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
            InOutDepencyProjects.Add( new LEProjectDesc( "Engine/Launch/Launch.vcxproj" ) );
        }

        public string GetName()
        {
            return "TestbedGame";
        }
    }
}
