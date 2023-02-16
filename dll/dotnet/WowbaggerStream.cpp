#include "WowbaggerStream.hpp"

Stepflow::Audio::WowbaggerStream::WowbaggerStream( AudioFrameType typ, uint srt, uint framesPerChunk )
	: AudioStreamBuffer( srt, typ.PcmTypeTag, typ.BitDepth, typ.ChannelCount, framesPerChunk, false )
{}

Stepflow::Audio::WowbaggerStream::WowbaggerStream( Audio^ audio )
	: AudioStreamBuffer( audio )
{}

uint
Stepflow::Audio::WowbaggerStream::Write( IntPtr data, int cbOffset, int cbSize )
{
	return ((NATIVE*)au)->Write( data.ToPointer(), cbSize, cbOffset );
}

uint
Stepflow::Audio::WowbaggerStream::Write( array<byte>^ data, int cbOffset, int cbSize )
{
	pin_ptr<byte> ptrData( &data[0] );
	return ((NATIVE*)au)->Write( ptrData, cbSize, cbOffset );
}

uint 
Stepflow::Audio::WowbaggerStream::WriteAudio( Audio^ buffer )
{
	return ((NATIVE*)au)->Write( buffer );
}

uint
Stepflow::Audio::WowbaggerStream::WriteFrame( IAudioFrame^ frame )
{
	uint ok = ((NATIVE*)au)->WriteFrame( frame->GetRaw().ToPointer() );
	if( !ok ) {
		stepflow::Format* fmt = ((NATIVE*)au)->GetFormat();
		((NATIVE*)au)->lastChunk()->append( NATIVE(fmt->SampleRate, fmt->PCMFormatTag, fmt->BitsPerSample, fmt->NumChannels, ( (NATIVE *)au )->chunkSize()).outscope() );
		return ((NATIVE*)au)->WriteFrame( frame->GetRaw().ToPointer() );
	}
}
			
uint
Stepflow::Audio::WowbaggerStream::MakeWritable( uint frames )
{
	return ( (NATIVE *)au )->ensureSize( ( (NATIVE *)au )->GetFormat()->BlockAlign * frames );
}
