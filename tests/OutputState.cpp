#include <aquamarine/output/Output.hpp>

#include "shared.hpp"

int main() {
    Aquamarine::COutputState state;
    int                      ret = 0;

    state.setContentType(DRM_MODE_CONTENT_TYPE_GAME);
    EXPECT(state.state().contentType, DRM_MODE_CONTENT_TYPE_GAME);
    EXPECT(!!(state.state().committed & Aquamarine::COutputState::AQ_OUTPUT_STATE_CONTENT_TYPE), true);

    state.setScanoutBuffer(nullptr);
    EXPECT(state.state().directScanoutBuffer, true);
    EXPECT(!!(state.state().committed & Aquamarine::COutputState::AQ_OUTPUT_STATE_BUFFER), true);
    EXPECT(!!(state.state().committed & Aquamarine::COutputState::AQ_OUTPUT_STATE_FORMAT), false);
    EXPECT(state.state().drmFormat, DRM_FORMAT_INVALID);

    state.setBuffer(nullptr);
    EXPECT(state.state().directScanoutBuffer, false);

    return ret;
}
