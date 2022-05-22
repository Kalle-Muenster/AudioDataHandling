#include "AudioBuffer.hpp"
#include "AudioStreamBuffer.hpp"
#include "CircularBuffer.hpp"
//...

generic<class T>
Stepflow::Audio::IAudioFrame^ Stepflow::Audio::Circular<T>::ReadFrame(void)
{
    MANAGEDTYPE_SWITCH(au->frameTypeCode(),
        CASE_TYPE::NATIVE fr = au->ReadFrame<CASE_TYPE::NATIVE>();
    return *(CASE_TYPE*)&fr; )
}
