/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WavReader.cpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 31.12.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "WavReader.hpp"

#define ReaderFunctionWrap(rTyp,name) rTyp Stepflow::Audio::FileIO::WaveFileReader:: ## name(void) \
{ return native->name(); }

Stepflow::Audio::FileIO::WaveFileReader::WaveFileReader(void)
{
    native = new stepflow::WaveFileReader();
}
Stepflow::Audio::FileIO::WaveFileReader::WaveFileReader( System::String^ fileName )
{
    char fnam[255];
    try { native = new stepflow::WaveFileReader(
        Stepflow::Audio::Audio::convertFromSystemString(&fnam[0], fileName)
                                                 );
    } catch (std::runtime_error err) {
        throw gcnew Exception( gcnew String(err.what()) );
    }
}
Stepflow::Audio::FileIO::WaveFileReader::~WaveFileReader(void)
{
	native->Close();
	delete native;
}

Stepflow::Audio::PcmFormat
Stepflow::Audio::FileIO::WaveFileReader::GetFormat(void)
{
	return *(PcmFormat*)native->GetFormat();
}

Stepflow::Audio::FileIO::WaveHeader
Stepflow::Audio::FileIO::WaveFileReader::GetHeader(void)
{
	return *(WaveHeader*)native->GetHeader();
}

unsigned
Stepflow::Audio::FileIO::WaveFileReader::GetPosition(void)
{
	return native->GetReadPosition()/native->GetFormat()->BlockAlign;
}

System::TimeSpan
Stepflow::Audio::FileIO::WaveFileReader::GetDuration(void)
{
    return TimeSpan::FromMilliseconds( native->GetDuration() );
}

unsigned
Stepflow::Audio::FileIO::WaveFileReader::Open(System::String^ fileName)
{
    char fnam[255];
	return native->Open( Stepflow::Audio::Audio::convertFromSystemString( &fnam[0], fileName ) );
}

void
Stepflow::Audio::FileIO::WaveFileReader::Seek(unsigned position)
{
	native->Seek(position);
}

void
Stepflow::Audio::FileIO::WaveFileReader::AttachBuffer(Audio^ attachee)
{
    native->AttachBuffer( Audio::operator stepflow::Audio(attachee) );
}

Stepflow::s24     
Stepflow::Audio::FileIO::WaveFileReader::ReadInt24(void)
{
    return *(Stepflow::s24*)&native->ReadInt24();
}

array<Stepflow::Audio::IAudioFrame^>^
Stepflow::Audio::FileIO::WaveFileReader::ReadFrames(uint count)
{
    uint available = native->getFramesAvailable();
    count = count < available
          ? count : available;
    AudioFrameType factory = GetFrameType();
    array<IAudioFrame^>^ output = gcnew array<IAudioFrame^>(count);
    for (int i = 0; i < count; ++i) {
        output[i] = factory.CreateAudioFrame( IntPtr(native->ReadFrame()) );
    } return output;
}


ReaderFunctionWrap(bool, isValid)
ReaderFunctionWrap(byte, ReadByte)
ReaderFunctionWrap(short, ReadInt16)
ReaderFunctionWrap(float, ReadFloat)
ReaderFunctionWrap(unsigned, ReadUInt32)
ReaderFunctionWrap(unsigned, GetLength)
ReaderFunctionWrap(unsigned, Close)

#undef ReaderFunctionWrap
