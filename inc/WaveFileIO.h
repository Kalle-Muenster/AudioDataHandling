#ifndef _WaveFileIO_h_
#define _WaveFileIO_h_

#include "../src/WaveSpacer.h"
#include "../src/WaveMacros.hpp"
#include "../src/WaveHandling.hpp"
#include "../src/WaveFileReader.hpp"
#include "../src/WaveFileWriter.hpp"

BEGIN_WAVESPACE

// classes and structures
enum    WAVELIB_API FormatMatch :unsigned;
enum    WAVELIB_API StreamDirection :char;
enum    WAVELIB_API FXRouteType :char;
enum    WAVELIB_API Initiatio :unsigned;
enum    WAVELIB_API Constants :unsigned;
enum    WAVELIB_API FrameTypeCode :word;

struct  WAVELIB_API AudioFrameType;
struct  WAVELIB_API IAudioFrame;
class   WAVELIB_API AbstractAudioFX;
class   WAVELIB_API AbstractAudioStream;
class   WAVELIB_API AbstractWaveFileStream;
class   WAVELIB_API AbstractCycleBuffer;

class   WAVELIB_API IReadLockable;
class   WAVELIB_API IWriteLockable;

template<const unsigned cc>
class   WAVELIB_API Channel;
template<class StreamBase>
class   WAVELIB_API IAudioInStream;
template<class StreamBase>
class   WAVELIB_API IAudioOutStream;
template<class FxBase>
class   WAVELIB_API IFxSend;
template<class FxBase>
class   WAVELIB_API IFxReturn;
class   WAVELIB_API IFxInsert;


struct  WAVELIB_API BufferFlags;
struct  WAVELIB_API WavFileHeader;
struct  WAVELIB_API SndFileHeader;
#ifdef PAM_WAVEFILE_HEADERS
struct  WAVELIB_API PamFileHeader;
#endif
union   WAVELIB_API ReadOrWriteHead;

template<typename ST,const unsigned CC>
struct  WAVELIB_API Frame;
struct  WAVELIB_API Audio;
struct  WAVELIB_API Format;
struct  WAVELIB_API Panorama;
class   WAVELIB_API Circular;
class   WAVELIB_API AudioStream;
class   WAVELIB_API WaveFileReader;
class   WAVELIB_API WaveFileWriter;

// 24bit integer types (signed and unsigned)
struct  WAVELIB_API INT_24BIT;
struct  WAVELIB_API UINT_24BIT;

// Audiosample data types:
// s's are signed, i's are unsigned
typedef WAVELIB_API unsigned char  i8;
typedef WAVELIB_API signed char    s8;
typedef WAVELIB_API unsigned short i16;
typedef WAVELIB_API signed short   s16;
typedef WAVELIB_API Float16        f16;
typedef WAVELIB_API UINT_24BIT     i24;
typedef WAVELIB_API INT_24BIT      s24;
typedef WAVELIB_API unsigned int   i32;
typedef WAVELIB_API signed int     s32;
typedef WAVELIB_API unsigned long long i64;
typedef WAVELIB_API slong          s64;
typedef WAVELIB_API float          f32;
typedef WAVELIB_API double         f64;


// defined frame types
template<const unsigned cC> struct WAVELIB_API AuPCMs8;
template<const unsigned cC> struct WAVELIB_API AuPCMi8;
template<const unsigned cC> struct WAVELIB_API AuPCMs16;
template<const unsigned cC> struct WAVELIB_API AuPCMf16;
template<const unsigned cC> struct WAVELIB_API AuPCMs24;
template<const unsigned cC> struct WAVELIB_API AuPCMi24;
template<const unsigned cC> struct WAVELIB_API AuPCMs32;
template<const unsigned cC> struct WAVELIB_API AuPCMf32;
template<const unsigned cC> struct WAVELIB_API AuPCMf64;
template<const unsigned cC> struct WAVELIB_API AuPCMi64;

// loose helper functions
template<typename STy>
WAVELIB_API STy           channelMixer( STy sample, int channel, float* mixer, bool FR );
WAVELIB_API Initiatio     operator |( Initiatio This, Initiatio That );
WAVELIB_API FormatMatch   operator == ( const Format&, const Format& );
WAVELIB_API FormatMatch   operator != ( const Format&, const Format& );
WAVELIB_API double        ConversionFactor( int fromBits, int toBits );
WAVELIB_API slong         SignedTypesShift( int fromBits, int toBits );

WAVELIB_API IAudioFrame*  CreateAudioFrame( uint typecode );
WAVELIB_API IAudioFrame*  CreateManyFrames( uint typecode, uint ammount );
WAVELIB_API IAudioFrame*  CreateAudioFrame( uint typecode, Data fromRawData );
// Construct a WaveHeader (file header) structure (into *in_waveFileHeader)
WAVELIB_API WavFileHeader CreateWaveHeader( uint samplerate, word bitdepth, word channelcount,
                                            WavFileHeader* in_waveFileHeader, uint sizeOfWaveData = 0 );
// Construct a WaveHeader (file header) structure
WAVELIB_API WavFileHeader CreateWaveHeader( uint samplingFrequency, word bitsPerSample,
                                            word numberOfChannels, uint sizeOfWaveData = 0 );
WAVELIB_API WavFileHeader CreateWaveHeader( AudioFrameType frameType, uint sizeOfWaveData = 0 );
WAVELIB_API SndFileHeader CreateSndFileHdr( const Format& format, int cbSize );

// Construct a WaveFormat structure
WAVELIB_API Format        CreateWaveFormat( int samplerate, int bitdepth, int channelcount );
WAVELIB_API Format        CreateWaveFormat( int samplerate, int bitdepth, int channelcount, WAV_PCM_TYPE_ID pcmTag );
WAVELIB_API Format        CreateWaveFormat( uint typecode, int samplerate = 0 );
WAVELIB_API Data          ReverseByteOrder( Data buffer, const Format& format, uint frameCount );
WAVELIB_API std::string   PictogramFromPan( Panorama pan );
WAVELIB_API const char*   NameFromTypeCode( FrameTypeCode frametypecode );
WAVELIB_API FrameTypeCode TypeCodeFromName( const char* frametypename );

#ifdef PAM_WAVEFILE_HEADERS
WAVELIB_API PamFileHeader CreatePamFileHdr( const Format& format, int cbSize = 0, ChannelMode planar = ChannelMode::Interleaved );
#endif


#include "../src/CleanupMacros.h"
ENDOF_WAVESPACE
#endif
