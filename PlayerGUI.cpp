#include "PlayerGUI.h"

// هيكل لتخزين بيانات الوسائط (ميتاداتا) للملف الصوتي
struct Metadata {
    juce::String title, artist, album, year, duration;
};

// دالة لقراءة بيانات الوسائط من ملف صوتي
static Metadata readMetadata(const juce::File& file)
{
    Metadata meta;

    // تهيئة مدير التنسيقات للتعرف على أنواع الملفات الصوتية
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    // إنشاء قارئ للملف الصوتي
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        // قراءة بيانات الوسائط من الملف
        juce::StringPairArray metadata = reader->metadataValues;

        // استخراج البيانات المختلفة مع قيم افتراضية إذا لم تكن موجودة
        meta.title = metadata.getValue("title", file.getFileNameWithoutExtension());
        meta.artist = metadata.getValue("artist", "Unknown Artist");
        meta.album = metadata.getValue("album", "Unknown Album");
        meta.year = metadata.getValue("year", "Unknown Year");

        // حساب مدة الملف الصوتي
        double seconds = reader->lengthInSamples / reader->sampleRate;
        int totalSeconds = static_cast<int>(seconds);
        int minutes = totalSeconds / 60;
        int secs = totalSeconds % 60;

        meta.duration = juce::String::formatted("%02d:%02d", minutes, secs);
    }
    else
    {
        // استخدام قيم افتراضية إذا تعذر قراءة الملف
        meta.title = file.getFileNameWithoutExtension();
        meta.artist = "Unknown Artist";
        meta.album = "Unknown Album";
        meta.year = "Unknown Year";
        meta.duration = "00:00";
    }

    // ضمان وجود عنوان للملف
    if (meta.title.isEmpty())
        meta.title = file.getFileNameWithoutExtension();

    return meta;
}

// ========== دالة البناء لـ PlayerGUI ==========
// تهيئة واجهة المستخدم لعنصر تشغيل الصوت
PlayerGUI::PlayerGUI(PlayerAudio& player)
    : playerAudio(player)
{
    // تهيئة مدير تنسيقات الصوت
    formatManager.registerBasicFormats();

    // ========== إضافة أزرار التحكم ==========
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

    // ========== تهيئة ملصق البيانات الوصفية ==========
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setFont(juce::Font("Arial", 32.0f, juce::Font::bold));
    addAndMakeVisible(metadataLabel);

    // ========== تهيئة سلايدر الصوت (Volume) ==========
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // ========== تهيئة سلايدر السرعة (Speed) ==========
    speedSlider.setRange(0.1, 1.95, 0.01);
    speedSlider.setValue(1);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    // ========== تهيئة سلايدر التقدم (Progress) ==========
    progressSlider.setRange(0, playerAudio.getLength());
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.addListener(this);
    addAndMakeVisible(progressSlider);
    startTimerHz(30); // بدء الموقت لتحديث واجهة المستخدم 30 مرة في الثانية

    // ========== تهيئة ملصق الوقت ==========
    addAndMakeVisible(timeLabel);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    // ========== تهيئة الملصقات الوصفية للسلايدرات ==========
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

    // ========== تهيئة قائمة التشغيل ==========
    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setRowHeight(25);
}

// ========== دالة التدمير ==========
PlayerGUI::~PlayerGUI() {}

// ========== إعداد تشغيل الصوت ==========
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// ========== الحصول على كتلة الصوت التالية ==========
void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

// ========== تحرير الموارد ==========
void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

// ========== دالة إعادة تحديد حجم المكون ==========
void PlayerGUI::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // ========== صف الأزرار العلوي ==========
    auto buttonRow = bounds.removeFromTop(40);
    int buttonWidth = buttonRow.getWidth() / 14; // قسم على عدد الأزرار تقريباً

    addMarkerButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    markersBox.setBounds(buttonRow.removeFromLeft(buttonWidth * 2).reduced(2));
    loadButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    goToStartButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    backward10Button.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    playPauseButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    forward10Button.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    goToEndButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    restartButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    muteButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    repeatButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2));
    AB_loopButton.setBounds(buttonRow.reduced(2));

    // ========== القائمة الجانبية ==========
    playlistBox.setBounds(bounds.removeFromLeft(bounds.getWidth() / 3).reduced(5));

    // ========== منطقة التحكم ==========
    auto controlArea = bounds;

    // ========== منطقة سلايدر الصوت ==========
    auto volumeArea = controlArea.removeFromTop(35);
    volumeLabel.setBounds(volumeArea.removeFromLeft(70).reduced(2));
    volumeSlider.setBounds(volumeArea.reduced(5));
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // ========== منطقة سلايدر السرعة ==========
    auto speedArea = controlArea.removeFromTop(35);
    speedLabel.setBounds(speedArea.removeFromLeft(70).reduced(2));
    speedSlider.setBounds(speedArea.reduced(5));
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // ========== منطقة سلايدر التقدم ==========
    auto progressArea = controlArea.removeFromTop(35);
    positionLabel.setBounds(progressArea.removeFromLeft(70).reduced(2));
    progressSlider.setBounds(progressArea.reduced(5));
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // ========== ملصق الوقت ==========
    timeLabel.setBounds(controlArea.removeFromTop(20).reduced(2));

    // ========== ملصق البيانات الوصفية ==========
    metadataLabel.setBounds(controlArea.reduced(5));
}

// ========== معالج حدث النقر على الأزرار ==========
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // فتح نافذة اختيار الملفات
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

// ========== تبديل حالة كتم الصوت ==========
void PlayerGUI::toggleMute()
{
    muted = !muted; // flip state
    if (onMuteChanged) onMuteChanged(); // notify MainComponent
    repaint();
}

// ========== معالج حدث تغيير قيمة السلايدر ==========
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    // ========== سلايدر الصوت ==========
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)volumeSlider.getValue());

    }
    // ========== سلايدر السرعة ==========
    if (slider == &speedSlider)
    {
        playerAudio.setSpeed((float)speedSlider.getValue());

    }
    // ========== سلايدر التقدم ==========
    if (slider == &progressSlider)
        playerAudio.setPosition((float)progressSlider.getValue());

}

// ========== الحصول على عدد الصفوف في قائمة التشغيل ==========
int PlayerGUI::getNumRows() { return playlistFiles.size(); }

// ========== رسم عنصر في قائمة التشغيل ==========
void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll(juce::Colours::white);
    if (rowNumber >= 0 && rowNumber < playlistFiles.size())
        g.drawText(playlistFiles[rowNumber].getFileNameWithoutExtension(), 5, 0, width, height, juce::Justification::centredLeft);
}

// ========== دالة لتحويل الوقت من ثواني إلى تنسيق MM:SS ==========
static juce::String formatTime(double seconds)
{
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    return juce::String::formatted("%02d:%02d", mins, secs);
}

// ========== دالة الموقت للتحديث المستمر للواجهة ==========
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

// ========== معالج حدث تغيير الصف المحدد في قائمة التشغيل ==========
void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistFiles.size())
    {
        juce::File file = playlistFiles[lastRowSelected];
        playerAudio.loadFile(file);

        // Read metadata using TagLib
        auto meta = readMetadata(file);

        juce::String info;
        info += "Title: " + (meta.title.isNotEmpty() ? meta.title : file.getFileNameWithoutExtension()) + "\n";
        info += "Artist: " + (meta.artist.isNotEmpty() ? meta.artist : "Unknown Artist") + "\n";
        info += "Album: " + (meta.album.isNotEmpty() ? meta.album : "Unknown Album") + "\n";
        info += "Year: " + (meta.year.isNotEmpty() ? meta.year : "Unknown Year") + "\n";
        info += "Duration: " + (meta.duration.isNotEmpty() ? meta.duration : "Unknown Duration");

        metadataLabel.setText(info, juce::dontSendNotification);
    }
}

// ========== دالة الرسم الرئيسية ==========
void PlayerGUI::paint(juce::Graphics& g)
{
    // juce::Colour(0xff6a3fa0);
    // juce::Colour(0xff1b082a);

}
