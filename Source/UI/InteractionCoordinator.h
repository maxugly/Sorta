#ifndef AUDIOFILER_INTERACTIONCOORDINATOR_H
#define AUDIOFILER_INTERACTIONCOORDINATOR_H

#include "Core/AppEnums.h"
#include <juce_core/juce_core.h>

/**
 * @class InteractionCoordinator
 * @brief Manages transient UI interaction states that are not part of the global SessionState.
 * 
 * This class coordinates states like "which marker is being zoomed" or "interaction mode 
 * overrides" to keep ControlPanel focused on layout and presenters focused on logic.
 */
class InteractionCoordinator
{
public:
    InteractionCoordinator() = default;
    ~InteractionCoordinator() = default;

    /** @brief Sets the currently active zoom point (In, Out, or None). */
    void setActiveZoomPoint(AppEnums::ActiveZoomPoint point) { m_activeZoomPoint = point; }

    /** @brief Returns the currently active zoom point. */
    AppEnums::ActiveZoomPoint getActiveZoomPoint() const { return m_activeZoomPoint; }

    /** @brief Sets a manual zoom point override (e.g. from mouse hover). */
    void setManualZoomPoint(AppEnums::ActiveZoomPoint point) { m_manualZoomPoint = point; }

    /** @brief Returns the manual zoom point override. */
    AppEnums::ActiveZoomPoint getManualZoomPoint() const { return m_manualZoomPoint; }

private:
    AppEnums::ActiveZoomPoint m_activeZoomPoint = AppEnums::ActiveZoomPoint::None;
    AppEnums::ActiveZoomPoint m_manualZoomPoint = AppEnums::ActiveZoomPoint::None;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractionCoordinator)
};

#endif
