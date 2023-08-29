#pragma once 

#include "pch.h"
#include "Utils.h"
#include "Constant.h"

struct Direction /* Angles are defined according to polar coordinates: theta[0,Pi], phi(-Pi,Pi] */
{
    double theta;
    double phi;
};

template <int Dim>
struct Point
{
    double x[Dim];
};

template <class Ty, int Dim>
class Vector
{
public:
    Vector();
    Vector( Ty Value );
    Vector( Ty Mag, Direction Dir );
    Vector( Vector<Ty, Dim> & Vec );

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

    std::string ToString();
    void Unit();
    void SetRandomComponents( Ty Min, Ty Max );

    double      Mag();
    double      MagSq();
    
    static double   Angle( Vector &, Vector & );
    
    Ty              x[ Dim ];
};






