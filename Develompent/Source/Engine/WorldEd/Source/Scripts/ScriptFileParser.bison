%{
    #include "ScriptFileParser_Bison.cpp.h"
    #include "Core.h"
    #include "Scripts/ScriptTokenStream.h"
    #include "Scripts/ScriptFileParser.h"

    // Some Bison defines
    #define YYDEBUG                 1
    #define YYERROR_VERBOSE         1
    #define YYPARSE_PARAM           param 
    #define YYLEX_PARAM             param 
    #define YYSTYPE                 YYSTYPE_File

    #define g_FileContext           ( ( CScriptFileParserInterface* )param )
    #define g_FileParser            ( ( ( CScriptFileParserInterface* )param )->GetFileParser() )

    /**
    * @ingroup WorldEd
    * @brief Structure to pass around parser parameters
    */
    struct YYSTYPE_File
    {
        /**
         * @brief Constructor
         */
        FORCEINLINE YYSTYPE_File()
            : flags( 0 )
            , context( nullptr )
        {}

        /**
         * @brief Constructor of copy
         * @param InOther   Other YYSTYPE_File
         */
        FORCEINLINE YYSTYPE_File( const YYSTYPE_File& InOther )
            : flags( InOther.flags )
            , withinClass( InOther.withinClass )
            , string( InOther.string )
            , context( InOther.context )
            , token( InOther.token )
        {}

        /**
         * @brief Constructor of move
         * @param InOther   Other YYSTYPE_File
         */
        FORCEINLINE YYSTYPE_File( YYSTYPE_File&& InOther )
            : flags( std::move( InOther.flags ) )
            , withinClass( std::move( InOther.withinClass ) )
            , string( std::move( InOther.string ) )
            , context( std::move( InOther.context ) )
            , token( std::move( InOther.token ) )
        {}

        /**
         * @brief Destructor
         */
        FORCEINLINE ~YYSTYPE_File()
        {}

        /**
         * @brief Override operator of copy
         *
         * @param InOther   Other YYSTYPE_File
         * @return Return reference to self
         */
        FORCEINLINE YYSTYPE_File& operator=( const YYSTYPE_File& InOther )
	    {
	    	if ( this != &InOther )
	    	{
                flags = InOther.flags;
                withinClass = InOther.withinClass;
                string = InOther.string;
	    		context = InOther.context;
	    		token = InOther.token;
	    	}
	    	return *this;
	    }

        /**
         * @brief Override operator of move
         *
         * @param InOther   Other YYSTYPE_File
         * @return Return reference to self
         */
        FORCEINLINE YYSTYPE_File& operator=( YYSTYPE_File&& InOther )
	    {
	    	if ( this != &InOther )
	    	{
                flags = std::move( InOther.flags );
                withinClass = std::move( InOther.withinClass );
                string = std::move( InOther.string );
	    		context = std::move( InOther.context );
	    		token = std::move( InOther.token );
	    	}
	    	return *this;
	    }

        uint32                  flags;          /**< Flags (look EClassFlags) */
        std::string_view        withinClass;    /**< Within class name */
        std::string_view        string;         /**< String value */
        ScriptFileContext*      context;        /**< Script file context */
        std::string_view        token;          /**< Token in string format */
    };

    /**
    * @ingroup WorldEd
    * @brief Script file parser interface
    */
    class CScriptFileParserInterface
    {
    public:
        /**
         * @brief Constructor
         *
         * @param InTokens      Tokens stream
         * @param InFileParser  File parser
         */
         CScriptFileParserInterface( CScriptTokenStream& InTokens, CScriptFileParser& InFileParser )
            : tokens( &InTokens )
            , fileParser( &InFileParser )
            , currentToken( nullptr )
         {}

        /**
         * @brief Get next token
         *
         * @param Inlvalp   YYSTYPE_File
         * @return Return ID of the next token. If is end of stream returns 0
         */
        int yylex( YYSTYPE* Inlvalp )
        {
            if ( tokens->IsEndOfStream() )
            {
                return 0;
            }

            // Get next token
            ScriptToken&    token = tokens->GetReadToken();
            tokens->IncrementReadPosition();

            // Update YYSTYPE_File
            Inlvalp->context  = &token.context;
		    Inlvalp->token    = token.tokenString;

            // Remember current token
            currentToken = &token;
            return token.tokenID;
        }

        /**
         * @brief Emit error
         *
         * @param InMessage     Error message
         * @param Inlvalp       YYSTYPE_File
         * @return Return zero if need continue, otherwise not-zero value
         */
        int yyerror( const achar *InMessage, YYSTYPE* Inlvalp )
        {
		    // Emit error
            fileParser->EmitError( Inlvalp->context, L_Sprintf( TEXT( "%s, near '%s'" ), ANSI_TO_TCHAR( InMessage ), !Inlvalp->token.empty() ? ANSI_TO_TCHAR( Inlvalp->token.data() ) : TEXT( "<TOKEN_EMPTY>" ) ) );

            // Continue
		    return 0;
        }

        /**
         * @brief Extract function code
         */
        void ExtractFunctionCode()
        {
            tokens->ExtractFunctionTokens( fileParser->GetFunctionCodeTokens() );
        }

        /**
         * @brief Get file parser
         * @return Return pointer to file parser
         */
        CScriptFileParser* GetFileParser()
	    {
		    return fileParser;
	    }

        /**
         * @brief Get current token line
         * @return Return current token line. If token isn't valid returns -1
         */
        int32 GetCurrentTokenLine() const
        {
            if( !currentToken )
            {
                return -1;
            }

            return currentToken->context.line;
        }

        /**
         * @brief Get last token line
         * @return Returns the line of the last token in current context
         */       
        int32 GetLastTokenLine() const
        {
            if ( tokens->IsEmpty() )
            {
                return -1;
            }

            return tokens->GetLastToken().context.line;
        }

    private:
        CScriptTokenStream*		tokens;         /**< Tokens stream */
	    CScriptFileParser*		fileParser;     /**< File parser */
        ScriptToken*			currentToken;   /**< Current token */
    };

    // Some Bison functions
    /*
    ==================
    yyerror_file_thread_safe
    Function for Bison to handle error cases
    ==================
    */
    static int yyerror_file_thread_safe( const achar *InMessage, void* InParam, YYSTYPE* Inlvalp )
    {
        CScriptFileParserInterface*     fileParserInterface = ( CScriptFileParserInterface* )InParam;
        return fileParserInterface->yyerror( InMessage, Inlvalp );
    }

    /*
    ==================
    yylex_file_thread_safe
    Function for Bison to take tokens
    ==================
    */    
    static int yylex_file_thread_safe( YYSTYPE* Inlvalp, void* InParam )
    {
        CScriptFileParserInterface*     fileParserInterface = ( CScriptFileParserInterface* )InParam;
        return fileParserInterface->yylex( Inlvalp );	
    }

    // Redefine some Bison functions
    #define yyparse                 yyparse_file
    #define yydebug                 yydebug_file
    #define yylex                   yylex_file_thread_safe
    #define yyerror( InMessage )    yyerror_file_thread_safe( InMessage, YYPARSE_PARAM, &yylval )
%}

%define api.pure

/* ------------------------------------------------------------------
   Token Definitions
   ------------------------------------------------------------------ */

/* Expect 0 shift/reduce conflicts */
%expect 0

/* Data tokens */
%token TOKEN_IDENT

/* Keywords */
%token TOKEN_CLASS
%token TOKEN_EXTENDS
%token TOKEN_NATIVE
%token TOKEN_TRANSIENT
%token TOKEN_ABSTRACT
%token TOKEN_DEPRECATED
%token TOKEN_CPPTEXT
%token TOKEN_CPPTEXT_BODY
%token TOKEN_WITHIN

%%

////////////////////////////////
// CLASS
////////////////////////////////

class
    : class_header class_body                                               { g_FileParser->EndDefinition( g_FileContext->GetCurrentTokenLine(), $<context>1, $<context>2 ); g_FileParser->PopContext(); }
    ;

class_header
    : TOKEN_CLASS ident class_extends class_flags semicolon                 { 
                                                                                if ( !$<withinClass>4.empty() )
                                                                                {
                                                                                    // Start class with custom within class
                                                                                    g_FileParser->StartClass( $<context>2, $<context>3, $<context>4, $<string>2, $<string>3, $<withinClass>4, $<flags>4 );
                                                                                }
                                                                                else
                                                                                {
                                                                                    // Start class with default within class
                                                                                    g_FileParser->StartClass( $<context>2, $<context>3, $<string>2, $<string>3, $<flags>4 );
                                                                                }
                                                                            }
    ;

class_extends
    : TOKEN_EXTENDS ident                                                   { $<string>$ = $<string>2; $<context>$ = $<context>2; }                
    ;

class_flags
    : TOKEN_NATIVE class_flags                                              { $<flags>$ = CLASS_Native | $<flags>2;     $<withinClass>$ = $<withinClass>2;  $<context>$ = $<context>2;  }
    | TOKEN_TRANSIENT class_flags                                           { $<flags>$ = CLASS_Transient | $<flags>2;  $<withinClass>$ = $<withinClass>2;  $<context>$ = $<context>2;  }
    | TOKEN_ABSTRACT class_flags                                            { $<flags>$ = CLASS_Abstract | $<flags>2;   $<withinClass>$ = $<withinClass>2;  $<context>$ = $<context>2;  }
    | TOKEN_DEPRECATED class_flags                                          { $<flags>$ = CLASS_Deprecated | $<flags>2; $<withinClass>$ = $<withinClass>2;  $<context>$ = $<context>2;  }
    | TOKEN_WITHIN '(' ident ')' class_flags                                { $<flags>$ = $<flags>5;                    $<withinClass>$ = $<string>3;       $<context>$ = $<context>3;  }
    | /* empty */                                                           { $<flags>$ = 0; }
    ;

class_body
    : class_body cpptext                                                    { g_FileParser->AddCppText( $<context>2, $<string>2 ); }
    | /* empty */                                                           
    ;

////////////////////////////////
// CPP TEXT
////////////////////////////////

cpptext
    : TOKEN_CPPTEXT '{' cpptext_body '}'                                    { $<string>$ = $<token>3; }
    ;

cpptext_body
    : TOKEN_CPPTEXT_BODY
    | /* empty */
    ;

////////////////////////////////
// TERMINALS
////////////////////////////////

ident
	: TOKEN_IDENT		                                                    { $<string>$ = $<token>1; $<context>$ = $<context>1; }
	; 

semicolon
    : ';'
    ;

%%

/**
 * @ingroup WorldEd
 * @brief Entry function to parse grammar a file source code
 * 
 * @param InTokens    Token stream
 * @param InFileParser      File parser
 * @return Return the result returned by yyparse
 */
int LifeScript_GrammarFile( CScriptTokenStream& InTokens, CScriptFileParser& InFileParser )
{
    CScriptFileParserInterface      fileParserInterface( InTokens, InFileParser );
    return yyparse( &fileParserInterface );
}