/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WavReader.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 31.12.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WavReader_hpp_
#define _WavReader_hpp_

#include "AudioBuffer.hpp"
#include "../../src/WaveBuffers.hpp"
#include "../../src/WaveFileReader.hpp"
#include <indiaccessfuncs.h>

using namespace Stepflow::Audio::FileIO;
#define CASETYPE_COMBO CASE_TYPE::NATIVE::TY,CASE_TYPE::NATIVE::CH

namespace Stepflow
{
	namespace Audio
	{
		namespace FileIO
		{

			public ref class WaveFileReader
				: public AbstractWaveFileStream, public IAudioInStream
			{
			private:
				stepflow::WaveFileReader* native;

			public:
                WaveFileReader(void);
				WaveFileReader(System::String^ fileName);
				virtual ~WaveFileReader(void);

				virtual PcmFormat  GetFormat(void) override;
				virtual WaveHeader GetHeader(void) override;
				virtual bool       isValid(void) override;
				virtual unsigned   GetPosition(void) override;
				virtual unsigned   Open(System::String^ fileName) override;
				virtual unsigned   Close(void) override;

				unsigned       GetLength(void);
                TimeSpan       GetDuration(void);
				virtual void   Seek(unsigned position) override;

                virtual void   AttachBuffer(Audio^ attachee) override;

			    virtual	Audio^ ReadAll(void)
				{
					return gcnew AudioBuffer(native->Read());
				}
                virtual	Audio^ Read(void)
				{
					return gcnew AudioBuffer(native->Read());
				}

                virtual IAudioFrame^ ReadFrame(void)
                {
                    IAudioFrame^ frame = GetFormat().CreateEmptyFrame();
                    int c = frame->FrameType.ChannelCount;
                    switch (frame->FrameType.BitDepth) {
                    case 8: {
                        for (int i = 0; i < c; ++i)
                            frame->Channel[i] = native->ReadByte();
                    } break;
                    case 16: {
                        for (int i = 0; i < c; ++i)
                            frame->Channel[i] = native->ReadInt16();
                    } break;
                    case 24: {
                        for (int i = 0; i < c; ++i)
                            frame->Channel[i] = ReadInt24();
                    } break;
                    case 32: {
                        for (int i = 0; i < c; ++i)
                            frame->Channel[i] = native->ReadFloat();
                    } break;
                    case 64: {
                        for (int i = 0; i < c; ++i)
                            frame->Channel[i] = native->ReadDouble();
                    } break;
                    } return frame;

                }

                virtual	Audio^ Read(int FrameCount)
				{
					return gcnew AudioBuffer(native->Read(FrameCount));
				}

                generic<class fT> where fT : IAudioFrame
                virtual Audio^ ReadFrames(array<fT>^ dst) {
                    stepflow::Format fmt = *native->GetFormat();
                    if (dst[0]->FrameType.Code == native->GetTypeCode()) {
                        for (int i = 0; i < dst->Length; ++i) {
                            native->Read(dst[i]->GetRaw().ToPointer(), 0, fmt.NumChannels);
                        } 
                    } else {
                        stepflow::Audio rd = native->Read(dst->Length);
                        rd.mode.addFlag(stepflow::OWN);
                        rd.convert( dst[0]->FrameType.Code );
                        for (int i = 0; i < dst->Length; ++i) {
                            _memccpy( dst[i]->GetRaw().ToPointer(), rd[i], rd.format.BlockAlign, 1);
                        } rd.mode.addFlag( stepflow::OWN );
                        rd.drop();
                    } pin_ptr<fT> pt = &dst[0];
                    return gcnew AudioBuffer( stepflow::Audio( fmt, pt, dst->Length*fmt.BlockAlign,
                                              stepflow::DONT_ALLOCATE_NEW ).outscope() );
                }
                generic<class fT> where fT : IAudioFrame
                    virtual fT ReadFrame(fT dst) {
                    stepflow::Format fmt = *native->GetFormat();
                    if (dst->FrameType.Code == native->GetTypeCode()) {
                        native->Read( dst->GetRaw().ToPointer(), 0, fmt.NumChannels );
                    } else {
                        throw gcnew Exception("converted ReadFrame() not implemented yet");
                    } return dst;
                }
                virtual	uint Read(Audio^ buffer, int numberFrames) {
					return native->Read(buffer->native_cast(), numberFrames);
				}
                virtual	uint Read(Audio^ buffer) {
					return native->Read(buffer->native_cast());
				}
				generic<class T> where T : IAudioFrame, gcnew()
					virtual unsigned Read(array<T>^ buffer, unsigned offseTs, unsigned countOnTs) {
					pin_ptr<T> dst = &buffer[offseTs];
					unsigned frameSize = native->GetFormat()->BlockAlign;
					unsigned available = (native->GetLength() - native->GetPosition( native->GetDirection() ))*frameSize;
					unsigned length = (sizeof(T)*countOnTs);
					if (length > available)
						length = available;
					MEMCPYFUNC(dst, native->Read(available / frameSize).data, 1, length);
					return length / sizeof(T);
				}
				generic<typename T> where T : value struct
					T read(void) {
					System::Type^ t = T::typeid; 
                    if (t == Int24::typeid) {
                        return (T)static_cast<System::Object^>(Int24(native->ReadInt24()));
                    } else {
                        switch (System::Type::GetTypeCode(t)) {
                        case System::TypeCode::Byte:
                        case System::TypeCode::SByte:
                            return (T)static_cast<System::Object^>(native->ReadByte());
                        case System::TypeCode::Int16:
                        case System::TypeCode::UInt16:
                            return (T)static_cast<System::Object^>(native->ReadInt16());
                        case System::TypeCode::Int32:
                        case System::TypeCode::UInt32:
                        case System::TypeCode::Single:
                            return (T)static_cast<System::Object^>(native->ReadUInt32());
                        case System::TypeCode::UInt64:
                        case System::TypeCode::Double:
                            return (T)static_cast<System::Object^>(*(System::UInt64*)&native->ReadFrame<stepflow::i32, 2>().channel[0]);
                        default:
                            return (T)static_cast<System::Object^>(native->ReadUInt32());
                        }
                    }
				}
                byte     ReadByte(void);
                short    ReadInt16(void);
                s24      ReadInt24(void);
                unsigned ReadUInt32(void);
                float    ReadFloat(void);
                virtual  AudioFrameType GetFrameType() {
                    return this->GetFormat().FrameType;
                }
                virtual property PcmFormat Format {
                    virtual PcmFormat get(void) override {
                        return GetFormat();
                    }
                }

                // Geerbt über IAudioInStream
                virtual uint GetPosition(StreamDirection direction) { return GetPosition(); }
                virtual property StreamDirection Direction { StreamDirection get(void) { return StreamDirection::INPUT; } }
                virtual StreamDirection CanSeek(void) { return StreamDirection::READ; }
                virtual void Seek(StreamDirection dir, uint pos) { return Seek(pos); }
                virtual uint AvailableBytes(void) { return ((stepflow::AbstractWaveFileStream*)native)->getFramesAvailable() * native->GetFormat()->BlockAlign; }
                virtual uint AvailableFrames(void) { return ((stepflow::AbstractWaveFileStream*)native)->getFramesAvailable(); }
                virtual TimeSpan AvailableTime(void) { return TimeSpan::FromMilliseconds(((float)native->GetFormat()->SampleRate / ((stepflow::AbstractWaveFileStream*)native)->getFramesAvailable()) * 1000); }
                virtual uint CanStream( StreamDirection stream ) {
                    if ( stream.In( StreamDirection::INPUT ) ) {
                        return AvailableFrames();
                    } else return 0;
                }
                virtual array<IAudioFrame^>^ ReadFrames(uint count);
                virtual uint Read( IntPtr dstMem, int countBytes, int offsetDstBytes ) {
                    return native->Read( ((byte*)dstMem.ToPointer() + offsetDstBytes), countBytes / native->GetFormat()->BlockAlign ) * native->GetFormat()->BlockAlign;
                }
};
		}
	}
}//end of Stepflow

#endif
