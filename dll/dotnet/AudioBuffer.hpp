#ifndef _AudioBuffer_hpp_
#define _AudioBuffer_hpp_

#include "Audio.hpp"
#include "../../src/WaveBuffers.hpp"

namespace Stepflow {
	namespace Audio {

		public ref struct AudioBuffer : public Audio
		{
		internal:
			stepflow::Audio* au;

            virtual stepflow::Audio* native_pntr(void) override { return au; }
			virtual stepflow::Audio  native_cast(void) override { return stepflow::Audio(*au); }

			AudioBuffer^ operator =(AudioBuffer^ That) {
                (*this->au) = (*That->au);
				return this;
			}

            operator stepflow::Audio&(void) { return *au; }

			AudioBuffer(stepflow::Audio&);
            AudioBuffer(void) { au = (stepflow::Audio*)stepflow::Audio::Silence; };

            ref class BufferChunk : public IChunkable {
            private: 
                int index;
                stepflow::Audio* track;
            internal:
                BufferChunk( AudioBuffer^ audio )
                    : track( audio->au->firstChunk() ), index(-1) {
                }
            public:
                virtual property Audio^ First {
                     Audio^ get(void) override { return gcnew AudioBuffer( *track->firstChunk() ); }
                }

                virtual property Audio^ Last {
                     Audio^ get(void) override { return gcnew AudioBuffer( *track->lastChunk() ); }
                }

                virtual property Audio^ Current {
                     Audio^ get(void) override { return gcnew AudioBuffer( *track ); }
                }

                virtual bool MoveNext( void ) override {
                    if( ++index == track->chunkCount() )
                        return false;
                    else if( index > 0 )
                        track = track->nextChunk();
                    return true;
                }

                virtual void Reset( void ) override {
                    track = track->firstChunk();
                    index = -1;
                }

                virtual Audio^ Next() override {
                     return gcnew AudioBuffer( *(track = track->nextChunk()) );
                }

                virtual property int Count {
                     int get(void) override { return track->chunkCount(); }
                }
                                      
                virtual property int Index {
                     int get(void) override { return track->chunkIndex(); }
                }

                virtual property Audio^ default[int] {
                    Audio^ get( int idx ) override {
                        return gcnew AudioBuffer( *track->chunkAtIndex( idx ) );
                    }
                    void set(int idx, Audio^ audio) override {
                        stepflow::Audio* merk1 = track->chunkAtIndex( idx-1 );
                        stepflow::Audio* index = merk1->nxt;
                        stepflow::Audio* merk2 = index->nxt;
                        merk1->nxt = ((AudioBuffer^)audio)->au;
                        merk1->nxt->nxt = merk2;
                        index->drop();
                    }
                }
            };

		public:

            // new constructors
			AudioBuffer(uint framecount);
			AudioBuffer(PcmFormat, uint framecount);
			
            AudioBuffer(AudioFrameType, uint sampleRate, uint framecount);
			AudioBuffer(int frq, int bit, int chn, uint framecount);
            
            // wrap constructors
            AudioBuffer(AudioFrameType, System::Array^);
            AudioBuffer(int frq, int bit, int chn, System::IntPtr rawData, uint rawSize, Audio::Flags flags);
            
            ~AudioBuffer(void);

            // wrap creators
            generic<class T> where T : value struct static AudioBuffer^
			wrapData( int frq, int bit, int chn, array<T>^ dataToWrap ) {
                pin_ptr<T> ptBuffer = &dataToWrap[0];
                stepflow::Audio au(ptBuffer, dataToWrap->Length * sizeof(T), stepflow::DONT_ALLOCATE_NEW);
                au.format.PCMFormatTag = (WAV_PCM_TYPE_ID)(bit > 16 ? 3 : 1);
                au.format.ByteRate = (au.format.BlockAlign = (bit >> 3)*chn)*frq;
                au.format.BitsPerSample = bit;
                au.format.SampleRate = frq;
                au.format.NumChannels = chn;
                return gcnew AudioBuffer(au);
            }
            generic<class T> where T : value struct static AudioBuffer^
            wrapData( AudioFrameType frameType, uint sampleRate, array<T>^ dataToWrap) {
                return wrapData<T>((int)sampleRate, frameType.BitDepth, frameType.ChannelCount, dataToWrap );
            }
            

			

			property PcmFormat Format {
				PcmFormat get(void) override {
					return *(PcmFormat*)&au->format;
				}
				void set(PcmFormat value) override {
					au->format = *(stepflow::Format*)&value;
				}
			}
			property unsigned FrameCount {
				unsigned get(void) override {
					return au->getLength();
				}
				void set(unsigned value) override {
					au->setLength(value);
				}
			}
            property TimeSpan Duration {
                TimeSpan get(void) override {
                    return TimeSpan::FromMilliseconds(au->getDuration()*1000);
                }
                void set(TimeSpan duration) override {
                    au->setLength((uint)(((double)au->format.ByteRate / 1000.0)*duration.TotalMilliseconds) / au->format.BlockAlign);
                }
            }
			property unsigned DataSize {
				unsigned get(void) override {
					return au->cbSize;
				}
				void set(unsigned value) override {
					au->setLength(value / au->format.BlockAlign);
				}
			}
			property uint TypeCode {
				uint get(void) override {
					return au->frameTypeCode();
				}
			}

            property IChunkable^ Chunk {
                virtual IChunkable^ get(void) override { return gcnew BufferChunk( this ); }
            }

			generic<typename FrameType> where FrameType : IAudioFrame, gcnew()
				FrameType getFrame(unsigned idx) {
                FrameType frame = safe_cast<FrameType>(GetFrameType().CreateEmptyFrame());
				memcpy( frame->GetRaw().ToPointer(), (*au)[idx], au->format.BlockAlign);
                return frame;
			}

			generic<typename FrameType> where FrameType : IAudioFrame, gcnew()
				void setFrame( unsigned idx, FrameType frame ) {
				SetFrame( idx, frame );
			}

            void SetFrameUnchecked( unsigned idx,IAudioFrame^ frame ) {
                memcpy( (*au)[idx], frame->GetRaw().ToPointer(), frame->FrameType.BlockAlign );
            }

            virtual IAudioFrame^ GetFrame(uint idx) override;
            virtual void SetFrame( unsigned idx, IAudioFrame^ frame ) override {
                SetFrameUnchecked( idx, frame->Convert( Format.FrameType ) );
            }

            // Modify either: start point in -, or endcut from - the buffer which this instance allocates (offset 
            // greater 0 moves the start point forward, offset smaller 0 moves the endcut point backward)
			AudioBuffer^ AddOffset( int offset );
            // Modify start and end points for this instances buffer (move start and end points within single call)
            AudioBuffer^ CutRange( uint startframe, uint endframe );
            // Reset start and end points of this instances buffer, so it again matches initially allocated data size
			AudioBuffer^ RemoveOffset(void);
            
            // Create a new container which points same data in memory, but which has an offset applied to either: 
            // its entry point (on positive value) or (on negative values) an offset subtracted from the endcut point 
            AudioBuffer^ withOffset( int frames );
            // Create a new container which points same data in memory, but which has modified start and end points 
            // applied to the one same buffer in memory. 
            AudioBuffer^ getRange( uint startFrame, uint endFrame );
            // Create a new container which points exact memory block which this instance once used to allocate initially,
            // but which not has any start or end point modifications applied to it which could have been applied earlier 
            // via AddOffset() or CutRange() calls - so does similar thing like RemoveOffset() but returns a new container
            // instead of changing it's own start and end point variables
            AudioBuffer^ getOrigin( void );

			void TrimIn(double threshold, float fadeTime);
            void TrimIn(double threshold);
			void TrimOut(double threshold, float fadeTime);
            void TrimOut(double threshold);
			void Trim(double threshold, float fadeTime);
			void Trim(double threshold);
			AudioBuffer^ trimmed(void);

			generic<typename FrameType>
			where FrameType : IAudioFrame
			AudioBuffer^ convert(void) {
				FrameType t = FrameType();
				au->convert( t->type.code );
				return this;
			}

            array<Stepflow::Audio::Audio^>^ split( uint atPosition, bool trim );


			// Geerbt ueber AbstractAudioBuffer

			virtual IntPtr GetRaw(void) override {
				return IntPtr(au->data);
			}

			virtual IntPtr GetRaw(int position) override {
				return IntPtr((*au)[position]);
			}

			virtual Audio^ converted(int, int, int) override;
			virtual Audio^ converted(PcmFormat) override;
			virtual Audio^ converted(AudioFrameType) override;
			virtual Audio^ converted(AudioFrameType, double amp) override;
			virtual Audio^ convert(int, int) override;
			virtual Audio^ convert(PcmFormat) override;
			virtual Audio^ convert(AudioFrameType) override;
			virtual Audio^ convert(AudioFrameType, double amp) override;
			virtual Audio^ convert(unsigned) override;
			virtual Audio^ amplified(double factor) override;
			virtual Audio^ amplify(double factor) override;
            virtual Audio^ paned(Panorama,Panorama::Axis) override;
            virtual Audio^ pan(Panorama,Panorama::Axis) override;
            virtual Audio^ paned(Panorama) override;
            virtual Audio^ pan(Panorama) override;

            // Geerbt über Audio
            virtual Audio ^ operator+(Audio ^ appendie) override;
            virtual Audio ^ operator+=(Audio ^ appendie) override;
            virtual Audio ^ append(Audio ^ appendie) override;
            virtual Audio ^ mixInto(Audio ^ trackB) override;
            virtual Audio ^ mixTrax(Audio ^ trackA, Audio ^ trackB) override;
        };
	} // end Audio
} //end Stepflow
#endif