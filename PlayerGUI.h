#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
class IconButton : public juce::TextButton {
public:
    enum class Type { Play, Pause, Start, End, Restart, Stop, Mute, Forward10, Backward10 };
    IconButton(Type t) : type(t) {}
    void setMuted(bool m) { muted = m; repaint(); } // setter
    bool isMuted() const { return muted; } // getter
    void paintButton(juce::Graphics& g, bool isMouseOver, bool
        isButtonDown) override {
        auto bounds = getLocalBounds().toFloat();
        // Background color
        g.setColour(isButtonDown ? juce::Colours::darkgrey
            : (isMouseOver ? juce::Colours::grey :
                juce::Colours::lightgrey));
        g.fillRoundedRectangle(bounds, 8.0f);
        g.setColour(juce::Colours::black);

        switch (type)
        {
        case Type::Play:
        {
            // ► triangle
            juce::Path playIcon;
            playIcon.addTriangle(bounds.getX() + bounds.getWidth() *
                0.35f,
                bounds.getY() + bounds.getHeight() * 0.25f,
                bounds.getX() + bounds.getWidth() * 0.35f,
                bounds.getY() + bounds.getHeight() * 0.75f,
                bounds.getX() + bounds.getWidth() * 0.75f,
                bounds.getY() + bounds.getHeight() * 0.5f);
            g.fillPath(playIcon);
            break;
        }
        case Type::Pause:
        {
            // ‖ bars
            float barWidth = bounds.getWidth() * 0.15f;
            float gap = barWidth;
            float height = bounds.getHeight() * 0.5f;
            float top = bounds.getCentreY() - height / 2.0f;
            g.fillRect(bounds.getCentreX() - gap / 2 - barWidth, top,
                barWidth, height);
            g.fillRect(bounds.getCentreX() + gap / 2, top, barWidth,
                height);
            break;
        }
        case Type::Start:
        {
            // |◄ icon
            juce::Path startIcon;
            float midY = bounds.getCentreY();
            float leftX = bounds.getX() + bounds.getWidth() * 0.2f;
            float rightX = bounds.getRight() - bounds.getWidth() * 0.2f;
            // Vertical bar |
            g.fillRect(
                leftX + bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.25f,
                bounds.getWidth() * 0.06f,
                bounds.getHeight() * 0.5f
            );
            // Triangle ◄
            startIcon.addTriangle(
                rightX - bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.25f,
                rightX - bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.75f,
                leftX + bounds.getWidth() * 0.22f,
                midY
            );
            g.fillPath(startIcon);
            break;
        }
        case Type::End:
        {
            // ►| icon
            juce::Path endIcon;
            float midY = bounds.getCentreY();
            float leftX = bounds.getX() + bounds.getWidth() * 0.2f;
            float rightX = bounds.getRight() - bounds.getWidth() * 0.2f;
            // Triangle ►
            endIcon.addTriangle(
                leftX + bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.25f,
                leftX + bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.75f,
                rightX - bounds.getWidth() * 0.18f,
                midY);
            g.fillPath(endIcon);
            // Vertical bar |
            g.fillRect(rightX - bounds.getWidth() * 0.15f,
                bounds.getY() + bounds.getHeight() * 0.25f,
                bounds.getWidth() * 0.06f,
                bounds.getHeight() * 0.5f);
            break;
        }
        case Type::Restart:
        {
            juce::Path restartIcon;
            auto center = bounds.getCentre();
            float radius = bounds.getWidth() * 0.25f;
            float thickness = bounds.getWidth() * 0.05f;
            // ⟳ shape
            float startAngle = juce::MathConstants<float>::pi * 0.7f;
            float endAngle = juce::MathConstants<float>::twoPi * 1.1f;
            restartIcon.addCentredArc(center.x, center.y, radius, radius,
                0.0f, startAngle, endAngle, true);
            // arrowhead
            float arrowAngle = endAngle +
                juce::MathConstants<float>::twoPi * 0.75f;
            float arrowLength = radius * 0.8f;
            float arrowWidth = radius * 0.5f;
            juce::Point<float> arrowTip(
                center.x + std::cos(arrowAngle) * (radius + 7.5),
                center.y + std::sin(arrowAngle) * (radius + 7.5));
            juce::Path arrow;
            arrow.addTriangle(
                arrowTip.x,
                arrowTip.y,
                arrowTip.x - std::cos(arrowAngle - 0.5f) * arrowLength -
                std::sin(arrowAngle - 0.5f) * arrowWidth,
                arrowTip.y - std::sin(arrowAngle - 0.5f) * arrowLength +
                std::cos(arrowAngle - 0.5f) * arrowWidth,
                arrowTip.x - std::cos(arrowAngle + 0.5f) * arrowLength -
                std::sin(arrowAngle + 0.5f) * arrowWidth,
                arrowTip.y - std::sin(arrowAngle + 0.5f) * arrowLength +
                std::cos(arrowAngle + 0.5f) * arrowWidth);
            g.strokePath(restartIcon, juce::PathStrokeType(thickness));
            g.fillPath(arrow);
            break;
        }
        case Type::Mute: {
            juce::Path speaker;
            // Speaker dimensions
            float speakerWidth = bounds.getWidth() * 0.25f;
            float speakerHeight = bounds.getHeight() * 0.35f;
            float x = bounds.getCentreX() - speakerWidth * 1.1f;
            float y = bounds.getCentreY() - speakerHeight / 2.0f;
            // Rectangle coordinates
            float left = x;
            float top = y;
            float right = x + speakerWidth * 0.8f;
            float bottom = y + speakerHeight;
            // Speaker body (rectangle)
            speaker.startNewSubPath(left, top); // top-left
            speaker.lineTo(right, top); // top-right
            speaker.lineTo(right, bottom); // bottom-right
            speaker.lineTo(left, bottom); // bottom-left
            speaker.closeSubPath();
            // Speaker cone (triangle)
            juce::Path startIcon;
            float midY = bounds.getCentreY();
            float leftX = bounds.getX() + bounds.getWidth() * 0.2f;
            float rightX = bounds.getRight() - bounds.getWidth() * 0.2f;
            startIcon.addTriangle(
                rightX - bounds.getWidth() * 0.1f,
                bounds.getY() + bounds.getHeight() * 0.05f,
                rightX - bounds.getWidth() * 0.1f,
                bounds.getY() + bounds.getHeight() * 0.95f,
                leftX + bounds.getWidth() * 0.05f,
                midY
            );
            g.fillPath(startIcon);
            g.setColour(juce::Colours::black);
            g.fillPath(speaker);
            // Mute line (cross line over the speaker)
            g.setColour(juce::Colours::red);
            if (muted) {
                g.drawLine(
                    bounds.getCentreX() - speakerWidth,
                    bounds.getCentreY() + speakerHeight,
                    bounds.getCentreX() + speakerWidth,
                    bounds.getCentreY() - speakerHeight,
                    3.0f // Line thickness
                );
            }
            break;
        }
        case Type::Stop:
        {
            float size = bounds.getWidth() * 0.4f;
            float x = bounds.getCentreX() - size / 2.0f;
            float y = bounds.getCentreY() - size / 2.0f;
            g.fillRect(x, y, size, size);
            break;
        }
            case Type::Backward10: {
            juce::Path back;

            float w = bounds.getWidth();
            float h = bounds.getHeight();
            float cx = bounds.getCentreX();
            float cy = bounds.getCentreY();

            // first arrow
            back.startNewSubPath(cx + w * 0.15f, cy - h * 0.25f);
            back.lineTo(cx - w * 0.25f, cy);
            back.lineTo(cx + w * 0.15f, cy + h * 0.25f);
            back.closeSubPath();

            // second arrow
            back.startNewSubPath(cx + w * 0.35f, cy - h * 0.25f);
            back.lineTo(cx - w * 0.05f, cy);
            back.lineTo(cx + w * 0.35f, cy + h * 0.25f);
            back.closeSubPath();

            g.setColour(juce::Colours::black);
            g.fillPath(back);
            break;
            }

            case Type::Forward10: {
            juce::Path forward;

            float w = bounds.getWidth();
            float h = bounds.getHeight();
            float cx = bounds.getCentreX();
            float cy = bounds.getCentreY();

            // first arrow
            forward.startNewSubPath(cx - w * 0.15f, cy - h * 0.25f);
            forward.lineTo(cx + w * 0.25f, cy);
            forward.lineTo(cx - w * 0.15f, cy + h * 0.25f);
            forward.closeSubPath();

            // second arrow
            forward.startNewSubPath(cx - w * 0.35f, cy - h * 0.25f);
            forward.lineTo(cx + w * 0.05f, cy);
            forward.lineTo(cx - w * 0.35f, cy + h * 0.25f);
            forward.closeSubPath();

            g.setColour(juce::Colours::black);
            g.fillPath(forward);
            break;
            }
        }
    }
    void setType(Type newType)
    {
        type = newType;
        repaint();
    }
private:
    Type type;
    bool muted = false;
};
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI(PlayerAudio& player);

    bool isMuted() const { return muted; }
    std::function<void()> onMuteChanged;
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

private:
    bool isRepeating = false;
    bool muted = false;
    bool isPlaying = false;
	int State = 0;
    void toggleMute();
    void timerCallback() override;
    PlayerAudio& playerAudio;
    
    // GUI elements
    juce::TextButton loadButton{ "Load" };
    IconButton goToStartButton{ IconButton::Type::Start };
    IconButton playPauseButton{ IconButton::Type::Play };
    IconButton goToEndButton{ IconButton::Type::End };
    IconButton restartButton{ IconButton::Type::Restart };
    IconButton stopButton{ IconButton::Type::Stop };
    IconButton muteButton{ IconButton::Type::Mute };
    IconButton forward10Button{ IconButton::Type::Forward10 };
    IconButton backward10Button{ IconButton::Type::Backward10 };
    juce::TextButton repeatButton{ "Repeat" };
    juce::TextButton AB_loopButton{ "AB Loop" };    
    juce::Slider volumeSlider;
    juce::Label metadataLabel;
    juce::Slider speedSlider;
    juce::Slider progressSlider;
    juce::Label timeLabel, volumeLabel, speedLabel, positionLabel;

    juce::AudioFormatManager formatManager;
    
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ListBox playlistBox;
    juce::Array<juce::File> playlistFiles;

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
