#include "MainComponent.h"
MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    player1.onMuteChanged = [this] { updateMute(); };
    player2.onMuteChanged = [this] { updateMute(); };

    // mix toggle
    addAndMakeVisible(mixToggle);
    mixToggle.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    mixToggle.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mixToggle.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    mixToggle.setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightgrey);
    mixToggle.setClickingTogglesState(true);

    mixToggle.onClick = [this]() {
        mixEnabled = mixToggle.getToggleState();

        if (mixEnabled) {
            mixToggle.setButtonText("Mix (both tracks): ON");
        } else {
            mixToggle.setButtonText("Mix (both tracks): OFF");
        }
    };
    // Crossfade slider setup
    addAndMakeVisible(crossfadeSlider);
    crossfadeSlider.setRange(0.0, 1.0, 0.01);
    crossfadeSlider.setValue(0.5); // Start in the middle
    crossfadeSlider.setSliderStyle(juce::Slider::LinearVertical);
    crossfadeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    crossfadeSlider.onValueChange = [this]() {
        // Update gains based on crossfade position
        float crossfadeValue = (float)crossfadeSlider.getValue();

        // When slider is at top (1.0): player1 full volume, player2 silent
        // When slider is at bottom (0.0): player1 silent, player2 full volume
        float player1Gain = crossfadeValue;
        float player2Gain = 1.0f - crossfadeValue;

        playerAudio1.setGain(player1Gain);
        playerAudio2.setGain(player2Gain);
    };

    addAndMakeVisible(crossfadeLabel);
    crossfadeLabel.setText("Crossfade", juce::dontSendNotification);
    crossfadeLabel.setJustificationType(juce::Justification::centred);
    crossfadeLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    setSize(1000, 700);
    setAudioChannels(0, 2);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
}
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixer.removeAllInputs();
    mixer.addInputSource(playerAudio1.getAudioSource(), false);
    mixer.addInputSource(playerAudio2.getAudioSource(), false);

    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    if (mixEnabled)
    {
        playerAudio1.updatePlaybackLogic();
        playerAudio2.updatePlaybackLogic();
        // mixer returns combined audio of both players
        mixer.getNextAudioBlock(bufferToFill);
    }
    else
    {
        player2.getNextAudioBlock(bufferToFill);

    }
}
void MainComponent::releaseResources()
{
    mixer.releaseResources();
    player1.releaseResources();
    player2.releaseResources();
}
void MainComponent::resized() {
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(30);
    mixToggle.setBounds(top.removeFromLeft(200).reduced(4));

    // Crossfade slider on the right side
    auto crossfadeArea = area.removeFromRight(60);
    crossfadeLabel.setBounds(crossfadeArea.removeFromTop(20));
    crossfadeSlider.setBounds(crossfadeArea.reduced(5));

    // split remaining vertically between players
    auto half = area.removeFromTop(area.getHeight() / 2);
    player1.setBounds(half);
    player2.setBounds(area);
}
void MainComponent::updateMute()
{
    bool player1Muted = player1.isMuted();
    bool player2Muted = player2.isMuted();

    playerAudio1.setMuted(player1Muted);
    playerAudio2.setMuted(player2Muted);
    repaint();
}
