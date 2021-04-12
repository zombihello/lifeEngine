using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace lifeBuildTool
{
    class CompileCacheItem
    {        
        public CompileCacheItem()
        {
            pathToSourceFile = "";
            pathToCompiledFile = "";
            md5Hash = "";
        }

        public CompileCacheItem( string InPathToSourceFile, string InPathToCompiledFile, string InMD5Hash )
        {
            pathToSourceFile = InPathToSourceFile;
            pathToCompiledFile = InPathToCompiledFile;
            md5Hash = InMD5Hash;
        }

        public string      pathToSourceFile;
        public string      pathToCompiledFile;
        public string      md5Hash;
    }

    class CompileCache
    {
        private Dictionary< string, CompileCacheItem >          cacheItems = new Dictionary< string, CompileCacheItem >();

        public bool Load( string InPath )
        {
            try
            {
                XmlDocument         xmlDocument = new XmlDocument();
                xmlDocument.Load( InPath );

                XmlElement          xmlRoot = xmlDocument.DocumentElement;
                foreach ( XmlNode xmlNode in xmlRoot )
                {
                    string      pathToSourceFile = xmlNode.Attributes.GetNamedItem( "SourceFile" ).Value;
                    string      pathToCompiledFile = xmlNode.Attributes.GetNamedItem( "CompiledFile" ).Value;
                    string      md5Hash = xmlNode.Attributes.GetNamedItem( "MD5Hash" ).Value;

                    cacheItems.Add( pathToSourceFile, new CompileCacheItem( pathToSourceFile, pathToCompiledFile, md5Hash ) );
                }
            }
            catch ( Exception InException )
            {
                return false;
            }

            return true;
        }

        public void Save( string InPath )
        {
            try
            {
                using ( StreamWriter streamWriter = new StreamWriter( InPath, false, System.Text.Encoding.Default ) )
                {
                    streamWriter.WriteLine( "<CompileCache>" );

                    foreach ( KeyValuePair< string, CompileCacheItem > keyValue in cacheItems)
                    {
                        streamWriter.WriteLine( "<CompileCacheItem SourceFile=\"{0}\" CompiledFile=\"{1}\" MD5Hash=\"{2}\" />", keyValue.Value.pathToSourceFile, keyValue.Value.pathToCompiledFile, keyValue.Value.md5Hash );
                    }

                    streamWriter.WriteLine( "</CompileCache>" );
                }
            }
            catch ( Exception InException )
            {
                Logging.WriteLine( InException.Message );
            }
        }

        public void Clear()
        {
            cacheItems.Clear();
        }

        public void AddItem( CompileCacheItem InCompileCacheItem )
        {
            if ( InCompileCacheItem == null )     return;
           
            CompileCacheItem        cacheItem;
            bool                    isSuccussed = cacheItems.TryGetValue( InCompileCacheItem.pathToSourceFile, out cacheItem );
            if ( !isSuccussed )
            {
                cacheItems.Add( InCompileCacheItem.pathToSourceFile, InCompileCacheItem );
            }
            else
            {
                cacheItem.md5Hash = InCompileCacheItem.md5Hash;
            }
        }

        public CompileCacheItem FindItem( string InPathToSourceFile )
        {
            CompileCacheItem        cacheItem;
            bool        isSuccussed = cacheItems.TryGetValue( InPathToSourceFile, out cacheItem );

            return isSuccussed ? cacheItem : null;
        }
    }
}
