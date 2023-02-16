/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveStream.cpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 22.01.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveSpacer.h"
#include <precomphead.hpp>
#ifndef  USE_PRE_COMPILED_HEADER
#include <limits.h>
#include <memory.h>

#include <WaveLib.inl/half.hpp>
#include <WaveLib.inl/numbersendian.h>
#include <WaveLib.inl/enumoperators.h>
#include <WaveLib.inl/indiaccessfuncs.h>
#include <WaveLib.inl/Int24bitTypes.hpp>

#include "WaveMacros.hpp"
#include "Panorama.hpp"
#include "WaveFormat.hpp"
#include "AudioChannel.hpp"

#include "WaveAudio.hpp"
#include "WaveStream.hpp"
#endif


bool
WaveSpace(AbstractWaveFileStream)::validCheck(const AbstractWaveFileStream* obj)
{
    bool checkVal = false;
    if (obj && obj != nullptr) {
        try { checkVal = (obj->hdr.isValid() && obj->f);
        } catch (...) { checkVal = false; }
    } return checkVal;
}

bool
WaveSpace(AbstractWaveFileStream)::isValid(void) const
{
    return validCheck(this);
}

FILE*
WaveSpace(AbstractWaveFileStream)::GetFile(void)
{
    return f;
}

WaveSpace(FileName)
WaveSpace(AbstractWaveFileStream)::GetFileName(void) const
{
    return &fileName[0];
}

WaveSpace(AbstractWaveFileStream::FileFormat)
WaveSpace(AbstractWaveFileStream)::GetFileFormat(void) const
{
    return headerFormat;
}

WaveSpace(Format*)
WaveSpace(AbstractWaveFileStream)::GetFormat(void) const
{
    return const_cast<Format*>(&fmt);
}

WaveSpace(AbstractAudioFileHeader*)
WaveSpace(AbstractWaveFileStream)::GetHeader(void)
{
    return &hdr;
}

word
WaveSpace(AbstractWaveFileStream)::GetTypeCode(void) const
{
    return hdr.GetTypeCode();
}

WaveSpace(AbstractWaveFileStream)::FileFormat
WaveSpace(AbstractWaveFileStream)::copyFileNameAndCheckExtension( char* dstFnam, const char* srcFnam, Format* pcmFmt, FileFormat fileType )
{
    FileFormat fileFormat = WAV;
    const char* extpos = srcFnam - 1;
    char* dst = dstFnam - 1;
    while (*++dst = *++extpos);
    while (*--extpos != '.');
    char ar[4] = { extpos[0], extpos[1], extpos[2], extpos[3] };
    uint Extension = *(uint*)&ar[0];

    if ( pcmFmt != NULL ) {
        if( Extension != FOURCC( '.', 'p', 'a', 'm' ) ) {
            if( pcmFmt->BitsPerSample == 16
             && pcmFmt->PCMFormatTag == PCMf ) {
                fileType = PAM;
            } else if( Extension == FOURCC( '.', 'w', 'a', 'v' ) ) {
                if( ( pcmFmt->BitsPerSample == 8
                      && pcmFmt->PCMFormatTag == PCMs ) ) {
                    fileType = SND;
                }
            }
        }
    }
    if ( fileType != Def ) {
        switch ( fileType ) {
        case FileFormat::WAV: Extension = FOURCC( 'w', 'a','v', 0 ); break;
        case FileFormat::SND: Extension = FOURCC( 'a', 'u', 0 , 0 ); break;
        case FileFormat::PAM: Extension = FOURCC( 'p', 'a','m', 0 ); break;
        }  while (*--dst != '.');
        *(uint*)++dst = Extension;
        fileFormat = fileType;
    } else if (( Extension == FOURCC('.', 'a', 'u', '\0'))
             || (Extension == SND) ) {
        if (pcmFmt) {
            if( pcmFmt->BitsPerSample != 8
             && pcmFmt->PCMFormatTag & PCMs ) {
                while (*--dst != '.');
                *(uint*)++dst = FOURCC('w', 'a', 'v', '\0');
                Extension = FOURCC('.', 'w', 'a', 'v');
                fileFormat = WAV; }
        } else fileFormat = SND;
    } else if ( Extension == FOURCC('.','p','a','m')
             || Extension == FOURCC('.','p','n','m')
             || fileType == PAM ) {
        fileFormat = PAM;
        if ( pcmFmt ) {
            if ( pcmFmt->BitsPerSample == 8 ) {
                while (*--dst != '.');
                *(uint*)dst = FOURCC('.','a','u','\0');
                fileFormat = SND;
            } 
        }
    } else fileFormat = WAV;
    return fileFormat;
}
