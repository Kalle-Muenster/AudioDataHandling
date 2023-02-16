/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioFrame.cpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 01.06.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include <limits.h>
#include <memory.h>

#include "WaveSpacer.h"
#include "WaveMacros.hpp"
#include "WaveFormat.hpp"
#include "WaveAudio.hpp"
#include "AudioFrame.hpp"


uint
WaveSpace(IAudioFrame)::Frequenz( void ) const
{
    double dur = Duration();
    if ( dur != 0 ) {
        return (uint)((1.0 / Duration()) * 1000);
    } else return 0;
}

WaveSpace(Format)
WaveSpace(IAudioFrame)::GetFormat( void ) const
{
    return CreateWaveFormat( Frequenz(), BitDepth(), Channels() );
}

WaveSpace(AudioFrameType)
WaveSpace(IAudioFrame)::FrameTyp( void ) const
{
    return AudioFrameType( BitDepth(), Channels(), Frequenz() );
}

WaveSpace(IAudioFrame)*
WaveSpace(AudioFrameType)::CreateEmptyFrame( void ) const {
    uint tc = info.type.data[0];
    FRAMETYPE_SWITCH( tc,
        CASE_TYPE* frame = new CASE_TYPE();
    return reinterpret_cast<IAudioFrame*>(frame);
    )
}

WaveSpace(IAudioFrame)*
WaveSpace(AudioFrameType)::CreateMuchoViele( uint viele ) const {
    uint tc = info.type.data[0];
    FRAMETYPE_SWITCH( tc,
        CASE_TYPE* frames = new CASE_TYPE[viele];
    return reinterpret_cast<IAudioFrame*>(frames);
    )
}