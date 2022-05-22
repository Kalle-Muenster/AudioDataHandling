/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioChannel.hpp                           ||
||     Author:    Kalle                                      ||
||     Generated: 01.06.2019                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _AudioChannel_hpp_
#define _AudioChannel_hpp_
#include "WaveAudio.hpp"

BEGIN_WAVESPACE

struct Audio;
class IChannel;

template<const unsigned cN, typename sT = s16, const unsigned cC = 2>
class ChannelIterator
    : public Frame<sT,cC>
    , public IChannel
{
public:
    typedef Frame<sT,cC> FRAME;

private:
    static ChannelIterator* preIncrement(FRAME* This) {
        return ++This;
    }
    static ChannelIterator* preDecrement(FRAME* This) {
        return --This;
    }
    static ChannelIterator* postIncrement(FRAME* This) {
        return This++;
    }
    static ChannelIterator* postDecrement(FRAME* This) {
        return This--;
    }
protected:
    virtual IChannel* preInc(void) { return preIncrement(this); };
    virtual IChannel* pstInc(void) { return postIncrement(this); };
    virtual IChannel* preDec(void) { return preDecrement(this); };
    virtual IChannel* pstDec(void) { return postDecrement(this); };
    virtual void* seekOp(int pos) { return (void*)&(this + pos)->channel[cN]; };

public:
    sT& operator [] (unsigned idx) {
        return getSample<sT>(idx);
    }
    sT& operator * (void) {
        return this->channel[cN];
    }
    ChannelIterator* operator & (void) {
        return this;
    }
    sT& at(unsigned idx) {
        return getSample<sT>(idx);
    }
    ChannelIterator& operator++(void) {
        return *preIncrement(this);
    }
    ChannelIterator& operator++(int) {
        return *postIncrement(this);
    }
    ChannelIterator& operator--(void) {
        return *preDecrement(this);
    }
    ChannelIterator& operator--(int) {
        return *postDecrement(this);
    }
};


ENDOF_WAVESPACE

#endif