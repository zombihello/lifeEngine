using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace lifeBuildTool
{
    enum LETargetPlatform
    {
        Unknown,
        Win32,
        Win64
    }

    enum LETargetConfiguration
    {
        Unknown,
        Debug,
        Release,
        Shipping
    }

    class LEProjectDesc
    {
        public string           projectPath;

        public LEProjectDesc( string ProjectPath )
        {
            projectPath = ProjectPath;
        }
    }

    partial class LEBuildTarget
    {
        LETargetPlatform                platform = LETargetPlatform.Unknown;
        LETargetConfiguration           configuration = LETargetConfiguration.Unknown;
        LEBuildProject                  project = null;        
        CPPEnvironment                  cppEnvironment = new CPPEnvironment();
        LinkEnvironment                 linkEnvironment = new LinkEnvironment();
        List< LEProjectDesc >           depencyProjects = new List< LEProjectDesc >();
        List< LEProjectDesc >           projects = new List< LEProjectDesc >();
        string                          outputPath = null;

        public LEBuildTarget( string[] InArguments )
        {
            ParseArguments( InArguments );
        }

        void ParseArguments( string[] InSourceArguments )
        {
            for ( UInt32 index = 0; index < InSourceArguments.Length; ++index )
            {
                switch (InSourceArguments[ index ].ToUpperInvariant() )
                {
                    // Projects
                    case "TESTBEDGAME":
                        project = new LEBuildTestbedGame();
                        break;

                    // Platforms
                    case "WIN32":
                        platform = LETargetPlatform.Win32; 
                        break;

                    case "WIN64":
                        platform = LETargetPlatform.Win64;
                        break;

                    // Configurations
                    case "DEBUG":
                        configuration = LETargetConfiguration.Debug;
                        linkEnvironment.isCreateDebugInfo = true;
                        break;

                    case "RELEASE":
                        configuration = LETargetConfiguration.Release;
                        break;

                    case "SHIPPING":
                        configuration = LETargetConfiguration.Shipping;
                        break;

                    // -Output specifies the output filename.
                    case "-OUTPUT":
                        if ( index + 1 >= InSourceArguments.Length )
                        {
                            throw new BuildException( "Expected path after -output argument, but found nothing." );
                        }

                        index++;
                        outputPath = InSourceArguments[ index ];
                        break;
                }
            }

            if ( platform == LETargetPlatform.Unknown )
            {
                throw new BuildException( "Couldn't find platform name." );
            }

            if ( configuration == LETargetConfiguration.Unknown )
            {
                throw new BuildException( "Couldn't determine configuration name." );
            }

            // Construct the output path based on configuration, platform, game if not specified.
            if ( outputPath == null )
            {      
                // e.g. ..\..\Binaries\Win64
                outputPath = Path.Combine( "..\\..\\Binaries\\", platform.ToString() );

                // e.g. LETestbedGame-Win32-Shipping.exe
                string          executableName = project.GetName() + "-" + platform.ToString() + "-" + configuration.ToString() + GetOutputExtension( platform );

                // Example: Binaries\\Win32\\ExampleGame.exe
                if ( configuration == LETargetConfiguration.Shipping )
                {
                    executableName = project.GetName() + GetOutputExtension( platform );
                }

                outputPath = Path.Combine( outputPath, executableName );
            }

            // Convert output path to absolute path so we can mix and match with invocation via target's nmake
            outputPath = Path.GetFullPath( outputPath );
            Directory.CreateDirectory( Path.GetDirectoryName( outputPath ) );

            cppEnvironment.platform = platform;
            cppEnvironment.configuration = configuration;

            linkEnvironment.platform = platform;
            linkEnvironment.configuration = configuration;

            linkEnvironment.intermediateDirectory = Path.GetFullPath( string.Format( "..\\Intermediate\\{0}\\{1}\\", platform.ToString(), configuration ) );
            Directory.CreateDirectory( linkEnvironment.intermediateDirectory );
        }

        public bool Build()
        {
            bool            isSuccessed = true;
            Logging.WriteLine( string.Format( "Building {0} - {1} - {2}", project.GetName(), platform, configuration ) );
            
            // Set up the global compile and link environment
            SetUpGlobalEnvironment();

            // Initialize the CPP and Link environments per game and per config
            SetUpConfigurationEnvironment();
            project.SetUpProjectEnvironment( cppEnvironment, depencyProjects, projects );

            // Compile depency projects
            isSuccessed = CompileDepencyProjects();
            if ( !isSuccessed )         return false;

            // Compile projects
            isSuccessed = CompileProjects();
            if ( !isSuccessed )         return false;

            // Link the object files into an executable
            List< string >          outputItems = null;
            Logging.WriteLine( string.Format( "\nLinking {0}", project.GetName() ) );

            switch ( platform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    outputItems = GetWindowsOutputItems( out isSuccessed );
                    break;
            }

            return isSuccessed;
        }

        void SetUpGlobalEnvironment()
        { 
            switch ( cppEnvironment.platform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    VCToolChain.SetUpGlobalEnvironment( cppEnvironment.platform );
                    SetUpWindowsEnvironment();
                    break;
            }
        }

        void SetUpConfigurationEnvironment()
        {
            switch ( cppEnvironment.configuration )
            {
                case LETargetConfiguration.Debug:
                    cppEnvironment.definitions.Add( "DEBUG=1" );
                    break;

                case LETargetConfiguration.Release:
                    cppEnvironment.definitions.Add( "RELEASE=1" );
                    break;

                case LETargetConfiguration.Shipping:
                    cppEnvironment.definitions.Add( "SHIPPING=1" );
                    break;
            }

            cppEnvironment.isCreateDebugInfo = DebugInfoHeuristic.ShouldCreateDebugInfo( cppEnvironment.platform, cppEnvironment.configuration );
        }

        bool CompileDepencyProjects()
        {
            return Utils.CompileProjects( cppEnvironment, linkEnvironment, depencyProjects );
        }

        bool CompileProjects()
        { 
            return Utils.CompileProjects( cppEnvironment, linkEnvironment, projects ); 
        }

        private string GetOutputExtension( LETargetPlatform InPlatform )
        {
            string          extension = "";
            switch ( InPlatform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    extension = ".exe";
                    break;

                default:
                    throw new BuildException( "Couldn't determine platform name." );
            }

            return extension;
        }
    }
}
