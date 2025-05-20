#include "PluginProcessor.h"
#include "PluginEditor.h"

static const std::vector<mrta::ParameterInfo> ParameterInfos
{
    { Param::ID::ModulationEnabled,   Param::Name::ModulationEnabled,   "Off", "On", true },
    { Param::ID::Frequency, Param::Name::Frequency, "Hz", 1000.f, 20.f, 20000.f, 1.f, 0.3f },
};

MainProcessor::MainProcessor() :
    parameterManager(*this, ProjectInfo::projectName, ParameterInfos), 
    frequency(1000.0f),
    phase(0.0f),
    modulationEnabled(true),
    sampleRate(44100.0)
{
    parameterManager.registerParameterCallback(Param::ID::ModulationEnabled,
    [this] (float value, bool /*forced*/)
    {
        DBG(Param::Name::ModulationEnabled + ": " + juce::String { value });
        modulationEnabled = value > 0.5f;
    });

    parameterManager.registerParameterCallback(Param::ID::Frequency,
    [this] (float value, bool /*forced*/)
    {
        DBG(Param::Name::Frequency + ": " + juce::String { value });
        frequency = (value);
    });
}

MainProcessor::~MainProcessor()
{
}

void MainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::uint32 numChannels { static_cast<juce::uint32>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    this->sampleRate = sampleRate;
    phase = 0.0f; // Reset phase when playback starts
    parameterManager.updateParameters(true);
}

void MainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();
    
    if (!modulationEnabled) 
        return; // Simply pass through audio when disabled

    // Calculate phase increment based on current frequency and sample rate
    // This determines how quickly we move through the modulation waveform
    float phaseIncrement = 2.0f * juce::MathConstants<float>::pi * frequency / sampleRate;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* samples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Standard ring modulation: multiply input by carrier (cosine)
            samples[i] *= std::cos(phase);

            // Advance phase for next sample
            phase += phaseIncrement;
            
            // Keep phase in the range of [0, 2π] to prevent floating point errors
            if (phase >= 2.0f * juce::MathConstants<float>::pi)
                phase -= 2.0f * juce::MathConstants<float>::pi;
        }
    }
}

void MainProcessor::releaseResources()
{
}

void MainProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void MainProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

juce::AudioProcessorEditor* MainProcessor::createEditor()
{
    return new MainProcessorEditor(*this);
}

//==============================================================================
const juce::String MainProcessor::getName() const { return JucePlugin_Name; }
bool MainProcessor::acceptsMidi() const { return false; }
bool MainProcessor::producesMidi() const { return false; }
bool MainProcessor::isMidiEffect() const { return false; }
double MainProcessor::getTailLengthSeconds() const { return 0.0; }
int MainProcessor::getNumPrograms() { return 1; }
int MainProcessor::getCurrentProgram() { return 0; }
void MainProcessor::setCurrentProgram (int) { }
const juce::String MainProcessor::getProgramName(int) { return {}; }
void MainProcessor::changeProgramName(int, const juce::String&) { }
bool MainProcessor::hasEditor() const { return true; }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MainProcessor();
}
