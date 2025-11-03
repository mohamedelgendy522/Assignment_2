#include "PlayerGUI.h"

struct Metadata {
    juce::String title, artist, album, year;
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

    addAndMakeVisible(playPauseButton);
    playPauseButton.addListener(this);

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

    addAndMakeVisible(AB_loopButton);
    AB_loopButton.addListener(this);

    addAndMakeVisible(forward10Button);
    forward10Button.addListener(this);

    addAndMakeVisible(backward10Button);
    backward10Button.addListener(this);

    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setFont(juce::Font("Arial", 32.0f, juce::Font::bold));
    addAndMakeVisible(metadataLabel);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.1, 1.95, 0.01);
    speedSlider.setValue(1);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    progressSlider.setRange(0, playerAudio.getLength());
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.addListener(this);
    addAndMakeVisible(progressSlider);
    startTimerHz(30);

    addAndMakeVisible(timeLabel);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(positionLabel);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);
    positionLabel.setText("Position", juce::dontSendNotification);

    speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    positionLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    volumeLabel.attachToComponent(&volumeSlider, true);
    speedLabel.attachToComponent(&speedSlider, true);
    positionLabel.attachToComponent(&progressSlider, true);

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

    // changing in width and height
    fb.items.add(juce::FlexItem(loadButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(goToStartButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(backward10Button).withMinWidth(60.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(playPauseButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(forward10Button).withMinWidth(60.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(goToEndButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(restartButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(stopButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(muteButton).withMinWidth(50.0f).withMinHeight(30.0f));
    fb.items.add(juce::FlexItem(repeatButton).withMinWidth(80.0f).withMinHeight(40.0f));
    fb.items.add(juce::FlexItem(AB_loopButton).withMinWidth(80.0f).withMinHeight(40.0f));


    fb.performLayout(getLocalBounds().reduced(20, 20).removeFromTop(50));

    metadataLabel.setBounds(5, getHeight() - 185, getWidth() - 20, 150);
    volumeSlider.setBounds(60, 100, getWidth() - 60, 30);
    speedSlider.setBounds(60, 80, getWidth() - 60, 30);
    progressSlider.setBounds(60, 120, getWidth() - 60, 30);
    timeLabel.setBounds(60, 135, getWidth() - 60, 30);

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
    else if (button == &goToStartButton)
    {
        playerAudio.start();
    }
    else if (button == &playPauseButton)
    {
        if (isPlaying)
        {
            playerAudio.pause();
            isPlaying = false;
            playPauseButton.setType(IconButton::Type::Play);
        }
        else
        {
            playerAudio.play();
            isPlaying = true;
            playPauseButton.setType(IconButton::Type::Pause);
        }
    }

    else if (button == &goToEndButton)
    {
        playerAudio.end();
    }
    else if (button == &restartButton)
    {
        playerAudio.restart();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &forward10Button)
    {
        double newPos = playerAudio.getPosition() + 10.0;
        if (newPos < playerAudio.getLength())
            playerAudio.setPosition(newPos);
        else
            playerAudio.setPosition(playerAudio.getLength());
    }
    else if (button == &backward10Button)
    {
        double newPos = playerAudio.getPosition() - 10.0;
        if (newPos > 0.0)
            playerAudio.setPosition(newPos);
        else
            playerAudio.setPosition(0.0);
    }
    else if (button == &AB_loopButton) {
        switch (State)
        {
        case 0:
            State = 1;
            AB_loopButton.setButtonText("Set A");
            break;

        case 1:
            playerAudio.setA(playerAudio.getPosition());
            State = 2;
            AB_loopButton.setButtonText("Set B");
            break;

        case 2:
            playerAudio.setB(playerAudio.getPosition());
            playerAudio.isOk(true);
            State = 3;
            AB_loopButton.setButtonText("AB Repeat ON");
            break;

        case 3:
            playerAudio.isOk(false);
            State = 0;
            AB_loopButton.setButtonText("AB Repeat");
        }
    }
    else if (button == &repeatButton) {
        isRepeating = !isRepeating;
        playerAudio.setRepeat(isRepeating);
        if (isRepeating)
            repeatButton.setButtonText("Repeat: ON");
        else
            repeatButton.setButtonText("Repeat: OFF");
    }
    else if (button == &muteButton) {
        muted = !muted;
        muteButton.setMuted(muted);
        playerAudio.setMuted(muted);
        if (!muted)
            playerAudio.setGain((float)volumeSlider.getValue());
        if (onMuteChanged) onMuteChanged();
    }
}
void PlayerGUI::toggleMute()
{
    muted = !muted; // flip state
    if (onMuteChanged) onMuteChanged(); // notify MainComponent
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
    if (slider == &speedSlider)
        playerAudio.setSpeed((float)speedSlider.getValue());
    if (slider == &progressSlider)
        playerAudio.setPosition((float)progressSlider.getValue());

}
static juce::String formatTime(double seconds)
{
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    return juce::String::formatted("%02d:%02d", mins, secs);
}
void PlayerGUI::timerCallback()
{
    double len = playerAudio.getLength();
    if (len > 0)
    {
        double pos = playerAudio.getPosition();
        progressSlider.setRange(0.0, len, juce::dontSendNotification);
        progressSlider.setValue(pos, juce::dontSendNotification);
        timeLabel.setText(
            formatTime((int)pos) + " / " + formatTime((int)len) + " s",
            juce::dontSendNotification
        );
    }
}
