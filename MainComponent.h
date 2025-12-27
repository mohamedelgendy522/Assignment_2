#pragma once
#include <JuceHeader.h>
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent {
public:
    MainComponent();
    ~MainComponent() override;
    void updateMute();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;
    void paint(juce::Graphics& g) override;



private:
    void updateMixState();
    void updateCrossfade();
    bool isMuted = false;

    PlayerAudio playerAudio1;
    PlayerAudio playerAudio2;

    PlayerGUI player1{ playerAudio1 };
    PlayerGUI player2{ playerAudio2 };

    // mixer and UI control
    juce::MixerAudioSource mixer;
    juce::TextButton mixToggle{ "Mix (both tracks)" };
    bool mixEnabled{ false };

    // Crossfade slider
    juce::Slider crossfadeSlider;
    juce::Label crossfadeLabel;
    juce::Label leftLabel, rightLabel;

    //void loadTrack(const juce::File& file);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
