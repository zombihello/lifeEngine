%{
    #include "ScriptFunctionParser_Bison.cpp.h"
    #include "Core.h"
    #include "Scripts/ScriptTokenStream.h"
    #include "Scripts/ScriptFunctionParser.h"
    #include "Scripts/ScriptSyntaxNode.h"

    // Some Bison defines
    #define YYDEBUG                 1
    #define YYERROR_VERBOSE         1
    #define YYPARSE_PARAM           param 
    #define YYLEX_PARAM             param 
    #define YYSTYPE                 YYSTYPE_Function

    #define g_FileContext           ( ( CScriptFunctionParserInterface* )param )
    #define g_FunctionParser        ( ( ( CScriptFunctionParserInterface* )param )->GetFunctionParser() )

    /**
    * @ingroup WorldEd
    * @brief Structure to pass around parser parameters
    */
    struct YYSTYPE_Function
    {
        /**
         * @brief Constructor
         */
        FORCEINLINE YYSTYPE_Function()
            : node( nullptr )
            , integer( 0 )
            , floating( 0.f )
        {}

        /**
         * @brief Constructor of copy
         * @param InOther   Other YYSTYPE_Function
         */
        FORCEINLINE YYSTYPE_Function( const YYSTYPE_Function& InOther )
            : context( InOther.context )
            , token( InOther.token )
            , node( InOther.node )
            , string( InOther.string )
            , integer( InOther.integer )
            , floating( InOther.floating )
        {}

        /**
         * @brief Constructor of move
         * @param InOther   Other YYSTYPE_Function
         */
        FORCEINLINE YYSTYPE_Function( YYSTYPE_Function&& InOther )
            : context( std::move( InOther.context ) )
            , token( std::move( InOther.token ) )
            , node( std::move( InOther.node ) )
            , string( std::move( InOther.string ) )
            , integer( std::move( InOther.integer ) )
            , floating( std::move( InOther.floating ) )
        {}

        /**
         * @brief Destructor
         */
        FORCEINLINE ~YYSTYPE_Function()
        {}

        /**
         * @brief Override operator of copy
         *
         * @param InOther   Other YYSTYPE_File
         * @return Return reference to self
         */
        FORCEINLINE YYSTYPE_Function& operator=( const YYSTYPE_Function& InOther )
	    {
	    	if ( this != &InOther )
	    	{
	    		context     = InOther.context;
	    		token       = InOther.token;
                node        = InOther.node;
                string      = InOther.string;
                integer     = InOther.integer;
                floating    = InOther.floating;
	    	}
	    	return *this;
	    }

        /**
         * @brief Override operator of move
         *
         * @param InOther   Other YYSTYPE_Function
         * @return Return reference to self
         */
        FORCEINLINE YYSTYPE_Function& operator=( YYSTYPE_Function&& InOther )
	    {
	    	if ( this != &InOther )
	    	{
	    		context     = std::move( InOther.context );
	    		token       = std::move( InOther.token );
                node        = std::move( InOther.node );
                string      = std::move( InOther.string );
                integer     = std::move( InOther.integer );
                floating    = std::move( InOther.floating );
	    	}
	    	return *this;
	    }

        ScriptFileContext           context;    /**< Script file context */
        std::string_view            token;      /**< Token in string format */
        CScriptSyntaxNode_Base*		node;       /**< Script syntax node */
        std::string_view            string;     /**< String value */
        uint32                      integer;    /**< Integer value */
        float                       floating;   /**< Float value */
    };

    /**
    * @ingroup WorldEd
    * @brief Script function parser interface
    */
    class CScriptFunctionParserInterface
    {
    public:
        /**
         * @brief Constructor
         *
         * @param InTokens          Tokens stream
         * @param InFunctionParser  Function parser
         */
         CScriptFunctionParserInterface( CScriptTokenStream& InTokens, CScriptFunctionParser& InFunctionParser )
            : tokens( &InTokens )
            , functionParser( &InFunctionParser )
         {}

        /**
         * @brief Get next token
         *
         * @param Inlvalp   YYSTYPE_Function
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

            // Update YYSTYPE_Function
            Inlvalp->context  = token.context;
		    Inlvalp->token    = token.tokenString;
            return token.tokenID;
        }

        /**
         * @brief Emit error
         *
         * @param InMessage     Error message
         * @param Inlvalp       YYSTYPE_Function
         * @return Return zero if need continue, otherwise not-zero value
         */
        int yyerror( const achar *InMessage, YYSTYPE* Inlvalp )
        {
            // Emit error
            functionParser->EmitError( Inlvalp->context, L_Sprintf( TEXT( "%s, near '%s'" ), ANSI_TO_TCHAR( InMessage ), !Inlvalp->token.empty() ? ANSI_TO_TCHAR( Inlvalp->token.data() ) : TEXT( "<TOKEN_EMPTY>" ) ) );

            // Continue
		    return 0;
        }

        /**
         * @brief Get function parser
         * @return Return pointer to function parser
         */
        CScriptFunctionParser* GetFunctionParser()
	    {
		    return functionParser;
	    }

    private:
        CScriptTokenStream*		    tokens;             /**< Tokens stream */
	    CScriptFunctionParser*		functionParser;     /**< Function parser */
    };

    // Some Bison functions
    /*
    ==================
    yyerror_function_thread_safe
    Function for Bison to handle error cases
    ==================
    */
    static int yyerror_function_thread_safe( const achar *InMessage, void* InParam, YYSTYPE* Inlvalp )
    {
        CScriptFunctionParserInterface*     functionParserInterface = ( CScriptFunctionParserInterface* )InParam;
        return functionParserInterface->yyerror( InMessage, Inlvalp );
    }

    /*
    ==================
    yylex_function_thread_safe
    Function for Bison to take tokens
    ==================
    */    
    static int yylex_function_thread_safe( YYSTYPE* Inlvalp, void* InParam )
    {
        CScriptFunctionParserInterface*     functionParserInterface = ( CScriptFunctionParserInterface* )InParam;
        return functionParserInterface->yylex( Inlvalp );	
    }

    /*
    ==================
    MergeContext
    Produces a single context that encompasses the space occupied by the combined of the inputs ("Union")
    ==================
    */ 
    static ScriptFileContext MergeContext( const ScriptFileContext& InAContext, const ScriptFileContext& InBContext )
    {
        //Assert( InAContext->file == InBContext->file );
        ScriptFileContext   mergedContext( InAContext );
        if( InBContext.charPosition < InAContext.charPosition )
        {
            mergedContext.line              = InBContext.line;
            mergedContext.charLineStart     = InBContext.charLineStart;
            mergedContext.charPosition      = InBContext.charPosition;
        }

        if( InBContext.charEndPosition > InAContext.charPosition )
        {
            mergedContext.charEndPosition = InBContext.charEndPosition;
        }

        return mergedContext;
    }

    // Redefine some Bison functions
    #define yyparse                 yyparse_function
    #define yydebug                 yydebug_function
    #define yylex                   yylex_function_thread_safe
    #define yyerror( Message )      yyerror_function_thread_safe( Message, YYPARSE_PARAM, &yylval )
%}

%define api.pure

/* ------------------------------------------------------------------
   Token Definitions
   ------------------------------------------------------------------ */

/* Expect 0 shift/reduce conflicts */
%expect 0

/* THIS LIST SHOLD MATCH EXACTLY THE OTHER LIST IN THE ScriptFileParser.bison */
/* THIS LIST SHOLD MATCH EXACTLY THE OTHER LIST IN THE ScriptFileParser.bison */
/* THIS LIST SHOLD MATCH EXACTLY THE OTHER LIST IN THE ScriptFileParser.bison */
/* THIS LIST SHOLD MATCH EXACTLY THE OTHER LIST IN THE ScriptFileParser.bison */

/* Data tokens */
%token TOKEN_IDENT
%token TOKEN_INTEGER
%token TOKEN_FLOAT
%token TOKEN_STRING

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
%token TOKEN_FUNCTION
%token TOKEN_NOEXPORT
%token TOKEN_VOID
%token TOKEN_RETURN

%%

////////////////////////////////
// FUNCTION CODE
////////////////////////////////

function_code
    : statement_list                                        { $<node>$ = new CScriptSyntaxNode_ListItem( yylval.context, $<node>1 ); g_FunctionParser->SetRootSyntaxNode( $<node>$ ); }
    ;

////////////////////////////////
// STATEMENTS
////////////////////////////////

statement_list
    : statement statement_list                              { $<node>$ = new CScriptSyntaxNode_ListItem( yylval.context, $<node>1 ); ( ( CScriptSyntaxNode_ListItem* )$<node>$ )->AddNode( $<node>2 ); }
    | /* empty */                                           { $<node>$ = new CScriptSyntaxNode_Nop( yylval.context ); }
    ;

statement
    : empty_statement                                       { $<node>$ = $<node>1; }
    | expression_statement                                  { $<node>$ = $<node>1; }
    | jump_statement                                        { $<node>$ = $<node>1; }
    ;

empty_statement
    : /* empty */ ';'                                       { $<node>$ = nullptr; }
    ;

expression_statement
    : expression ';'                                        { $<node>$ = $<node>1; }    
    ;

jump_statement
    : TOKEN_RETURN ';'                                      { $<node>$ = new CScriptSyntaxNode_Return( $<context>1 ); }
    ;

////////////////////////////////
// EXPRESSIONS
////////////////////////////////

expression
    : primary_expression                                    { $<node>$ = $<node>1; }
    ;

primary_expression
    : postfix_expression                                    { $<node>$ = $<node>1; }
    | base_const                                            { $<node>$ = $<node>1; }
    ;

postfix_expression
    : prefix_expression                                     { $<node>$ = $<node>1; }
    | postfix_expression '(' func_param_list ')'            { $<node>$ = new CScriptSyntaxNode_FuncCall( MergeContext( $<context>1, $<context>4 ), $<node>1, $<node>3 ); }

prefix_expression
    : base_ident                                            { $<node>$ = $<node>1; }
    ;

func_param_list
    : func_expression_list                                  { $<node>$ = $<node>1; }
    ;

func_expression_list
    : func_expression                                       { $<node>$ = $<node>1; }
    | func_expression ',' func_expression_list              { $<node>$ = new CScriptSyntaxNode_ListItem( yylval.context, $<node>1 ); ( ( CScriptSyntaxNode_ListItem* )$<node>$ )->AddNode( $<node>3 ); }
    ;

func_expression
    : expression                                            { $<node>$ = $<node>1; }
    | /* empty */                                           { $<node>$ = nullptr; }
    ;

base_ident
    : ident                                                 { $<node>$ = new CScriptSyntaxNode_Ident( yylval.context, $<string>1 ); }
    ;

base_const
    : integer_const                                         { $<node>$ = $<node>1; }
    | float_const                                           { $<node>$ = $<node>1; }
    | string_const                                          { $<node>$ = $<node>1; }
    ;

integer_const
    : integer                                               { $<node>$ = new CScriptSyntaxNode_IntConst( $<context>1, $<integer>1 ); }
    ;

float_const
    : float                                                 { $<node>$ = new CScriptSyntaxNode_FloatConst( $<context>1, $<floating>1 ); }
    ;

string_const
    : string                                                { $<node>$ = new CScriptSyntaxNode_StringConst( $<context>1, $<string>1 ); }
    ;

////////////////////////////////
// TERMINALS
//////////////////////////////// 

integer
    : TOKEN_INTEGER                                         {
                                                                Assert( !$<token>1.empty() );
                                                                if ( !L_Strtoi( ( int32& )$<integer>$, $<token>1.data(), nullptr, NUMBER_BASE_AUTO ) )
                                                                {
                                                                    if ( errno == ERANGE )
                                                                    {
                                                                        yyerror( "Integer overflow" );
                                                                    }
                                                                    else
                                                                    {
                                                                        yyerror( "Token is not a number" );
                                                                    }
                                                                    YYERROR;
                                                                }

                                                                $<string>$ = $<token>1; $<context>$ = $<context>1;
                                                            }
    ;

float
    : TOKEN_FLOAT                                           { Assert( !$<token>1.empty() ); $<floating>$ = L_Atof( $<token>1.data() ); $<string>$ = $<token>1; $<context>$ = $<context>1; }
    ;

string
    : TOKEN_STRING                                          { $<string>$ = $<token>1; $<context>$ = $<context>1; }
    ;

ident
	: TOKEN_IDENT                                           { $<string>$ = $<token>1; }
	; 

%%

/**
 * @ingroup WorldEd
 * @brief Entry function to parse grammar a function source code
 * 
 * @param InTokens      Tokens stream
 * @param InFunctionParser  Function parser
 * @return Return the result returned by yyparse
 */
int LifeScript_GrammarFunction( CScriptTokenStream& InTokens, CScriptFunctionParser& InFunctionParser )
{
    CScriptFunctionParserInterface      functionParserInterface( InTokens, InFunctionParser );
    return yyparse( &functionParserInterface );
}