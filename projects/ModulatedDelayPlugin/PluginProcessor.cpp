#include "PluginProcessor.h"
#include "PluginEditor.h"


void ModulatedDelay::prepare(double sampleRate, int maxDelayMilliseconds)
{
    sr = sampleRate;
    size_t maxDelaySamples = size_t((maxDelayMilliseconds / 1000.0) * sr) + 1;
    buffer.setSize (1, (int)maxDelaySamples);
    buffer.clear();
    writeIndex = 0;

    delayTimeSmooth  = targetDelayMs;
    feedbackSmooth   = targetFeedback;
    wetDrySmooth     = targetWetDry;     
}

void ModulatedDelay::setParameters(float newDelayMs, float newFeedback, float newWet)
{
    targetDelayMs = newDelayMs;
    targetFeedback = newFeedback;
    targetWetDry = newWet;
}

void ModulatedDelay::processBlock(AudioBuffer<float>& bufferToFill)
{
    auto* buf = buffer.getWritePointer(0);
    auto* in  = bufferToFill.getReadPointer (0);
    auto* out = bufferToFill.getWritePointer (0);
    int numSamples = bufferToFill.getNumSamples();
        
    for (int n = 0; n < numSamples; ++n)
    {
        delayTimeSmooth  += smoothingCoeff * (targetDelayMs  - delayTimeSmooth);
        feedbackSmooth   += smoothingCoeff * (targetFeedback - feedbackSmooth);
        wetDrySmooth     += smoothingCoeff * (targetWetDry   - wetDrySmooth);
        float dryGain = 1.0f - wetDrySmooth;
            
        float delaySamples = (delayTimeSmooth / 1000.0f) * float(sr);
        
        int readIndex = writeIndex - int(delaySamples);
        if (readIndex < 0) 
            readIndex += buffer.getNumSamples();
        
        float delayedIn  = buf[readIndex];
        float delayedOut = buf[readIndex]; 
    
        float xn = in[n];
        float yn = wetDrySmooth * (delayedIn + feedbackSmooth * delayedOut)
                   + dryGain * xn;
        
        buf[writeIndex] = yn;
        out[n] = yn;
    
        if (++writeIndex >= buffer.getNumSamples())
            writeIndex = 0;
    }
}

static const std::vector<mrta::ParameterInfo> ParameterInfos
{
    // ID,       name,                  unit,  default,    min,     max,       step,   skew
    { Param::ID::Enabled,   Param::Name::Enabled,   "Off", "On", true },
    { Param::ID::DelayTime, "Delay Time",  "ms",  500.0f,     1.0f,    2000.0f,   1.0f,   1.0f },
    { Param::ID::Feedback,  "Feedback",     "",    0.5f,       0.0f,    0.99f,     0.01f,  1.0f },
    { Param::ID::WetDryMix, "Wet/Dry Mix",  "",    0.5f,       0.0f,    1.0f,      0.01f,  1.0f },
};

MainProcessor::MainProcessor() :
    parameterManager(*this, ProjectInfo::projectName, ParameterInfos),
    modulatedDelay()
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
        [this](float value, bool)
        {
            enabled = value > 0.5f;
        });
    
    parameterManager.registerParameterCallback(Param::ID::DelayTime,
        [this](float value, bool)
        {
            delayTime = value;
        });
    
    parameterManager.registerParameterCallback(Param::ID::Feedback,
        [this](float value, bool)
        {
            feedback = value;
        });
    
    parameterManager.registerParameterCallback(Param::ID::WetDryMix,
        [this](float value, bool)
        {
            wetDry = value;
        });
}

MainProcessor::~MainProcessor()
{
}

void MainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::uint32 numChannels { static_cast<juce::uint32>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    parameterManager.updateParameters(true);

    modulatedDelay.prepare(sampleRate, 2000);
}

void MainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();
    if (!enabled) return;

    modulatedDelay.setParameters(delayTime, feedback, wetDry);
    modulatedDelay.processBlock(buffer);
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
