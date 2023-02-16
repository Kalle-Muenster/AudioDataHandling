#include "AudioBuffer.hpp"
#include "CircularBuffer.hpp"

using namespace stepflow;


Stepflow::Audio::AudioBuffer::AudioBuffer(stepflow::Audio& wrap)
{
    au = new stepflow::Audio(wrap);
	if (!au->isValid()) {
        delete au;
		throw gcnew System::Exception("invalid audio");
    } 
}

Stepflow::Audio::AudioBuffer::AudioBuffer(uint sizeInBytes)
{
	au = new stepflow::Audio(sizeInBytes);
}
Stepflow::Audio::AudioBuffer::AudioBuffer(PcmFormat fmt, uint framecount)
{
	au = new stepflow::Audio(*(stepflow::Format*)&fmt, framecount);
}
Stepflow::Audio::AudioBuffer::AudioBuffer(AudioFrameType type, System::Array^ data)
{
	uint length = data->Length;
    MANAGEDTYPE_SWITCH( type.Code,
		pin_ptr<CASE_TYPE> pt = &((array<CASE_TYPE>^)data)[0];
    au = new stepflow::Audio((stepflow::Audio::Data)pt, type.BlockAlign*length, Initiatio(DONT_ALLOCATE_NEW | OWN));
	);
}

Stepflow::Audio::AudioBuffer::AudioBuffer(int frq, int bit, int chn, uint len)
{
	au = new stepflow::Audio(frq, bit, chn, len);
}

Stepflow::Audio::AudioBuffer::AudioBuffer(AudioFrameType frameType, uint sampleRate, uint frameCount )
	: AudioBuffer( sampleRate, frameType.BitDepth, frameType.ChannelCount, frameCount )
{}

Stepflow::Audio::AudioBuffer::AudioBuffer(int frq, PcmTag pcm, int bit, int chn, System::IntPtr data, uint size, Flags flags)
{
    pin_ptr<byte> rawBufferData = (byte*)data.ToPointer();
	au = new stepflow::Audio(rawBufferData, size, (Initiatio)(uint)flags);
	au->format.PCMFormatTag = WAV_PCM_TYPE_ID( pcm );
	au->format.ByteRate = (au->format.BlockAlign = (bit >> 3)*chn)*frq;
	au->format.BitsPerSample = bit;
	au->format.SampleRate = frq;
	au->format.NumChannels = chn;
}

Stepflow::Audio::AudioBuffer::~AudioBuffer(void)
{
	delete au;
}

Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::withOffset( int frames )
{
	stepflow::Audio setOff( *au );
    if ( frames < 0) setOff.addEndcut( (uint)-frames );
    else setOff.addOffset( (uint)frames );
	return gcnew AudioBuffer( setOff );
}
Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::getOrigin(void)
{
	stepflow::Audio notSetOff( *au );
	notSetOff.removeOffset();
    notSetOff.removeEndcut();
	return gcnew AudioBuffer(notSetOff);
}
Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::AddOffset( int frames )
{
    if ( frames < 0 ) au->addEndcut( (uint)-frames );
    else au->addOffset( (uint)frames );
	return this;
}
Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::RemoveOffset( void )
{
	au->removeOffset();
    au->removeEndcut();
	return this;
}
Stepflow::Audio::AudioBuffer^ 
Stepflow::Audio::AudioBuffer::CutRange( uint start, uint cutend )
{
    au->removeOffset();
    au->removeEndcut();
    au->addOffset(start);
    au->addEndcut(cutend);
    return this;
}
Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::getRange( uint startcut, uint endpoint )
{
    stepflow::Audio slice( *au );
    endpoint = slice.frameCount - endpoint;
    slice.removeOffset();
    slice.removeEndcut();
    slice.addOffset(startcut);
    slice.addEndcut(endpoint);
    return gcnew AudioBuffer( slice );
}


Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::convert( int sampleRate, PcmTag pcmFlags, int bitDepth, int channelCount )
{
    uint pcm = (uint)pcmFlags;
	au->convert( FrameTypeCode( AUDIOFRAME_CODE(bitDepth,channelCount,pcm) ), 1.0 );
	return this;
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::convert(PcmFormat fmt)
{
	au->convert( (stepflow::Format*)&fmt, 1.0 );
	return this;
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::convert(AudioFrameType frty)
{
	au->convert( FrameTypeCode(frty.Code), 1.0 );
	return this;
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::convert( AudioFrameType frty, double amp )
{
	au->convert( frty.native(), amp );
	return this;
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::convert( unsigned typecode )
{
	au->convert( reinterpret_cast<stepflow::AudioFrameType&>( typecode ), 1.0 );
	return this;
}

Stepflow::Audio::Audio^ Stepflow::Audio::AudioBuffer::amplified(double factor)
{
	return gcnew AudioBuffer( au->amplified(factor) );
}

Stepflow::Audio::Audio^ Stepflow::Audio::AudioBuffer::amplify( double factor )
{
	au->amplify( factor );
	return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::converted( int frq, PcmTag pcm, int bit, int chn )
{
	return gcnew AudioBuffer( frq, pcm, bit, chn,
        IntPtr( au->converted( stepflow::WAV_PCM_TYPE_ID(pcm), bit, chn, 1.0 ).detachBuffer() ),
        FrameCount*( (bit>>3)*chn ),
        Flags( DONT_ALLOCATE_NEW|OWN ) );
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::converted( PcmFormat format )
{
	return converted(
        format.SampleRate,
        format.Tag,
        format.BitsPerSample,
        format.NumChannels );
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::converted( AudioFrameType frty )
{
    uint rate = frty.native().Rate();
    rate = rate ? rate : Format.SampleRate;
	return converted( rate,
        frty.PcmTypeTag,
        frty.BitDepth,
        frty.ChannelCount );
}
Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::converted( AudioFrameType frty, double amp )
{
    uint rate = frty.native().Rate();
    rate = rate ? rate : Format.SampleRate;
	return gcnew AudioBuffer( rate,
        frty.PcmTypeTag, frty.BitDepth, frty.ChannelCount,
        IntPtr( au->converted(frty.native(), amp).detachBuffer()),
        FrameCount*((frty.BitDepth >> 3)*frty.ChannelCount),
        Flags( DONT_ALLOCATE_NEW|OWN ) );
}
void Stepflow::Audio::AudioBuffer::TrimIn(double threshold, float fadeTime)
{
	au->trimIn(threshold, fadeTime);
}
void Stepflow::Audio::AudioBuffer::TrimOut(double threshold, float fadeTime)
{
	au->trimOut(threshold, fadeTime);
}
void Stepflow::Audio::AudioBuffer::TrimIn(double threshold)
{
    au->trimIn(threshold);
}
void Stepflow::Audio::AudioBuffer::TrimOut(double threshold)
{
    au->trimOut(threshold);
}
void Stepflow::Audio::AudioBuffer::Trim(double threshold, float fadeTime)
{
	au->trim(threshold, fadeTime-(fadeTime/2),fadeTime+(fadeTime/2));
}
void Stepflow::Audio::AudioBuffer::Trim(double threshold)
{
	au->trim(threshold);
}
Stepflow::Audio::AudioBuffer^
Stepflow::Audio::AudioBuffer::trimmed(void)
{
    AudioBuffer^ temp = gcnew AudioBuffer( stepflow::Audio(*(const stepflow::Audio*)au, 1.0f) );
	temp->au->trim();
	return temp;
}


Stepflow::Audio::IAudioFrame^ 
Stepflow::Audio::AudioBuffer::GetFrame(unsigned idx)
{
    MANAGEDTYPE_SWITCH( au->frameTypeCode().Code(),
        return (IAudioFrame^)getFrame<CASE_TYPE>(idx);
    );
}


Stepflow::Audio::Audio^ 
Stepflow::Audio::AudioBuffer::paned(Panorama panorama,Panorama::Axis axt)
{
    return gcnew AudioBuffer( au->paned( *panorama, stepflow::Panorama::Axis(axt) ) );
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::pan(Panorama panorama, Panorama::Axis axt)
{
    au->pan(*panorama, stepflow::Panorama::Axis(axt));
    return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::paned(Panorama panorama)
{
    return gcnew AudioBuffer(au->paned(*panorama));
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::pan(Panorama panorama)
{
    au->pan(*panorama);
    return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::operator+(Audio ^ appendie)
{
    return gcnew AudioBuffer( au->operator+(appendie->native_cast()) );
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::operator+=(Audio ^ appendie)
{
    au->operator+=( *appendie->native_pntr() );
    return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::append(Audio ^ appendie)
{
    au->append( *appendie->native_cast() );
    return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::mixInto(Audio ^ trackB)
{
    Panorama neutral = Panorama();
    MANAGEDTYPE_SWITCH( TypeCode, 
        array<CASE_TYPE>^ dst = this->toArrayOf<CASE_TYPE>();
        array<CASE_TYPE>^ src = ((AudioBuffer^)trackB)->toArrayOf<CASE_TYPE>();
        for (int i = 0; i < this->FrameCount && i < trackB->FrameCount; ++i) {
            dst[i].Mix(src[i],neutral)->Amp(2);
        } break;
    );
    return this;
}

Stepflow::Audio::Audio^
Stepflow::Audio::AudioBuffer::mixTrax(Audio ^ trackA, Audio ^ trackB)
{
    Panorama neutral = Panorama();
    MANAGEDTYPE_SWITCH( TypeCode,
        array<CASE_TYPE>^ srcA = ((AudioBuffer^)trackA)->toArrayOf<CASE_TYPE>();
        array<CASE_TYPE>^ srcB = ((AudioBuffer^)trackB)->toArrayOf<CASE_TYPE>();
        this->FrameCount = maxOf(trackA->FrameCount,trackB->FrameCount);
        array<CASE_TYPE>^ dst = this->toArrayOf<CASE_TYPE>();
        for (int i = 0; i < this->FrameCount && i < trackB->FrameCount; ++i) {
            dst[i].Mix( srcA[i], 0.0 )->Mix( srcB[i], neutral )->Amp(2);
        } break;
    );
    return this;
}

array<Stepflow::Audio::Audio^>^
Stepflow::Audio::AudioBuffer::split( uint atTimePosition, bool trim )
{
    stepflow::Audio splits = au->split( atTimePosition, trim, true );
    array<Audio^>^ splots = gcnew array<Audio^>( splits.chunkCount() );
    stepflow::Audio* chunk = splits.firstChunk();
    for (int i = 0; i < splots->Length; ++i) {
        splots[i] = gcnew AudioBuffer(*chunk);
        chunk = chunk->nextChunk();
    } return splots;
}


