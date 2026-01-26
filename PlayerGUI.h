#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class IconButton : public juce::TextButton {
public:
    void setRepeating(bool r) { repeating = r; repaint(); }
    bool isRepeating() const { return repeating; }

    enum class Type { Play, Pause, Start, End, Restart, Stop, Mute, Forward10, Backward10, Repeat };
    IconButton(Type t) : type(t) {}
    void setMuted(bool m) { muted = m; repaint(); } // setter
    bool isMuted() const { return muted; } // getter

    void paintButton(juce::Graphics& g, bool isMouseOver, bool isButtonDown) override {

        auto bounds = getLocalBounds().toFloat().reduced(6.0f);

        g.setColour(juce::Colours::transparentBlack);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

        // 🔹 Draw light borders only on hover
        if (isMouseOver) {
            g.setColour(juce::Colour(0xFF00E5FF)); // Neon color on mouse hover
        } else {
            g.setColour(juce::Colours::white.withAlpha(0.9f)); // Soft white color
        }

        // 🔹 Basic icon drawing
        g.setColour(juce::Colours::white);

        switch (type)
        {
        case Type::Play:
        {
            // ► triangle
            juce::Path playIcon;
            playIcon.addTriangle(bounds.getX() + bounds.getWidth() * 0.35f,
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
            g.fillRect(bounds.getCentreX() - gap / 2 - barWidth, top, barWidth, height);
            g.fillRect(bounds.getCentreX() + gap / 2, top, barWidth, height);
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
            // 1. Fixed dimensions (to ensure clarity regardless of button size)
            float r = 7.0f;           // Radius (Diameter 14px - small and suitable)
            float strokeWidth = 1.5f; // Line thickness (thin like Repeat)

            // Centering
            float cx = bounds.getCentreX();
            float cy = bounds.getCentreY();

            g.setColour(juce::Colours::white);

            juce::Path p;

            // 2. Draw Arc (open at the top)
            // Angle 0 is 12 o'clock
            // Start from 1 o'clock (0.2pi) to 11 o'clock (1.8pi)
            p.addCentredArc(cx, cy, r, r, 0.0f,
                0.2f * juce::MathConstants<float>::pi,
                1.8f * juce::MathConstants<float>::pi,
                true); // true = Clockwise (to draw the circle from the bottom)

            g.strokePath(p, juce::PathStrokeType(strokeWidth));

            // 3. Draw Arrow (small and simple)
            // Arrow is at the start of the arc (1 o'clock) pointing backwards (North-West)
            juce::Path arrow;
            float arrowSize = 3.5f; // Small arrow size

            // Calculate arrow tip position accurately
            // (Same position as arc start)
            float angle = 0.2f * juce::MathConstants<float>::pi;
            float tipX = cx + r * std::sin(angle);
            float tipY = cy - r * std::cos(angle);

            // Draw Triangle (pointing North-West)
            // Simple approximate coordinates instead of complex calculations
            arrow.addTriangle(tipX, tipY,                     // Tip
                tipX + 1.0f, tipY + arrowSize * 1.5f, // Right leg
                tipX - arrowSize * 1.5f, tipY + 2.0f);// Left leg

            // Simple rotation for the arrow to align with circle curvature
            arrow.applyTransform(juce::AffineTransform::rotation(-0.2f, tipX, tipY));

            g.fillPath(arrow);

            // 4. Center dot (adds aesthetic touch)
            g.fillEllipse(cx - 1.5f, cy - 1.5f, 3.0f, 3.0f);

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
            g.setColour(juce::Colours::white);
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

            g.setColour(juce::Colours::white);
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

            g.setColour(juce::Colours::white);
            g.fillPath(forward);
            break;
        }
        case Type::Repeat:
        {
            // 1. Dimensions and Centering
            float iconW = 20.0f;
            float iconH = 14.0f;
            float corner = 4.0f;
            float strokeWidth = 1.5f; // Thin and clean line

            float cx = bounds.getCentreX();
            float cy = bounds.getCentreY();
            float x = cx - iconW / 2.0f;
            float y = cy - iconH / 2.0f;

            g.setColour(repeating ? juce::Colour(0xFF00E5FF) : juce::Colours::white);

            juce::Path p;

            // 2. Drawing "Clockwise"
            // Start: From bottom-left corner (to ensure this corner exists)
            p.startNewSubPath(x + corner, y + iconH); // Start from the bottom line towards the left

            // Draw bottom-left corner (BL) and go up
            p.quadraticTo(x, y + iconH, x, y + iconH - corner);

            // Left side (going up)
            p.lineTo(x, y + corner);

            // Top-left corner (TL)
            p.quadraticTo(x, y, x + corner, y);

            // Top side (right)
            p.lineTo(x + iconW - corner, y);

            // Top-right corner (TR)
            p.quadraticTo(x + iconW, y, x + iconW, y + corner);

            // Right side (going down)
            p.lineTo(x + iconW, y + iconH - corner);

            // Bottom-right corner (BR)
            p.quadraticTo(x + iconW, y + iconH, x + iconW - corner, y + iconH);

            // Bottom side (left) - stop in the middle to leave space for the arrow
            p.lineTo(x + iconW * 0.45f, y + iconH); // Stop at 45% of the width

            g.strokePath(p, juce::PathStrokeType(strokeWidth));

            // 3. Draw arrow (pointing left, at the end of the line)
            juce::Path arrow;
            float tipX = x + iconW * 0.45f; // Where we stopped the line
            float tipY = y + iconH;
            float arrowSize = 3.5f;

            // Arrowhead points to the left (towards the gap)
            arrow.addTriangle(tipX - arrowSize, tipY,             // Tip (left)
                tipX + arrowSize * 0.5f, tipY - arrowSize * 0.7f, // Upper wing
                tipX + arrowSize * 0.5f, tipY + arrowSize * 0.7f);// Lower wing

            g.fillPath(arrow);

            // 4. Dot (optional)
            if (repeating)
            {
                g.fillEllipse(cx - 1.5f, cy - 1.5f, 3.0f, 3.0f);
            }
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
    bool repeating = false;
};

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel,
    public juce::ChangeListener
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
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;


private:
    int buttonHeight = 40;
    int controlHeight = 100;
    int playlistHeight = 200;
    bool isRepeating = false;
    bool muted = false;
    bool isPlaying = false;
    bool isActive = false ;
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
    IconButton repeatButton{ IconButton::Type::Repeat };
    juce::TextButton AB_loopButton{ "AB Loop" };
    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    juce::Label metadataLabel;

    juce::Slider progressSlider;
    juce::Label timeLabel, volumeLabel, speedLabel, positionLabel;

    juce::AudioFormatManager formatManager;

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ListBox playlistBox;
    juce::Array<juce::File> playlistFiles;

    juce::AudioThumbnailCache thumbnailCache{ 5 }; // Caches the last 5 files for performance
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    juce::TextButton addMarkerButton{ "Add Marker" };
    juce::ComboBox markersBox;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
