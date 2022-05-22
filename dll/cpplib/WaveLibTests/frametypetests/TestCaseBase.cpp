
#include "TestCaseBase.hpp"

#undef COMMANDLINER_ESCENTIALS_DEFINED
#include <.stringPool.h>



FILE* Wave::Test::AbstractTestCase::log = NULL;

void Wave::Test::AbstractTestCase::SetLogFile( FILE* file ) {
    log = file;
}

Wave::Test::PerFrameType::PerFrameType() 
    : PerTypeTests() {
    AddTestCasesPerTypeCase( &cases );
}

char*
Wave::Test::AbstractTestCase::GetFileNameForFrameType(
    const char* fileName, const Wave::AudioFrameType* typeName,
    const char* extension
) { pool_scope
    
    char tag = typeName->FormatTag();
    tag = tag == PCMs ? 's' : PCMf ? 'f' : 'i';
    pool_setf("%s_", fileName);
    pool_set("AuPCM");
    pool_setc(tag, 1);
    pool_setfi("%ibit", typeName->BitDepth());
    pool_setfi("%ichn.", typeName->Channels());
    if ( extension ) pool_set( extension );
    else pool_set( typeName->BitDepth() > 8 ? "wav" : "au" );
    return pool_merge(6);
}

void
Wave::Test::PerFrameType::AddTestCasesPerTypeCase(TypeCases* testcases)
{
    Wave::WAV_PCM_TYPE_ID typ = Wave::PCMs;
    for (uint bit = 8; bit <= 64; bit += 8) {
        if (bit == 40)
            bit = 64;
        if (bit == 32)
            typ = Wave::PCMf;
        testcases->Add(new Wave::AudioFrameType(typ, bit, 1, 48000));
        testcases->Add(new Wave::AudioFrameType(typ, bit, 2, 48000));
        testcases->Add(new Wave::AudioFrameType(typ, bit, 4, 48000));
        testcases->Add(new Wave::AudioFrameType(typ, bit, 6, 48000));
        testcases->Add(new Wave::AudioFrameType(typ, bit, 8, 48000));
    }
}

ulong
Wave::Test::AbstractTestCase::F2CC( float val )
{
    ulong longcc = 0;
    sprintf((char*)&longcc, "%3.3f", val);
    return longcc;
}

const char*
Wave::Test::AbstractTestCase::pool_setFl( float value )
{ pool_scope

    char conv[16];
    sprintf( &conv[0], "%f", value );
    return pool_set( &conv[0] );
}

const char*
Wave::Test::AbstractTestCase::pool_setFl( const char* fmt, float val )
{ pool_scope

    char* conv = new char[ strlen(fmt) + 16 ];
    sprintf( conv, fmt, val );
    pool_set( conv );
    delete[] conv;
    return pool_get();
}

const char*
Wave::Test::AbstractTestCase::pool_setDl( const char* fmt, double val )
{ pool_scope

    char* conv = new char[ strlen(fmt) + 32 ];
    sprintf( conv, fmt, val );
    pool_set( conv );
    delete[] conv;
    return pool_get();
}

char*
Wave::Test::AbstractTestCase::FrameToString(Wave::IAudioFrame* testling)
{ pool_scope
    pool_setCheckpoint();
    for (int i = 0; i < testling->Channels(); i++) {
        if (i) pool_setc(',', 1);
        switch ( testling->BitDepth() ) {
        case 8:  pool_setfi( "%i", *(Wave::s8*)testling->GetChannel(i) ); break;
        case 16: pool_setfi( "%i", *(Wave::s16*)testling->GetChannel(i) ); break;
        case 24: pool_setfi( "%i", *(Wave::s24*)testling->GetChannel(i) ); break;
        case 32: pool_setFl( *(Wave::f32*)testling->GetChannel(i) ); break;
        case 64: pool_setFl( *(Wave::f64*)testling->GetChannel(i) ); break;
        }
    } pool_set("\n");
    return pool_collectCheckpoint();
}

uint
Wave::Test::AbstractTestCase::PcmTagToFourCC( WAV_PCM_TYPE_ID tag )
{
    char fourcc[4] = { 'P','C','M',tag == PCMf ? 'f' : PCMs ? 's' : PCMi ? 'i' : 0 };
    if (!fourcc[3]) return *(uint*)"FAIL";
    else return *(uint*)&fourcc[0];
}

char*
Wave::Test::AbstractTestCase::FormatToString( const Format& fmt )
{ pool_scope
    pool_set("Format:------------\n");
    pool_set("pcmtag: ");
    pool_seti( PcmTagToFourCC( WAV_PCM_TYPE_ID(fmt.PCMFormatTag) ) );
    pool_set("\n");
    pool_setfi("bitdepth: %i\n", fmt.BitsPerSample);
    pool_setfi("channels: %i\n", fmt.NumChannels);
    pool_setfi("blockalign: %i\n", fmt.BlockAlign);
    pool_setfi("samplerate: %i\n", fmt.SampleRate);
    pool_setfi("byterate: %i\n", fmt.ByteRate);
    pool_set("-------------------\n");
    return pool_merge(10);
}

char*
Wave::Test::AbstractTestCase::TypeCodeToString( uint typecode )
{ pool_scope

    pool_setfi( "TypeCode:-- %i --\n", typecode );
    const AudioFrameType t = *(const AudioFrameType*)&typecode;
    Format f = CreateWaveFormat(
        t.Rate(), t.BitDepth(), t.Channels(), t.FormatTag()
                                 );
    pool_set("FrameType bit: ");
    if (f.BitsPerSample != t.BitDepth()) {
        pool_set("FAIL"); return pool_merge(2);
    } else pool_setfi("%i\n", t.BitDepth());
    
    pool_set("FrameType chn: ");
    if (f.NumChannels != t.Channels()) {
        pool_set("FAIL"); return pool_merge(4);
    } else pool_setfi("%i\n", t.Channels());
    
    pool_set("FrameType tag: ");
    if (f.PCMFormatTag != t.FormatTag()) {
        pool_set("FAIL"); return pool_merge(6);
    } pool_seti(PcmTagToFourCC(t.FormatTag()));
    pool_setc('\n', 1);
    
    pool_set("FrameType len: ");
    if (f.BlockAlign != t.ByteSize()) {
        pool_set("FAIL"); return pool_merge(9);
    } else pool_setfi("%i\n", t.ByteSize());
    
    pool_set("FrameType frq: ");
    if (f.SampleRate != t.Rate()) {
        pool_set("FAIL"); return pool_merge(11);
    } else pool_setfi("%i\n", t.Rate());
    
    pool_set("-------------------\n");
    return pool_merge(13);
}

void
Wave::Test::AbstractTestCase::PrintLog(const char* text)
{
    const uint len = strlen(text);
    fwrite(text, 1, len, stdout);
    fwrite(text, 1, len, log);
}
