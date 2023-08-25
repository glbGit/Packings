#pragma once 

#include "pch.h"
#include "Utils.h"
#include "Constant.h"

struct Direction /* Angles are defined according to polar coordinates: theta[0,Pi], phi(-Pi,Pi] */
{
    double theta;
    double phi;
};

struct Point
{
    double x;
    double y; 
    double z;
};

class Vector
{
public:
    Vector();
    Vector( double );
    Vector( double, double, double );
    Vector( double, Direction );
    
    Vector operator+( Vector );
    Vector operator-( Vector );
    Vector operator*( double );
    Vector operator^( Vector );
    Vector operator/( double );
    void   operator=( double );
    bool   operator==( Vector );
    void   operator+=( Vector & );
    double operator*( Vector );
    friend Vector operator*( double, Vector );
    friend Vector operator-( Vector );

    std::string toString();
    void SetX( double x ) { m_x = x; }
    void SetY( double y ) { m_y = y; }
    void SetZ( double z ) { m_z = z; }
    void Set( double x, double y, double z ) { m_x = x; m_y = y; m_z = z; }
    void setRandomComponents( double, double );
    void rotate( Direction );

    double      getMod() { return sqrt( m_x * m_x + m_y * m_y + m_z * m_z ); }
    double      SqrtMod() { return m_x * m_x + m_y * m_y + m_z * m_z; }
    Direction   getDirection();
    
    static Vector   Unit( Vector );
    static Vector   RandomVector( double, double );
    static double   Angle( Vector, Vector );
    
    double              m_x;
    double              m_y;
    double              m_z;
};






