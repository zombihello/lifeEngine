using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lifeBuildTool
{
    class LinkEnvironment
    {
        public string                           outputDirectory = null;
        public string                           intermediateDirectory = null;
        public string                           outputFilePath = null;
        public LETargetPlatform                 platform = LETargetPlatform.Unknown;
        public LETargetConfiguration            configuration = LETargetConfiguration.Unknown;
        public List< string >                   libraryPaths = new List< string >();
        public List< string >                   excludedLibraries = new List< string >();
        public List< string >                   delayLoadDLLs = new List< string >();
        public List< string >                   inputFiles = new List< string >();
        public List< string >                   additionalLibraries = new List< string >();
        public string                           additionalArguments = "";
        public bool                             isShippingBinary = false;
        public bool                             isCreateDebugInfo = false;
        
        public LinkEnvironment()
        {}

        public LinkEnvironment( LinkEnvironment InCopyEnvironment )
        {
            outputDirectory = InCopyEnvironment.outputDirectory;
            outputFilePath = InCopyEnvironment.outputFilePath;
            platform = InCopyEnvironment.platform;
            configuration = InCopyEnvironment.configuration;
            libraryPaths.AddRange( InCopyEnvironment.libraryPaths );
            delayLoadDLLs.AddRange( InCopyEnvironment.delayLoadDLLs );
            inputFiles.AddRange( InCopyEnvironment.inputFiles );
            additionalArguments = InCopyEnvironment.additionalArguments;
            isShippingBinary = InCopyEnvironment.isShippingBinary;
            isCreateDebugInfo = InCopyEnvironment.isCreateDebugInfo;
        }

        public string LinkExecutable( out bool OutIsSuccessed )
        {
            OutIsSuccessed = false;

            switch ( platform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    return VCToolChain.LinkFiles( this, out OutIsSuccessed);

                default:
                    return null;
            }
        }
    }
}
