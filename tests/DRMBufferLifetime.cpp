#include <aquamarine/backend/drm/BufferLifetime.hpp>

#include "shared.hpp"

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct SPlaneSlots {
    const void* last  = nullptr;
    const void* front = nullptr;
    const void* back  = nullptr;
};

class CBufferLifetimeHarness {
  public:
    void submit(const void* primaryBuffer, const void* cursorBuffer = nullptr) {
        m_primary.back = primaryBuffer;
        m_cursor.back  = cursorBuffer;
        lock(primaryBuffer);
        lock(cursorBuffer);
    }

    void present() {
        std::vector<const void*> retired;
        rotate(m_primary, retired);
        rotate(m_cursor, retired);
        releaseRetired(retired);
    }

    void replaceStash(const void* primaryBuffer, const void* cursorBuffer = nullptr) {
        const std::array retired = {m_stashedPrimary, m_stashedCursor};
        m_stashedPrimary         = primaryBuffer;
        m_stashedCursor          = cursorBuffer;
        lock(primaryBuffer);
        lock(cursorBuffer);
        releaseRetired(retired);
    }

    void discardStash() {
        const std::array retired = {m_stashedPrimary, m_stashedCursor};
        m_stashedPrimary         = nullptr;
        m_stashedCursor          = nullptr;
        releaseRetired(retired);
    }

    void teardown() {
        const std::array retired = {
            m_primary.last, m_primary.front, m_primary.back, m_cursor.last, m_cursor.front, m_cursor.back, m_pendingCursor, m_stashedPrimary, m_stashedCursor,
        };
        m_primary        = {};
        m_cursor         = {};
        m_pendingCursor  = nullptr;
        m_stashedPrimary = nullptr;
        m_stashedCursor  = nullptr;
        releaseRetired(retired);
    }

    bool locked(const void* buffer) const {
        return m_lockedBuffers.contains(buffer);
    }

    size_t releases(const void* buffer) const {
        const auto it = m_releaseCounts.find(buffer);
        return it == m_releaseCounts.end() ? 0 : it->second;
    }

  private:
    static void rotate(SPlaneSlots& plane, std::vector<const void*>& retired) {
        retired.emplace_back(plane.last);
        plane.last  = nullptr;
        plane.last  = plane.front;
        plane.front = plane.back;
        plane.back  = nullptr;
        retired.emplace_back(plane.last);
        plane.last = nullptr;
    }

    void lock(const void* buffer) {
        if (buffer)
            m_lockedBuffers.emplace(buffer);
    }

    template <typename T>
    void releaseRetired(const T& retired) {
        const auto                      LIVE = liveBuffers();
        std::unordered_set<const void*> seen;
        for (const auto* buffer : retired) {
            if (!buffer || !seen.emplace(buffer).second || Aquamarine::drmBufferIsReferenced(buffer, LIVE) || !m_lockedBuffers.erase(buffer))
                continue;
            ++m_releaseCounts[buffer];
        }
    }

    std::vector<const void*> liveBuffers() const {
        return {
            m_primary.last, m_primary.front, m_primary.back, m_cursor.last, m_cursor.front, m_cursor.back, m_pendingCursor, m_stashedPrimary, m_stashedCursor,
        };
    }

    SPlaneSlots                             m_primary;
    SPlaneSlots                             m_cursor;
    const void*                             m_pendingCursor  = nullptr;
    const void*                             m_stashedPrimary = nullptr;
    const void*                             m_stashedCursor  = nullptr;
    std::unordered_set<const void*>         m_lockedBuffers;
    std::unordered_map<const void*, size_t> m_releaseCounts;
};

int main() {
    int ret = 0;
    int a = 0, b = 0, c = 0;

    EXPECT(Aquamarine::drmBufferIsReferenced(&a, std::array<const void*, 1>{&a}), true);         // A -> A keeps A front.
    EXPECT(Aquamarine::drmBufferIsReferenced(&a, std::array<const void*, 1>{&b}), false);        // A -> B retires A.
    EXPECT(Aquamarine::drmBufferIsReferenced(&a, std::array<const void*, 3>{&a, &a, &b}), true); // More than one slot owns A.
    EXPECT(Aquamarine::drmBufferIsReferenced(&b, std::array<const void*, 3>{&a, &b, &c}), true); // Stashed B remains live.
    EXPECT(Aquamarine::drmBufferIsReferenced(&b, std::array<const void*, 2>{&a, &c}), false);    // Replaced stash retires B.
    EXPECT(Aquamarine::drmBufferIsReferenced(nullptr, std::array<const void*, 1>{nullptr}), false);

    CBufferLifetimeHarness lifetime;

    lifetime.submit(&a);
    lifetime.present();
    lifetime.submit(&a);
    lifetime.present();
    EXPECT(lifetime.locked(&a), true);
    EXPECT(lifetime.releases(&a), 0UL);

    lifetime.submit(&b);
    lifetime.present();
    EXPECT(lifetime.locked(&a), false);
    EXPECT(lifetime.releases(&a), 1UL);
    EXPECT(lifetime.locked(&b), true);

    lifetime.submit(&b, &a);
    lifetime.present();
    lifetime.submit(&b, &a);
    lifetime.present();
    EXPECT(lifetime.releases(&a), 1UL);
    lifetime.submit(&b, &c);
    lifetime.present();
    EXPECT(lifetime.releases(&a), 2UL);
    EXPECT(lifetime.locked(&c), true);

    lifetime.replaceStash(&b);
    lifetime.discardStash();
    EXPECT(lifetime.releases(&b), 0UL);
    lifetime.replaceStash(&a);
    lifetime.replaceStash(&c);
    EXPECT(lifetime.releases(&a), 3UL);

    lifetime.teardown();
    EXPECT(lifetime.locked(&b), false);
    EXPECT(lifetime.locked(&c), false);
    EXPECT(lifetime.releases(&b), 1UL);
    EXPECT(lifetime.releases(&c), 1UL);

    return ret;
}
