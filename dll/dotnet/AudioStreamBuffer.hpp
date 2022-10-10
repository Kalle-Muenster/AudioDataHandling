/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioStreamBuffer.hpp                      ||
||     Author:    Kalle                                      ||
||     Generated: 04.03.2019                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _AudioStreamBuffer_hpp_
#define _AudioStreamBuffer_hpp_

#include "AudioBuffer.hpp"

#define silence (stepflow::Audio*)stepflow::Audio::Silence;

namespace Stepflow { namespace Audio {  
    public ref class AudioStreamBuffer 
    : public AudioBuffer
    , public IAudioInStream
    , public IAudioOutStream
    { typedef stepflow::AudioStream NATIVE;

    internal:
        IntPtr data;
        operator NATIVE&( void ) { return *reinterpret_cast<NATIVE*>(au); }
        AudioStreamBuffer( const NATIVE& );

        ref class BufferChunk : public IChunkable {
        private:
            int     index;
            NATIVE* track;

        internal:
            BufferChunk( AudioStreamBuffer^ audio )
                : track( (NATIVE*)audio->au->firstChunk() ), index( -1 ) {
            }
        public:
            property Audio^ First {
                virtual Audio^ get(void) override { index = 0; return gcnew AudioStreamBuffer(*(track = (NATIVE*)track->firstChunk())); }
            }

            property Audio^ Last {
                virtual Audio^ get(void) override { index = track->chunkCount()-1; return gcnew AudioStreamBuffer(*(track=(NATIVE*)track->lastChunk())); }
            }

            property Audio^ Current {
                virtual Audio^ get( void ) override {
                    track = (NATIVE*)track->getChunk( track->GetReadPosition() );
                    index = track->chunkIndex();
                    return gcnew AudioStreamBuffer( *track );
                }
            }

            virtual bool MoveNext() override {
                if (++index >= track->chunkCount())
                    return false;
                else if (index > 0)
                    track = (NATIVE*)track->nextChunk();
                return true;
            }

            virtual void Reset() override {
                track = (NATIVE*)track->firstChunk();
                index = -1;
            }

            virtual Audio^ Next() override {
                if (++index < track->chunkCount())
                    return gcnew AudioStreamBuffer( *(track = (NATIVE*)track->nextChunk()) );
                else return nullptr;
            }

            virtual property int Count {
                int get(void) override { return track->chunkCount(); }
            }

            virtual property int Index {
                int get(void) override { return index = track->chunkIndex(); }
            }

            virtual property Audio^ default[int] {
                Audio^ get(int idx) override {
                    return gcnew AudioStreamBuffer(*track->chunkAtIndex(idx));
                }
                void   set(int idx, Audio^ audio) override {
                    stepflow::Audio* merk1 = track->chunkAtIndex(idx - 1);
                    stepflow::Audio* index = merk1->nxt;
                    stepflow::Audio* merk2 = index->nxt;
                    merk1->nxt = ((AudioStreamBuffer^)audio)->au;
                    merk1->nxt->nxt = merk2;
                    index->drop();
                }
            }
        };

    public:
        AudioStreamBuffer( PcmFormat% fmt, uint framecount, bool allocateManagedMemory ) {
            const uint size = fmt.BlockAlign * framecount;
            NATIVE* AU;
            if ( allocateManagedMemory ) {
                data = Marshal::AllocCoTaskMem( size );
                pin_ptr<PcmFormat> ptr = &fmt;
                AU = new NATIVE( *(const stepflow::Format*)ptr, data.ToPointer(), size );
                AU->mode.remFlag( stepflow::Initiatio::DONT_ALLOCATE_NEW );
                AU->mode.addFlag( stepflow::Initiatio::ALLOCATE_NEW_COPY );
            } else {
                AU = new NATIVE( uint(fmt.SampleRate), uint(fmt.BitsPerSample), uint(fmt.NumChannels), int(framecount) );
            } au = AU;
        }

        AudioStreamBuffer( int frq, int bit, int chn, uint framecount, bool allocateManagedMemory )
            : AudioStreamBuffer( *(PcmFormat*)&stepflow::CreateWaveFormat(frq,bit,chn),
                                 framecount, allocateManagedMemory ) {
        }

        AudioStreamBuffer( PcmFormat% fmt, IntPtr pData, uint dataSize ) {
            data = pData;
            pin_ptr<PcmFormat> ptr = &fmt;
            au = new NATIVE( *(const stepflow::Format*)ptr, pData.ToPointer(), dataSize );
        }

        AudioStreamBuffer( Audio^ wrap ) 
            : AudioBuffer( *wrap->native_pntr() ) {
        }

        generic<class T> where T : value struct 
        static AudioStreamBuffer^ wrapData( AudioFrameType type, uint sampleRate, array<T>^ rawData ) {
            pin_ptr<T> ptBuffer = &rawData[0];
            stepflow::AudioStream au( stepflow::Audio(ptBuffer, rawData->Length * sizeof(T), stepflow::DONT_ALLOCATE_NEW));
            au.format.PCMFormatTag = WAV_PCM_TYPE_ID( type.BitDepth > 24 ? 3 : 1 );
            au.format.ByteRate = (au.format.BlockAlign = (type.BitDepth >> 3)*type.ChannelCount)*sampleRate;
            au.format.BitsPerSample = type.BitDepth;
            au.format.SampleRate = sampleRate;
            au.format.NumChannels = type.ChannelCount;
            return gcnew AudioStreamBuffer(au);
        }

        virtual ~AudioStreamBuffer(void) {
            NATIVE* AU = reinterpret_cast<NATIVE*>(au);
            if ( AU->mode.hasFlag( stepflow::Initiatio::ALLOCATE_NEW_COPY ) )
                if ( data != IntPtr::Zero ) {
                    AU->mode.remFlag( stepflow::Initiatio::ALLOCATE_NEW_COPY );
                    Marshal::FreeCoTaskMem( data );
                    data = IntPtr::Zero;
                }
            AU->drop();
            delete AU;
            au = silence;
        }


        void SeekRead(uint position) { return Seek(StreamDirection::READ,position); }
        uint GetReadPosition(void) { return GetPosition(StreamDirection::READ); }
        void SeekWrite(uint position) { return Seek(StreamDirection::WRITE, position); }
        uint GetWritePosition(void) { return GetPosition(StreamDirection::WRITE); }


        // Geerbt über IAudioStream
        virtual PcmFormat GetFormat(void) {
            return Format;
        }
        virtual uint GetPosition(StreamDirection direction) {
            return ((NATIVE*)au)->GetPosition( stepflow::StreamDirection((byte)direction) );
        }
        virtual property StreamDirection Direction {
            StreamDirection get(void) { return StreamDirection::INOUT; }
        }
        virtual StreamDirection CanSeek(void) {
            return StreamDirection::INOUT;
        }
        virtual AudioFrameType  GetFrameType() {
            PcmFormat fmt = this->Format;
            return AudioFrameType( (uint)fmt.Tag,fmt.BitsPerSample, fmt.NumChannels, fmt.SampleRate );
        }
        virtual void Seek( StreamDirection direction, uint position ) {
            ((NATIVE*)au)->Seek( position, stepflow::StreamDirection( (byte)direction ) );
        }

        // Geerbt über IAudioInStream
        virtual uint AvailableBytes(void);
        virtual uint AvailableFrames(void);
        virtual TimeSpan AvailableTime(void);

        virtual uint CanStream( StreamDirection stream ) {
            if ( stream.In( StreamDirection::OUTPUT ) )
                return ((NATIVE*)au)->framesWritable();
            else if ( stream.In( StreamDirection::INPUT ) )
                return ((NATIVE*)au)->framesReadable();
        }

        property IChunkable^ Chunk {
            IChunkable^ get(void) override { return gcnew BufferChunk(this); }
        }

        virtual Audio^ ReadAll(void);
        virtual Audio^ Read(void);
        virtual Audio^ Read(int frames);
        virtual uint   Read(Audio ^ buffer);
        virtual uint   Read(Audio ^ buffer, int numberFrames);
        virtual uint   Read(IntPtr dstMem, int countBytes, int offsetDstBytes);

        virtual IAudioFrame^         ReadFrame(void);
        virtual array<IAudioFrame^>^ ReadFrames(uint count);


        // Geerbt über IAudioOutStream
        virtual uint WrittenBytes(void);
        virtual uint WrittenFrames(void);
        virtual TimeSpan WrittenTime(void);

        virtual uint Write(Audio^ srcBuffer, int countOnFs, int FsOffsterInSrc);
        virtual uint Write(IntPtr rawData, int countBytes, int offsetSrcBytes);

        virtual uint WriteAudio(Audio^ buffer);
        virtual uint WriteSample(short);
        virtual uint WriteSample(float);

        virtual uint WriteFrame(stepflow::s16 sample );
        virtual uint WriteFrame(stepflow::f32 sample );
        virtual uint WriteFrame(stepflow::s16 sample, Panorama mixer);
        virtual uint WriteFrame(stepflow::f32 sample, Panorama mixer);
        virtual uint WriteFrame(stepflow::f64 sample, Panorama mixer);

        virtual uint WriteFrame( IAudioFrame ^ frame );

        generic<class FT> where FT : System::ValueType
        virtual unsigned WriteFrame(FT frame) {           
            return ((NATIVE*)au)->Write((void*)&frame,1);
        }

        // Geerbt über IAudioInStream
        generic<typename FrTy> where FrTy : IAudioFrame, gcnew()
        virtual uint Read(array<FrTy>^ dst, uint count, uint ofset) {
                array<FrTy>^ Data = toArrayOf<FrTy>();
                const uint start = GetReadPosition();
                const uint available = WrittenFrames()-start;
                count = minOf( count, available );
                const uint end = start + count;
                for (uint f = start; f < end; ++f) {
                    dst[f + ofset] = Data[f];
                } SeekRead(end);
                return count;
        }
};

}} //end of Stepflow::Audio

#endif
