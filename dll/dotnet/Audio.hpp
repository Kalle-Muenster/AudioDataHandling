/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      Audio.hpp                                  ||
||     Author:    Kalle                                      ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _Audio_hpp_
#define _Audio_hpp_

#ifdef _WIN64
#define _AMD64_ (1)
#else
#ifdef _WIN32
#define _X86_ (1)
#endif
#endif

#include <eszentielle/.CommandlinerEndian.h>

// include native wavelib c++ sources being wrapped:
#include "../../src/WaveHandling.hpp"
#if HAVE_CONTROLLED_VALUES > 0
#include "../../src/WaveFilter.hpp"
#endif

using namespace System;
using namespace stepflow;
using namespace System::Runtime::InteropServices;

#ifndef uint
typedef unsigned uint;
#endif



// macros for creating AudioFrame structures:
#include "CreationMacros.h"


namespace Stepflow {
    
    using  i8  = stepflow::i8;
    using  s8  = stepflow::s8;
    using  i16 = stepflow::i16;
    using  s16 = stepflow::s16;
    using  i24 = Stepflow::UInt24;
    using  s24 = Stepflow::Int24;
    using  i32 = stepflow::i32;
    using  s32 = stepflow::s32;
    using  i64 = stepflow::i64;
    using  s64 = stepflow::s64;
    using  f32 = stepflow::f32;
    using  f64 = stepflow::f64;

	namespace Audio {

        template<typename NAT,typename TYP>
        TYP SampleTypeConversion( Object^ sample ) {
            Type^ sT = sample->GetType();
            if ( sT == TYP::typeid ) return safe_cast<TYP>(sample);
            if ( sT == Int24::typeid )
                return TYP( stepflow::ConversionFactor(24,NAT::TS) * safe_cast<Int24>(sample) );
            else switch ( Type::GetTypeCode(sT) ) {
            case System::TypeCode::SByte: 
                return TYP(stepflow::ConversionFactor(8,NAT::TS) * safe_cast<System::SByte>(sample));
            case System::TypeCode::Int16:
                return TYP(stepflow::ConversionFactor(16,NAT::TS) * safe_cast<System::Int16>(sample));
            case System::TypeCode::Single:
                return TYP(stepflow::ConversionFactor(32,NAT::TS) * safe_cast<System::Single>(sample));
            case System::TypeCode::Double:
                return TYP(stepflow::ConversionFactor(64,NAT::TS) * safe_cast<System::Double>(sample));
            default:
                throw gcnew Exception("unsupported data type "+sT->ToString());
            }
        }

        public enum class AudioFileHeader : uint {
            Wav = stepflow::HEADER_CHUNK_TYPE::WavFormat,
            Snd = stepflow::HEADER_CHUNK_TYPE::SndFormat,
            PB7 = stepflow::HEADER_CHUNK_TYPE::P7mFormat,
            PB8 = stepflow::HEADER_CHUNK_TYPE::P8mFormat,

            WavHeaderSize = stepflow::SimpleHeaderSize,
            SndHeaderSize = stepflow::SndFileHeaderSize,
            PamHeaderSize = stepflow::PamFileHeaderSize
        };

		interface class IAudioFrame;
        value struct AudioFrameType;
        public enum class PcmTag : word {
            PCMi = 0, PCMs = 1, PCMf = 3
        };
		[StructLayoutAttribute(LayoutKind::Sequential, Size = 16)]
		public value struct PcmFormat {
		public:
            PcmTag Tag;
            System::UInt16 NumChannels;
            System::UInt32 SampleRate;
            System::UInt32 ByteRate;
            System::UInt16 BlockAlign;
            System::UInt16 BitsPerSample;
			IAudioFrame^   CreateEmptyFrame(void); 
            property AudioFrameType FrameType { AudioFrameType get(void); }
            static PcmFormat Create(int srt, int bit, int chn);
            static PcmFormat Create(AudioFrameType type, word rate);
		};


		[StructLayoutAttribute(LayoutKind::Explicit, Size = sizeof(unsigned long long))]
		public value struct Panorama 
		{
		public:
            typedef stepflow::Panorama NATIVE;

            const static Panorama Neutral = Panorama( 0.5f, 0.5f );
            const static Panorama LeftSided = Panorama( NATIVE::LEFT|NATIVE::NONFRONTAL );
            const static Panorama RightSided = Panorama( NATIVE::RIGHT|NATIVE::NONFRONTAL );
            const static Panorama FrontSided = Panorama( NATIVE::CENTERED|NATIVE::FRONT );
            const static Panorama RearSided = Panorama( NATIVE::CENTERED|NATIVE::REAR );

			[FieldOffsetAttribute(0)]
			unsigned long long data;
			[FieldOffsetAttribute(0)]
			float LR;
			[FieldOffsetAttribute(4)]
			float FR;

		internal:

			virtual NATIVE* native_cast() {
                pin_ptr<System::UInt64> d = &data;
				return (NATIVE*)d;
			}
			Panorama( NATIVE& copy ) {
				data = copy.data.I64;
			}
            NATIVE& operator ->() {
                pin_ptr<System::UInt64> d = &data;
				return *(NATIVE*)d;
			}
            NATIVE* operator &() {
                return native_cast();
			}
            NATIVE& operator *() {
                pin_ptr<System::UInt64> d = &data;
                return *(NATIVE*)d;
			}

		public:
            [FlagsAttribute()]
			enum class Axis : char { Any = 0,
				LeftRight = NATIVE::Axis::LeftToRight,
				FrontRear = NATIVE::Axis::FrontToRear
			};

			Panorama(float mix) {
				data = NATIVE(mix).data.I64;
			}
			Panorama(float leftright, float frontrear) {
				data = NATIVE(leftright,frontrear).data.I64;
			}
			Panorama(float mix, Axis axe) {
				data = NATIVE(mix, NATIVE::Axis(axe)).data.I64;
			}
			Panorama(array<float>^ mixer) {
				LR = mixer[0];
				FR = mixer[1];
			}
			
            float getAxis(Axis direction) {
                return direction == Axis::LeftRight ? LR : FR;
			}

            property float LeftSpeaker {
                float get(void) { return native_cast()->Left[NATIVE::Absolute]; }
                void set(float value) { native_cast()->Left[NATIVE::Subractive] = value; }
            }
            property float RightSpeaker {
                float get(void) { return native_cast()->Right[NATIVE::Absolute]; }
                void set(float value) { native_cast()->Right[NATIVE::Subractive] = value; }
            }
            property float FrontSpeaker {
                float get(void) { return native_cast()->Front[NATIVE::Absolute]; }
                void set(float value) { native_cast()->Front[NATIVE::Subractive] = value; }
            }
            property float RearSpeaker {
                float get(void) { return native_cast()->Rear[NATIVE::Absolute]; }
                void set(float value) { native_cast()->Rear[NATIVE::Subractive] = value; }
            }

            bool effectsAxis( Axis axis ) {
                switch (axis) {
                    case Axis::Any:       return native_cast()->isEffective();
                    case Axis::LeftRight: return native_cast()->effectsStereo();
                    case Axis::FrontRear: return native_cast()->effectsFrontal();
                    default: throw gcnew System::Exception(
                        System::String::Format("unknown 'Axis' {0}", axis)
                                                            );
                }
            }

            /// <summary>Returns 'true' if this <class>Panorama</class> is effective at all, to either stero, frontal or to both axis.
            ///  So it returns 'false' on a 'neutral' <class>Panorama</class> which wouldn't cause any changes when applied to an
            /// <class>IAudioFrame</class> or to an <class>AudioBuffer</class> </sumary>
            bool isEffective() {
                return native_cast()->isEffective();
            }

            /// <summary>Creates a new <class>Panorama</class> from this <class>Panorama</class> which only
            /// will be effective on that axis which is specified by given <parameter>Axis</parameter> parameter</sumary>
            template<const Axis axe> 
            Panorama forAxis(void) {
                switch(axe) {
                case Axis::LeftRight: return NATIVE::extract_stereo( this->data );
                case Axis::FrontRear: return NATIVE::extract_frontal( this->data );
                case Axis::Any:       return *this;
                default: throw gcnew System::Exception(
                        System::String::Format( "unknown 'Axis' {0}", axis )
                    );
                }
            }
            
            
            /// <summary>
            /// Create a new Panorama from this Panorama which only is effective on the frontal axis.
            /// </summary>
            Panorama frontal(void) {
                return Panorama( NATIVE::extract_frontal( this->data ) );
            }
            /// <summary>
            /// Create a new Panorama from this Panorama which is only effective on the stereo axis.
            /// </summary>
            Panorama stereo(void) {
                return Panorama( NATIVE::extract_stereo( this->data ) );
            }
            /// <summary>
            /// Return new panorama but with both axis exchanged against each other
            /// </summary>
            Panorama flipped(void) {
                return Panorama( native_cast()->flipped() );
            }
            /// <summary>Exchange this panorama LeftRight axis with this panorama FrontRear axis</summary>
            Panorama% flip(void) {
                data = native_cast()->flipped().data.I64;
                return *this;
            }
            /// <summary>Invert this panoram's stereo axis</sumary>
            Panorama% flipStereo(void) {
                data = native_cast()->flipped(NATIVE::Axis::LeftToRight).data.I64;
                return *this;
            }
            /// <summary>Invert this panoram's frontal axis</sumary>
            Panorama% flipFrontal(void) {
                data = native_cast()->flipped(NATIVE::Axis::FrontToRear).data.I64;
                return *this;
            }

            virtual String^ ToString(void) override {
                return String::Format( "Panorama:\nraw value: {0}\nAxis: LR {1}, FB {2}\nSides: L:{3}, R:{4}, F:{5}, B:{6}",
                                       data, LR, FR, LeftSpeaker, RightSpeaker, FrontSpeaker, RearSpeaker );
            }

            String^ ToPictogram(void) {
                return gcnew String( native_cast()->getPictogram().data(), 0, 98 );
            }

            array<float>^ ToBalanceArray(void) {
                return gcnew array<float>(2) { LR, FR };
            }

            array<float>^ ToVolumesArray(void) {
                // due to the 'r' identifier here already is used for the 'rear' factor, 
                // here 'o' ('other left') is used as identifier for the 'right' factor:
                float f = FR < 0.5f ? 2.0f * FR : 1.0f;
                float r = FR > 0.5f ? 2.0f * (1.0f - FR) : 1.0f;
                float l = LR < 0.5f ? 2.0f * LR : 1.0f;
                float o = LR > 0.5f ? 2.0f * (1.0f - LR) : 1.0f;  
                return gcnew array<float>(4) { l*f, o*f, l*r, o*r };
            }

			static bool operator <(Panorama This, Panorama That) {
				return *This < *That;
			}
			static bool operator >(Panorama This, Panorama That) {
				return *This > *That;
			}
			static bool operator ==(Panorama This, Panorama That) {
				return *This == *That;
			}
			static bool operator !=(Panorama This, Panorama That) {
				return *This != *That;
			}
            
            void addStep(Panorama step) {
                native_cast()->addStep( step.data );
            }

			static Panorama operator +(Panorama This, Panorama That) {
				return Panorama((*This) + (*That));
			}
			static Panorama operator *(Panorama This, Panorama That) {
				return Panorama((*This) * (*That));
			}
			static Panorama operator -(Panorama This, Panorama That) {
				return Panorama((*This) - (*That));
			}
			static Panorama operator /(Panorama This, Panorama That) {
				return Panorama((*This) / (*That));
			}
			static Panorama operator *(Panorama ElPan, float scalar) {
				return Panorama((*ElPan) * scalar);
			}
			static Panorama operator /(Panorama ElPan, float scalar) {
				return Panorama((*ElPan) / scalar);
			}

            static Panorama MorphPoint( Panorama from, Panorama till, double lerp ) {
                from.data = NATIVE::morphPoint(
                    reinterpret_cast<NATIVE&>(from.data),
                    reinterpret_cast<NATIVE&>(till.data),
                lerp ).data.I64;
                return from;
            }
            static Panorama MovingStep( Panorama from, Panorama till, uint steps ) {
                from.data = NATIVE::movingStep(
                    reinterpret_cast<NATIVE&>(from.data),
                    reinterpret_cast<NATIVE&>(till.data),
                steps ).data.I64;
                return from;
            }

		};

        interface class IAudioFrame;
		[StructLayoutAttribute(LayoutKind::Explicit, Size = 4, Pack = 1)]
		const public value struct AudioFrameType {
        public:
            [FieldOffsetAttribute(0)]
            const uint code;

            property word BitDepth {
                word get(void) {
                    return native().BitDepth();
                }
            }
            property word Code {
                word get(void) {
                    return native().info.type.code;
                }
            }
			property word SampleSize {
				word get(void) {
                    return native().BitDepth() >> 3;
				}
			}
            property word ChannelCount {
                word get(void) {
                    return native().Channels(); 
                }
            }
			property word BlockAlign {
				word get(void) {
					return native().BitDepth() >> 3 
                         * native().Channels();
				}
			}
            property short Signednes {
                short get(void) {
                    return native().IsSigned();
                }
            }
            property PcmTag PcmTypeTag {
                PcmTag get(void) {
                    return (PcmTag)native().FormatTag();
                }
            }
			virtual System::String^ ToString( void ) override	{
				return System::String::Format( "Stepflow.Audio.FrameTypes.Au{0}{1}bit{2}ch",
                       PcmTypeTag.ToString(), BitDepth.ToString(), ChannelCount.ToString() );                
			}
            static operator AudioFrameType( word typecode ) {
                return AudioFrameType( typecode );
            }
            PcmFormat    CreateFormatStruct( int sampleRate );
            IAudioFrame^ CreateEmptyFrame( void );
            IAudioFrame^ CreateAudioFrame( IntPtr rawdata );

		internal:
            const stepflow::AudioFrameType native() {
                return stepflow::AudioFrameType::fromTypeCode( code );
            }
            AudioFrameType( const stepflow::AudioFrameType& cpy ) 
                : code( cpy.info.data ) {
            }
            AudioFrameType( uint pcmTag, int bitdepth, int channelCount, word srt )
                : AudioFrameType( stepflow::AudioFrameType(WAV_PCM_TYPE_ID(pcmTag),
                                  uint(bitdepth), uint(channelCount), uint(srt) ) ) {
            }
			AudioFrameType( int bitdepth, int channelCount ) 
                : AudioFrameType( bitdepth>24?3:1, bitdepth, channelCount, 0 ) {
			}
            AudioFrameType( word typecode )
                : AudioFrameType(stepflow::AudioFrameType(typecode, 0)) {
            }
		};

        public interface class IAudioFrame {
        public:

            const static AudioFrameType type;

            property AudioFrameType FrameType {
                AudioFrameType get();
            };
            
            property Object^ Channel[int]{
                Object ^ get( int );
                void set( int, Object^ );
            }

            IntPtr       GetRaw( void );
            void         SetChannel( int channel, IntPtr ptrToValue );
            IntPtr       GetPointer( int channel );

            /// <summary> multiplicationResult(IAudioFrame):
            /// ( operator * (operant) ) </summary>
            /// <param name="modulative">frame where to multiply by</param>
            /// <returns>the result of multiplying this frame by a 'modulative' frame...
            /// This frame will stay untouched </returns>
            IAudioFrame^ multiplicationResult( IAudioFrame^ modulative );

            /// <summary> additionResult(IAudioFrame):
            /// ( operator + (operant) ) </summary>
            /// <param name="additional">frame which will be added</param>
            /// <returns> the summ of this frame and 'additional' frame...
            /// This frame will stay untouched </returns>
            IAudioFrame^ additionResult( IAudioFrame^ additional );

            /// <summary> subtractionResult(IAudioFrame):
            /// ( operator - (operant) ) </summary>
            /// <param name="extractive">frame which will be subtracted</param>
            /// <returns>The result of removing 'extractive' frame from 'this' frame...
            /// This frame will stay untouched </returns>
            IAudioFrame^ subtractionResult( IAudioFrame^ extractive );

            /// <summary> Multiply(IAudioFrame):
            /// ( operator *= (operant) ) </summary>
            /// <param name="modulation">frame to be multiplied by</param>
            /// <returns>this frame after assigning result of multiplication
            /// by 'modulation' frame to it... This frame will change its value!
            /// </returns>
            IAudioFrame^ Multiply( IAudioFrame^ modulation );

            /// <summary> Add(IAudioFrame):
            /// ( operator += (operant) ) </summary>
            /// <param name="additive">frame which will be added to this</param>
            /// <returns>This frame after assigning summarization to it... This
            /// frame will change its value!</returns>
            IAudioFrame^ Add( IAudioFrame^ additive );

            /// <summary> Subtract(IAudioFrame):
            /// ( operator -= (operant) ) </summary>
            /// <param name="extractive">frame which value will be
            /// removed from this frames value</param>
            /// <returns>This frame after having 'that' frame subtracted from it...
            /// This frame will change its value!</returns>
            IAudioFrame^ Subtract( IAudioFrame^ extractive );

            /// <summary> inverse()
            /// ( operator - (self) ) </summary>
            /// <returns> this frame's inversed counterpart </returns>
            IAudioFrame^ inverse( void );

            void         Clear();
            IAudioFrame^ Amp( float amplifier );
            IAudioFrame^ Mix( Object^ sample, Panorama mixer );
            IAudioFrame^ Mix( IAudioFrame^ frame, float mix );
            IAudioFrame^ Pan( Panorama mixer );
            IAudioFrame^ Pan( float balance, char axis );
            IAudioFrame^ Pan( Panorama splatMix, Panorama::Axis splitAxt );
            IAudioFrame^ Convert( AudioFrameType toFrameType );
            IAudioFrame^ Clone( void );
            Panorama     GetPanorama( void );

            IAudioFrame^ Set( IAudioFrame^ );
            void         SetRaw( IntPtr data );

            static operator AudioFrameType( IAudioFrame^ cast ) {
                return cast->FrameType;
            }
        };

		namespace FileIO {

			public enum class HeaderChunkType : unsigned {
				RIFF = stepflow::HEADER_CHUNK_TYPE::RIFFChunk,
				data = stepflow::HEADER_CHUNK_TYPE::DataChunk,
				fmrt = stepflow::HEADER_CHUNK_TYPE::FrmtChunk,
				fact = stepflow::HEADER_CHUNK_TYPE::FactChunk,
			};

			[StructLayoutAttribute(LayoutKind::Sequential, Size = 8)]
			public value class SubChunk {
			public:
                HeaderChunkType   type;
                System::UInt32    size;

				byte* Content(void) {
					pin_ptr<SubChunk> ptr = this;
					return (byte*)&ptr[1];
				}

				SubChunk* NextChunk(void) {
					return (SubChunk*)(Content() + size);
				}

				unsigned ToUInt32(void) { 
                    return size; 
                }

				int ToInt32(void) {
                    return (int)size;
                }

				virtual System::String^ ToString(void) override {
                    return type.ToString();
                }
			};

			[StructLayoutAttribute(LayoutKind::Sequential, Size = 44)]
			public value class WaveHeaderSp {
			public:
				SubChunk       RiffChunk;
                System::UInt32 FileFormat;
				SubChunk       FormatChunk;
				PcmFormat      AudioFormat;
				SubChunk       DataChunk;
			};

			[StructLayoutAttribute(LayoutKind::Explicit, Size = 88)]
			public value class WaveHeaderEx {
			public:
				[FieldOffsetAttribute(0)]  SubChunk        RiffChunk;  
				[FieldOffsetAttribute(8)]  System::UInt32  FileFormat;
				[FieldOffsetAttribute(12)] SubChunk        FormatChunk;
				[FieldOffsetAttribute(20)] PcmFormat       AudioFormat;
				[FieldOffsetAttribute(36)] SubChunk        FactChunk;  
				[FieldOffsetAttribute(44)] byte*           FactContent;
				[FieldOffsetAttribute(80)] SubChunk        DataChunk;  
			};

			[StructLayoutAttribute(LayoutKind::Explicit, Size = 88)]
			public value struct WaveHeader
            {
			public:
				[FieldOffsetAttribute(0)]
				WaveHeaderSp simple;
				[FieldOffsetAttribute(0)]
				WaveHeaderEx extend;

				WaveHeader(AudioFrameType% frametype, int sampleRate, int framecount) {
					simple.RiffChunk.type            = HeaderChunkType::RIFF;
					simple.RiffChunk.size            = (System::UInt32)(framecount*frametype.BlockAlign + 40);
					simple.FileFormat                = (System::UInt32)FOURCC('W', 'A', 'V', 'E');
					simple.FormatChunk.type          = HeaderChunkType::fmrt;
					simple.FormatChunk.size          = (System::UInt32)16;
					simple.AudioFormat.Tag           = (PcmTag)(frametype.BitDepth > 24 ? 3 : 1);
					simple.AudioFormat.NumChannels   = (System::UInt16)frametype.ChannelCount;
					simple.AudioFormat.SampleRate    = (System::UInt32)sampleRate;
					simple.AudioFormat.ByteRate      = (System::UInt32)(frametype.BlockAlign * sampleRate);
					simple.AudioFormat.BlockAlign    = (System::UInt16)frametype.BlockAlign;
					simple.AudioFormat.BitsPerSample = (System::UInt32)frametype.BitDepth;
					simple.DataChunk.type            = HeaderChunkType::data;
					simple.DataChunk.size            = (System::UInt32)(framecount * frametype.BlockAlign);
				}
				WaveHeader( AudioFrameType% frametype, int sampleRate, int framecount, IntPtr ptFacillities ) 
					: WaveHeader(frametype,sampleRate,framecount) {
					extend.RiffChunk.size           += 44; 
					extend.FactChunk.type            = HeaderChunkType::fact;
					extend.FactChunk.size            = 36;
					memcpy(extend.FactChunk.Content(), ptFacillities.ToPointer(), extend.FactChunk.size);
					extend.DataChunk.type            = HeaderChunkType::data;
					extend.DataChunk.size            = extend.RiffChunk.size - 84;
				}
				virtual System::String^ ToString(void) override;
			};

            public enum class SndTypeTag : uint {
                MULAW_8 = 1,
                LINEAR_8 = 2,
                LINEAR_16 = 3,
                LINEAR_24 = 4,
                LINEAR_32 = 5,
                FLOAT = 6,
                DOUBLE = 7,
                ALAW_8 = 8
            };

            [StructLayoutAttribute( LayoutKind::Sequential, Size = (int)AudioFileHeader::SndHeaderSize)]
            public value struct SndHeader {
            public:
                AudioFileHeader   SndTag;          //    0      4    fourCC    ".snd"
                uint              HeaderSize;      //    4      4    uint      Offset to start of data (should be 28 byte)
                uint              DataSize;        //    8      4    uint      Number of bytes of data
                SndTypeTag        FormatCode;      //    12     4    uint      Data format code
                uint              SampleRate;      //    16     4    uint      Sampling rate
                uint              NumChannels;     //    20     4    uint      Number of channels
                property word     BitDepth { word get(void); }
                property AudioFrameType FrameType { AudioFrameType get(void); }
                static SndHeader FromRawData( IntPtr data );
            };

            public ref class PamHeader {
            protected:
                stepflow::PamFileHeader* native;
                PamHeader( stepflow::PamFileHeader* hdr ) {
                    if( hdr->isValid() ) {
                        native = hdr;
                    } else {
                        *hdr = stepflow::wirdPassendGemacht( &hdr->hdr.dat[0] );
                        native = hdr;
                    } native->makeValueBased();
                }
            public:
                PamHeader( AudioFrameType from, int srt ) {
                    native = new PamFileHeader();
                    PcmFormat fmt = from.CreateFormatStruct(srt);
                    pin_ptr<PcmFormat> ptr = &fmt;
                    stepflow::initializePamFileHeader( native, (const stepflow::Format*)ptr );
                }
                virtual String^ ToString(void) override {
                    return gcnew String( native->makeStringBased() );
                }
                array<byte>^ ToByteArray(void) {
                    array<byte>^ dst = gcnew array<byte>(122);
                    byte* src = (byte*)native->makeStringBased();
                    for (int i = 0; i < 122; ++i) {
                        dst[i] = src[i];
                    } return dst;
                }
                static PamHeader^ FromBytes( array<byte>^ filedata ) {
                    PamFileHeader* native = new PamFileHeader();
                    for (int i = 0; i < filedata->Length && i < 256; ++i) {
                        native->hdr.dat[i] = (char)filedata[i];
                    } return gcnew PamHeader( native );
                }
                static PamHeader^ FromString( String^ filedata) {
                    array<wchar_t>^ data = filedata->ToCharArray();
                    PamFileHeader* native = new PamFileHeader();
                    for (int i = 0; i < data->Length && i < 256; ++i) {
                        native->hdr.dat[i] = (char)data[i];
                    } return gcnew PamHeader( native );
                }
                static PamHeader^ FromData( IntPtr ptData ) {
                    PamFileHeader* native = new PamFileHeader();
                    byte* dst = (byte*)&native->hdr.dat[0];
                    byte* src = (byte*)ptData.ToPointer();
                    do { *dst++ = *src++; } while ( *src != '\0' );
                    return gcnew PamHeader( native );
                }
                property int BitDepth {
                    int get(void) { return native->BitDepth; } 
                    void set(int value) { native->BitDepth = value; }
                }
                property int Channels {
                    int get(void) { return native->ChannelCount; }
                    void set(int value) { native->ChannelCount = value; }
                }
                property int DataSize {
                    int get(void) { return native->DataSize; }
                    void set(int value) { native->DataSize = value; }
                }
                property int FrameCount {
                    int get(void) { return native->Value[PamFileHeader::WIDTH]; }
                    void set(int value) { native->Value[PamFileHeader::WIDTH] = value; }
                }
                property int SampleRate {
                    int get(void) { return native->SampleRate; }
                    void set(int value) { native->SampleRate = value; }
                }
                property AudioFrameType FrameType {
                    AudioFrameType get(void) {
                        return AudioFrameType( native->GetTypeCode() );
                    }
                }
            };
		}//end of FileIO

        // 'IAudioFrame' concrete implementations of meta type for all possible
        // 'DataType at ChannelConfiguration' variants, done the CreationMacro:
		namespace FrameTypes {

			DefineAudioFrameTypeStructure(s,8, 1);
			DefineAudioFrameTypeStructure(s,8, 2);
			DefineAudioFrameTypeStructure(s,8, 4);
			DefineAudioFrameTypeStructure(s,8, 6);
			DefineAudioFrameTypeStructure(s,8, 8);

			DefineAudioFrameTypeStructure(s,16, 1);
			DefineAudioFrameTypeStructure(s,16, 2);
			DefineAudioFrameTypeStructure(s,16, 4);
			DefineAudioFrameTypeStructure(s,16, 6);
			DefineAudioFrameTypeStructure(s,16, 8);

            DefineAudioFrameTypeStructure(s,24, 1);
            DefineAudioFrameTypeStructure(s,24, 2);
            DefineAudioFrameTypeStructure(s,24, 4);
            DefineAudioFrameTypeStructure(s,24, 6);
            DefineAudioFrameTypeStructure(s,24, 8);

			DefineAudioFrameTypeStructure(f,32, 1);
			DefineAudioFrameTypeStructure(f,32, 2);
			DefineAudioFrameTypeStructure(f,32, 4);
			DefineAudioFrameTypeStructure(f,32, 6);
			DefineAudioFrameTypeStructure(f,32, 8);

			DefineAudioFrameTypeStructure(f,64, 1);
			DefineAudioFrameTypeStructure(f,64, 2);
			DefineAudioFrameTypeStructure(f,64, 4);
			DefineAudioFrameTypeStructure(f,64, 6);
			DefineAudioFrameTypeStructure(f,64, 8);

		}


        ref struct Audio;

        public ref class IChunkable
            : public System::Collections::Generic::IEnumerator<Audio^> {
        public:
            property Audio^ First { virtual Audio^ get() abstract; }
            property Audio^ Last { virtual Audio^ get() abstract; }
            virtual Audio^  Next() abstract;
            virtual bool    MoveNext() abstract;
            virtual Object^ GetCurrent() = System::Collections::IEnumerator::Current::get{ return Current; };
            virtual void    Reset() abstract;
            virtual ~IChunkable() {};
            property Audio^ Current { virtual Audio^ get() abstract; }
            property int    Count { virtual int get() abstract; }
            property int    Index { virtual int get() abstract; }
            virtual property Audio^ default[int] {
                Audio^ get(int idx) abstract;
                void   set(int idx, Audio^ audio) abstract;
            }
        };

		// abstract base class where audio buffers derive from:
		public ref struct Audio abstract {
		public:
			[System::FlagsAttribute()]
			enum class Flags {
				DEFAULT           = stepflow::DEFAULT,
				RAW_FILE_DATA     = stepflow::RAW_FILE_DATA,
				DONT_ALLOCATE_NEW = stepflow::DONT_ALLOCATE_NEW,
				CIRCULAR          = stepflow::CIRCULAR,
				OWN               = stepflow::OWN,
                NEW               = stepflow::ALLOCATE_NEW_COPY
			};

			virtual property PcmFormat     Format;
			virtual property uint          FrameCount;
            virtual property TimeSpan      Duration;
			virtual property uint          DataSize;
			virtual property uint          TypeCode;
            virtual property IChunkable^   Chunk;

            AudioFrameType GetFrameType() { 
                return AudioFrameType( WAV_PCM_TYPE_ID((word)Format.Tag),
                    Format.BitsPerSample, Format.NumChannels, Format.SampleRate );
            };

			// these functions retreiving IntPtr to the audio's 'raw' buffer memory
			virtual IntPtr GetRaw(void)         abstract;
			virtual IntPtr GetRaw(int position) abstract;

			// manipulating and conversion functions (the 'ed' ended versions
			// will leave instances as are, returning new allocated copies)
			virtual Audio^ converted(int frq, int bit, int chn) abstract;
			virtual Audio^ converted(PcmFormat format) abstract;
			virtual Audio^ converted(AudioFrameType frameType) abstract;
			virtual Audio^ converted(AudioFrameType, double amp) abstract;
			virtual Audio^ convert(int bit, int chn) abstract;
			virtual Audio^ convert(PcmFormat format) abstract;
			virtual Audio^ convert(AudioFrameType frameType) abstract;
			virtual Audio^ convert(AudioFrameType, double amp) abstract;
			virtual Audio^ convert(unsigned typecode) abstract;
            
			virtual Audio^ amplified(double factor) abstract;
			virtual Audio^ amplify(double factor) abstract;
            virtual Audio^ paned(Panorama panorama,Panorama::Axis) abstract;
            virtual Audio^ pan(Panorama panorama,Panorama::Axis) abstract;
            virtual Audio^ paned(Panorama panorama) abstract;
            virtual Audio^ pan(Panorama panorama) abstract;
            virtual IAudioFrame^ GetFrame(uint idx) abstract;
            virtual void   SetFrame(uint idx, IAudioFrame^ frame) abstract;

            virtual Audio^ operator+(Audio^ appendie) abstract;
            virtual Audio^ operator+=(Audio^ appendie) abstract;
            virtual Audio^ append(Audio^ appendie) abstract;
            virtual Audio^ mixInto(Audio^ trackB) abstract;
            virtual Audio^ mixTrax(Audio^ trackA, Audio^ trackB) abstract;

            array<IAudioFrame^>^ ToFrameArray(void) {
                return (array<IAudioFrame^>^)Marshal::GetObjectsForNativeVariants(GetRaw(), FrameCount );
            }
            generic<class FT> where FT : IAudioFrame, gcnew()
                array<FT>^ toArrayOf(void) { // TODO: type error at runtime
                    return (array<FT>^)Marshal::GetObjectsForNativeVariants( GetRaw(), (FrameCount*Format.BlockAlign)/sizeof(FT) );
            }

            static void SetLogWriter(Action<String^>^ logwrite) {
                debuglogout = logwrite;
            }

            void Compact(void) {
                native_pntr()->compact();
            }

		internal:
			virtual stepflow::Audio  native_cast(void) = 0;
            virtual stepflow::Audio* native_pntr(void) = 0;
            static const char* convertFromSystemString( char* dst, System::String^ systring );
            static Action<System::String^>^ debuglogout;
			static operator stepflow::Audio(Audio^ cast) {
				return cast->native_cast();
			}
		};


        [StructLayoutAttribute(LayoutKind::Explicit, Size = 1)]
        public value struct StreamDirection 
        {
        public:
            const static StreamDirection NONE = StreamDirection(0);
            const static StreamDirection READ = StreamDirection(stepflow::StreamDirection::READ);
            const static StreamDirection INPUT = StreamDirection(stepflow::StreamDirection::READ);
            const static StreamDirection WRITE = StreamDirection(stepflow::StreamDirection::WRITE);
            const static StreamDirection OUTPUT = StreamDirection(stepflow::StreamDirection::WRITE);
            const static StreamDirection INOUT = StreamDirection(stepflow::StreamDirection::INOUT);

            operator byte() {
                return data;
            }
            operator bool() {
                return data != NONE.data;
            }
            bool In(StreamDirection direction) {
                return (data & direction.data) != NONE.data;
            }
            
        private:
            StreamDirection(int direction)
                : data((byte)direction) {
            }
            [FieldOffsetAttribute(0)]
            char data;
        };

        public interface class IAudioStream
        {
        public:
            property StreamDirection Direction {
                StreamDirection get(void);
            }
            AudioFrameType  GetFrameType(void);
            PcmFormat       GetFormat(void);
            uint            GetPosition(StreamDirection);
            StreamDirection CanSeek( void );
            void            Seek( StreamDirection, uint );
            uint            CanStream( StreamDirection );
        };

        public interface class IAudioOutStream
            : public IAudioStream
        {
        public:
            const static StreamDirection Direction = StreamDirection::OUTPUT;

            uint WrittenBytes(void);
            uint WrittenFrames(void);
            TimeSpan WrittenTime(void);

            uint Write(IntPtr rawData, int countBytes, int offsetSrcBytes);
            uint Write(Audio^ srcBuffer, int countFs, int FsOffsetSrc);

            uint WriteAudio(Audio^ buffer);
            uint WriteFrame(IAudioFrame^ frame);
            uint WriteSample(short);
            uint WriteSample(float);

            uint WriteFrame(stepflow::s16 sample);
            uint WriteFrame(stepflow::f32 sample);
            uint WriteFrame(stepflow::s16 sample, Panorama mixer);
            uint WriteFrame(stepflow::f32 sample, Panorama mixer);
            uint WriteFrame(stepflow::f64 sample, Panorama mixer);
        };

        public interface class IAudioInStream
            : public IAudioStream
        {
        public:
            const static StreamDirection Direction = StreamDirection::INPUT;

            uint AvailableBytes(void);
            uint AvailableFrames(void);
            TimeSpan AvailableTime(void);

            Audio^               ReadAll(void);
            Audio^               Read(void);
            Audio^               Read(int frames);
            IAudioFrame^         ReadFrame(void);
            array<IAudioFrame^>^ ReadFrames( uint count );
            uint     Read( IntPtr dstMem, int countBytes, int offsetDstBytes );
            uint     Read( Audio^ buffer, int numberFrames );
            uint     Read( Audio^ buffer );
        };
		
        namespace FileIO
        {
            public enum class FileFormat : uint {
                WAV = stepflow::AbstractWaveFileStream::WAV,
                SND = stepflow::AbstractWaveFileStream::SND,
                PAM = stepflow::AbstractWaveFileStream::PAM,
            };

            // base class for the WaveFileReader and WaveFileWriter 
            public ref class AbstractWaveFileStream abstract {
            public:
                virtual bool           isValid(void) abstract;
                virtual PcmFormat      GetFormat(void) abstract;
                virtual WaveHeader     GetHeader(void) abstract;
                virtual unsigned       GetPosition(void) abstract;
                virtual void           AttachBuffer(Audio^) abstract;
                virtual unsigned       Open(System::String^ fileName) abstract;
                virtual unsigned       Close(void) abstract;
            };
        }

	} //end of Audio
}//end of Stepflow

#endif
