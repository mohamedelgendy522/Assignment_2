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
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::updatePlaybackLogic()
{
    if (ok && A >= 0 && B > A)
    {
        if (transportSource.getCurrentPosition() >= B)
        {
            transportSource.setPosition(A);
        }
    }
    else if (Repeat) {
        if ((transportSource.getLengthInSeconds() - transportSource.getCurrentPosition()) < 0.05)
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    updatePlaybackLogic();

    resampleSource.getNextAudioBlock(bufferToFill);

}
void PlayerAudio::releaseResources()
{
    resampleSource.releaseResources();
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
            return true;
        }
    }
    return false;
}
void PlayerAudio::play() {
    transportSource.start();
}
void PlayerAudio::stop() {
    transportSource.stop();
    transportSource.setPosition(0.0);
}
void PlayerAudio::pause() {
    transportSource.stop();
}
void PlayerAudio::restart() {
    transportSource.setPosition(0.0);
    //transportSource.start();
}
void PlayerAudio::end() {
    auto length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length);
}
void PlayerAudio::start() {
    transportSource.setPosition(0.0);
    transportSource.stop();
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
void PlayerAudio::setMuted(bool shouldMute) {
    isMuted = shouldMute;
    float currentGain = transportSource.getGain();
    transportSource.setGain(shouldMute ? 0.0f : currentGain);
}
void PlayerAudio::setRepeat(bool shouldRepeat) {
    Repeat = shouldRepeat;
}
void PlayerAudio::setSpeed(float s) {
    resampleSource.setResamplingRatio(s);
}
void PlayerAudio::setA(float a)
{
    A = a;

}
void PlayerAudio::setB(float b)
{
    B = b;
}

void PlayerAudio::isOk(bool check)
{
	ok = check;
}
juce :: AudioSource* PlayerAudio::getAudioSource()
{
    return &resampleSource;
}
