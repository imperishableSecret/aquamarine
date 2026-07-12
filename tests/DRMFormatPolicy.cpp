#include <aquamarine/backend/drm/FormatPolicy.hpp>

#include "shared.hpp"

#include <drm_fourcc.h>

int main() {
    int ret = 0;

    EXPECT(Aquamarine::drmBufferFormatRequiresLegacyReconfigure(false, DRM_FORMAT_XBGR2101010, DRM_FORMAT_XRGB8888), true);
    EXPECT(Aquamarine::drmBufferFormatRequiresLegacyReconfigure(true, DRM_FORMAT_XBGR2101010, DRM_FORMAT_XRGB8888), false);
    EXPECT(Aquamarine::drmBufferFormatRequiresLegacyReconfigure(false, DRM_FORMAT_XRGB8888, DRM_FORMAT_XRGB8888), false);
    EXPECT(Aquamarine::drmBufferFormatRequiresLegacyReconfigure(true, DRM_FORMAT_XRGB8888, DRM_FORMAT_XRGB8888), false);

    EXPECT(Aquamarine::drmLinkFormat(true, DRM_FORMAT_XBGR2101010, DRM_FORMAT_XRGB8888), DRM_FORMAT_XBGR2101010);
    EXPECT(Aquamarine::drmLinkFormat(false, DRM_FORMAT_XBGR2101010, DRM_FORMAT_XRGB8888), DRM_FORMAT_XRGB8888);
    EXPECT(Aquamarine::drmLinkFormat(true, DRM_FORMAT_INVALID, DRM_FORMAT_XRGB8888), DRM_FORMAT_XRGB8888);
    EXPECT(Aquamarine::drmFormatBPC(8, 16, DRM_FORMAT_XRGB8888), 8);
    EXPECT(Aquamarine::drmFormatBPC(8, 16, DRM_FORMAT_XBGR2101010), 10);
    EXPECT(Aquamarine::drmFormatBPC(8, 12, DRM_FORMAT_XRGB16161616), 12);
    EXPECT(Aquamarine::drmFormatBPC(10, 16, DRM_FORMAT_XRGB8888), 10);

    return ret;
}
