/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioFrame.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 01.06.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _AudioFrame_hpp_
#define _AudioFrame_hpp_

#include "WaveMacros.hpp"
#include "Panorama.hpp"

BEGIN_WAVESPACE


    /*
    struct WAVELIB_API IAudioFrame;
    struct WAVELIB_API AudioFrameType {
    public:
        union Info {
        union Type {
            const byte data[2];
            const word code;
        }   const type;
            const word time[2];
            const uint data;
            Info(uint argum) : data(argum) {}
        }   const info;
        f64  Duration() const { return (1.0 / (double)info.time[1]) / 1000.0; }
        word TypeCode() const { return info.time[0]; }
        word BitDepth() const { return info.type.data[0]; }
        word Channels() const { return info.type.data[1]; }
        word Frequenz() const { return info.time[1]; }
        word ByteSize() const {
            return ((info.type.data[0] & 0xff00) >> 3)
                * info.type.data[1];
        }
        AudioFrameType( uint bit, uint chn, uint srt = 0 )
            : info((const uint)(bit | chn << 8) | (srt << 16)) {
        }
        IAudioFrame* CreateEmptyFrame(void) {
            uint typcod = info.type.data[0];
            FRAMETYPE_SWITCH( typcod,
                CASE_TYPE::FRAME* frame = new CASE_TYPE::FRAME();
                return reinterpret_cast<IAudioFrame*>(frame);
            )
        }
        IAudioFrame* CreateMuchoViele(uint viele);

        bool operator ==(const AudioFrameType& other) const {
            return this->info.type.code == other.info.type.code;
        }
        bool operator !=(const AudioFrameType& other) const {
            return this->operator==(other);
        }
    };
    */
#define EatKommas1 CASE_TYPE::FRAME * neu = FrameTyp().CreateEmptyFrame(neuType);\
    *neu = ((from*)this)->converted<CASE_TYPE::TY, CASE_TYPE::CH>();\
    return neu
#define EatKommas2 into* newFrame = new into();\
    *newFrame = ((CASE_TYPE::FRAME*)this)->converted<into::TY, into::CH>();\
    return newFrame
/*
    struct WAVELIB_API IAudioFrame {
    public:
        virtual Data Pan(Panorama p, Panorama::Axis a) = 0;
                Data Pan(Panorama p) { return Pan( p, Panorama::Axis::Any ); }
        virtual Data Mix(Data frame, float drywet) = 0;
        virtual Data Mix(Data sample, Panorama pano) = 0;
        virtual Data Amp(double factor) = 0;

        template<typename from>
        IAudioFrame* CreateConverted(word neuType) {
            FRAMETYPE_SWITCH( neuType, EatKommas1 );
        }
        template<typename into>
        into* ConverseCreate(word fromTc) {
            FRAMETYPE_SWITCH( fromTc, EatKommas2 );
        }

        Format           GetFormat() const;
        virtual Panorama GetPanorama( void ) = 0;
        virtual void     SetPanorama(Panorama) = 0;

        Audio::FrameType FrameTyp() const;
                uint   Frequenz() const;
        virtual uint   Channels() const = 0;
        virtual uint   BitDepth() const = 0;
        virtual uint   ByteSize() const = 0;
        virtual  f64   Duration() const = 0;
        virtual bool   Signedty() const = 0;

        virtual void   set_channel(int index, Data sample) = 0;
        virtual Data   get_channel(int index) const = 0;

        LISTPROP( Data, channel, 10 );

    };
    */
ENDOF_WAVESPACE
#endif
