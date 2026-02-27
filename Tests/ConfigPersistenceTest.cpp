
#include <juce_core/juce_core.h>
#include "Utils/Config.h"

class ConfigPersistenceTest : public juce::UnitTest {
public:
    ConfigPersistenceTest() : juce::UnitTest("ConfigPersistence") {}

    void runTest() override {
        beginTest("Theme Persistence");

        // 1. Ensure we have a clean state or known state
        Config::initializeConfigs();
        
        juce::String testTheme = "test_theme_" + juce::String(juce::Random::getSystemRandom().nextInt()) + ".conf";
        
        // 2. Save a new theme name
        Config::saveCurrentTheme(testTheme);
        
        // 3. Re-initialize to see if it loads it back
        Config::Advanced::currentTheme = ""; // Clear it first
        Config::initializeConfigs();
        
        expectEquals(Config::Advanced::currentTheme, testTheme, "Persisted theme should match saved theme");
        
        // 4. Test robustness by manually corrupting the file
        auto settingsFile = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(".config/audiofiler/settings.conf");
        settingsFile.replaceWithText("invalid json { [ }");
        
        // Should not crash and should still be able to save
        Config::saveCurrentTheme("recovered.conf");
        expectEquals(Config::Advanced::currentTheme, juce::String("recovered.conf"));
        
        auto content = settingsFile.loadFileAsString();
        auto parsed = juce::JSON::parse(content);
        expect(parsed.isObject(), "Settings file should be a valid JSON object after recovery");
        expectEquals(parsed.getDynamicObject()->getProperty("currentTheme").toString(), juce::String("recovered.conf"));
    }
};

static ConfigPersistenceTest configPersistenceTest;
