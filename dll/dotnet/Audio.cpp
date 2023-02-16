/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      Audio.cpp                                  ||
||     Author:    Kalle                                      ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "Audio.hpp"

using namespace System;


const char* _fourCCtoString( ulong& dst, unsigned value)
{
    dst = (ulong)value & 0x00000000ffffffff;
    return (char*)&dst;
}

const char* Stepflow::Audio::Audio::convertFromSystemString( char* dst, System::String^ systring )
{
    const int end = systring->Length;
    char* chpt = dst;
    chpt[end] = '\0';
    for (int i = 0; i < end; ++i) {
        *chpt = (char)systring[i];
        ++chpt;
    } return dst;
}


System::String^
Stepflow::Audio::FileIO::WaveHeader::ToString(void)
{
    ulong filefmt = this->simple.FileFormat;
    void* s = &filefmt;
    
    System::String^ neu = gcnew System::String("FileFormat: ");
    neu->Concat( gcnew System::String((char*)s) );
    neu->Concat("\n");
	neu->Concat( System::String::Format( "{0}: {1}\n{2}: {3}\n{4}: {5}\n",
        simple.RiffChunk.type.ToString(), simple.RiffChunk.size.ToString(),
		simple.FormatChunk.type.ToString(), simple.FormatChunk.size.ToString(),
		simple.DataChunk.type.ToString(), simple.DataChunk.size.ToString()
    ));
	if (this->simple.DataChunk.type != HeaderChunkType::data) {
		neu->Concat( System::String::Format(
            "{0}: {1}\n", extend.DataChunk.type.ToString(), extend.DataChunk.size.ToString()
                                              ) );
    } return neu;  
}

Stepflow::Audio::AudioFrameType
Stepflow::Audio::PcmFormat::FrameType::get(void)
{
    return *reinterpret_cast<AudioFrameType*>(&stepflow::AudioFrameType(stepflow::WAV_PCM_TYPE_ID(Tag), BitsPerSample, NumChannels, SampleRate));
}

Stepflow::Audio::IAudioFrame^ 
Stepflow::Audio::PcmFormat::CreateEmptyFrame( void )
{
    MANAGEDTYPE_SWITCH( AUDIOFRAME_CODE( BitsPerSample, NumChannels, WAV_PCM_TYPE_ID(Tag) ), return gcnew CASE_TYPE(); );
}

Stepflow::Audio::PcmFormat
Stepflow::Audio::PcmFormat::Create( int srt, PcmTag tag, int bit, int chn ) {
    return *reinterpret_cast<const PcmFormat*>( &stepflow::CreateWaveFormat( srt, bit, chn, WAV_PCM_TYPE_ID(tag) ) );
}

Stepflow::Audio::PcmFormat
Stepflow::Audio::PcmFormat::Create( AudioFrameType type, word rate ) {
    return *reinterpret_cast<const PcmFormat*>( &stepflow::CreateWaveFormat( type.code, rate ) );
}

Stepflow::Audio::IAudioFrame^ 
Stepflow::Audio::AudioFrameType::CreateEmptyFrame( void )
{
    MANAGEDTYPE_SWITCH( Code, return gcnew CASE_TYPE(); );
}

Stepflow::Audio::IAudioFrame^
Stepflow::Audio::AudioFrameType::CreateAudioFrame( IntPtr src )
{
    MANAGEDTYPE_SWITCH( Code, return gcnew CASE_TYPE( src ); );
}

Stepflow::Audio::PcmFormat
Stepflow::Audio::AudioFrameType::CreateFormatStruct( int sampleRate )
{
    return *(PcmFormat*)&stepflow::CreateWaveFormat( sampleRate, BitDepth, ChannelCount );
}

Stepflow::Audio::FileIO::SndHeader
Stepflow::Audio::FileIO::SndHeader::FromRawData( IntPtr data ) {
    stepflow::AbstractAudioFileHeader* raw = (stepflow::AbstractAudioFileHeader*)data.ToPointer();
    if( raw->isSndFile() && raw->isValid() ) {
        raw->makeReadable();
        return *reinterpret_cast<SndHeader *>( raw );
    } else throw gcnew Exception( "Pointed data not is an SndHeader" );
}

word
Stepflow::Audio::FileIO::SndHeader::BitDepth::get(void) {
    switch (FormatCode) {
        case SndTypeTag::MULAW_8:
        case SndTypeTag::LINEAR_8: return 8;
        case SndTypeTag::LINEAR_16: return 16;
        case SndTypeTag::LINEAR_24: return 24;
        case SndTypeTag::LINEAR_32:
        case SndTypeTag::FLOAT: return 32;
        case SndTypeTag::DOUBLE: return 64;
        case SndTypeTag::ALAW_8: return 8;
    }
}

Stepflow::Audio::AudioFrameType
Stepflow::Audio::FileIO::SndHeader::GetFrameType(void) {
    stepflow::WAV_PCM_TYPE_ID tag = (FormatCode >= SndTypeTag::FLOAT)
                                  ? stepflow::WAV_PCM_TYPE_ID::PCMf
                                  : stepflow::WAV_PCM_TYPE_ID::PCMs;
    return *(AudioFrameType*)&stepflow::AudioFrameType(
                tag, BitDepth, NumChannels, SampleRate );
}
