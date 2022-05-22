/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      TrimmingTests.hpp                          ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _TrimmingTests_hpp_
#define _TrimmingTests_hpp_
#include "TestCaseBase.hpp"

namespace Wave {
    namespace Test {
        class TrimmingTests
            : public PerFrameType
        {
        public:
            WaveFileWriter  writer;
            Audio           buffer;
            uint            expect;
            int             tollerate;
            int             failed;
            TrimmingTests( const char* wavefilename, uint expectation, uint tollerance );

        protected:
            virtual int runTestCase( const AudioFrameType* frametype ) override;
        };
    } //end of Wave
}
#endif
