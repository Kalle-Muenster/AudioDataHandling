/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileWriter.cpp                         ||
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
#include "WaveFileWriter.hpp"
#endif


WaveSpace(WaveFileWriter)::WaveFileWriter(void)
    : IAudioOutStream<AbstractWaveFileStream>()
    , attachedBuffer(Audio::Silence)
    , fixedSize(EMPTY)
{}

WaveSpace(WaveFileWriter)::WaveFileWriter( const char* filename, unsigned frequency, i16 bitDepth,
                                           i16 channels, unsigned dataSize, Audio::Data buffer,
                                           StreamingMode streamMode )
    : WaveFileWriter()
{
    isWriting = false;
    whdr = CreateWaveHeader(frequency, bitDepth, channels, &whdr, dataSize);
    whdr.AudioFormat.SampleRate = frequency;
    whdr.AudioFormat.BitsPerSample = bitDepth;
    whdr.AudioFormat.NumChannels = channels;
    writehead = (ReadOrWriteHead*)whdr.GetAudioData();

    if ( !Open( filename, FileFormat::Def, dataSize ) ) {
#if !defined(__GNUG__)
        throw std::runtime_error( &fileName[0] );
#else
        throw std::exception();
#endif
    }
    attachedBuffer = buffer;
    mode = streamMode;
}

WaveSpace(WaveFileWriter)::WaveFileWriter( const char* fileName, unsigned dataSize )
    : WaveFileWriter( fileName, 44100, 16, 2, dataSize, WaveSpace(Audio)::Silence,
                      (dataSize==0) ? WaveFileWriter::StreamingMode(1)
                                    : WaveFileWriter::StreamingMode(0) )
{}

WaveSpace(WaveFileWriter)::WaveFileWriter( const char* fileName, const Audio& dataChunk,
                                           WaveFileWriter::StreamingMode streamMode )
    : WaveFileWriter::WaveFileWriter( fileName, dataChunk.format.SampleRate,
                                      dataChunk.format.BitsPerSample,
                                      dataChunk.format.NumChannels,
                                      dataChunk.cbSize )
{
  /*  if( headerFormat == SND ) //&& dataChunk.format.BitsPerSample == 24 ) {
        Audio reversed = dataChunk.byteOrderReversed().outscope();
        attachedBuffer = reversed.buffer<byte>();
        fixedSize = reversed.getLength() * reversed.format.BlockAlign;
        ownBuffer = true;
    } else { */
        attachedBuffer = dataChunk.buffer<byte>();
        fixedSize = dataChunk.getLength() * dataChunk.format.BlockAlign;
        ownBuffer = false;
  //  }
    mode = streamMode;
    isWriting = false;
}

WaveSpace(WaveFileWriter)::WaveFileWriter( const char* fileName, const WaveSpace(Format&) fmt, unsigned cbSize )
    : WaveFileWriter::WaveFileWriter( fileName, fmt.SampleRate, fmt.BitsPerSample, fmt.NumChannels, cbSize )
{}

WaveSpace(WaveFileWriter)::WaveFileWriter( FILE* fileHandle, const Format& fmt,
                                           Audio::Data attachedBuffer,
                                           unsigned fixedframeCount )
{
    STR_STRCPY( 2, &fileName[0], "\0" );
    SetFormat( fmt );
    if ( !Open( fixedframeCount * whdr.AudioFormat.BlockAlign, fileHandle ) ) {
#if !defined(__GNUG__)
        throw std::runtime_error("file not writable");
#else
        throw std::exception();
#endif
    }
    this->attachedBuffer = attachedBuffer;
    ownBuffer = false;
    isWriting = false;
}

WaveSpace(WaveFileWriter)::~WaveFileWriter(void)
{
    if ( Audio::isDataValid( attachedBuffer ) && ownBuffer ) {
        delete attachedBuffer;
    } attachedBuffer = Audio::Silence;

    if (f) {
        FLUSH_FUNC(f);
        fclose(f);
        f = NULL;
    }
    if (fx)
        delete fx;
}

unsigned
WaveSpace(WaveFileWriter)::Open( const char* fileName )
{
    return Open( fileName, FileFormat::Def );
}

bool
WaveSpace(WaveFileWriter)::Open( const char* filename, FileFormat fileType, unsigned dataSize )
{
    isWriting = false;
    if (f) { FLUSH_FUNC(f); Close(); }

    headerFormat = copyFileNameAndCheckExtension(
        &fileName[0], filename, &whdr.AudioFormat, fileType );

    if ( FOPEN_FUNC( f, &fileName[0], "wb" ) ) {
        return false;
    } prepareWavHdr( dataSize );
    if (fx) fx->stateReset();
    return true;
}

bool
WaveSpace(WaveFileWriter)::Open( uint planedSize, FILE* fileHandle, FileFormat fileType )
{
    isWriting = false;
    if (f) { FLUSH_FUNC(f);
        Close();
    } try { f = fileHandle;
        headerFormat = fileType;
        prepareWavHdr( planedSize );
        if (fx) fx->stateReset();
    } catch (...) {
        f = NULL;
        return false;
    } return true;
}

void
WaveSpace(WaveFileWriter)::prepareWavHdr( unsigned dataSize )
{
    isWriting = false;
    fixedSize = dataSize == 0
              ? EMPTY
              : dataSize;
    if ( whdr.AudioFormat.BitsPerSample == 8 ) {
        SndFileHeader sndhdr = CreateSndFileHdr(
            CreateWaveFormat( whdr.AudioFormat.SampleRate,
                              whdr.AudioFormat.BitsPerSample,
                              whdr.AudioFormat.NumChannels ),
                              fixedSize );
        reverseSndHeader( &sndhdr );
        writehead = (ReadOrWriteHead*)whdr.GetAudioData();
        sndhdr.SndTag = HEADER_CHUNK_TYPE::SndFormat;
        WRITE_DATA( &sndhdr, 1, SndFileHeaderSize, f );
        headerFormat = SND;
    } else if (headerFormat == WAV) {
        whdr = CreateWaveHeader( whdr.AudioFormat.SampleRate,
                                 whdr.AudioFormat.BitsPerSample,
                                 whdr.AudioFormat.NumChannels,
                                 fixedSize );
        writehead = (ReadOrWriteHead*)whdr.GetAudioData();
        whdr.ChunkHeader.type = HEADER_CHUNK_TYPE::DataChunk;
        WRITE_DATA( &whdr, 1, SimpleHeaderSize, f );
        headerFormat = WAV;
    } else {
        PamFileHeader pamhdr = CreatePamFileHdr(
                    whdr.AudioFormat, fixedSize );
        writehead = (ReadOrWriteHead*)whdr.GetAudioData();
        WRITE_DATA( pamhdr.makeStringBased(), 1,
                    PamFileHeaderSize, f );
        headerFormat = PAM;
    }
    FLUSH_FUNC(f);
    write = 0;
}

bool
WaveSpace(WaveFileWriter)::NewFile( const char* name, unsigned frq, unsigned bit, unsigned nch, FileFormat fileType  )
{
    Close();
    whdr.AudioFormat = CreateWaveFormat( frq, bit, nch );
    headerFormat = fileType;
    return Open( name );
}

bool
WaveSpace(WaveFileWriter)::NewFile( const char* fileName, const AudioFrameType type, int rate, FileFormat fileType )
{
    return NewFile( fileName, rate, type.BitDepth(), type.Channels(), fileType );
}

bool
WaveSpace(WaveFileWriter)::NewFile( const char* name, const Format& fmt, FileFormat fileType )
{
    Close();
    whdr.AudioFormat = fmt;
    headerFormat = fileType;
    return Open( name );
}

void
WaveSpace(WaveFileWriter)::ReStart( bool rewriteHeader )
{
    if (rewriteHeader) {
        FLUSH_FUNC(f);
        FSEEK_FUNC(f, 0, SEEK_SET);
        switch (headerFormat) {
        case SND: {
            SndFileHeader sndhdr = CreateSndFileHdr( whdr.AudioFormat, whdr.GetDataSize() );
            reverseSndHeader(&sndhdr);
            sndhdr.SndTag = HEADER_CHUNK_TYPE::SndFormat;
            WRITE_DATA( &sndhdr, 1, SndFileHeaderSize, f );
        } break;
        case PAM: {
            PamFileHeader pamhdr = CreatePamFileHdr( whdr.AudioFormat, whdr.GetDataSize() );
            WRITE_DATA( pamhdr.makeStringBased(), 1, PamFileHeaderSize, f );
        } break;
        case WAV:
            WRITE_DATA( &whdr, 1, SimpleHeaderSize, f );
        break;
        }
        FLUSH_FUNC(f);
        write = 0;
    } else {
        FLUSH_FUNC(f);
        FSEEK_FUNC(f, headerFormat==SND ? SndFileHeaderSize
                    : headerFormat==PAM ? PamFileHeaderSize
                    : whdr.GetHeaderSize(), SEEK_SET );
        write = 0;
    } if (fx) fx->stateReset();
}


unsigned
WaveSpace(WaveFileWriter)::Write( void )
{
    if (mode == Stream)
        return ( write += Flush() );
    else if ( Audio::isDataValid(attachedBuffer) && (fixedSize != EMPTY) ) {
        unsigned writePosition = write % fixedSize;
        return Write( &((byte*)attachedBuffer)[writePosition],
                      fixedSize - writePosition );
    } return 0;
}

unsigned
WaveSpace(WaveFileWriter)::Write( Audio::Data data, int frameCount )
{
    const uint startPosition = write;
    if (lock(write)) {
        i32 bytewrite = frameCount * whdr.AudioFormat.BlockAlign;
        if ((write + bytewrite) > fixedSize)
            frameCount = (bytewrite = (fixedSize - write))
                       / whdr.AudioFormat.BlockAlign;
        if (fx)
            fx->doFrames( data, frameCount );
        WRITE_DATA( data, frameCount, whdr.AudioFormat.BlockAlign, f );
        if (fixedSize == (write += bytewrite))
            Close();
    unlock(write); }
    return write - startPosition;
}

unsigned
WaveSpace(WaveFileWriter)::Write( const Audio& audio )
{
    unsigned bytewrite=0;
    if( audio.canBeTransfaredBy( INTERLEAVE, whdr.AudioFormat ) ) {
        if( lock(write) ) {
            if (fx || (audio.chunkCount() > 1) ) {
                Audio cmpct = audio.compacted();
                bytewrite = WRITE_DATA(fx ? fx->doFrames(cmpct.data, cmpct.frameCount) : cmpct.data, cmpct.format.BlockAlign, cmpct.frameCount, f);
            } else {
                bytewrite = WRITE_DATA( audio.buffer<byte>(), 1, audio.getLength()*audio.format.BlockAlign, f );
            }
        unlock(write); }
    } else if( write > 0) {
        if( lock(write) ) {
            Audio conv = audio.converted( whdr.AudioFormat.BitsPerSample, whdr.AudioFormat.NumChannels );
            bytewrite = WRITE_DATA(fx ? fx->doFrames(conv.data, conv.frameCount) : conv.data, whdr.AudioFormat.BlockAlign, conv.frameCount, f);
        unlock(write); }
    } else {
        whdr.AudioFormat = audio.format;
        whdr.AudioFormat.PCMFormatTag = whdr.AudioFormat.BitsPerSample >= 32
                                      ? PCMf : PCMs;
        if ( whdr.AudioFormat.BitsPerSample == 8 ) {
            headerFormat = SND;
        } ReStart( true );
        Write( audio );
    } if ( fixedSize == (write += bytewrite) )
            Close();
    return bytewrite;
}

unsigned
WaveSpace(WaveFileWriter)::WriteSample( s8 data )
{
    if (fx) data = fx->doSample<s8>( data );
    write += WRITE_BYTE( data, f );
    return write >= fixedSize ? Close() : write;

    if (!whdr.isFloatType()) {
        if (whdr.AudioFormat.BitsPerSample == 8) {
            if (fx) data = fx->doSample<s8>(data);
            return (write += WRITE_BYTE(data, f)) >= fixedSize ? Close() : write;
        }
        else switch (whdr.AudioFormat.BitsPerSample) {
        case 16: return WriteSample((s16)(data*256));
        case 24: return WriteSample( s24(data*65536) );
        case 32: return WriteSample((i32)( ((s32)data*i32_0DB) + i32_0DB) );
        }
    }
    else switch (whdr.AudioFormat.BitsPerSample) {
    case 32: return WriteSample((f32)data / s8_MAX);
    case 64: return WriteSample((f64)data / s8_MAX);
    } throw std::exception( INVALID_TYPECONVRSION );
}

unsigned
WaveSpace(WaveFileWriter)::WriteSample( s16 data )
{
    if ( !whdr.isFloatType() ) {
        if ( whdr.AudioFormat.BitsPerSample == 16 ) {
            if (fx) data = fx->doSample<s16>( data );
            return ( write += WRITE_S16S(data,f) ) >= fixedSize ? Close() : write;
        } else switch ( whdr.AudioFormat.BitsPerSample ) {
            case  8: return WriteSample( (s8)(data/256) );
            case 24: return WriteSample( s24(data*256) );
            case 32: return WriteSample( (i32) (((s32)data*s16_MAX) + i32_0DB) );
        }
    } else switch (whdr.AudioFormat.BitsPerSample) {
        case 32: return WriteSample( (f32)data/s16_MAX );
        case 64: return WriteSample( (f64)data/s16_MAX );
    } throw std::exception( INVALID_TYPECONVRSION );
}

uint
WaveSpace(WaveFileWriter)::WriteSample( i32 data )
{
    if (!whdr.isFloatType()) {
        if (whdr.AudioFormat.BitsPerSample == 32) {
            if (fx) data = fx->doSample<i32>(data);
            return (write += WRITE_DATA(&data, 1, 4, f)) >= fixedSize ? Close() : write;
        }
        else switch (whdr.AudioFormat.BitsPerSample) {
        case  8: return WriteSample((s8) ((data - i32_0DB) / 16777216) );
        case 16: return WriteSample((s16)((data - i32_0DB) / 65536));
        case 24: return WriteSample( s24( (s32)(data - i32_0DB) / 256) );
        }
    }
    else switch (whdr.AudioFormat.BitsPerSample) {
    case 32: return WriteSample( (f32)(data - i32_0DB) / s32_MAX );
    case 64: return WriteSample( ((f64)data / i32_MAX) - 0.5);
    } throw std::exception( INVALID_TYPECONVRSION );
}

uint
WaveSpace(WaveFileWriter)::WriteSample( s24 data )
{
    if (!whdr.isFloatType()) {
        if (whdr.AudioFormat.BitsPerSample == 24) {
            if (fx) data = fx->doSample<s24>(data);
            return (write += WRITE_DATA( &data, 1, 3, f )) >= fixedSize ? Close() : write;
        }
        else switch (whdr.AudioFormat.BitsPerSample) {
        case  8: return WriteSample( (s8)(data / 65536 ) );
        case 16: return WriteSample( (s16)( data / 256) );
        case 32: return WriteSample((i32)(( data * 256 ) + i32_0DB));
        }
    }
    else switch (whdr.AudioFormat.BitsPerSample) {
    case 32: return WriteSample((f32)data / std::numeric_limits<s24>::max());
    case 64: return WriteSample((f64)data / std::numeric_limits<s24>::max());
    } throw std::exception( INVALID_TYPECONVRSION );
}

uint
WaveSpace(WaveFileWriter)::WriteSample( f64 data )
{
    if (fx) data = fx->doSample<f64>( data );
    return (write += WRITE_DATA(&data, 1, 8, f)) >= fixedSize
         ? Close()
         : write;

    if (whdr.isFloatType()) {
        if (whdr.AudioFormat.BitsPerSample == 64) {
            if (fx) data = fx->doSample<f64>(data);
            return (write += WRITE_DATA(&data, 1, 8, f)) >= fixedSize ? Close() : write;
        }
        else switch (whdr.AudioFormat.BitsPerSample) {
        case 32: return WriteSample((f32)data);
        default: throw std::exception( INVALID_TYPECONVRSION );
        }
    }
    else switch (whdr.AudioFormat.BitsPerSample) {
    case  8: return WriteSample((s8)(data*128.0f));
    case 16: return WriteSample((s16)(data*s16_MAX));
    case 24: return WriteSample( s24(data*s24_MAX));
    case 32: return WriteSample((i32)(data*i32_0DB)+i32_0DB);
    } throw std::exception( INVALID_TYPECONVRSION );
}

uint
WaveSpace(WaveFileWriter)::WriteSample( f32 data )
{
    if ( whdr.isFloatType() ) {
        if ( whdr.AudioFormat.BitsPerSample == 32 ) {
            if (fx) data = fx->doSample<f32>( data );
            return ( write += WRITE_DATA( &data, 1, 4, f) ) >= fixedSize ? Close() : write;
        } else switch ( whdr.AudioFormat.BitsPerSample ) {
            case 64: return WriteSample( (f64)data );
            default: throw std::exception( INVALID_TYPECONVRSION );
        }
    } else switch ( whdr.AudioFormat.BitsPerSample ) {
        case  8: return WriteSample( (s8)(data*128.0f) );
        case 16: return WriteSample( (s16)(data*s16_MAX) );
        case 24: return WriteSample( s24(data*s24_MAX));
        case 32: return WriteSample( (i32)(data*i32_0DB) + i32_0DB);
    } throw std::exception( INVALID_TYPECONVRSION );
}

unsigned
WaveSpace(WaveFileWriter)::WriteFrame(void)
{
    if( Audio::isDataValid( attachedBuffer ) && f )
        *writehead = *(ReadOrWriteHead*)&((byte*)attachedBuffer)[ (fixedSize != EMPTY_(unsigned)
                                                                   ?
                                               (write % fixedSize) : write) ];
    else return 0;
    return ( (write += WRITE_DATA(fx? fx->doFrame(writehead):writehead, 1, whdr.AudioFormat.BlockAlign, f ) ) >= fixedSize )
         ? Close()
         : write;
}

unsigned
WaveSpace(WaveFileWriter)::WriteFrame( Audio::Data frame )
{
    if( (!f) || (!frame) || (frame == Audio::Silence) ) return 0;
    if( fx ) { frame = fx->doFrame(
        MEMCPYFUNC( writehead, frame, whdr.AudioFormat.BlockAlign, 1)
                                     );
    } write += WRITE_DATA( frame, 1, whdr.AudioFormat.BlockAlign, f );
    return write >= fixedSize ? Close() : write;
}

unsigned
WaveSpace(WaveFileWriter)::Close(void)
{
    uint writtenData = 0;
    if(f) if (lock(write)) {
        if (write) { FLUSH_FUNC(f);
            if ( whdr.AudioFormat.BitsPerSample == 8 )
                headerFormat = SND;
            if ( fixedSize == EMPTY || mode == Stream ) {
                WRITE_DATA(Audio::Silence, 1, 8, f);
                unsigned filesize = ftell(f) - 8;
                FSEEK_FUNC(f, 0, SEEK_SET);
                switch (headerFormat) {
                case WAV:
                    whdr.RiffChunk.size = filesize;
                    writtenData = whdr.RiffChunk.size - Constants::RIFFChunkSize;
                    whdr.ChunkHeader.type = HEADER_CHUNK_TYPE::DataChunk;
                    whdr.ChunkHeader.size = writtenData;
                    WRITE_DATA( &whdr, 1, Constants::SimpleHeaderSize, f );
                    break;
                case SND:
                    SndFileHeader sndhdr = CreateSndFileHdr( whdr.AudioFormat,
                                  writtenData = filesize - SndFileHeaderSize );
                    reverseSndHeader( &sndhdr );
                    sndhdr.SndTag = HEADER_CHUNK_TYPE::SndFormat;
                    whdr.ChunkHeader.size = writtenData;
                    WRITE_DATA( &sndhdr, 1, Constants::SndFileHeaderSize, f );
                    break;
                case PAM:
                    PamFileHeader pamhdr = CreatePamFileHdr( whdr.AudioFormat,
                             writtenData = filesize - (PamFileHeaderSize - 8) );
                    whdr.ChunkHeader.size = writtenData;
                    pamhdr.makeStringBased();
                    WRITE_DATA( &pamhdr.hdr.dat[0], 1, PamFileHeaderSize, f );
                    break;
                }
            } else {
                writtenData = ftell(f) - (headerFormat == SND ? SndFileHeaderSize : headerFormat == PAM ? PamFileHeaderSize : SimpleHeaderSize);
                write = (fixedSize - writtenData);
                if ( write ) while ( write -= WRITE_DATA( Audio::Silence, 1, whdr.AudioFormat.BlockAlign, f ) );
            }
        } FLUSH_FUNC(f);
        fclose(f);
        f = NULL;
    unlock(write);
    } return writtenData;
}

/*
unsigned
WaveSpace(WaveFileWriter)::Save( FileFormat header )
{
    headerFormat = header;
    if ( mode == Stream )
        fixedSize = write;
    write += Flush();
    return Close();
}
*/

unsigned
WaveSpace(WaveFileWriter)::Save( const Audio& audio, const char* filename, FileFormat fileType )
{
    whdr.AudioFormat = audio.format;
    attachBuffer( audio, Store, true );
    if( ! Open( filename, fileType, audio.format.BlockAlign * audio.getLength() ) )
        return 0;
    headerFormat = fileType;
    if (mode == Stream)
        fixedSize = write;
    write += Flush();
    return Close();
}

unsigned
WaveSpace(WaveFileWriter)::Flush(void)
{
    if (f) {
        if (lock(write)) {
            unsigned written = 0;
            if( attachedBuffer != Audio::Silence ) {
                if (Audio::isDataValid(attachedBuffer) && write < fixedSize)
                {
                    byte* src;
                    uint  elm;
                    uint  cnt;

                    if (mode == Stream) {
                        unsigned cbSize = fixedSize;
                        unsigned cwPos = write % (fixedSize + 1);
                        src = &((byte*)attachedBuffer)[cwPos];
                        cnt = ((fixedSize - cwPos) - (fixedSize - cbSize))
                            / whdr.AudioFormat.BlockAlign;
                        elm = whdr.AudioFormat.BlockAlign;
                        written = WRITE_DATA(src, elm, cnt, f) * elm;
                    } // if(mode == Store)
                    else if ( enum_utils::is_val(fixedSize) ) {
                        elm = whdr.AudioFormat.BlockAlign;
                        cnt = (fixedSize - write) / elm;
                        src = &((byte*)attachedBuffer)[write];
                        written = WRITE_DATA(src, elm, cnt, f) * elm;
                    }  
                    /*
                    if (headerFormat == SND && whdr.AudioFormat.BitsPerSample > 8) {
                        Audio revOrd(src,cnt*elm);
                        ReverseByteOrder( revOrd.data, whdr.AudioFormat, cnt);
                        written = WRITE_DATA( revOrd.data, elm, cnt, f) * elm;
                        revOrd.drop();
                    } else {
                        written = WRITE_DATA(src, elm, cnt, f) * elm;
                    }
                    */
                }
            } FLUSH_FUNC(f);
            unlock(write);
            return written;
        } else return 0;
    } else {// if(!f)
        return EMPTY;
    }
}

uint
WaveSpace(WaveFileWriter)::GetPosition(StreamDirection direction) const
{
    return ( write / whdr.AudioFormat.BlockAlign );
}

uint
WaveSpace(WaveFileWriter)::GetDuration(void) const
{
    return WrittenTime();
}

uint
WaveSpace(WaveFileWriter)::WrittenTime(void) const
{
    return (uint)(((double)write / whdr.AudioFormat.ByteRate) * 1000);
}
uint
WaveSpace(WaveFileWriter)::WrittenFrames(void) const
{
    return write / whdr.AudioFormat.BlockAlign;
}
uint
WaveSpace(WaveFileWriter)::WrittenBytes(void) const
{
    return write;
}

void
WaveSpace(WaveFileWriter)::SetFile( FILE* newFile )
{
    if (f) Close();
    f = newFile;
    prepareWavHdr( fixedSize );
}
void
WaveSpace(WaveFileWriter)::SetFileName( const char* newFileName )
{
    ARR_STRCPY( fileName, newFileName );
}

WaveSpace(WaveFileWriter)::StreamingMode
WaveSpace(WaveFileWriter)::getMode(void)
{
    return mode;
}
void
WaveSpace(WaveFileWriter)::setMode(StreamingMode streamMode)
{
    mode = streamMode;
}

void
WaveSpace(WaveFileWriter)::attachBuffer( Audio datachunk, StreamingMode mode, bool forceClose )
{
    if ( datachunk.chunkCount() > 1 )
        throw new std::exception( "ERROR: Audio datachunk must be single data chunk and must not have negative offset applied" );
    if( forceClose )
        Close();

    if( ownBuffer && Audio::isDataValid( attachedBuffer ) ) {
        delete [] (byte*)attachedBuffer;
        ownBuffer = false;
    }

    whdr.AudioFormat = datachunk.format;
    fixedSize = datachunk.getLength() * datachunk.format.BlockAlign;

    attachedBuffer = datachunk.buffer<byte>(0);
    ownBuffer = datachunk.mode.hasFlag( OWN );
    datachunk.mode.remFlag( OWN );
    setMode( mode );
    write = 0;
}

void
WaveSpace(WaveFileWriter)::SetFormat( const Format& fmt )
{
    SetFormat( fmt.SampleRate, fmt.BitsPerSample, fmt.NumChannels );
    whdr.AudioFormat.PCMFormatTag = fmt.PCMFormatTag;
}

void
WaveSpace(WaveFileWriter)::SetFormat( unsigned sFreq, unsigned bitDepth, unsigned numChan )
{
    if( whdr.AudioFormat.SampleRate != sFreq
     || whdr.AudioFormat.BitsPerSample != bitDepth
     || whdr.AudioFormat.NumChannels != numChan ) {
        if (f) Close();
        whdr.AudioFormat.ByteRate = (
            ( ( whdr.AudioFormat.SampleRate = sFreq ) * (
                whdr.AudioFormat.BlockAlign = ((
                    whdr.AudioFormat.BitsPerSample = bitDepth) / 8) * (
                        whdr.AudioFormat.NumChannels = numChan
                    )
                )
            )
        );
    } whdr.AudioFormat.PCMFormatTag = whdr.AudioFormat.BitsPerSample > 24
                                    ? PCMf : PCMs;
}

unsigned
WaveSpace(WaveFileWriter)::GetLength(void) const
{
    return ( enum_utils::is_val(fixedSize) ? fixedSize / whdr.AudioFormat.BlockAlign : GetPosition( WRITE ) );
}

void
WaveSpace(WaveFileWriter)::Seek( unsigned frame, StreamDirection origin )
{
    int seekdst;
    if( enum_utils::hasFlag( origin, StreamDirection::CUR ) ) {
        seekdst = (frame * whdr.AudioFormat.BlockAlign);
        FSEEK_FUNC( f, seekdst, SEEK_CUR );
        write += seekdst;
    } else {
        if( enum_utils::hasFlag( origin, StreamDirection::END ) )
            seekdst = ((GetLength() - frame) * whdr.AudioFormat.BlockAlign);
        else
            seekdst = (frame * whdr.AudioFormat.BlockAlign);
        int headerSize = headerFormat == SND ? SndFileHeaderSize
                       : headerFormat == PAM ? PamFileHeaderSize
                       : whdr.GetHeaderSize();
        FSEEK_FUNC(f, seekdst + headerSize, SEEK_SET);
        write = seekdst;
    } if (fx) fx->stateReset();
}

uint
WaveSpace(WaveFileWriter)::WriteChannel(int channel, Audio::Data dat, int samplecount)
{
    throw std::exception("Not Supported yet!");
}

uint
WaveSpace(WaveFileWriter)::Write(Audio::Data data, uint cbSize, uint offset)
{
    return Write(data, cbSize / whdr.AudioFormat.BlockAlign) * whdr.AudioFormat.BlockAlign;
}

uint
WaveSpace(WaveFileWriter)::framesWritable(void) const
{
    return enum_utils::is_val(fixedSize)
         ? fixedSize - GetWritePosition()
         : EMPTY;
}



