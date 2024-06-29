#include "Scripts/ScriptSyntaxCheckerVisitor.h"
#include "Reflection/FieldIterator.h"

/*
==================
CScriptSyntaxCheckerVisitor::CScriptSyntaxCheckerVisitor
==================
*/
CScriptSyntaxCheckerVisitor::CScriptSyntaxCheckerVisitor( const CScriptClassStub& InClassStub, const CScriptFunctionStub& InFunctionStub )
	: bHasError( false )
	, classStub( InClassStub )
	, functionStub( InFunctionStub )
{}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Nop
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )
{}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_ListItem
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_ListItem( class CScriptSyntaxNode_ListItem* InNode )
{
	// Check on syntax errors in children
	const std::vector<CScriptSyntaxNode_Base*>&		children = InNode->GetNodes();
	for ( uint32 index = 0, count = children.size(); index < count; ++index )
	{
		children[index]->AcceptVisitor( *this );
	}
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Ident
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )
{
	CScriptSyntaxNode_Base*		parentNode	= InNode->GetParentNode();
	std::wstring				identName	= ANSI_TO_TCHAR( InNode->GetName().c_str() );

	// Is this identifier its function call
	if ( parentNode && parentNode->GetNodeType() == SSNT_FuncCall )
	{
		// Try find this function in the class
		CClass*		theClass = classStub.GetCreatedClass();
		CFunction*	function = theClass->FindFunction( identName.c_str() );
		if ( !function )
		{
			Errorf( TEXT( "%s: Could not find function '%s'\n" ), InNode->GetContext().ToString().c_str(), identName.c_str() );
			bHasError = true;
		}

		// Save resolved the function
		InNode->SetCFunction( function );

		// For function call we are done
		return;
	}

	// Find property in the function (function parameters and local properties) and in the class
	CProperty*		property = FindPropertyInScopes( identName.c_str() );
	if ( !property )
	{
		Errorf( TEXT( "%s: Invalid identifier '%s'\n" ), InNode->GetContext().ToString().c_str(), identName.c_str() );
		bHasError = true;
		return;
	}

	// Save resolved property and convert property into expression value type
	InNode->SetCProperty( property );
	lastVisitedNodeData.expressionValueType = ConvertPropertyToExpressionValueType( property );
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	// Check function identifier first
	TGuardValue<LastVisitedNodeData>	lastVisitedNodeDataGuard( lastVisitedNodeData );
	CScriptSyntaxNode_Base*				syntaxNameNode = InNode->GetNameNode();
	Assert( syntaxNameNode && syntaxNameNode->GetNodeType() == SSNT_Ident );
	syntaxNameNode->AcceptVisitor( *this );

	// Get function from function identifier
	CFunction*		function = ( ( CScriptSyntaxNode_Ident* )syntaxNameNode )->GetCFunction();
	if ( !function )
	{
		return;
	}

	// Check function parameters
	CScriptSyntaxNode_Base*					syntaxParamsNode		= InNode->GetParametersNode();
	std::vector<CScriptSyntaxNode_Base*>	parameters;
	if ( syntaxParamsNode )
	{
		// We may have a few parameters and in this case it will be a list item node
		if ( syntaxParamsNode->GetNodeType() == SSNT_ListItem )
		{
			parameters = ( ( CScriptSyntaxNode_ListItem* )syntaxParamsNode )->GetNodes();
		}
		// Otherwise we have only one parameter
		else
		{
			parameters.push_back( syntaxParamsNode );
		}
	}

	// Check on too many parameters are specified
	uint32		expectedNumParameters	= function->GetNumParams();
	uint32		numParameters			= parameters.size();
	if ( numParameters > expectedNumParameters )
	{
		Errorf( TEXT( "%s: Function '%s' does not take %i param(s) but only %i\n" ), InNode->GetContext().ToString().c_str(), function->GetName().c_str(), numParameters, expectedNumParameters );
		bHasError = true;
		return;
	}

	// Check parameter types
	uint32			paramIdx = 0;
	for ( TFieldIterator<CProperty> it( function, false ); it; ++it, ++paramIdx )
	{
		// We may have less parameters specified in the function call than the expected number
		CProperty*		currentExpectedParam = *it;
		if ( paramIdx >= numParameters )
		{
			Errorf( TEXT( "%s: Too few parameters in call to function '%s'\n" ), InNode->GetContext().ToString().c_str(), function->GetName().c_str() );
			bHasError = true;
			return;
		}
		CScriptSyntaxNode_Base*		currentSyntaxParamNode = parameters[paramIdx];

		// Check on valid a node
		if ( !currentSyntaxParamNode )
		{
			Errorf( TEXT( "%s: Required parameter %i (%s) was omitted\n" ), InNode->GetContext().ToString().c_str(), paramIdx, currentExpectedParam->GetName().c_str() );
			bHasError = true;
			return;
		}

		// Check only existing parameters on value type
		if ( paramIdx < numParameters && currentSyntaxParamNode )
		{
			// Reset expression value type because after previous call may have in there a data,
			// but for make sure what all okay with parameter type it need to reset
			lastVisitedNodeData.expressionValueType = EVT_Void;

			// Visit the parameter node to get expression value type
			currentSyntaxParamNode->AcceptVisitor( *this );

			// Check property type
			if ( !CheckExpressionValueType( lastVisitedNodeData.expressionValueType, currentExpectedParam ) )
			{
				Errorf( TEXT( "%s: Incompatible type for parameter '%s' in function '%s'\n" ), InNode->GetContext().ToString().c_str(), currentExpectedParam->GetName().c_str(), function->GetName().c_str() );
				bHasError = true;
				return;
			}
		}
	}
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Return
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )
{}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_IntConst
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_IntConst( class CScriptSyntaxNode_IntConst* InNode )
{
	// Mark expression value type of the node as 'integer'
	lastVisitedNodeData.expressionValueType = EVT_Int;
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FloatConst
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FloatConst( class CScriptSyntaxNode_FloatConst* InNode )
{
	// Mark expression value type of the node as 'float'
	lastVisitedNodeData.expressionValueType = EVT_Float;
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_StringConst
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_StringConst( class CScriptSyntaxNode_StringConst* InNode )
{
	// Mark expression value type of the node as 'string'
	lastVisitedNodeData.expressionValueType = EVT_String;
}

/*
==================
CScriptSyntaxCheckerVisitor::ConvertPropertyToExpressionValueType
==================
*/
CScriptSyntaxCheckerVisitor::EExpressionValueType CScriptSyntaxCheckerVisitor::ConvertPropertyToExpressionValueType( CProperty* InProperty ) const
{
	Assert( InProperty );
	CClass*		propertyClass = InProperty->GetClass();

	// TODO yehor.pohuliaka - Need add cast types (e.g int -> float, float -> int and etc)
	// Is this integer type
	if ( propertyClass->HasAnyCastFlags( CASTCLASS_CIntProperty ) )
	{
		return EVT_Int;
	}

	// Is this float type
	else if ( propertyClass->HasAnyCastFlags( CASTCLASS_CFloatProperty ) )
	{
		return EVT_Float;
	}

	// Is this string type
	else if ( propertyClass->HasAnyCastFlags( CASTCLASS_CStringProperty ) )
	{
		return EVT_String;
	}

	// Unknown type
	return EVT_Void;
}

/*
==================
CScriptSyntaxCheckerVisitor::FindPropertyInScopes
==================
*/
CProperty* CScriptSyntaxCheckerVisitor::FindPropertyInScopes( const CName& InName ) const
{
	// Find property in the function (function parameters and local properties)
	// Local function properties
	CFunction*	function	= functionStub.GetCreatedFunction();
	for ( TFieldIterator<CProperty> it( function, false ); it; ++it )
	{
		CProperty*		currentProperty = *it;
		if ( !currentProperty->HasAnyFlags( CPF_Parmeter ) && currentProperty->GetCName() == InName )
		{
			return *it;
		}
	}

	// Function parameters
	for ( TFieldIterator<CProperty> it( function, false ); it; ++it )
	{
		CProperty*		currentProperty = *it;
		if ( currentProperty->HasAnyFlags( CPF_Parmeter ) && currentProperty->GetCName() == InName )
		{
			return *it;
		}
	}

	// Find property in the class
	CClass*		theClass = classStub.GetCreatedClass();
	for ( TFieldIterator<CProperty> it( theClass ); it; ++it )
	{
		if ( it->GetCName() == InName )
		{
			return *it;
		}
	}

	// We nothing found
	return nullptr;
}