#ifndef _WowbaggerStream_hpp_
#define _WowbaggerStream_hpp_

#include "AudioStreamBuffer.hpp"

namespace Stepflow {
	namespace Audio {
		public ref class WowbaggerStream
			: public AudioStreamBuffer
		{
		public:
			WowbaggerStream( AudioFrameType typ, uint srt, uint framesPerChunk );
			WowbaggerStream( Audio^ audio );

			virtual uint Write( IntPtr data, int offset, int length ) override;
			virtual uint Write( array<byte>^ data, int offset, int length ) override;

			virtual uint WriteAudio( Audio ^ buffer ) override;
			virtual uint WriteFrame( IAudioFrame ^ frame ) override;
			
			uint MakeWritable( uint frames );
		};
	}
}
#endif