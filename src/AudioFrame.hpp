/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      AudioFrame.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 01.06.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _AudioFrame_hpp_
#define _AudioFrame_hpp_

#include "WaveMacros.hpp"
#include "Panorama.hpp"

BEGIN_WAVESPACE

#define EatKommas1 CASE_TYPE::FRAME * neu = FrameTyp().CreateEmptyFrame(neuType);\
    *neu = ((from*)this)->converted<CASE_TYPE::TY, CASE_TYPE::CH>();\
    return neu
#define EatKommas2 into* newFrame = new into();\
    *newFrame = ((CASE_TYPE::FRAME*)this)->converted<into::TY, into::CH>();\
    return newFrame

ENDOF_WAVESPACE
#endif
