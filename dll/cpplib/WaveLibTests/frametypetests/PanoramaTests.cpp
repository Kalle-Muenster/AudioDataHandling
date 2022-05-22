/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      PanoramaTests.cpp                          ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "PanoramaTests.hpp"
#undef COMMANDLINER_ESCENTIALS_DEFINED
#include <.stringPool.h>


Wave::Test::PanoramaTests::PanoramaTests(void)
{
    AddTestCasesPerTypeCase( &cases );
}

Wave::Test::PanoramaTests::~PanoramaTests(void)
{
    cases.Clear();
}



const char* Wave::Test::PanoramaTests::PanoramaToString(Panorama* panorama)
{ pool_scope
    char buffer[16];
    sprintf(&buffer[0], "%f", panorama->Stereo);
    pool_setf( "Left/Right axis: %s, ", &buffer[0] );
    sprintf(&buffer[0], "%f", panorama->Frontal);
    pool_setf( "Front/Rear axis: %s\n", &buffer[0] );
    pool_set( PictogramFromPan( *panorama ).c_str() );
    return pool_merge(3);
}

int Wave::Test::PanoramaTests::runTestCase(Panorama::Modus mode)
{
    PrintLog("|------------------------------------------------------------|\n");
    PrintLog("Testing Panorama mixer type:\n  pan1(0.8,0.5), pan2(0.1,0.2)\n");
    Panorama pan1 = Panorama(0.8f,0.5f);
    Panorama pan2 = Panorama(0.1f,0.2f);
    PrintLog("    pan1 = ");
    PrintLog( PanoramaToString(&pan1) );
    PrintLog("    pan2 = ");
    PrintLog( PanoramaToString(&pan2) );
    Panorama pan = pan1 * 2;
    PrintLog("    pan1 * 2 = ");
    PrintLog( PanoramaToString(&pan) );
    pan = pan2 / 2;
    PrintLog("    pan2 / 2 = ");
    PrintLog( PanoramaToString(&pan) );
    pan = pan1 + pan2;
    PrintLog("    pan1 + pan2 = ");
    PrintLog( PanoramaToString(&pan) );
    pan = pan1 - pan2;
    PrintLog("    pan1 - pan2 = ");
    PrintLog( PanoramaToString(&pan) );
    pan = pan1 * pan2;
    PrintLog("    pan1 * pan2 = ");
    PrintLog( PanoramaToString(&pan) );
    pan = pan1 / pan2;
    PrintLog("    pan1 / pan2 = ");
    PrintLog( PanoramaToString(&pan) );

    pan = Panorama::NEUTRAL;
    PrintLog("Neutral Panorama will have Side values assigned...");
    pan.Left[mode] = 0.8f;
    PrintLog("assigned 0.8 to the left speakers:");
    PrintLog( PanoramaToString(&pan) );
    pan.Rear[mode] = 0.1f;
    PrintLog("assigned 0.1 to the rear speakers:");
    PrintLog( PanoramaToString(&pan) );
    pan.Right[mode] = 0.75f;
    PrintLog("assigned 0.75 to the right speakers:");
    PrintLog( PanoramaToString(&pan) );
    pan.Front[mode] = 0.33f;
    PrintLog("assigned 0.33 to the front speakers:");
    PrintLog( PanoramaToString(&pan) );
    return 0;
}

void
Wave::Test::PanoramaTests::AddTestCasesPerTypeCase(TypeCases * testcases)
{
    testcases->setSlotNuller(Panorama::Modi);
    testcases->Add(Panorama::Subractive);
    testcases->Add(Panorama::Absolute);
}
