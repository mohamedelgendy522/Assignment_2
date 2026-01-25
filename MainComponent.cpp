#include "MainComponent.h"

MainComponent::MainComponent()
{
    setLookAndFeel(&modernLook);

    playerAudio2.loadLastSession();
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    player1.onMuteChanged = [this] { updateMute(); };
    player2.onMuteChanged = [this] { updateMute(); };

    // mix toggle
    addAndMakeVisible(mixToggle);
    mixToggle.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2B2B2B)); // Dark gray
    mixToggle.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF00E5FF)); // Cyan when active
    mixToggle.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    mixToggle.setColour(juce::TextButton::textColourOnId, juce::Colours::black); // Black text when active
    mixToggle.setClickingTogglesState(true);

    mixToggle.onClick = [this]() {
        mixEnabled = mixToggle.getToggleState();
        mixToggle.setButtonText(mixEnabled ? "Mix (both tracks): ON" : "Mix (both tracks): OFF");
    };

    // Crossfade slider setup
    addAndMakeVisible(crossfadeSlider);
    crossfadeSlider.setRange(0.0, 1.0, 0.01);
    crossfadeSlider.setValue(0.5); // Start in the middle
    crossfadeSlider.setSliderStyle(juce::Slider::LinearVertical);
    crossfadeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    crossfadeSlider.onValueChange = [this]() {
        updateCrossfade();
    };

    addAndMakeVisible(crossfadeLabel);
    crossfadeLabel.setText("Crossfade", juce::dontSendNotification);
    crossfadeLabel.setJustificationType(juce::Justification::centred);
    crossfadeLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    setSize(1200, 700); // Increase width to fit the new layout
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    playerAudio2.saveLastSession();
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

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
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
    auto area = getLocalBounds().reduced(15);

    // Mix button at the top
    auto top = area.removeFromTop(30);
    mixToggle.setBounds(top.removeFromLeft(200).reduced(4));

    // Split the remaining space into three horizontal parts
    // player1 | crossfade | player2
    int crossfadeWidth = 60;  // Crossfade strip width
    int playerWidth = (area.getWidth() - crossfadeWidth) / 2;

    // player1 on the left
    auto player1Area = area.removeFromLeft(playerWidth);
    player1.setBounds(player1Area.reduced(5));

    // crossfade in the middle
    auto crossfadeArea = area.removeFromLeft(crossfadeWidth);

    // Solution for the cut-off text problem:
    // 1. Increase the allocated height for the Label
    crossfadeLabel.setBounds(crossfadeArea.removeFromTop(25));
    // 2. Slightly reduce font size
    crossfadeLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    crossfadeLabel.setJustificationType(juce::Justification::centred); // Center

    crossfadeSlider.setBounds(crossfadeArea.reduced(5));

    // player2 on the right
    player2.setBounds(area.reduced(5));
}

void MainComponent::updateMute()
{
    bool player1Muted = player1.isMuted();
    bool player2Muted = player2.isMuted();

    playerAudio1.setMuted(player1Muted);
    playerAudio2.setMuted(player2Muted);
    repaint();
}

void MainComponent::paint(juce::Graphics& g)
{
    // 1. Very dark background for the application
    g.fillAll(juce::Colour(0xFF121212));

    auto area = getLocalBounds().reduced(10);
    auto topArea = area.removeFromTop(50); // Title area

    // 2. Draw the title nicely
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("DJ STUDIO PRO", topArea, juce::Justification::centred);

    // 3. Draw backgrounds for playback areas (Decks)
    // We need to calculate areas using the same logic as resized
    auto contentArea = area;
    int crossfadeWidth = 60;
    int playerWidth = (contentArea.getWidth() - crossfadeWidth) / 2;

    auto leftDeck = contentArea.removeFromLeft(playerWidth).reduced(5);
    auto crossfadeDeck = contentArea.removeFromLeft(crossfadeWidth); // The middle
    auto rightDeck = contentArea.reduced(5);

    // Draw left Deck background
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRoundedRectangle(leftDeck.toFloat(), 15.0f);
    g.setColour(juce::Colour(0xFF333333)); // Light borders
    g.drawRoundedRectangle(leftDeck.toFloat(), 15.0f, 1.5f);

    // Draw right Deck background
    g.setColour(juce::Colour(0xFF1E1E1E));
    g.fillRoundedRectangle(rightDeck.toFloat(), 15.0f);
    g.setColour(juce::Colour(0xFF333333));
    g.drawRoundedRectangle(rightDeck.toFloat(), 15.0f, 1.5f);
}

void MainComponent::updateCrossfade()
{
    float crossfadeValue = (float)crossfadeSlider.getValue();

    // player1 starts from left (when crossfade = 1.0)
    // player2 starts from right (when crossfade = 0.0)
    float player1Gain = crossfadeValue;
    float player2Gain = 1.0f - crossfadeValue;

    playerAudio1.setGain(player1Gain);
    playerAudio2.setGain(player2Gain);

    // Update layout if necessary
    repaint();
}
