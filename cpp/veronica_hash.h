#pragma once

#include "veronica_type.h"

namespace veronica
{
    uint64 hash_within_int(const uint64 input)
    {
        return (input * 16807) % (uint64)2147483647;
    }
}