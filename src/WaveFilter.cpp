/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      FrequencySplit.cpp                         ||
||     Author:    Kalle                                      ||
||     Generated: 25.09.2019                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveSpacer.h"
#include "WaveFilter.hpp"
#ifdef HAVE_CONTROLLED_VALUES


#endif

/*
#include <enumoperators.h>
#include "WaveFormat.hpp"
#include "WaveAudio.hpp"
#include "WaveFilter.hpp"
WaveSpace(DotChannelSpliter)::DotChannelSpliter(void)
    : m_filter(NULL)
    , m_channels(c5d1)
    , m_format()
{
    m_effect.Split.range = NotEffective;
    m_effect.Band[0] = {0,0.25,1.25};
    m_effect.Band[1] = {0,0.25,1.25};
    m_effect.Band[2] = {0,1,0};
    m_dotband = {1,0,0};
    Initialize(CreateWaveFormat(44100,16,8),MidPass);
}

WaveSpace(DotChannelSpliter)::~DotChannelSpliter(void)
{
    delete[] m_filter;
}

bool
WaveSpace(DotChannelSpliter)::applyOn(Audio* track)
{
    if ( track->format != m_format )
         SetFormat( track->format );
    else Reset();
    const uint end = track->getLength();
    FRAMETYPE_SWITCH( track->frameTypeCode(),
        for (uint f = 0; f < end; ++f) {
            CASE_TYPE* fr = (CASE_TYPE*)(*track)[f]; uint fC = 0;
            for (uint c = 4; c < m_format.NumChannels; ++c) {
                m_filter[fC] = (toDbl*fr->channel[c]);
                fr->channel[c] = CASE_TYPE::TY(toTyp*m_filter[fC]);
            }
        } break
    );
    for (uint c = 0; c < m_format.NumChannels-4; ++c)
        if(m_filter[c].GetPin<double>(PEAKVal)>1)
            return false;
    return true;
}

void
WaveSpace(DotChannelSpliter)::doFrame(Data next)
{
    #define PerTypeAction(T) {\
        T* chns = (T*)next;\
        for(int i = 0;i<m_channels;++i){int I=i+4;\
            m_filter[i] = (toDbl*chns[I]);\
            chns[I] = T(toTyp*m_filter[i]);\
        }\
    } SAMPLETYPE_SWITCH(m_format);
    #undef PerTypeAction
}


void
WaveSpace(DotChannelSpliter)::Initialize(const Format& fmt, DotPoint cnf)
{
    int check = fmt.NumChannels > 4;
    if (!check) {
        //m_effect = false;
        return;
    }
    check = fmt.NumChannels - 4;
    if (check > m_channels) {
        m_channels = check;
        m_effect.Split.range = cnf;
        m_format = fmt;
        Renew();
    } else {
        m_channels = check;
        m_format = fmt;
        if ( m_effect.Split.range != cnf ) {
            Reset();
        }
    }
}

void
WaveSpace(DotChannelSpliter)::connectPins(Type config)
{
    if ( config == m_channels )
        return;

    if ( !config )
        config = Type(m_channels);

    uint chan = 0;
    if ( config == c7d1 ) {
        while( chan<2 ) {
            m_filter[chan].GetPin<double*>(LO_GAIN) = &m_effect.Band[chan].lo;
            m_filter[chan].GetPin<double*>(MI_GAIN) = &m_effect.Band[chan].mi;
            m_filter[chan].GetPin<double*>(HI_GAIN) = &m_effect.Band[chan].hi;
        ++chan; }
    }
    m_filter[chan].GetPin<double*>(LO_GAIN) = &m_effect.Band[2].lo;
    m_filter[chan].GetPin<double*>(MI_GAIN) = &m_effect.Band[2].mi;
    m_filter[chan].GetPin<double*>(HI_GAIN) = &m_effect.Band[2].hi;
    ++chan;
    m_filter[chan].GetPin<double*>(LO_GAIN) = &m_dotband.lo;
    m_filter[chan].GetPin<double*>(MI_GAIN) = &m_dotband.mi;
    m_filter[chan].GetPin<double*>(HI_GAIN) = &m_dotband.hi;
}

void
WaveSpace(DotChannelSpliter)::Renew(void)
{
    if( m_filter )
        delete[] m_filter;
    m_filter = new STEPFLOW(VALUES,Controlled<double>)[m_channels];
    int chan = 0;
    if (m_channels == c7d1) {
        for(chan; chan < 2; ++chan) {
            m_filter[chan].SetUserMode<EQ3BandFilter<double>>(
                (double)m_effect.Split.point.lo,
                (double)m_effect.Split.point.hi,
                (double)0, (double)m_format.SampleRate
            );
        }
    }
    m_filter[chan].SetUserMode<EQ3BandFilter<double>>(
        m_effect.Split.point.lo,
        m_effect.Split.point.hi,
        0,m_format.SampleRate
    );
    ++chan;
    m_filter[chan].SetUserMode<EQ3BandFilter<double>>(
        m_effect.Split.point.lo,
        m_effect.Split.point.hi,
        0, m_format.SampleRate
    );
    connectPins(Type(0));
}
void
WaveSpace(DotChannelSpliter)::Reset(void)
{
    toDbl = ConversionFactor(m_format.BitsPerSample, 64);
    toTyp = ConversionFactor(64, m_format.BitsPerSample);
    for (int i = 0; i < m_channels; i++) {
        m_filter[i].MIN = m_effect.Split.point.lo;
        m_filter[i].MAX = m_effect.Split.point.hi;
        m_filter[i].MOVE = m_format.SampleRate;
        m_filter[i].template GetUserMode<EQ3BandFilter<double>>()->resetState();
    }
}

void
WaveSpace(DotChannelSpliter)::SetFormat(const Format& fmt)
{
    if (m_format.NumChannels != fmt.NumChannels) {
        Initialize(fmt,DotPoint(m_effect.Split.range));
    } else {
        m_format = fmt;
        Reset();
    }
}

void
WaveSpace(DotChannelSpliter)::SetSplitFrequencies(word lo, word hi)
{
    m_effect.Split.point.lo = lo;
    m_effect.Split.point.hi = hi;
    Reset();
}
#endif
*/