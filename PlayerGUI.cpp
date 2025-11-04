#include "PlayerGUI.h"
#include <cstring>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

struct Metadata {
    juce::String title, artist, album, year, duration;
};

static Metadata readMetadata(const juce::File& file)
{
    Metadata meta;

    TagLib::FileRef f(file.getFullPathName().toRawUTF8());

    if (!f.isNull() && f.tag())
    {
        auto* tag = f.tag();
        meta.title = juce::String(tag->title().toCString(true));
        meta.artist = juce::String(tag->artist().toCString(true));
        meta.album = juce::String(tag->album().toCString(true));
        unsigned int year = tag->year();
        meta.year = (year != 0 ? juce::String(year) : "Unknown Year");
    }

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        double seconds = reader->lengthInSamples / reader->sampleRate;
        int totalSeconds = static_cast<int>(seconds);

        int minutes = totalSeconds / 60;
        int secs = totalSeconds % 60;

        meta.duration = juce::String::formatted("%02d:%02d", minutes, secs);
    }

    if (meta.title.isEmpty())
        meta.title = file.getFileNameWithoutExtension();

    return meta;
}

PlayerGUI::PlayerGUI(PlayerAudio& player)
    : playerAudio(player)
{
	formatManager.registerBasicFormats();
	
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    
	addAndMakeVisible(goToStartButton);
    goToStartButton.addListener(this);
  
	addAndMakeVisible(playButton);
    playButton.addListener(this);

    addAndMakeVisible(pauseButton);
    pauseButton.addListener(this);
	
    addAndMakeVisible(goToEndButton);
    goToEndButton.addListener(this);

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
	
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setFont(juce::Font("Arial",32.0f,juce::Font::bold));
    addAndMakeVisible(metadataLabel);
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
	fb.items.add(juce::FlexItem(goToStartButton).withMinWidth(80.0f).withMinHeight(40.0f));
	fb.items.add(juce::FlexItem(playButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(pauseButton).withMinWidth(80.0f).withMinHeight(40.0f));
	fb.items.add(juce::FlexItem(goToEndButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(restartButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(stopButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(muteButton).withMinWidth(80.0f).withMinHeight(40.0f));
	fb.items.add(juce::FlexItem(repeatButton).withMinWidth(80.0f).withMinHeight(40.0f));

    fb.performLayout(getLocalBounds().reduced(20, 20).removeFromTop(50));
    volumeSlider.setBounds(20, 240, getWidth() - 40, 30);
	metadataLabel.setBounds(20, getHeight() - , getWidth() - 20, 150);
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
            {
                playerAudio.loadFile(file);

                // Read metadata and show info    
				auto meta = readMetadata(file);

        juce::String info;
        info += "Title: " + (meta.title.isNotEmpty() ? meta.title : file.getFileNameWithoutExtension()) + "\n";
        info += "Artist: " + (meta.artist.isNotEmpty() ? meta.artist : "Unknown Artist") + "\n";
        info += "Album: " + (meta.album.isNotEmpty() ? meta.album : "Unknown Album") + "\n";
        info += "Year: " + (meta.year.isNotEmpty() ? meta.year : "Unknown Year") + "\n";
        info += "Duration: " + (meta.duration.isNotEmpty() ? meta.duration : "Unknown Duration");

        metadataLabel.setText(info, juce::dontSendNotification);
            }

        });
}

    if (button == &goToStartButton)
    {
    playerAudio.start();
    }
	
	if (button == &playButton)
    {
    playerAudio.play();
    }
	
    if (button == &pauseButton)
    {
    playerAudio.pause();
    }
		
	if (button == &goToEndButton)
    {
    playerAudio.end();
    }

    if (button == &restartButton)
    {
        playerAudio.restart();
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
