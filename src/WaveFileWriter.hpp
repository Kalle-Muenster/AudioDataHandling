/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileWriter.hpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFileWriter_hpp_
#define _WaveFileWriter_hpp_


BEGIN_WAVESPACE

    class WAVELIB_API AbstractWaveFileStream;
    class WAVELIB_API IWriteLockable;
    class WAVELIB_API WaveFileWriter
        : public IAudioOutStream<AbstractWaveFileStream>
        , public IWriteLockable
    {
    public:
        enum StreamingMode : uint {
            Store,
            Stream
        };

    private:
        unsigned      fixedSize;
        StreamingMode mode;
        Audio::Data   attachedBuffer;
        ReadOrWriteHead* writehead;
        void          prepareWavHdr( uint cbSize = 0 );
        bool          Open( uint cbSize, FILE* file, FileFormat ext = WAV);
    public:
        WaveFileWriter(void);

        WaveFileWriter(
            FILE* FileHandle,
            const Format& fmt,
            Audio::Data buffer = Audio::Silence,
            unsigned fixedframeCount = 0
        );
        WaveFileWriter(
            const char* fileName,
            unsigned    dataSize = 0
        );
        WaveFileWriter(
            const char*   fileName,
            unsigned      frequency,
            word          bitDepth,
            word          channels,
            unsigned      dataSize = 0,
            Audio::Data   dataBuffer = Audio::Silence,
            StreamingMode streamMode = Store
        );
        WaveFileWriter(
            const char*   fileName,
            const Audio&  dataChunk,
            StreamingMode streamMode = Store
        );
        WaveFileWriter(
            const char*   fileName,
            const Format& fmt,
            unsigned      cbSize = 0
        );

        virtual uint  Open( const char* fileName ) override;
        virtual uint  Close(void) override;
        virtual uint  GetLength(void) const override;
        virtual uint  GetDuration(void) const override;
        virtual void  Seek(unsigned position,StreamDirection) override;
        virtual      ~WaveFileWriter(void);

        bool          Open( const char* fileName, FileFormat ext, uint framecount = 0 );
        bool          NewFile( const char* fileName, const Format& fmt, FileFormat ext = Def );
        bool          NewFile( const char* fileName, const AudioFrameType type, int rate, FileFormat ext = Def );
        bool          NewFile( const char* nam, unsigned frq, unsigned bit, unsigned chn, FileFormat ext = Def );
        uint          Save( const Audio& buffer, const char* filename, FileFormat fileFormat = Def );
        uint          Flush(void);
        void          SetFormat( const Format& fmt );
        void          SetFormat( unsigned frq, unsigned bit, unsigned chn );
        void          ReStart( bool rewriteHeader = false );

        uint          Write( void );
        virtual uint  Write( const Audio& );
        virtual uint  Write( Audio::Data data, int framesCount );
        virtual uint  Write( Audio::Data data, uint cbSize, uint offset );

        uint          WriteSample(s8);
        uint          WriteSample(s16);
        uint          WriteSample(s24);
        uint          WriteSample(i32);
        uint          WriteSample(f32);
        uint          WriteSample(f64);

        uint          WriteFrame( void );
        virtual uint  WriteFrame( Audio::Data frame );
        virtual uint  WriteChannel( int channel, Audio::Data dat, int samplecount );

        uint          WrittenBytes(void) const;
        uint          WrittenFrames(void) const;
        uint          WrittenTime(void) const;
        virtual uint  GetPosition(StreamDirection direction=WRITE) const override;
        virtual uint  GetWritePosition(void) const override { return GetPosition(); }
        virtual StreamDirection GetDirection(void) const override { return WRITE; }
        void          SetFile(FILE*);
        void          SetFileName(const char*);
        StreamingMode getMode(void);
        void          setMode( StreamingMode );
        void          attachBuffer( Audio datachunk,
                                    StreamingMode = Store,
                                    bool forceClose = true );

        template< typename T >
        unsigned      MixFrame(T sample) {
            if (!f)return 0;
            const uint sizeofT = whdr.AudioFormat.BitsPerSample / 8;
            for (int i = 0; i < whdr.AudioFormat.NumChannels; i++) {
                T smpl = fx ? fx->doChannel<T>(i,sample) : sample;
                write += WRITE_DATA( &smpl, 1, sizeofT, f );
            } return write >= fixedSize ? Close() : write;
        }

        template< typename T >
        unsigned      MixFrame(T sample, Panorama mixer) {
            if( !f ) return 0;
            if( !mixer.isEffective() )
                return MixFrame<T>( sample );
            const uint sizeofT = whdr.AudioFormat.BitsPerSample / 8;
            for( int i = 0; i < whdr.AudioFormat.NumChannels; i++) {
                T smpl = channelMixer<T>( sample, i, mixer,
                            whdr.AudioFormat.NumChannels>2 );
                if (fx) smpl = fx->doChannel<T>( i, smpl );
                write += WRITE_DATA( &smpl, 1, sizeofT, f);
            } return write >= fixedSize ? Close() : write;
        }

        // from IAudioOutStream
        virtual uint framesWritable(void) const override;

        // from AbstractWaveFileStream
        virtual bool lock(volatile uint& access) override {
            if ( &access == &write ) {
                if ( !isWriting )
                    return isWriting = true;
            } return false;
        }
        virtual bool unlock(volatile uint& access) override {
            if ( &access == &write ) {
                if (isWriting) {
                    isWriting = false;
                } return true;
            } return false;
        }
    };
ENDOF_WAVESPACE
#endif
