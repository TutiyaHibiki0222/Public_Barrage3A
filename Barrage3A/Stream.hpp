#pragma once
/*
    Stream.hpp

   Fclass
        - Stream

    ì¬“ú         : 2025/04/02
    ÅI•ÏX“ú     : 2025/04/02
*/

#include <fstream>
#include <string>

namespace System {
    namespace IO {
        class Stream {
        public:
            virtual ~Stream() = default;
            virtual void Close() = 0;
        };
    }// namespace IO
}// namespace System