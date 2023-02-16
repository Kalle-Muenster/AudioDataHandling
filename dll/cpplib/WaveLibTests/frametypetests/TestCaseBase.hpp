#ifndef TEST_CASE_BASE_HPP
#define TEST_CASE_BASE_HPP

#include <wavelib.hpp>

#define SET_STEPFLOW_NAMESPACE Wave
#define SET_LISTS_NAMESPACE Test
#include <StepLists.h>

namespace Wave {
    namespace Test {

        class AbstractTestCase
        {
        protected:
            static FILE* log;
            char*        FormatToString(const Format&);
            char*        FrameToString(IAudioFrame*);
            char*        TypeCodeToString(uint typecode);
            uint         PcmTagToFourCC(WAV_PCM_TYPE_ID tag);
            void         PrintLog(const char* msg);
            const char*  pool_setFl(float value);
            const char*  pool_setFl(const char* fmt,float val);
            const char*  pool_setDl(const char* fmt,double val);
            const char*  pool_setHl(const char* fmt,Float16 val);
            char*        GetFileNameForFrameType( const char* filePrefix,
                            const AudioFrameType* typeSufix,
                            const char* extension = NULL
                         );
            ulong   F2CC(float val);

            int            steps;

        public:

            static void    SetLogFile(FILE* file);
        };

        template<typename T = const AudioFrameType*>
        class PerTypeTests : public AbstractTestCase
        {
        public:
            typedef DynamicStrait<T> TypeCases;

        protected:
            TypeCases         cases;
            virtual int       runTestCase(T frametype) = 0; 
            virtual void      AddTestCasesPerTypeCase( TypeCases* testcases ) = 0;

        public:

             ~PerTypeTests(void) { cases.Clear(true); }

            int Run(void) {
                steps = cases.fromEachReturn<int>( this, &PerTypeTests::runTestCase );
                return steps;
            }
        };

        class PerFrameType
            : public PerTypeTests<const AudioFrameType*> {
        public:
            PerFrameType();
        protected:
            virtual void AddTestCasesPerTypeCase( TypeCases* testcases ) override;
        };
    }
}
#endif