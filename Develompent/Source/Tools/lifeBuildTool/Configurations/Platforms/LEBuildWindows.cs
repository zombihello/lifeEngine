using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lifeBuildTool
{
    partial class LEBuildTarget
    {
        void SetUpWindowsEnvironment()
        {
            //
            // External libs
            //

            // SDL2
            SetUpSDL2Environment();

            // RapidJSON
            SetUpRapidJSONEnvironment();

            //
            // Engine/Game/Tools projects
            //

            // Set depency projects for Windows platform
            depencyProjects.Add( new LEProjectDesc( "Engine/Platforms/Windows/Windows.vcxproj" ) );
            depencyProjects.Add( new LEProjectDesc( "Engine/RHI/D3D11RHI/D3D11RHI.vcxproj" ) );

            // Set include paths
            cppEnvironment.includePaths.Add( "Engine/Platforms/Windows/Include" );
            cppEnvironment.includePaths.Add( "Engine/RHI/D3D11RHI/Include" );

            linkEnvironment.additionalLibraries.Add( "d3d11.lib" );
            linkEnvironment.additionalLibraries.Add( "dxgi.lib" );
            linkEnvironment.additionalLibraries.Add( "dxguid.lib" );
        }

        void SetUpSDL2Environment()
        {
            string              sdl2Home = "../External/SDL2-2.0.14";

            // Settings includes
            cppEnvironment.includePaths.Add( sdl2Home + "/include" );

            // Settings path to libs
            switch ( platform )
            {
                case LETargetPlatform.Win32:
                    linkEnvironment.libraryPaths.Add( sdl2Home + "/lib/Win32" );
                    break;

                case LETargetPlatform.Win64:
                    linkEnvironment.libraryPaths.Add( sdl2Home + "/lib/Win64" );
                    break;

                default:
                    throw new BuildException( "Not supported platform for SDL2" );
            }

            linkEnvironment.additionalLibraries.Add( "SDL2.lib" );
        }

        void SetUpRapidJSONEnvironment()
        {
            string          rapidJSONHome = "../External/rapidjson";

            // Settings includes
            cppEnvironment.includePaths.Add( rapidJSONHome + "/include" );
        }

        List< string > GetWindowsOutputItems( out bool OutIsSuccessed )
        {
            OutIsSuccessed = true;

            // Verify that the user has specified the expected output extension.
            string          desiredExtension = ".EXE";
            if ( Path.GetExtension( outputPath ).ToUpperInvariant() != desiredExtension && Path.GetExtension( outputPath ).ToUpperInvariant() != ".DLL" )
            {
                throw new BuildException( "Unexpected output extension: {0} instead of {1}", Path.GetExtension( outputPath ), desiredExtension );
            }

            // Put the non-executable output files (PDB, import library, etc) in the same directory as the executables
            linkEnvironment.outputDirectory = Path.GetDirectoryName( outputPath );

            // Link the EXE file.
            linkEnvironment.outputFilePath = outputPath;
            string          exeFile = linkEnvironment.LinkExecutable( out OutIsSuccessed );

            // Return a list of the output files.
            List< string >              outputFiles = new List< string >();
            outputFiles.Add( exeFile );
            
            return outputFiles;
        }
    }
}
