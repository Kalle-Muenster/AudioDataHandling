/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveAudio.cpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 12.01.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveSpacer.h"
#include <precomphead.hpp>
#ifndef  USE_PRE_COMPILED_HEADER
#include <limits.h>
#include <memory.h>

#include <WaveLib.inl/enumoperators.h>
#include <WaveLib.inl/indiaccessfuncs.h>
#include <WaveLib.inl/Int24bitTypes.hpp>

#include "WaveMacros.hpp"
#include "Panorama.hpp"
#include "WaveFormat.hpp"
#include "AudioChannel.hpp"

#include "WaveAudio.hpp"
#endif


#define own mode.flags.group.owned


static bool
isAudioPointerValid(const WaveSpace(Audio)*  audivalidi)
{
    if (audivalidi)
        return (audivalidi != nullptr)
        ? ((audivalidi->frameCount
            + (audivalidi->offset<0 ? -audivalidi->offset : 0)
            ) * audivalidi->format.BlockAlign
            ) == audivalidi->cbSize
        : false;
    return false;
}

WaveSpace(Audio*)
WaveSpace(Audio)::cloneContainer(Audio*auA, Audio*auB, unsigned sizeOpt)
{
    return (Audio*)MEMCPYFUNC(auA, auB, sizeOpt ? sizeOpt : sizeof(Audio), 1);
}

bool
WaveSpace(Audio)::isDataValid(Data check) {
    return !((check == Audio::Silence)
           ||(check == nullptr)
           ||(check == NULL));
}

WaveSpace(Audio)::Audio(Initiatio initflags)
    : format({ WAV_PCM_TYPE_ID::PCMs, i16(2), i32(44100),
        i32(176400), i16(4), i16(16) })
    , mode(initflags)
    , data(Silence)
    , offset(0)
    , cutend(0)
    , ptd(NULL)
    , nxt(NULL)
{
    if (mode.hasFlag(CIRCULAR)) {
        nxt = this;
    }
}

WaveSpace(Audio)::Audio(void)
    : Audio(DEFAULT)
{}

WaveSpace(Audio)::Audio(byte channels)
    : format({ WAV_PCM_TYPE_ID::PCMs, (i16)channels, (i32)44100,
    (i32)(88200 * channels), (i16)(2 * channels), (i16)16 })
    , mode(DEFAULT)
    , data(Silence)
    , offset(0)
    , cutend(0)
    , ptd(NULL)
    , nxt(NULL)
{}

//container copy (copied container will use same buffer data)
WaveSpace(Audio)::Audio(const Audio& wrap)
    : mode( Initiatio( ( wrap.mode.flags.all|DONT_ALLOCATE_NEW ) & ~OWN ) )
    , frameCount(wrap.frameCount)
    , cbSize(wrap.cbSize)
    , format(wrap.format)
    , offset(wrap.offset)
    , cutend(wrap.cutend)
    , data(wrap.data)
    , ptd(wrap.ptd)
    , nxt(wrap.nxt)
{}

/*container copy (copied container will use same buffer data)
WaveSpace(Audio)::Audio(Audio &wrap)
    : Audio(*(const Audio*)&wrap)
{} */

//buffer copy (copied container will allocate a new, own buffer)
WaveSpace(Audio)::Audio(const Audio& clone, float amplificator)
    : Audio(clone.format, clone.frameCount)
{
    mode = clone.mode;
    FRAMETYPE_SWITCH( frameTypeCode(),
        convertData<CASE_TYPE::TY>((CASE_TYPE::FRAME*)data, amplificator, format.NumChannels, (Audio*)&clone);
    )
    mode.addFlag(OWN);
}

//conditional copy ('flags' will decide if buffer should be an own clone or a shared reference)
WaveSpace(Audio)::Audio( const Audio& copy, Initiatio flags )
    : Audio(copy.format.SampleRate, copy.format.BitsPerSample, copy.format.NumChannels, copy.frameCount, flags)
{}

WaveSpace(Audio&)
WaveSpace(Audio)::operator=(Audio& other)
{
    if (this->own && (this->data != other.data))
        if (isDataValid(this->data))
            delete this->data;

    this->data = other.data;
    this->cbSize = other.cbSize;
    this->format = other.format;
    this->frameCount = other.frameCount;
    this->offset = other.offset;
    this->cutend = other.cutend;
    this->mode = other.mode;
    this->ptd = (size_t)&other;
    this->takeOwnership();
    return this[0];
}

bool
WaveSpace(Audio)::takeOwnership(bool ifOwnerUnknown)
{
    if( ptd == (size_t)this ) {
        this->mode.addFlag(OWN);
    } else if ( ptd ) {
        Audio* dist = (Audio*)ptd;
        dist->ptd = (size_t)this;
        dist->mode.remFlag(OWN);
        this->ptd = dist->ptd;
        if( !dist->mode.hasFlag( RAW_FILE_DATA ) )
            this->mode.addFlag(OWN);
    } else if ( ifOwnerUnknown ) {
        this->ptd = (size_t)this;
        this->mode.addFlag(OWN);
    } return mode.hasFlag(OWN);
}


WaveSpace(Audio)*
WaveSpace(Audio)::operator&(void)
{
    ptd = (size_t)this;
    return this;
}

WaveSpace(Audio)
WaveSpace(Audio)::operator*(void)
{
    if (((NoData)this == Silence) || (!isValid())) {
        Audio silence(Silence);
        return silence;
    } else
        return *this;
}

void
WaveSpace(Audio)::addOffset( uint frames )
{
    if( offset < 0 ) applyOffset( frames );
    else offset += frames;
}

bool
WaveSpace(Audio)::applyOffset( uint frames )
{
    if( offset <= 0 ) {
        offset -= frames;
        unsigned byteOffset = frames * format.BlockAlign;
        data = (Data)((byte*)data + byteOffset);
        cbSize -= byteOffset;
        return true;
    } return false;
}

void
WaveSpace(Audio)::removeOffset( void )
{
    if( offset > 0 )
        offset = 0;
}

void
WaveSpace(Audio)::addEndcut( uint shortenByFrames )
{
    const uint maxcut = frameCount - (cutend + (offset >= 0 ? offset : 0));
    cutend += minOf( shortenByFrames, maxcut );
}

void
WaveSpace(Audio)::removeEndcut(void)
{
    if( cutend )
        cutend = 0;
}

void
WaveSpace(Audio)::resetOffset(void)
{
    if( offset < 0 ) {
        unsigned byteOffset = -offset * format.BlockAlign;
        data = (Data)((byte*)data - byteOffset);
        cbSize += byteOffset;
        frameCount = cbSize * format.BlockAlign;
    } offset = 0;

}

// allocate and own a new buffer of 'size' bytes
// (if already owning some data this will be freed before)
bool
WaveSpace(Audio)::createBuffer(unsigned size)
{
    if ( isDataValid(data) && own )
        free( data );

    if ( size!=NULL && size!=EMPTY ) {
        data = (byte*)malloc(cbSize = size);
        frameCount = cbSize / format.BlockAlign;
        own = true;
    } else {
        own = cbSize = frameCount = 0;
        data = Silence;
    } return (bool)cbSize;
}

// create an 'Audio' which allocates and owns 'size' bytes
WaveSpace(Audio)::Audio(unsigned allocateSize)
    : Audio(byte(2))
{
    createBuffer( allocateSize );
}

// allocate a new buffer which at 'format' can hold 'framecount' frames
WaveSpace(Audio)::Audio(const Format& fmt, int framecount)
    : Audio(byte(fmt.NumChannels))
{
    format = CreateWaveHeader(fmt.SampleRate, fmt.BitsPerSample, fmt.NumChannels).AudioFormat;
    createBuffer( framecount * fmt.BlockAlign );
}

// from 'raw data' constructor ( BufferInitiatio decides if wrap data or new copy )
WaveSpace(Audio)::Audio(const Format& fmt, Data buffer, unsigned sizeBytes, Initiatio initiatio)
    : mode(initiatio)
    , format(fmt)
    , offset(0)
    , cutend(0)
    , data(0)
    , nxt(0)
    , ptd(0)
    , cbSize(sizeBytes)
    , frameCount(sizeBytes/fmt.BlockAlign)
{
    if (mode.flags.group.initiationflags) {
        if (mode.hasFlag(ALLOCATE_NEW_COPY)) { // allocate and clone
            if (createBuffer(sizeBytes))
                memcpy(data, buffer, sizeBytes);
        }
        else if (mode.hasFlag(DONT_ALLOCATE_NEW)) {
            if ( sizeBytes == EMPTY ) {
                frameCount = cbSize = 0;
            } else {
                frameCount = (sizeBytes / format.BlockAlign);
                cbSize = sizeBytes;
            } data = buffer; // just reference the origin
        }
    } if (mode.hasFlag(CIRCULAR)) {
        nxt = this;
    }
}

WaveSpace(Audio)::Audio( const AbstractAudioFileHeader* hdr )
    : Audio(RAW_FILE_DATA|DONT_ALLOCATE_NEW)
{
    format = CreateWaveFormat(
        hdr->GetSampleRate(),
        hdr->GetBitDepth(),
        hdr->GetChannelCount()
    );
    frameCount = ( cbSize = hdr->GetDataSize() )
               / format.BlockAlign;
    data = hdr->GetAudioData();
    own = false;
}

// takes 'WaveFile' header for determining format
WaveSpace(Audio)::Audio( const WavFileHeader& hdr, Initiatio initiatio = ALLOCATE_NEW_COPY )
    : Audio(initiatio)
{
    format = hdr.AudioFormat;

    if (!mode.hasFlag(DONT_ALLOCATE_NEW)) {
        createBuffer(hdr.GetDataSize());
    }
    else {
        if (mode.hasFlag(RAW_FILE_DATA) ) {
            data = hdr.GetAudioData();
        } frameCount = (cbSize = hdr.GetDataSize()) / hdr.AudioFormat.BlockAlign;
    }
}

WaveSpace(Audio)::Audio( Audio::Data ptBuffer,
                         unsigned dataSize,
                         Initiatio initiatio )
    : Audio( (RAW_FILE_DATA&initiatio
              ? *(WavFileHeader*)ptBuffer
              : initializeNewWaveheader(dataSize==EMPTY?0:dataSize) ),
             (ptBuffer == Audio::Silence)
              ? (DONT_ALLOCATE_NEW | initiatio)
              : initiatio )
{
    Audio::Data beginData = Silence;

    if (initiatio & RAW_FILE_DATA) {
     // interprete pointed data being 'file stream'
     // which is beginning with a wave file header.
        beginData = ((AbstractAudioFileHeader*)ptBuffer)->GetAudioData();
    } else {
     // just reference the given data 'as is'
        beginData = ptBuffer;
        cbSize = dataSize;
    }

    if (initiatio & ALLOCATE_NEW_COPY) {
        if ( isDataValid(beginData) && isDataValid(data) ) {
            for (uint i = 0; i < cbSize / 8; i++)
                ((unsigned long*)data)[i] = ((unsigned long*)beginData)[i];
        } else {
            data = Audio::Silence;
        }
    } else {
        data = beginData;
    }

    if (own)
        own = isDataValid( data );

    offset = cutend = 0;
}

WaveSpace(Audio)::Audio( unsigned sRate, unsigned bitDepth,
                         unsigned channels, unsigned framecount,
                         Initiatio flags )
    : Audio(flags)
{
    unsigned size = (framecount*((bitDepth >> 3)*channels));
    format = CreateWaveHeader(sRate, bitDepth, channels, size).AudioFormat;
    createBuffer( framecount*format.BlockAlign );
}

WaveSpace(Audio)::~Audio(void)
{
    if(own) drop();
}

void WaveSpace(Audio)::drop()
{
    if( mode.flags.group.chained && (!mode.hasFlag(LAST)) ) {
        if( isAudioPointerValid( nxt ) && (nxt != this) )
            delete nxt;
    }
    if( data )
    if( ptd == (size_t)this || ptd == NULL ) {
        if ( isDataValid( data ) && own ) {
            if (offset < 0) {
                resetOffset();
            } else {
                removeOffset();
            } removeEndcut();
            if (!mode.hasFlag(RAW_FILE_DATA)) {
                if (frameCount>1)
                    free(data);
            }
            data = Silence;
        }
    } else if (own) {
       ((Audio*)ptd)->mode.addFlag(OWN);
       data = Silence;
       cbSize = frameCount = offset = cutend = 0;
    }
}

WaveSpace(Data)
WaveSpace(Audio)::detachBuffer(void)
{
    Data tmp = data;
    data = Silence;
    return tmp;
}

WaveSpace(Audio)&
WaveSpace(Audio)::outscope(void)
{
    own = false;
    return this[0];
}

bool
WaveSpace(Audio)::isValid(void) const
{
    bool checkResult = false;
    try {
        checkResult = isAudioPointerValid(this);
    if (checkResult)
        checkResult = isDataValid(data);
    } catch (...) {
        checkResult = false;
    } return
        checkResult;
}

WaveSpace(Audio) // returns a compacted copy of the whole audio (e.g. merges all chained chunks into one new chunk without any offsets )
WaveSpace(Audio)::compacted(void) const
{
    const unsigned length = getLength();
    Audio cmpqtd( format, length );
    for ( unsigned i = 0; i < length; ++i )
        memcpy( cmpqtd[i], (*this)[i], format.BlockAlign );
    return cmpqtd.outscope();
}

//WaveSpace(Audio&) // compacts any chained sub-chunks (if there may be) into this chunk (if this chunks allocated buffer is too small, it will be reallocated )
void
WaveSpace(Audio)::compact(void)
{
    if ( mode.flags.group.chained || offset != 0 || cutend != 0 ) {
        const unsigned length = getLength();
        Audio cmpqtd( format, length );
        int i = 0;
        cmpqtd.mode.flags.all = firstChunk()->mode.flags.all;
        byte* end = cmpqtd.end<byte>();
        for ( byte* dst = (byte*)cmpqtd.data; dst < end; dst += format.BlockAlign )
            memcpy( dst, (*this)[i], format.BlockAlign );
        for (Audio* current = firstChunk(); current; current = current->nextChunk())
            if (current->mode.flags.group.owned) { current->drop();
                if (current != this) delete current;
            } 
        this->mode.flags.all = cmpqtd.mode.flags.all;
        this->mode.flags.group.chained = false;
        this->frameCount = cmpqtd.frameCount;
        this->cbSize = cmpqtd.cbSize;
        this->offset = 0;
        this->cutend = 0;
        this->nxt = NULL;
        this->data = cmpqtd.data;
        cmpqtd.mode.remFlag(OWN);
        this->mode.setFlags(OWN);
    }
}

WaveSpace(Audio) // creates a new Audio and clones this ones (non-chained) data into.
WaveSpace(Audio)::copy( void )
{
    Audio cpy(*const_cast<Audio*>(this),OWN|ALLOCATE_NEW_COPY);
    cpy.mode.flags.all = this->mode.flags.all;
    cpy.mode.setFlags(OWN);
    return cpy.outscope();
}

unsigned
WaveSpace(Audio)::frameTypeCode( void ) const
{
    return AUDIOFRAME_CODE( format.BitsPerSample, format.NumChannels, format.PCMFormatTag );
}

double
WaveSpace(Audio)::sampleTypeMax( void ) const
{
    switch ( format.BitsPerSample ) {
    case 8:  return format.PCMFormatTag ?  s8_MAX : i8_MAX;
    case 16: return format.PCMFormatTag ? s16_MAX : i16_MAX;
    case 24: return format.PCMFormatTag ? s24_MAX : i24_MAX;
    case 32:
    case 64: return 1;
    default: throw std::exception( SAMPLETYPE_MISSMATCH );
    }
}

double
WaveSpace(Audio)::sampleType0db( void ) const
{
    switch ( format.BitsPerSample ) {
    case 8:  return format.PCMFormatTag ?  s8_0DB : i8_0DB;
    case 16: return format.PCMFormatTag ? s16_0DB : i16_0DB;
    case 24: return format.PCMFormatTag ? s24_0DB : i24_0DB;
    case 32:
    case 64: return 0;
    default: throw std::exception( SAMPLETYPE_MISSMATCH );
    }
}

double
WaveSpace(Audio)::sampleTypeMin(void) const
{
    switch ( format.BitsPerSample ) {
    case 8:  return format.PCMFormatTag ?  s8_MIN : i8_MIN;
    case 16: return format.PCMFormatTag ? s16_MIN : i16_MIN;
    case 24: return format.PCMFormatTag ? s24_MIN : i24_MIN;
    case 32:
    case 64: return 0;
    default: throw std::exception( SAMPLETYPE_MISSMATCH );
    }
}

BEGIN_WAVESPACE
static void
audio_fade_function( Audio* audio, unsigned position,
                     double duration, double targetValue = 0 )
{
    const int numSamples = (int)( duration
                                * audio->format.ByteRate
                                / audio->format.BlockAlign );

    const double change = (1.0 - targetValue) / numSamples;
    int targetFrame = position + numSamples;
    double amplification = targetFrame > position
                         ? targetValue : 1;

    if( position > targetFrame ) {
        targetFrame = position;
        position += numSamples;
    }

    FRAMETYPE_SWITCH( audio->frameTypeCode(),
        for ( unsigned f = position; f <= targetFrame; ++f ) {
            CASE_TYPE::FRAME* fr = audio->template buffer<CASE_TYPE::FRAME>(f);
            for ( unsigned c = 0; c < audio->format.NumChannels; ++c )
                fr->channel[c] = CASE_TYPE::TY( fr->channel[c] * amplification );
            amplification += change;
        }
    )
}
ENDOF_WAVESPACE

static bool
audio_trim_function( float trimin, float trimout, double threshold,
                     WaveSpace(Audio*) audio )
{
    if ( isnan<float>(trimin) ) {
        if ( audio->trimIn(threshold).isValid() )
            return audio->trimOut(threshold).isValid();
    } else if ( audio->trimIn( threshold, trimin ).isValid() ) {
        return audio->trimOut( threshold, isnan<float>(trimout)
             ? trimin * 3
             : trimout ).isValid();
    } return false;
}

bool
WaveSpace(Audio)::trimmi( double threshold, float duration )
{
    threshold *= sampleTypeMax();
    const uint end = getLength();
#define PerTypeAction(T) {\
    const T maximum = T(sampleType0db()+threshold); \
    const T minimum = T(sampleType0db()-threshold); \
    for( uint position = 0; position < end; ++position ) { \
        const T* frame = buffer<T>( position ); \
        const T* topsp = frame+format.NumChannels; \
        for( ; frame != topsp; ++frame ) { \
            const T sample = *frame; \
            if ((sample > maximum) || (sample < minimum)) { \
                audio_fade_function( this, position, duration, 0 ); \
                addOffset( position ); \
                return isValid(); \
            } \
        } \
    } } break
    SAMPLETYPE_SWITCH( format )
#undef PerTypeAction
    return isValid();
}

bool
WaveSpace(Audio)::trimmo( double threshold, float duration )
{
    duration = duration > 0 ? -duration : 0;
    threshold *= sampleTypeMax();
    const uint end = getLength();
#define PerTypeAction(T) {\
    T maximum = T(sampleType0db()+threshold); \
    T minimum = T(sampleType0db()-threshold); \
    for ( int reverse = end - 1; reverse >= 0; --reverse ) { \
        const T* davor = buffer<T>(reverse)-1; \
        const T* frame = davor+format.NumChannels; \
        for( ; frame != davor; --frame ) { \
            const T sample = *frame; \
            if ((sample > maximum) || (sample < minimum)) { \
                audio_fade_function( this, reverse, duration, 0 ); \
                Audio* curCnk = getChunk( reverse ); \
                curCnk->addEndcut( (end - reverse) - 1 ); \
                return isValid(); \
            } \
        } \
    } } break
    SAMPLETYPE_SWITCH( format )
#undef PerTypeAction
    return isValid();
}

// Audio.trimIn(threshold,duration):
// Will remove leading silences from the beginning of an 'Audio'.
// Samples below 'threshold' parameter are treated being 'Silence'.
// To ensure 'Audio' begins by ZERO sample, a short 'fade in' time
// can be set by giving a 'duration' parameter greater zero seconds.
WaveSpace(Audio&)
WaveSpace(Audio)::trimIn( double threshold, float duration )
{
    trimmi( threshold, duration );
    return *this;
}

// Audio.trimOut(threhold,duration)
// Does same as 'trimIn()' but removes silence from the end.
// Then fades 'out' over 'duration' seconds to the new end.
WaveSpace(Audio&)
WaveSpace(Audio)::trimOut( double threshold, float duration )
{
    trimmo( threshold, duration );
    return *this;
}

// Audio.trimed(threshold,duration)
// Does both (trimIn and trimOut) at once
WaveSpace(Audio&)
WaveSpace(Audio)::trim( double threshold, float trimin, float trimout )
{
    if( !audio_trim_function( trimin, trimout, threshold, this ) )
        throw std::exception( TRIMMING_ERROR );
    return *this;
}

WaveSpace(Audio)
WaveSpace(Audio)::trimed( double threshold, float trimin, float trimout ) const
{
    Audio cpy = compacted();
    cpy.mode = mode;
    cpy.mode.addFlag(OWN);
    if( !audio_trim_function( trimin, trimout, threshold, &cpy ) )
        throw std::exception( TRIMMING_ERROR );
    return cpy.outscope();
}




// converts audio buffer to different frame type
// defined by given 'typeCode' parameter. if the
// given type differes from the audio's actual
// frame type, then a new audio buffer at target
// frame type, containing converted data will be
// created and returned. (actual buffer won't be
// over written). If given typeCode may be equal
// to current audio buffer's type code, then it
// just will be returned 'as is' without any
// conversion taking place.
WaveSpace(Audio)
WaveSpace(Audio)::converted( unsigned typeCode, double ampl ) const
{
    typeCode = (typeCode & 0x0000ffffu);
    FRAMETYPE_SWITCH( typeCode,
        return converted<CASE_TYPE>(ampl).outscope();
    )
}
WaveSpace(Audio)
WaveSpace(Audio)::converted( int bitDepth, int channelCount, double ampl ) const
{
    int sign = PCMs;
    if (bitDepth < 0) sign = 0;
    else if (bitDepth >= 32) sign = PCMf;
    FRAMETYPE_SWITCH( AUDIOFRAME_CODE( bitDepth, channelCount, sign ),
        return converted<CASE_TYPE>(ampl).outscope();
    )
}
WaveSpace(Audio)
WaveSpace(Audio)::converted(Format* ptFormat, double ampl) const
{
    FRAMETYPE_SWITCH(
        AUDIOFRAME_CODE(ptFormat->BitsPerSample, ptFormat->NumChannels, ptFormat->PCMFormatTag),
        return converted<CASE_TYPE>(ampl).outscope();
    );
}

WaveSpace(Audio&)
WaveSpace(Audio)::convert( unsigned typeCode, double ampl )
{
    uint ownFrametypecode = frameTypeCode();
    if ( typeCode != ownFrametypecode || ampl != 1 ) {
        Audio temp = this->converted( typeCode, ampl );
        if( isDataValid(this->data) && this->own )
            free( this->data );
        this->data = temp.data;
        this->format = temp.format;
        this->cbSize = temp.cbSize;
        this->frameCount = temp.frameCount;
        this->offset = 0;
        this->cutend = 0;
        this->own = true;
        temp.own = false;
    } return this[0];
}
WaveSpace(Audio&)
WaveSpace(Audio)::convert(unsigned short bitDepth, unsigned short channelCount, double ampl)
{
    int sign = PCMs;
    if (bitDepth < 0) sign = 0;
    else if (bitDepth >= 32) sign = PCMf;
    return convert(AUDIOFRAME_CODE(bitDepth, channelCount,sign), ampl);
}
WaveSpace(Audio&)
WaveSpace(Audio)::convert(Format* targetFmt, double ampl)
{
    return convert(AUDIOFRAME_CODE(targetFmt->BitsPerSample, targetFmt->NumChannels, targetFmt->PCMFormatTag), ampl);
}

WaveSpace(Audio)
WaveSpace(Audio)::amplified(float amplificator) const
{
    return Audio(*this,amplificator).outscope();
}
WaveSpace(Audio&)
WaveSpace(Audio)::amplify(float amplificator)
{
    return convert( frameTypeCode(), amplificator );
}

WaveSpace(Audio)
WaveSpace(Audio)::paned(Panorama panorama, Panorama::Axis autoAxting)
{
    Audio p(format,getLength());
    FRAMETYPE_SWITCH( frameTypeCode(),
        CASE_TYPE::FRAME* dst = p.buffer<CASE_TYPE::FRAME>();
    for (uint frame = 0; frame < p.frameCount; frame++)
        *(dst++) = ((CASE_TYPE::FRAME*)((*this)[frame]))->paned( panorama, Panorama::Axis(3-autoAxting) );
    )
    return p.outscope();
}

WaveSpace(Audio&)
WaveSpace(Audio)::pan(Panorama panorama, Panorama::Axis autoAxting)
{
    FRAMETYPE_SWITCH( frameTypeCode(),
        const CASE_TYPE::FRAME* end = this->end<CASE_TYPE::FRAME>();
    for (CASE_TYPE::FRAME* frame = this->buffer<CASE_TYPE::FRAME>(); frame != end; frame++)
        frame->pan(panorama, Panorama::Axis(3-autoAxting));
    )
    return *this;
}

// returns new allocated copy of byte order inversed samples
WaveSpace(Audio)
WaveSpace(Audio)::byteOrderReversed( void )
{

    if( format.BitsPerSample == 8 ) {
        return this->compacted();
    }
    Audio revOrdered(format,getLength());
    const uint bytePerSample = (format.BitsPerSample/8);
    const uint end = getLength();
    for( uint i = 0; i < end; i++ ) {
        byte* srcFrame = (byte*)(*this)[i];
        byte* dstFrame = (byte*)revOrdered[i];
        for (int c = 0; c < format.NumChannels; ++c) {
            const int channel = bytePerSample * c;
            int bd = channel+bytePerSample;
            for( int bs = 0; bs < bytePerSample; ++bs )
                dstFrame[--bd] = srcFrame[channel+bs];
        }
    } return revOrdered.outscope();
}

// reverse byte order of each sample contained in this 'Audio'
WaveSpace(Audio&)
WaveSpace(Audio)::reverseByteOrder( void )
{
    ReverseByteOrder( data, format, frameCount );
    return *this;
}

bool
WaveSpace(Audio)::isCompact(void) const
{
    return (chunkCount() == 1)
        && (offset == 0) && (cutend == 0);
}

unsigned
WaveSpace(Audio)::getLength(void) const
{
    if (mode.hasFlag(CIRCULAR))
        return frameCount;
    Audio* current = firstChunk();
    unsigned total = ( current->frameCount
                   - ((current->offset < 0
                     ?-current->offset
                     : current->offset)
                     + current->cutend ) );
    while ( current = current->nextChunk() )
        total += ( current->frameCount
               - ((current->offset < 0
                 ?-current->offset
                 : current->offset)
                 + current->cutend ) );
    return total;
}

unsigned
WaveSpace(Audio)::getDataSize(void) const
{
    return getLength() * format.BlockAlign;
}

int
WaveSpace(Audio)::chunkSize(void) const
{
    return ( frameCount - ( (offset < 0 ? -offset : offset) + cutend ) ) * format.BlockAlign;
}

double
WaveSpace(Audio)::getDuration(void) const
{
    return (double)getLength() / format.SampleRate;
}

WaveSpace(Audio*)
WaveSpace(Audio)::getChunk(unsigned atFramePosition) const
{
    uint total = 0;
    Audio* current = firstChunk();
    do { total += ( current->frameCount
                - ((current->offset < 0
                  ?-current->offset
                  : current->offset)
                  + current->cutend) );
        if( total >= atFramePosition ) {
            return current;
        }
    } while (
        current = current->nextChunk()
    ); return NULL;
}

WaveSpace(Audio*)
WaveSpace(Audio)::lastChunk(void) const
{
    const Audio* last = this;
    if (mode.flags.group.chained) {
        while (!last->mode.hasFlag(LAST))
            last = last->nextChunk();
    } return const_cast<Audio*>(last);
}

WaveSpace(Audio*)
WaveSpace(Audio)::nextChunk(void) const
{
    if ( this != nxt ) {
        if ( mode.hasFlag( LAST ) )
            return NULL;
    } return nxt;
}

WaveSpace(Audio*)
WaveSpace(Audio)::firstChunk(void) const
{
    const Audio* first = this;
    if ( mode.flags.group.chained ) {
        while ( !first->mode.hasFlag( FIRST ) )
            first = first->nxt;
    } return const_cast<Audio*>(first);
}

int
WaveSpace(Audio)::chunkCount(void) const
{
    if (!isDataValid(data))
        return 0;
    int count = 1;
    if (!mode.flags.group.chained)
        return count;
    Audio* chunk = firstChunk();
    while ( (!chunk->mode.hasFlag( LAST ) ) && chunk->nxt ) {
        chunk = chunk->nxt;
        ++count;
    } return count;
}

int
WaveSpace(Audio)::chunkIndex(void) const
{
    int index = 0;
    if (mode.flags.group.chained) {
        const Audio* chunk = firstChunk();
        while ( chunk != this ) {
            index++; chunk = chunk->nxt;
        }
    } return index;
}

WaveSpace(Audio*)
WaveSpace(Audio)::chunkAtIndex( int idx ) const
{
    if (!mode.flags.group.chained)
        return (!idx)
             ? const_cast<Audio*>(this)
             : (Audio*)Silence;
    Audio* chunk = firstChunk();
    while ( chunk && (idx-- >= 0) ) chunk = chunk->nextChunk();
    return idx < 0 ? chunk : NULL;
}

WaveSpace(Audio&)
WaveSpace(Audio)::operator= (Data raw)
{
    if (!Audio::isDataValid(raw)) {
        setSilent();
    } else {
        if (isDataValid(data) && own)
            free( data );
        data = raw;
    } return *this;
}

void
WaveSpace(Audio)::setSilent(void)
{
    if (isDataValid(data) && own)
        free( data );
    data = Silence;
    cbSize = (frameCount = 1)*format.BlockAlign;
}

bool
WaveSpace(Audio)::isSilent(void) const
{
    if ( this != Silence )
        return data == Silence;
    return true;
}

const WaveSpace(Audio)::Data
WaveSpace(Audio)::chunkData(void) const
{
    return (const Data)( (byte*)data + ( (offset > 0 ? offset : 0) * format.BlockAlign ) );
}

static uint _copyChunkWise( WaveSpace(Audio)* dst, WaveSpace(Audio)* src )
{
    uint cpybytes = 0;
    WaveSpace(Audio)* current = src->firstChunk();
    do { uint chunkbytes = current->chunkSize();
        MEMCPYFUNC( &dst->buffer<byte>()[cpybytes], current->chunkData(), 1, chunkbytes );
        cpybytes += chunkbytes;
    } while ( current = current->nextChunk() );
    return cpybytes;
}

WaveSpace(Audio)
WaveSpace(Audio)::operator + ( Audio other )
{
    uint cpybytes = 0;
    Audio merged( format, getLength() + other.getLength() );

    if (mode.flags.group.chained) {
        cpybytes = _copyChunkWise( &merged, this );
    } else {
        MEMCPYFUNC(merged.data, buffer<byte>(), format.BlockAlign, cpybytes = getLength());
        cpybytes *= format.BlockAlign;
    } merged.addOffset( getLength() );

    if ( (other.format == this->format) != DIRECTCOPY ) {
        Audio conv = other.converted( frameTypeCode() );
        other.mode.remFlag(OWN);
        other = conv;
        other.takeOwnership();
    }
    if ( other.mode.flags.group.chained ) {
        cpybytes += _copyChunkWise( &merged, &other );
    } else {
        MEMCPYFUNC(merged.buffer<byte>(), other.buffer<byte>(), format.BlockAlign, other.getLength());
        cpybytes += other.getLength()*format.BlockAlign;
    } merged.removeOffset();
    return merged.outscope();
}

WaveSpace(Audio&)
WaveSpace(Audio)::operator += ( Audio& other )
{
    Audio* last = lastChunk();

    if (frameTypeCode() != other.frameTypeCode())
        last->nxt = new Audio(other.converted(this->frameTypeCode()));
    else
        last->nxt = new Audio(other);

    last->nxt->nxt = firstChunk();
    if (last->nxt->nxt->mode.hasFlag(CIRCULAR)) {
        last->nxt->mode.addFlag(SUBCHUNK);
    } else {
        last->nxt->nxt->mode.addFlag(FIRST);
        last->nxt->mode.addFlag(LAST);
        last->mode.remFlag(LAST);
    } return *this;
}

WaveSpace(Audio&)
WaveSpace(Audio)::append(Audio other)
{
    if ( canBeTransfaredBy( INTERLEAVE, other.format ) )
        other.format.SampleRate = this->format.SampleRate;
    return operator +=(other);
}

void
WaveSpace(Audio)::setLength( unsigned newLength )
{
    removeEndcut();
    if( newLength > frameCount ) {
        lastChunk()->nxt = new Audio( format, (newLength-frameCount) );
    } else {
        addEndcut( frameCount - newLength );
    }
}

WaveSpace(FormatMatch)
WaveSpace(Audio)::operator ==(const Audio& other)
{
    return this->format == other.format;
}
WaveSpace(FormatMatch)
WaveSpace(Audio)::operator !=(const Audio& other)
{
    return this->format != other.format;
}


WaveSpace(FormatMatch)
WaveSpace(Audio)::canTranfareTo(const Audio& other) const
{
    return this->format == other.format;
}
bool
WaveSpace(Audio)::canBeTransfaredBy(FormatMatch transfareMode, const Format& toOther) const
{
    return (format == toOther) <= transfareMode;
}

void
WaveSpace(Audio)::setFormat(const Format& newFormat, bool convertIfNotMatching)
{
    if ( !canBeTransfaredBy( INTERLEAVE, newFormat ) ) {
        if ( convertIfNotMatching ) {
            convert( (Format*)&newFormat );
        } else {
            resetOffset();
            format = newFormat;
            frameCount = ( cbSize / format.BlockAlign );
        }
    } else format.SampleRate = newFormat.SampleRate;
}

WAVELIB_API WaveSpace(IAudioFrame*)
WaveSpace(CreateAudioFrame)(uint typecode)
{
    FRAMETYPE_SWITCH( typecode, return (IAudioFrame*)new CASE_TYPE(); )
    return NULL;
}

WaveSpace(IAudioFrame*)
WaveSpace(CreateAudioFrame)( uint typecode, Data rawdat )
{
    IAudioFrame* neuframen = CreateAudioFrame( typecode );
    if (neuframen) {
        neuframen->Mix( rawdat, 1.0f );
    } return neuframen;
}

WAVELIB_API WaveSpace(IAudioFrame*)
WaveSpace(CreateManyFrames)(uint typecode, uint ammount)
{
    FRAMETYPE_SWITCH( typecode, return (IAudioFrame*)(CASE_TYPE*)(CASE_TYPE::FRAME*)new CASE_TYPE::FRAME::TY[ammount*CASE_TYPE::FRAME::CH]; )
    return NULL;
}

WaveSpace(IAudioFrame*)
WaveSpace(AudioFrameType)::CreateFrame( Data rawdat ) const
{
    IAudioFrame* neuframen = CreateAudioFrame( info.type.code );
    if( neuframen ) {
        neuframen->Mix( rawdat, 1.0f );
    } return neuframen;
}

double
WaveSpace(IAudioFrame)::SampleTypeMaxValue(void) const
{
    word typcode = GetFrameType().Code();
    if ( AUDIO_FLOATTYPE_TYPECODE( typcode ) ) {
        return 1.0;
    } else {
        FRAMETYPE_SWITCH( typcode, return std::numeric_limits<CASE_TYPE::TY>::max(); )
    }
}

double
WaveSpace(IAudioFrame)::SampleTypeMinValue(void) const
{
    word typcode = GetFrameType().Code();
    if (AUDIO_FLOATTYPE_TYPECODE(typcode)) {
        return -1.0;
    }
    else {
        FRAMETYPE_SWITCH(typcode, return std::numeric_limits<CASE_TYPE::TY>::min(); )
    }
}

double
WaveSpace(IAudioFrame)::SampleType0dbValue(void) const
{
    word typcode = GetFrameType().Code();
    if (AUDIO_FLOATTYPE_TYPECODE(typcode)) {
        return 0.0;
    }
    else {
        FRAMETYPE_SWITCH(typcode, return (std::numeric_limits<CASE_TYPE::TY>::max() - std::numeric_limits<CASE_TYPE::TY>::min()) / 2.0; )
    }
}

WaveSpace(Audio&)
WaveSpace(Audio)::split( uint atPosition, bool trim, bool positionIsDuration )
{
    const uint len = getLength();
    double duration = getDuration();
    if (positionIsDuration)
        atPosition = (uint)( ( (double)atPosition / (duration*1000) ) * len );
    Audio* fragment = getChunk( atPosition );
    Audio* begining = firstChunk();
    Audio* endchunk = lastChunk();
    if( trim ) {
        audio_fade_function( fragment, atPosition, -0.01, 0);
        audio_fade_function( fragment, atPosition, 0.01, 0 );
    } byte* cutPoint = buffer<byte>( atPosition );
    const uint leftover = (uint)(size_t)(cutPoint - (byte*) fragment->chunkData() );
    const uint tailpart = fragment->chunkSize() - leftover;
    Audio* splitpart = new Audio( format, cutPoint, tailpart, DONT_ALLOCATE_NEW|SUBCHUNK );
    fragment->cbSize -= tailpart;
    fragment->cutend += tailpart;
    fragment->frameCount = leftover / format.BlockAlign;
    splitpart->mode.setFlags( DONT_ALLOCATE_NEW | SUBCHUNK );   
    if( fragment != endchunk ) {
        splitpart->append( fragment->nxt );
    }
    fragment->nxt = splitpart;
    if (fragment->mode.hasFlag(LAST)) {
        fragment->mode.remFlag(LAST);
        splitpart->mode.addFlag(LAST);
        splitpart->nxt = begining;
        begining->mode.addFlag(FIRST);
    }
    return *begining;
}