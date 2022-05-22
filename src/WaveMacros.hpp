#ifndef _WaveFileMacros_hpp_
#define _WaveFileMacros_hpp_

#if defined(__GNUG__)
#define UNKNOWN_TYPECODE
#define SAMPLETYPE_MISSMATCH
#define INVALID_TYPECONVRSION
#define TRIMMING_ERROR
#define INVALID_AUDIODATA
#define INVALID_WAVEFILEHEADER
#else
#define UNKNOWN_TYPECODE "unknown typecode"
#define SAMPLETYPE_MISSMATCH "sample type not supported"
#define INVALID_TYPECONVRSION "invalid type conversion"
#define TRIMMING_ERROR "error while trimming"
#define INVALID_AUDIODATA "invalid audio data"
#define INVALID_WAVEFILEHEADER "invalid file header"
#endif

// helper macros, audio frame typecode related 
#define AUDIOFRAME_TYPE(TypeSignTag,TypeSizeBits,TypeChannelCount) Au ## TypeSignTag ## TypeSizeBits <TypeChannelCount>
#define AUDIOFRAME_CODE(Tsize,Ccount,PcmTyp) (Tsize|((Ccount|(PcmTyp<<6))<<8))
#define AUDIOFRAMETYPE_CODE(frameType) (frameType::STATIC_TYPESIZE_CONST|((frameType::STATIC_CHANNELS_CONST|(((-1*frameType::ST)+2)<<6))<<8))

#define AUDIO_BITS_FROM_TYPECODE(bitdepthFromTypeCode) word((uint)bitdepthFromTypeCode&0x000000FFu)
#define AUDIO_SIGN_FROM_TYPECODE(isSignedTypeTypeCode) uint((uint)isSignedTypeTypeCode&0x00004000u)
#define AUDIO_FLOATTYPE_TYPECODE(isFloatPointTypeCode) uint((uint)isFloatPointTypeCode&0x00008000u)
#define AUDIO_CHANCOUNT_TYPECODE(chanCountFromTypCode) word(((uint)chanCountFromTypCode&0x00003F00u)>>8)
#define AUDIO_FRAMESIZE_TYPECODE(blockSizeFromTypCode) word((AUDIO_BITS_FROM_TYPECODE(blockSizeFromTypCode)>>3) * \
                                                             AUDIO_CHANCOUNT_TYPECODE(blockSizeFromTypCode))

// FRAMETYPE_SWITCH implements cases per all declared FrameTypes.
// (e.g. implements cases for all possible sampletype/channelcount
// combinations for passed code block 'actiontodo'. means that
// code block which is passed is capable performing it's operations
// on any of the supported frame type configurations same way then,
// and doesn't need to care about data types, channel counts or sampling
// rates within that code fragment or snippedt which is passed
#define FrametypeCase( NumberOne, NumberTwo, NumberTri, actionOne ) case \
        AUDIOFRAME_CODE( NumberOne, NumberTwo, NumberTri ): { \
            typedef AUDIOFRAME_TYPE( NumberTri, NumberOne, NumberTwo ) CASE_TYPE; \
            actionOne \
        } break;
#define FRAMETYPE_SWITCH( tyco, actionToDo ) \
    switch (tyco) { \
        FrametypeCase(8,1, PCMi, actionToDo ) \
        FrametypeCase(8,2, PCMi, actionToDo ) \
        FrametypeCase(8,4, PCMi, actionToDo ) \
        FrametypeCase(8,6, PCMi, actionToDo ) \
        FrametypeCase(8,8, PCMi, actionToDo ) \
        FrametypeCase(8,1, PCMs, actionToDo ) \
        FrametypeCase(8,2, PCMs, actionToDo ) \
        FrametypeCase(8,4, PCMs, actionToDo ) \
        FrametypeCase(8,6, PCMs, actionToDo ) \
        FrametypeCase(8,8, PCMs, actionToDo ) \
        FrametypeCase(16,1,PCMs, actionToDo ) \
        FrametypeCase(16,2,PCMs, actionToDo ) \
        FrametypeCase(16,4,PCMs, actionToDo ) \
        FrametypeCase(16,6,PCMs, actionToDo ) \
        FrametypeCase(16,8,PCMs, actionToDo ) \
        FrametypeCase(24,1,PCMi, actionToDo ) \
        FrametypeCase(24,2,PCMi, actionToDo ) \
        FrametypeCase(24,4,PCMi, actionToDo ) \
        FrametypeCase(24,6,PCMi, actionToDo ) \
        FrametypeCase(24,8,PCMi, actionToDo ) \
        FrametypeCase(24,1,PCMs, actionToDo ) \
        FrametypeCase(24,2,PCMs, actionToDo ) \
        FrametypeCase(24,4,PCMs, actionToDo ) \
        FrametypeCase(24,6,PCMs, actionToDo ) \
        FrametypeCase(24,8,PCMs, actionToDo ) \
        FrametypeCase(32,1,PCMf, actionToDo ) \
        FrametypeCase(32,2,PCMf, actionToDo ) \
        FrametypeCase(32,4,PCMf, actionToDo ) \
        FrametypeCase(32,6,PCMf, actionToDo ) \
        FrametypeCase(32,8,PCMf, actionToDo ) \
        FrametypeCase(64,1,PCMf, actionToDo ) \
        FrametypeCase(64,2,PCMf, actionToDo ) \
        FrametypeCase(64,4,PCMf, actionToDo ) \
        FrametypeCase(64,6,PCMf, actionToDo ) \
        FrametypeCase(64,8,PCMf, actionToDo ) \
        default: { \
    throw std::exception(UNKNOWN_TYPECODE); \
                     } break; \
}

// small variant SAMPLETYPE_SWITCH macro
// just implements cases per sample type
// not by sampletype/channelcount pairs
// needs macro 'PerTypeAction(T)' being
// #defined and #undefed before and after
// implementations which make usage of it
#define SAMPLETYPE_SWITCH(fmt) \
switch ( fmt.BitsPerSample | fmt.PCMFormatTag<<8 ) { \
    case 8|(WaveSpace(PCMi)<<8):  PerTypeAction(WaveSpace(i8)); \
    case 8|(WaveSpace(PCMs)<<8):  PerTypeAction(WaveSpace(s8)); \
    case 16|(WaveSpace(PCMs)<<8): PerTypeAction(WaveSpace(s16)); \
    case 24|(WaveSpace(PCMi)<<8): PerTypeAction(WaveSpace(i24)); \
    case 24|(WaveSpace(PCMs)<<8): PerTypeAction(WaveSpace(s24)); \
    case 32|(WaveSpace(PCMf)<<8): PerTypeAction(WaveSpace(f32)); \
    case 64|(WaveSpace(PCMf)<<8): PerTypeAction(WaveSpace(f64)); \
}

#define FOURCC(c1,c2,c3,c4) HEADER_CHUNK_TYPE( c1 + (c2<<8) + (c3<<16) + (c4<<24) )
#define LONGCC(a1,b1,c1,d1,a2,b2,c2,d2) ulong( FOURCC(a1,b1,c1,d1) | (FOURCC(a2,b2,c2,d2)<<32) )

#ifdef _MSC_VER
// declare a property of type <type> named <prop> from
// (already existing) functions <type>toGet get_<prop>()
// and void set_<prop>(<type>toSet)
#define DECLPROP( type, prop ) __declspec( property( get = get_ ## prop, put = set_ ## prop ) ) type prop
#define PROPDECL( type, name ) __declspec( property( get = Get ## name, put = Set ## name ) ) type name
// declare an array of <type> properties named <prop>
// from existing functions get_<prop>(int idx) and
// set_<prop>(int idx,<type> val) as <type> <prop>[<size>]
#define LISTPROP( type, prop, size ) __declspec( property( get = get_ ## prop, put = set_ ## prop ) ) type prop[size]
#define PROPARRAY( type, prop, size ) __declspec( property( get = Get ## prop, put = Set ## prop ) ) type prop[size]
#endif


#endif

