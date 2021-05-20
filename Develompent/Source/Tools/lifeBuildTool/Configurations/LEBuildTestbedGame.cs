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
            bool            withEditor = InOutCPPEnvironment.configuration != LETargetConfiguration.Shipping;

            InOutDepencyProjects.Add( new LEProjectDesc( "Engine/Core/Core.vcxproj" ) );
            InOutDepencyProjects.Add( new LEProjectDesc( "Engine/Engine/Engine.vcxproj" ) );
            InOutDepencyProjects.Add( new LEProjectDesc( "Engine/Launch/Launch.vcxproj" ) );
            InOutDepencyProjects.Add( new LEProjectDesc( "Games/TestbedGame/TestbedGame.vcxproj" ) );

            InOutCPPEnvironment.includePaths.Add( "Engine/Core/Include" );
            InOutCPPEnvironment.includePaths.Add( "Engine/Engine/Include" );         
            InOutCPPEnvironment.includePaths.Add( "Engine/Launch/Include" );
            InOutCPPEnvironment.includePaths.Add( "Games/TestbedGame/Include" );

            // Enable or disable editor
            if ( withEditor )
            {
                InOutCPPEnvironment.definitions.Add( "WITH_EDITOR=1" );
                InOutDepencyProjects.Add( new LEProjectDesc( "Engine/WorldEd/WorldEd.vcxproj" ) );
                InOutCPPEnvironment.includePaths.Add( "Engine/WorldEd/Include" );
            }
            else
            {
                InOutCPPEnvironment.definitions.Add( "WITH_EDITOR=0" );
            }
        }

        public string GetName()
        {
            return "TestbedGame";
        }
    }
}
