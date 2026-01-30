# DrunkDrawnLookAndFeel Settings Guide

## Quick Start

```cpp
DrunkDrawnLookAndFeel customLF;

// Apply to your components
setLookAndFeel(&customLF);

// Don't forget to clean up in destructor
setLookAndFeel(nullptr);
```

## Color Customization

Each color range has a base color and variation parameters:

```cpp
DrunkDrawnLookAndFeel::ColorRange range = {
    baseColor,      // The base juce::Colour
    hueVariation,   // ±variation in hue (0.0 - 1.0), typical: 0.05 - 0.15
    satVariation,   // ±variation in saturation (0.0 - 1.0), typical: 0.1 - 0.2
    brightVariation // ±variation in brightness (0.0 - 1.0), typical: 0.1 - 0.2
};
```

### Available Color Ranges:

1. **Button Off (Unpressed/Untoggled)**
```cpp
customLF.setButtonOffColorRange({
    juce::Colour(0xff3a3a3a),  // Dark grey
    0.05f, 0.1f, 0.15f
});
```

2. **Button On (Toggled)**
```cpp
customLF.setButtonOnColorRange({
    juce::Colour(0xffff1493),  // Hot pink
    0.08f, 0.12f, 0.1f
});
```

3. **Button Outline**
```cpp
customLF.setButtonOutlineColorRange({
    juce::Colour(0xff00ffff),  // Cyan
    0.1f, 0.15f, 0.15f
});
```

4. **Text Color**
```cpp
customLF.setTextColorRange({
    juce::Colours::white,
    0.0f, 0.0f, 0.08f  // Keep hue/sat low for white text
});
```

5. **Text Box Background**
```cpp
customLF.setTextBoxColorRange({
    juce::Colour(0xff2a2a2a),
    0.05f, 0.1f, 0.15f
});
```

## Wobble Settings

Control how "drunk" the drawing looks:

```cpp
DrunkDrawnLookAndFeel::WobbleSettings wobble;
wobble.edgeWobble = 4.0f;              // Pixel wobble on edges (0-10)
wobble.cornerRadiusVariation = 0.4f;   // Corner variation (0.0-1.0)
wobble.controlPointVariation = 0.6f;   // Bezier drunkness (0.0-1.0)
wobble.segmentsPerSide = 10;           // Detail level (4-20)
wobble.strokeWidthVariation = 0.3f;    // Line thickness variation (0.0-0.5)
wobble.rotationVariation = 2.0f;       // Max rotation in degrees (0-5)

customLF.setWobbleSettings(wobble);
```

### Wobble Parameter Guide:

- **edgeWobble**: Higher = more wavy edges
  - Subtle: 2.0f
  - Moderate: 4.0f
  - Drunk: 6.0f+

- **cornerRadiusVariation**: How different each corner is
  - Uniform: 0.0f
  - Slight: 0.2f
  - Wild: 0.5f+

- **controlPointVariation**: How curved the wobbles are
  - Gentle: 0.3f
  - Moderate: 0.5f
  - Chaotic: 0.8f+

- **segmentsPerSide**: More segments = more detailed wobble
  - Simple: 4-6
  - Standard: 8-12
  - Detailed: 15-20 (may impact performance)

- **strokeWidthVariation**: Outline thickness variation
  - Consistent: 0.1f
  - Variable: 0.3f
  - Very variable: 0.5f+

- **rotationVariation**: Subtle tilt to buttons
  - None: 0.0f
  - Subtle: 1-2 degrees
  - Noticeable: 3-5 degrees

## Other Settings

```cpp
customLF.setBaseStrokeWidth(2.5f);     // Base outline thickness
customLF.setBaseCornerRadius(15.0f);   // Base corner roundness
```

## Color Presets

### Cyberpunk (matching your screenshots):
```cpp
customLF.setButtonOffColorRange({ juce::Colour(0xff3a3a3a), 0.05f, 0.1f, 0.15f });
customLF.setButtonOnColorRange({ juce::Colour(0xffff1493), 0.08f, 0.12f, 0.1f });
customLF.setButtonOutlineColorRange({ juce::Colour(0xff00ffff), 0.1f, 0.15f, 0.15f });
```

### Neon Green:
```cpp
customLF.setButtonOffColorRange({ juce::Colour(0xff2a2a2a), 0.05f, 0.1f, 0.15f });
customLF.setButtonOnColorRange({ juce::Colour(0xff39ff14), 0.08f, 0.15f, 0.1f });
customLF.setButtonOutlineColorRange({ juce::Colour(0xff00ff88), 0.12f, 0.2f, 0.15f });
```

### Purple Haze:
```cpp
customLF.setButtonOffColorRange({ juce::Colour(0xff2d2d3d), 0.08f, 0.12f, 0.15f });
customLF.setButtonOnColorRange({ juce::Colour(0xff9d4edd), 0.1f, 0.15f, 0.12f });
customLF.setButtonOutlineColorRange({ juce::Colour(0xffc77dff), 0.12f, 0.18f, 0.15f });
```

### Radioactive:
```cpp
customLF.setButtonOffColorRange({ juce::Colour(0xff1a1a1a), 0.05f, 0.1f, 0.15f });
customLF.setButtonOnColorRange({ juce::Colour(0xffccff00), 0.1f, 0.15f, 0.12f });
customLF.setButtonOutlineColorRange({ juce::Colour(0xff88ff00), 0.15f, 0.2f, 0.15f });
```

## Tips

1. **Consistent Randomization**: Each button gets a unique seed based on its name and memory address, so it looks consistent across redraws but different from other buttons.

2. **Toggle State**: Toggle buttons automatically use `buttonOnColorRange` when toggled on.

3. **Button Names**: Set unique names for your buttons to ensure consistent randomization:
   ```cpp
   myButton.setName("playButton");
   ```

4. **Performance**: More segments = more detailed wobble but slightly slower rendering. 8-12 segments is a good balance.

5. **Experimenting**: Start with moderate settings and adjust incrementally. Small changes in wobble can have big visual impacts!

## Integration Example

```cpp
class MyAudioComponent : public juce::Component
{
public:
    MyAudioComponent()
    {
        // Setup look and feel
        customLF.setButtonOnColorRange({ juce::Colour(0xffff1493), 0.08f, 0.12f, 0.1f });
        
        // Setup buttons
        playButton.setName("playBtn");
        playButton.setClickingTogglesState(true);
        addAndMakeVisible(playButton);
        
        // Apply to component
        setLookAndFeel(&customLF);
    }
    
    ~MyAudioComponent() override
    {
        setLookAndFeel(nullptr);  // Important!
    }
    
private:
    DrunkDrawnLookAndFeel customLF;
    juce::TextButton playButton;
};
```

## Troubleshooting

**Buttons look too uniform?**
- Increase `edgeWobble` and `controlPointVariation`
- Increase color variations in ColorRange structs

**Buttons too chaotic?**
- Decrease `edgeWobble` and `rotationVariation`
- Reduce `segmentsPerSide` to 6-8
- Lower `cornerRadiusVariation`

**Colors not visible enough?**
- Increase brightness variation
- Adjust base colors to have more contrast
- Check that alpha channel is 0xff (fully opaque)

**Performance issues?**
- Reduce `segmentsPerSide` (8 is usually plenty)
- Simplify color ranges (won't impact performance much though)
