#pragma once

#include <span>

namespace Aquamarine {
    bool drmBufferIsReferenced(const void* buffer, std::span<const void* const> liveBuffers);
}
