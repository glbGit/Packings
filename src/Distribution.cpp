#include "Distribution.h"

using namespace Constant;
using namespace Packings;

Distribution::Info Distribution::GetDistributionInfo()
{
    Info info{};
    double ratio = Ratio;
    double exp_sigma_D = exp( lgamma(1. + (double) D / 2.) ) * phi_0 * pow( L, D ) / N / pow( 0.25 * Pi, (double) D / 2.);

    if ( phi_0 < epsilon )
    {
        info.type = Type::Uniform;
        info.sigma_min = info.sigma_max = 0;
        return info;
    }

    if ( ratio < 0 )  Exit( INVALID_PARAMETER, { "Diameters ratio must be positive." } );
    if ( ratio > 1 ) ratio = 1. / ratio;

    if ( fabs( ratio - 1 ) < epsilon )
    {
        info.type = Type::Uniform;
        info.sigma_min = info.sigma_max = pow( exp_sigma_D, 1. / D );
        return info;
    }

    switch ( DType )
    {
    case Uniform:
    {
        info.sigma_max = pow( ( ( D + 1 ) * ( 1 - ratio ) / ( 1 - pow( ratio, D + 1 ) ) ) * exp_sigma_D, 1. / D );
        info.sigma_min = ratio * info.sigma_max;
        info.type = Uniform;
        return info;
    }
    case Binary:
    {
        info.type = Binary;
        info.param = DParameters;
        if ( info.param[0] < 0 || info.param[0] > 1 ) Exit( INVALID_PARAMETER, { "Binary distribution requires param in range [0:1]. " } );

        info.sigma_max = pow( ( 1. / (info.param[0] + ( 1 - info.param[0] ) * pow( ratio, D ) ) ) * exp_sigma_D, 1. / D );
        info.sigma_min = ratio * info.sigma_max;
        return info;
    }
    case PowerLaw:
    {
        info.type = PowerLaw;
        info.param = DParameters ;
        int n = (int) info.param[0];
        if ( n == 0 )
        {
            info.sigma_max = pow( ( ( D + 1 ) * ( 1 - ratio ) / ( 1 - pow(ratio, D + 1 ) ) ) * exp_sigma_D, 1. / D );
            info.sigma_min = ratio * info.sigma_max;
            return info;
        }
        else if ( n == (int) D )
        {
            if ( D == 1 )
            {
                info.sigma_max = (1. / ( ( 1 - ratio ) * log(ratio) ) ) * exp_sigma_D;
                info.sigma_min = ratio * info.sigma_max;
            }
            else
            {
                info.sigma_max = pow( ( ( 1 - pow( ratio, 1 - D ) ) / ( ( 1. - D ) * ( 1 - ratio ) ) ) * exp_sigma_D, 1. / D );
                info.sigma_min = ratio * info.sigma_max;
            }
        }
        else if ( n == (int) D + 1 )
        {
            info.sigma_max = pow( ( 1 - pow( ratio, -D ) ) / ( (double) D * log(ratio) ) * exp_sigma_D, 1. / D );
            info.sigma_min = ratio * info.sigma_max;
        }
        else
        {
            if ( n == 1 )
            {
                info.sigma_max = pow( ( (double) D / ( ( 1 - pow( ratio, D ) ) * log(1. / ratio) ) ) * exp_sigma_D, 1. / D );
                info.sigma_min = ratio * info.sigma_max;
            }
            else
            {
                info.sigma_max = pow( ( (double) ( 1 - n + D ) / ( 1 - n ) * ( 1 - pow( ratio, 1 - n ) ) / (1 - pow( ratio, 1 - n + D ) ) ) * exp_sigma_D, 1. / D );
                info.sigma_min = ratio * info.sigma_max;
            }
        }
        return info;
    }
    case LogNormal:
    {
        info.type = LogNormal;
        info.param = DParameters;
        if (info.param[0] < 0) Exit( INVALID_PARAMETER, { "Lognormal distribution requires param to be positive. " } );

        double mu = info.param[0];
        /*** TEMP ***/
        double sigma = 0.05;
        info.sigma_max = 0.1 * mu / sigma;
        /*** TEMP ***/
        info.sigma_min = ratio * info.sigma_max;
        return info;
    }

    default:
        Exit( UNDEFINED_DISTRIBUTION, { "Undefined distribution. " } );
    }
    return info;
}

double Distribution::RandomDiameter( const Info & d )
{
    double diameter = 0;
    double a = d.sigma_min, b = d.sigma_max;
    if ( b - a < epsilon )
        return a;
    switch ( d.type )
    {
    case Uniform:
        diameter = u_rand_in( a, b );
        break;
    case Binary:
        diameter = ( u_rand < d.param[0] ) ? b : a;
        break;
    case PowerLaw:
    {
        double A;
        int n = (int) d.param[0];
        if ( n == 1 )
            A = log(d.sigma_min / d.sigma_max);
        else
            A = ( 1 - n ) / ( pow( d.sigma_max, 1 - n ) - pow( d.sigma_min, 1 - n ) );
        double _a = A / pow( b, n );
        double _b = A / pow( a, n );
        double rnd = u_rand_in( _a, _b );
        double x = pow( rnd / A, -1. / n );
        diameter = x;
        break;
    }
    case LogNormal:
    {
        double mu = d.param[0];
        double sigma = d.param[1];
        double sigma_z = sqrt( log(sigma * sigma / mu / mu + 1 ) );
        double mu_z = log(mu) - 0.5 * sigma_z * sigma_z;
        double u1, u2, mag, z;
        do
        {
            do
            {
                u1 = u_rand;
            } while ( u1 <= epsilon );
        u2 = u_rand;

        mag = sigma_z * sqrt( -2.0 * log(u1) );
        z = mag * cos(2 * Pi * u2) + mu_z;
        } while ( z < log(a) || z > log(b) );
        diameter = exp(z);
        break;
    }

    default:
        break;
    }
    return diameter;
}

double Distribution::Moment( const double * val, int n )
{
    double sum = 0;
    for ( int i = 0; i < N; i++ ) 
        sum += pow( val[i], n );
    return sum / N;
}

