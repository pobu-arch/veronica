#pragma once

namespace veronica
{
    #if defined __APPLE__
    typedef unsigned char byte;
    #endif
    
    typedef          long long int  int64;
    typedef unsigned long long int uint64;
}

#if defined __linux__
using byte   = std::byte;
#endif
using int64  = veronica::int64;
using uint64 = veronica::uint64;