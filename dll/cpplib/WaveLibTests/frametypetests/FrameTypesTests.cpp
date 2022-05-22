/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      FrameTypesTests.cpp                        ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "FrameTypesTests.hpp"
#include <./WaveLib.inl/src/WaveAudio.hpp>

#undef COMMANDLINER_ESCENTIALS_DEFINED
#include <.stringPool.h>


Wave::Test::FrameTypes::FrameTypes(void)
    : PerFrameType()
{
    char d[] = { 1,2,3,10,20,30,40,50 };
    for (int i = 0; i < 8; ++i)
        datas[i] = d[i];
}

int Wave::Test::FrameTypes::runTestCase( const AudioFrameType* frametype )
{ pool_scope

    steps = 0; 
    bool failed = false;
    WAV_PCM_TYPE_ID tag = frametype->FormatTag();
    IAudioFrame* testling = Wave::CreateAudioFrame( frametype->Code() );
    pool_set("============================================\n");
    pool_setf("= Audio FrameType : AuPCM%s",
        tag == WAV_PCM_TYPE_ID::PCMs ? "s"
        : tag == WAV_PCM_TYPE_ID::PCMf ? "f"
        : "i");
    pool_setfi("%i<", frametype->BitDepth());
    pool_setfi("%i>\n", frametype->Channels());
    pool_set("============================================\n");
    PrintLog( pool_merge(5) );
    ++steps;
    Format fmt = testling->GetFormat(44100);
    Format chk = CreateWaveFormat( frametype->Code(), 44100 );
    if ((fmt == chk) == FormatMatch::CONVERSION) {
        PrintLog("FAIL: CreateWaveFormat(typecode,samplerate) differs from AudioFrame->GetFormat(samplerate)\n    Created: \n");
        PrintLog( FormatToString(chk) );
        PrintLog("    from frametype: \n");
        PrintLog( FormatToString(fmt) );
        failed = true;
    } else {
        PrintLog("PASS: CreateWaveFormat(typecode,samplerate) equals to AudioFrame->GetFormat(samplerate)\n");
    }++steps;
    chk = CreateWaveFormat(44100, frametype->BitDepth(), frametype->Channels(), frametype->FormatTag());
    if ((fmt == chk) == FormatMatch::CONVERSION) {
        PrintLog("FAIL: CreateWaveFormat(samplerate,bitdepth,channels,pcmtag) differs from AudioFrame->GetFormat(samplerate)\n    Created: \n");
        PrintLog( FormatToString(chk) );
        PrintLog("    from frametype: \n");
        PrintLog( FormatToString(fmt) );
        failed = true;
    } else {
        PrintLog("PASS: CreateWaveFormat(samplerate,bitdepth,channels,pcmtag) equals to AudioFrame->GetFormat(samplerate)\n");
    } ++steps;

    uint typecode = AUDIOFRAME_CODE(fmt.BitsPerSample, fmt.NumChannels, fmt.PCMFormatTag);
    PrintLog( TypeCodeToString( typecode ) );
    FRAMETYPE_SWITCH( AUDIOFRAME_CODE( fmt.BitsPerSample, fmt.NumChannels, fmt.PCMFormatTag ),
        pool_setfi( "Logical framesize (by BlockAlign): %i\n", testling->ByteSize());
        pool_setfi( "Concrete framesize (by sizeof(TYPE)): %i\n", sizeof(CASE_TYPE));
        pool_setfi( "Concrete framesize (sizeof(FrameInst)): %i\n", sizeof(*(CASE_TYPE*)testling));
        pool_setfi( "Concrete samplesize (by sizeof(TYPE)): %i\n", sizeof(CASE_TYPE::TY));
        )
    PrintLog( pool_merge(4) );
    PrintLog( FormatToString( fmt ) );
    int bitnes = testling->BitDepth();
    int channels = testling->Channels();
    
    
    try { 
        pool_setCheckpoint();
        for (int i = 0; i < testling->Channels(); i++) {
            pool_setfi("    channel %i:\nbefore assigning data: ", i);
            switch (bitnes) {
            case 8:  { pool_setfi("%i\n", *(s8*)testling->GetChannel(i)); s8 val = (s8)datas[i]; testling->SetChannel(i,&val); pool_setfi("after assigning data: %i\n",*(s8*)testling->GetChannel(i)); break; }
            case 16: { pool_setfi("%i\n", *(s16*)testling->GetChannel(i)); s16 val = (s16)datas[i]; testling->SetChannel(i,&val); pool_setfi("after assigning data: %i\n", *(s16*)testling->GetChannel(i)); break; }
            case 24: { pool_setfi("%i\n", (*(s24*)testling->GetChannel(i)).arithmetic_cast()); s24 val = datas[i]; testling->SetChannel(i,&val); pool_setfi("after assigning data: %i\n", (*(s24*)testling->GetChannel(i)).arithmetic_cast()); break; }
            case 32: { pool_setFl("%f\n", *(f32*)testling->GetChannel(i)); f32 val = (f32)datas[i] / 127.0f; testling->SetChannel(i,&val); pool_setFl("after assigning data: %f\n", *(f32*)testling->GetChannel(i)); break; }
            case 64: { pool_setDl("%f\n", *(f64*)testling->GetChannel(i)); f64 val = (f64)datas[i] / 127.0f; testling->SetChannel(i,&val); pool_setDl("after assigning data: %f\n", *(f64*)testling->GetChannel(i)); break; }
            }
        } ++steps;
        PrintLog( pool_collectCheckpoint() );

        Panorama pan = Panorama(0.8f, 0.5f);
        pool_setCheckpoint();
        pool_set("will mix mono sample by Panorama(");
        pool_setFl(pan.get(Panorama::LeftToRight));
        pool_set(",");
        pool_setFl(pan.get(Panorama::FrontToRear));
        pool_set(") into the frame...\nsample: ");
        switch (bitnes) {
        case 8:  {  s8 val = 33;      pool_setfi("%i",val); testling->Mix(&val, pan); break; }
        case 16: { s16 val = 10000;   pool_setfi("%i",val); testling->Mix(&val, pan); break; }
        case 24: { s24 val = 2796000; pool_setfi("%i",val.arithmetic_cast()); testling->Mix(&val, pan); break; }
        case 32: { f32 val = 0.333f;  pool_setFl("%f",val); testling->Mix(&val, pan); break; }
        case 64: { f64 val = 0.333;   pool_setDl("%f",val); testling->Mix(&val, pan); break; }
        } pool_set("\n");
        pool_set( "channels nach dem mix: " );
        PrintLog(pool_collectCheckpoint());
        PrintLog( FrameToString(testling) );
        ++steps;

        PrintLog( "\nApplying amplification of factor 0.5!\nresulting channels: " );
        testling->Amp( 0.5 );
        PrintLog( FrameToString( testling ) );
        ++steps;

        Panorama pan2 = Panorama(0.1f, 0.2f);
        Panorama pan3 = pan + pan2;
        PrintLog( "\nApplying panorama change of Pan(0.8,0.5)+Pan(0.1,0.2)\n" );
        testling->Pan( pan3 );
        pool_setCheckpoint();
        pool_set("channels nach Pan(");
        pool_setFl(pan3.get(Panorama::LeftToRight));
        pool_set(",");
        pool_setFl(pan3.get(Panorama::FrontToRear));
        pool_set(") : ");
        PrintLog(pool_collectCheckpoint());
        PrintLog( FrameToString( testling ) );
        PrintLog("\n");
        ++steps;
    }
    catch (std::exception ex) {
        failed = true;
        pool_setf("FEHLER: %s", ex.what());
        pool_setfi(" at FrameTypesTests.Step %i", steps);
        PrintLog( pool_merge(2) );
    } delete testling;
    return failed ? (7-steps) : 0;
}
