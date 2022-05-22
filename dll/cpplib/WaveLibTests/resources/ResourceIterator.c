/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ResourceIterator.c                         ||
||     Author:    Kalle                                      ||
||     Generated: 04.06.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include <string.h>
#include "ResourceIterator.h"

#include "trimming108_Resources.h"
#include "trimming216_Resources.h"
#include "trimming224_Resources.h"
#include "trimming232_Resources.h"


const Resource ResourcenList[] = {
    RESOURCE_LIST_ENTRY(trimming108),
    RESOURCE_LIST_ENTRY(trimming216),
    RESOURCE_LIST_ENTRY(trimming224),
    RESOURCE_LIST_ENTRY(trimming232),
};

#define COUNT_ON_RESOURCES 4


const Resource* TestData_resourceByName(const char *nam)
{
    int it = -1;
    while (++it<COUNT_ON_RESOURCES)
        if( !strcmp( ResourcenList[it].name(), nam ) )
            return &ResourcenList[it];
    return 0;
}

const Resource *TestData_resourceByIndex(int idx)
{
    return idx<COUNT_ON_RESOURCES
         ? &ResourcenList[idx]
         : 0;
}

uint TestData_resourcesHowMany(void)
{
    return (uint)COUNT_ON_RESOURCES;
}

