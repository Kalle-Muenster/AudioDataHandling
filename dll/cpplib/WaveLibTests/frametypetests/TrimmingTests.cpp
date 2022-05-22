/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      TrimmingTests.cpp                          ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "TrimmingTests.hpp"
#undef COMMANDLINER_ESCENTIALS_DEFINED
#include <.stringPool.h>
#include "../resources/ResourceIterator.h"



Wave::Test::TrimmingTests::TrimmingTests( const char* resourcename, uint expectedTrimmedLength, uint divergenceTollerance ) : PerFrameType()
{ pool_scope

    PrintLog("\n\n################ Trimming Tests #####################\n");
    pool_set("Preparing for trimming tests,... Loading ");
    const AbstractAudioFileHeader* fileData = (const AbstractAudioFileHeader*)TestData_resourceByName(resourcename)->data();
    HEADER_CHUNK_TYPE fileType = fileData->GetFileFormat();
    switch (fileType) {
        case HEADER_CHUNK_TYPE::WavFormat: pool_set("Wav-File"); break;
        case HEADER_CHUNK_TYPE::SndFormat: pool_set("Snd-File"); break;
        case HEADER_CHUNK_TYPE::P7mFormat: pool_set("Pam-File"); break;
    }
    pool_setf(" resource: %s\n", resourcename );
    PrintLog( pool_merge(3) );
    buffer = Audio( fileData );
    expect = expectedTrimmedLength;
    tollerate = (int)divergenceTollerance;
    pool_setfi( " ...wave data got length of %i frames", buffer.getLength() );
    int millisecs = (int)(buffer.getDuration() * 1000);
    pool_setfi( " at duration of %i miliseconds\n", millisecs );
    PrintLog( pool_merge(2) );
}

int
Wave::Test::TrimmingTests::runTestCase( const AudioFrameType* frametype )
{
    pool_scope
    PrintLog("\n<------------------------------------------------->\n");
    char tag = frametype->FormatTag();
    tag = tag == PCMs ? 's' : PCMf ? 'f' : 'i';
    uint tc = frametype->Code();
    tc = tc|(48000<<16);

    Audio casebuffer = buffer.converted( tc ).outscope();
    PrintLog( TypeCodeToString(tc) );
    PrintLog( "Audio:------------\n" );
    pool_setfi("geLength(): %i frames\n", casebuffer.getLength());
    pool_setfi("getSize(): %i bytes\n", casebuffer.getDataSize());
    pool_setFl("getDuration(): %f seconds\n", casebuffer.getDuration());
    PrintLog( pool_merge(3) );
    PrintLog("------------------\n");
    PrintLog( FormatToString( casebuffer.format ) );
    //////////////////////////////////////////////////////////////////////
    casebuffer.trim();                                          // TEST //
    //////////////////////////////////////////////////////////////////////
    casebuffer.takeOwnership( true );
    int trimmed = (int)(casebuffer.getDuration() * 1000);
    pool_setfi("duration of the trimmed buffer: %ims ", trimmed);
    pool_setfi(", expected %ims ", expect);
    pool_setfi("(including +/- %ims tollerance)  \n", tollerate);
    PrintLog( pool_merge(3) );
    PrintLog("Will store resulting buffer to file: ");
    char* fout = GetFileNameForFrameType( "trimmed", frametype );
    PrintLog(fout);
    PrintLog("\n");
    writer.Save( casebuffer, fout );
    trimmed -= expect;
    ///////////////////////////////////////////////////////////////////////
    bool fail = (trimmed > tollerate || trimmed < -tollerate); // Result //
    ///////////////////////////////////////////////////////////////////////
    if (fail) { PrintLog("\nFAIL\n\n"); return 1;
    } return 0;
}


