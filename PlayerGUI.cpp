#include "PlayerGUI.h"
PlayerGUI::PlayerGUI()
{
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
  
	addAndMakeVisible(playButton);
    playButton.addListener(this);

    addAndMakeVisible(pauseButton);
    pauseButton.addListener(this);

    addAndMakeVisible(restartButton);
    restartButton.addListener(this);

    addAndMakeVisible(stopButton);
    stopButton.addListener(this);

    addAndMakeVisible(muteButton);
    muteButton.addListener(this);
	
	addAndMakeVisible(repeatButton);
    repeatButton.addListener(this);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::resized()
{
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    fb.items.add(juce::FlexItem(loadButton).withMinWidth(80.0f).withMinHeight(40.0f));
	fb.items.add(juce::FlexItem(playButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(pauseButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(restartButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(stopButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(muteButton).withMinWidth(80.0f).withMinHeight(40.0f));
	fb.items.add(juce::FlexItem(repeatButton).withMinWidth(80.0f).withMinHeight(40.0f));

    fb.performLayout(getLocalBounds().reduced(20, 20).removeFromTop(50));
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    playerAudio.loadFile(file);
            });

    }
	
    if (button == &playButton)
    {
    playerAudio.play();
    }
	
    if (button == &pauseButton)
    {
    playerAudio.pause();
    }

    if (button == &restartButton)
    {
        playerAudio.play();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }
	if (button == &repeatButton)
	{
		static bool isRepeating = false;
		isRepeating = !isRepeating;
		playerAudio.setRepeat(isRepeating);
		if (isRepeating)
            repeatButton.setButtonText("Repeat: ON");
		else
			repeatButton.setButtonText("Repeat: OFF");
	}
	if (button == &muteButton){
        muted = !muted;
        muteButton.setMuted(muted);
        playerAudio.setMuted(muted);
        if (onMuteChanged) onMuteChanged();
    }
}
void PlayerGUI::toggleMute()
{
    muted = !muted;                        // flip state
    if (onMuteChanged) onMuteChanged();    // notify MainComponent
    repaint();
}
void PlayerGUI::paint(juce::Graphics& g)
{
    // optional: draw background/text
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)volumeSlider.getValue());
}
