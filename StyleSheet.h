// StyleSheet.h
#pragma once
#include <JuceHeader.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        // General color settings
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00E5FF)); // Slider thumb color (Cyan)
        setColour(juce::Slider::trackColourId, juce::Colour(0xFF404040)); // Slider background track color
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1E1E1E));

        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2B2B2B));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    // Draw slider in a modern style (Refined Linear Slider)
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider) override
{
    bool isVertical = slider.isVertical();

    if (isVertical) // Fix for Crossfade
    {
        // Make thickness constant and thin regardless of area width
        float trackThickness = 6.0f;
        float thumbSize = 18.0f; // Circle size

        float centerX = x + width * 0.5f;

        // Draw track (background line)
        juce::Rectangle<float> trackBounds(centerX - trackThickness * 0.5f,
                                           (float)y + 5,
                                           trackThickness,
                                           (float)height - 10);

        g.setColour(juce::Colour(0xFF2B2B2B)); // Dark color for track
        g.fillRoundedRectangle(trackBounds, trackThickness * 0.5f);

        // Draw thumb (circle)
        g.setColour(juce::Colour(0xFF00E5FF)); // Cyan color
        g.fillEllipse(centerX - thumbSize * 0.5f, sliderPos - thumbSize * 0.5f, thumbSize, thumbSize);

        // Draw simple glow
        g.setColour(juce::Colour(0xFF00E5FF).withAlpha(0.3f));
        g.drawEllipse(centerX - thumbSize * 0.5f, sliderPos - thumbSize * 0.5f, thumbSize, thumbSize, 3.0f);
    }
    else // Horizontal sliders (Volume, Speed)
    {
        float trackThickness = 6.0f;
        float thumbSize = 16.0f;
        float centerY = y + height * 0.5f;

        // Draw track
        juce::Rectangle<float> trackBounds((float)x, centerY - trackThickness * 0.5f, (float)width, trackThickness);
        g.setColour(juce::Colour(0xFF2B2B2B));
        g.fillRoundedRectangle(trackBounds, trackThickness * 0.5f);

        // Draw colored part (Progress)
        juce::Rectangle<float> fillBounds((float)x, centerY - trackThickness * 0.5f, sliderPos - x, trackThickness);
        g.setColour(juce::Colour(0xFF00E5FF).withAlpha(0.7f));
        g.fillRoundedRectangle(fillBounds, trackThickness * 0.5f);

        // Draw thumb
        g.setColour(juce::Colours::white);
        g.fillEllipse(sliderPos - thumbSize * 0.5f, centerY - thumbSize * 0.5f, thumbSize, thumbSize);
    }
}
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

        // Make corners more rounded (capsule shape)
        float cornerSize = 6.0f;

        // Determine background color
        auto baseColour = backgroundColour;
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f); // Slight brightening on press

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        // Add light colored border
        // If button is Mix Toggle (big button on top) make it distinctive
        if (button.getName().contains("Mix"))
        {
            // Exception for mix button to keep it looking different if desired
            g.setColour(juce::Colours::white.withAlpha(0.2f));
        }
        else
        {
            // Load and Loop buttons take light Cyan borders on mouse hover
            if (shouldDrawButtonAsHighlighted)
                g.setColour(juce::Colour(0xFF00E5FF).withAlpha(0.8f));
            else
                g.setColour(juce::Colours::white.withAlpha(0.1f)); // Very faint borders normally
        }

        g.drawRoundedRectangle(bounds, cornerSize, 1.5f);
    }

    // Don't forget to add function to draw text smaller and nicer
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        juce::Font font(13.0f, juce::Font::bold); // Font slightly smaller (was 14)
        g.setFont(font);

        // Text color
        g.setColour(button.findColour(juce::TextButton::textColourOffId)
                          .withAlpha(button.isEnabled() ? 1.0f : 0.5f));

        g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
    }
    // You can add more here to customize buttons and menus
};
