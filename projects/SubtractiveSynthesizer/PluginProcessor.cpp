#include "PluginProcessor.h"
#include "PluginEditor.h"

static const std::vector<mrta::ParameterInfo> ParameterInfos
{
    // Oscillator parameters
    { Param::ID::OscType, Param::Name::OscType, { "Sine", "Saw", "Square", "Triangle" }, 1 },
    { Param::ID::Osc2Type, Param::Name::Osc2Type, { "Sine", "Saw", "Square", "Triangle" }, 1 },
    { Param::ID::OscMix, Param::Name::OscMix, "", 0.5f, 0.0f, 1.0f, 0.01f, 1.0f },
    
    // Filter parameters
    { Param::ID::FilterEnabled, Param::Name::FilterEnabled, "Off", "On", true },
    { Param::ID::FilterCutoff, Param::Name::FilterCutoff, "Hz", 1000.0f, 20.0f, 20000.0f, 1.0f, 0.3f },
    { Param::ID::FilterResonance, Param::Name::FilterResonance, "", 0.2f, 0.0f, 1.0f, 0.01f, 1.0f },
    { Param::ID::FilterMode, Param::Name::FilterMode, { "LPF12", "HPF12", "BPF12", "LPF24", "HPF24", "BPF24" }, 3 },
    
    // ADSR parameters
    { Param::ID::Attack, Param::Name::Attack, "ms", 100.0f, 1.0f, 5000.0f, 1.0f, 0.3f },
    { Param::ID::Decay, Param::Name::Decay, "ms", 250.0f, 1.0f, 5000.0f, 1.0f, 0.3f },
    { Param::ID::Sustain, Param::Name::Sustain, "", 0.8f, 0.0f, 1.0f, 0.01f, 1.0f },
    { Param::ID::Release, Param::Name::Release, "ms", 300.0f, 1.0f, 5000.0f, 1.0f, 0.3f },
    
    // Master parameters
    { Param::ID::MasterGain, Param::Name::MasterGain, "dB", 0.0f, -60.0f, 6.0f, 0.1f, 3.0f },
};

// SynthVoice Implementation
SynthVoice::SynthVoice()
{
    oscillator1.initialise([](float x) { return std::sin(x); });
    
    oscillator2.initialise([](float x) {
        // Band-limited sawtooth
        constexpr int harmonics = 30;
        float result = 0.0f;
        
        for (int i = 1; i <= harmonics; ++i)
            result += std::sin(i * x) / i;
        
        return result * (2.0f / juce::MathConstants<float>::pi);
    });
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    noteOn = true;
    midiNote = midiNoteNumber;
    
    level = velocity;
    
    auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator1.setFrequency(freq);
    oscillator2.setFrequency(freq);

    oscillator1.reset();
    oscillator2.reset();
    filter.reset();

    adsr.noteOn();
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        adsr.reset();
        clearCurrentNote();
        noteOn = false;
    }
}

void SynthVoice::pitchWheelMoved(int /*newValue*/)
{
}

void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!noteOn)
        return;
    
    const juce::ScopedNoDenormals noDenormals;

    juce::AudioBuffer<float> voiceBuffer(outputBuffer.getNumChannels(), numSamples);
    voiceBuffer.clear();
    

    const int numChannels = std::max(2, voiceBuffer.getNumChannels());

    const float osc1Mix = 1.0f - oscMix;
    const float osc2Mix = oscMix;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = voiceBuffer.getWritePointer(channel);
        
        // Process samples
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get oscillator samples (no phase offset - this could be a source of issues)
            float osc1Sample = oscillator1.processSample(0.0f);
            float osc2Sample = oscillator2.processSample(0.0f);
            
            // Mix the oscillators with proper levels
            channelData[sample] = (osc1Sample * osc1Mix + osc2Sample * osc2Mix) * level;
        }
    }
  
    juce::dsp::AudioBlock<float> voiceBlock(voiceBuffer);
    
    if (filterEnabled)
    {
        juce::dsp::ProcessContextReplacing<float> filterContext(voiceBlock);
        filter.process(filterContext);
    }

    adsr.applyEnvelopeToBuffer(voiceBuffer, 0, numSamples);
    
    juce::dsp::ProcessContextReplacing<float> gainContext(voiceBlock);
    masterGain.process(gainContext);
    
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        int sourceChannel = channel % numChannels;
        outputBuffer.addFrom(channel, startSample, voiceBuffer, sourceChannel, 0, numSamples);
    }
    
    if (!adsr.isActive())
    {
        clearCurrentNote();
        noteOn = false;
    }
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    
    oscillator1.prepare(spec);
    oscillator2.prepare(spec);
    
    oscillator1.reset();
    oscillator2.reset();
    

    filter.prepare(spec);
    filter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);
    filter.setCutoffFrequencyHz(1000.0f);
    filter.setResonance(0.1f);
    filter.reset();
    
    adsr.setSampleRate(sampleRate);
    setADSRParameters(20.0f, 100.0f, 0.7f, 200.0f);
    
    masterGain.prepare(spec);
    masterGain.setGainLinear(0.7f); 
}

void SynthVoice::configureOscillatorWaveform(juce::dsp::Oscillator<float>& oscillator, int type)
{
    switch (type)
    {
        case 0: // Sine wave - pure sine function
            oscillator.initialise([](float x) { return std::sin(x); });
            break;
            
        case 1: // Sawtooth wave - band-limited approximation
            oscillator.initialise([](float x) {
                constexpr int harmonics = 30;
                float result = 0.0f;
                
                for (int i = 1; i <= harmonics; ++i)
                    result += std::sin(i * x) / i;
                
                return result * (2.0f / juce::MathConstants<float>::pi);
            });
            break;
            
        case 2:
            oscillator.initialise([](float x) {
                constexpr int harmonics = 15;
                float result = 0.0f;
                
                for (int i = 1; i <= harmonics * 2; i += 2)
                    result += std::sin(i * x) / i;
                
                return result * (4.0f / juce::MathConstants<float>::pi);
            });
            break;
            
        case 3: // Triangle wave - band-limited approximation
            oscillator.initialise([](float x) {
                constexpr int harmonics = 15;
                float result = 0.0f;
                
                for (int i = 1; i <= harmonics * 2; i += 2)
                {
                    float harmonic = std::sin(i * x) / (i * i);
                    result += i % 4 == 1 ? harmonic : -harmonic;
                }
                
                return result * (8.0f / (juce::MathConstants<float>::pi * juce::MathConstants<float>::pi));
            });
            break;
            
        default:
            oscillator.initialise([](float x) { return std::sin(x); });
    }
    
    oscillator.reset();
}

void SynthVoice::setOscType(int type)
{
    configureOscillatorWaveform(oscillator1, type);
}

void SynthVoice::setOsc2Type(int type)
{
    configureOscillatorWaveform(oscillator2, type);
}

void SynthVoice::setOscMix(float mix)
{
    oscMix = mix;
}

void SynthVoice::setFilterEnabled(bool enabled)
{
    filterEnabled = enabled;
}

void SynthVoice::setFilterCutoff(float freq)
{
    filter.setCutoffFrequencyHz(freq);
}

void SynthVoice::setFilterResonance(float res)
{
    filter.setResonance(res);
}

void SynthVoice::setFilterMode(int mode)
{
    filter.setMode(static_cast<juce::dsp::LadderFilter<float>::Mode>(mode));
}

void SynthVoice::setADSRParameters(float attack, float decay, float sustain, float release)
{
    adsrParams.attack = std::max(5.0f, attack) / 1000.0f;    // Minimum 5ms attack
    adsrParams.decay = std::max(10.0f, decay) / 1000.0f;     // Minimum 10ms decay
    adsrParams.sustain = juce::jlimit(0.0f, 1.0f, sustain);  // Clamp to 0-1
    adsrParams.release = std::max(10.0f, release) / 1000.0f; // Minimum 10ms release
    
    adsr.setParameters(adsrParams);
}

void SynthVoice::setMasterGain(float gain)
{
    masterGain.setGainDecibels(gain);
}

void SynthVoice::getADSRParameters(float& attack, float& decay, float& sustain, float& release) const
{
    attack = adsrParams.attack * 1000.0f;  // Convert back to ms
    decay = adsrParams.decay * 1000.0f;    // Convert back to ms
    sustain = adsrParams.sustain;
    release = adsrParams.release * 1000.0f; // Convert back to ms
}

void SynthVoice::getADSRForAttack(float& decay, float& sustain, float& release) const
{
    decay = adsrParams.decay * 1000.0f;    // Convert back to ms
    sustain = adsrParams.sustain;
    release = adsrParams.release * 1000.0f; // Convert back to ms
}

void SynthVoice::getADSRForDecay(float& attack, float& sustain, float& release) const
{
    attack = adsrParams.attack * 1000.0f;  // Convert back to ms
    sustain = adsrParams.sustain;
    release = adsrParams.release * 1000.0f; // Convert back to ms
}

void SynthVoice::getADSRForSustain(float& attack, float& decay, float& release) const
{
    attack = adsrParams.attack * 1000.0f;  // Convert back to ms
    decay = adsrParams.decay * 1000.0f;    // Convert back to ms
    release = adsrParams.release * 1000.0f; // Convert back to ms
}

void SynthVoice::getADSRForRelease(float& attack, float& decay, float& sustain) const
{
    attack = adsrParams.attack * 1000.0f;  // Convert back to ms
    decay = adsrParams.decay * 1000.0f;    // Convert back to ms
    sustain = adsrParams.sustain;
}

MainProcessor::MainProcessor() :
    parameterManager(*this, ProjectInfo::projectName, ParameterInfos)
{
    synth.addSound(new SynthSound());
    
    for (int i = 0; i < numVoices; ++i)
    {
        synth.addVoice(new SynthVoice());
    }

    parameterManager.registerParameterCallback(Param::ID::OscType,
    [this] (float value, bool)
    {
        int type = static_cast<int>(std::floor(value));

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setOscType(type);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::Osc2Type,
    [this] (float value, bool)
    {
        int type = static_cast<int>(std::floor(value));
        
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setOsc2Type(type);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::OscMix,
    [this] (float value, bool)
    {

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setOscMix(value);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::FilterEnabled,
    [this] (float value, bool)
    {
        bool enabled = value > 0.5f;
        
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setFilterEnabled(enabled);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::FilterCutoff,
    [this] (float value, bool)
    {

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setFilterCutoff(value);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::FilterResonance,
    [this] (float value, bool)
    {

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setFilterResonance(value);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::FilterMode,
    [this] (float value, bool)
    {
        int mode = static_cast<int>(std::floor(value));
        
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setFilterMode(mode);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::Attack,
    [this] (float value, bool /*forced*/)
    {

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                float attack = value;
                float decay = 250.0f;
                float sustain = 0.8f;
                float release = 300.0f;
                

                if (i == 0)
                {
                    auto* firstVoice = dynamic_cast<SynthVoice*>(synth.getVoice(0));
                    if (firstVoice)
                    {
                        firstVoice->getADSRForAttack(decay, sustain, release);
                    }
                }
                voice->setADSRParameters(attack, decay, sustain, release);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::Decay,
    [this] (float value, bool /*forced*/)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                float attack = 100.0f;
                float decay = value;
                float sustain = 0.8f;
                float release = 300.0f;
                
                if (i == 0)
                {
                    auto* firstVoice = dynamic_cast<SynthVoice*>(synth.getVoice(0));
                    if (firstVoice)
                    {
                        firstVoice->getADSRForDecay(attack, sustain, release);
                    }
                }
                voice->setADSRParameters(attack, decay, sustain, release);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::Sustain,
    [this] (float value, bool /*forced*/)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                float attack = 100.0f;
                float decay = 250.0f;
                float sustain = value;
                float release = 300.0f;
                
                if (i == 0)
                {
                    auto* firstVoice = dynamic_cast<SynthVoice*>(synth.getVoice(0));
                    if (firstVoice)
                    {
                        firstVoice->getADSRForSustain(attack, decay, release);
                    }
                }
                voice->setADSRParameters(attack, decay, sustain, release);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::Release,
    [this] (float value, bool /*forced*/)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                float attack = 100.0f;  // Default values
                float decay = 250.0f;
                float sustain = 0.8f;
                float release = value;
                
                if (i == 0)
                {
                    auto* firstVoice = dynamic_cast<SynthVoice*>(synth.getVoice(0));
                    if (firstVoice)
                    {
                        firstVoice->getADSRForRelease(attack, decay, sustain);
                    }
                }
                voice->setADSRParameters(attack, decay, sustain, release);
            }
        }
    });
    
    parameterManager.registerParameterCallback(Param::ID::MasterGain,
    [this] (float value, bool)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            {
                voice->setMasterGain(value);
            }
        }
    });
}

MainProcessor::~MainProcessor()
{
}

void MainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }

    parameterManager.updateParameters(true);
}

void MainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    buffer.clear();
    
    parameterManager.updateParameters();
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if (channelData[sample] > 0.95f)
                channelData[sample] = 0.95f;
            else if (channelData[sample] < -0.95f)
                channelData[sample] = -0.95f;
        }
    }
}

void MainProcessor::releaseResources()
{
    // Nothing to do here
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
bool MainProcessor::acceptsMidi() const { return true; }
bool MainProcessor::producesMidi() const { return false; }
bool MainProcessor::isMidiEffect() const { return false; }
double MainProcessor::getTailLengthSeconds() const { return 0.0; }
int MainProcessor::getNumPrograms() { return 1; }
int MainProcessor::getCurrentProgram() { return 0; }
void MainProcessor::setCurrentProgram(int) { }
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
