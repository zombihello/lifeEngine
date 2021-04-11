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
            // Set depency projects for Windows platform
            depencyProjects.Add( new LEProjectDesc( "Engine/Platforms/Windows/Windows.vcxproj" ) );            
            
            // Set include paths
            cppEnvironment.includePaths.Add( "Engine/Platforms/Windows/Include" );
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
