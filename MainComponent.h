#pragma once

#include <JuceHeader.h>
#include "PlayerGUI.h"


class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;

private:
    PlayerGUI player1;

    //void loadTrack(const juce::File& file);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
