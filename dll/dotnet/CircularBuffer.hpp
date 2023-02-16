/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      CircularBuf.hpp                            ||
||     Author:    Kalle                                      ||
||     Generated: 05.01.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _CircularBuffer_hpp_
#define _CircularBuffer_hpp_

#include "Audio.hpp"
#include "../../src/WaveBuffers.hpp"
#include "AudioStreamBuffer.hpp"

using namespace System;
//using namespace stepflow;
using namespace System::Runtime::InteropServices;

#define MAX_LOCK_WAIT_CYCLE 1000

namespace Stepflow {
	namespace Audio {

		generic<class T> where T : IAudioFrame, gcnew()
		public ref class Circular
            : public Audio
            , public IAudioInStream
            , public IAudioOutStream 
		{
			typedef T Frame;

		internal:
			const static AudioFrameType type = T()->FrameType;

			stepflow::Circular* au;

			Circular(const stepflow::Circular& copy) {
				au = new stepflow::Circular(copy);
			}
			virtual stepflow::Audio native_cast(void) override {
				return stepflow::Audio(au->detachBuffer(), au->cbSize, Initiatio(DONT_ALLOCATE_NEW | CIRCULAR));
			}
            virtual stepflow::Audio* native_pntr(void) override {
                return (stepflow::Audio*)au;
            }
			operator stepflow::Circular(void) {
				return *au;
			}

            ref class BufferChunk : public IChunkable {
            private:
                int                              cnt;
                Circular^                        track;

            internal:
                BufferChunk(Circular^ audio,int count)
                    : track(audio), cnt(count) {
                }
            public:
                property Audio^ First {
                    virtual Audio^ get(void) override { return track->GetReadable(); }
                }

                property Audio^ Last {
                    virtual Audio^ get(void) override { return gcnew AudioBuffer(*track->au->getReadableAudio().lastChunk()); }
                }

                property Audio^ Current {
                    virtual Audio^ get(void) override { return gcnew AudioBuffer(*track->au); }
                }

                virtual Audio^ Next() override {
                    return Last;
                }

                virtual void Reset() override {
                    track->Seek( Stepflow::Audio::StreamDirection::INOUT, 0 );
                }
                
                virtual bool MoveNext() override {
                    track->Seek( Stepflow::Audio::StreamDirection::READ, track->GetPosition(Stepflow::Audio::StreamDirection::READ) + track->AvailableFrames() );
                    return true;
                }

                property int Count {
                    virtual int get(void) override { return cnt; }
                }

                property int Index {
                    virtual int get(void) override { return 0; }
                }

                property Audio^ default[int]{
                    virtual Audio^ get(int idx) override {
                        return gcnew AudioBuffer(*track->au->chunkAtIndex(idx));
                    }
                    virtual void   set(int idx, Audio^ audio) override {
                        stepflow::Audio* merk1 = track->au->chunkAtIndex(idx - 1);
                        stepflow::Audio* index = merk1->nxt;
                        stepflow::Audio* merk2 = index->nxt;
                        merk1->nxt = ((AudioBuffer^)audio)->au;
                        merk1->nxt->nxt = merk2;
                        index->drop();
                    }
                }
            };

		public:
			Circular(Audio^ init) {
				au = new stepflow::Circular( TypeCode, init->native_pntr()->detachBuffer(), init->DataSize);
			}
			Circular(uint framecount) {
				au = new stepflow::Circular( TypeCode, framecount );
			}
			Circular(IntPtr data, uint cbSize) {
				au = new stepflow::Circular( TypeCode, data.ToPointer(), cbSize );
			}

			virtual ~Circular(void) {
				delete au;
			}

			property uint FramesAvailable {
				uint get(void) {
					return au->availableFrames();
				}
			}

			property uint TypeCode {
				uint get(void) override {
                    return (uint)type.code;
				}
			}

            T readNextFrame() {
                T frame = T();
                int readyRead = MAX_LOCK_WAIT_CYCLE;
                while (readyRead>0) {
                    stepflow::Circular::ReadAccessLock key = au->getReadLock();
                    if (&key != stepflow::Audio::Silence) {
                        memcpy(frame->GetRaw().ToPointer(), au->ReadFrame(), frame->FrameType.BlockAlign);
                        au->releaseReadLock(key);
                        return frame;
                    } else readyRead--;
                } throw gcnew System::Exception("read access lock");
			}
			void writeNextFrame(T frame) {
                int readyWrite = MAX_LOCK_WAIT_CYCLE;
                while ( readyWrite>0 ) {
				    stepflow::Circular::WriteAccessLock key = au->getWriteLock();
				    if (&key!=stepflow::Audio::Silence) {
					    au->WriteFrame(frame->GetRaw().ToPointer());
					    au->releaseWriteLock(key);
                        return;
                    } else readyWrite--;
				} throw gcnew System::Exception("write access lock");
			}

            virtual IAudioFrame^ GetFrame(uint idx) override {
                T f = T();
                memcpy(f->GetRaw().ToPointer(),(*au)[idx],f->FrameType.BlockAlign);
                return (IAudioFrame^)f;
            }
            virtual void SetFrame(uint idx, IAudioFrame^ frame) override {
                IAudioFrame^ f = frame->Convert( this->type );
                memcpy((*au)[idx], f->GetRaw().ToPointer(), f->FrameType.BlockAlign);
            }

            AudioStreamBuffer^ CopyReadable(void) {
				return gcnew AudioStreamBuffer(au->ReadFrames(au->availableFrames()));
			}
            AudioStreamBuffer^ GetReadable(void) {
				return gcnew AudioStreamBuffer(au->getReadableAudio());
			}
            property IChunkable^ Chunk {
                IChunkable^ get(void) override { return gcnew BufferChunk(this, au->getReadPosition() > au->getWritePosition() ? 2 : 1); }
            }
			Audio^ Read(uint framecount) {
				return gcnew AudioBuffer(au->ReadFrames(framecount));
			}
            virtual uint Read(Audio^ buffer) {
				return au->ReadFrames( buffer->native_pntr() );
			}
			virtual uint Read(Audio^ buffer, int framecount) {
				return au->ReadFrames( buffer->native_pntr(), (uint)framecount);
			}
			virtual uint WriteAudio(Audio^ data) {
				return au->WriteFrames(*((AudioBuffer^)data)->au);
			}
			virtual uint Write(Audio^ data, int length, int offset) {
                stepflow::Audio src = data;
                src.addOffset(offset);
                return au->WriteFrames(src,length);
			}

			generic<typename T> where T : ValueType
			uint   WriteChannel(int channelNumber, array<T>^ channelData, uint length) {
				//todo
				throw gcnew System::Exception("todo");
			}

			generic<typename T> where T : System::ValueType
				uint   Write(array<T>^ data, uint Tcount) {
				pin_ptr<T> ptr = &data[0];
				return au->WriteFrames(ptr, 0, Tcount * sizeof(T));
			}
			uint   Write(System::IntPtr rawdata, uint cbSize) {
				return au->WriteFrames(rawdata.ToPointer(), 0, cbSize);
			}



			// Geerbt über AbstractAudioBuffer

			virtual IntPtr GetRaw(void) override {
				return IntPtr(au->data);
			}
			virtual IntPtr GetRaw(int position) override {
				return IntPtr((*au)[au->getReadPosition()+position]);
			}
			virtual Audio ^ converted(int frq, PcmTag tag, int bit, int chn) override {
				return gcnew Circular( stepflow::Circular( au->converted( WAV_PCM_TYPE_ID(tag), bit, chn, 1.0 ) ) );
			}
			virtual Audio ^ converted(PcmFormat fmt) override {
				return converted( (int)fmt.SampleRate, fmt.Tag, (int)fmt.BitsPerSample, (int)fmt.NumChannels );
			}
			virtual Audio ^ converted(AudioFrameType frty) override {
				return converted( Format.SampleRate, frty.PcmTypeTag, frty.BitDepth, frty.ChannelCount );
			}
			virtual Audio ^ converted(AudioFrameType frty,double amp) override {
				return gcnew Circular( stepflow::Circular( au->converted(FrameTypeCode(frty.Code), 1.0 ) ) );
			}
			virtual Audio ^ convert(int frq, PcmTag pcm, int bit, int chn) override {
				au->convert( WAV_PCM_TYPE_ID(pcm), bit, chn );
				return this;
			}
			virtual Audio ^ convert(PcmFormat fmt) override {
				au->convert( reinterpret_cast<stepflow::Format&>(fmt) );
				return this;
			}
			virtual Audio ^ convert(AudioFrameType frty) override {
				au->convert( frty.native(), 1.0 );
				return this;
			}
			virtual Audio ^ convert(AudioFrameType frty,double amp) override {
				au->convert( frty.native(), amp );
				return this;
			}
			virtual Audio ^ convert( unsigned typecode ) override {
				au->convert( reinterpret_cast<stepflow::AudioFrameType&>(typecode), 1.0 );
				return this;
			}

			virtual Audio ^ amplified(double factor) override {
				return gcnew Circular( au->amplified(factor) );
			}
			virtual Audio ^ amplify(double factor) override {
				au->amplify(factor);
				return this;
			}
            virtual Audio^ paned(Panorama panorama,Panorama::Axis axt) override {
                return gcnew Circular( au->paned( *panorama, stepflow::Panorama::Axis(axt) ) );
            }
            virtual Audio^ pan(Panorama panorama, Panorama::Axis axt) override {
                au->pan(*panorama, stepflow::Panorama::Axis(axt));
                return this;
            }
            virtual Audio^ paned(Panorama panorama) override {
                return gcnew Circular(au->paned(*panorama));
            }
            virtual Audio^ pan(Panorama panorama) override {
                au->pan(*panorama);
                return this;
            }

            virtual PcmFormat GetFormat(void)
            {
                return Format;
            }
            virtual uint GetPosition(StreamDirection direction) override
            {
                return direction.In( StreamDirection::READ )
                     ? au->getReadPosition()
                     : au->getWritePosition();
            }
            virtual property StreamDirection Direction
            {
                StreamDirection get(void) { return StreamDirection::INOUT; }
            }

            virtual uint CanStream(StreamDirection stream) override
            {
                if ( stream.In(  StreamDirection::OUTPUT ) ) {
                    return au->frameCount - au->availableFrames();
                } else if ( stream.In( StreamDirection::INPUT ) ) {
                    return au->availableFrames();
                }
            }

            virtual StreamDirection CanSeek(void)
            {
                return StreamDirection::NONE;
            }

            virtual void Seek(StreamDirection direction, uint pos)
            {
                if ( direction.In( StreamDirection::WRITE ) ) {
                    stepflow::Circular::WriteAccessLock writelock = au->getWriteLock();
                    writelock = pos;
                    au->releaseWriteLock(writelock);
                } else {
                    stepflow::Circular::ReadAccessLock readlock = au->getReadLock();
                    readlock = pos;
                    au->releaseReadLock(readlock);
                }
            }
            
            virtual uint AvailableBytes(void)
            {
                return au->availableFrames() * au->format.BlockAlign;
            }
            virtual uint AvailableFrames(void)
            {
                return au->availableFrames();
            }
            virtual TimeSpan AvailableTime(void)
            {
                return TimeSpan::FromMilliseconds(((float)au->format.SampleRate / au->availableFrames()) * 1000);
            }
            virtual Audio ^ Read(int frames)
            {
                return gcnew AudioBuffer(au->ReadFrames(frames));
            }
            virtual array<IAudioFrame^>^ ReadFrames(uint count)
            {
                stepflow::Circular::ReadAccessLock lock = au->getReadLock();
                if (&lock == stepflow::Audio::Silence) return nullptr;
                uint available = AvailableFrames();
                count = available < count
                      ? available : count;
                array<IAudioFrame^>^ buffer = gcnew array<IAudioFrame^>(count);
                for (int i = 0; i < count; ++i) {
                    buffer[i] = ReadFrame();
                } au->releaseReadLock(lock);
                return buffer;
            }
            virtual uint Read( IntPtr dstMem, int countBytes, int offsetDstBytes )
            {
                return au->Read( (byte*)dstMem.ToPointer(), offsetDstBytes, countBytes );
            }

            virtual uint Read( array<T>^ dst, uint count, uint ofset )
            {
                stepflow::Circular::ReadAccessLock lock = au->getReadLock();
                if (&lock == stepflow::Audio::Silence) return 0;
                uint available = AvailableFrames();
                count = available < count
                      ? available : count;
                count += ofset;
                for (int i = ofset; i < count; ++i) {
                    dst[i] = (T)ReadFrame();
                } au->releaseReadLock(lock);
                return count - ofset;
            }

            virtual Audio ^ ReadAll(void)
            {
                return CopyReadable();
            }
            virtual Audio ^ Read(void)
            {
                return GetReadable();
            }
            virtual IAudioFrame ^ ReadFrame(void);


            virtual AudioFrameType GetFrameType(void)
            {
                return type;
            }




            // Geerbt über IAudioOutStream

            virtual uint Write(IntPtr src, int byteCount, int offset)
            {
                return au->WriteFrames(src.ToPointer(), offset, byteCount);
            }
            virtual uint WriteFrame(IAudioFrame ^ frame)
            {
                writeNextFrame( (T)frame->Convert( GetFrameType() ) );
                return 1;
            }
            virtual uint WriteSample(short sample)
            {
                ulong fr = (sample | sample << 16 | sample << 32 | sample << 56);
                au->WriteFrame(&fr);
                return 1;
            }
            virtual uint WriteSample(float sample)
            {
                const uint conv = *(uint*)&sample;
                ulong fr = (conv | conv << 32);
                au->WriteFrame(&fr);
                return 1;
            }
            virtual uint WriteFrame(short sample)
            { 
                writeNextFrame((T)AudioFrameType(PcmTag::PCMs,16,au->format.NumChannels,au->format.SampleRate).CreateEmptyFrame()->Mix(sample,Panorama::Neutral)->Convert( GetFrameType() ));
                return 1;
            }
            virtual uint WriteFrame(float sample)
            {
                writeNextFrame((T)AudioFrameType(PcmTag::PCMf,32,au->format.NumChannels,au->format.SampleRate).CreateEmptyFrame()->Mix(sample,Panorama::Neutral)->Convert( GetFrameType() ));
                return 1;
            }
            virtual uint WriteFrame(short sample, Panorama mixer)
            {
                writeNextFrame((T)AudioFrameType(PcmTag::PCMs,16,au->format.NumChannels,au->format.SampleRate).CreateEmptyFrame()->Mix(sample,mixer)->Convert( GetFrameType() ));
                return 1;
            }
            virtual uint WriteFrame(float sample, Panorama mixer)
            {
                writeNextFrame((T)AudioFrameType(PcmTag::PCMf,32,au->format.NumChannels,au->format.SampleRate).CreateEmptyFrame()->Mix(sample,mixer)->Convert( GetFrameType() ));
                return 1;
            }
            virtual uint WriteFrame(stepflow::f64 sample, Panorama mixer)
            {
                writeNextFrame((T)AudioFrameType(PcmTag::PCMf,64,au->format.NumChannels,au->format.SampleRate).CreateEmptyFrame()->Mix(sample,mixer)->Convert( GetFrameType() ));
                return 1;
            }
            virtual uint WrittenBytes(void)
            {
                return au->availableFrames()*au->format.BlockAlign;
            }
            virtual uint WrittenFrames(void)
            {
                return au->availableFrames();
            }
            virtual TimeSpan WrittenTime(void)
            {
                return TimeSpan::FromMilliseconds( 1000.0 / (au->availableFrames() * Format.SampleRate) );
            }

            virtual Audio ^ operator+(Audio ^ appendie) override
            {
                return gcnew AudioBuffer(au->operator+(appendie->native_cast()));
            }
            virtual Audio ^ operator+=(Audio ^ appendie) override
            {
                au->operator+=( *appendie->native_pntr() );
                return this;
            }
            virtual Audio ^ append(Audio ^ appendie) override
            {
                au->append( *appendie->native_pntr() );
                return this;
            }
            virtual Audio ^ mixInto(Audio ^ trackB) override
            {
                uint position = au->getReadPosition();
                uint length = au->availableFrames();
                AudioBuffer^ readable = gcnew AudioBuffer(au->getReadableAudio());
                if( readable->au->getLength() < length )
                    throw gcnew Exception("read access is locked");
                stepflow::Circular::ReadAccessLock readhead = au->getReadLock();
                readable->mixInto( trackB );
                readhead = position;
                au->releaseReadLock( readhead );
                return this;
            }

            virtual Audio ^ mixTrax(Audio ^ trackA, Audio ^ trackB) override
            {
                AudioBuffer^ temp = gcnew AudioBuffer( trackA->native_cast().copy() );
                au->WriteFrames( temp->mixInto( trackB )->native_cast() );
                return this;
            }

            generic<class TT> where TT : IAudioFrame, gcnew()
            virtual uint Read(array<TT>^ dst, uint count, uint ofset)
            {
                TT f=TT();
                count += ofset;
                for (uint i = ofset; i < count; ++i)
                    memcpy(dst[i]->GetRaw().ToPointer(), au->ReadFrame(), f->FrameType.BlockAlign);
                return count - ofset;
            }

            uint ReadFrames(IntPtr dst, uint framecount) {
                return au->Read((byte*)dst.ToPointer(), 0, framecount*au->format.BlockAlign);
            }
            uint ReadAvailable(IntPtr dst) {
                return au->Read((byte*)dst.ToPointer(), 0, au->availableFrames()*au->format.BlockAlign);
            }
        };

	}
}//end of Stepflow
#endif
