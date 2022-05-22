// Include the wavelib header
#include <wavelib.hpp>

// Include utils
#undef COMMANDLINER_ESCENTIALS_DEFINED
#include <.stringPool.h>
POOL* DEFAULT_POOLBOTTOM;
#include <.commandLiner.h>

// Include Test Cases
#include "frametypetests/FrameTypesTests.hpp"
#include "frametypetests/TrimmingTests.hpp"
#include "frametypetests/PamHeaderTests.hpp"
#include "frametypetests/PanoramaTests.hpp"
#include "frametypetests/BufferConversionTests.hpp"
#include "frametypetests/ReadWriteFiles.hpp"
#include "frametypetests/SplittingTests.hpp"
#include "frametypetests/ByFrameApi.hpp"

// Include Test Data
#include "resources/ResourceIterator.h"

using namespace Wave;
using namespace Wave::Test;

int main(int argc, char** argv)
{
    Pool = pool_InitializeCycle();
    char* file = pool_set( argv[0] );
    while (*++file != '\0');
    while (*--file != '.');
    *(unsigned*)file = 1735355438u;
    FILE* log;
    FOPEN_FUNC( log, pool_get(), "w+" );
    AbstractTestCase::SetLogFile( log );

    const char* versione = pool_setf( "\n########## Test WaveLib v.%s ##########\n", GetVersionString() );
    printf( versione );
    fwrite( versione, 1, strlen(versione), log );

    int failures = 0;
    FrameTypes* frametypetests = new FrameTypes();
    failures += frametypetests->Run();
    delete frametypetests;

    ByFrameApi* frameconversion = new ByFrameApi();
    failures += frameconversion->Run();
    delete frameconversion;

    TrimmingTests* trimmotests = new TrimmingTests( "trimming216.wav", 1000, 15 );
    failures += trimmotests->Run();
    delete trimmotests;

    PamHeaderTests* pamformattests = new PamHeaderTests();
    failures += pamformattests->Run();
    delete pamformattests;

    ReadWriteFiles* fileio = new ReadWriteFiles();
    failures += fileio->Run();
    delete fileio;

    PanoramaTests* pannotests = new PanoramaTests();
    failures += pannotests->Run();
    delete pannotests;

/*
    BufferConversionTests* conversion = new BufferConversionTests();
    failures += conversion->Run();
    delete conversion;

    SplittingTests* splitting = new SplittingTests();
    failures += splitting->Run();
    delete splitting;
*/


    if (failures)
        pool_setfi("TEST FAILED: %i FAILURES\n", failures);
    else
        pool_set("TEST PASSED\n");

    fprintf(log,"%s",pool_get());
    printf("%s", pool_get());
    fflush(log);
    fclose(log);
    pool_freeAllCycles();

}
