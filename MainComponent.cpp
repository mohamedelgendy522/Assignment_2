#include "MainComponent.h"
MainComponent::MainComponent()
{

    playerAudio2.loadLastSession();
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    player1.onMuteChanged = [this] { updateMute(); };
    player2.onMuteChanged = [this] { updateMute(); };

    // mix toggle
    addAndMakeVisible(mixToggle);
    mixToggle.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    mixToggle.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mixToggle.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    mixToggle.setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightgrey);
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

    setSize(1200, 700); // زيادة العرض ليتناسب مع التخطيط الجديد
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

    // زر Mix في الأعلى
    auto top = area.removeFromTop(30);
    mixToggle.setBounds(top.removeFromLeft(200).reduced(4));

    // تقسيم المساحة المتبقية إلى ثلاثة أجزاء أفقية
    // player1 | crossfade | player2
    int crossfadeWidth = 80;  // عرض شريط crossfade
    int playerWidth = (area.getWidth() - crossfadeWidth) / 2;

    // player1 على اليسار
    auto player1Area = area.removeFromLeft(playerWidth);
    player1.setBounds(player1Area.reduced(5));

    // crossfade في المنتصف
    auto crossfadeArea = area.removeFromLeft(crossfadeWidth);
    crossfadeLabel.setBounds(crossfadeArea.removeFromTop(20));
    crossfadeSlider.setBounds(crossfadeArea.reduced(5));

    // player2 على اليمين
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

    juce::Colour colMaxPurple = juce::Colour(0xFF733381);  // Maximum Purple
    juce::Colour colImperial = juce::Colour(0xFF612E6C);   // Imperial
    juce::Colour colJapViolet = juce::Colour(0xFF4F2958);  // Japanese Violet
    juce::Colour colAmerPurple = juce::Colour(0xFF3E2443); // American Purple
    juce::Colour colDarkPurple = juce::Colour(0xFF2C1F2F); // Dark Purple
    juce::Colour colEerieBlack = juce::Colour(0xFF1A1A1A); // Eerie Black

    // إنشاء تدرج رأسي
    juce::ColourGradient gradient(
        colMaxPurple,
        getLocalBounds().getTopLeft().toFloat(),
        colEerieBlack,
        getLocalBounds().getBottomLeft().toFloat(),
        false
    );

    gradient.addColour(0.2, colImperial);
    gradient.addColour(0.4, colJapViolet);
    gradient.addColour(0.6, colAmerPurple);
    gradient.addColour(0.8, colDarkPurple);

    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    // رسم العنوان بالأبيض
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Dual Audio Player",
        getLocalBounds().removeFromTop(50),
        juce::Justification::centred);

    // إضافة خطوط فاصلة للتصميم
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    int centerX = getWidth() / 2;
    g.drawLine(centerX - 40, 40, centerX - 40, getHeight() - 40, 2.0f);
    g.drawLine(centerX + 40, 40, centerX + 40, getHeight() - 40, 2.0f);
}
void MainComponent::updateCrossfade()
{
    float crossfadeValue = (float)crossfadeSlider.getValue();

    // player1 يبدأ من اليسار (عندما crossfade = 1.0)
    // player2 يبدأ من اليمين (عندما crossfade = 0.0)
    float player1Gain = crossfadeValue;
    float player2Gain = 1.0f - crossfadeValue;

    playerAudio1.setGain(player1Gain);
    playerAudio2.setGain(player2Gain);

    // تحديث المخطط إذا لزم الأمر
    repaint();
}
