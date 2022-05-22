/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ResourceIterator.h                         ||
||     Author:    Kalle                                      ||
||     Generated: 04.06.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _TestData_Resource_Included_
#define _TestData_Resource_Included_
#ifdef __cplusplus
extern "C" {
#endif
//*************Resource includig intrfaface******************//

    typedef unsigned char byte;
    typedef unsigned int  uint;

    typedef enum TYPE {
        BIN = 'b',
        B64 = 's'
    } TYPE;

    typedef unsigned(*TestResources_Size)(void);
    typedef const char*(*TestResources_Name)(void);
    typedef const byte*(*TestResources_Data)(void);
    typedef const TYPE(*TestResources_Type)(void);

    typedef struct Resource {
        TestResources_Name name;
        TestResources_Type type;
        TestResources_Size size;
        TestResources_Data data;
    } Resource;

#define RESOURCE_LIST_ENTRY(ResNam) {\
    &##ResNam##_getName,\
    &##ResNam##_getType,\
    &##ResNam##_getSize,\
    &##ResNam##_getData\
}
#define RESOURCE_DATA_ACCES(ResNam) \
    unsigned    ResNam##_getSize(void); \
    const char *ResNam##_getName(void); \
    const byte *ResNam##_getData(void); \
    const TYPE  ResNam##_getType(void)

//*********Resource accessor function declarations***********//

    const Resource* TestData_resourceByIndex(int idx);
    const Resource* TestData_resourceByName(const char *nam);
          unsigned  TestData_resourcesHowMany(void);

#ifdef __cplusplus
}
#endif
#endif
