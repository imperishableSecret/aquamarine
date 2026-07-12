#include <aquamarine/backend/drm/FormatPolicy.hpp>

#include <algorithm>
#include <drm_fourcc.h>

bool Aquamarine::drmBufferFormatRequiresLegacyReconfigure(bool directScanoutBuffer, uint32_t outputFormat, uint32_t bufferFormat) {
    return outputFormat != bufferFormat && !directScanoutBuffer;
}

uint32_t Aquamarine::drmLinkFormat(bool directScanoutBuffer, uint32_t outputFormat, uint32_t bufferFormat) {
    return directScanoutBuffer && outputFormat != DRM_FORMAT_INVALID ? outputFormat : bufferFormat;
}

uint8_t Aquamarine::drmFormatBPC(uint64_t minimum, uint64_t maximum, uint32_t drmFormat) {
    uint8_t formatBPC = 8;

    switch (drmFormat) {
        case DRM_FORMAT_XRGB8888:
        case DRM_FORMAT_XBGR8888:
        case DRM_FORMAT_RGBX8888:
        case DRM_FORMAT_BGRX8888:
        case DRM_FORMAT_ARGB8888:
        case DRM_FORMAT_ABGR8888:
        case DRM_FORMAT_RGBA8888:
        case DRM_FORMAT_BGRA8888: formatBPC = 8; break;

        case DRM_FORMAT_XRGB2101010:
        case DRM_FORMAT_XBGR2101010:
        case DRM_FORMAT_RGBX1010102:
        case DRM_FORMAT_BGRX1010102:
        case DRM_FORMAT_ARGB2101010:
        case DRM_FORMAT_ABGR2101010:
        case DRM_FORMAT_RGBA1010102:
        case DRM_FORMAT_BGRA1010102: formatBPC = 10; break;

        case DRM_FORMAT_XRGB16161616:
        case DRM_FORMAT_XBGR16161616:
        case DRM_FORMAT_ARGB16161616:
        case DRM_FORMAT_ABGR16161616: formatBPC = 16; break;

        default: formatBPC = 8; break;
    }

    return std::clamp<uint64_t>(formatBPC, minimum, maximum);
}
