    /* Options */
%option nounistd
%option noyywrap
%option 8bit
%option reentrant
%option nodefault
%option extra-type="LexerInternalState*"
%option verbose

%option nounput
%option noyyget_lineno  noyyset_lineno
%option noyyget_debug   noyyset_debug
%option noyyget_lval    noyyset_lval
%option noyyget_out     noyyset_out
%option noyyget_in      noyyset_in

    /* C++ Section */
%{
    #include "ScriptFileParser_Bison.cpp.h"
    #include "Logger/LoggerMacros.h"
    #include "Scripts/LexerListener.h"

    #define YY_FATAL_ERROR( Message )   { yyguts_t* yyg = ( struct yyguts_t* )yyscanner; yyextra->EmitError(); }
    #define YY_USER_ACTION              yyextra->StoreTokenStart(); yyextra->UpdateContext( yytext, yyleng );

    #define EMIT_TOKEN( TokeID )        yyextra->EmitToken( TokeID )
    #define EMIT_SEQUENCE( TokeID )     yyextra->EmitSequence( TokeID )
    #define EMIT_COMMENT()              yyextra->EmitComment()
    #define EMIT_ERROR()                yyextra->EmitError()

    /*
    ==================
    CountUtf8Bytes
    ==================
    */
    static uint32 CountUtf8Bytes( const achar InUTF8Byte )
    {
        unsigned long      index = 0;
        unsigned long      mask = ( ~InUTF8Byte );

        #if PLATFORM_WINDOWS
            _BitScanReverse( &index, mask );
        #else
            // We're counting the bits 
	        index = 31 - __lzcnt32( mask );
        #endif // PLATFORM_WINDOWS

        // We have the 0-index Context of the most significant bit, which we need to turn into a count of the number of bits (or 1-index Context)
	    // Then reverse the order so that we have a count of the number of bits leading up to the MSB
	    return 8 - ( index + 1 );
    }

    /*
    ==================
    CountCharacters
    ==================
    */  
    uint32 CountCharacters( const achar* InToken, uint32 InSize )
    {
        uint32      length = 0;
	    for ( uint32 index = 0; index < InSize; ++index )
	    {
	    	if ( ( InToken[index] & 0x80 ) )
	    	{
	    		const uint32    sizeOfUtf8Character = CountUtf8Bytes( InToken[index] );
	    		const uint32    bytesToSkip         = sizeOfUtf8Character - 1;
			    index += bytesToSkip;
		    }

		    ++length;
	    }

	    return length;   
    }

    /**
     * @ingroup WorldEd
     * @brief Internal state of the lexer
     */
    struct LexerInternalState : public LexerState
    {
        /**
         * @brief Constructor
         *
         * @param InSource          The raw input buffer
         * @param InLexerListener   Lexer listener
         */
        FORCEINLINE LexerInternalState( const achar* InSourceCode, CBaseLexerListener* InLexerListener )
            : LexerState( InSourceCode )
            , lexerListener( InLexerListener )
        {}

        /**
         * @brief Next line
         */
        FORCEINLINE void NextLine()
        {
            ++currentContext.line;
            lineStart = currentContext;
        }

        /**
         * @brief Start to store token
         */
        FORCEINLINE void StoreTokenStart()
        {
            tokenStart = currentContext;
        }

        /**
         * @brief Start to store sequence
         * @param InCharactersToSkip    Number characters to skip
         */
        FORCEINLINE void StoreSequenceStart( uint32 InCharactersToSkip = 0 )
        {
            sequenceStart = tokenStart;
            if ( InCharactersToSkip )
            {
                sequenceStart.byteOffset    += InCharactersToSkip;
                sequenceStart.charOffset    += InCharactersToSkip;
            }

            sectionLineStart = lineStart;
        }

        /**
         * @brief Update current context
         *
         * @param InToken           The token
         * @param InTokenByteLength Length of the token in bytes
         */
        FORCEINLINE void UpdateContext( const achar* InToken, uint32 InTokenByteLength )
        {
            currentContext.byteOffset += InTokenByteLength;
            currentContext.charOffset += CountCharacters( InToken, InTokenByteLength );
        }

        /**
         * @brief Emit token
         * @param InTokenID     Token ID
         */
        FORCEINLINE void EmitToken( uint32 InTokenID )
        {
            lexerListener->Token( *this, InTokenID );
        }

        /**
         * @brief Emit sequence
         * @param InTokenID     Token ID
         */
        FORCEINLINE void EmitSequence( uint32 InTokenID )
        {
            lexerListener->Sequence( *this, InTokenID );
        }

        /**
         * @brief Emit comment
         */
        FORCEINLINE void EmitComment()
        {
            lexerListener->Comment( *this );
        }

        /**
         * @brief Emit error
         */
        FORCEINLINE void EmitError()
        {
            lexerListener->Error( *this );
        }

    private:
        CBaseLexerListener*     lexerListener;      /**< Lexer listener */
    };
%}

    /* C++ Top Section */
%top {
    #include <stdint.h>
    #include <string>

    #include "CoreDefines.h"

    #if PLATFORM_WINDOWS
        #include <io.h>         // For fix error with unknown 'isatty'
    #endif // PLATFORM_WINDOWS
}

    /* https://westes.github.io/flex/manual/Numbers.html#Numbers */
dseq      ([[:digit:]]+)
dseq_opt  ([[:digit:]]*)
frac      (({dseq_opt}"."{dseq})|{dseq}".")
fsuff     [fF]
fsuff_opt ({fsuff}?)

	/* Decimal floating const */
dfc       (({frac}{fsuff_opt})|({dseq}{fsuff_opt}))

    /* Definitions */
%x MultiLineComment
%x StringLiteral

%%
 
    /* Rules */

    /* UTF-8 Byte Order Mark */
\xef\xbb\xbf									yyextra->currentContext.charOffset = 0;

    /* Keywords */
"class"                                         EMIT_TOKEN( TOKEN_CLASS );
"extends"                                       EMIT_TOKEN( TOKEN_EXTENDS );
"function"                                      EMIT_TOKEN( TOKEN_FUNCTION );
"super"                                         EMIT_TOKEN( TOKEN_SUPER );
"this"                                          EMIT_TOKEN( TOKEN_THIS );

    /* Type names */
"void"                                          EMIT_TOKEN( TOKEN_VOID_TYPE );
"int"                                           EMIT_TOKEN( TOKEN_INTEGER_TYPE );
"float"                                         EMIT_TOKEN( TOKEN_FLOAT_TYPE );
"string"                                        EMIT_TOKEN( TOKEN_STRING_TYPE );

    /* Function parsing tokens */
"native"                                        EMIT_TOKEN( TOKEN_NATIVE );

    /* Syntax */
"("												EMIT_TOKEN( '(' ); 
")"												EMIT_TOKEN( ')' );
"{"												{ EMIT_TOKEN( '{' ); ++yyextra->scopeLevel; }
"}"												{ EMIT_TOKEN( '}' ); --yyextra->scopeLevel; }
"["												EMIT_TOKEN( '[' );
"]"												EMIT_TOKEN( ']' );
"<"												EMIT_TOKEN( '<' );
">"												EMIT_TOKEN( '>' );
"&"												EMIT_TOKEN( '&' );
"|"												EMIT_TOKEN( '|' );
"^"												EMIT_TOKEN( '^' );
"~"												EMIT_TOKEN( '~' );
"+"												EMIT_TOKEN( '+' );
"-"												EMIT_TOKEN( '-' );
"*"												EMIT_TOKEN( '*' );
"/"												EMIT_TOKEN( '/' );
"%"												EMIT_TOKEN( '%' );
"!"												EMIT_TOKEN( '!' );
"."												EMIT_TOKEN( '.' );
";"												EMIT_TOKEN( ';' );
"?"												EMIT_TOKEN( '?' );
"="												EMIT_TOKEN( '=' );
","												EMIT_TOKEN( ',' );
":"												EMIT_TOKEN( ':' );

    /* Identifier */
[a-zA-Z_]+[a-zA-Z_0-9]*							EMIT_TOKEN( TOKEN_IDENT );

    /* Invalid Unicode chars */
[^\x00-\x7F]*									EMIT_ERROR();

    /* Whitespace */
[ \t\r]

    /* Integer */
[0-9]+											EMIT_TOKEN( TOKEN_INTEGER );

	/* Hexadecimal Integer */
"0"[Xx][0-9a-fA-F]+								EMIT_TOKEN( TOKEN_INTEGER );

    /* float */
{dfc}											EMIT_TOKEN( TOKEN_FLOAT );

    /* Strings */
\"                                              { yyextra->StoreSequenceStart(); BEGIN( StringLiteral ); }
<StringLiteral>
{
    [^"]*
	\"											{ EMIT_SEQUENCE( TOKEN_STRING ), BEGIN( INITIAL ); }
}

    /* Comments */
"//"[^\r\n]*									{ yyextra->StoreSequenceStart(); EMIT_COMMENT(); }
"/*"											{ yyextra->StoreSequenceStart(); BEGIN( MultiLineComment ); }
<MultiLineComment>
{
	[^*\n]+
	"*"
	"*/"										{ EMIT_COMMENT(); BEGIN( INITIAL ); }
}

    /* Next line */
<*>\n                                           yyextra->NextLine();

    /* Default Rule */
.|\n                                            EMIT_ERROR();

%%

/**
 * @ingroup WorldEd
 * @brief Entry function to tokenize LifeScript source code
 * 
 * @param InSourceCode  Source code
 */
void LifeScript_Tokenize( const std::string& InSourceCode, CBaseLexerListener* InLexerListener )
{
    yyscan_t            scanner;
    LexerInternalState  lexerData( InSourceCode.c_str(), InLexerListener );
    
	yylex_init_extra( &lexerData, &scanner );
    YY_BUFFER_STATE     buffer = yy_scan_string( InSourceCode.c_str(), scanner );    // This buffer will be parsed
    yylex( scanner );

    yy_delete_buffer( buffer, scanner );
    yylex_destroy( scanner );
}