/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveAudio.hpp                              ||
||     Author:    Kalle                                      ||
||     Generated: 12.01.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveAudio_hpp_
#define _WaveAudio_hpp_

#define maxOf( a, b ) ( ( a > b ) ? a : b )
#define minOf( a, b ) ( ( a < b ) ? a : b )

#include <limits>
#define FLOAT_NAN std::numeric_limits<float>::quiet_NaN()

#define STATIC_TYPESIZE_CONST TS
#define STATIC_CHANNELS_CONST CH

//TODO:   make Joint-Stereo channel constellations possible

#ifndef _MSC_VER
#include "Panorama.hpp"
#endif

#include <WaveLib.inl/int24bittypes.hpp>

BEGIN_WAVESPACE

class WAVELIB_API IChannel
{
protected:
    virtual IChannel* preInc(void) = 0;
    virtual IChannel* pstInc(void) = 0;
    virtual IChannel* preDec(void) = 0;
    virtual IChannel* pstDec(void) = 0;
    virtual void* seekOp(int pos) = 0;
    template<typename SampleType = s16>
    SampleType& getSample(int position = 0) {
        return *(SampleType*)seekOp(position);
    }

public:
    IChannel& operator++(void) {
        return *preInc();
    }
    IChannel& operator++(int) {
        return *pstInc();
    }
    IChannel& operator--(void) {
        return *preDec();
    }
    IChannel& operator--(int) {
        return *pstDec();
    }

};

struct WAVELIB_API Audio;
template <typename tT, const unsigned cC>
struct Audiom;
struct WAVELIB_API WavFileHeader;


template <typename SampleDataType>
SampleDataType WAVELIB_API
#ifdef IMPORT_WAVELIB
channelMixer( SampleDataType sample, int channel, float *mixer, bool FR = false );
#else
channelMixer( SampleDataType sample, int channel, float *mixer, bool FR = false )
{
    if ( channel > 3 )
        return sample;
    float m1 = ( 2.0f * ( channel % 2 ? ( 1.0 - mixer[0] ) : mixer[0] ) );
    m1 = m1 > 1 ? 1 : m1;
    if ( FR ) {
        float m2 = ( 2.0f * ( channel > 1 ? ( 1.0 - mixer[1] ) : mixer[1] ) );
        m2 = m2 > 1 ? 1 : m2;
        return SampleDataType( ( m1 * m2 ) * sample );
    } return SampleDataType( m1 * sample );
}
#endif

template<typename FT,typename AX>
AX WAVELIB_API
#ifdef IMPORT_WAVELIB
frameSpliter( const FT& frame, Panorama::Axis axis );
#else
frameSpliter( const FT& frame, Panorama::Axis axis )
{
    const uint maxCH = FT::CH > 4 ? 4 : FT::CH;
    AX axt;
    if (axis == 3) {
        for (int i = 0; i < maxCH; i += 2) {
            axt.channel[i/2] = FT::TY(
                (frame.channel[i]/2)
               +(frame.channel[i+1]/2)
            );
        } if (axt.CH > 1) {
            axt.channel[0] =
            axt.channel[1] = FT::TY(
                (axt.channel[0]/2)
               +(axt.channel[1]/2)
            );
        }
    } else if (axis) {
        for (int i = 0; i < maxCH; i += 2) {
            axt.channel[i/2] = FT::TY(
                (frame.channel[i]/2)
               +(frame.channel[i+axis]/2)
            );
        }
    } return axt;
}
#endif

WAVELIB_API IAudioFrame*  CreateAudioFrame( uint typecode );

struct WAVELIB_API IAudioFrame;
struct WAVELIB_API AudioFrameType {
public:
    union Info {
        union Type {
            const byte data[2];
            const FrameTypeCode code;
        }   type;
        const word time[2];
        const uint data;
        Info( uint argum ) : data(argum) {}
    }   info;

    double  Time() const { return info.time[1] ? ((1.0 / (double)info.time[1]) / 1000.0) : std::numeric_limits<double>::quiet_NaN(); }
    FrameTypeCode Code() const { return info.type.code; }
    word Rate() const { return info.time[1]; }

    WAV_PCM_TYPE_ID FormatTag() const { return WAV_PCM_TYPE_ID( info.type.data[1] >> 6 ); }
    short IsSigned() const { return (AUDIO_SIGN_FROM_TYPECODE( info.type.code ) * 2) - 1; }
    word BitDepth() const { return info.type.data[0] & 0xFFu; }
    word Channels() const { return info.type.data[1] & 0x3Fu; }
    word ByteSize() const { return ((info.type.data[0] & 0xFFu) >> 3)
                                   *(info.type.data[1] & 0x3Fu);
    }
    AudioFrameType( WAV_PCM_TYPE_ID tag, uint bit, uint chn, uint srt = 0 )
        : info((const uint)(bit|((chn|(tag<<6))<<8)|(srt<<16))) {
    }
    AudioFrameType( uint bit, uint chn )
        : info((const uint)(bit|((chn|((bit>=32?PCMf:bit!=24?PCMs:PCMi)<<6))<<8))) {
    }
    AudioFrameType( const Format& fmt )
        : info((const uint)( ( fmt.BitsPerSample
                           | ((fmt.NumChannels|(fmt.PCMFormatTag<<6))<<8))
                           | (fmt.SampleRate << 16) ) ) {
    }
    AudioFrameType( FrameTypeCode typecode, word samplerate = 0 )
        : info((const uint)(typecode | (samplerate << 16)) ) {
    }
    bool operator ==(const AudioFrameType& other) const {
        return this->info.type.code == other.info.type.code;
    }
    bool operator !=(const AudioFrameType& other) const {
        return this->info.type.code != other.info.type.code;
    }
    AudioFrameType operator =(AudioFrameType from) {
        *reinterpret_cast<uint*>(&info) = from.info.data;
        return *this;
    }
    static const AudioFrameType fromTypeCode( uint typecode ) {
        return *reinterpret_cast<AudioFrameType*>(&typecode);
    }
    IAudioFrame* CreateFrame( Data fromRawData ) const;
};



struct IAudioFrame {
protected:

    template<typename FROM_TYPE>
    IAudioFrame* convertFromThis( uint toType ) {
#define CASETYPE_COMBO typename CASE_TYPE::FRAME::TY,CASE_TYPE::FRAME::CH
#define CONVERTED_COMBO(t,c) converted<t,c>()
        FRAMETYPE_SWITCH( toType,
            Frame<CASETYPE_COMBO> conv( dynamic_cast<FROM_TYPE*>(this)->CONVERTED_COMBO( CASE_TYPE::FRAME::TY, CASE_TYPE::FRAME::CH ) );
            IAudioFrame* frme = CreateAudioFrame( toType );
            frme->Set( &conv );
            return frme;
        )
#undef  CONVERTED_COMBO
    }

    template<typename INTO_TYPE = IAudioFrame>
    INTO_TYPE convertToType( uint fromThis ) {
#define INTOTYPE_COMBO INTO_TYPE::TY,INTO_TYPE::CH
        FRAMETYPE_SWITCH( fromThis,
            return ( dynamic_cast<CASE_TYPE*>(this)->converted<INTOTYPE_COMBO>() ); )
#undef  INTOTYPE_COMBO
    }

public:
    virtual AudioFrameType GetFrameType() const = 0;
    virtual Data Pan(Panorama p, Panorama::Axis a) = 0;
    virtual Data Mix(Data frame, float drywet) = 0;
    virtual Data Mix(Data sample, Panorama pano) = 0;
    virtual Data Amp(double factor) = 0;
    virtual void SetData( Data concrete ) = 0;
    virtual uint Channels() const = 0;
    virtual uint BitDepth() const = 0;
    virtual uint ByteSize() const = 0;
    virtual bool Signedty() const = 0;
    virtual Panorama GetPanorama(void) = 0;
    virtual void     SetPanorama(Panorama) = 0;
    virtual void SetChannel( int index, Data sample ) = 0;
    virtual Data GetChannel( int index ) const = 0;
    virtual IAudioFrame* Convert( uint byTypecode ) = 0; // muss weg!... muss so:  //IAudioFrame* ConvertTo(Audio::Data darein, word alsDieses);
    virtual IAudioFrame* Set( Data concrete ) = 0;
    
    double SampleTypeMaxValue() const;
    double SampleTypeMinValue() const;
    double SampleType0dbValue() const;

    Data Pan( Panorama p ) {
        return Pan( p, Panorama::Axis::Any );
    }
    Format GetFormat( uint SampleRate ) const {
        return CreateWaveFormat(
            SampleRate, BitDepth(), Channels()
        );
    }
    const AudioFrameType FrameType( void ) const {
        return AudioFrameType(
            BitDepth(), Channels() );
    }
#ifdef _MSC_VER
    PROPARRAY( Data, Channel, 10 );
#endif
};


template <typename bT,
    const unsigned C>
    struct Frame
{
    typedef bT TY;
    typedef Frame<TY, (C >> 2) + 2> Axt;
    static const byte CH = C;
    static const byte TS = (sizeof(bT) * 8);
    static const char ST = (char)(std::numeric_limits<TY>().is_signed
                         - (!std::numeric_limits<TY>().is_integer));
    static unsigned frameSizeByte() { return (TS >> 3) * CH; };
    static bool     isFloatType() { return ST > 0; }
    static bool     isSignedType() { return ST >= 0; }

    // static 'Frame' API:

    // construct a whole 'Frame' from a single mono sample by placing it
    // by 'placement' parameter into the new frame's Stereo or 3D field.
    static Frame MixFromSample(TY sample, Panorama placement)
    {
        Frame neu;
        for (int i = 0; i < CH; i++) {
            neu.channel[i] = channelMixer(sample, i, placement, CH > 2);
        } return neu;
    }

    // allocated data of one audio 'Frame'
    TY channel[C];


    // per-frame operations:
    // Panorama struct which is used as parameter for placements into stereo/3D fields should be able to expand
    // from mono to stereo, quadro, 5.1 till 7.1 configurations (where 5.1 and 7.1 placement may rather experimental
    // actually - there are no filters applied at all yet, channels are just leveled individually and .1 channels
    // catching full summs, clipp-safe mixed)

    // place Frame 'that' into this frame, by dry/wet 'mix' ratio (0=this,1=that,0.5=1:1)
    Frame* mix( const Frame<bT,C> &that, float mix = 0.5f )
    {
        for (int i = 0; i < CH; i++) {
            this->channel[i] = channelMixer( this->channel[i], 0, &mix )
                + channelMixer( that.channel[i], 1, &mix );
        } return this;
    }

    // place a mono 'sample' by panorama 'mixer' into this frames stereo/3D field
    Frame* mix( TY sample, Panorama mixer )
    {
        for (int i = 0; i < CH; i++) {
            channel[i] += channelMixer( sample, i, &mixer.data.F32[0], CH > 2 );
        } return this;
    }

    // create a new 'Frame' consisting from 'mix' ratio 'this' and 'that' parts
    Frame mixed(Frame that, float mix = 0.5f) const
    {
        for (int i = 0; i < CH; i++) {
            that.channel[i] = channelMixer( this->channel[i], 0, &mix )
                + channelMixer( that.channel[i], 1, &mix );
        } return that;
    }

    // create a new 'Frame' from this frame having mono 'sample' mixed within 'panorama' stereo/3D field
    Frame mixed(TY sample, Panorama panorama) const
    {
        Frame neu;
        for (int i = 0; i < CH; i++) {
            neu.channel[i] = channel[i] + channelMixer( sample, i, &panorama.data.F32[0], CH > 2 );
        } return neu;
    }

    // create a new 'Frame' from this frame amplified by 'factor'
    Frame amped(double factor) const
    {
        Frame neu;
        for (int i = 0; i < CH; i++) {
            neu.channel[i] = TY((double)channel[i] * factor);
        } return neu;
    }

    // amplify this frame by 'factor'
    Frame* amp(double factor)
    {
        for (int i = 0; i < CH; i++)
            channel[i] = TY((double)channel[i] * factor);
        return this;
    }

    // silence channel number 'chanNum' from this frame
    Frame* remove(uint chanNum)
    {
        channel[chanNum] = 0;
        return this;
    }

    // create a new Frame from this frame, having any channels above or equal to 'chanIdx' silenced
    Frame removed(uint chanIdx) const
    {
        Frame ret(*this);
        for (chanIdx; chanIdx < CH; chanIdx++)
            ret.channel[chanIdx] = 0;
        return ret;
    }

    template <typename dT, const unsigned dC>
    // create a new 'Frame' by converting this frame to a new frame of different frame type
    Frame<dT,dC> converted(void) const
    {
        if (ST == Frame<dT, dC>::ST && TS == Frame<dT, dC>::TS && dC == C)
            return *reinterpret_cast<const Frame<dT,dC>*>( this );

        double conversionfactor = ConversionFactor( TS, Frame<dT,dC>::TS );
        slong  centerShiftValue = SignedTypesShift( TS*ST, Frame<dT,dC>::TS
                                                         * Frame<dT,dC>::ST );
        Frame<dT,dC> conv;
        if (dC >= CH) {
            for ( int chan = 0; chan < CH; ++chan ) {
                conv.channel[chan] = (dT)(channel[chan] * conversionfactor);
            } if (dC > CH) for ( int chan = CH; chan < dC; ++chan ) {
                conv.channel[chan] = 0;
                for ( int mx = 0; mx < CH; ++mx ) {
                    conv.channel[chan] += (dT)( (channel[mx] / CH)
                                              * conversionfactor
                                              + centerShiftValue );
                }
            }
        } else { // if dC < CH
            unsigned ampl = (CH - dC) + 1;
            conversionfactor *= (1.0 / ampl);
            for ( int chan = 0; chan < CH; ++chan ) {
                conv.channel[chan % dC] = (dT)( channel[chan]
                                              * conversionfactor
                                              + centerShiftValue );
            }
        } return conv;
    }

    // re-adjust channel levels to new stereo/3D field 'panorama'.
    // if additional 'axis' parameter is given, stereo/3D fields
    // will be split along direction 'axis'. each resulting split field
    // is downmixed to a mono sample then. both samples then will be
    // arranged into a new stereo/3D field which 'panorama' parameter
    // defines (where the second splitt axe is placed then by the antiproper
    // counterpart of the 'panorama' parameter - e.g. 0.2,0.3 -> 0.8,0.7 )
    Frame* pan( Panorama panno, Panorama::Axis axis = Panorama::Axis::Any )
    {
        if( panno == panno.NEUTRAL )
            return this;

        if( axis > 0 ) {
            Axt axt = frameSpliter<Frame,Axt>( *this, axis );
            for (int i = 0; i < CH; i++)
                channel[i] = channelMixer(
                    axt.channel[( axis < 2 ? (i / 2) : (i % 2) ) * (axt.CH - 1)],
                    i, &panno.data.F32[0], CH > 2 );
        } else
            for (int i = 0; i < CH; i++)
                channel[i] = channelMixer(
                     channel[i], i, &panno.data.F32[0], CH > 2
                                           );
        return this;
    }

    // same like 'pan' but doesn't effects this frame instance but
    // returnes a copy with rearranged stereo/3D fields instead.
    Frame paned( Panorama panno, Panorama::Axis axis = Panorama::Axis::Any ) const
    {
        if( panno == panno.NEUTRAL )
            return *this;

        Frame neu;
        if( axis > 0 ) {
            Axt axt = frameSpliter<Frame,Axt>(*this, axis);
            for (int i = 0; i < CH; i++)
                neu.channel[i] = channelMixer(
                    axt.channel[( axis < 2 ? (i / 2) : (i % 2) ) * (axt.CH - 1)],
                    i, &panno.data.F32[0], CH > 2 );
        } else
            for (int i = 0; i < CH; i++)
                neu.channel[i] = channelMixer(
                    channel[i], i, &panno.data.F32[0], CH > 2
                                                );
        return neu;
    }

#pragma warning( push )
#pragma warning( disable : 4723 )
    Panorama panorama() const
    {
        Panorama p = Panorama::NEUTRAL;
        if ( CH > 1 ) {
            TY axe[2] = { TY(0), TY(0) };
            if (CH >= 2) {
                for (int i = 0; i < CH; i += 2) {
                    axe[0] += (channel[i] / (CH / 2));
                    axe[1] += (channel[i + 1] / (CH / 2));
                } p.Left[Panorama::Absolute] = axe[0];
                p.Right[Panorama::Absolute] = axe[1];
            } if (CH > 2) { axe[0] = 0; axe[1] = 0;
                for (int i = 0; i < (CH / 2); ++i) {
                    axe[i % 2] += ((channel[2 * i] / 2
                        + channel[2 * i + 1] / 2)
                        / ((CH / 2) - (i % 2)));
                } p.Front[Panorama::Absolute] = axe[0];
                p.Rear[Panorama::Absolute] = axe[1];
            }
        } return p;
    }
#pragma warning( pop )

};


struct WAVELIB_API Audio
{
protected:
    static Audio* cloneContainer( Audio *auA, Audio *auB,
                                  unsigned optDivergedSize = 0 );

    template <typename srcST, typename dstFT>
    static void   convertData( dstFT *destination, const double conFactor,
                               const int numChanOfCopy, Audio *source ) {
        if (conFactor < 0 ) {
            throw std::exception( INVALID_TYPECONVRSION );
        }
        const int MaxNumChannels = numChanOfCopy > source->format.NumChannels
                                 ? numChanOfCopy
                                 : source->format.NumChannels;
        double multiplier = 1.0;
        if( numChanOfCopy < MaxNumChannels )
            multiplier = ((float)numChanOfCopy / MaxNumChannels);
        dstFT::TY* dst = (dstFT::TY*)destination;
        const int dstSigned = ( 1 + std::numeric_limits<typename dstFT::TY>().is_signed * -2 );
        const int srcSigned = ( 1 + std::numeric_limits<srcST>().is_signed * -2 );
        if ( srcSigned != dstSigned ) {
            // do conversion with taking care of signednes of even the source and the target formats sample type
            slong centerShift = SignedTypesShift( (int)(sizeof(srcST)*8) * srcSigned, (int)dstFT::TS * dstSigned );
            const unsigned end = source->getLength();
            for ( unsigned frm = 0; frm < end; ++frm ) {
                srcST *src = (srcST *)( *source )[frm];
                for (int i = 0; i < numChanOfCopy; ++i)
                    dst[i] = 0;
                for ( int c = 0; c < MaxNumChannels; c++ ) {
                    int Cd = c % numChanOfCopy;
                    int Cs = c % source->format.NumChannels;
                    dst[Cd] += ( typename dstFT::TY )( (( conFactor * src[Cs] ) + centerShift) * multiplier  );
                } dst += numChanOfCopy;
            }
        } else { // do cheaper conversion with assuming source as well as target format have signed sample type
            multiplier *= conFactor;
            const unsigned end = source->getLength();
            for ( unsigned frm = 0; frm < end; frm++ ) {
                srcST *src = (srcST *)( *source )[frm];
                for (int i = 0; i < numChanOfCopy; ++i)
                    dst[i] = 0;
                for ( int c = 0; c < MaxNumChannels; ++c ) {
                    int Cd = c % numChanOfCopy;
                    int Cs = c % source->format.NumChannels;
                    dst[Cd] += ( typename dstFT::TY )( multiplier * src[Cs] );
                        // this if I could remember was intended to ensure sampletype +/- ranges being same size
                        // (e.g. for 8bit: range -127! to +127 instead of the native type range -128 to +127...)
                        // but don't know anymore for what reason I've implemented it. Due to it would make type
                        // conversion quite a lot more expensive when enabled, I'll try if any evil sideeffects
                        // may occur when having the code disabled.
                   /*   if ( Cd >= c ) {
                          dst->channel[Cd] /= 2.0;
                          dst->channel[Cd] += ( typename dstFT::TY )( ( conversionFactor * src[Cs] ) / 2.0 );
                        }   */
                } dst += numChanOfCopy;
            }
        }
    }

    void setSilent( void );

    bool trimmi(double threshold, float length);
    bool trimmo(double threshold, float length);

public:

    typedef void* Data;
    typedef const Data NoData;
    static bool   isDataValid(Data check);

    /* hardware friendly 'Null-Audio-pointer'
       which, when mistakenly passed to some
       output device, isn't able causing any
       clicks or cracks in output facillities
       like speekers, amps, and mixers and so
       on, like nullptr may effect. */
    static NoData Silence;

    Data data;
    Format format;
    unsigned cbSize;
    size_t frameCount;
    int offset;
    int cutend;
    BufferFlags mode;
    size_t ptd;
    Audio *nxt;

    // type speciffics:
    unsigned frameTypeCode( void ) const;
    double   sampleTypeMax( void ) const;
    double   sampleType0db( void ) const;
    double   sampleTypeMin( void ) const;

    // construction:
    Audio( void );
    Audio( Initiatio initmode );
    Audio( unsigned allocateSize );
    Audio( byte channels );
    Audio( unsigned sRate,
           unsigned bitDepth,
           unsigned channels,
           unsigned frameCont,
           Initiatio initiatio = DEFAULT );
   // Audio( Audio &wrap );
    Audio( const Audio &wrap );
    Audio( const Audio &clone, float amplificator );
    Audio( const Audio &copy, Initiatio flags );
    Audio( const Format &fmt, int framecount );
    Audio( const Format &fmt,
           Data rawData = Silence,
           unsigned cbSize = EMPTY,
           Initiatio initiatio = ALLOCATE_NEW_COPY );
    Audio( Audio::Data rawData, unsigned cbSize = EMPTY,
           Initiatio initiatio = ALLOCATE_NEW_COPY|OWN );
    Audio( const WavFileHeader& hdr, Initiatio initiatio );
    Audio( const AbstractAudioFileHeader* hdr );

    virtual bool isValid( void ) const;
    bool isSilent( void ) const;
    virtual ~Audio( void );
    void drop( void );

    // buffer handling:
    bool createBuffer( unsigned size );
    Audio::Data detachBuffer( void );
    Audio& outscope( void );
    bool takeOwnership( bool WhenOwnerUnknown = false );
    virtual void addOffset( uint frames );
    virtual void addEndcut( uint frames );
    void removeOffset( void );
    void removeEndcut( void );
    bool applyOffset( uint frames );
    void resetOffset( void );
    unsigned getLength( void ) const;
    unsigned getDataSize( void ) const;

    double getDuration( void ) const;
    void setLength( unsigned );
    FormatMatch canTranfareTo( const Audio &other ) const;
    bool canBeTransfaredBy( FormatMatch transfareMode, const Format &toTargetFormat ) const;
    void setFormat( const Format &newFormat, bool converIfNotMatching = false );

    // buffer manipulation:
    Audio &trimIn( double threshold, float length = 0.005 );
    Audio &trimOut( double threshold, float length = 0.015 );
    Audio &trim( double threshold = 0.001, float trimin = FLOAT_NAN, float trimout = FLOAT_NAN );
    Audio trimed( double threshold = 0.001, float trimin = FLOAT_NAN, float trimout = FLOAT_NAN ) const;
    Audio converted( int bitDepth, int channelCount, double amplification ) const;
    Audio converted( Format *targetFormat, double amplification = 1 ) const;
    Audio converted( unsigned typeCode, double amplification = 1 ) const;
    Audio &convert( unsigned short bitDepth, unsigned short channelCount, double amplification );
    Audio &convert( unsigned typeCode, double amplification = 1 );
    Audio &convert( Format *targetFormat, double amplification = 1 );
    Audio amplified( float amplificator ) const;
    Audio &amplify( float amplificator );
    Audio paned( Panorama, Panorama::Axis = Panorama::Axis( 3 ) );
    Audio &pan( Panorama, Panorama::Axis = Panorama::Axis( 3 ) );
    Audio compacted( void ) const;
    void  compact( void );
    Audio copy( void );
    Audio byteOrderReversed( void );
    Audio &reverseByteOrder( void );
    Audio &split( uint position, bool trim = true, bool positionIsDuration = false );

    // operators:
    Audio*      operator&();
    Audio       operator*(); //<<<<<<<<<<<<<<<<<<<<<<
    Audio&      operator=( Audio& other );
    Audio&      operator=( Data raw );
    FormatMatch operator==(const Audio &);
    FormatMatch operator!=(const Audio &);

    // create a copy and append other buffer to it (slow, copies whole buffers in any case)
    Audio operator+(Audio other);
    // append another buffer to this buffer (quick, if sampletypes match no copying is needed)
    Audio& operator+=(Audio &other);
    // append another buffer to this buffer (quick, if sampletypes match no copying is needed)
    Audio& append(Audio buffer);

    // get first chunk in the row (of append() or +=operator concaternated 'Audio')
    Audio* firstChunk(void) const;

    // get the next chunk in the row
    // On append() or operator(+=) concaternated 'Audio':
    // - returns NULL if called on the last chunk
    // On Circular buffers or Sample loops
    // - returns the first chunk when called on the last chunk
    Audio* nextChunk(void) const;

    // get the last chunk at the end of the row
    Audio* lastChunk(void) const;
    // get that chunk which contains that frame at 'framePosition'
    Audio* getChunk( unsigned atFramePosition ) const;
    // check if an 'Audio's memory is compact or fragmended
    bool   isCompact(void) const;
    // count on chunks the 'Audio' consists from
    int chunkCount(void) const;
    // count on bytes this chunks data measures
    int chunkSize(void) const;
    // index position of currently processed chunk
    int chunkIndex(void) const;
    // begin of data in a chunk (takes the chunks individual offsets in account)
    const Data chunkData(void) const;
    // returns the idxth chunk in the row
    Audio* chunkAtIndex(int idx) const;

    // [] access operator:
    // returns pointer to the frame at given index (where index range is summ of
    // all frameCounts of all concaternated chunks the 'Audio' may consist from.
    Data operator[]( unsigned index ) const
    {
        return ( Data )( (index += (offset > 0 ? offset : 0)) < (frameCount - cutend)
                       ? (byte *)data + ( index * format.BlockAlign )
                       : nxt->operator[]( index - (frameCount - cutend)) );
    }


    // inlined templates:
    template <typename T>
    T *buffer( unsigned frame = 0 )
    {
        return (T *)( *this )[frame];
    }

    template <typename T>
    T *buffer( unsigned frame = 0 ) const
    {
        return (T*)(*this)[frame];
    }

    template <typename T>
    T *const end( void ) const
    {
        return (T *)( buffer<byte>( getLength()-1 ) + format.BlockAlign );
    }

    template < typename frameType = Frame<s16,2> >
    Audio converted( double ampl = 1.0 ) const
    {
        Audio convertedBuffer( this->format.SampleRate,
                               frameType::TS, frameType::CH,
                               this->getLength(),
                               ALLOCATE_NEW_COPY|OWN );

#define PerTypeAction( smpltyp ) \
        convertData<smpltyp>( \
            convertedBuffer.buffer<frameType>(), \
            ConversionFactor( this->format.BitsPerSample, \
                              frameType::TS ) * ampl, \
            frameType::CH, \
            (Audio*)this \
        ); break;
        SAMPLETYPE_SWITCH( format );
#undef  PerTypeAction

        return convertedBuffer.outscope();
    }

    template <typename T, const unsigned C>
    Audiom<T,C> converted( double ampl = 1.0 ) const
    {
        bool conv;
        if ( ampl == 1.0
        && (!(conv =( sizeof(T)*8 != this->format.BitsPerSample
                   || C != this->format.NumChannels ) ) ) ) {
            return *(Audiom<T,C>*)this;
        } else {
            Audiom<T,C> copy = conv
                             ? Audiom<T,C>( format.SampleRate, frameCount )
                             : Audiom<T,C>( *this );
            Frame<T,C>* dst = conv
                                   ? copy.begin()
                                   : (Frame<T,C>*)data;
            const double factor = ConversionFactor( format.BitsPerSample,
                                                    copy.format.BitsPerSample ) * ampl;
#define PerTypeAction( smpltyp ) \
            convertData<smpltyp>( \
                dst, factor, copy.format.NumChannels, (Audio*)this \
            ); break;
            SAMPLETYPE_SWITCH( format );
#undef  PerTypeAction

            return copy.outscope();
        }
    }
};


// creation macro for 'IAudioFrame' (meta-frame-interface) types which are used for implementation of
// concrete definition of each of the possible datatype/channel configuration variations which audio
// buffers may use. It derives each type from an abstract IAudioFrame interface for making possible to
// access distinct channels of concrete datatype at runtime, without knowing either concrete channel
// count nor concrete data type of acessed frame variables.
// !!! These IAudiframe variables are NOT always matching exact byte sizes with that data block they
// represent - so be carefull to NOT use these for buffer allocation. for buffer allocation, relaying
// 'Frame<TY,CH>' structure templates (which always DO match exact data size) are to be used instead!
#define TemplateForFrameStruct template <const unsigned cC>
#define AudioFrameTypeDefinition(sign,size) TemplateForFrameStruct \
struct WAVELIB_API AuPCM ## sign ## size \
    : public IAudioFrame, public Frame< sign ## size , cC> { \
public: \
    typedef Frame< sign ## size , cC> FRAME; \
    typedef AuPCM ## sign ## size TYPE; \
    virtual AudioFrameType GetFrameType() const override { \
        return AudioFrameType( WAV_PCM_TYPE_ID::PCM ## sign, size, cC ); \
    } \
    virtual Data Pan(Panorama p, Panorama::Axis a) override { \
        return (Data)pan(p, a); \
    } \
    virtual Data Mix(Data frame, float drywet) override { \
        return (Data)mix(*(FRAME*)frame, drywet); \
    } \
    virtual Data Mix(Data sample, Panorama pano) override { \
        return (Data)mix(*(sign ## size*)sample, pano); \
    } \
    virtual Data Amp(double factor) override { \
        return (Data)this->amp(factor); \
    } \
    virtual uint Channels() const override { \
        return cC; \
    } \
    virtual uint BitDepth() const override { \
        return FRAME::TS; \
    } \
    virtual uint ByteSize() const override { \
        return GetFrameType().ByteSize(); \
    } \
    virtual bool Signedty() const override { \
        return FRAME::ST >= 0; \
    } \
    virtual Panorama GetPanorama(void) override { \
        return panorama(); \
    } \
    virtual void SetPanorama(Panorama panorama) override { \
        this->pan(panorama); \
    } \
    virtual void SetChannel(int index, Data sample) override { \
        this->channel[index] = *( sign ## size* )sample; \
    } \
    virtual Data GetChannel(int index) const override { \
        return (Data)&this->channel[index]; \
    } \
    virtual IAudioFrame* Convert( uint toType ) { \
        return this->convertFromThis<TYPE>( toType ); \
    } \
    template<typename T> T Convert( void ) { \
        return this->convertToType<T>( AUDIOFRAMETYPE_CODE(TYPE) ); \
    } \
    virtual void SetData( Data concrete ) { \
        MEMCPYFUNC( &this->channel[0], concrete, size >> 3, cC ); \
    } \
    virtual IAudioFrame* Set( Data concrete ) { \
        SetData( concrete ); \
        return this; \
    } \
};

AudioFrameTypeDefinition(s, 8)
AudioFrameTypeDefinition(i, 8)
AudioFrameTypeDefinition(s,16)
AudioFrameTypeDefinition(s,24)
AudioFrameTypeDefinition(i,24)
AudioFrameTypeDefinition(f,32)
AudioFrameTypeDefinition(f,64)

#undef TemplateForFrameStruct
#undef AudioFrameTypeDefinition

enum WAVELIB_API StreamDirection :char;
enum WAVELIB_API AudioFXRoutingType :char {
    SendFX=1, FXReturn=2, InsertFX=3
};

class WAVELIB_API AbstractAudioStream;
class WAVELIB_API AbstractAudioFx {
     friend class AbstractAudioStream;
protected:
    AbstractAudioFx* fxnx;
    int              fxch;
    uint             fxid;

public:
    AbstractAudioFx() : fxnx(0), fxch(-1) {};
    virtual Audio::Data doFrames( Audio::Data in, uint fcount) = 0;
    virtual Audio::Data doFrame( Audio::Data in ) = 0;
    template<typename ST = s16>
    ST doChannel(int number, ST in) { return ST(NULL); }
    template<typename ST = s16>
    ST doSample(ST in) { return NULL; }

    virtual void   stateReset() = 0;
    virtual bool   getBypass(void) const = 0;
    virtual void   setBypass(bool bypass) = 0;
    virtual double getParameter(double number) const = 0;
    virtual void   setParameter(double number, double value) = 0;
    virtual double getWet() const = 0;
    virtual void   setWet(double) = 0;
            uint   getID(void) const { return fxid; }
    virtual uint   getParameterCount(void) const = 0;
    virtual AbstractAudioFx* chain( AbstractAudioFx* append=EMPTY_(AbstractAudioFx*) ) {
        if ((size_t)append != EMPTY_(size_t)) {
            fxnx = append;
        } return fxnx;
    }
};

template<class AudioFXBase>
class WAVELIB_API IFxReturn;

template<class AudioFXBase = AbstractAudioFx>
class WAVELIB_API IFxSend : public AudioFXBase {
protected: struct SendConfig {
    AbstractAudioStream* stream;
    Format* format;
    double  level;
}   Send;
    IFxReturn<AbstractAudioFx>* output;
public:
    IFxSend() : output(0) { Send.stream = 0; Send.level = 0; };
    void Init(AbstractAudioStream* attached, Format* fmt, void* returnDst) {
        output = (IFxReturn<AbstractAudioFx>*)returnDst;
        Send.stream = attached;
        Send.format = fmt;
    }
    template<typename ST = s16>
    ST doSample(ST in) {
        AudioFXBase::fxch = (++AudioFXBase::fxch != Send.format->NumChannels)
                            ?  AudioFXBase::fxch : 0;
        return AudioFXBase::doChannel(AudioFXBase::fxch,in);
    }
    void setLevel(double amplitude) {
        Send.level = amplitude;
    }
    double getLevel(void) {
        return Send.level;
    }
};

template<class AudioFXBase = AbstractAudioFx>
class WAVELIB_API IFxReturn : public AudioFXBase {
protected: struct ReturnConfig {
    AbstractAudioStream* stream;
    Format*  format;
    double   level;
}   Return;
    IFxSend<IFxReturn<AbstractAudioFx >>* input;
public:
    IFxReturn() : input(0) { Return.stream = 0; Return.level = 0; };
    void Init(AbstractAudioStream* attached, Format* fmt, void* sendSrc) {
        input = (IFxSend<IFxReturn<AbstractAudioFx >>*)sendSrc;
        Return.stream = attached;
        Return.format = fmt;
    }
    virtual double getWet(void) {
        return Return.level;
    }
    virtual void setWet(double dryWetMix) {
        Return.level = dryWetMix;
    }

};


class WAVELIB_API IFxInsert
    : public IFxSend< IFxReturn< AbstractAudioFx > > {
public:
    virtual void Init(AbstractAudioStream* attach,Format* fmt) {
        IFxSend::Init(attach, fmt, this);
        IFxReturn::Init(attach, fmt, this);
    }
    virtual AbstractAudioFx* chain( AbstractAudioFx* newMember = EMPTY_(AbstractAudioFx*)) {
        if (newMember != EMPTY_(AbstractAudioFx*)) {
            ((IFxReturn*)newMember)->Init(Return.stream, Return.format, this);
            ((IFxSend*)newMember)->Init(Return.stream, Send.format, newMember);
            this->output = (IFxReturn*)newMember;
            ((IFxInsert*)newMember)->input = (IFxSend<IFxReturn<AbstractAudioFx>>*)this;
            this->fxnx = newMember;
        } return fxnx;
    }
    _declspec(property(get = getWet, put = setWet)) double DryWet;
    _declspec(property(get = getBypass, put = setBypass)) bool Bypass;
};

template<class AudioStreamBase = AbstractAudioStream>
class WAVELIB_API IAudioOutStream : public AudioStreamBase
{
public:
    IAudioOutStream() : AudioStreamBase() {};
    virtual uint framesWritable(void) const = 0;
    virtual uint GetWritePosition(void) const = 0;
    virtual uint GetDuration() const = 0;
    virtual uint Write( Audio::Data send, uint cbSize, uint offset ) = 0;
    virtual uint Write( Audio::Data send, int framecount ) = 0;
    virtual uint Write( const Audio& send ) = 0;
    virtual uint WriteChannel( int chandex, Audio::Data send, int samplescount ) = 0;
    virtual uint WriteFrame( Audio::Data frame ) = 0;
};

template<class AudioStreamBase = AbstractAudioStream>
class WAVELIB_API IAudioInStream : public AudioStreamBase
{
public:
    IAudioInStream() : AudioStreamBase() {};
    virtual uint   framesReadable(void) const = 0;
    virtual uint   GetReadPosition(void) const = 0;
    virtual uint   GetDuration() const = 0;
    virtual uint   Read( Audio::Data receive, uint cbSize, uint offset ) = 0;
    virtual uint   Read( Audio &receive, uint framecount ) = 0;
    virtual Audio  Read( uint framecount ) = 0;
    virtual uint   ReadChannel( int chandex, Audio::Data receive, int samplecount ) = 0;
    template <typename Fr>
    Fr ReadFrame( void );
};

class WAVELIB_API NoFX : public IFxInsert
{
public:
    virtual bool getBypass(void) const { return true; }
    virtual void setBypass(bool bypass) {};
    virtual double getParameter(double number) const { return 0; };
    virtual void setParameter(double number, double value) {};
    virtual double getWet() const { return 0; };
    virtual void setWet(double) {};
    virtual uint getID(void) const { return 0; };
    virtual void stateReset(void) {};
    virtual uint getParameterCount(void) const { return 0; }
    template<typename FT = AuPCMs16<2>>
    void doFrames(FT* in, uint count) {};
    template<typename FT = AuPCMs16<2>>
    FT* doFrame(FT* in) { return in; };
    template<typename ST = s16>
    ST doChannel(int channel, ST in) { return in; };
};

#define fileheaderfunc(returntype,functionname) returntype functionname(void) const { \
        return isWavFile() ? wav()->functionname() \
             : isSndFile() ? snd()->functionname() \
                           : pam()->functionname(); }

struct WAVELIB_API AbstractAudioFileHeader {
private: uint headercaster[27];
    const WavFileHeader* wav() const { return (const WavFileHeader*)&headercaster[0]; }
    const SndFileHeader* snd() const { return (const SndFileHeader*)&headercaster[0]; }
    const PamFileHeader* pam() const { return (const PamFileHeader*)&headercaster[0]; }
public:
    bool isWavFile() const { return headercaster[2] == HEADER_CHUNK_TYPE::WavFormat; }
    bool isSndFile() const { return headercaster[0] == HEADER_CHUNK_TYPE::SndFormat; }
    bool isPamFile() const { return headercaster[0] == HEADER_CHUNK_TYPE::P7mFormat
                                 || headercaster[0] == HEADER_CHUNK_TYPE::P8mFormat; }

    fileheaderfunc(word, GetHeaderSize)
    fileheaderfunc(uint, GetDataSize)
    fileheaderfunc(Data, GetAudioData)
    fileheaderfunc(bool, isValid)
    fileheaderfunc(word, GetBitDepth)
    fileheaderfunc(word, GetBlockAlign)
    fileheaderfunc(word, GetChannelCount)
    fileheaderfunc(AudioFrameType, GetTypeCode)
    fileheaderfunc(uint, GetSampleRate)
    fileheaderfunc(bool, isFloatType)

    HEADER_CHUNK_TYPE GetFileFormat() const {
        if (!isWavFile()) {
            return *(const HEADER_CHUNK_TYPE*)&headercaster[0];
        } else {
            return HEADER_CHUNK_TYPE::WavFormat;
        }
    }
};
#undef fileheaderfunc

template<typename AuDaTy> class datatype_limits
    : public std::numeric_limits<AuDaTy>
{
public:
    static constexpr AuDaTy(db0)() _THROW0()
    { /* return maximum value */ return 0;
    }
};

template<> class datatype_limits<s24>
{
public:
    static constexpr s24::AritmeticType(db0)() _THROW0()
    { /* return maximum value */ return INT24_0DB; }
};

template<> class datatype_limits<i24>
{
public:
    static constexpr i24::AritmeticType(db0)() _THROW0()
    { /* return maximum value */ return UINT24_0DB; }
};

template<> class datatype_limits<i8>
{
public:
    static constexpr i8(db0)() _THROW0()
    { /* return maximum value */ return UINT8_0DB;
    }
};

template<> class datatype_limits<i16>
{
public:
    static constexpr i16(db0)() _THROW0()
    { /* return maximum value */ return UINT16_0DB;
    }
};

template<> class datatype_limits<i32>
{
public:
    static constexpr i32(db0)() _THROW0()
    { /* return maximum value */ return UINT32_0DB;
    }
};

ENDOF_WAVESPACE


#include "AudioInline.hpp"


#endif
