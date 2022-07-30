/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFormat.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 24.09.2016                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFormat_hpp_
#define _WaveFormat_hpp_

#ifndef   USE_PRECOMPILED_PROJECT_HEADER
#include <exception>
#endif

#ifndef _MSVC
#include <WaveLib.inl/enumoperators.h>
#include <WaveLib.inl/int24bittypes.hpp>
#endif

BEGIN_WAVESPACE


    typedef void*      Data;
    struct WAVELIB_API Audio;
    struct WAVELIB_API AbstractAudioFileHeader;
    struct WAVELIB_API IAudioFrame;
    struct WAVELIB_API AudioFrameType;
    struct WAVELIB_API Panorama;


    enum WAVELIB_API WAV_PCM_TYPE_ID : unsigned short {
        PCMi = 0,
        PCMs = 1,
        PCMf = 3,
    };

    enum WAVELIB_API SND_PCM_TYPE_ID : unsigned int {
        MULAW_8   = 1,
        LINEAR_8  = 2,
        LINEAR_16 = 3,
        LINEAR_24 = 4,
        LINEAR_32 = 5,
        FLOAT     = 6,
        DOUBLE    = 7,
        ALAW_8    = 8
    };

    // AudioFormat equation result values:                              
    enum WAVELIB_API FormatMatch : unsigned int                         
    {                                                                   
        // matching format components:                                  
        SAMPLERATE   = 0x01,                                              
        DATATYPE     = 0x02,                                                
        CHANNELCOUNT = 0x04,                                            
                                                                        
        // possible buffer transfer modes:                              
        CONVERSION   = 0x00,                                              
        PERCHANNEL   = DATATYPE,                                          
        INTERLEAVE   = 0x02 + 0x04,                                       
        DIRECTCOPY   = 0x01 + 0x02 + 0x04                                 
    };

    enum WAVELIB_API HEADER_CHUNK_TYPE : unsigned int { /* FourCC-Values used in file headers: */
        FrmtChunk = FOURCC('f', 'm', 't', ' '),          //544501094   // FourCC-Value: 'fmt '
        RIFFChunk = FOURCC('R', 'I', 'F', 'F'),         //1179011410  // FourCC-Value: 'RIFF'
        DataChunk = FOURCC('d', 'a', 't', 'a'),        //1635017060  // FourCC-Value: 'data'
        FactChunk = FOURCC('f', 'a', 'c', 't'),       //1952670054  // FourCC-Value: 'fact'
        WavFormat = FOURCC('W', 'A', 'V', 'E'),      //1163280727  // FourCC-Value: 'WAVE'
        SndFormat = FOURCC('.', 's', 'n', 'd'),     //1684960046  // FourCC-Value: '.snd'
#ifdef PAM_WAVEFILE_HEADERS
        P7mFormat = FOURCC('P', '7', '\n', '#'),  // P7 arbitrarry map file (binary data)
        P8mFormat = FOURCC('P', '8', '\n', '#')  // P8 arbitrarry map file (ascii data)
    };

    enum ChannelMode {
        Planar      = PERCHANNEL,
        Interleaved = INTERLEAVE,
    };

    enum PAM_HEADER_VALUE : ulong {
        RATE     = LONGCC('#', 'R', 'A', 'T', 'E', 0, 0, 0),
        TUPLTYPE = LONGCC('T', 'U', 'P', 'L', 'T', 'Y', 'P', 'E'),
        MAXVAL   = LONGCC('M', 'A', 'X', 'V', 'A', 'L', 0, 0),
        DEPTH    = LONGCC('D', 'E', 'P', 'T', 'H', 0, 0, 0),
        HEIGHT   = LONGCC('H', 'E', 'I', 'G', 'H', 'T', 0, 0),
        WIDTH    = LONGCC('W', 'I', 'D', 'T', 'H', 0, 0, 0),
        ENDHDR   = LONGCC('E', 'N', 'D', 'H', 'D', 'R', 0x0a, 0)
    };

    enum PAM_HEADER_MASKS : ulong {
        RATE_MASK     = 0x000000fffffffffful,
        TUPLTYPE_MASK = 0xfffffffffffffffful,
        MAXVAL_MASK   = 0x0000fffffffffffful,
        DEPTH_MASK    = 0x000000fffffffffful,
        HEIGHT_MASK   = MAXVAL_MASK,
        WIDTH_MASK    = DEPTH_MASK,
        ENDHDR_MASK   = 0x000afffffffffffful
    };
#else
    };
#endif


#define OneLine(b,c,t) Au_##t##_##b##_##c = AUDIOFRAME_CODE(b,c,t)
#define OneType(bit,typ) OneLine(bit,1,typ),OneLine(bit,2,typ),\
        OneLine(bit,4,typ),OneLine(bit,6,typ),OneLine(bit,8,typ)

    enum WAVELIB_API FrameTypeCode : unsigned short {
        OneType(8, PCMi),
        OneType(8, PCMs),
        OneType(16,PCMs),
        OneType(24,PCMi),
        OneType(24,PCMs),
        OneType(32,PCMf),
        OneType(64,PCMf)
    };

#undef OneType
#undef OneLine

    enum WAVELIB_API Constants : unsigned int
    {
        // wave header constants and fourCCs,
        // by positional appeareance in headers:
        RIFFChunkID = HEADER_CHUNK_TYPE::RIFFChunk,     // Contains the letters "RIFF" in ASCII form //0x52494646,
        RIFFChunkSize = 36,                             // size of the entire file in bytes minus 8 (for Wave data of length 0 this is 36)
        //FileFormat = WavFormat,                       // Contains the letters "WAVE" (0x57415645 big-endian form).
        FormatChunkID = HEADER_CHUNK_TYPE::FrmtChunk,   // Contains the letters "fmt " (0x666d7420 big-endian form).
        FormatChunkSize = 16,                           // 16 for PCM. This is the size of the rest of the Subchunk which follows this number.
        FactChunkID = HEADER_CHUNK_TYPE::FactChunk,     // If the header contains an additional fact-chunk, this Contains the letters "fact".
        FactChunkSize = 36,                             // When a fact-chunk is present, it contains the size of the chunk in byte. Otherwise it contains data-size.
        DataChunkID = HEADER_CHUNK_TYPE::DataChunk,     // Contains the letters "data" (0x64617461 big-endian form). It is followed by 32bit-Int 'data-size', which then is followed by exactly that count on bytes of raw audio data.
        //
        // other wave audio header related constants:
        SimpleHeaderSize = 44,                          // Size of WAV type 'simple' audio file header
        ExtendedHeaderSize = 88,                        // Size of WAV type 'extended' audio file header (adds a 'fact' chunc which may contain additional information)
        SndFileHeaderSize = 24,                         // Size of SND type 'Au/snd' audio file header
        PamFileHeaderSize = 122,                        // Size of PAM type 'NetPbm' p7 file header (* wavelib always stores .pam audio with header size of 122 byte - netpbm defines pam file header sizes may vary. so when loading .pam files, header order and size could be different depending on the tool which wrote the file)
        ReadHeadSize = 64,
        WaveHeaderPlusReadHeadSize = ExtendedHeaderSize+ReadHeadSize,               // Maximum size of instanciated 'WavFileHeader' struct (includes one frame of audio data to be used as read/write 'head').
    };

    // Creation flags for Audio buffers:
    enum WAVELIB_API Initiatio : unsigned int
    {
        // initiation flags:
        DEFAULT = 0x00,
        RAW_FILE_DATA = 0x01,                 // means the 'Audio' asumes the data being passed to the initialization as being WAVE-RIFF header containing file data from a .wav file, and will try to gain format information from this.
        ALLOCATE_NEW_COPY = 0x02,             // means the 'Audio' will allocate a new, owned buffer and will copy any audio data passed to the initialisation, to this new, own buffer.
        DONT_ALLOCATE_NEW = 0x04,             // means the 'Audio' will not create any new buffers, but will try to gain format information from the buffer that was passed, and will point to that buffer then.
        //
        // conchainment flags:                // <- does this word really exist ???
        FIRST = 0x01 << 8,                    // means the 'Audio' is the 'FIRST' chunk of a chained 'row' of 'Audio'.
        LAST = 0x02 << 8,                     // means the 'Audio' is the 'LAST' chunk of a chained 'row' of 'Audio'.
        CIRCULAR = (0x01 << 8)|(0x02 << 8),   // means the 'Audio' will handle it's buffer being circular.
        SUBCHUNK = 0x04 << 8,                 // means the 'Audio' is element of a chained 'row', by not being neither the 'FIRST', nor the 'LAST'.
        //
        // specialization flags:
        SHUTTLE = 0x01 << 16,                 // means the 'Audio' will initialize a new buffer and will copy any data which may be passed, but will not take ownership of that buffer at all. (when it's used for returning audio data by return value for example)
        //
        // buffer ownership flags:
        OWN = 0x01 << 24,                     // means the 'Audio' takes ownership of the allocated memory it's pointing to.
        WHOLE = 0x02 << 24,                   // means the 'Audio' takes ownership of all allocated memory which all subchunks in the 'row' may pointing to.
    };

    struct WAVELIB_API BufferFlags {
    public:
        union BufferMode {
            byte raw[4];
            struct Category {
                bool initiationflags;
                bool chained;
                bool specialflags;
                bool owned;
            } group;
            Initiatio all;
            unsigned value;
        } flags;
        BufferFlags()
            : flags()
        {}
        BufferFlags(Initiatio inits) {
            setFlags(inits);
        }
        bool hasFlag(Initiatio flag) const {
            return flag == (flags.value & flag);
        }
        void addFlag(Initiatio flag) {
            enum_utils::operator|=( flags.all, flag );
        }
        void remFlag(Initiatio flag) {
            enum_utils::operator&=( flags.all, flag );
        }
        void setFlags(Initiatio inits) {
            flags.all = inits;
        }
    };


//////////////////////////////////////////////////////////////////////////////
/* PCM format struct -------------------------------------------------------*/
                                                                            //
    // PCM Format struct:                                                   //
    struct WAVELIB_API Format {                                             //
        WAV_PCM_TYPE_ID PCMFormatTag;                                       //
        word NumChannels;                                                   //
        uint SampleRate;                                                    //
        uint ByteRate;                                                      //
        word BlockAlign;                                                    //
        word BitsPerSample;                                                 //
    };                                                                      //
                                                                            //
    WAVELIB_API FormatMatch operator ==(const Format&, const Format&);      //
    WAVELIB_API FormatMatch operator !=(const Format&, const Format&);      //
    WAVELIB_API Initiatio   operator |(Initiatio This, Initiatio That);     //
                                                                            //
//////////////////////////////////////////////////////////////////////////////


/* File header implementations: --------------------------------------------*/

    union WAVELIB_API ReadOrWriteHead {
        f64 F64[8];
        f32 F32[16];
        i32 I32[16];
        s16 S16[32];
        i8  _i8[64];
    };

    struct WAVELIB_API RiffChunkHeader {
        HEADER_CHUNK_TYPE type;
        uint              size;
    };

    // Wav-Format File header
    /*-----------------------------------------------------------------------------------------------------------------*/
    struct WAVELIB_API WavFileHeader { //       Offset  Size  Type       Description
    // ----------------------------------------------------------------------------------------------------------------
        RiffChunkHeader      RiffChunk;      // 0       8     Header     'RIFF' + size of chunk (filesize - 8)
        uint                 FileFormat;     // 8       4     FourCC     'WAVE'
        RiffChunkHeader      FormatChunk;    // 12      8     Header     'fmt ' + size of Format structure (16)
        Format               AudioFormat;    /* 20      16    Format     Structure consisting from these values:
                             PcmFlags        // 20      2     Word       (1 Signed / 2 Float) - Float: signed AND float
                             Channels        // 22      2     Word       (samples per frame)
                             SampleRate      // 24      4     DWord      (frames per second)
                             ByteRate        // 28      4     DWord      (bytes per second)
                             BlockAlign      // 32      2     Word       (byte per frame)
                             BitDepth        // 34      2     Word       (bits per sample)  */
        RiffChunkHeader      ChunkHeader;    // 36      8     Header     'data' + size of data / 'fact' + size of facts
        union SubChunkData {                 // 44      eof   WaveData   simple header: wave data begins with offset 44 
            byte raw[152];
            struct Simple {                
                ReadOrWriteHead ReadHead;
            } simple;
            struct Extended {                // 44      44    FactChunk  extended: the FactChunk begins with offset 44 
                byte FactChunk[36];            
                RiffChunkHeader ChunkHeader;
                ReadOrWriteHead ReadHead;    // 88      eof   WaveData   extended header: data then begins with offset 88 
            } extended;     
        } DataChunk;

        // interface functions each WaveLib supported file type header implent
        WavFileHeader& operator= (WavFileHeader);
        word  GetHeaderSize(void) const;
        uint  GetDataSize(void) const;
        Data  GetAudioData(void) const;
        void  GetFormat(Format* getter) const;
        bool  isValid(void) const;
        word  GetBitDepth(void) const;
        word  GetBlockAlign(void) const;
        word  GetChannelCount(void) const;
        const AudioFrameType GetTypeCode(void) const;
        uint  GetSampleRate(void) const;
        bool  isFloatType(void) const;
        bool  isSignedType(void) const;
        HEADER_CHUNK_TYPE GetFileFormat(void) const;
    };

    // WavFileHeader internal used functions:
    WavFileHeader* getWaveheaderProtostructure( void );
    WavFileHeader  initializeNewWaveheader( unsigned fileSize = EMPTY);
    WavFileHeader* prepareInitializedHeader( WavFileHeader* notYetPrepared, unsigned dataSize = EMPTY );


    // Snd/Au Format file header   (be aware, when reading header from snd or au files on windows, the header data read 
    // from a file, need to be byte order reversed before it could match this structure the following table describes.)
    /*-----------------------------------------------------------------------------------------------------------------*/
    struct WAVELIB_API SndFileHeader  {    //  Offset  Size  Type     Contents
    //  ----------------------------------------------------------------------------------------------------------------
        HEADER_CHUNK_TYPE SndTag;          //    0      4    fourCC    ".snd"
        uint              HeaderSize;      //    4      4    uint      Offset to start of data (should be 28 byte)
        uint              DataSize;        //    8      4    uint      Number of bytes of data
        SND_PCM_TYPE_ID   FormatCode;      //    12     4    uint      Data format code
        uint              SampleRate;      //    16     4    uint      Sampling rate
        uint              NumChannels;     //    20     4    uint      Number of channels
   //   uint              DataTag;         //    24     4    fourCC    user data/tag (may be any fourCC or even blank) ?
    //  ----------------------------------------------------------------------------------------------------------------
        ReadOrWriteHead   ReadHead;        //    24     x    sample    first sample of data -> for read/write head usage

        SndFileHeader operator=(SndFileHeader);
        word  GetHeaderSize(void) const;
        uint  GetDataSize(void) const;
        Data  GetAudioData(void) const;
        void  GetFormat(Format* getter) const;
        bool  isValid(void) const;
        word  GetBitDepth(void) const;
        word  GetBlockAlign(void) const;
        word  GetChannelCount(void) const;
        const AudioFrameType GetTypeCode(void) const;
        uint  GetSampleRate(void) const;
        bool  isFloatType(void) const;
        bool  isSignedType(void) const;
        HEADER_CHUNK_TYPE GetFileFormat(void) const;
    };

    // SndFileHeader internal used functions
    void reverseSndHeader( SndFileHeader* sndhdr );


#ifdef PAM_WAVEFILE_HEADERS

    // Pam (Portable arbitrary map) format file header
    struct WAVELIB_API PamFileHeader {

        enum Values : uint {
            RATE     = 3|(14<<16)|(6<<24),
            TUPLTYPE = 18|(22<<16)|(9<<24),
            MAXVAL   = 41|(27<<16)|(7<<24),
            DEPTH    = 69|(9<<16)|(6<<24),
            HEIGHT   = 79|(10<<16)|(7<<24),
            WIDTH    = 90|(24<<16)|(6<<24),
            ENDHDR   = 115|(6<<16)|(6<<24),
            HeaderSize = 122,
        };

        union { // (the header data is string based, so no concrete binary structure exists)
            char              dat[256];
            HEADER_CHUNK_TYPE tag; // except that it begins with two netpbm magic bytes 'P7' 
        } hdr;

        // char pointers, each pointing entries which exist inside the header's string data:
        /*------------------------------------------------------------------------------------------------------
        char* RATE;      // sample playback rate, optional - (so stored as a comment line beginning with '#RATE')
        char* TUPLTYPE;  // frametype (like 'Au_PCMs_16_2' for signed16 stereo or 'Au_PCMf_32_6' for float32 5.1 )
        char* MAXVAL;    // maximum value of an unsigned integer at sample data type size
        char* DEPTH;     // channel count when interleaved channel storage is selected - otherwse 1
        char* HEIGHT;    // channel count when planar channel storage is selected - otherwise 1
        char* WIDTH;     // length of audio data (total count on frames) which each 'HEIGHT'(channel) row measures.

        // positions internally used by the header
        char* ENDHDR;     // points an in the string data contained end mark which signals end of the header 
        char* ReadHead;   // initially it's pointing the first byte of audio data
        */
        // Audio-file header-interface functions 
        PamFileHeader& operator=( const PamFileHeader& );
        word  GetHeaderSize(void) const;
        uint  GetDataSize(void) const;
        Data  GetAudioData(void) const;
        void  GetFormat( Format* getter ) const;
        bool  isValid(void) const;
        word  GetBitDepth(void) const;
        word  GetBlockAlign(void) const;
        word  GetChannelCount(void) const;
        const AudioFrameType GetTypeCode(void) const;
        uint  GetSampleRate(void) const;
        bool  isFloatType(void) const;
        bool  isSignedType(void) const;
        HEADER_CHUNK_TYPE GetFileFormat(void) const;

        // Additional Pam header speciffic functions
        void        set_Value(Values index, uint set);
        uint        get_Value(Values index) const;
        const char* makeStringBased(void);
        void        makeValueBased(void);
        bool        isStringBased(void) const;
        ChannelMode GetChannelMode(void) const;
        void        SetChannelMode( ChannelMode set );

        operator const PamFileHeader();
        operator PamFileHeader&() const;
        operator const PamFileHeader*();
        operator PamFileHeader*() const;

        void  SetSampleRate(uint set);
        void  SetBitDepth(word set);
        void  SetChannelCount(word set);
        void  SetPcmFlags(WAV_PCM_TYPE_ID pcmflags);
        void  SetDataSize(uint set);
        void  updateTypeCode(void);

        PROPDECL( word, BitDepth );
        PROPDECL( uint, DataSize );
        PROPDECL( uint, SampleRate );
        PROPDECL( word, ChannelCount );
        PROPDECL( ChannelMode, ChannelMode );
        LISTPROP( uint, Value, 6 );

    };

    PamFileHeader wirdPassendGemacht( const char* nichtPassend );
    void initializePamFileHeader( PamFileHeader* init, const Format* format = NULL, ChannelMode mode = ChannelMode::Interleaved );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // exported helper functions:
    WAVELIB_API PamFileHeader CreatePamFileHdr( const Format& fmt, int cBs = 0, ChannelMode mod = ChannelMode::Interleaved );
#endif
    WAVELIB_API WavFileHeader CreateWaveHeader( uint samplingFrequency, word bitsPerSample,
                                                word numberOfChannels, WavFileHeader* whdr,
                                                uint sizeOfWaveData = 0 );
    WAVELIB_API WavFileHeader CreateWaveHeader( uint samplingFrequency, word bitsPerSample,
                                                word numberOfChannels, uint sizeOfWaveData = 0 );
    WAVELIB_API SndFileHeader CreateSndFileHdr( const Format& fmt, int cbs );
    WAVELIB_API Format        CreateWaveFormat( int frq, int bit, int chn );
    WAVELIB_API Format        CreateWaveFormat( int frq, int bit, int chn, WAV_PCM_TYPE_ID tag );
    WAVELIB_API Format        CreateWaveFormat( uint formatCode, int sampleRate );
    WAVELIB_API Data          ReverseByteOrder( Data buffer, const Format& format, uint frameCount );
    WAVELIB_API double        ConversionFactor( int fromBits, int toBits );
    WAVELIB_API slong         SignedTypesShift( int fromBits, int toBits );
    WAVELIB_API std::string   PictogramFromPan( Panorama pan );
    WAVELIB_API IAudioFrame*  CreateAudioFrame( uint typecode );
    WAVELIB_API IAudioFrame*  CreateManyFrames( uint typecode, uint ammount );
    WAVELIB_API IAudioFrame*  CreateAudioFrame( uint typecode, Data fromRawData );
    WAVELIB_API const char*   NameFromTypeCode( FrameTypeCode frametypecode );
    WAVELIB_API FrameTypeCode TypeCodeFromName( const char* frametypename );

ENDOF_WAVESPACE
#endif
