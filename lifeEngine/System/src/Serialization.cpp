#include "../Serialization.h"
#include "../System.h"
using namespace le;

//-------------------------------------------------------------------------//

static string							sSaveText = "<!-- lifeEngine " + string( ENGINE_VERSION ) + " -->\n<SAVE ";
static string						    sAttributesSave;

static vector<string>					vBuffer;
static vector<SaveValuables>			vSaveValuables;

//-------------------------------------------------------------------------//

Serialization::Serialization( string sNameClass )
{
	this->sNameClass = sNameClass;

	vBuffer.push_back( string( "<" + sNameClass + "/>\n" ) );

	iIdClass = vBuffer.size() - 1;
	isCreateTag = false;
}

//-------------------------------------------------------------------------//

Serialization::~Serialization()
{
	vBuffer[ iIdClass ] = "";
}

//-------------------------------------------------------------------------//

void Serialization::Add( int value , string nameValue )
{
	string tmp = vBuffer[ iIdClass ];

	if ( !isCreateTag )
		for ( int i = 1; i < 4; i++ )
			tmp.erase( tmp.end() - 1 );
	else
		for ( int i = 0; i < 7 + sNameClass.size(); i++ )
			tmp.erase( tmp.end() - 1 );

	stringstream StringStream;
	StringStream << value;

	tmp += " " + nameValue + "=\"" + StringStream.str() + "\"/>\n";
	if ( isCreateTag ) tmp += "</" + sNameClass + ">\n";

	vBuffer[ iIdClass ] = tmp;
}

//-------------------------------------------------------------------------//

void Serialization::Add( float value , string nameValue )
{
	string tmp = vBuffer[ iIdClass ];

	if ( !isCreateTag )
		for ( int i = 1; i < 4; i++ )
			tmp.erase( tmp.end() - 1 );
	else
		for ( int i = 0; i < 7 + sNameClass.size(); i++ )
			tmp.erase( tmp.end() - 1 );

	stringstream StringStream;
	StringStream << value;

	tmp += " " + nameValue + "=\"" + StringStream.str() + "\"/>\n";
	if ( isCreateTag ) tmp += "</" + sNameClass + ">\n";

	vBuffer[ iIdClass ] = tmp;
}

//-------------------------------------------------------------------------//

void Serialization::Add( string value , string nameValue )
{
	string tmp = vBuffer[ iIdClass ];

	if ( !isCreateTag )
		for ( int i = 1; i < 4; i++ )
			tmp.erase( tmp.end() - 1 );
	else
		for ( int i = 0; i < 7 + sNameClass.size(); i++ )
			tmp.erase( tmp.end() - 1 );

	stringstream StringStream;
	StringStream << value;

	tmp += " " + nameValue + "=\"" + StringStream.str() + "\"/>\n";
	if ( isCreateTag ) tmp += "</" + sNameClass + ">\n";

	vBuffer[ iIdClass ] = tmp;
}

//-------------------------------------------------------------------------//

SaveValuables* Serialization::GetValuables()
{
	for ( int i = 0; i < vSaveValuables.size(); i++ )
	{
		if ( !vSaveValuables[ i ].bIsLoadSave && vSaveValuables[ i ].sNameClass == sNameClass )
			return &vSaveValuables[ i ];
	}

	return NULL;
}

//-------------------------------------------------------------------------//

vector<SaveValuables> Serialization::GetSaveValuables()
{
	return vSaveValuables;
}

//-------------------------------------------------------------------------//

void Serialization::SaveInFile( string route )
{
	sSaveText += sAttributesSave + ">\n";

	for ( auto i = 0; i < vBuffer.size(); i++ )
		sSaveText += vBuffer[ i ];

	ofstream of( route.c_str() );
	of << sSaveText << "</SAVE>\n";
	of.close();
	Clear();
}

//-------------------------------------------------------------------------//

void Serialization::LoadFromFile( string route )
{
	TiXmlDocument File( route.c_str() );

	// Загружаем файл
	if ( !File.LoadFile() )
	{
		cout << "Error: File [" + route + "] Not Found" << endl;
		return;
	}

	// Работаем с контейнером SAVE
	TiXmlElement *save;
	save = File.FirstChildElement( "SAVE" );

	SaveValuables MainSave;
	MainSave.sNameClass = "Main";

	if ( save->Attribute( "title" ) != NULL )
		MainSave.mProperties[ "title" ] = save->Attribute( "title" );

	if ( save->Attribute( "level" ) != NULL )
		MainSave.mProperties[ "level" ] = save->Attribute( "level" );

	vSaveValuables.push_back( MainSave );

	// Работаем с контейнерами в SAVE
	TiXmlElement *Tag;
	Tag = save->FirstChildElement();
	while ( Tag )
	{
		SaveValuables Valuables;
		Valuables.sNameClass = Tag->Value();
		Valuables.bIsLoadSave = false;

		// Работаем с атрибутами в контейнере
		TiXmlAttribute *AttributeTag;
		AttributeTag = Tag->FirstAttribute();
		while ( AttributeTag )
		{
			Valuables.mProperties[ AttributeTag->Name() ] = AttributeTag->Value();
			AttributeTag = AttributeTag->Next();
		}

		// Работаем с вложеным контейнером
		TiXmlElement *inTag;
		inTag = Tag->FirstChildElement();
		while ( inTag )
		{
			SaveValuables inTag_Valuables;
			inTag_Valuables.sNameClass = Tag->Value();
			inTag_Valuables.bIsLoadSave = false;

			// Работаем с атрибутами в контейнере
			TiXmlAttribute *AttributeTag;
			AttributeTag = inTag->FirstAttribute();
			while ( AttributeTag )
			{
				inTag_Valuables.mProperties[ AttributeTag->Name() ] = AttributeTag->Value();
				AttributeTag = AttributeTag->Next();
			}

			Valuables.mSaveValuables[ inTag_Valuables.sNameClass ] = inTag_Valuables;
			inTag = inTag->NextSiblingElement();
		}

		vSaveValuables.push_back( Valuables );
		Tag = Tag->NextSiblingElement();
	}
}

//-------------------------------------------------------------------------//

void Serialization::CreateTag( string nameTag )
{
	string tmp = vBuffer[ iIdClass ];

	if ( !isCreateTag )
	{
		for ( int i = 1; i < 4; i++ )
			tmp.erase( tmp.end() - 1 );

		tmp += ">\n";
	}
	else
	{
		for ( int i = 0; i < 4 + sNameClass.size(); i++ )
			tmp.erase( tmp.end() - 1 );
	}

	tmp += "<" + nameTag + "/>\n";
	tmp += "</" + sNameClass + ">\n";

	isCreateTag = true;

	vBuffer[ iIdClass ] = tmp;
}

//-------------------------------------------------------------------------//

void Serialization::Clear()
{
	sSaveText = "<!-- lifeEngine " + string( ENGINE_VERSION ) + " -->\n<SAVE ";

	// Сбрасываем буфер сохранения до такого типа [ <ИМЯ_КЛАССА/> ]
	for ( int id = 0; id < vBuffer.size(); id++ )
	{
		string sNameClass;
		string tmpString = vBuffer[ id ];

		for ( int i = 0; i < tmpString.size() && tmpString[ i ] != ' ' && tmpString[ i ] != '/'; i++ )
			sNameClass += tmpString[ i ];

		vBuffer[ id ] = sNameClass + "/>\n";
	}
}

//-------------------------------------------------------------------------//

void Serialization::SetAttributsSave( string titleSave , string levelSave )
{
	sAttributesSave = "title=\"" + titleSave + "\" level=\"" + levelSave + "\"";
}

//-------------------------------------------------------------------------//