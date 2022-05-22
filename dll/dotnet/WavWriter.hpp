/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WavWriter.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 31.12.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WavWriter_hpp_
#define _WavWriter_hpp_

#include "Audio.hpp"
#include "../../src/WaveFileWriter.hpp"

using namespace Stepflow::Audio::FileIO;

namespace Stepflow
{
	namespace Audio
	{
		typedef stepflow::f64 f64;

		namespace FileIO
		{
			public ref class WaveFileWriter
				: public AbstractWaveFileStream
                , public IAudioOutStream
			{
			private:
				stepflow::WaveFileWriter* native;

			public:
				enum class StreamingMode {
					Store = stepflow::WaveFileWriter::Store,
					Stream = stepflow::WaveFileWriter::Stream
				};
                WaveFileWriter( void );
				WaveFileWriter( System::String^ fileName );
				WaveFileWriter( System::String^ fileName, int rate, int depth, int chan );
				WaveFileWriter( System::String^ fileName, PcmFormat% format );
                WaveFileWriter( Audio^ store, System::String^ fileName );

				virtual        ~WaveFileWriter(void);

				virtual bool           isValid(void) override;
				virtual PcmFormat      GetFormat(void) override { return this->Format; }
				virtual WaveHeader     GetHeader(void) override { return *(WaveHeader*)native->GetHeader(); }
				virtual unsigned       GetPosition(void) override { return this->WrittenFrames(); }
                virtual void           Seek(uint position) override { native->Seek(position,stepflow::WRITE); }
				virtual unsigned       Open(System::String^ fileName) override;
                        unsigned       Open(System::String^ newFile, PcmFormat% newFormat);
				virtual unsigned       Close(void) override;
                virtual void           AttachBuffer(Audio^) override;
                virtual AudioFrameType GetFrameType() {
                    PcmFormat fmt = this->GetFormat();
                    return AudioFrameType(fmt.BitsPerSample, fmt.NumChannels);
                }

				void             ReStart(bool rewriteHeader);
				unsigned         Save( Audio^, System::String^, FileFormat );
                unsigned         Save( Audio^, System::String^ );
				unsigned         Flush(void);
                virtual unsigned Write(Audio^ srcBuffer, int countFs, int FsOffsetSrc) override;
				unsigned         WriteSample(stepflow::s8);
                virtual	unsigned WriteSample(short);
                unsigned         WriteSample(s24);
				unsigned         WriteSample(System::UInt32);
                virtual	unsigned WriteSample(float);
				virtual	unsigned WriteAudio(Audio^ buffer);
				virtual	unsigned WriteFrame(IAudioFrame^ frame);
				        unsigned FlushFrame(void);
				virtual	unsigned WriteFrame(short);
				virtual	unsigned WriteFrame(float);
				virtual	unsigned WriteFrame(short sample, Panorama mixer);
				virtual	unsigned WriteFrame(float sample, Panorama mixer);
				virtual	unsigned WriteFrame(f64 sample, Panorama mixer);
				virtual	unsigned WrittenBytes(void);
				virtual	unsigned WrittenFrames(void);
				virtual	TimeSpan WrittenTime(void);
				virtual unsigned CanStream(StreamDirection stream) override;
				void             AttachBuffer(System::IntPtr buffer, uint cbSize);

				generic<class T>
					void AttachBuffer(array<T>^ data)
					{
						const int Tsize = sizeof(T);
                        native->GetFormat()->BitsPerSample = Tsize * 8;
						pin_ptr<T> pinned = &data[0];
						native->attachBuffer( stepflow::Audio( *native->GetFormat(),
							(stepflow::Audio::Data)pinned,
							data->Length*Tsize,
							stepflow::DONT_ALLOCATE_NEW)
						);
					}


                    generic<class T> where T : value struct
                    virtual uint Write(array<T>^ data, unsigned TsOffset, unsigned countOnTs) override {
                        pin_ptr<T> ptr = &data[TsOffset];
                        return native->Write(ptr, (countOnTs * sizeof(T)) / native->GetFormat()->BlockAlign);
                    }

					// write buffer pointed by given IntPtr to file and discard it as soon it has been written completely.
					generic<class T>
					unsigned Write(System::IntPtr data, unsigned TsOffset, unsigned countOnTs)
					{
						byte* buffer = (byte*)data.ToPointer() + (TsOffset * sizeof(T));
						uint r = native->Write((void*)buffer, (countOnTs * sizeof(T)) / native->GetFormat()->BlockAlign);
						delete data.ToPointer();
						return r;
					}

					property StreamingMode Mode
					{
						StreamingMode get(void) {
							return StreamingMode(native->getMode());
						}
						void set(StreamingMode value) {
							native->setMode(stepflow::WaveFileWriter::StreamingMode((uint)value));
						}
					}

					property PcmFormat Format 
					{
						virtual PcmFormat get(void) override {
							return *(PcmFormat*)native->GetFormat();
						}
						virtual void set(PcmFormat value) override {
							*native->GetFormat() = *(stepflow::Format*)&value;
						}
					}

					property System::IO::FileInfo^ FileInfo
					{
						System::IO::FileInfo^ get(void) {
							return gcnew System::IO::FileInfo(gcnew System::String(native->GetFileName()));
						}
					}

                    // Geerbt über IAudioOutStream
                    virtual uint GetPosition(StreamDirection direction) { return GetPosition(); }
					virtual property StreamDirection Direction { StreamDirection get(void) { return StreamDirection::OUTPUT; } }
                    virtual StreamDirection CanSeek(void) { return StreamDirection::WRITE; }
                    virtual void Seek(StreamDirection direction, uint position) { Seek(position); }
                    virtual uint Write(IntPtr rawData, int countBytes, int offsetSrcBytes);
};
		}
	}
}//end of Stepflow

#endif
