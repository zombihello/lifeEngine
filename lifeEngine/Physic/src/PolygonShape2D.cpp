#include "../PolygonShape2D.h"

//-------------------------------------------------------------------------//

le::PolygonShape2D::PolygonShape2D( Vector2f SizeBody )
{
	Shape.SetAsBox( SizeBody.x/30.f/2, SizeBody.y/30.f/2 );

	fAngle = 0;
	this->SizeBody = SizeBody;
	Center = Vector2f( 0,0 );
	fDef.shape = &Shape;
	Fixture = NULL;
}

//-------------------------------------------------------------------------//

le::PolygonShape2D::PolygonShape2D( Vector2f SizeBody, Vector2f Center, float Angle )
{
	Shape.SetAsBox( SizeBody.x/30.f/2, SizeBody.y/30.f/2, b2Vec2( Center.x/30.f, Center.y/30.f ), Angle*57.29577f );

	fAngle = Angle*57.29577f;
	this->SizeBody = SizeBody;
	this->Center = Center;
	fDef.shape = &Shape;
	Fixture = NULL;
}

//-------------------------------------------------------------------------//

void le::PolygonShape2D::SetPropirtes( float Density, float Restitution, float Friction )
{
	fDef.density = Density;
	fDef.restitution = Restitution;
	fDef.friction = Friction;
}

//-------------------------------------------------------------------------//

void le::PolygonShape2D::SetSize( Vector2f SizeBody )
{
	this->SizeBody.x += SizeBody.x - this->SizeBody.x;
	this->SizeBody.y += SizeBody.y - this->SizeBody.y;

	Shape.SetAsBox( this->SizeBody.x/30.f/2, this->SizeBody.y/30.f/2, b2Vec2( Center.x/30.f, Center.y/30.f ), fAngle );
}

//-------------------------------------------------------------------------//

void le::PolygonShape2D::SetFixture( b2Fixture *Fixture )
{
	this->Fixture = Fixture;
}

//-------------------------------------------------------------------------//

b2FixtureDef& le::PolygonShape2D::GetFixtureDef( )
{
	return fDef;
}

//-------------------------------------------------------------------------//

b2Fixture& le::PolygonShape2D::GetFixture( )
{
	return *Fixture;
}

//-------------------------------------------------------------------------//
