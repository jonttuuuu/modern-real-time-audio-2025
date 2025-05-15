#pragma once

#include <JuceHeader.h>

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String DelayTime  { "delayTime" };
        static const juce::String Feedback   { "feedback"  };
        static const juce::String WetDryMix  { "wetDry"    };
    }

    namespace Name
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String DelayTime  { "Delay Time (ms)" };
        static const juce::String Feedback   { "Feedback"         };
        static const juce::String WetDryMix  { "Wet/Dry Mix"      };
    }
}

class ModulatedDelay
{
public:
    ModulatedDelay() = default;
    ~ModulatedDelay() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void setParameters(float newDelayMs, float newFeedback, float newWet);
    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    juce::AudioBuffer<float> buffer;
    double sr = 48000.0;
    int writeIndex = 0;

    float targetDelayMs   = 500.0f;
    float targetFeedback  = 0.5f;
    float targetWetDry    = 0.5f;

    float delayTimeSmooth = 500.0f;
    float feedbackSmooth  = 0.5f;
    float wetDrySmooth    = 0.5f;
    
    const float smoothingCoeff = 0.001f;
};

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
    ModulatedDelay modulatedDelay;
    bool enabled;
    float delayTime = 500.0f;
    float feedback  = 0.5f;
    float wetDry    = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessor)
};
