/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileReader.hpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFileReader_hpp_
#define _WaveFileReader_hpp_

BEGIN_WAVESPACE

    class WAVELIB_API WaveFileReader
            : public IAudioInStream<AbstractWaveFileStream>
            , public IReadLockable
        {
        private:
            unsigned         dataSize;
            ReadOrWriteHead* readhead;
            Audio            attachedBuffer;

        public:
                       WaveFileReader( void );
                       WaveFileReader( const Format& targetFormat );
                       WaveFileReader( AudioFrameType frametype );
                       WaveFileReader( Audio::Data buffer );
                       WaveFileReader( const char* fileName );
                       WaveFileReader( const char* fileName, Audio::Data buffer, uint cbsize );
            virtual   ~WaveFileReader( void );

            virtual uint        GetLength(void) const override;
            virtual uint        GetDuration(void) const override;
            virtual uint        Open(const char* fileName);
            bool                HasTargetFormat(void) const;
            Format              GetTargetFormat(void) const;
            void                SetTargetFormat(const Format& readAs);
            virtual uint        Close(void);
            virtual void        Seek(unsigned frame, StreamDirection direction = READ);
            unsigned         Read(Audio::Data buffer, unsigned samplesOffset, unsigned samplesCount );
            virtual uint     Read(Audio::Data buffer, int framecount);
            unsigned         Read(Audio& buffer, unsigned frames=0 );
            virtual Audio    Read(uint = NULL);
            virtual uint     GetPosition(StreamDirection = READ) const override;
            virtual uint     GetReadPosition(void) const override { return GetPosition(); }
            virtual StreamDirection GetDirection(void) const override { return READ; };
            virtual uint     ReadChannel(int chn, Audio::Data, int samplecount);
            i8               ReadByte(void);
            s16              ReadInt16(void);
            s24              ReadInt24(void);
            f16              ReadHalf(void);
            f32              ReadFloat(void);
            i32              ReadUInt32(void);
            f64              ReadDouble(void);
            s8               ReadSampleS8(void);
            s16              ReadSampleS16(void);
            f16              ReadSampleF16(void);
            s24              ReadSampleS24(void);
            f32              ReadSampleF32(void);
            i32              ReadSampleI32(void);
            f64              ReadSampleF64(void);
            Audio*           GetBuffer(void) {
                                 return &attachedBuffer;
                             }
            void             AttachBuffer( Audio buffer, bool converse=false ) {
                                 if( attachedBuffer.isValid() )
                                     attachedBuffer.drop();
                                 buffer.removeOffset();
                                 attachedBuffer = buffer;
                                 if (!converse) {
                                     hdr.GetFormat( &attachedBuffer.format );
                                     attachedBuffer.frameCount = attachedBuffer.cbSize
                                                   / attachedBuffer.format.BlockAlign;
                                 }
                                 attachedBuffer.mode.addFlag(OWN);
                                 buffer.mode.remFlag(OWN);
                                 ownBuffer = true;
                             }

            #define ReturnConvertedTypeFrame return ( ((CASE_TYPE::FRAME*)readhead)->converted<readAsTy,readAsCc>() );
            template<typename readAsTy,const unsigned readAsCc>
            WaveSpace(Frame)<readAsTy,readAsCc > ReadFrame(void) {
                if( getnext( fmt.BlockAlign ) ) {
                    FREAD_FUNC( readhead, 1, fmt.BlockAlign, f );
                    FRAMETYPE_SWITCH( GetTypeCode(), ReturnConvertedTypeFrame )
                } else {
                    return *(WaveSpace(Frame)<readAsTy,readAsCc>*)(Audio::Silence);
                }
            }
            #undef ReturnConvertedTypeFrame

            Audio::Data ReadFrame() {
                if( getnext( fmt.BlockAlign ) ) {
                    return FREAD_FUNC( readhead, 1, fmt.BlockAlign, f ) ? (Audio::Data)readhead : Audio::Silence;
                } else {
                    return Audio::Silence;
                }
            }

            void clearReadableFrames(void) {
                framesAvailable = 0;
            }

        private:
            bool canRead;
            bool getnext( unsigned );

            virtual bool lock(volatile uint& access) override {
                if (&access == &read) {
                    if (!isReading)
                        return isReading = true;
                } return false;
            }
            virtual bool unlock(volatile uint& access) override {
                if (&access == &read) {
                    if (isReading) {
                        isReading = false;
                    } return true;
                } return false;
            }

            virtual uint framesReadable(void) const override;
};

ENDOF_WAVESPACE
#endif
