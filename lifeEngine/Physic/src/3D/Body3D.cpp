#include "..\..\3D\Body3D.h"

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_StaticPlane ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btStaticPlaneShape( btVector3( ShapeType.PlaneNormal.x, ShapeType.PlaneNormal.y, ShapeType.PlaneNormal.z ), ShapeType.fPlaneConstant );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Box ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btBoxShape( btVector3( ShapeType.Size.x / 2, ShapeType.Size.y / 2, ShapeType.Size.z / 2 ) );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Capsule ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btCapsuleShape( ShapeType.fRadius, ShapeType.fHeight );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );

	//Body->setAngularFactor( btVector3( 0, 0, 0 ) );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Cone ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btConeShape( ShapeType.fRadius, ShapeType.fHeight );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Cylinder ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btCylinderShape( btVector3( ShapeType.Size.x / 2, ShapeType.Size.y / 2, ShapeType.Size.z / 2 ) );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Mesh ShapeType )
{
	Physic3D = &Physic;
	vCollisionIdVertexs = ShapeType.vIdVertexs;
	vCollisionVertexs = ShapeType.vVertexs;

	IndexVertexArrays = new btTriangleIndexVertexArray
		(
		( int ) ( vCollisionIdVertexs->size( ) / 3 ),
		&(*vCollisionIdVertexs)[0],
		sizeof( int ) * 3,
		( int ) ( vCollisionVertexs->size( ) / 3 ),
		&(*vCollisionVertexs)[0],
		sizeof( float ) * 3
		);

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btConvexTriangleMeshShape( IndexVertexArrays );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

le::Body3D::Body3D( le::Physic3D& Physic, Body3D_ConstructionInfo* ConstructionInfo, le::ShapeType_Sphere ShapeType )
{
	Physic3D = &Physic;
	IndexVertexArrays = NULL;

	MotionState = new btDefaultMotionState( btTransform( ConstructionInfo->Rotation, ConstructionInfo->Position ) );
	Shape = new btSphereShape( ShapeType.fRadius );

	if ( ConstructionInfo->fMass != 0 )
		Shape->calculateLocalInertia( ConstructionInfo->fMass, ConstructionInfo->Inertia );

	btRigidBody::btRigidBodyConstructionInfo BulletConstructionInfo( ConstructionInfo->fMass, MotionState, Shape, ConstructionInfo->Inertia );

	Body = new btRigidBody( BulletConstructionInfo );
	Physic3D->AddBody( Body );
}

//-------------------------------------------------------------------------//

void le::Body3D::Move( le::Body3D::TypeMove typeMove, Vector3f Factor, Vector3f CenterBody )
{
	switch ( typeMove )
	{
	case le::Body3D::Force:

		if ( CenterBody == Vector3f() )
			Body->applyCentralForce( btVector3( Factor.x, Factor.y, Factor.z ) );
		else
			Body->applyForce( btVector3( Factor.x, Factor.y, Factor.z ), btVector3( CenterBody.x, CenterBody.y, CenterBody.z ) );

		break;

	case le::Body3D::Impulse:

		if ( CenterBody == Vector3f( ) )
			Body->applyCentralImpulse( btVector3( Factor.x, Factor.y, Factor.z ) );
		else
			Body->applyImpulse( btVector3( Factor.x, Factor.y, Factor.z ), btVector3( CenterBody.x, CenterBody.y, CenterBody.z ) );

		break;
	}

	Body->activate( true );
}

//-------------------------------------------------------------------------//

void le::Body3D::SetPosition( Vector3f Position )
{
	Body->clearForces();
	Transform.setOrigin( btVector3( Position.x, Position.y, Position.z ) );

	Body->setWorldTransform( Transform );
	MotionState->setWorldTransform( Transform );
	Body->activate( true );
}

//-------------------------------------------------------------------------//

void le::Body3D::SetRotation( Vector3f Rotation )
{
	Body->getMotionState()->getWorldTransform( Transform );
	Transform.setRotation( btQuaternion( Rotation.x, Rotation.y, Rotation.z, 1 ) );
	Body->getMotionState()->setWorldTransform( Transform );
}

//-------------------------------------------------------------------------//

void le::Body3D::SetFreezeMoving( bool FreezeX, bool FreezeY, bool FreezeZ )
{
	btVector3 FreezeAxis( 0, 0, 0 );

	if ( !FreezeX ) FreezeAxis.setX( 1 );
	if ( !FreezeY ) FreezeAxis.setY( 1 );
	if ( !FreezeZ ) FreezeAxis.setZ( 1 );

	Body->setLinearFactor( FreezeAxis );
}

//-------------------------------------------------------------------------//

void le::Body3D::SetFreezeRotation( bool FreezeX, bool FreezeY, bool FreezeZ )
{
	btVector3 FreezeAxis( 0, 0, 0 );

	if ( !FreezeX ) FreezeAxis.setX( 1 );
	if ( !FreezeY ) FreezeAxis.setY( 1 );
	if ( !FreezeZ ) FreezeAxis.setZ( 1 );

	Body->setAngularFactor( FreezeAxis );
}

//-------------------------------------------------------------------------//

Vector3f le::Body3D::GetPosition()
{
	Body->getMotionState()->getWorldTransform( Transform );
	btVector3 Position = Transform.getOrigin();

	return Vector3f( Position.getX(), Position.getY(), Position.getZ() );
}

//-------------------------------------------------------------------------//

glm::quat le::Body3D::GetRotation()
{
	Body->getMotionState()->getWorldTransform( Transform );
	btQuaternion Rotation = Transform.getRotation();

	return glm::quat( Rotation.w(), Rotation.x(), Rotation.y(), Rotation.z() );
}

//-------------------------------------------------------------------------//

btRigidBody* le::Body3D::GetBulletBody()
{
	return Body;
}

//-------------------------------------------------------------------------//

btTriangleIndexVertexArray* le::Body3D::GetIndexVertexArrays()
{
	return IndexVertexArrays;
}

//-------------------------------------------------------------------------//

le::Body3D::~Body3D()
{
	Physic3D->DestroyBody( Body );

	delete MotionState;
	delete Shape;	

	if ( IndexVertexArrays != NULL )
		delete IndexVertexArrays;
}

//-------------------------------------------------------------------------//