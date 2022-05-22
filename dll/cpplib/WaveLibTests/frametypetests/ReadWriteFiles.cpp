/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ReadWriteFiles.cpp                         ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "ReadWriteFiles.hpp"
#include "../ScopedPointer.hpp"
#include <.stringPool.h>

Wave::Test::ReadWriteFiles::ReadWriteFiles(void)
    : PerFrameType(), reader(), writer()
{

}

Wave::Test::ReadWriteFiles::~ReadWriteFiles(void)
{
    cases.Clear();
    reader.Close();
    writer.Close();
}

byte compareFrameToSampleValues( const Wave::AudioFrameType* frameType, Wave::IAudioFrame* frameData, double* testData, int CHt = 1 )
{
    const int CH = frameType->Channels();
    const int CC = CHt;
    for ( int ch = 0; ch < CH; ++ch ) {
        CHt = ch < CC ? ch : (CC-1);
        switch ( frameType->BitDepth() ) {
        case 8:  if (*(Wave::s8*) frameData->GetChannel(ch) != Wave::s8( testData[CHt])) return ch + 1; else break;
        case 16: if (*(Wave::s16*)frameData->GetChannel(ch) != Wave::s16(testData[CHt])) return ch + 1; else break;
        case 24: if (*(Wave::s24*)frameData->GetChannel(ch) != Wave::s24(testData[CHt])) return ch + 1; else break;
        case 32: if (*(Wave::f32*)frameData->GetChannel(ch) != Wave::f32(testData[CHt])) return ch + 1; else break;
        case 64: if (*(Wave::f64*)frameData->GetChannel(ch) != Wave::f64(testData[CHt])) return ch + 1; else break;
        }
    } return 0;
}

byte compareAudioFrames( const Wave::AudioFrameType* frameType, Wave::IAudioFrame* frameData, Wave::IAudioFrame* testData )
{
    const int CH = frameType->Channels();
    if (testData->Channels() != CH) return 'C';
    if (testData->BitDepth() != frameType->BitDepth()) return 'B';
    if (testData->Signedty() != frameData->Signedty()) return 'T';
    for (int ch = 0; ch < CH; ++ch) {
        switch (frameType->BitDepth()) {
        case 8:  if ( *(Wave::s8*) frameData->GetChannel(ch) !=  *(Wave::s8*)testData->Channel[ch] ) return ch + 1; else break;
        case 16: if (*(Wave::s16*) frameData->GetChannel(ch) != *(Wave::s16*)testData->Channel[ch] ) return ch + 1; else break;
        case 24: if (*(Wave::s24*) frameData->GetChannel(ch) != *(Wave::s24*)testData->Channel[ch] ) return ch + 1; else break;
        case 32: if (*(Wave::f32*) frameData->GetChannel(ch) != *(Wave::f32*)testData->Channel[ch] ) return ch + 1; else break;
        case 64: if (*(Wave::f64*) frameData->GetChannel(ch) != *(Wave::f64*)testData->Channel[ch] ) return ch + 1; else break;
        }
    } return 0;
}

int
Wave::Test::ReadWriteFiles::runTestCase( const AudioFrameType* frametype )
{ pool_scope
    int errors = 0;
    PrintLog( "\n=====================================================\n" );
    PrintLog( "loading file: " );
    char* caseFile = GetFileNameForFrameType( "trimmed", frametype );
    PrintLog( caseFile );
    PrintLog( "\n" );
    reader.Open( caseFile );
    if ( !reader.isValid() ) {
        errors++;
        PrintLog("WaveFileReader: opening files FAIL\n");
        PrintLog("..skipping further tests for this frametype\n");
        return errors;
    }
    Format format;
    format = *(reader.GetFormat());
    if( format.BitsPerSample != frametype->BitDepth() ) {
        errors++;
        PrintLog( pool_setfi("format.BitsPerSample NOT is %i as expected\n", frametype->BitDepth()) );
    }
    if( format.NumChannels != frametype->Channels() ) {
        errors++;
        PrintLog( pool_setfi("format.NumChannnels NOT is %i as expected\n", frametype->Channels()) );
    }
    if (format.SampleRate != 48000) {
        errors++;
        PrintLog( "format.SampleRate NOT is 48kHz as expected\n" );
    }

    caseFile = GetFileNameForFrameType("writing", frametype);
    writer.Open( caseFile );
    PrintLog( "Audio = WaveFileReader::Read()\n" );
    Audio test( reader.Read().outscope() );
    test.takeOwnership();
    PrintLog( "WaveFileWriter::SetFormat( Audio::format )\n" );
    writer.SetFormat( test.format );
    PrintLog( "WaveFileWriter::Save( Audio, \"");
    PrintLog( caseFile );
    PrintLog( "\" )\n" );
    writer.Save( test, caseFile );

    PrintLog( "WaveFileReader::Seek(0)\n" );
    reader.Seek(0);
    PrintLog( "WaveFileReader::Seek(1000)\n" );
    reader.Seek(1000);
    PrintLog( "WaveFileReader::Read(100) ..frames\n" );
    test = reader.Read(100).outscope();
    test.takeOwnership();
    caseFile = GetFileNameForFrameType("ReadFrames_100", frametype);
    PrintLog( "WaveFileWriter::Save( Audio, \"" );
    PrintLog( caseFile );
    PrintLog( "\" )\n" );
    writer.Save( test, caseFile );


    PrintLog( "IAudioFrame reader.ReadFrame()\n" );
    ScopedPointer<IAudioFrame> frameA = CreateAudioFrame( frametype->Code() );
    ScopedPointer<IAudioFrame> frameB = CreateAudioFrame( frametype->Code() );

    frameA->Set( reader.ReadFrame() );
    PrintLog( "IAudioFrame::Set( WaveFileReader::ReadFrame() )\n" );
    PrintLog( FrameToString( frameA ) );
    
    double nix[1] = { 0 };
    if( !compareFrameToSampleValues( frametype, frameA, &nix[0] ) ) {
        PrintLog( "silent frame found at position 100 - FAIL\n" );
        ++errors;
    } else {
        PrintLog( FrameToString( frameA ) );
        PrintLog( "\n" );
        PrintLog( "IAudioFrame::GetFormat(48000):\n" );
        PrintLog( FormatToString( frameA->GetFormat(48000) ) );
        PrintLog( "\n" );
    }
    
    PrintLog( "IAudioFrame::Set( WaveFileReader::ReadFrame() )\n" );
    frameB->Set( reader.ReadFrame() );
    if( !compareAudioFrames( frametype, frameA, frameB ) ) {
        PrintLog( "read frame is equal to its ancestor: FAIL\n" );
        ++errors;
    }
    reader.Seek(0);
    uint frames = reader.GetLength();
    PrintLog( "Whole file 'framewise' by: WaveFileWriter::WriteFrame( WaveFileReader::ReadFrame() )\n" );
    writer.Flush();
    writer.Close();
    writer.Open( GetFileNameForFrameType( "framewise", frametype ) );
    PrintLog("file opened for writing!\n");
    while( writer.WriteFrame( reader.ReadFrame() ) );
    PrintLog("all data streamed!\n");
    writer.Flush();
    PrintLog("writer flushed!\n");
    writer.Close();
    PrintLog("header written and file closed!\n");
    return errors;
}

