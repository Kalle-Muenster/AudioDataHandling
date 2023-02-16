/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileReader.cpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveSpacer.h"
#include <precomphead.hpp>
#ifndef  USE_PRE_COMPILED_HEADER
#include <limits.h>
#include <memory.h>
#include "WaveHandling.hpp"
#include "WaveFileReader.hpp"
#endif


WaveSpace(WaveFileReader)::WaveFileReader(void)
    : IAudioInStream<AbstractWaveFileStream>()
    , attachedBuffer(Audio::Silence)
    , canRead(false)
{}

WaveSpace(WaveFileReader)::WaveFileReader(const char* fileName)
    : WaveFileReader() {
    if( !Open(fileName) ) {
#if !defined(__GNUG__)
        throw std::runtime_error(fileName);
#else
        throw std::exception();
#endif
    }
}

WaveSpace(WaveFileReader)::WaveFileReader(const char* fileName, Audio::Data buffer, uint bufferSizeByte)
    : attachedBuffer(buffer) {
    isReading = false;
    read = 0;
    attachedBuffer.cbSize = bufferSizeByte;
    if( !Open(fileName) ) {
#if !defined(__GNUG__)
    throw std::runtime_error(fileName);
#else
    throw std::exception();
#endif
    } framesAvailable = EMPTY;
}

WaveSpace(WaveFileReader)::WaveFileReader( const Format& targetFormat ) : WaveFileReader()
{
    attachedBuffer.format = targetFormat;
}

WaveSpace(WaveFileReader)::WaveFileReader( AudioFrameType aT ) : WaveFileReader()
{
    attachedBuffer.format = CreateWaveFormat( aT.Rate() == 0 ? 44100 : aT.Rate(),
           aT.BitDepth(), aT.Channels(), aT.FormatTag() );
}

WaveSpace(WaveFileReader)::~WaveFileReader(void)
{
    if (f) {
        fflush(f);
        fclose(f);
        f = NULL;
        readhead = NULL;
    }
}


bool
WaveSpace(WaveFileReader)::HasTargetFormat(void) const
{
    return (attachedBuffer.format == fmt) == FormatMatch::DIRECTCOPY;
}

WaveSpace(Format)
WaveSpace(WaveFileReader)::GetTargetFormat(void) const
{
    return attachedBuffer.format;
}

void
WaveSpace(WaveFileReader)::SetTargetFormat( const Format& readAs )
{
    if( attachedBuffer.isValid() )
        attachedBuffer.frameCount = attachedBuffer.cbSize / readAs.BlockAlign;
    attachedBuffer.format = readAs;
}


uint
WaveSpace(WaveFileReader)::Read( Audio::Data buffer, int frames )
{
    unsigned currentPosition = read;
    if ( !getnext( frames * fmt.BlockAlign ) )
        return FREAD_FUNC( buffer, 1, read - currentPosition, f );
    else
        return FREAD_FUNC( buffer, fmt.BlockAlign, frames, f );
}

unsigned
WaveSpace(WaveFileReader)::Read( Audio::Data buffer, unsigned samplesOffset, unsigned samplesCount )
{
    byte* b = (byte*)buffer;
    b += ( samplesOffset * fmt.BitsPerSample >> 3 );
    return Read( b, samplesCount * fmt.NumChannels );
}

unsigned
WaveSpace(WaveFileReader)::Read( Audio& buffer, unsigned frames )
{
    frames = frames ? frames : buffer.getLength();
    int frameCount = (int)frames;
    frames = dataSize / fmt.BlockAlign;
    frameCount = frameCount > 0 && frameCount < frames
               ? frameCount : frames;
    frames = frameCount;
    frameCount = buffer.frameCount < frameCount
               ? buffer.frameCount : frameCount;
#define CASETYPE_COMBO CASE_TYPE::FRAME::TY,CASE_TYPE::FRAME::CH
    if( (buffer.format.NumChannels == fmt.NumChannels)
    &&  (buffer.format.BitsPerSample == fmt.BitsPerSample)
    &&  (buffer.format.PCMFormatTag == fmt.PCMFormatTag))
        getnext( FREAD_FUNC( buffer.data, fmt.BlockAlign, frameCount, f ) );
    else { FRAMETYPE_SWITCH( buffer.frameTypeCode(),
            for (int i = 0; i < frameCount; ++i) {
                *buffer.buffer<CASE_TYPE::FRAME>(i) = ReadFrame<CASETYPE_COMBO>();
            } );
    }
#undef  CASETYPE_COMBO
    if (headerFormat == SND)
        buffer.reverseByteOrder();
    return frameCount;
}

WaveSpace(Audio)
WaveSpace(WaveFileReader)::Read( uint frmCnt )
{
    int frameCount = (int)frmCnt;
    bool reUseBuffer = attachedBuffer.isValid() && (attachedBuffer.data != hdr.GetAudioData());
    frmCnt = dataSize / fmt.BlockAlign;
    frameCount = frameCount > 0 && frameCount < frmCnt
               ? frameCount : frmCnt;
    if (!reUseBuffer) {
        dataSize = frameCount * fmt.BlockAlign;
        Audio readbuffer( &hdr, OWN );
        ownBuffer = false;
        dataSize = frmCnt * fmt.BlockAlign;
        getnext( FREAD_FUNC( readbuffer.data, fmt.BlockAlign, frameCount, f) );
        if( headerFormat == SND ) readbuffer.reverseByteOrder();
        return readbuffer.outscope();
    } else {
            frmCnt = frameCount;
        frameCount = attachedBuffer.frameCount < frameCount
                   ? attachedBuffer.frameCount : frameCount;
#define CASETYPE_COMBO CASE_TYPE::FRAME::TY,CASE_TYPE::FRAME::CH
        if( (attachedBuffer.format.NumChannels == fmt.NumChannels)
        &&  (attachedBuffer.format.BitsPerSample == fmt.BitsPerSample)
        &&  (attachedBuffer.format.PCMFormatTag == fmt.PCMFormatTag) )
            getnext( FREAD_FUNC( attachedBuffer.data, fmt.BlockAlign, frameCount, f ) );
        else { FRAMETYPE_SWITCH( attachedBuffer.frameTypeCode(),
            for( int i = 0; i < frameCount; ++i ) {
                CASE_TYPE::FRAME fr = ReadFrame<CASETYPE_COMBO>();
                CASE_TYPE::FRAME* tf = attachedBuffer.buffer<CASE_TYPE::FRAME>(i);
                for (int c = 0; c < CASE_TYPE::FRAME::CH; ++c) {
                    tf->channel[c] = fr.channel[c];
                }
            } );
        }
#undef  CASETYPE_COMBO
        if( headerFormat == SND ) attachedBuffer.reverseByteOrder();
        return attachedBuffer;
    }
}

WaveSpace(i8)
WaveSpace(WaveFileReader)::ReadByte(void)
{
    return getnext(1) ? fgetc(f) : 0;
}
WaveSpace(s16)
WaveSpace(WaveFileReader)::ReadInt16(void)
{
    return getnext(2) ? fgetwc(f) : 0;
}
WaveSpace(s24)
WaveSpace(WaveFileReader)::ReadInt24(void)
{
    s24 sample;
    if ( getnext(3) ) {
        if (headerFormat == SND) {
            sample.bytes[2] = fgetc(f);
            sample.bytes[1] = fgetc(f);
            sample.bytes[0] = fgetc(f);
        } else {
            sample.bytes[0] = fgetc(f);
            sample.bytes[1] = fgetc(f);
            sample.bytes[2] = fgetc(f);
        }
    } return sample;
}
WaveSpace(i32)
WaveSpace(WaveFileReader)::ReadUInt32(void)
{
    if( getnext(4) ) {
        readhead->S16[0] = fgetwc(f);
        readhead->S16[1] = fgetwc(f);
    } return readhead->I32[0];
}

WaveSpace(f16)
WaveSpace(WaveFileReader)::ReadHalf(void)
{
    short s = getnext(2) ? fgetwc(f) : 0;
    return reinterpret_cast<f16&>(s);
}

WaveSpace(f32)
WaveSpace(WaveFileReader)::ReadFloat(void)
{
    if( getnext(4) ) {
        readhead->S16[0] = fgetwc(f);
        readhead->S16[1] = fgetwc(f);
    } return readhead->F32[0];
}

WaveSpace(f64)
WaveSpace(WaveFileReader)::ReadDouble(void)
{
    if( getnext(8) ) {
        FREAD_FUNC( &readhead->F64[0], 8, 1, f);
    } return readhead->F64[0];
}

bool
WaveSpace(WaveFileReader)::getnext( unsigned pos )
{
    if ( !(canRead = ((read += pos) <= dataSize)) ) {
        readhead->I32[0] = readhead->I32[1] = 0;
        read = dataSize;
    } return canRead;
}

WaveSpace(s8)
WaveSpace(WaveFileReader)::ReadSampleS8(void)
{
    if( HasTargetFormat() ) {
        return (s8)ReadByte();
    } else if ( hdr.isFloatType() ) {
    switch (fmt.BitsPerSample) {
        case 32: return (s8)(ReadFloat() * s8_MAX);
        case 64: return (s8)(ReadDouble() * s8_MAX);
    }} else
    switch(fmt.BitsPerSample) {
        case 16: return (s8)(ReadInt16() / 256);
        case 24: return (s8)(ReadInt24() / 65536);
        case 32: return (s8)((s32)(ReadUInt32() - i32_0DB) / 16777216);
    } return 0;
}

WaveSpace(s16)
WaveSpace(WaveFileReader)::ReadSampleS16(void)
{
    if( HasTargetFormat() ) {
        return ReadInt16();
    } else if ( hdr.isFloatType() ) {
    switch (fmt.BitsPerSample) {
        case 32: return (s16)(ReadFloat() * s16_MAX);
        case 64: return (s16)(ReadDouble() * s16_MAX);
    }} else
    switch (fmt.BitsPerSample) {
        case 8: return (s16)( (s8)ReadByte() * 256 );
        case 24: return (s16) (ReadInt24() / 256);
        case 32: return (s16)((s32)(ReadUInt32() - i32_0DB) / 65536);
    } return 0;
}

WaveSpace(f16)
WaveSpace(WaveFileReader)::ReadSampleF16(void)
{
    if( HasTargetFormat() ) {
        return ReadHalf();
    } else if ( hdr.isFloatType() ) {
    switch (fmt.BitsPerSample) {
        case 32: return f16( ReadFloat() );
        case 64: return f16( ReadDouble() );
    }} else
    switch (fmt.BitsPerSample) {
        case 8: return f16( (s8)ReadByte() ) / 127.0_h;
        case 16: return (f16)ReadInt16() / (f16)s16_MAX;
        case 24: return (f16)ReadInt24() / (f16)s24_MAX;
        case 32: return f16( (int)(ReadUInt32() - i32_0DB) ) / (f16)s32_MAX;
    } return 0.0_h;
}

WaveSpace(s24)
WaveSpace(WaveFileReader)::ReadSampleS24(void)
{
    if( HasTargetFormat() ) {
        return ReadInt24();
    } else if (hdr.isFloatType()) {
    switch (fmt.BitsPerSample) {
        case 16: return s24( ReadHalf() * s24_MAX );
        case 32: return s24( ReadFloat() * s24_MAX );
        case 64: return s24( ReadDouble() * s24_MAX );
    }} else
    switch (fmt.BitsPerSample) {
        case 8:  return s24( (s16)ReadByte() * 65536 );
        case 16: return s24( ReadInt16() * 256 );
        case 32: return s24( (s32)(ReadUInt32() - i32_0DB) / 256 );
    } return 0;
}

WaveSpace(i32)
WaveSpace(WaveFileReader)::ReadSampleI32(void)
{
    if( HasTargetFormat() ) {
        return ReadUInt32();
    } else if (hdr.isFloatType()) {
    switch (fmt.BitsPerSample) {
        case 16: return (i32)(i32_0DB+(s32)(ReadHalf() * i32_0DB));
        case 32: return (i32)(i32_0DB+(s32)(ReadFloat() * i32_0DB));
        case 64: return (i32)(i32_0DB+(s32)(ReadDouble() * i32_0DB));
    }} else
    switch (fmt.BitsPerSample) {
        case 8:  return (i32)(i32_0DB+((s8)ReadByte() * 16777216));
        case 16: return (i32)(i32_0DB+(ReadInt16() * 65536));
        case 24: return (i32)(i32_0DB+(ReadInt24() * 256));
    } return 0;
}


WaveSpace(f32)
WaveSpace(WaveFileReader)::ReadSampleF32(void)
{
    if( HasTargetFormat() ) {
        return ReadFloat();
    } else if (hdr.isFloatType()) {
        switch( fmt.BitsPerSample ) {
        case 16: return ReadHalf().operator float();
        case 64: return (f32)ReadDouble();
        }
    } else
    switch ( fmt.BitsPerSample ) {
        case 8:  return ( (f32)(s8)ReadByte())/127.0f;
        case 16: return ( (f32)ReadInt16() / s16_MAX );
        case 24: return ( (f32)ReadInt24().arithmetic_cast() / s24_MAX );
        case 32: return ( (f32)s32(ReadUInt32()-i32_0DB) / s32_MAX );
    } return 0;
}

WaveSpace(f64)
WaveSpace(WaveFileReader)::ReadSampleF64(void)
{
    if (HasTargetFormat()) {
        return ReadDouble();
    } else if (hdr.isFloatType()) {
                switch( fmt.BitsPerSample ) {
        case 16: return (f64)ReadHalf();
        case 32: return (f64)ReadFloat();
        }
    } else
    switch ( fmt.BitsPerSample ) {
        case 8:  return ((f64)(s8)ReadByte()) / 127.0f;
        case 16: return ((f64)ReadInt16() / s16_MAX);
        case 24: return ((f64)ReadInt24().arithmetic_cast() / s24_MAX);
        case 32: return ((f64)s32(ReadUInt32() - i32_0DB) / s32_MAX);
    } return 0;
}

uint
WaveSpace(WaveFileReader)::GetLength(void) const
{
    return dataSize / fmt.BlockAlign;
}

uint
WaveSpace(WaveFileReader)::GetDuration(void) const
{
    return (uint)(((double)dataSize / fmt.ByteRate) * 1000);
}

uint
WaveSpace(WaveFileReader)::GetPosition( StreamDirection direction ) const
{
    return (read / fmt.BlockAlign);
}

void
WaveSpace(WaveFileReader)::Seek(uint frame,StreamDirection)
{
    unsigned seekPos = frame * hdr.GetBlockAlign();
    
    if( seekPos < dataSize )
        FSEEK_FUNC(f, (read = seekPos) + hdr.GetHeaderSize(), SEEK_SET );
}

uint
WaveSpace(WaveFileReader)::Open( const char* filename )
{
    if (f) Close();

    headerFormat = copyFileNameAndCheckExtension(
                    &fileName[0], filename, NULL );

    if( FOPEN_FUNC( f, filename, "rb" ) ) {
        return false;
    }
    ulong offsetToWaveData = -1;

    if( headerFormat == SND ) {
        SndFileHeader* sndhdr = (SndFileHeader*)&hdr;
        FREAD_FUNC( sndhdr, 1, SndFileHeaderSize+ReadHeadSize, f );
        sndhdr->reverseSndHeader();
        fmt = CreateWaveFormat( sndhdr->GetTypeCode(), sndhdr->SampleRate );
        offsetToWaveData = sndhdr->HeaderSize;
    } else if (headerFormat == WAV) {
        WavFileHeader* whdr = (WavFileHeader*)&hdr;
        FREAD_FUNC( whdr, 1, ExtendedHeaderSize + ReadHeadSize, f );
        offsetToWaveData = whdr->GetHeaderSize();
        fmt = whdr->AudioFormat;
    } else if (headerFormat == PAM) {
        PamFileHeader* pamhdr = (PamFileHeader*)&hdr;
        initializePamFileHeader( pamhdr );
        FREAD_FUNC( &pamhdr->hdr.dat[0], 1, 256, f );
        if( !pamhdr->isValid() ) *pamhdr = wirdPassendGemacht( &pamhdr->hdr.dat[0] );
        pamhdr->makeValueBased();
        offsetToWaveData = PamFileHeaderSize;
        fmt = CreateWaveFormat( pamhdr->GetTypeCode(), pamhdr->GetSampleRate() );
    }

    dataSize =  hdr.GetDataSize();
    if (offsetToWaveData == -1)
        offsetToWaveData = hdr.GetHeaderSize();

    hdr.GetFormat( &fmt );

    FSEEK_FUNC( f, offsetToWaveData, SEEK_SET );
    if ( (!attachedBuffer.isValid()) || (!ownBuffer) ) {
        attachedBuffer = Audio(
            fmt, hdr.GetAudioData(),
            fmt.BlockAlign,
            DONT_ALLOCATE_NEW
        ).outscope(); //////
        attachedBuffer.mode.remFlag( OWN );
        ownBuffer = false;
        framesAvailable = EMPTY;
    } else {
        attachedBuffer.frameCount =
            attachedBuffer.cbSize / hdr.GetBlockAlign();
    }
    attachedBuffer.format = fmt;
    readhead = (ReadOrWriteHead*)hdr.GetAudioData();
    read = 0;
    canRead = true;
    return dataSize;
}

unsigned
WaveSpace(WaveFileReader)::Close(void)
{
    if(f) {
        fflush(f);
        fclose(f);
        dataSize = canRead = f = NULL;
        framesAvailable = EMPTY;
        readhead = NULL;
    } return read;
}

uint
WaveSpace(WaveFileReader)::ReadChannel(int chn, Audio::Data dst, int samplecount)
{
    const int end = samplecount * fmt.BlockAlign;
    const int typesize = (fmt.BitsPerSample >> 3);
    for (int i = 0; i < end; i+=fmt.BlockAlign) {
        if( getnext( FREAD_FUNC( readhead, fmt.BlockAlign, 1, f ) ) )
            memcpy( dst, &readhead->_i8[0] + (chn*typesize), typesize );
        else return i / fmt.NumChannels;
    } return EMPTY;
}

uint
WaveSpace(WaveFileReader)::framesReadable(void) const
{
    return GetLength() - GetPosition( READ );
}

