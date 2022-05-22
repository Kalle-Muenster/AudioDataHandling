/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WavWriter.cpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 31.12.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "WavWriter.hpp"

#include <indiaccessfuncs.h>
#include <typeinfo>


#define WriterFunctionWrap(name,aT) uint Stepflow::Audio::FileIO::WaveFileWriter:: ## name(aT arg) \
{ return native->name(arg); }

#define WriterCallWrap(name) uint Stepflow::Audio::FileIO::WaveFileWriter:: ## name(void) \
{ return native->name(); }

#define WriterFunctionCustom(export,natnam,argtyp) uint Stepflow::Audio::FileIO::WaveFileWriter:: ## export(argtyp arg) \
{ return native->natnam(arg); }

#define WriterCustomCall(export,natnam) uint Stepflow::Audio::FileIO::WaveFileWriter:: ## export(void) \
{ return native->natnam(); }

Stepflow::Audio::FileIO::WaveFileWriter::WaveFileWriter( void )
{
    native = new stepflow::WaveFileWriter();
}
    
Stepflow::Audio::FileIO::WaveFileWriter::WaveFileWriter( System::String^ fileName )
{
    char fnam[255];
	try { native = new stepflow::WaveFileWriter( Stepflow::Audio::Audio::convertFromSystemString( &fnam[0],fileName) );
	} catch (const std::exception& ex) {
		throw gcnew System::Exception( gcnew System::String(ex.what()) );
	}
}

Stepflow::Audio::FileIO::WaveFileWriter::WaveFileWriter( System::String^ fileName,
	                                      int rate, int depth, int chan ) {
    char fnam[255];
	try { native = new stepflow::WaveFileWriter( Stepflow::Audio::Audio::convertFromSystemString( &fnam[0],fileName),
			                                     rate, depth, chan );
	} catch (const std::exception& ex) {
		throw gcnew System::Exception( gcnew System::String(ex.what()) );
	}
}

Stepflow::Audio::FileIO::WaveFileWriter::WaveFileWriter( System::String^ fileName, PcmFormat% format )
	: WaveFileWriter( fileName, format.SampleRate, format.BitsPerSample, format.NumChannels ) {
}

Stepflow::Audio::FileIO::WaveFileWriter::WaveFileWriter( Audio^ store, System::String^ fileName )
    : WaveFileWriter( fileName, store->Format ) {
    store->native_pntr()->compact();
    WriteAudio( store );
    Flush();
    Close();
}

Stepflow::Audio::FileIO::WaveFileWriter::~WaveFileWriter(void)
{
	native->Close();
	delete native;
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::Open( System::String^ newFile )
{
    char fnam[_MAX_FNAME];
    return native->Open( Stepflow::Audio::Audio::convertFromSystemString( &fnam[0], newFile ) );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::Open( System::String^ newFile,
                                               PcmFormat% newFormat )
{
    char fnam[_MAX_FNAME];
    pin_ptr<const PcmFormat> ptNewFormat = &newFormat;
    native->SetFormat(*(const stepflow::Format*)ptNewFormat);
    return native->Open( Stepflow::Audio::Audio::convertFromSystemString( &fnam[0], newFile ) );
}

bool
Stepflow::Audio::FileIO::WaveFileWriter::isValid(void)
{
	return native->isValid();
}

void
Stepflow::Audio::FileIO::WaveFileWriter::ReStart(bool rewriteHeader)
{
	native->ReStart(rewriteHeader);
}

unsigned     
Stepflow::Audio::FileIO::WaveFileWriter::Save( Audio^ data, System::String^ fileName )
{
    char fnam[255];
	return native->Save( data, Stepflow::Audio::Audio::convertFromSystemString( &fnam[0], fileName ) );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::Close(void)
{
	return native->Close();
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::Flush(void)
{
    return native->Flush();
}


uint Stepflow::Audio::FileIO::WaveFileWriter::Save( Audio^ buffer, String^ fileName, FileFormat fileType )
{
    char fnam[255];
    return native->Save( buffer, Stepflow::Audio::Audio::convertFromSystemString( &fnam[0], fileName ),
                         stepflow::AbstractWaveFileStream::FileFormat(fileType)
                          );
}

WriterFunctionWrap(WriteSample,stepflow::s8)
WriterFunctionWrap(WriteSample,short)
WriterFunctionWrap(WriteSample,System::UInt32)
WriterFunctionWrap(WriteSample,float)
WriterFunctionCustom(WriteFrame,MixFrame, short)
WriterFunctionCustom(WriteFrame,MixFrame, float)
WriterCustomCall(FlushFrame,WriteFrame)
WriterCallWrap(WrittenBytes)
WriterCallWrap(WrittenFrames)


TimeSpan
Stepflow::Audio::FileIO::WaveFileWriter::WrittenTime(void)
{
    return TimeSpan::FromMilliseconds( ((float)native->GetWritePosition() / native->GetFormat()->SampleRate ) * 1000 );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::WriteSample( s24 sample )
{
    return native->WriteSample( (stepflow::s24)sample );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::Write( Audio^ srcBuffer, int countFs, int FsOffsetSrc )
{
    stepflow::Audio src = srcBuffer;
    src.addOffset( FsOffsetSrc );
    src.setLength( countFs );
    return native->Write( src );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::WriteAudio( Audio^ srcBuffer )
{
    return native->Write( srcBuffer );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::WriteFrame( IAudioFrame^ frame )
{
	return native->WriteFrame( frame->GetRaw().ToPointer() );
}

unsigned
Stepflow::Audio::FileIO::WaveFileWriter::WriteFrame(short sample, Panorama mixer)
{
	return native->MixFrame( sample, *(stepflow::Panorama*)&mixer );
}
unsigned      
Stepflow::Audio::FileIO::WaveFileWriter::WriteFrame(float sample, Panorama mixer)
{
	return native->MixFrame( sample, *(stepflow::Panorama*)&mixer );
}
unsigned      
Stepflow::Audio::FileIO::WaveFileWriter::WriteFrame(f64 sample, Panorama mixer)
{
	return native->MixFrame( sample, *(stepflow::Panorama*)&mixer );
}

void
Stepflow::Audio::FileIO::WaveFileWriter::AttachBuffer(Audio^ attachee)
{
    native->attachBuffer( attachee->native_cast(), stepflow::WaveFileWriter::StreamingMode::Stream, false );
}

void
Stepflow::Audio::FileIO::WaveFileWriter::AttachBuffer(System::IntPtr data, uint cbSize)
{
    stepflow::Audio audio( (stepflow::Audio::Data)data.ToPointer(), cbSize, stepflow::SHUTTLE );
    audio.format = *native->GetFormat();
    audio.cbSize = cbSize;
    native->attachBuffer( audio, stepflow::WaveFileWriter::Store, false );
}

uint Stepflow::Audio::FileIO::WaveFileWriter::Write(IntPtr rawData, int countBytes, int offsetSrcBytes)
{
    return native->Write( rawData.ToPointer(), countBytes, offsetSrcBytes );
}

uint Stepflow::Audio::FileIO::WaveFileWriter::CanStream( StreamDirection  stream )
{
    if( stream.In( StreamDirection::OUTPUT ) ) {
        return native->framesWritable();
    } else return 0;
}

#undef WriterFunctionWrap
#undef WriterCallWrap

