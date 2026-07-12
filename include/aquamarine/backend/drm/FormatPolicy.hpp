#pragma once

#include <cstdint>

namespace Aquamarine {
    bool     drmBufferFormatRequiresLegacyReconfigure(bool directScanoutBuffer, uint32_t outputFormat, uint32_t bufferFormat);
    uint32_t drmLinkFormat(bool directScanoutBuffer, uint32_t outputFormat, uint32_t bufferFormat);
    uint8_t  drmFormatBPC(uint64_t minimum, uint64_t maximum, uint32_t drmFormat);
}
