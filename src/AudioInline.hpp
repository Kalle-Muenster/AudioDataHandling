#ifndef AUDIOINLINE_HPP
#define AUDIOINLINE_HPP



BEGIN_WAVESPACE

struct Audio;
template<typename tT, const unsigned cC>
struct Audiom : public Audio {
public:
    typedef Audiom AUDIO;
    typedef Frame<tT,cC> FRAME;
    template<const unsigned cN>
    class CHANNEL
        : public FRAME, public IChannel
    {
    private:
        static CHANNEL* preIncrement(FRAME* This) {
            return ++This;
        }
        static CHANNEL* preDecrement(FRAME* This) {
            return --This;
        }
        static CHANNEL* postIncrement(FRAME* This) {
            return This++;
        }
        static CHANNEL* postDecrement(FRAME* This) {
            return This--;
        }
    protected:
        // Geerbt über IChannel
        virtual IChannel * preInc(void) override
        {
            return preIncrement(this);
        }
        virtual IChannel * pstInc(void) override
        {
            return postIncrement(this);
        }
        virtual IChannel * preDec(void) override
        {
            return preDecrement(this);
        }
        virtual IChannel * pstDec(void) override
        {
            return postDecrement(this);
        }
        virtual void * seekOp(int pos) override
        {
            return &(this+pos)->channel[cN];
        }

    public:
        tT& operator = (tT other) {
            channel[cN] = other;
            return channel[cN];
        }
        operator tT() {
            return channel[cN];
        }
        tT& operator [] (unsigned idx) {
            (this + idx)->channel[cN];
        }
        typename tT& operator*(void) {
            return this->channel[cN];
        }
        typename tT& at(unsigned idx) {
            return this->operator[](idx);
        }
        CHANNEL& operator++(void) {
            return *preIncrement(this);
        }
        CHANNEL& operator++(int) {
            return *postIncrement(this);
        }
        CHANNEL& operator--(void) {
            return *preDecrement(this);
        }
        CHANNEL& operator--(int) {
            return *postDecrement(this);
        }

    }; // CHANNEL


    Audiom(Initiatio flags = DEFAULT) : Audio(flags) {
        format.ByteRate = (
            format.BlockAlign = (
                format.NumChannels = cC)*(
                    format.BitsPerSample = sizeof(tT) * 8) / 8)*(
                        format.SampleRate = 44100);
        cbSize = frameCount = 0;
    }

    Audiom(unsigned SampleRate, unsigned FrameCount, Initiatio flags = DEFAULT)
        : Audio(SampleRate, sizeof(tT) * 8, cC, FrameCount, flags)
    {}

    Audiom(int sampleRate, Audio::Data rawData, unsigned bufferSize, Initiatio flags = DONT_ALLOCATE_NEW)
        : Audio(rawData, bufferSize, flags)
    {
        format.ByteRate = (
            format.BlockAlign = (
                format.NumChannels = cC)*(
                    format.BitsPerSample = sizeof(tT) * 8) / 8)*(
                        format.SampleRate = sampleRate);
    }

    Audiom(const Audio&copy) : Audio(copy) {
        format.ByteRate = (
            format.BlockAlign = (
                format.NumChannels = cC)*(
                    format.BitsPerSample = sizeof(tT) * 8) / 8)*
            format.SampleRate;
        if ((copy.format.BitsPerSample == (sizeof(tT) * 8))
            && copy.format.NumChannels == cC) {
            this->data = copy.data;
            mode.flags.group.owned = false;
        }
        else {
            this->data = copy.converted<tT, cC>().detachBuffer();
            mode.flags.group.owned = true;
        }
    }

    FRAME& getFrame(unsigned position) {
        return *buffer<FRAME>(position);
    }
    FRAME* begin(void) {
        return this->buffer<FRAME>();
    }
    FRAME* end(void) {
        return this->buffer<FRAME>(getLength()-1) + 1;
    }

    operator Audio&(void) {
        return *(Audio*)this;
    }

    Audiom& outscope(void) {
        mode.flags.group.owned = false;
        return this[0];
    }
    FRAME& operator[](unsigned idx) {
        return *(FRAME*)((Audio*)this)->operator[](idx);
    }

    template<const unsigned ChannelNumber>
    CHANNEL<ChannelNumber>& getChannel() {
        return *(CHANNEL<ChannelNumber>*)&begin();
    }
};

ENDOF_WAVESPACE
#endif

