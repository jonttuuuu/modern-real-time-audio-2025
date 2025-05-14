#pragma once

#include <JuceHeader.h>

namespace Param
{
    namespace ID
    {
        static const juce::String ModulationEnabled { "modulation" };
        static const juce::String Frequency { "frequency" };
    }

    namespace Name
    {
        static const juce::String ModulationEnabled { "modulation" };
        static const juce::String Frequency { "Frequency" };
    }
}

class MainProcessor : public juce::AudioProcessor
{
public:
    MainProcessor();
    ~MainProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParameterManager() { return parameterManager; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================

private:
    mrta::ParameterManager parameterManager;
    float frequency;
    float phase;
    bool modulationEnabled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessor)
};
