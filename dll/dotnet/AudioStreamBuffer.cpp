/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioStreamBuffer.cpp                      ||
||     Author:    Kalle                                      ||
||     Generated: 04.03.2019                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "AudioStreamBuffer.hpp"


Stepflow::Audio::AudioStreamBuffer::AudioStreamBuffer( const NATIVE& wrap ) 
{
    data = IntPtr::Zero;

    if( !wrap.isValid() )
        throw gcnew System::Exception("invalid audio");

    NATIVE* AU = new NATIVE( wrap );

    if( !AU->isValid() ) 
        throw gcnew System::Exception("invalid audio");

    au = AU;
}

Stepflow::Audio::Audio^ Stepflow::Audio::AudioStreamBuffer::ReadAll(void)
{
    NATIVE* AU = reinterpret_cast<NATIVE*>(au);
    uint positionStored = AU->GetReadPosition();
    AU->Seek( 0, stepflow::StreamDirection::READ );
    Audio^ clone = Read();
    AU->Seek( positionStored, stepflow::StreamDirection::READ );
    return clone;
}

Stepflow::Audio::Audio^ Stepflow::Audio::AudioStreamBuffer::Read(void)
{
    NATIVE* AU = reinterpret_cast<NATIVE*>(au);
    return gcnew AudioBuffer( AU->Read( AU->getLength() ) );
}

uint Stepflow::Audio::AudioStreamBuffer::Read( Audio^ buffer, int numberFrames )
{
    return reinterpret_cast<NATIVE*>(au)->Read( buffer->native_cast(), numberFrames );
}

uint Stepflow::Audio::AudioStreamBuffer::Read( Audio^ buffer )
{
    return reinterpret_cast<NATIVE*>(au)->Read( buffer->native_cast() );
}

Stepflow::Audio::IAudioFrame^ Stepflow::Audio::AudioStreamBuffer::ReadFrame(void)
{
    MANAGEDTYPE_SWITCH( au->frameTypeCode(),
        CASE_TYPE::NATIVE fr = reinterpret_cast<NATIVE*>(au)->ReadFrame<CASE_TYPE::NATIVE>();
    return *(CASE_TYPE*)&fr; )
}

Stepflow::Audio::Audio^ Stepflow::Audio::AudioStreamBuffer::Read( int FrameCount )
{
    return gcnew AudioBuffer( reinterpret_cast<NATIVE*>(au)->Read( FrameCount ) );
}

uint Stepflow::Audio::AudioStreamBuffer::AvailableBytes(void)
{
    return CanStream(StreamDirection::INPUT) * au->format.BlockAlign;
}

uint Stepflow::Audio::AudioStreamBuffer::AvailableFrames(void)
{
    return CanStream(StreamDirection::INPUT);
}

System::TimeSpan Stepflow::Audio::AudioStreamBuffer::AvailableTime(void)
{
    return TimeSpan::FromMilliseconds(((float)au->format.SampleRate / AvailableFrames()) * 1000);
}

array<Stepflow::Audio::IAudioFrame^>^ Stepflow::Audio::AudioStreamBuffer::ReadFrames(uint count)
{
    stepflow::IReadLockable::ReadAccessLock read = reinterpret_cast<NATIVE*>(au)->getReadLock();
    if (&read == stepflow::Audio::Silence) return nullptr;
    uint available = AvailableFrames();
    count = count < available
          ? count : available;
    array<IAudioFrame^>^ buffer = gcnew array<IAudioFrame^>(count);
    for (int i = 0; i < count; ++i) {
        buffer[i] = ReadFrame();
    } reinterpret_cast<NATIVE*>(au)->releaseReadLock( read );
    return buffer;
}

uint Stepflow::Audio::AudioStreamBuffer::Read(IntPtr dstMem, int countBytes, int offsetDstBytes)
{
    return reinterpret_cast<NATIVE*>(au)->Read( dstMem.ToPointer(), countBytes, offsetDstBytes );
}
uint Stepflow::Audio::AudioStreamBuffer::Write(IntPtr rawData, int countBytes, int offsetSrcBytes)
{
    return reinterpret_cast<NATIVE*>(au)->Write( rawData.ToPointer(), countBytes, offsetSrcBytes );
}

uint Stepflow::Audio::AudioStreamBuffer::WriteFrame( stepflow::s16 sample )
{
    if(au->format.PCMFormatTag == PCMs && au->format.BitsPerSample == 16 )
        return WriteFrame( GetFrameType().CreateEmptyFrame()->Mix( sample, Panorama::Neutral ) );
    return 0;
}

uint Stepflow::Audio::AudioStreamBuffer::WriteFrame( stepflow::f32 sample )
{
    return WriteFrame( GetFrameType().CreateEmptyFrame()->Mix( sample, Panorama::Neutral ) );
}

uint Stepflow::Audio::AudioStreamBuffer::WriteFrame( stepflow::s16 sample, Panorama mixer)
{
    return WriteFrame( GetFrameType().CreateEmptyFrame()->Mix(sample, mixer) );
}

uint Stepflow::Audio::AudioStreamBuffer::WriteFrame(stepflow::f32 sample, Panorama mixer)
{
    return WriteFrame( GetFrameType().CreateEmptyFrame()->Mix(sample, mixer) );
}

uint Stepflow::Audio::AudioStreamBuffer::WriteFrame(stepflow::f64 sample, Panorama mixer)
{
    return WriteFrame( GetFrameType().CreateEmptyFrame()->Mix(sample, mixer) );
}

unsigned Stepflow::Audio::AudioStreamBuffer::WriteAudio(Audio^ buffer)
{
    return ((NATIVE*)au)->Write( buffer->native_cast() );
}

unsigned Stepflow::Audio::AudioStreamBuffer::Write( Audio^ buffer, int countOnFs, int FsOffsetInSrc )
{
    stepflow::Audio src = buffer;
    src.applyOffset( FsOffsetInSrc );
    src.setLength( countOnFs );
    return reinterpret_cast<NATIVE*>(au)->Write( src );
}

uint Stepflow::Audio::AudioStreamBuffer::WriteSample( short sample )
{
    return reinterpret_cast<NATIVE*>(au)->Write( &sample, 2, 0 );
}
uint Stepflow::Audio::AudioStreamBuffer::WriteSample( float sample )
{
    return reinterpret_cast<NATIVE*>(au)->Write( &sample, 4, 0 );
}



unsigned Stepflow::Audio::AudioStreamBuffer::WrittenBytes(void)
{
    NATIVE* AU = reinterpret_cast<NATIVE*>(au);
    return AU->GetWritePosition() * AU->format.BlockAlign;
}

unsigned Stepflow::Audio::AudioStreamBuffer::WrittenFrames(void)
{
    return reinterpret_cast<NATIVE*>(au)->GetWritePosition();
}

System::TimeSpan Stepflow::Audio::AudioStreamBuffer::WrittenTime(void)
{
    NATIVE* AU = reinterpret_cast<NATIVE*>(au);
    return TimeSpan::FromMilliseconds((1.0 / AU->GetFormat()->SampleRate) * AU->GetWritePosition());
}



uint Stepflow::Audio::AudioStreamBuffer::WriteFrame( IAudioFrame^ frame )
{
    return reinterpret_cast<NATIVE*>(au)->WriteFrame( frame->GetRaw().ToPointer() );
}
