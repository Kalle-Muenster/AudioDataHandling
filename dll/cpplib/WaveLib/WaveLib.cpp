#include "precomphead.hpp"
#include "wavelib.hpp"

const char VersionNumberString[] = WAVELIB_VERSION_STRING;

WAVELIB_API unsigned
NameSpace(GetVersionNumber)() {
    return WAVELIB_VERSION_NUMBER;
}

WAVELIB_API const char*
NameSpace(GetVersionString)() {
    return &VersionNumberString[0];
}
