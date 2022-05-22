#ifndef _ScopedPointer_hpp_
#define _ScopedPointer_hpp_

namespace Wave {
    namespace Test {
        template<typename P> struct ScopedPointer {
        private: P* pointer;
        public: ScopedPointer(P* to) : pointer(to) {}
        virtual ~ScopedPointer(void) { delete pointer; }
        operator P* () { return pointer; }
        P& operator *() { return *pointer; }
        P* operator ->() { return pointer; }
        };
    }
}

#endif
