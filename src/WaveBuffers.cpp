/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveBuffer.cpp                             ||
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
#include <WaveLib.inl/int24bittypes.hpp>

#include "WaveMacros.hpp"
#include "Panorama.hpp"
#include "WaveFormat.hpp"
#include "AudioChannel.hpp"

#include "WaveAudio.hpp"
#include "WaveStream.hpp"

#include "WaveBuffers.hpp"
#endif



#ifdef  THREAD_WAITCYCLE_FUNC

#define log_lock_state(msg)
#define log_wait_state(cyc)

#define do_wait_cycles(cycles) { log_wait_state(cycles) THREAD_WAITCYCLE_FUNC( cycles * THREAD_WAITCYCLE_TIME ); }

#define enter_locked_scope( dirpos ) do if( lock( dirpos ) ) { 
#define write_scope_return( value ) unlock( write ); return value
#define read_locked_return( value ) unlock( read ); return value
#define close_locked_scope( timout ) } else do_wait_cycles( timout ) while( true );
#define  read_locked_scope  enter_locked_scope( read ) log_lock_state("read")
#define write_locked_scope  enter_locked_scope( write ) log_lock_state("write")

#else

#define enter_locked_scope( dirpos )
#define write_scope_return( value ) return value
#define read_locked_return( value ) return value
#define close_locked_scope( timout )
#define  read_locked_scope
#define write_locked_scope

#endif


//////////////////////////////////////////////////////////////
// AudioStream


WaveSpace(Format*)
WaveSpace(AudioStream)::GetFormat(void) const
{
    return &const_cast<AudioStream*>(this)->format;
}

word
WaveSpace(AudioStream)::GetTypeCode(void) const
{
    return frameTypeCode();
}

uint
WaveSpace(AudioStream)::GetLength(void) const
{
    return getLength();
}

WaveSpace(StreamDirection)
WaveSpace(AudioStream)::GetDirection(void) const
{
    return StreamDirection::INOUT;
}

uint
WaveSpace(AudioStream)::GetPosition(StreamDirection dir) const
{
    if (dir == StreamDirection::READ) {
        return this->read; // / format.BlockAlign;
    } else {
        return this->write; // / format.BlockAlign;
    }
}

uint
WaveSpace(AudioStream)::GetDuration(void) const
{
    return uint( ((float)format.SampleRate/GetPosition(WRITE))*1000 );
}

uint
WaveSpace(AudioStream)::GetWritePosition(void) const
{
    return this->write; // / format.BlockAlign;
}

uint
WaveSpace(AudioStream)::GetReadPosition(void) const
{
    return this->read; // / format.BlockAlign;
}

uint
WaveSpace(AudioStream)::framesReadable(void) const
{
    int fr = getLength();
    return  fr - (int)GetReadPosition(); // +(((fr - (int)GetReadPosition()) + (int)GetWritePosition()) % fr);
}

uint
WaveSpace(AudioStream)::framesWritable(void) const
{
    int fr = getLength();
    return fr - (int)GetWritePosition();
}

void
WaveSpace(AudioStream)::Seek( uint positionAsFrames, StreamDirection direction )
{
    if ( direction & READ ) {
    read_locked_scope
        this->read = positionAsFrames;
        read_locked_return();
    close_locked_scope(1)
    } else
    if ( direction & WRITE ) {
    write_locked_scope
        this->write = positionAsFrames;
        write_scope_return();
    close_locked_scope(1) }
}

uint
WaveSpace(AudioStream)::Write( Audio::Data send, uint cbsize, uint offset )
{ write_locked_scope

        byte* src = (byte*)send + offset;
        uint copybytes = minOf(nextBarrier(), cbsize);
        const int W = this->write * this->format.BlockAlign;
        byte* dst = (byte*)lastChunk()->data + (W % this->cbSize);
        MEMCPYFUNC(dst, src, 1, copybytes);
        if( fx ) fx->doFrames( dst, copybytes/ this->format.BlockAlign );
        if ( cbsize > copybytes ) {
            Audio chunk( this->format, this->cbSize / this->format.BlockAlign );
            dst = (byte*)chunk[0];
            MEMCPYFUNC( dst, src + copybytes, 1, cbsize - copybytes );
            if( fx ) fx->doFrames( dst, (cbsize-copybytes)/ this->format.BlockAlign );
            append( chunk );
        } this->write = ((W + cbsize) / this->format.BlockAlign);
        write_scope_return( cbsize );

close_locked_scope(2) }

uint
WaveSpace(AudioStream)::Write( Audio::Data send, int framecount )
{
    return Write( send, (framecount* this->format.BlockAlign), 0 ) / this->format.BlockAlign;
}

uint
WaveSpace(AudioStream)::Write( const Audio& send )
{
    uint copybytes = send.getLength() * this->format.BlockAlign;
    if ( this->canTranfareTo( send ) & DIRECTCOPY ) {
        const int chuncks = send.chunkCount();
        for ( const Audio* src = send.firstChunk(); src != NULL; src = src->nextChunk() )
            return Write( src->chunkData(), src->chunkSize(), 0 ) / this->format.BlockAlign;
    } else {
        return Write( send.converted( frameTypeCode() ) );
    }
}

uint
WaveSpace(AudioStream)::WriteFrame( Audio::Data frame )
{ write_locked_scope

    byte* dst = (byte*)(*this)[this->write++];
    byte* src = (byte*)frame;
    for ( int i = 0; i < this->format.BlockAlign; ++i ) {
        byte cpy = src[i];
        dst[i] = cpy;
    }
//    MEMCPYFUNC(dst, frame, format.BlockAlign, 1);
    if (fx) fx->doFrame(dst);
    write_scope_return( this->format.BlockAlign );

close_locked_scope(2) }

uint
WaveSpace(AudioStream)::WriteChannel(int chandex, Audio::Data send, int samplescount)
{ write_locked_scope

        ensureSize( samplescount* this->format.BlockAlign );
        FRAMETYPE_SWITCH( AUDIOFRAME_CODE( this->format.BitsPerSample, this->format.NumChannels, this->format.PCMFormatTag ),
            CASE_TYPE::FRAME::TY* src = (CASE_TYPE::FRAME::TY*) send;
            for (int i = 0; i < samplescount; i++) {
                ((CASE_TYPE::FRAME*)(*this)[this->write + i])->channel[chandex] = fx? fx->doChannel(chandex,*src++) : *src++;
            } break;
        ) this->write += samplescount;
        write_scope_return( samplescount );

 close_locked_scope( 2 ) }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


uint
WaveSpace(AudioStream)::Read(Audio::Data receive, uint cbsize, uint offsetData)
{ read_locked_scope

    byte* dst = (byte*)receive + offsetData;
    const uint cbAvailable = (GetLength() - GetReadPosition())* this->format.BlockAlign;
    cbsize = minOf(cbsize, cbAvailable);
    if (cbsize > nextBarrier()) {
        const int end = (this->read + (cbsize / this->format.BlockAlign));
        for (int i = read; i < end; i++) {
            MEMCPYFUNC(dst, (*this)[i], 1, this->format.BlockAlign);
            dst += this->format.BlockAlign;
        }
    } else {
        MEMCPYFUNC(dst, buffer<byte>() + (this->read * this->format.BlockAlign), 1, cbsize);
    } this->read += (cbsize / this->format.BlockAlign);
    read_locked_return( cbsize );

close_locked_scope( 2 ) }


uint
WaveSpace(AudioStream)::Read(Audio& receive, uint framecount)
{ read_locked_scope
 
        receive.resetOffset();
        const uint available = getLength() - GetReadPosition();
        framecount = minOf( framecount, available );
        bool dstNeedsExtension = framecount > receive.frameCount;
        if ((framecount > (nextBarrier() / this->format.BlockAlign))
            || dstNeedsExtension) {
            if (dstNeedsExtension)
                receive += Audio(receive.format, framecount - receive.frameCount);
            for (int i = 0; i < framecount; i++)
                MEMCPYFUNC(receive[i], (*this)[i], 1, this->format.BlockAlign);
        } else {
            MEMCPYFUNC(receive[0], (*this)[0], this->format.BlockAlign, framecount);
        } this->read += framecount;
        read_locked_return( framecount );

close_locked_scope(2) }


WaveSpace(Audio)
WaveSpace(AudioStream)::Read(uint framecount)
{ read_locked_scope

    uint available = GetLength() - GetReadPosition();
    if ( available ) {
        framecount = minOf( framecount, available );
        Audio newBuffer( this->format, framecount );
        if ( framecount > (nextBarrier() / this->format.BlockAlign) ) {
            for (int i = 0; i < framecount; i++)
                MEMCPYFUNC(newBuffer[i], (*this)[i], 1, this->format.BlockAlign);
        } else {
            void* dst = newBuffer[0];
            void* src = (*this)[this->read / this->format.BlockAlign];
            MEMCPYFUNC(dst, src, this->format.BlockAlign, framecount);
        } read += framecount;
        read_locked_return( newBuffer.outscope() );
    } else
        read_locked_return( Silence );

close_locked_scope(2) }

uint
WaveSpace(AudioStream)::ReadChannel(int chandex, Audio::Data receive, int samplecount)
{ read_locked_scope

    const uint readFrames = GetReadPosition();
    samplecount = minOf(samplecount, readFrames);
    FRAMETYPE_SWITCH(
        AUDIOFRAME_CODE(format.BitsPerSample,format.NumChannels,format.PCMFormatTag),
        CASE_TYPE::FRAME::TY* dst = (CASE_TYPE::FRAME::TY*)receive;
        for (int i = 0; i < samplecount; i++) {
            *dst++ = ((CASE_TYPE::FRAME*)(*this)[readFrames + i])->channel[chandex];
        } break;
    )
    this->read += samplecount;
    read_locked_return( samplecount );

 close_locked_scope(2) }

uint
WaveSpace(AudioStream)::nextBarrier(void)
{
    return this->cbSize - ((this->read* this->format.BlockAlign) % this->cbSize);
}

int
WaveSpace(AudioStream)::ensureSize( uint byteCount )
{
    uint actualSize = getLength()* this->format.BlockAlign;
    int sizeneeded = ((this->write * this->format.BlockAlign) + byteCount) - actualSize;
    if (sizeneeded > 0) {
        Audio chunk( this->format, this->cbSize / this->format.BlockAlign );
        append( chunk );
        sizeneeded - this->cbSize;
        return ensureSize( sizeneeded );
    } return (int)this->cbSize + sizeneeded;
}


bool
WaveSpace(AudioStream)::isValid(void) const
{
    return Audio::isValid();// ((const Audio*)this)->isValid();
}


WaveSpace(AudioStream)
WaveSpace(AudioStream)::outscope(void)
{
    this->mode.flags.group.owned = false;
    return this[0];
}


//////////////////////////////////////////////////////////////
// Circular

WaveSpace(Audio)
WaveSpace(AbstractCycleBuffer)::getReadableAudio(void)
{
    uint currentWrite = this->write % this->frameCount;
    uint currentRead = this->read % this->frameCount;
    if (lock(this->read)) {
        uint chunkSize = ( currentWrite<=currentRead
                         ? (this->frameCount - currentRead)
                         : currentWrite );
        Audio readableBuffer( this->format, (*this)[this->read], chunkSize* this->format.BlockAlign, DONT_ALLOCATE_NEW);
        if ( (currentWrite <= currentRead) && currentWrite ) {
            readableBuffer.mode.addFlag(FIRST);
            readableBuffer.append(
                Audio( this->format, this->data, currentWrite* this->format.BlockAlign, DONT_ALLOCATE_NEW|LAST )
            );
        } this->read += chunkSize;
    unlock( this->read );
    return readableBuffer;
    } else
        return Audio::Silence;
}



WaveSpace(Circular)::Circular(Audio& initFrom)
    : AbstractCycleBuffer(initFrom)
{}

WaveSpace(Circular)::Circular(i16 typecode, uint framecount, int frq /*=44100*/ )
    : AbstractCycleBuffer(typecode, framecount, frq) {
    this->needCounterReset = this->isReading = this->isWriting = this->read = this->write = 0;
}

WaveSpace(Circular)::Circular(i16 typecode, Data allocatedBuffer, uint bufferSizeBytes, int frq /*=44100*/ )
    : AbstractCycleBuffer(typecode, allocatedBuffer, bufferSizeBytes, frq) {
    this->needCounterReset = this->isReading = this->isWriting = this->read = this->write = 0;
}

// read 'length' (or, if ommited 'available') count on frames
// into Audio 'dst'. if dst Audio may come at wrong fortmat,
// it will be set to match src format then. reading is threadsafe
// and will free these frames which where read being writable again.
// if one chunk need to be read several times, optional 'stealth'
// can be can be given 'true'. 'stealth' reading operations won't
// increase read position relative to the current write position.
// so same buffer can be read completely through several times
// without having to make it freed being writable again.
uint WaveSpace(Circular)::ReadFrames(Audio* dst, uint length /*=EMPTY*/, bool stealth /*=false*/) {
    uint count = 0;
    if( lock( this->read ) ) {
        count = dst->frameCount;
        dst->setFormat( this->format );
        if (length != EMPTY)
            count = dst->frameCount < length
                  ? dst->frameCount : length;
        count = count < availableFrames()
              ? count : availableFrames();
        uint readHead = this->read;
        for (uint frame = 0; frame < count; frame++)
            memcpy( (*dst)[frame], (*this)[readHead++], this->format.BlockAlign );
        if (!stealth) this->read = readHead;
    unlock( this->read );
    } return count;
}

// read 'count' on frames into a new allocated buffer of matching size
WaveSpace(Circular)::Audio WaveSpace(Circular)::ReadFrames(uint count, bool stealth /*=false*/) {
    count = count < availableFrames()
          ? count : availableFrames();
    Audio audio(this->format, count);
    if (ReadFrames(&audio,audio.frameCount,stealth))
        return audio.outscope();
    else
        return Audio::Silence;
}

// read 'cbLength' count on bytes to a memory block pointed by 'buffer'
uint WaveSpace(Circular)::Read( byte* buffer, uint cbOffset, uint cbLength, bool stealth /*=false*/ ) {
    uint count = 0;
    if (lock(this->read)) {
        cbLength = cbLength < availableFrames()
                 ? cbLength : availableFrames();
        const byte* end = buffer + (cbOffset + cbLength);
        uint readHead = this->read;
        for (byte* dst = buffer + cbOffset; dst != end; dst += this->format.BlockAlign)
            memcpy(dst, (*this)[readHead++], this->format.BlockAlign);
        count = cbLength* this->format.BlockAlign;
        if (!stealth) this->read = readHead;
    unlock(this->read);
    } return count;
}

// WriteFrames(Audio), WriteFrames(Audio,framecount)
// copy 'framecount' count on frames (or if ommited all frames) from 'src' into the cycle
// by passing these from the 'writable' range over into the 'now readable' range being readable.
uint WaveSpace(Circular)::WriteFrames( const Audio& src, uint length /*=EMPTY*/ ) {
    if (!src.isValid())
        throw std::exception( INVALID_AUDIODATA );
    uint count = 0;
    if (lock(this->write)) {
        count = src.frameCount < length
              ? src.frameCount : length;
        for (uint frame = 0; frame < count; frame++)
            memcpy((*this)[this->write++], src[frame], this->format.BlockAlign);
    unlock(this->write);
    } return count;
}

// WriteFrames(ptRawData,cbOffset,cbLength)
// copy 'cbLength' bytes, beginning at position 'cbOffset' from 'ptRawData'
// into the cycle and set cycle being readable for read opperations then
uint WaveSpace(Circular)::WriteFrames( Data buffer, uint cbOffset, uint cbLength ) {
    uint count = 0;
    if (lock(this->write)) {
        byte* inputData = (byte*)buffer;
        count = cbLength / this->format.BlockAlign;
        count = count < this->frameCount
              ? count : this->frameCount;
        const byte* end = inputData + (cbOffset + count);
        for (byte* src = inputData + cbOffset; src != end; src += this->format.BlockAlign)
            memcpy((*this)[this->write++], src, this->format.BlockAlign);
        count *= this->format.BlockAlign;;
    unlock( this->write );
    } return count;
}

// acheive a 'WriteAccesLock'. no other thread
// than the caller will be able writing into
// the cycle as long caller is holding the lock.
// caller can release lock by passing it back via
// 'releaseWriteLock(WriteAccessLock)' function.
//
// Also, during the lock is holden by some caller
// the buffer range where the lock applyes to, also
// isn't readable by any 'read' api calls. achieving a
// 'ReadAccessLock' (by getReadLock()) for this
// buffer range isn't possible as long the write
// lock is passed back for unlocking it.
WaveSpace(Circular)::WriteAccessLock WaveSpace(Circular)::getWriteLock(void) {
    if ( lock(this->write) ) return this->write;
    else return *(uint*)Audio::Silence;
}

// releases a lock which previously was
// achieved by 'getWriteLock()' function.
bool WaveSpace(Circular)::releaseWriteLock( WaveSpace(Circular)::WriteAccessLock key ) {
    return unlock(key);
}

