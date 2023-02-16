/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveStream.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 22.01.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveStream_hpp_
#define _WaveStream_hpp_


BEGIN_WAVESPACE

////////////////////////////////////////////////////////////////////////
/* Abstract Audio Stream interfaces ----                              */

enum WAVELIB_API StreamDirection : char {
    SET = 0, CUR = 1, END = 2, READ = 4, WRITE = 8,  INPUT = READ, OUTPUT = WRITE, INOUT = READ|WRITE
};

// base for Audio streaming buffers for providing
// thread safe read/write access functionallities.
// It doesn't declare any API for read/write access
// at all, but it (protected) declares a locking
// mechanism which can be used for implementing
// access API via interface classes 'IAudioInStream'
// and/or 'IAudioOutStream' or 'AbstractWaveFileStream'
class WAVELIB_API AbstractAudioStream
{
protected:
    //void(*onStreamEmpty)(AbstractAudioStream* sender);
    AbstractAudioFx* fx;
    virtual bool lock(volatile uint& access) = 0;
    virtual bool unlock(volatile uint& access) = 0;

public:
    typedef volatile uint& AccessLock;

    AbstractAudioStream()
        : fx(0) {
    };
    virtual bool            isValid(void) const = 0;
    virtual Format*         GetFormat(void) const = 0;
    virtual uint            GetPosition(StreamDirection) const = 0;
    virtual StreamDirection GetDirection(void) const = 0;
    virtual word            GetTypeCode(void) const = 0;
    virtual uint            GetLength(void) const = 0;
    // das is pralle dumm! ->      StreamDirection can be given SET or END or CUR
    //                             ...also mit bem unsigned position whert geht CUR nur vorwerts
    //                                (relative zum aktueller position verschieben)
    virtual void            Seek(unsigned position, StreamDirection) = 0;
    void fxAdd( AbstractAudioFx* audioFx ) {
        if(!fx) fx = audioFx;
        else { AbstractAudioFx* next = fx;
            while( next->chain() )
                next = next->chain();
            next->chain( audioFx );
        }
    }
    AbstractAudioFx* fxGet( uint id ) {
        if (fx) { AbstractAudioFx* next = fx;
            do{ if (next->getID() == id)
                return next;
            } while ( next = next->chain() );
        } return 0;
    }
    void fxDel( uint id ) {
        if( fx ) { AbstractAudioFx* next=fx, *last=0;
            do{ if( next->getID() == id ) {
                if( last ) last->chain( next->chain() );
                next->chain( 0 ); delete next; next = 0;
              break; } last = next;
            } while( next = next->chain() );
        }
    }
};

class WAVELIB_API IReadLockable {
protected:
    uint          read;
    volatile uint isReading;
    virtual bool  lock(volatile uint& access) = 0;
    virtual bool  unlock(volatile uint& access) = 0;
public:
    typedef AbstractAudioStream::AccessLock ReadAccessLock;
    ReadAccessLock getReadLock(void) {
        if ( lock( read ) )
            return read;
        else return *(uint*)Audio::Silence;
    }
    bool releaseReadLock( ReadAccessLock key ) {
        return unlock( key );
    }
};

class WAVELIB_API IWriteLockable {
protected:
    uint          write;
    volatile uint isWriting;
    virtual bool  lock(volatile uint& access) = 0;
    virtual bool  unlock(volatile uint& access) = 0;
public:
    typedef AbstractAudioStream::AccessLock WriteAccessLock;
    WriteAccessLock getWriteLock(void) {
        if ( lock( write ) )
            return write;
        else return *(uint*)Audio::Silence;
    }
    bool releaseWriteLock( WriteAccessLock key ) {
        return unlock( key );
    }
};

// Base class for WaveFileReader and WaveFleWriter
typedef const char* FileName;
class WAVELIB_API AbstractWaveFileStream : public AbstractAudioStream
{
public:
    enum FileFormat : uint {
        Def = 0,
        WAV = HEADER_CHUNK_TYPE::WavFormat,
        SND = HEADER_CHUNK_TYPE::SndFormat,
        PAM = HEADER_CHUNK_TYPE::P7mFormat,
    };

private:
    static bool validCheck(const AbstractWaveFileStream*);

protected:
    Format         fmt;
    unsigned       framesAvailable;
    FileFormat     headerFormat;
    FILE*          f;
    bool           ownBuffer;
    char           fileName[255];
    AbstractAudioFileHeader hdr;

    FileFormat     copyFileNameAndCheckExtension( char*, const char*, Format*, FileFormat = Def );

public:

    virtual unsigned Open(FileName fileName) = 0;
    virtual unsigned Close(void) = 0;
    const char*      File(void) const { return &fileName[0]; };
    virtual bool     isValid(void) const;
    FileFormat       GetFileFormat(void) const;
    FILE*            GetFile(void);
    FileName         GetFileName(void) const;
    AbstractAudioFileHeader* GetHeader(void);
    virtual Format*  GetFormat(void) const;
    virtual word     GetTypeCode(void) const;
    virtual unsigned GetPosition(StreamDirection) const = 0;

    unsigned         getFramesAvailable(void) const {
        if (framesAvailable == EMPTY)
            return (hdr.GetDataSize() - GetPosition(GetDirection())) / fmt.BlockAlign;
        return framesAvailable;
    }
    unsigned         getBytesAvailable(void) const {
        if (framesAvailable == EMPTY)
            return hdr.GetDataSize() - GetPosition(GetDirection());
        return framesAvailable * fmt.BlockAlign;
    }
    AbstractWaveFileStream(void)
        : headerFormat(WAV)
        , framesAvailable(EMPTY)
        , f(NULL) {
        *(WavFileHeader*)&hdr = initializeNewWaveheader();
    }
};

ENDOF_WAVESPACE
#endif
