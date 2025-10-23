#include "PlayerAudio.h"
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio :: ~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
    if (Repeat){
        if ((transportSource.getLengthInSeconds() - transportSource.getCurrentPosition()) < 0.05)
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}
bool PlayerAudio::loadFile(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
            return true;
        }
    }
    return false;
}
void PlayerAudio::play() {
    transportSource.start();
}
void PlayerAudio::pause() {
    transportSource.stop();
}
void PlayerAudio::end() {
    auto length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
}
void PlayerAudio::start() {
    transportSource.setPosition(0.0);
    transportSource.stop();
}
void PlayerAudio::stop() {
    transportSource.stop();
    transportSource.setPosition(0.0);
}
void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
}
void PlayerAudio::setPosition(float pos) {
    transportSource.setPosition(pos);
}
double PlayerAudio::getPosition() {
    return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() {
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::setRepeat(bool shouldRepeat) {
	Repeat = shouldRepeat;
}
void PlayerAudio::setMuted(bool shouldMute ) {
    isMuted = shouldMute;
    if (shouldMute) {
        lastGain = transportSource.getGain();
        transportSource.setGain(0.0f);
    } else {
        transportSource.setGain(lastGain);
    }
}
