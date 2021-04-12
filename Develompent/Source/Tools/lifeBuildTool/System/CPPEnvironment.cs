using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace lifeBuildTool
{
    class CPPOutput
    {
        public List< string >       objectFiles = new List< string >();
        public List< string >       debugDataFiles = new List< string >();
    }

    class CPPEnvironment
    {
        public string                          outputDirectory = null;
        public LETargetPlatform                platform = LETargetPlatform.Unknown;
        public LETargetConfiguration           configuration = LETargetConfiguration.Unknown;
        public List< string >                  includePaths = new List< string >();
        public List< string >                  definitions = new List< string >();
        public bool                            isCreateDebugInfo = true;
        public string                          additionalArguments = "";

        public CPPEnvironment()
        { }

        public CPPEnvironment( CPPEnvironment InCopy )
        {
            outputDirectory = InCopy.outputDirectory;
            platform = InCopy.platform;
            configuration = InCopy.configuration;
            includePaths = InCopy.includePaths;
            definitions = InCopy.definitions;
            additionalArguments = InCopy.additionalArguments;
            isCreateDebugInfo = InCopy.isCreateDebugInfo;
        }

        public CPPOutput CompileFiles( List< string > InCPPFiles, CompileCache InOutCompileCache, out bool OutIsSuccessed )
        {
            OutIsSuccessed = false;

            switch ( platform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    return VCToolChain.CompileCPPFiles( this, InCPPFiles, InOutCompileCache, out OutIsSuccessed );

                default:
                    Debug.Fail( "Unrecognized C++ target platform." );
                    return new CPPOutput();
            }
        }

        public CPPOutput CompileRCFiles( List< string > InRCFiles, CompileCache InOutCompileCache, out bool OutIsSuccessed )
        {
            OutIsSuccessed = false;

            switch (platform)
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    return VCToolChain.CompileRCFiles( this, InRCFiles, InOutCompileCache, out OutIsSuccessed );

                default:
                    return new CPPOutput();
            }
        }
    }
}
