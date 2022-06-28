/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      namespacer.h                               ||
||     Author:    Kalle                                      ||
||     Generated: 25.09.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef NAMESPACER_H
#define NAMESPACER_H

#ifndef SET_NAMESPACE
#include <settings.h>
#endif

/*
Set preprocessor sympol '/D:SET_NAMESPACE=NamespaceToUse' or include
some "settings.h" header file before this file, which contains:
'#define SET_NAMESPACE NamespaceToUse'. Anything which includes these
headers and which is surrounded by 'BEGIN_NAMESPACE' and 'ENDOF_NAMESPACE'
macros (in .hpp files) or has a leading 'USING_NAMESPACE' (in .cpp files)
than will be declared in that namespace defined by SET_NAMESPASE.
To disable declaring in any namespace at all, 'SET_NAMESPACE -1' or
'SET_NAMESPACE EMPTY' can be declared to make anything being declared global.
*/

#if !defined(SET_NAMESPACE)
#define SET_NAMESPACE -1
#endif

#if !defined(NAMESPACE_API)
#define NAMESPACE_API
#endif

#if SET_NAMESPACE == -1 || SET_NAMESPACE == 0xffffffff
#ifndef NAMESPACE
#define NAMESPACE
#define NameSpace(decl) decl
#define BEGIN_NAMESPACE
#define ENDOF_NAMESPACE
#define USING_NAMESPACE
#endif
#else
#ifndef NAMESPACE
#define NAMESPACE       SET_NAMESPACE
#endif
#define USING_NAMESPACE using namespace NAMESPACE;
#define BEGIN_NAMESPACE namespace NAMESPACE {
#define NameSpace(decl) NAMESPACE :: decl
#define ENDOF_NAMESPACE }
#endif

#endif





