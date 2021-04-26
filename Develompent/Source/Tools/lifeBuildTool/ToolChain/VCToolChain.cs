using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using Microsoft.Win32;
using System.Security.Permissions;
using System.Text.RegularExpressions;

namespace lifeBuildTool
{
    class VCToolChain
    {
        static bool             areEnvironmentVariablesAlreadyInitialized = false;

        public static void SetUpGlobalEnvironment( LETargetPlatform InPlatform )
        {
            InitializeEnvironmentVariables( InPlatform );
        }

        static string GetCLArguments_Global( CPPEnvironment InCompileEnvironment )
        {
            string      result = "";

            // Prevents the compiler from displaying its logo for each invocation.
            result += " /nologo";

            // Enable intrinsic functions.
            result += " /Oi";

            // Separate functions for linker.
            result += " /Gy";

            // Relaxes floating point precision semantics to allow more optimization.
            result += " /fp:fast";

            // Compile into an .obj file, and skip linking.
            result += " /c";

            bool            useDebugRuntimeLibrary = false;
            bool            useStaticRuntimeLibrary = false;

            //
            // Debug
            //
            if ( InCompileEnvironment.configuration == LETargetConfiguration.Debug )
            {
                // Use debug runtime in debug configuration.
                useDebugRuntimeLibrary = true;

                // Disable compiler optimization.
                result += " /Od";
            }
            //
            //	Release and Shipping
            //
            else
            {
                // Maximum optimizations.
                result += " /Ox";

                // Favor code speed.
                result += " /Ot";

                // Allow inline method expansion.			
                result += " /Ob2";

                //
                // Shipping
                //
                if ( InCompileEnvironment.configuration == LETargetConfiguration.Shipping )
                {
                    // Enable link-time code generation.
                    result += " /GL";
                }
            }

            //
            // PC
            //
            if ( InCompileEnvironment.platform == LETargetPlatform.Win32 || InCompileEnvironment.platform == LETargetPlatform.Win64 )
            {
                // SSE options are not allowed when 64 bit toolchain
                if ( InCompileEnvironment.platform != LETargetPlatform.Win64 )
                    result += " /arch:SSE2";

                // Prompt the user before reporting internal errors to Microsoft.
                result += " /errorReport:prompt";

                // Enable C++ exception handling, but not C exceptions.
                result += " /EHsc";
            }

            // If enabled, create debug information.
            if ( InCompileEnvironment.isCreateDebugInfo )
                result += " /Zi";

            // Specify the appropriate runtime library based on the platform and config.
            result += string.Format( " /M{0}{1}", useStaticRuntimeLibrary ? 'T' : 'D', useDebugRuntimeLibrary ? "d" : "" );
            return result;
        }

        static string GetCLArguments_CPP()
        {
            string      result = "";

            // Explicitly compile the file as C++.
            result += " /TP";

            // Treat warnings as errors.
            result += " /WX";

            // Level 4 warnings.
            result += " /W4";

            // Disable warning C4996 'This function or variable may be unsafe _CRT_SECURE_NO_WARNINGS'
            result += " /wd4996";

            // Disable warning C4100 'Unreferenced formal parameter'
            result += " /wd4100";

            return result;
        }

        static string GetCLArguments_C()
        {
            string      result = "";

            // Explicitly compile the file as C.
            result += " /TC";

            // Level 0 warnings.  Needed for external C projects that produce warnings at higher warning levels.
            result += " /W0";

            return result;
        }

        public static CPPOutput CompileCPPFiles( CPPEnvironment InCompileEnvironment, List< string > InSourceFiles, CompileCache InOutCompileCache, out bool OutIsSuccessed )
        {
            OutIsSuccessed = true;
            string      arguments = GetCLArguments_Global( InCompileEnvironment );

            // Add include paths to the argument list.
            foreach ( string includePath in InCompileEnvironment.includePaths )
                arguments += string.Format( " /I \"{0}\"", includePath );

            // Add preprocessor definitions to the argument list.
            foreach ( string definition in InCompileEnvironment.definitions )
                arguments += string.Format( " /D \"{0}\"", definition );

            // Add the additional arguments specified by the environment.
            arguments += InCompileEnvironment.additionalArguments;

            // Compile source files
            CPPOutput               cppOutput = new CPPOutput();
            Process                 compile = new Process();
            ProcessStartInfo        compileStartInfo = compile.StartInfo;
            compileStartInfo.FileName = "cl";
            compileStartInfo.UseShellExecute = false;
            compileStartInfo.RedirectStandardOutput = true;
            compileStartInfo.RedirectStandardError = true;

            compile.OutputDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
           {
               if ( InEvent != null && InEvent.Data != null )
                   Logging.WriteLine( InEvent.Data );
            });

            compile.ErrorDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
            {
                if ( InEvent != null && InEvent.Data != null )
                    Logging.WriteLine( InEvent.Data );
            });

            foreach ( string sourceFile in InSourceFiles )
            {
                bool            isPlainCFile = Path.GetExtension( sourceFile.ToUpper() ) == ".C";
                string          outputFile = string.Format( "{0}{1}.obj", InCompileEnvironment.outputDirectory, Path.GetFileNameWithoutExtension( sourceFile ) );
                string          finalArguments = arguments + string.Format( " \"{0}\" /Fo\"{1}\"", sourceFile, outputFile );             

                if ( isPlainCFile )
                    finalArguments += GetCLArguments_C();
                else
                    finalArguments += GetCLArguments_CPP();

                if ( InCompileEnvironment.configuration == LETargetConfiguration.Debug )
                {
                    string      pdbFile = string.Format( "{0}{1}.pdb", InCompileEnvironment.outputDirectory, Path.GetFileNameWithoutExtension( sourceFile ) );
                    cppOutput.debugDataFiles.Add( pdbFile );
                    finalArguments += string.Format( " /Fd\"{0}\"", pdbFile );
                }
               
                compileStartInfo.Arguments = finalArguments;

                compile.Start();
                compile.BeginOutputReadLine();
                compile.BeginErrorReadLine();

                compile.WaitForExit();
                compile.CancelOutputRead();
                compile.CancelErrorRead();

                OutIsSuccessed = compile.ExitCode == 0 && OutIsSuccessed ? true : false;
                if ( OutIsSuccessed )
                {
                    InOutCompileCache.AddItem( new CompileCacheItem( sourceFile, outputFile, Utils.ComputeMD5Checksum( sourceFile ) ) );
                    cppOutput.objectFiles.Add( outputFile );
                }
            }

            return cppOutput;
        }

        public static CPPOutput CompileRCFiles( CPPEnvironment InCompileEnvironment, List<string> InRCFiles, CompileCache InOutCompileCache, out bool OutIsSuccessed )
        {
            OutIsSuccessed = true;

            CPPOutput           cppOutput = new CPPOutput();
            Process             compile = new Process();
            ProcessStartInfo    compileStartInfo = compile.StartInfo;
            compileStartInfo.FileName = "rc";
            compileStartInfo.UseShellExecute = false;
            compileStartInfo.RedirectStandardOutput = true;
            compileStartInfo.RedirectStandardError = true;

            compile.OutputDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
            {
                if ( InEvent != null && InEvent.Data != null )
                    Logging.WriteLine( InEvent.Data );
            });

            compile.ErrorDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
            {
                if ( InEvent != null && InEvent.Data != null )
                    Logging.WriteLine( InEvent.Data );
            });

            // Initialize arguments of compiler RC files
            string          arguments = "";
            
            // Suppress header spew
            arguments += " /nologo";

            // If we're compiling for 64-bit Windows, also add the _WIN64 definition to the resource
            // compiler so that we can switch on that in the .rc file using #ifdef.
            if ( InCompileEnvironment.platform == LETargetPlatform.Win64 )
                arguments += " /D _WIN64=1";

            // Language
            arguments += " /l 0x409";

            // Add include paths to the argument list.
            foreach ( string includePath in InCompileEnvironment.includePaths )
                arguments += string.Format( " /i \"{0}\"", includePath );

            // Add preprocessor definitions to the argument list.
            foreach ( string definition in InCompileEnvironment.definitions )
                arguments += string.Format( " /d \"{0}\"", definition );

            foreach ( string rcFile in InRCFiles )
            {
                string      outputFile = string.Format( "{0}{1}.res", InCompileEnvironment.outputDirectory, Path.GetFileNameWithoutExtension( rcFile ) );
                string      finalArguments = arguments + string.Format( " \"{0}\" /fo\"{1}\"", rcFile, outputFile );

                compileStartInfo.Arguments = finalArguments;
                compile.Start();
                compile.BeginOutputReadLine();
                compile.BeginErrorReadLine();

                compile.WaitForExit();
                compile.CancelOutputRead();
                compile.CancelErrorRead();

                OutIsSuccessed = compile.ExitCode == 0 && OutIsSuccessed ? true : false;
                if ( OutIsSuccessed )
                {
                    InOutCompileCache.AddItem( new CompileCacheItem( rcFile, outputFile, Utils.ComputeMD5Checksum( rcFile ) ) );
                    cppOutput.objectFiles.Add( outputFile );
                }
            }

            return cppOutput;
        }

        static string GetLinkArguments( LinkEnvironment InLinkEnvironment )
        {
            string          result = "";

            // Is target 64-bit?
            bool            isTarget64Bit = InLinkEnvironment.platform == LETargetPlatform.Win64;

            // Don't create a side-by-side manifest file for the executable.
            result += " /MANIFEST:NO";

            // Prevents the linker from displaying its logo for each invocation.
            result += " /NOLOGO";

            if ( InLinkEnvironment.isCreateDebugInfo )
            {
                // Output debug info for the linked executable.
                result += " /DEBUG";
            }

            // Prompt the user before reporting internal errors to Microsoft.
            result += " /errorReport:prompt";

            //
            //	PC
            //
            if (InLinkEnvironment.platform == LETargetPlatform.Win32 || InLinkEnvironment.platform == LETargetPlatform.Win64)
            {
                if (isTarget64Bit)
                {
                    result += " /MACHINE:x64";
                }
                // 32 bit executable/ target.
                else
                {
                    result += " /MACHINE:x86";
                }

                // Link for Windows.
                result += " /SUBSYSTEM:WINDOWS";

                // Allow the OS to load the EXE at different base addresses than its preferred base address.
                result += " /FIXED:No";

                // Option is only relevant with 32 bit toolchain.
                if (InLinkEnvironment.platform == LETargetPlatform.Win32)
                {
                    // Disables the 2GB address space limit on 64-bit Windows and 32-bit Windows with /3GB specified in boot.ini
                    result += " /LARGEADDRESSAWARE";
                }

                // Explicitly declare that the executable is compatible with Data Execution Prevention.
                result += " /NXCOMPAT";

                if (InLinkEnvironment.isShippingBinary)
                {
                    // Set the default stack size.
                    result += " /STACK:1000000,1000000";
                }
                else
                {
                    // Set the default stack size.
                    if (InLinkEnvironment.platform == LETargetPlatform.Win64)
                    {
                        // Building editor for 64 bit executable/ target - use larger stack.
                        result += " /STACK:6500000,5000000";
                    }
                    else
                    {
                        // 32 bit executable/ target or not building the editor - use standard stack size.
                        result += " /STACK:5000000,5000000";
                    }
                }
            }
            else
                Debug.Fail( "Unknown platform: {0}", InLinkEnvironment.platform.ToString() );

            //
            //	Shipping binary
            //
            if ( InLinkEnvironment.isShippingBinary )
            {
                // Generate an EXE checksum.
                result += " /RELEASE";

                // Eliminate unreferenced symbols.
                result += " /OPT:REF";

                // Remove redundant COMDATs.
                result += " /OPT:ICF";
            }
            //
            //	Regular development binary. 
            //
            else
            {
                // Keep symbols that are unreferenced.
                result += " /OPT:NOREF";

                // Disable identical COMDAT folding.
                result += " /OPT:NOICF";

                // result += " /VERBOSE"; // Verbose
            }

            return result;
        }

        public static string LinkFiles( LinkEnvironment InLinkEnvironment, out bool OutIsSuccessed )
        {
            OutIsSuccessed = true;
            string          arguments = GetLinkArguments( InLinkEnvironment );

            // Delay-load these DLLs.
            foreach ( string delayLoadDLL in InLinkEnvironment.delayLoadDLLs )
            {
                arguments += string.Format(" /DELAYLOAD:\"{0}\"", delayLoadDLL );
            }

            // Add the library paths to the argument list.
            foreach ( string libraryPath in InLinkEnvironment.libraryPaths )
            {
                arguments += string.Format(" /LIBPATH:\"{0}\"", libraryPath );
            }

            // Add the excluded default libraries to the argument list.
            foreach ( string excludedLibrary in InLinkEnvironment.excludedLibraries )
            {
                arguments += string.Format( " /NODEFAULTLIB:\"{0}\"", excludedLibrary );
            }

            // Add the input files to a response file, and pass the response file on the command-line.
            List< string >          inputFileNames = new List< string >();
            foreach ( string inputFile in InLinkEnvironment.inputFiles )
            {
                inputFileNames.Add( string.Format( "\"{0}\"", Path.GetFullPath( inputFile ) ) );
            }

            foreach ( string additionalLibrary in InLinkEnvironment.additionalLibraries )
            {
                inputFileNames.Add( string.Format( "\"{0}\"", additionalLibrary ) );
            }

            string          responseFileName = Path.Combine( InLinkEnvironment.intermediateDirectory, Path.GetFileName( InLinkEnvironment.outputFilePath ) + ".response" );
            arguments += string.Format( " @\"{0}\"", ResponseFile.Create( responseFileName, inputFileNames ) );

            // Add the output file to the command-line.
            arguments += string.Format(" /OUT:\"{0}\"", Path.GetFullPath( InLinkEnvironment.outputFilePath ) );

            if ( InLinkEnvironment.isCreateDebugInfo )
            {
                // Write the PDB file to the output directory.			
                string          pdbFilePath = Path.Combine( InLinkEnvironment.outputDirectory, Path.GetFileNameWithoutExtension( InLinkEnvironment.outputFilePath ) + ".pdb" );
                arguments += string.Format( " /PDB:\"{0}\"", pdbFilePath );
            }

            // Add the additional arguments specified by the environment.
            arguments += InLinkEnvironment.additionalArguments;

            // Add flag for denuvo security
            if ( InLinkEnvironment.configuration == LETargetConfiguration.Shipping && InLinkEnvironment.platform == LETargetPlatform.Win64 )
            {
                // Output a map file. 
                arguments += string.Format( " /MAP:\"{0}\".map", Path.GetFullPath( InLinkEnvironment.outputFilePath ) );
            }

            // Link program
            Process                 link = new Process();
            ProcessStartInfo        linkStartInfo = link.StartInfo;
            linkStartInfo.FileName = "link";
            linkStartInfo.UseShellExecute = false;
            linkStartInfo.RedirectStandardOutput = true;
            linkStartInfo.RedirectStandardError = true;
            linkStartInfo.Arguments = arguments;

            link.OutputDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
            {
                if ( InEvent != null && InEvent.Data != null )
                    Logging.WriteLine( InEvent.Data );
            });

            link.ErrorDataReceived += new DataReceivedEventHandler( ( InSender, InEvent ) =>
            {
                if ( InEvent != null && InEvent.Data != null )
                    Logging.WriteLine( InEvent.Data );
            });

            link.Start();
            link.BeginOutputReadLine();
            link.BeginErrorReadLine();

            link.WaitForExit();
            link.CancelOutputRead();
            link.CancelErrorRead();

            OutIsSuccessed = link.ExitCode == 0 ? true : false;
            return InLinkEnvironment.outputFilePath;
        }

        static string GetVCToolPath( LETargetPlatform InPlatform, LETargetConfiguration InConfiguration, string InToolName )
        {
            // Initialize environment variables required for spawned tools.
            InitializeEnvironmentVariables( InPlatform );
            return InToolName;
        }

        static void InitializeEnvironmentVariables( LETargetPlatform InPlatform )
        {
            if ( areEnvironmentVariablesAlreadyInitialized ) return;

            // Getting path to tool VSWHERE
            string      vswherePath = string.Format( "{0}\\Microsoft Visual Studio\\Installer\\vswhere.exe", Environment.GetFolderPath( Environment.SpecialFolder.ProgramFilesX86 ) );
            var         vswhereProcess = new Process();
            var         startInfo = vswhereProcess.StartInfo;

            startInfo.Arguments = "-latest -property installationPath";
            startInfo.FileName = vswherePath;
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.RedirectStandardInput = true;

            vswhereProcess.Start();
            vswhereProcess.WaitForExit();

            // Grab path to Visual Studio binaries from the system environment
            string          installationPath = vswhereProcess.StandardOutput.ReadLine();
            if ( string.IsNullOrEmpty( installationPath ) )
                throw new BuildException( "Visual Studio 2019 must be installed in order to build this target." );

            // 64 bit tool chain.
            string          vcVarsBatchFile;
            if ( InPlatform == LETargetPlatform.Win64 )
            {
                vcVarsBatchFile = string.Format( "{0}\\VC\\Auxiliary\\Build\\vcvarsx86_amd64.bat", installationPath );
            }
            // The 32 bit vars batch file in the binary folder simply points to the one in the common tools folder.
            else
            {
                vcVarsBatchFile = string.Format( "{0}\\VC\\Auxiliary\\Build\\vcvars32.bat", installationPath );
            }

            string          batchFileArgs = "";
            Utils.SetEnvironmentVariablesFromBatchFile( vcVarsBatchFile, batchFileArgs );

            // Print Visual Studio display name and version
            startInfo.Arguments = "-latest -property displayName";

            vswhereProcess.Start();
            vswhereProcess.WaitForExit();
            string          disaplyName = vswhereProcess.StandardOutput.ReadLine();

            startInfo.Arguments = "-latest -property catalog_productDisplayVersion";

            vswhereProcess.Start();
            vswhereProcess.WaitForExit();
            string          disaplyVersion = vswhereProcess.StandardOutput.ReadLine();

            Logging.WriteLine( string.Format( "Used {0} version {1}", disaplyName, disaplyVersion ) );
            areEnvironmentVariablesAlreadyInitialized = true;
        }
    }
}
