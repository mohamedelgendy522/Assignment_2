#include "MainComponent.h"
MainComponent::MainComponent() : player1(playerAudio)
{
    addAndMakeVisible(player1);
    player1.onMuteChanged = [this] { updateMute(); };
    setSize(650, 450);
    setAudioChannels(0, 2);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
}
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double
sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo&
bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    player1.getNextAudioBlock(bufferToFill);
}
void MainComponent::releaseResources()
{
    player1.releaseResources();
}
void MainComponent::resized() {
    player1.setBounds(getLocalBounds());
}
void MainComponent::updateMute()
{
    isMuted = player1.isMuted();
    playerAudio.setMuted(isMuted);
    repaint();
}
