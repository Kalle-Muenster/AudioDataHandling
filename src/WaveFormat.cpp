/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFormat.cpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 24.09.2016                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveSpacer.h"
#include <precomphead.hpp>
#ifndef  USE_PRE_COMPILED_HEADER
#include <limits.h>
#include <memory.h>

#include <WaveLib.inl/enumoperators.h>
#include <WaveLib.inl/indiaccessfuncs.h>
#include <WaveLib.inl/int24bittypes.hpp>

#include "WaveMacros.hpp"
#include "Panorama.hpp"
#include "WaveFormat.hpp"
#include "AudioChannel.hpp"
#include "WaveAudio.hpp"
#endif


const static WaveSpace(s64) NullBuffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
const WaveSpace(Audio)::NoData WaveSpace(Audio)::Silence = (void*)&NullBuffer[0];

#define OneLine(b,c,t,B,C,T) {\
    {Au_PCM ## t ## _ ## b ## _ ## c },\
    { "Au_PCM" ## T ## "_" ## B ## "_" ## C }\
}

#define OneType(bi,ty,BI,TY) \
    OneLine(bi,1,ty,BI,"1",TY),\
    OneLine(bi,2,ty,BI,"2",TY),\
    OneLine(bi,4,ty,BI,"4",TY),\
    OneLine(bi,6,ty,BI,"6",TY),\
    OneLine(bi,8,ty,BI,"8",TY)

struct FrameType {
    const WaveSpace(FrameTypeCode) code;
    const char* name;
};

BEGIN_WAVESPACE
const static FrameType AudioFrameTypes[] = {
    OneType(8,i,"8","i"),
    OneType(8,s,"8","s"),
    OneType(16,s,"16","s"),
    OneType(24,i,"24","i"),
    OneType(24,s,"24","s"),
    OneType(32,f,"32","f"),
    OneType(64,f,"64","f"),
{{FrameTypeCode(0)},{false}} };

const char* NameFromTypeCode( FrameTypeCode frametypecode ) {
    const FrameType* frametype = &AudioFrameTypes[0];
    while( frametype->code )
       if( frametype->code == frametypecode ) break;
       else ++frametype;
    return frametype->name;
}

FrameTypeCode TypeCodeFromName( const char* frametypename ) {
    FrameTypeCode code = FrameTypeCode( atoi( frametypename ) );
    if ( NameFromTypeCode( code ) ) return code;
    const FrameType* frametype = &AudioFrameTypes[0];
    const ulong find = (*(ulong*)(frametypename + 5) & 0x00ffffffffffffffu);
    while( frametype->code )
       if( find == (*(ulong*)(frametype->name + 5) & 0x00ffffffffffffffu) ) break;
    else ++frametype;
    return frametype->code;
}
ENDOF_WAVESPACE
#undef OneType
#undef OneLine


std::string
WaveSpace(PictogramFromPan)( Panorama pan ) {
    char pict[100] = "FL----F----FR\n|           |\n|           |\nL           R\n|           |\n|           |\nRL----R----RR\n\0";
    float LR = pan.get(Panorama::LeftToRight);
    float FB = pan.get(Panorama::FrontToRear);
    LR = LR < 0 ? 0 : LR > 1 ? 1 : LR;
    FB = FB < 0 ? 0 : FB > 1 ? 1 : FB;
    pict[ 1+(int)( LR * 11.0f ) + (14 * ((int)( (1.0f-FB)*5.0f)+1) )] = 'X';
    return std::string( &pict[0] );
}

static WaveSpace(WavFileHeader) WavFileHeaderInit = WaveSpace(WavFileHeader) {
    WaveSpace(HEADER_CHUNK_TYPE)::RIFFChunk, WaveSpace(Constants)::RIFFChunkSize,
    WaveSpace(HEADER_CHUNK_TYPE)::WavFormat, WaveSpace(HEADER_CHUNK_TYPE)::FrmtChunk,
    WaveSpace(Constants)::FormatChunkSize, { WaveSpace(WAV_PCM_TYPE_ID)::PCMs,
    2, 44100, 176400, 4, 16 }, WaveSpace(HEADER_CHUNK_TYPE)::DataChunk, EMPTY,
    { 0,0,0,0,0,0,0,0 }
};

static WaveSpace(SndFileHeader) SndFileHeaderInit = {
    WaveSpace(HEADER_CHUNK_TYPE)::SndFormat, uint(28), uint(0),
    WaveSpace(SND_PCM_TYPE_ID)::LINEAR_8, uint(44100), uint(2),
    { 0,0,0,0,0,0,0,0 }
};

#ifdef PAM_WAVEFILE_HEADERS
const static char PamFileHeaderInitStr[] = "P7_#RATE 44100   _TUPLTYPE 16912        _MAXVAL 32767               _DEPTH 2  _HEIGHT 1  _WIDTH 1                 _ENDHDR__";
const static byte PamFileHeaderOffsets[] = { 3, 18, 41, 69, 79, 90, 115, 122 };
const static byte PamFileHeaderLangths[] = { 14, 22, 27, 9, 10, 24, 6, 0 };

void WaveSpace(initializePamFileHeader)( PamFileHeader* create, const Format* format, ChannelMode mode )
{
    STR_STRCPY( 256, &create->hdr.dat[0], &PamFileHeaderInitStr[0] );
    char* str = &create->hdr.dat[0];
    while (*str) { if (*str == '_') *str = '\0';
          ++str; }
    if (format) {
        create->Value[PamFileHeader::TUPLTYPE] = word( AUDIOFRAME_CODE(
            format->BitsPerSample, format->NumChannels, format->PCMFormatTag ) );
        create->Value[PamFileHeader::RATE] = format->SampleRate;
        uint d = format->BitsPerSample == 64 ? 2 : 1;
        uint maxv = format->BitsPerSample < 16
                  ? s8_MAX
                  : format->BitsPerSample < 24
                  ? s16_MAX
                  : format->BitsPerSample < 32
                  ? s24_MAX : i32_MAX;
        create->Value[PamFileHeader::MAXVAL] = maxv;
        if ( mode == ChannelMode::Planar ) {
            create->Value[PamFileHeader::HEIGHT] = format->NumChannels;
            create->Value[PamFileHeader::DEPTH] = d;
        } else {
            create->Value[PamFileHeader::DEPTH] = format->NumChannels * d;
            create->Value[PamFileHeader::HEIGHT] = 1;
        }        
    }
}
#endif

WaveSpace(Format)
WaveSpace(CreateWaveFormat)( int frq, int bit, int chn )
{
    return CreateWaveFormat( frq, bit, chn, EMPTY_(WAV_PCM_TYPE_ID) );
}

WaveSpace(Format)
WaveSpace(CreateWaveFormat)( int frq, int bit, int chn, WAV_PCM_TYPE_ID tag )
{
    word pcm = is_val( tag ) ? tag : word(bit >= 32 ? (bit <= 64 ? 3 : bit) : 1);
    return { WAV_PCM_TYPE_ID(pcm), word(chn), uint(frq), uint(frq*chn*(bit >> 3)), word(chn*(bit >> 3)),
             word(bit) };
}

WaveSpace(Format)
WaveSpace(CreateWaveFormat)( uint formatCode, int frq )
{
    word pcm = AUDIO_SIGN_FROM_TYPECODE(formatCode)
             ? AUDIO_FLOATTYPE_TYPECODE(formatCode) ? 3 : 1 : 0;
    word blk = AUDIO_FRAMESIZE_TYPECODE(formatCode);
    return{ WAV_PCM_TYPE_ID(pcm), AUDIO_CHANCOUNT_TYPECODE(formatCode), uint(frq), uint(frq*blk),
            blk, AUDIO_BITS_FROM_TYPECODE(formatCode) };
}


WAVELIB_API WaveSpace(FormatMatch)
WaveSpace(operator ==)(const Format& This, const Format& That)
{
    return FormatMatch( (This.NumChannels == That.NumChannels) ? CHANNELCOUNT : 0
                      |  (This.SampleRate == That.SampleRate) ? SAMPLERATE : 0
                      | ( (This.BitsPerSample == That.BitsPerSample)
                        && (This.PCMFormatTag == That.PCMFormatTag) )
                      ? DATATYPE
                      : 0 );
}

WAVELIB_API WaveSpace(FormatMatch)
WaveSpace(operator !=)(const Format& This, const Format& That)
{
    return FormatMatch( DIRECTCOPY & ~(This == That) );
}

WaveSpace(WavFileHeader)
WaveSpace(initializeNewWaveheader)(unsigned filesize)
{
    WavFileHeader* pt = (WaveSpace(WavFileHeader*))&WavFileHeaderInit;
    pt->ChunkHeader.size = filesize;
    return *pt;
}

WaveSpace(WavFileHeader*)
WaveSpace(getWaveheaderProtostructure)(void)
{ return (WaveSpace(WavFileHeader*))&WavFileHeaderInit; }

WaveSpace(WavFileHeader*)
WaveSpace(prepareInitializedHeader)(WaveSpace(WavFileHeader*) whdr, unsigned dataSize)
{
    whdr->AudioFormat.PCMFormatTag = whdr->AudioFormat.BitsPerSample >= 32
                                   ? WAV_PCM_TYPE_ID::PCMf
                                   : WAV_PCM_TYPE_ID::PCMs;

    whdr->AudioFormat.BlockAlign = (i16)( (whdr->AudioFormat.BitsPerSample >> 3)
                                         * whdr->AudioFormat.NumChannels );

    whdr->AudioFormat.ByteRate = whdr->AudioFormat.SampleRate
                               * whdr->AudioFormat.BlockAlign;

    if (whdr->ChunkHeader.type == HEADER_CHUNK_TYPE::FactChunk ) {
        whdr->DataChunk.extended.ChunkHeader.size =
            dataSize == EMPTY ? whdr->DataChunk.extended.ChunkHeader.size
                              : dataSize;
    } else {
        whdr->ChunkHeader.size = dataSize == EMPTY
                               ? whdr->ChunkHeader.size
                               : unsigned( dataSize );
    }
    return whdr;
}

WAVELIB_API WaveSpace(WavFileHeader)
WaveSpace(CreateWaveHeader)( uint frq, word bit, word chn, uint cbsize )
{
    WavFileHeader whdr = initializeNewWaveheader( cbsize );
    whdr.AudioFormat.NumChannels = chn;
    whdr.AudioFormat.SampleRate = frq;
    whdr.AudioFormat.BitsPerSample = bit;
    return *prepareInitializedHeader( &whdr, cbsize );
}

WAVELIB_API WaveSpace(WavFileHeader)
WaveSpace(CreateWaveHeader)( uint frq, word bit, word chn,  WavFileHeader* hdr, uint cbs ) {
     hdr->AudioFormat.NumChannels = chn;
     hdr->AudioFormat.SampleRate = frq;
     hdr->AudioFormat.BitsPerSample = bit;
     return *prepareInitializedHeader( hdr, cbs );
 }

static WaveSpace(WavFileHeader)*
_assignWhdr( WaveSpace(WavFileHeader)* This, const WaveSpace(WavFileHeader)* That )
{
    if( That->ChunkHeader.type == WaveSpace(HEADER_CHUNK_TYPE)::DataChunk )
        return (WaveSpace(WavFileHeader*)) memcpy( This, That, WaveSpace(SimpleHeaderSize) );
    else
        return (WaveSpace(WavFileHeader*)) memcpy( This, That, WaveSpace(ExtendedHeaderSize) );
}

void
WaveSpace(reverseSndHeader)( SndFileHeader* sndhdr )
{
    byte* it = (byte*)sndhdr;
    const byte* end = it + ( SndFileHeaderSize + ReadHeadSize );
    for( ; it!=end; it+=4 ) {
         byte store = it[0];
         it[0] = it[3];
         it[3] = store;
         store = it[1];
         it[1] = it[2];
         it[2] = store;
    }
}

WaveSpace(Data)
WaveSpace(ReverseByteOrder)( Data buffer, const Format& format, uint frameCount )
{
    if (format.BitsPerSample > 8) {
        byte reversor[8] = { 0 };
        const uint bytePerSample = ( format.BitsPerSample >> 3 );
        const uint end = frameCount;
        for (uint i = 0; i < end; ++i) {
            byte* curFrame = (byte*)buffer+(i*format.BlockAlign);
            for (uint c = 0; c < format.NumChannels; ++c) {
                const uint channel = bytePerSample * c;
                uint bd = bytePerSample;
                for (uint bs = 0; bs < bytePerSample; ++bs) {
                    reversor[--bd] = curFrame[channel + bs];
                }
                for (uint b = 0; b < bytePerSample; ++b) {
                    curFrame[channel + b] = reversor[b];
                }
            }
        }
    } return buffer;
}


WAVELIB_API WaveSpace(WavFileHeader)&
WaveSpace(WavFileHeader)::operator= ( WaveSpace(WavFileHeader) that )
{
    return *_assignWhdr( this, &that );
}


WAVELIB_API WaveSpace(SndFileHeader)
WaveSpace(CreateSndFileHdr)( const WaveSpace(Format&) fmt, int cbs )
{
    SndFileHeader whdr = SndFileHeaderInit;
    whdr.DataSize      = cbs;
    whdr.NumChannels   = fmt.NumChannels;
    whdr.SampleRate    = fmt.SampleRate;
    switch ( fmt.BitsPerSample ) {
        case 8:  whdr.FormatCode = LINEAR_8; break;
        case 16: whdr.FormatCode = LINEAR_16; break;
        case 24: whdr.FormatCode = LINEAR_24; break;
        case 32: whdr.FormatCode = FLOAT; break;
        case 64: whdr.FormatCode = DOUBLE; break;
        default: whdr.FormatCode = EMPTY_(SND_PCM_TYPE_ID); break;
    } return whdr;
}

// retreive a the conversion factor by which samples can
// be converted to other type by being multiplyed by it.
#define ConversionCase(fr,to) (fr|(to<<8))
WAVELIB_API double
WaveSpace(ConversionFactor)(int fromBits, int toBits)
{
    if (fromBits == toBits)
        return 1.0;

    switch ( ConversionCase(fromBits, toBits) )
    {
        case ConversionCase( 8,16):
        case ConversionCase(16,24): return 256.0;
        case ConversionCase( 8,24): return 65536.0;
        case ConversionCase( 8,32):
        case ConversionCase( 8,64): return 1.0 / 127.0;
        case ConversionCase(16, 8):
        case ConversionCase(24,16): return 1.0 / 256.0;
        case ConversionCase(16,32):
        case ConversionCase(16,64): return 1.0 / (double)s16_MAX;
        case ConversionCase(24, 8): return 1.0 / 65536.0;
        case ConversionCase(24,32):
        case ConversionCase(24,64): return 1.0 / (double)s24_MAX;
        case ConversionCase(32, 8):
        case ConversionCase(64, 8): return (double)s8_MAX;
        case ConversionCase(32,16):
        case ConversionCase(64,16): return (double)s16_MAX;
        case ConversionCase(32,24):
        case ConversionCase(64,24): return (double)s24_MAX;
        case ConversionCase(32,64):
        case ConversionCase(64,32): return 1.0;
                           default: return -1.0;
    }
}
#undef ConversionCase

WAVELIB_API slong
WaveSpace(SignedTypesShift)(int fromBits, int toBits)
{
    if ( toBits == fromBits )
        return 0;
    uint tBits = *(uint*)&toBits;
    const unsigned SIGNMASK = 0x80000000;
    uint toSign = (tBits & SIGNMASK);
    if ( toSign == (*(uint*)&fromBits & SIGNMASK) )
        return 0;
    tBits &= SIGNMASK;
    if (toSign)
        tBits = (-s32_MIN) - tBits;
    slong returnum;
    switch (tBits) {
        case  8: returnum =  s8_MIN; break;
        case 16: returnum = s16_MIN; break;
        case 24: returnum = s24_MIN; break;
        case 32: returnum = s32_MIN; break;
        default: return 0;
    }
    return toSign
         ? -returnum
         :  returnum;
}


WAVELIB_API WaveSpace(Initiatio)
WaveSpace(operator |)(Initiatio This, Initiatio That) {
    return Initiatio((unsigned)This | (unsigned)That);
}


WaveSpace(HEADER_CHUNK_TYPE)
WaveSpace(WavFileHeader)::GetFileFormat(void) const
{
    return (HEADER_CHUNK_TYPE)FileFormat;
}

word
WaveSpace(WavFileHeader)::GetHeaderSize(void) const
{
    return ChunkHeader.type == DataChunkID
         ? SimpleHeaderSize
         : ExtendedHeaderSize;
}
uint
WaveSpace(WavFileHeader)::GetDataSize(void) const
{
    return ChunkHeader.type == FactChunkID
         ? DataChunk.extended.ChunkHeader.size
         : ChunkHeader.size;
}
WaveSpace(Data)
WaveSpace(WavFileHeader)::GetAudioData(void) const
{
    return ChunkHeader.type == FactChunkID
         ? (Data)&DataChunk.extended.ReadHead
         : (Data)&DataChunk.simple.ReadHead;
}

void
WaveSpace(WavFileHeader)::GetFormat(Format* getter) const
{
    *getter = AudioFormat;
}

bool
WaveSpace(WavFileHeader)::isValid(void) const {
    return (( FileFormat == WavFormat )
           && NameSpace(is_val)( AudioFormat.PCMFormatTag ));
}

word
WaveSpace(WavFileHeader)::GetChannelCount(void) const
{
    return AudioFormat.NumChannels;
}

const WaveSpace(AudioFrameType)
WaveSpace(WavFileHeader)::GetTypeCode(void) const
{
    return FrameTypeCode( AUDIOFRAME_CODE(
        AudioFormat.BitsPerSample,
        AudioFormat.NumChannels,
        AudioFormat.PCMFormatTag
    ));
}

bool
WaveSpace(WavFileHeader)::isFloatType(void) const
{
    return AudioFormat.PCMFormatTag == WAV_PCM_TYPE_ID::PCMf;
}

bool
WaveSpace(WavFileHeader)::isSignedType(void) const
{
    return !(AudioFormat.PCMFormatTag & WAV_PCM_TYPE_ID::PCMs);
}

word
WaveSpace(WavFileHeader)::GetBitDepth(void) const
{
    return AudioFormat.BitsPerSample;
}
word
WaveSpace(WavFileHeader)::GetBlockAlign(void) const
{
    return AudioFormat.BlockAlign;
}
uint
WaveSpace(WavFileHeader)::GetSampleRate(void) const
{
    return (uint)AudioFormat.SampleRate;
}

WaveSpace(HEADER_CHUNK_TYPE)
WaveSpace(SndFileHeader)::GetFileFormat(void) const
{
    return SndTag;
}

word
WaveSpace(SndFileHeader)::GetHeaderSize(void) const
{
    return HeaderSize;
}

uint
WaveSpace(SndFileHeader)::GetDataSize(void) const
{
    return DataSize;
}

WaveSpace(Data)
WaveSpace(SndFileHeader)::GetAudioData(void) const
{
    return (Data)&ReadHead;
}

void
WaveSpace(SndFileHeader)::GetFormat( Format* getter ) const
{
    int bitdepth;
    switch (FormatCode) {
    case ALAW_8:
    case MULAW_8:
    case LINEAR_8:  bitdepth = 8;  break;
    case LINEAR_16: bitdepth = 16; break;
    case LINEAR_24: bitdepth = 24; break;
    case LINEAR_32:
    case FLOAT:     bitdepth = 32; break;
    case DOUBLE:    bitdepth = 64; break;
    default:        bitdepth = EMPTY_(word);
    } *getter = CreateWaveFormat( SampleRate,
           bitdepth, NumChannels );
    getter->PCMFormatTag = ( FormatCode & (FLOAT|DOUBLE) )
         ? WAV_PCM_TYPE_ID::PCMf
         : WAV_PCM_TYPE_ID::PCMs;
}

bool
WaveSpace(SndFileHeader)::isValid(void) const
{
    Format check;
    GetFormat( &check );
    word tag = (FormatCode & (FLOAT|DOUBLE))
             ? WAV_PCM_TYPE_ID::PCMf
             : WAV_PCM_TYPE_ID::PCMs;
    return SndTag == SndFormat
        && HeaderSize == SndFileHeaderSize
        && tag == check.PCMFormatTag;
}

bool
WaveSpace(SndFileHeader)::isFloatType(void) const
{
    return FormatCode & (FLOAT|DOUBLE);
}

bool
WaveSpace(SndFileHeader)::isSignedType(void) const
{
    return true;
}

word
WaveSpace(SndFileHeader)::GetChannelCount(void) const
{
    return NumChannels;
}

const WaveSpace(AudioFrameType)
WaveSpace(SndFileHeader)::GetTypeCode(void) const
{
    word tag = (FormatCode & (FLOAT|DOUBLE))
             ? WAV_PCM_TYPE_ID::PCMf
             : WAV_PCM_TYPE_ID::PCMs;
    FrameTypeCode code;
    switch( FormatCode ) {
        case MULAW_8:
        case ALAW_8:
        case LINEAR_8:
            code = FrameTypeCode(AUDIOFRAME_CODE(8,NumChannels,tag));
        case LINEAR_16:
            code = FrameTypeCode(AUDIOFRAME_CODE(16,NumChannels,tag));
        case LINEAR_24:
            code = FrameTypeCode(AUDIOFRAME_CODE(24,NumChannels,tag));
        case LINEAR_32:
        case FLOAT:
            code = FrameTypeCode(AUDIOFRAME_CODE(32,NumChannels,tag));
        case DOUBLE:
            code = FrameTypeCode(AUDIOFRAME_CODE(64,NumChannels,tag));
        default: code = EMPTY_(FrameTypeCode);
    } return code;
}

word
WaveSpace(SndFileHeader)::GetBitDepth(void) const
{
    return GetTypeCode().BitDepth();
}

word
WaveSpace(SndFileHeader)::GetBlockAlign(void) const
{
    return NumChannels * ( GetBitDepth() / 8 );
}

uint
WaveSpace(SndFileHeader)::GetSampleRate(void) const
{
    return (uint)SampleRate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PAM_WAVEFILE_HEADERS
void WaveSpace(PamFileHeader)::set_Value( Values index, uint set )
{
    int size = (index & 0xff000000) >> 24;
    const char* str = &hdr.dat[index & 0x0000ffff] + size;
    size = ((index & 0x00ff0000) >> 16) - size;
    char* data = (char*)(str - 1);
    while (*++data) *data = ' ';
    data = (char*)str;
    char bufdat[32];
    if ( index == Values::TUPLTYPE ) {
        str = NameFromTypeCode(
        FrameTypeCode(set) );
    } else {
        LONG_TOSTR( set, &bufdat[0], 32, 10 );
        str = (const char*)&bufdat[0];
    } while (*str) *data++ = *str++;
}

uint WaveSpace(PamFileHeader)::get_Value(Values index) const
{
    const char* data = &hdr.dat[index & 0x0000ffff]
        + ((index & 0xff000000) >> 24);
    return (index == Values::TUPLTYPE)
         ? TypeCodeFromName( data )
         : (uint)atoll( data );
}

WaveSpace(PamFileHeader)
WaveSpace(wirdPassendGemacht)(const char* nichtPassend)
{
    const word tag7 = *(word*)"P7";
    const word tag8 = *(word*)"P8";
    const long type = *(long*)"TUPLTYPE";
    const uint rate = *(uint*)"#RAT";
    const uint mval = *(uint*)"MAXV";
    const uint dept = *(uint*)"DEPT";
    const uint wids = *(uint*)"WIDT";
    const uint high = *(uint*)"HEIG";
    const uint size = strlen(nichtPassend);
    PamFileHeader pam;
    if (size == 122) {
        initializePamFileHeader(&pam);
        STR_STRCPY(256, &pam.hdr.dat[0], nichtPassend);
        pam.makeValueBased();
        return pam;
    }
    char buffer[64]; char* buf = &buffer[0];
    Format fmt;
    fmt.SampleRate = 0; fmt.NumChannels = 0;
    HEADER_CHUNK_TYPE typ = HEADER_CHUNK_TYPE(0);
    FrameTypeCode tupl = FrameTypeCode(0);
    int maxval = 0; int length = 0; ChannelMode mod = ChannelMode(0);
    for (int i = 0; i < size; ++i) {
        if (typ == 0) {
            if (*(word*)&nichtPassend[i] == tag7) typ = HEADER_CHUNK_TYPE::P7mFormat;
            else if (*(word*)&nichtPassend[i] == tag8) typ = HEADER_CHUNK_TYPE::P8mFormat;
        }
        if (tupl == 0) {
            if (*(long*)&nichtPassend[i] == type) {
                const char* str = &nichtPassend[i] + 8;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == ' ' || *buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } tupl = TypeCodeFromName(buf);
            }
        }
        if (fmt.SampleRate == 0) {
            if (*(uint*)&nichtPassend[i] == rate) {
                const char* str = &nichtPassend[i] + 5;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } fmt.SampleRate = atoi(buf);
            }
        }
        if (maxval == 0) {
            if (*(uint*)&nichtPassend[i] == mval) {
                const char* str = &nichtPassend[i] + 5;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == ' ' || *buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } maxval = atoi(buf);
            }
        }
        if (fmt.NumChannels == 0) {
            if (*(uint*)&nichtPassend[i] == dept) {
                const char* str = &nichtPassend[i] + 6;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == ' ' || *buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } fmt.NumChannels = atoi(buf);
                if (fmt.NumChannels == 1) fmt.NumChannels = 0;
                else mod = ChannelMode::Interleaved;
            }
            else if (*(uint*)&nichtPassend[i] == high) {
                const char* str = &nichtPassend[i] + 7;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == ' ' || *buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } fmt.NumChannels = atoi(buf);
                if (fmt.NumChannels == 1) fmt.NumChannels = 0;
                else mod = ChannelMode::Planar;
            }
        }
        if (length == 0) {
            if (*(uint*)&nichtPassend[i] == wids) {
                const char* str = &nichtPassend[i] + 6;
                buf = &buffer[0];
                while (*buf = *++str) {
                    if (*buf == ' ' || *buf == '\n') {
                        *buf = 0; buf = &buffer[0]; break;
                    }
                    else ++buf;
                } length = atoi(buf);
            }
        }
    } fmt = CreateWaveFormat(fmt.SampleRate, 24, fmt.NumChannels);
    initializePamFileHeader(&pam, &fmt, mod);
    pam.hdr.tag = typ;
    pam.makeValueBased();
    pam.DataSize = length;
    return pam;
}

WAVELIB_API WaveSpace(PamFileHeader)
WaveSpace(CreatePamFileHdr)( const WaveSpace(Format&) fmt, int datasize, ChannelMode mod )
{
    PamFileHeader whdr;
    AudioFrameType type(fmt);
    datasize /= fmt.BlockAlign;
    datasize = datasize < 1 ? 1 : datasize;
    initializePamFileHeader( &whdr, &fmt, mod );
    whdr.Value[PamFileHeader::WIDTH] = datasize;
    whdr.makeStringBased();
    return whdr;
}

WaveSpace(PamFileHeader)&
WaveSpace(PamFileHeader)::operator=( const PamFileHeader& copy )
{
    bool state = !copy.isStringBased();
    PamFileHeader* nonconst = const_cast<PamFileHeader*>( &copy );
    const char* src = nonconst->makeStringBased();
    STR_STRCPY( strlen(src), &hdr.dat[0], src );
    if ( state ) nonconst->makeValueBased();
    return *this;
}

word
WaveSpace(PamFileHeader)::GetHeaderSize(void) const
{
    bool state = !isStringBased();
    PamFileHeader* nonconst = const_cast<PamFileHeader*>( this );
    word size = (word)strlen( nonconst->makeStringBased() );
    if ( state ) nonconst->makeValueBased();
    return size;
}

uint
WaveSpace(PamFileHeader)::GetDataSize(void) const
{
    return Value[WIDTH] * GetBlockAlign();
}

WaveSpace(Audio)::Data
WaveSpace(PamFileHeader)::GetAudioData(void) const
{
    return (Audio::Data)&hdr.dat[HeaderSize];
}

void
WaveSpace(PamFileHeader)::GetFormat( Format* getter ) const
{
    const AudioFrameType type = GetTypeCode();
    getter->BitsPerSample = type.BitDepth();
    getter->SampleRate    = GetSampleRate();
    getter->NumChannels   = type.Channels();
    getter->BlockAlign    = type.ByteSize();
    getter->ByteRate      = getter->SampleRate
                          * getter->BlockAlign;
}

bool
WaveSpace(PamFileHeader)::isValid(void) const
{
    return ( hdr.tag == HEADER_CHUNK_TYPE::P7mFormat
        || ( hdr.tag == HEADER_CHUNK_TYPE::P8mFormat )
        && ( GetAudioData() != Audio::Silence )
        && ( GetBlockAlign() == ( GetChannelCount() * (GetBitDepth() >> 3) ) ));
}

word
WaveSpace(PamFileHeader)::GetBitDepth(void) const
{
    uint find = Value[MAXVAL];
    if (find == i32_MAX) find = ( 32 *
        ( Value[DEPTH] / Value[HEIGHT] ) );
    else if (find <= i8_MAX)  return 8;
    else if (find <= i16_MAX) return 16;
    else if (find <= i24_MAX) return 24;
    return ( find > 64 ) ? 64 
           : find > 0 
           ? find : 32;
}

word
WaveSpace(PamFileHeader)::GetBlockAlign(void) const
{
    return ChannelCount * (BitDepth / 8);
}

word
WaveSpace(PamFileHeader)::GetChannelCount(void) const
{
    const AudioFrameType typ = GetTypeCode();
    word d = typ.BitDepth() == 64 ? 2 : 1;
    word find = word( Value[DEPTH] );
    if ( find == d ) find = word( Value[HEIGHT] );
    else find /= d;
    return find;
}

const WaveSpace(AudioFrameType)
WaveSpace(PamFileHeader)::GetTypeCode(void) const
{
    const char* str = &hdr.dat[TUPLTYPE & 0x0000ffff] + 9;
    return TypeCodeFromName( str );
}

bool
WaveSpace(PamFileHeader)::isFloatType(void) const
{
    return GetTypeCode().FormatTag() == 3;
}

bool
WaveSpace(PamFileHeader)::isSignedType(void) const
{
    return GetTypeCode().IsSigned();
}

uint
WaveSpace(PamFileHeader)::GetSampleRate(void) const
{
    return Value[RATE];
}

WaveSpace(HEADER_CHUNK_TYPE)
WaveSpace(PamFileHeader)::GetFileFormat(void) const
{
    return hdr.tag;
}

WaveSpace(ChannelMode)
WaveSpace(PamFileHeader)::GetChannelMode(void) const
{
    const AudioFrameType typ = GetTypeCode();
    if ( typ.Channels() == 1 ) return ChannelMode::Planar;
    int check = typ.BitDepth() == 64 ? 2 : 1;
    return ( (Value[DEPTH] == check) && (Value[HEIGHT] > 1) )
         ? ChannelMode::Planar : ChannelMode::Interleaved;
}

void
WaveSpace(PamFileHeader)::SetChannelMode( WaveSpace(ChannelMode) set )
{
    if ( set == GetChannelMode() ) return;
    const AudioFrameType typ = GetTypeCode();
    uint d = typ.BitDepth() == 64 ? 2 : 1;
    switch( set ) {
        case ChannelMode::Planar: {
            Value[DEPTH] = d;
            Value[HEIGHT] = ChannelCount;
        } break;
        case ChannelMode::Interleaved: {
            Value[DEPTH] = d * ChannelCount;
            Value[HEIGHT] = 1;
        } break;
    }
}

#define SwitchHeaderMode(idx,to) *(&hdr.dat[idx & 0x0000ffff] + ((idx & 0x00ff0000) >> 16)) = to
const char*
WaveSpace(PamFileHeader)::makeStringBased(void)
{
    if( !isStringBased() ) {
        for ( int i = 0; i < PamFileHeaderSize; ++i )
            if (hdr.dat[i] == '\0') hdr.dat[i] = '\n';
        SwitchHeaderMode( ENDHDR, '\0' );
    } return (const char*)&hdr.dat[0];
}

void
WaveSpace(PamFileHeader)::makeValueBased(void)
{
    if ( isStringBased() ) {
        for (int i = 0; i < PamFileHeaderSize; ++i)
            if (hdr.dat[i] == '\n') hdr.dat[i] = '\0';
        SwitchHeaderMode(ENDHDR, '\n');
    }
}
#undef SwitchHeaderMode

bool
WaveSpace(PamFileHeader)::isStringBased(void) const
{
    return hdr.dat[2] == '\n';
}

WaveSpace(PamFileHeader)::operator const PamFileHeader()
{
    makeValueBased();
    return *const_cast<const PamFileHeader*>(this);
}
WaveSpace(PamFileHeader)::operator PamFileHeader&() const
{
    return *const_cast<PamFileHeader*>(this);
}
WaveSpace(PamFileHeader)::operator const PamFileHeader*()
{
    makeValueBased();
    return const_cast<const PamFileHeader*>(this);
}
WaveSpace(PamFileHeader)::operator PamFileHeader*() const
{
    return const_cast<PamFileHeader*>(this);
}

void
WaveSpace(PamFileHeader)::SetSampleRate( uint set )
{
    Value[RATE] = set;
}

void
WaveSpace(PamFileHeader)::SetBitDepth( word set )
{
    ulong max = 0;
    const AudioFrameType typ = GetTypeCode();
    if ( set == 64 ) {
        max = i32_MAX;
        if ( typ.Channels() > 1 && Value[HEIGHT] == 1 ) {
            Value[DEPTH] = typ.Channels() * 2;
        } else {
            Value[DEPTH] = 2;
        }
    } else { max = set == 16
        ? std::numeric_limits<s16>::max()
                 : set == 24
        ? std::numeric_limits<s24>::max()
                 : set == 32
        ? std::numeric_limits<i32>::max()
        : std::numeric_limits<s8>::max();
        if ( typ.BitDepth() == 64 ) {
            uint d = (Value[DEPTH] / 2);
            Value[DEPTH] = d < 1 ? 1 : d;
        }
    } Value[MAXVAL] = max;
    updateTypeCode();
}

void
WaveSpace(PamFileHeader)::updateTypeCode( void )
{
    AudioFrameType typ( BitDepth, ChannelCount );
    char* str = &hdr.dat[(TUPLTYPE & 0x0000ffff) + 8];
    while (*++str) *str = ' ';
    str = &hdr.dat[(TUPLTYPE & 0x0000ffff) + 9];
    STR_STRCPY( 22, str, NameFromTypeCode( typ.Code() ) );
    str[strlen(str)] = ' ';
    hdr.dat[(TUPLTYPE & 0x0000ffff) + ((TUPLTYPE & 0xff000000) >> 24)] = '\0';
}

void
WaveSpace(PamFileHeader)::SetChannelCount( word set )
{
    if ( GetChannelMode() == ChannelMode::Planar && ChannelCount > 1 ) {
        Value[HEIGHT] = set;
    } else {
        uint d = BitDepth == 64 ? 2 : 1;
        Value[DEPTH] = set * d;
    } updateTypeCode();
}

void
WaveSpace(PamFileHeader)::SetPcmFlags( WAV_PCM_TYPE_ID pcmflags )
{
    uint isbits = BitDepth;
    if (!(pcmflags & 1)) {
        ulong max = isbits == 8
            ? i8_MAX
            : isbits == 16
            ? i16_MAX
            : isbits == 24
            ? i24_MAX : i32_MAX;
        Value[MAXVAL] = max;
        updateTypeCode();
    } else BitDepth = isbits;
}

void 
WaveSpace(PamFileHeader)::SetDataSize( uint cbSize )
{
    cbSize /= GetBlockAlign();
    Value[WIDTH] = cbSize > 1
                 ? cbSize : 1;
}

#endif
