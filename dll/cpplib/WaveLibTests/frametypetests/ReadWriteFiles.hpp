/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      ReadWriteFiles.hpp                         ||
||     Author:    Kalle                                      ||
||     Generated: 31.05.2020                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _ReadWriteFiles_hpp_
#define _ReadWriteFiles_hpp_

#include "TestCaseBase.hpp"

namespace Wave { 
    namespace Test {
        class ReadWriteFiles : public PerFrameType
        {
        public:
            WaveFileReader reader;
            WaveFileWriter writer;

            ReadWriteFiles(void);
            virtual ~ReadWriteFiles(void);

        protected:
            virtual int runTestCase(const AudioFrameType* frametype) override;
        };
    }
} //end of Wave

#endif
