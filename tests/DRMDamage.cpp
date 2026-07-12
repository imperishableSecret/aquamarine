#include <aquamarine/backend/drm/Atomic.hpp>

#include "shared.hpp"

using namespace Hyprutils::Math;

int main() {
    int                ret = 0;
    constexpr Vector2D SIZE{1920, 1080};

    EXPECT(Aquamarine::drmDamageNeedsBlob({}, SIZE), false);
    EXPECT(Aquamarine::drmDamageNeedsBlob(CBox{0, 0, 1920, 1080}, SIZE), false);
    EXPECT(Aquamarine::drmDamageNeedsBlob(CBox{-10, -10, 1940, 1100}, SIZE), false);
    EXPECT(Aquamarine::drmDamageNeedsBlob(CBox{0, 0, 100, 100}, SIZE), true);
    EXPECT(Aquamarine::drmDamageNeedsBlob(CBox{2000, 1200, 100, 100}, SIZE), false);

    CRegion disjoint;
    disjoint.add(CBox{0, 0, 960, 1080}).add(CBox{960, 0, 960, 1080});
    EXPECT(Aquamarine::drmDamageNeedsBlob(disjoint, SIZE), false);

    CRegion holed{CBox{0, 0, 1920, 1080}};
    holed.subtract(CBox{100, 100, 100, 100});
    EXPECT(Aquamarine::drmDamageNeedsBlob(holed, SIZE), true);

    return ret;
}
