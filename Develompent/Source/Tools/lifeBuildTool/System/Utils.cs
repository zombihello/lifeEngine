using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace lifeBuildTool
{
    class Utils
    {
        public static bool CompileProjects( CPPEnvironment InCppEnvironment, LinkEnvironment InLinkEnvironment, List<LEProjectDesc> InProjects )
        {
            foreach ( LEProjectDesc projectDesc in InProjects )
            {
                Logging.WriteLine( string.Format( "\nCompiling '{0}' project", Path.GetFileNameWithoutExtension( projectDesc.projectPath ) ) );

                // If the project file doesn't exist, display a friendly error.
                if ( !File.Exists( projectDesc.projectPath ) )
                    throw new BuildException( "Project file \"{0}\" doesn't exist.", projectDesc.projectPath );

                // Set output dir
                InCppEnvironment.outputDirectory = Path.GetFullPath( string.Format( "..\\Intermediate\\{0}\\", Path.GetFileNameWithoutExtension( projectDesc.projectPath ) ) );
                Directory.CreateDirectory( InCppEnvironment.outputDirectory );

                // Load the list of files from the specified project file.
                string              projectDirectory = Path.GetDirectoryName( projectDesc.projectPath );
                List< string >      projectFilePaths = VCProject.GetProjectFiles( projectDesc.projectPath );

                // Gather a list of CPP files in the project.
                List< string >      cppFiles = new List< string >();
                List< string >      cFiles = new List< string >();
                List< string >      rcFiles = new List< string >();

                foreach ( string projectFilePath in projectFilePaths )
                {
                    string      extension = Path.GetExtension( projectFilePath ).ToUpperInvariant();
                    switch ( extension )
                    {
                        case ".CPP":
                            cppFiles.Add( projectFilePath );
                            break;

                        case ".C":
                            cFiles.Add( projectFilePath );
                            break;

                        case ".RC":
                            rcFiles.Add( projectFilePath );
                            break;
                    }
                }

                bool                isSuccessed = false;

                // Compile C++ source files
                InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileFiles( cppFiles, out isSuccessed ).objectFiles );
                if ( !isSuccessed )         return false;

                // Compile C files
                InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileFiles( cFiles, out isSuccessed ).objectFiles );
                if ( !isSuccessed )         return false;

                // Compile RC files
                InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileRCFiles( rcFiles, out isSuccessed ).objectFiles );
                if ( !isSuccessed )         return false;
            }

            return true;
        }

        /// <summary>
        /// Set environment variables from batch file
        /// </summary>
        /// <param name="InBatchFileName">Path to batch file</param>
        /// <param name="InBatchFileArgs">Args for start batch file</param>
        public static void SetEnvironmentVariablesFromBatchFile( string InBatchFileName, string InBatchFileArgs )
        {
            if ( !File.Exists( InBatchFileName ) )
                throw new BuildException( "Couldn't find batch file to get environment variables: {0}.", InBatchFileName );

            // Create a wrapper batch file that echoes environment variables to a text file
            var         envOutputFileName = Path.GetTempFileName();
            var         envReaderTempFileName = Path.GetTempFileName();
            var         envReaderBatchFileName = envReaderTempFileName + ".bat";

            {
                var     envReaderBatchFileContent = new List<string>();

                // Run 'vcvars32.bat' (or similar x64 version) to set environment variables
                envReaderBatchFileContent.Add( String.Format( "call \"{0}\" {1}|| exit /b 1", InBatchFileName, InBatchFileArgs ) );

                // Pipe all environment variables to a file where we can read them in
                envReaderBatchFileContent.Add( String.Format( "set >\"{0}\"", envOutputFileName ) );

                ResponseFile.Create( envReaderBatchFileName, envReaderBatchFileContent );
            }

            var         batchFileProcess = new Process();
            var         startInfo = batchFileProcess.StartInfo;
            startInfo.FileName = envReaderBatchFileName;
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.RedirectStandardInput = true;

            // Try to launch the process, and produce a friendly error message if it fails.
            try
            {
                // Start the process up and then wait for it to finish
                batchFileProcess.Start();
                batchFileProcess.WaitForExit();

                if ( batchFileProcess.ExitCode != 0 )
                    throw new BuildException( "Batch file to get environment variables returned an error code" );

                // Load environment variables
                var         envStringsFromFile = File.ReadAllLines( envOutputFileName );

                foreach ( var envString in envStringsFromFile )
                {
                    // Parse the environment variable name and value from the string ("name=value")
                    int equalSignPos = envString.IndexOf('=');
                    var environmentVariableName = envString.Substring( 0, equalSignPos );
                    var environmentVariableValue = envString.Substring( equalSignPos + 1 );

                    // Set the environment variable
                    Environment.SetEnvironmentVariable( environmentVariableName, environmentVariableValue );
                }
            }
            catch ( BuildException )
            {
                // if it's already a build exception, just rethrow
                throw;
            }
            catch ( Exception )
            {
                throw new BuildException( "Failed to write to or subsequently read environment variables from file {0} {1}", startInfo.FileName, startInfo.Arguments );
            }
            finally
            {
                // GBX:deubanks - Delete the output files so future calls to GetTempFileName() don't eventually fail
                try { File.Delete( envReaderTempFileName ); } catch { }
                try { File.Delete( envReaderBatchFileName ); } catch { }
                try { File.Delete( envOutputFileName ); } catch { }
            }
        }
    }
}
