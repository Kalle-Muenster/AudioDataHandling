/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFilter.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 28.07.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifdef HAVE_CONTROLLED_VALUES
   #if HAVE_CONTROLLED_VALUES > 0
#ifndef _WaveFilter_hpp_
#define _WaveFilter_hpp_

#ifndef WITH_BANDFILTER_MODE
#define WITH_BANDFILTER_MODE
#endif
#include <ControlledValues.h>

BEGIN_WAVESPACE

#ifdef IMPORT_WAVELIB
inline uint generateFxID(void);
#else
inline uint generateFxID(void)
{
    static uint idgenerator = 0;
    return ++idgenerator;
}
#endif

template<typename TY, const unsigned CH, typename StepflowControlMode>
class WAVELIB_API StepflowFX : public IFxInsert
{
protected:
    typedef Frame<TY, CH>        FRAME;
    STEPFLOW( VALUES, PinFieldController<TY> )  impl[CH];

public:
    FRAME FX;
    StepflowFX() {
        fxid = generateFxID();
        for( int ch = 0; ch < CH; ++ch ) {
            if( std::numeric_limits<TY>().is_integer )
                impl[ch].SetUp<StepflowControlMode<FT::TY>>(
                    std::numeric_limits<TY>().min()+2,
                    std::numeric_limits<TY>().max()-1,0,0 );
            else
                impl[ch].SetUp<StepflowControlMode<TY>>(-1,1,0,0);
            impl[ch].SetVariable( &FX.channel[ch] );
            impl[ch].SetCheckAtGet(false);
            impl[ch].SetActive(true);
        }
    }
    void           applyFrame() {
        for (int ch = 0; ch < FT::CH; ++ch)
            impl[ch].Check();
    }
    void           applyChannel(int number) {
        impl[number].Check();
    }

    virtual bool   getBypass(void) const {
        return impl[0].IsActive();
    }
    virtual void   setBypass(bool bypass) {
        for (int ch = 0; ch < FT::CH; ++ch)
            impl[ch].SetActive(bypass);
    }
    template<typename PT>
    PT     getParameter(uint number) const {
        return impl[0].GetPin<PT>(number);
    }
    template<typename PT>
    void   setParameter(uint number, PT value) {
        for (int ch = 0; ch < FT::CH; ++ch)
            impl[ch].GetPin<PT>(number) = value;
    }
    virtual double getWet(void) const { return Return.level; };
    virtual void   setWet(double val) { Return.level = val; };
    virtual uint   getID(void) const = 0;
    virtual void   stateReset(void) {
        for (int ch = 0; ch < FT::CH; ++ch) {
            impl[ch].SetActive(false);
            impl[ch].SetActive(true);
        }
    }
    virtual uint   getParameterCount(void) const = 0;

    virtual Audio::Data doFrames( Audio::Data buffer, uint count ) {
        FT* in = (FT*)buffer;
        for (uint frame = 0; frame < count; ++frame) {
            FX = in[frame]; applyFrame(); in[frame] = FX;
        } return buffer;
    };

    virtual Audio::Data doFrame( Audio::Data frame ) {
        FX = (FT*)frame; apply();
        return &FX;
    };

    TY doChannel( int number, TY sample ) {
        FX.channel[number] = sample;
        applyChannel(number);
        return FX.channel[number];
    };
};



template<typename TY=s16,const unsigned CH=2, typename PZ=float>
class WAVELIB_API ThreeBandEQ
    : public StepflowFX<TY,CH,STEPFLOW(VALUES,DreiBandEQ)<TY,PZ>>
{
    typedef TY                                         TYPE;
    typedef STEPFLOW(VALUES, DreiBandEQ)<TY,PZ>        MODE;
    typedef StepflowFX<TYPE,CH,MODE>                   BASE;
    typedef STEPFLOW(VALUES, pins::DreiBandEQ)         PINS;
    typedef STEPFLOW(VALUES, ControllerVariables)      VARS;

private:
    static PINS getPinIndex(int idx){
        switch (idx) {
        case 0: return PINS::LO_GAIN;
        case 1: return PINS::MI_GAIN;
        case 2: return PINS::HI_GAIN;
        case 3: return PINS::IN_GAIN;
        case 4: return PINS::LO_BAND;
        case 5: return PINS::MI_BAND;
        case 6: return PINS::HI_BAND;
        case 7: return PINS::LoSplit;
        case 8: return PINS::HiSplit;
        case 9: return PINS::PEAKVAL;
        default: return EMPTY_(PINS);
        }
    }
    PZ OmniPZ[4];
    TY OmniST[2];
    bool UseOmnichannelParameters;

public:
    struct PARA {
        const static PZ IN_GAIN = 3.0f;
        const static PZ LO_GAIN = 0.0f;
        const static PZ MI_GAIN = 1.0f;
        const static PZ HI_GAIN = 2.0f;
        const static PZ LO_BAND = 4.0f*256;
        const static PZ MI_BAND = 5.0f*256;
        const static PZ HI_BAND = 6.0f*256;
        const static TY LOSPLIT = 7;
        const static TY HISPLIT = 8;
        const static TY PEAKSMP = 9*256;
    };


public:
    ThreeBandEQ() : BASE() {
        UseOmnichannelParameters = true;
        for( int ch = 0; ch < CH; ++ch ) {
            impl[ch].SetPin<uint>( &Send.format->SampleRate, getPinIndex(8) );
            if( UseOmnichannelParameters ) {
                impl[ch].LetPoint(VARS::Min,&OmniST[0]);
                impl[ch].LetPoint(VARS::Max,&OmniST[1]);
                impl[ch].SetPin<PZ>(&OmniPZ[0],getPinIndex(0));
                impl[ch].SetPin<PZ>(&OmniPZ[1],getPinIndex(1));
                impl[ch].SetPin<PZ>(&OmniPZ[2],getPinIndex(2));
                impl[ch].SetPin<PZ>(&OmniPZ[3],getPinIndex(3));
            }
        }
    }
    virtual uint getParameterCount(void) const {
        return 12;
    }
    virtual bool getBypass(void) const {
        return impl[0].GetPin<bool>(PINS::BY_PASS);
    }
    virtual void setBypass(bool bypass) {
        for (int ch = 0; ch < CH; ++ch)
            impl[ch].GetPin<bool>(PINS::BY_PASS) = bypass;
    }
    virtual void stateReset(void) {
        for (int ch = 0; ch < CH; ++ch) {
            impl[ch].GetUserMode<MODE>()->resetState();
        }
    }
    template<typename PT=double>
    PT getParameter(PT number) const {
       int idx = (int)number;
       if( idx < 4 )
           return OmniPZ[idx];
       if (idx < 9)
           return OmniST[idx];
       return impl[idx%256].GetPin<PT>(
           getPinIndex(idx/256)
       );
    }
    template<typename PT = double>
    void setParameter( PT number, PT value ) {
        int idx = (int)number;
        if (idx < 4)
            OmniPZ[idx] = value;
        else if (idx < 9)
            OmniST[idx] = value;
        else impl[idx%256].GetPin<PT>(
            getPinIndex(idx/256)
        ) = value;
    }
};


ENDOF_WAVESPACE

#endif
#endif
#endif
