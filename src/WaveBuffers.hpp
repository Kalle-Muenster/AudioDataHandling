/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveBuffer.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 12.01.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveBuffer_hpp_
#define _WaveBuffer_hpp_

#if defined(__GNUC__)||defined(__MINGW32__)||defined(__MINGW64__)
#include <typeinfo>
#endif



BEGIN_WAVESPACE

    enum WAVELIB_API StreamDirection:char;

    class WAVELIB_API AudioStream
        : public Audio
        , public IAudioInStream<IAudioOutStream<AbstractAudioStream>>
        , public IReadLockable
        , public IWriteLockable
    {
    private:
        uint nextBarrier(void);
        int  ensureSize(uint byteCount);

    protected:
        virtual bool lock( volatile uint& access ) override {
            if ( &access == &this->read ) {
                if ( !locked() )
                    return (this->isReading = 1u);
            } else
            if (&access == &this->write) {
                if ( !locked() )
                    return (this->isWriting = 1u);
            } return false;
        }
        virtual bool unlock( volatile uint& access ) override {
            if ( &access == &this->read ) {
                if (this->isReading)
                    this->isReading = 0u;
                return true;
            } else
            if ( &access == &this->write ) {
                if (this->isWriting) {
                    this->isWriting = 0u;
                } return true;
            } return false;
        }
        bool locked() {
            return  this->isReading || this->isWriting;
        }

    public:
        AudioStream(void) : Audio() { fx = 0; };

        AudioStream( uint srt, uint bit, uint chn, int framesPerChunk )
            : Audio(srt, bit, chn, framesPerChunk) {
            this->mode.addFlag( OWN | FIRST | LAST );
            this->read = this->write = 0;
            this->fx = 0;
        };

        AudioStream( const Audio &wrap )
            : Audio( wrap ) {
            this->mode.addFlag( FIRST | LAST );
            this->read = this->write = 0;
            this->fx = 0;
        };

        AudioStream( const Format& fmt, Audio::Data dat, uint siz )
        {
            this->cbSize = siz;
            this->format = fmt;
            this->frameCount = this->cbSize / this->format.BlockAlign;
            this->offset = this->cutend = 0;
            this->read = this->write = 0;
            this->mode.setFlags( Initiatio::DONT_ALLOCATE_NEW );
            this->fx = 0;
        };

        virtual ~AudioStream(void) {
            if((this->mode.flags.group.owned)||(this->ptd == (size_t)this))
                if(this->fx) delete this->fx;
            drop();
        }

        AudioStream outscope(void);

        // implements AbstractAudioStream
        virtual bool isValid(void) const override;
        virtual Format* GetFormat(void) const override;
        virtual word GetTypeCode(void) const override;
        virtual uint GetLength(void) const override;
        virtual uint GetDuration(void) const override;
        virtual uint GetPosition(StreamDirection direction) const override;
        virtual StreamDirection GetDirection(void) const override;
        virtual void Seek(unsigned newPosition, StreamDirection direction = StreamDirection(READ | WRITE)) override;

        // implements IAudioOutStream
        virtual uint Write(Audio::Data send, uint cbsize, uint offset) override;
        virtual uint Write(Audio::Data send, int framecount) override;
        virtual uint Write(const Audio& send) override;
        virtual uint WriteFrame( Audio::Data frame ) override;
        virtual uint WriteChannel(int chandex, Audio::Data send, int samplescount) override;
        virtual uint GetWritePosition(void) const override;
        virtual uint framesWritable(void) const override;

        // implements IAudioInStream
        virtual uint   Read(Audio::Data receive, uint cbSize, uint offset) override;
        virtual uint   Read(Audio &receive, uint framecount = -1);
        virtual Audio  Read(uint framecount) override;
        virtual uint   ReadChannel(int chandex, Audio::Data receive, int samplecount) override;
        virtual uint   GetReadPosition(void) const override;
        virtual uint   framesReadable(void) const override;

        AudioStream* operator&(void) {
            return this;
        }

        template <typename TY>
        TY ReadFrame(void) {
            TY dstFrm;
            byte* dst = (byte*)&dstFrm;
            byte* src = (byte*)(*this)[this->read++];
            for (int i = 0; i < this->format.BlockAlign; ++i)
                dst[i] = src[i];
            return dstFrm;
        }
    };


    class WAVELIB_API AbstractCycleBuffer
        : public Audio
    {
    protected:
        volatile bool isReading;
        volatile bool isWriting;
        volatile bool needCounterReset;
        unsigned      wholeCycle;

        uint read;
        uint write;

        void normalizeCounters(void) {
            if (needCounterReset) {
                this->wholeCycle = ((int)this->write - (int)this->read) == this->frameCount
                           ? this->frameCount : 0;
                if (!this->isReading) {
                    this->read = this->read % this->frameCount;
                    this->write = this->write % this->frameCount;
                } this->needCounterReset = false;

            } else if (!(this->needCounterReset = this->isWriting)) {
                this->wholeCycle = ((int)this->write - (int)this->read) == this->frameCount
                           ? frameCount : 0;
                this->read = this->read % this->frameCount;
                this->write = this->write % this->frameCount;
            }
        }

        bool lock( volatile uint& access ) {
            if (&access == &this->read) {
                if ( (!this->isReading) && (!this->isWriting) )
                    return (this->isReading = true);
                return false;
            } else if (&access == &this->write) {
                if ((!this->isReading) && (!this->isWriting))
                    return (this->isWriting = true);
                return false;
            } return false;
        }

        bool unlock( volatile uint& access ) {
            if (&access == &this->read) {
                if (this->isReading)
                    this->isReading = false;
                normalizeCounters();
                return true;
            } else if (&access == &this->write) {
                if (this->isWriting) {
                    normalizeCounters();
                    this->isWriting = false;
                } return true;
            } return false;
        }

    public:
        typedef volatile uint& AccessLock;
        typedef AccessLock ReadAccessLock;

        AbstractCycleBuffer( Audio& initFrom )
            : Audio( initFrom.format, initFrom.data, initFrom.cbSize, OWN|CIRCULAR|DONT_ALLOCATE_NEW )
        {
            this->wholeCycle = this->needCounterReset = this->isReading = this->isWriting = this->read = this->write = 0;
            initFrom.mode.remFlag(OWN);
            this->nxt = this;
        }
        AbstractCycleBuffer( i16 typecode, uint framecount, int frq )
            : Audio(frq, AUDIO_BITS_FROM_TYPECODE(typecode), AUDIO_CHANCOUNT_TYPECODE(typecode), framecount, CIRCULAR|ALLOCATE_NEW_COPY|OWN )
        {
            this->wholeCycle = this->needCounterReset = this->isReading = this->isWriting = this->read = this->write = 0;
            this->nxt = this;
        }
        AbstractCycleBuffer(i16 typecode, Data allocatedBuffer, uint bufferSizeBytes, int frq)
            : Audio( CreateWaveFormat( frq, AUDIO_BITS_FROM_TYPECODE(typecode), AUDIO_CHANCOUNT_TYPECODE(typecode) ), allocatedBuffer, bufferSizeBytes, CIRCULAR|DONT_ALLOCATE_NEW )
        {
            this->wholeCycle = this->needCounterReset = this->isReading = this->isWriting = this->read = this->write = 0;
            this->nxt = this;
        }

        uint availableFrames(void) const {
            if (this->write == this->read)
                return this->wholeCycle;
            else if (this->write < this->read)
                return (this->write + this->frameCount) - this->read;
            else
                return this->write - this->read;
        }

        uint getReadPosition(void) const {
            return this->read % this->frameCount;
        }

        // achieves a 'ReadAccessLock', which makes
        // the buffer range ( 'availableFrames()' of
        // length, beginning at framecount position
        // 'getReadPosition()' ) which it applies to,
        // inaccessible for other threads being read
        // or even being written, as long the lock is
        // holden by a caller untill caller finished
        // reading and released the lock via passing
        // it back via 'releaseReadLock(AccessLock)'
        ReadAccessLock getReadLock(void) {
            if (lock(this->read))
                return this->read;
            else return *(uint*)Audio::Silence;
        }
        bool releaseReadLock(ReadAccessLock key) {
            return unlock(key);
        }

        // non-blocking frame wise read function.
        // for using it threadsafe, a lock should
        // be achieved by calling 'getReadLock()'
        // just before, and 'releaseReadLock()'
        // just after processing read operations.
        virtual Data ReadFrame(void) {
            return (*this)[this->read++];
        }
        virtual Data PeekFrame(void) {
            return (*this)[this->read];
        }

        // get a reference to the buffer (maybe truncated at ueberschlag points and re-arranged, but still
        // straight through readable by Audio::operator[]() )... so no raw pointer access may be possible.
        Audio getReadableAudio(void);

        // pure abstract virtuals:
        virtual uint  ReadFrames(Audio* dst, uint length = EMPTY, bool stealth = false) = 0;
        virtual Audio ReadFrames(uint count, bool stealth = false) = 0;
        virtual uint  Read(byte* buffer, uint cbOffset, uint cbLength,bool stealth=false) = 0;

        template <typename TY>
        TY ReadFrame(void) {
            TY dst;
            MEMCPYFUNC(&dst, (*this)[this->read], this->format.BlockAlign, 1);
            ++this->read;
            return dst;
        }
    };

    class WAVELIB_API Circular
        : public AbstractCycleBuffer
    {
    public:
        typedef AccessLock WriteAccessLock;

        Circular(Audio& initFrom);
        Circular(i16 typecode, uint framecount, int frq = 44100);
        Circular(i16 typecode, Data allocatedBuffer, uint bufferSizeBytes, int frq = 44100);

        //Circular operator->() {
        //    return *this;
        //}
        Circular* operator&() {
            return this;
        }

        // read 'length' (or, if ommited 'available') count on frames
        // into the Audio 'dst' is pointing to. if dst Audio may come
        // at wrong fotmat, it will be reset to match src format then
        virtual uint ReadFrames(Audio* dst, uint length = EMPTY, bool stealth = false);

        // read 'count' on frames into a new allocated buffer of matching size
        virtual Audio ReadFrames(uint count, bool stealth = false);

        // read 'cbLength' count on bytes to a memory block pointed by 'buffer'
        virtual uint Read(byte* buffer, uint cbOffset, uint cbLength, bool stealth = false);

        // get end mark of the written data
        uint getWritePosition(void) {
            return this->write % this->frameCount;
        }

        // copy 'length' count on frames (or if ommited all frames) from 'src' into the cycle
        uint WriteFrames(const Audio& src, uint length = EMPTY);

        // copy 'cbLength' bytes of data pointed by 'buffer' + 'cbOffset' into the cycle
        uint WriteFrames(Data buffer, uint cbOffset, uint cbLength);

        // non-blocking frame wise write function.
        // for using it threadsafe, a lock should
        // be achieved by calling 'getWriteLock()'
        // just before, and 'releaseWriteLock()'
        // just after precessing write operations.
        void WriteFrame( Data frame ) {
            MEMCPYFUNC((*this)[this->write++],frame,1, this->format.BlockAlign);
        }

        // acheive a 'WriteAccesLock'. no other thread
        // than actual caller will be able writing into
        // the cycle untill the caller returns its lock
        // by calling 'releaseWriteLock(WriteAccessLock)'
        WriteAccessLock getWriteLock(void);

        // releases a lock which caller previously has
        // achieved by 'getWriteLock()' function.
        bool releaseWriteLock(WriteAccessLock key);



    };

/*
#define ifGetBufferLock(aubuf,key) \
WaveSpace(Circular):: key ## AccessLock key = aubuf->get ## key ## Lock();\
 if( & key != WaveSpace(Audio)::Silence )

#define unlockBuffer(aubuf,key) aubuf->release ## key ## Lock( key );

#define releaseLock(key) }\
WaveSpace(Circular)::lockOwner<WaveSpace(Circular):: ## key ## AccessLock>(key)->release ## key ## Lock( key );\
}
*/

ENDOF_WAVESPACE
#endif
