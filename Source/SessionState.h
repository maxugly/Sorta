#pragma once

#include "MainDomain.h"

class SessionState {
public:
    SessionState();

    MainDomain::CutPreferences cutPrefs;
    bool isLooping;
    bool autoplay;
};
