#include "Particle.h"
#include "Box.h"

Particle::Particle() : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position(),
    velocity(),
    diameter(0)
{
}

Particle::Particle( Vector r, double d ) : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position( r.m_x, r.m_y, r.m_z ),
    velocity(),
    diameter(d)
{
}

Particle::Particle( Vector r, Vector v ) : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position( r.m_x, r.m_y, r.m_z ),
    velocity( v.m_x, v.m_y, v.m_z ),
    diameter(0)
{
}

Particle::Particle( int index, Vector r, Vector v, double d ) : 
	id( index ),
	sector( NO_INDEX ),
    position( r.m_x, r.m_y, r.m_z ),
    velocity( v.m_x, v.m_y, v.m_z ),
    diameter(d)
{
}

Particle::~Particle()
{
}

void Particle::move( double t )
{ 
    Vector vt = this->velocity * t;
    Vector NewPosition = this->position + vt;
    NewPosition.SetX( Box::BoundaryConditions::Periodic::Image( NewPosition.m_x ) );
    NewPosition.SetY( Box::BoundaryConditions::Periodic::Image( NewPosition.m_y ) );
    NewPosition.SetZ( Box::BoundaryConditions::Periodic::Image( NewPosition.m_z ) );
    position = NewPosition; 
}

void Particle::move( Vector r )
{ 
    Vector NewPosition = this->position + r;
    NewPosition.SetX( Box::BoundaryConditions::Periodic::Image( NewPosition.m_x ) );
    NewPosition.SetY( Box::BoundaryConditions::Periodic::Image( NewPosition.m_y ) );
    NewPosition.SetZ( Box::BoundaryConditions::Periodic::Image( NewPosition.m_z ) );
    this->position = NewPosition; 
}




