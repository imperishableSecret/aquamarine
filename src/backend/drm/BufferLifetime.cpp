#include <aquamarine/backend/drm/BufferLifetime.hpp>

#include <algorithm>

bool Aquamarine::drmBufferIsReferenced(const void* buffer, std::span<const void* const> liveBuffers) {
    return buffer && std::ranges::find(liveBuffers, buffer) != liveBuffers.end();
}
