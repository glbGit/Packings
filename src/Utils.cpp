#include "Utils.h"

using namespace Packings;

int Packings::iPow( int b, int e )
{
    int res = 1;
    while ( true )
    {
        if ( e & 1 ) res *= b;
        e >>= 1;
        if ( !e ) break;
        b *= b;
    }
    return res;
}

unsigned long long Packings::Factorial( unsigned long long n )
{
    if ( n > 1 ) 
        return n * Factorial( n - 1 );
    else return 1;
}

/**
* @brief Print date and time together with a message.
* @param 0 Exit message; 
* @param 1(any_integer) Welcome message. 
*/
void Packings::Time( On _time )
{
    /* Time */
    char time_s[26];
    time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
#ifdef _LINUX
	localtime_r( &rawtime, &timeinfo );
    asctime_r( &timeinfo, time_s );
#else
	localtime_s( &timeinfo, &rawtime );
    asctime_s( time_s, &timeinfo );
#endif // _LINUX
    switch ( _time )
    {
    case On::Begin: 
        printf(
            "***********************************************\n"
            "PACKINGS                                       \n"
            "***********************************************\n"
            "Running program...    %s\n", time_s 
        );
        break;

    case On::End: 
        printf(
            "\nExiting program...    %s"
            "***********************************************\n", time_s 
        );
        break;

    default:
        break;
    }

}

/**
* @brief Handle exception and exit.
* @param Code Error identifier;
* @param Message Message to display.
*/
void Packings::Exit( int code, std::vector<const char *> message )
{
    printf( "\nError(%x): ", code );
    for ( auto Block : message ) printf( Block );
    printf("\n");
    exit( code ); 
}