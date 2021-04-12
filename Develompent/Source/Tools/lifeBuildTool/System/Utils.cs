using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace lifeBuildTool
{
    class Utils
    {
        public static bool CompileProjects( CPPEnvironment InCppEnvironment, LinkEnvironment InLinkEnvironment, List<LEProjectDesc> InProjects )
        {
            bool                isSuccessed = true;

            foreach ( LEProjectDesc projectDesc in InProjects )
            {
                string          projectName = Path.GetFileNameWithoutExtension( projectDesc.projectPath );

                // If the project file doesn't exist, display a friendly error.
                if ( !File.Exists( projectDesc.projectPath ) )
                    throw new BuildException( "Project file \"{0}\" doesn't exist.", projectDesc.projectPath );

                // Set output dir
                InCppEnvironment.outputDirectory = Path.GetFullPath( string.Format( "..\\Intermediate\\{0}\\", projectName ) );
                Directory.CreateDirectory( InCppEnvironment.outputDirectory );

                // Create and loading compiled cache
                string                  compileCachePath = InCppEnvironment.outputDirectory + string.Format( "CompileCache-{0}.xml", projectName );
                CompileCache            compileCache = new CompileCache();
                compileCache.Load( compileCachePath );

                // Load the list of files from the specified project file.
                string              projectDirectory = Path.GetDirectoryName( projectDesc.projectPath );
                List< string >      projectFilePaths = VCProject.GetProjectFiles( projectDesc.projectPath );

                // Gather a list of CPP files in the project.
                List< string >      cppFiles = new List< string >();
                List< string >      cFiles = new List< string >();
                List< string >      rcFiles = new List< string >();

                foreach ( string projectFilePath in projectFilePaths )
                {
                    CompileCacheItem        cacheItem = compileCache.FindItem( projectFilePath );
                    if ( cacheItem != null && cacheItem.md5Hash == ComputeMD5Checksum( projectFilePath ) && File.Exists( cacheItem.pathToCompiledFile ) )
                    {
                        InLinkEnvironment.inputFiles.Add( cacheItem.pathToCompiledFile );
                        continue;
                    }

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

                // If we have nothing to compile, go to the next project
                if ( cppFiles.Count == 0 && cFiles.Count == 0 && rcFiles.Count == 0 )
                {
                    continue;
                }

                Logging.WriteLine( string.Format( "\nCompiling '{0}' project", projectName ) );

                // Compile C++ source files
                if ( cppFiles.Count > 0 )
                {
                    InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileFiles( cppFiles, compileCache, out isSuccessed ).objectFiles );
                }

                // Compile C files
                if ( isSuccessed && cFiles.Count > 0 )
                {
                    InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileFiles( cFiles, compileCache, out isSuccessed ).objectFiles );
                }

                // Compile RC files
                if ( isSuccessed && rcFiles.Count > 0 )
                {
                    InLinkEnvironment.inputFiles.AddRange( InCppEnvironment.CompileRCFiles( rcFiles, compileCache, out isSuccessed ).objectFiles );
                }

                // Updating file of compile cache
                compileCache.Save( compileCachePath );

                // If there was a compilation error, then there is no point in continuing to compile projects
                if ( !isSuccessed )         break;
            }

            return isSuccessed;
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

        public static string ComputeMD5Checksum( string InPath )
        {
            using ( FileStream fileStream = File.OpenRead( InPath ) )
            using ( MD5 md5 = new MD5CryptoServiceProvider() )
            {
                byte[]          checkSum = md5.ComputeHash( fileStream );
                string          result = BitConverter.ToString( checkSum ).Replace( "-", String.Empty );
                return result;
            }
        }
    }
}
