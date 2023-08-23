#include "Box.h"

using namespace Box;
using namespace Constant;

float Box::Variables::L = 10;
float Box::Variables::V = L * L * L;

double BoundaryConditions::Periodic::Image( double x )
{
    if ( x >= 0 && x < L )
        return x;
    else
    {
        if ( x < 0 )
        {
            do
            {
                x += L;
            } while ( x < 0 );
            return x;
        }
        else
        {
            do
            {
                x -= L;
            } while ( x > L );
            return x;
        }
    }
}

double BoundaryConditions::Periodic::Distance_sq( double x, double y )
{
    double d = x - y;
    if ( d > L_H )
        d = d - L;
    else if ( d < -L_H )
        d = d + L;
    return d * d;
}

double BoundaryConditions::Periodic::Distance_sq( Point & a, Point & b )
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return dx * dx + dy * dy + dz * dz;
}

double BoundaryConditions::Periodic::Distance_sq( Vector & a, Vector & b )
{
    double dx = a.m_x - b.m_x;
    double dy = a.m_y - b.m_y;
    double dz = a.m_z - b.m_z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return dx * dx + dy * dy + dz * dz;
}

double BoundaryConditions::Periodic::Distance( Vector & a, Vector & b )
{
    double dx = a.m_x - b.m_x;
    double dy = a.m_y - b.m_y;
    double dz = a.m_z - b.m_z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return sqrt( dx * dx + dy * dy + dz * dz );
}

Vector BoundaryConditions::Periodic::Relative( Vector a, Vector b ) // returns periodic Vector (a-b)
{
    double x = a.m_x - b.m_x;
    double y = a.m_y - b.m_y;
    double z = a.m_z - b.m_z;
    if ( x > L_H )
        while ( x > L_H )
            x -= L;
    else if ( x < -L_H ) 
        while ( x < -L_H )
            x += L;
    if ( y > L_H )
        while ( y > L_H )
            y -= L;
    else if ( y < -L_H )
        while ( y < -L_H )
            y += L;
    if ( z > L_H )
        while ( z > L_H )
            z -= L;
    else if ( z < -L_H )
        while ( z < -L_H )
            z += L;
    return Vector{ x, y, z };
}

Sector::Sector() : 
    x(0),
    y(0),
    z(0)
{
}

Sector::~Sector()
{
}

std::string Sector::toString()
{
    return "(" + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")";
}

void Sector::MakeNeighbors()
{
    // this->neighbor_list.push_back(Find(x, y, z));                     // 0 0 0

    this->neighbor_list.push_back(Find(x, y, z + 1));         // 0 0 1
    this->neighbor_list.push_back(Find(x, y + 1, z));         // 0 1 0
    this->neighbor_list.push_back(Find(x, y + 1, z + 1));     // 0 1 1
    this->neighbor_list.push_back(Find(x, y + 1, z - 1));     // 0 1 -1
    this->neighbor_list.push_back(Find(x + 1, y, z));         // 1 0 0
    this->neighbor_list.push_back(Find(x + 1, y, z + 1));     // 1 0 1
    this->neighbor_list.push_back(Find(x + 1, y, z - 1));     // 1 0 -1
    this->neighbor_list.push_back(Find(x + 1, y + 1, z));     // 1 1 0
    this->neighbor_list.push_back(Find(x + 1, y - 1, z));     // 1 -1 0
    this->neighbor_list.push_back(Find(x + 1, y + 1, z + 1)); // 1 1 1
    this->neighbor_list.push_back(Find(x + 1, y + 1, z - 1)); // 1 1 -1
    this->neighbor_list.push_back(Find(x + 1, y - 1, z - 1)); // 1 -1 -1
    this->neighbor_list.push_back(Find(x + 1, y - 1, z + 1)); // 1 -1 1
    //---------------------------OPPOSITE-----------------------------//
    // this->neighbor_list.push_back(Find(x, y, z - 1));                // 0 0 -1
    // this->neighbor_list.push_back(Find(x, y - 1, z));                // 0 -1 0
    // this->neighbor_list.push_back(Find(x, y - 1, z - 1));            // 0 -1 -1
    // this->neighbor_list.push_back(Find(x, y - 1, z + 1));            // 0 -1 1
    // this->neighbor_list.push_back(Find(x - 1, y, z));                // -1 0 0
    // this->neighbor_list.push_back(Find(x - 1, y, z - 1));            // -1 0 -1
    // this->neighbor_list.push_back(Find(x - 1, y, z + 1));            // -1 0 1
    // this->neighbor_list.push_back(Find(x - 1, y - 1, z));            // -1 -1 0
    // this->neighbor_list.push_back(Find(x - 1, y + 1, z));            // -1 1 0
    // this->neighbor_list.push_back(Find(x - 1, y - 1, z - 1));        // -1 -1 -1
    // this->neighbor_list.push_back(Find(x - 1, y - 1, z + 1));        // -1 -1 1
    // this->neighbor_list.push_back(Find(x - 1, y + 1, z + 1));        // -1 1 1
    // this->neighbor_list.push_back(Find(x - 1, y + 1, z - 1));        // -1 1 -1
}

int Sector::Find( int x, int y, int z )
{
    while ( x < 0 )
        x += Sector::_S_;
    x = x % Sector::_S_;
    while ( y < 0 )
        y += Sector::_S_;
    y = y % Sector::_S_;
    while ( z < 0 )
        z += Sector::_S_;
    z = z % Sector::_S_;
    return ( z + y * _S_ + x * _S_ * _S_ );
}
