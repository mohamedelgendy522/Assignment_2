#pragma once
#include <JuceHeader.h>
class PlayerAudio {
public:
    PlayerAudio();
    ~PlayerAudio();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    bool loadFile(const juce::File& file);
	void setMuted(bool shouldMute);
    void play();
    void pause();
    void restart();
    void end();
    void start();
    void stop();
    void setGain(float gain);
    void setPosition(float pos);
    double getPosition();
    double getLength();
	void setRepeat(bool shouldRepeat);
private:
    bool Repeat = false;
	float lastGain = 1.0f;
    bool isMuted = false;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{ &transportSource, false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
