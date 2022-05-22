/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      PanoramaTests.hpp                          ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _PanoramaTests_hpp_
#define _PanoramaTests_hpp_

#include "TestCaseBase.hpp"

namespace Wave { 
    namespace Test {
        class PanoramaTests : public PerTypeTests<Panorama::Modus>
        {
        protected:
            virtual int  runTestCase(Panorama::Modus modi) override;
            virtual void AddTestCasesPerTypeCase(TypeCases* testcases) override;

        public:        
            static const char* PanoramaToString(Panorama*);
            PanoramaTests(void);
            virtual ~PanoramaTests(void);
        };
    }
} //end of Wave

#endif
