using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lifeBuildTool
{
    interface LEBuildProject
    {
        void SetUpProjectEnvironment( CPPEnvironment InOutCPPEnvironment, List< LEProjectDesc > InOutDepencyProjects, List< LEProjectDesc > InOutProjects );

        string GetName();
    }
}
