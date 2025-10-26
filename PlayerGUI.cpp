#include "PlayerGUI.h"
#include <cstring>

struct Metadata {
    juce::String title, artist, album, year, genre;
};

Metadata readMetadata(const juce::File& file)
{
    Metadata meta;
    juce::FileInputStream stream(file);

    if (!stream.openedOk())
        return meta;

    char header[10];
    if (stream.read(header, 10) != 10)
        return meta;

    if (std::strncmp(header, "ID3", 3) != 0)
        return meta; 

    int tagSize =
        ((header[6] & 0x7F) << 21) |
        ((header[7] & 0x7F) << 14) |
        ((header[8] & 0x7F) << 7) |
        (header[9] & 0x7F);

    juce::MemoryBlock tagData;
    stream.readIntoMemoryBlock(tagData, tagSize);

    auto readFrame = [&](const char* id) -> juce::String
        {
            const char* data = (const char*)tagData.getData();
            const char* end = data + tagData.getSize();

            for (const char* p = data; p + 10 < end; )
            {
                juce::String frameID(juce::CharPointer_ASCII(p), 4);
                int frameSize = (p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7];

                if (frameSize <= 0 || p + 10 + frameSize > end)
                    break;

                if (frameID == id)
                {
                    const char* frameData = p + 10;
                    juce::String text(frameData + 1, frameSize - 1);
                    return text.trim();
                }

                p += 10 + frameSize;
            }

            return {};
        };

    meta.title = readFrame("TIT2");
    meta.artist = readFrame("TPE1");
    meta.album = readFrame("TALB");
    meta.year = readFrame("TYER");
    meta.genre = readFrame("TCON");

    return meta;
}

PlayerGUI::PlayerGUI()
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
    metadataLabel.setFont(juce::Font(32.0f));
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
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
	metadataLabel.setBounds(2, getHeight() - 185, getWidth() - 20, 150);
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

                std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
                if (reader)
                {
                    double durationSecs = reader->lengthInSamples / reader->sampleRate;
                    int totalSeconds = static_cast<int>(durationSecs);
                    int hours = totalSeconds / 3600;
                    int minutes = (totalSeconds % 3600) / 60;
                    int seconds = totalSeconds % 60;

                    info += juce::String::formatted("Duration: %02d:%02d:%02d", hours, minutes, seconds);
                }

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
