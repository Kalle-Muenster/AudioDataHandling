/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      FrameTypesTests.hpp                        ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _FrameTypesTests_hpp_
#define _FrameTypesTests_hpp_

#include "TestCaseBase.hpp"

namespace Wave {
    namespace Test {
        class FrameTypes : public PerFrameType {
        public:
            FrameTypes(void);
        protected:
            char datas[10];
            virtual int runTestCase(const AudioFrameType* frametype);
        };
    } //end of Wave::Tests
}
#endif
