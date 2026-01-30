#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    Custom LookAndFeel for procedurally generated "drunk drawn" buttons
    with wobbly edges and randomized colors.
*/
class DrunkDrawnLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //==============================================================================
    struct ColorRange
    {
        juce::Colour baseColor;
        float hueVariation = 0.1f;      // ±variation in hue (0.0 - 1.0)
        float satVariation = 0.15f;     // ±variation in saturation
        float brightVariation = 0.15f;  // ±variation in brightness
        
        juce::Colour getRandomColor(juce::Random& rng) const
        {
            float h = baseColor.getHue() + rng.nextFloat() * hueVariation * 2.0f - hueVariation;
            float s = juce::jlimit(0.0f, 1.0f, baseColor.getSaturation() + rng.nextFloat() * satVariation * 2.0f - satVariation);
            float b = juce::jlimit(0.0f, 1.0f, baseColor.getBrightness() + rng.nextFloat() * brightVariation * 2.0f - brightVariation);
            
            // Wrap hue around
            while (h < 0.0f) h += 1.0f;
            while (h > 1.0f) h -= 1.0f;
            
            return juce::Colour::fromHSV(h, s, b, baseColor.getFloatAlpha());
        }
    };
    
    //==============================================================================
    struct WobbleSettings
    {
        float edgeWobble = 3.0f;           // How much edges wobble (pixels)
        float cornerRadiusVariation = 0.3f; // Variation in corner radius (0.0 - 1.0)
        float controlPointVariation = 0.4f; // How drunk the bezier curves are
        int segmentsPerSide = 8;            // More segments = more wobbly detail
        float strokeWidthVariation = 0.3f;  // Variation in stroke width
        float rotationVariation = 2.0f;     // Max rotation in degrees
    };
    
    //==============================================================================
    DrunkDrawnLookAndFeel()
    {
        // Default color schemes inspired by your screenshots
        buttonOffColorRange = { juce::Colour(0xff444444), 0.05f, 0.1f, 0.2f };
        buttonOnColorRange = { juce::Colour(0xffff1493), 0.08f, 0.15f, 0.15f }; // Hot pink
        buttonOutlineColorRange = { juce::Colour(0xff00ffff), 0.1f, 0.15f, 0.2f }; // Cyan
        textColorRange = { juce::Colours::white, 0.0f, 0.0f, 0.1f };
        textBoxColorRange = { juce::Colour(0xff2a2a2a), 0.05f, 0.1f, 0.15f };
        
        wobbleSettings.edgeWobble = 3.0f;
        wobbleSettings.cornerRadiusVariation = 0.35f;
        wobbleSettings.controlPointVariation = 0.5f;
        wobbleSettings.segmentsPerSide = 8;
        wobbleSettings.strokeWidthVariation = 0.25f;
        wobbleSettings.rotationVariation = 1.5f;
    }
    
    //==============================================================================
    // Setters for customization
    void setWobbleSettings(const WobbleSettings& settings) { wobbleSettings = settings; }
    void setButtonOffColorRange(const ColorRange& range) { buttonOffColorRange = range; }
    void setButtonOnColorRange(const ColorRange& range) { buttonOnColorRange = range; }
    void setButtonOutlineColorRange(const ColorRange& range) { buttonOutlineColorRange = range; }
    void setTextColorRange(const ColorRange& range) { textColorRange = range; }
    void setTextBoxColorRange(const ColorRange& range) { textBoxColorRange = range; }
    void setBaseStrokeWidth(float width) { baseStrokeWidth = width; }
    void setBaseCornerRadius(float radius) { baseCornerRadius = radius; }
    
    //==============================================================================
    void drawButtonBackground(juce::Graphics& g,
                            juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(4.0f);
        
        // Use button's component ID as seed for consistent randomization
        auto buttonId = button.getName().hashCode() + (int)(intptr_t)&button;
        juce::Random rng(buttonId);
        
        // Determine if button is toggled
        bool isToggled = button.getToggleState();
        
        // Select color range based on toggle state
        const ColorRange& fillRange = isToggled ? buttonOnColorRange : buttonOffColorRange;
        
        // Generate randomized colors
        juce::Colour fillColor = fillRange.getRandomColor(rng);
        juce::Colour outlineColor = buttonOutlineColorRange.getRandomColor(rng);
        
        // Adjust for button state
        if (shouldDrawButtonAsDown)
        {
            fillColor = fillColor.brighter(0.2f);
            outlineColor = outlineColor.brighter(0.2f);
        }
        else if (shouldDrawButtonAsHighlighted)
        {
            fillColor = fillColor.brighter(0.1f);
            outlineColor = outlineColor.brighter(0.1f);
        }
        
        // Generate wobbly path
        juce::Path wobblePath = createWobblyRoundedRectangle(bounds, rng);
        
        // Apply slight rotation
        float rotation = (rng.nextFloat() * 2.0f - 1.0f) * wobbleSettings.rotationVariation;
        auto center = bounds.getCentre();
        wobblePath.applyTransform(juce::AffineTransform::rotation(
            rotation * juce::MathConstants<float>::pi / 180.0f,
            center.x, center.y
        ));
        
        // Draw fill
        g.setColour(fillColor);
        g.fillPath(wobblePath);
        
        // Draw outline with variable width
        float strokeWidth = baseStrokeWidth + (rng.nextFloat() * 2.0f - 1.0f) * 
                           wobbleSettings.strokeWidthVariation * baseStrokeWidth;
        g.setColour(outlineColor);
        g.strokePath(wobblePath, juce::PathStrokeType(strokeWidth));
    }
    
    //==============================================================================
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto buttonId = button.getName().hashCode() + (int)(intptr_t)&button;
        juce::Random rng(buttonId + 12345); // Different seed for text color
        
        juce::Colour textColor = textColorRange.getRandomColor(rng);
        
        if (shouldDrawButtonAsDown)
            textColor = textColor.brighter(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            textColor = textColor.brighter(0.1f);
        
        g.setColour(textColor);
        g.setFont(button.getHeight() * 0.5f);
        
        g.drawText(button.getButtonText(),
                  button.getLocalBounds(),
                  juce::Justification::centred,
                  true);
    }
    
    //==============================================================================
    void drawTextEditorOutline(juce::Graphics& g, int width, int height,
                              juce::TextEditor& textEditor) override
    {
        auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height).reduced(2.0f);
        
        auto editorId = textEditor.getName().hashCode() + (int)(intptr_t)&textEditor;
        juce::Random rng(editorId);
        
        juce::Colour outlineColor = buttonOutlineColorRange.getRandomColor(rng);
        
        juce::Path wobblePath = createWobblyRoundedRectangle(bounds, rng);
        
        float strokeWidth = baseStrokeWidth + (rng.nextFloat() * 2.0f - 1.0f) * 
                           wobbleSettings.strokeWidthVariation * baseStrokeWidth;
        
        g.setColour(outlineColor);
        g.strokePath(wobblePath, juce::PathStrokeType(strokeWidth));
    }
    
    //==============================================================================
    void fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                 juce::TextEditor& textEditor) override
    {
        auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height).reduced(2.0f);
        
        auto editorId = textEditor.getName().hashCode() + (int)(intptr_t)&textEditor;
        juce::Random rng(editorId);
        
        juce::Colour bgColor = textBoxColorRange.getRandomColor(rng);
        
        juce::Path wobblePath = createWobblyRoundedRectangle(bounds, rng);
        
        g.setColour(bgColor);
        g.fillPath(wobblePath);
    }
    
    //==============================================================================
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        // Use same rendering as regular button
        drawButtonBackground(g, button, juce::Colours::transparentBlack,
                           shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

private:
    //==============================================================================
    juce::Path createWobblyRoundedRectangle(juce::Rectangle<float> bounds, juce::Random& rng)
    {
        juce::Path path;
        
        // Randomize corner radii
        float baseRadius = baseCornerRadius;
        float radiusVar = wobbleSettings.cornerRadiusVariation;
        
        std::array<float, 4> cornerRadii;
        for (int i = 0; i < 4; ++i)
        {
            cornerRadii[i] = baseRadius * (1.0f + (rng.nextFloat() * 2.0f - 1.0f) * radiusVar);
            cornerRadii[i] = juce::jmax(2.0f, cornerRadii[i]);
        }
        
        auto x = bounds.getX();
        auto y = bounds.getY();
        auto w = bounds.getWidth();
        auto h = bounds.getHeight();
        
        // Create wobbly path around the rectangle
        // We'll break each side into segments and add bezier curves
        
        int segs = wobbleSettings.segmentsPerSide;
        float wobble = wobbleSettings.edgeWobble;
        float cpVar = wobbleSettings.controlPointVariation;
        
        // Helper to add wobbly point
        auto addWobble = [&](float fx, float fy) -> juce::Point<float>
        {
            return juce::Point<float>(
                fx + (rng.nextFloat() * 2.0f - 1.0f) * wobble,
                fy + (rng.nextFloat() * 2.0f - 1.0f) * wobble
            );
        };
        
        // Start at top-left corner (after corner radius)
        auto startPoint = addWobble(x + cornerRadii[0], y);
        path.startNewSubPath(startPoint);
        
        // Top edge
        for (int i = 1; i < segs; ++i)
        {
            float t = (float)i / (float)segs;
            auto nextPoint = addWobble(x + cornerRadii[0] + t * (w - cornerRadii[0] - cornerRadii[1]), y);
            
            auto cp1 = addWobble(
                startPoint.x + (nextPoint.x - startPoint.x) * (0.33f + rng.nextFloat() * cpVar),
                startPoint.y + (rng.nextFloat() * 2.0f - 1.0f) * wobble
            );
            auto cp2 = addWobble(
                startPoint.x + (nextPoint.x - startPoint.x) * (0.67f + rng.nextFloat() * cpVar),
                nextPoint.y + (rng.nextFloat() * 2.0f - 1.0f) * wobble
            );
            
            path.cubicTo(cp1, cp2, nextPoint);
            startPoint = nextPoint;
        }
        
        // Top-right corner
        auto cornerPoint = addWobble(x + w, y + cornerRadii[1]);
        path.cubicTo(
            addWobble(startPoint.x + cornerRadii[1] * 0.5f, y),
            addWobble(x + w, y + cornerRadii[1] * 0.5f),
            cornerPoint
        );
        startPoint = cornerPoint;
        
        // Right edge
        for (int i = 1; i < segs; ++i)
        {
            float t = (float)i / (float)segs;
            auto nextPoint = addWobble(x + w, y + cornerRadii[1] + t * (h - cornerRadii[1] - cornerRadii[2]));
            
            auto cp1 = addWobble(
                startPoint.x + (rng.nextFloat() * 2.0f - 1.0f) * wobble,
                startPoint.y + (nextPoint.y - startPoint.y) * (0.33f + rng.nextFloat() * cpVar)
            );
            auto cp2 = addWobble(
                nextPoint.x + (rng.nextFloat() * 2.0f - 1.0f) * wobble,
                startPoint.y + (nextPoint.y - startPoint.y) * (0.67f + rng.nextFloat() * cpVar)
            );
            
            path.cubicTo(cp1, cp2, nextPoint);
            startPoint = nextPoint;
        }
        
        // Bottom-right corner
        cornerPoint = addWobble(x + w - cornerRadii[2], y + h);
        path.cubicTo(
            addWobble(x + w, startPoint.y + cornerRadii[2] * 0.5f),
            addWobble(x + w - cornerRadii[2] * 0.5f, y + h),
            cornerPoint
        );
        startPoint = cornerPoint;
        
        // Bottom edge
        for (int i = 1; i < segs; ++i)
        {
            float t = (float)i / (float)segs;
            auto nextPoint = addWobble(x + w - cornerRadii[2] - t * (w - cornerRadii[2] - cornerRadii[3]), y + h);
            
            auto cp1 = addWobble(
                startPoint.x - (startPoint.x - nextPoint.x) * (0.33f + rng.nextFloat() * cpVar),
                startPoint.y + (rng.nextFloat() * 2.0f - 1.0f) * wobble
            );
            auto cp2 = addWobble(
                startPoint.x - (startPoint.x - nextPoint.x) * (0.67f + rng.nextFloat() * cpVar),
                nextPoint.y + (rng.nextFloat() * 2.0f - 1.0f) * wobble
            );
            
            path.cubicTo(cp1, cp2, nextPoint);
            startPoint = nextPoint;
        }
        
        // Bottom-left corner
        cornerPoint = addWobble(x, y + h - cornerRadii[3]);
        path.cubicTo(
            addWobble(x + cornerRadii[3] * 0.5f, y + h),
            addWobble(x, y + h - cornerRadii[3] * 0.5f),
            cornerPoint
        );
        startPoint = cornerPoint;
        
        // Left edge
        for (int i = 1; i < segs; ++i)
        {
            float t = (float)i / (float)segs;
            auto nextPoint = addWobble(x, y + h - cornerRadii[3] - t * (h - cornerRadii[3] - cornerRadii[0]));
            
            auto cp1 = addWobble(
                startPoint.x + (rng.nextFloat() * 2.0f - 1.0f) * wobble,
                startPoint.y - (startPoint.y - nextPoint.y) * (0.33f + rng.nextFloat() * cpVar)
            );
            auto cp2 = addWobble(
                nextPoint.x + (rng.nextFloat() * 2.0f - 1.0f) * wobble,
                startPoint.y - (startPoint.y - nextPoint.y) * (0.67f + rng.nextFloat() * cpVar)
            );
            
            path.cubicTo(cp1, cp2, nextPoint);
            startPoint = nextPoint;
        }
        
        // Top-left corner (close the path)
        path.cubicTo(
            addWobble(x, y + cornerRadii[0] * 0.5f),
            addWobble(x + cornerRadii[0] * 0.5f, y),
            path.getCurrentPosition() // Close to start
        );
        
        path.closeSubPath();
        
        return path;
    }
    
    //==============================================================================
    ColorRange buttonOffColorRange;
    ColorRange buttonOnColorRange;
    ColorRange buttonOutlineColorRange;
    ColorRange textColorRange;
    ColorRange textBoxColorRange;
    
    WobbleSettings wobbleSettings;
    
    float baseStrokeWidth = 2.5f;
    float baseCornerRadius = 15.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrunkDrawnLookAndFeel)
};
