#pragma once

namespace MainDomain {

struct CutPreferences {
    bool active{false};

    struct Auto {
        bool inActive{false};
        bool outActive{false};
        float thresholdIn{0.0f};
        float thresholdOut{0.0f};
    } autoCut;
};

} // namespace MainDomain
