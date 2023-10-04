#include "pch.h"
#include "Packing.h"

using namespace System;
using namespace System::Variables;
using namespace System::Interaction;
using namespace Constant;
using namespace Packings;

double System::Variables::s_min;
double System::Variables::s_max;
double System::Variables::g_min;
double System::Variables::g_max;
Distribution::Type System::Variables::distribution_type;
Potential System::Variables::interaction_type = Potential::Hard;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int main()
{
	srand( 321 );

	/* Entry point */
	Time( On::Begin );
	Packing R;
	R.Make();
	R.PrintSystemInfo( On::Start );

	/* Relax */
	if ( InitialRelaxation )
	{
		interaction_type = Potential::SoftRepulsive;

		/* Relax pre-compression */
		printf( "Relaxing the system before compressing..    " );
		for ( int i = 0; i < 50; i++ )  
		{
			R.Step();
			R.Update( System::State::Relaxation );
		}
		printf( "Completed.\n" );
	}

	/* Compression */
	interaction_type = Potential::Hard;
	R.Compress();

	/* Print */
	R.PrintToFile();
	R.PrintDiameterDistribution();
	R.PrintSystemInfo( On::Exit );

	Time( On::End );
	return 0;
}
