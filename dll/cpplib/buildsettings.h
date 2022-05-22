#include "../../inc/importsettings.h"

#define WAVELIB_VERSION_STRING {\
      ((WAVELIB_VERSION_NUMBER & 0xff000000) >> 24)+0x30,\
 '.', ((WAVELIB_VERSION_NUMBER & 0x00ff0000) >> 16)+0x30,\
 '.', ((WAVELIB_VERSION_NUMBER & 0x0000ff00) >> 8) +0x30,\
 '.',  (WAVELIB_VERSION_NUMBER & 0x000000ff)+0x30, '\0' }
