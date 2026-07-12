#include <aquamarine/output/Output.hpp>

#include "shared.hpp"

int main() {
    Aquamarine::COutputState state;
    int                      ret = 0;

    state.setContentType(DRM_MODE_CONTENT_TYPE_GAME);
    EXPECT(state.state().contentType, DRM_MODE_CONTENT_TYPE_GAME);
    EXPECT(!!(state.state().committed & Aquamarine::COutputState::AQ_OUTPUT_STATE_CONTENT_TYPE), true);

    return ret;
}
