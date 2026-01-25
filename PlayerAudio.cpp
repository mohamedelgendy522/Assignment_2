#include "PlayerGUI.h"

// Structure to store media metadata for the audio file
struct Metadata {
    juce::String title, artist, album, year, duration;
};

// Function to read media metadata from an audio file
static Metadata readMetadata(const juce::File& file)
{
    Metadata meta;

    // Initialize format manager to recognize audio file types
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    // Create a reader for the audio file
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        // Read metadata from the file
        juce::StringPairArray metadata = reader->metadataValues;

        // Extract various data with default values if not present
        meta.title = metadata.getValue("title", file.getFileNameWithoutExtension());
        meta.artist = metadata.getValue("artist", "Unknown Artist");
        meta.album = metadata.getValue("album", "Unknown Album");
        meta.year = metadata.getValue("year", "Unknown Year");

        // Calculate the duration of the audio file
        double seconds = reader->lengthInSamples / reader->sampleRate;
        int totalSeconds = static_cast<int>(seconds);
        int minutes = totalSeconds / 60;
        int secs = totalSeconds % 60;

        meta.duration = juce::String::formatted("%02d:%02d", minutes, secs);
    }
    else
    {
        // Use default values if the file cannot be read
        meta.title = file.getFileNameWithoutExtension();
        meta.artist = "Unknown Artist";
        meta.album = "Unknown Album";
        meta.year = "Unknown Year";
        meta.duration = "00:00";
    }

    // Ensure there is a title for the file
    if (meta.title.isEmpty())
        meta.title = file.getFileNameWithoutExtension();

    return meta;
}

// ========== Constructor for PlayerGUI ==========
// Initialize the UI for the audio player component
PlayerGUI::PlayerGUI(PlayerAudio& player)
    : playerAudio(player)
{
    thumbnail.addChangeListener(this); // To notify us when loading is finished

    // Initialize audio format manager
    formatManager.registerBasicFormats();

    // ========== Add control buttons ==========
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

    // ========== Initialize metadata label ==========
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setFont(juce::Font("Arial", 16.0f, juce::Font::bold));
    addAndMakeVisible(metadataLabel);

    // ========== Initialize Volume slider ==========
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // ========== Initialize Speed slider ==========
    speedSlider.setRange(0.1, 1.95, 0.01);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setValue(1);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    // ========== Initialize Progress slider ==========
    progressSlider.setRange(0, playerAudio.getLength());
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.addListener(this);
    addAndMakeVisible(progressSlider);
    startTimerHz(30); // Start timer to update UI 30 times per second

    // ========== Initialize time label ==========
    addAndMakeVisible(timeLabel);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    // ========== Initialize description labels for sliders ==========
    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(positionLabel);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);
    positionLabel.setText("Position", juce::dontSendNotification);

    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    positionLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    volumeLabel.attachToComponent(&volumeSlider, true);
    speedLabel.attachToComponent(&speedSlider, true);
    positionLabel.attachToComponent(&progressSlider, true);

    // ========== Initialize playlist ==========
    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setRowHeight(25);
}

// ========== Destructor ==========
PlayerGUI::~PlayerGUI() {}

// ========== Prepare to play audio ==========
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// ========== Get next audio block ==========
void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

// ========== Release resources ==========
void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

// ========== Resized function ==========
void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    // --- Header (Buttons) ---
    auto headerRow = area.removeFromTop(40);
    loadButton.setBounds(headerRow.removeFromLeft(50).reduced(2));
    AB_loopButton.setBounds(headerRow.removeFromRight(60).reduced(2));

    int numCenterButtons = 9;
    int btnWidth = headerRow.getWidth() / numCenterButtons;

    // Arrange buttons
    goToStartButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    backward10Button.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    playPauseButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    forward10Button.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    goToEndButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    restartButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    stopButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    repeatButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));
    muteButton.setBounds(headerRow.removeFromLeft(btnWidth).reduced(2));

    area.removeFromTop(5);

    // --- Controls (Sliders) ---
    auto controlsArea = area.removeFromTop(80);
    auto leftControls = controlsArea.removeFromLeft(controlsArea.getWidth() / 2).reduced(5, 0);
    auto rightControls = controlsArea;

    // Volume
    auto volRow = leftControls.removeFromTop(30);
    volumeLabel.setBounds(volRow.removeFromLeft(50));
    volumeSlider.setBounds(volRow);

    // Speed
    auto speedRow = leftControls.removeFromTop(30);
    speedLabel.setBounds(speedRow.removeFromLeft(50));
    speedSlider.setBounds(speedRow);

    // Position
    positionLabel.setBounds(rightControls.removeFromLeft(60));
    progressSlider.setBounds(rightControls.removeFromTop(30));
    timeLabel.setBounds(rightControls);

    area.removeFromTop(5);

    // --- Metadata Area (New Area) ---
    // Here we place the song title instead of above the waveform
    metadataLabel.setBounds(area.removeFromTop(25));
    metadataLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    metadataLabel.setJustificationType(juce::Justification::centred);

    area.removeFromTop(5);

    // --- Waveform ---
    // Reserve space for drawing (we will use the same calculation in paint)
    // removeFromTop here just to reserve space and push the Playlist down
    area.removeFromTop(70);

    area.removeFromTop(5);

    // --- Playlist ---
    playlistBox.setBounds(area);
}
// ========== Button click event handler ==========
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // Open file chooser window
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                juce::Array<juce::File> files = fc.getResults();
                for (auto& file : files)
                {
                    if (file.existsAsFile() && !playlistFiles.contains(file))
                        playlistFiles.add(file);
                }

                playlistBox.updateContent();

                if (playlistFiles.size() > 0)
                    playlistBox.selectRow(0);
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

        // 1. Update playback logic
        playerAudio.setRepeat(isRepeating);

        // 2. Update button appearance (this was the missing line)
        repeatButton.setRepeating(isRepeating);

        // (Note: removed setButtonText lines because we rely on drawing now, not text)
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

// ========== Toggle mute state ==========
void PlayerGUI::toggleMute()
{
    muted = !muted; // flip state
    if (onMuteChanged) onMuteChanged(); // notify MainComponent
    repaint();
}

// ========== Slider value change event handler ==========
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    // ========== Volume Slider ==========
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)volumeSlider.getValue());

    }
    // ========== Speed Slider ==========
    if (slider == &speedSlider)
    {
        playerAudio.setSpeed((float)speedSlider.getValue());

    }
    // ========== Progress Slider ==========
    if (slider == &progressSlider)
        playerAudio.setPosition((float)progressSlider.getValue());

}

// ========== Get number of rows in playlist ==========
int PlayerGUI::getNumRows() { return playlistFiles.size(); }

// ========== Paint playlist item ==========
void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        // Selection color (Cyan) with transparency to avoid being too bright
        g.fillAll(juce::Colour(0xFF00E5FF).withAlpha(0.2f));
        g.setColour(juce::Colour(0xFF00E5FF)); // Line on the left to indicate selection
        g.fillRect(0, 0, 4, height);
    }
    else
    {
        // Normal background color (same as Container)
        g.fillAll(juce::Colour(0xFF1E1E1E));
    }

    if (rowNumber >= 0 && rowNumber < playlistFiles.size())
    {
        // Text color
        g.setColour(rowIsSelected ? juce::Colour(0xFF00E5FF) : juce::Colours::white);
        g.setFont(14.0f);
        // Draw text with padding to avoid sticking to edge
        g.drawText(playlistFiles[rowNumber].getFileNameWithoutExtension(),
                   10, 0, width - 10, height,
                   juce::Justification::centredLeft);
    }

    // Draw a very light separator line between items
    g.setColour(juce::Colours::grey.withAlpha(0.2f));
    g.drawLine(0, height, width, height, 1.0f);
}

// ========== Function to convert time from seconds to MM:SS format ==========
static juce::String formatTime(double seconds)
{
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    return juce::String::formatted("%02d:%02d", mins, secs);
}

// ========== Timer callback for continuous UI update ==========
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

        // Very important: Repaint screen so waveform cursor moves
        repaint();
    }
}

// ========== Playlist selected row change event handler ==========
void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistFiles.size())
    {
        juce::File file = playlistFiles[lastRowSelected];
        playerAudio.loadFile(file);
        thumbnail.setSource(new juce::FileInputSource(file));

        auto meta = readMetadata(file);

        // --- Modification: Display information in one or two lines only ---
        juce::String title = meta.title.isNotEmpty() ? meta.title : file.getFileNameWithoutExtension();
        juce::String artist = meta.artist.isNotEmpty() ? meta.artist : "";

        // Display: Title - Artist (Duration)
        juce::String info = title;
        if (artist.isNotEmpty()) info += " - " + artist;
        info += "  (" + meta.duration + ")";

        metadataLabel.setText(info, juce::dontSendNotification);
    }
}

// ========== Main paint function ==========
void PlayerGUI::paint(juce::Graphics& g)
{
    // Manually calculate waveform position to be below metadata
    // (Buttons 40 + gap 5 + Controls 80 + gap 5 + Meta 25 + gap 5) = 160
    int waveformY = 160;
    int waveformHeight = 70;

    auto bounds = getLocalBounds().reduced(10);
    auto waveformArea = bounds;
    waveformArea.setY(waveformY);
    waveformArea.setHeight(waveformHeight);

    // Background
    g.setColour(juce::Colour(0xFF0F0F0F));
    g.fillRoundedRectangle(waveformArea.toFloat(), 6.0f);
    g.setColour(juce::Colour(0xFF333333));
    g.drawRoundedRectangle(waveformArea.toFloat(), 6.0f, 1.5f);

    if (thumbnail.getNumChannels() > 0)
    {
        g.setColour(juce::Colour(0xFF00E5FF));

        // Draw channels (Stereo looks good, keep it as is but in wider space)
        thumbnail.drawChannels(g, waveformArea.reduced(2), 0.0, thumbnail.getTotalLength(), 1.0f);

        // Draw Playhead
        double progress = playerAudio.getPosition() / playerAudio.getLength();
        // Ensure progress is within normal bounds
        if (progress >= 0.0 && progress <= 1.0)
        {
            float x = waveformArea.getX() + (float)(waveformArea.getWidth() * progress);

            g.setColour(juce::Colours::white);
            // Draw vertical line
            g.drawLine(x, (float)waveformArea.getY(), x, (float)waveformArea.getBottom(), 2.0f);

            // Draw small triangle on top
            juce::Path p;
            p.addTriangle(x - 5, (float)waveformArea.getY(), x + 5, (float)waveformArea.getY(), x, (float)waveformArea.getY() + 6);
            g.fillPath(p);
        }
    }
    else
    {
        g.setColour(juce::Colours::grey);
        g.setFont(16.0f);
        g.drawText("NO TRACK LOADED", waveformArea, juce::Justification::centred);
    }
}
void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail)
        repaint(); // When drawing is ready, repaint screen
}
