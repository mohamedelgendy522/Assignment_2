#pragma once
#include <JuceHeader.h>
class PlayerAudio {
public:
    PlayerAudio();
    ~PlayerAudio();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo&
        bufferToFill);
    void releaseResources();
    bool loadFile(const juce::File& file);
    void setMuted(bool shouldMute);
    void play();
    void stop();
    void pause();
    void restart();
    void end();
    void start();
    void setGain(float gain);
    void setPosition(float pos);
    double getPosition();
    double getLength();
    void setRepeat(bool shouldRepeat);
    void setSpeed(float s);
    void setA(float a);
    void setB(float b);
    void isOk(bool check);
private:
    bool Repeat = false;
    bool isMuted = false;
    int State = 0;
    bool ok = false;
    float A = -1, B = -1;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{ &transportSource, false };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
