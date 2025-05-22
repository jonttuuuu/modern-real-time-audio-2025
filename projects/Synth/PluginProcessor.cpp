#include "PluginProcessor.h"
#include "PluginEditor.h"

void setOscSawVol(std::vector<DSP::SynthVoice*> voices, float dB, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [dB, skipRamp] (auto& v) { v->setOscSawVol(dB, skipRamp); });
}

void setOscTriVol(std::vector<DSP::SynthVoice*> voices, float dB, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [dB, skipRamp] (auto& v) { v->setOscTriVol(dB, skipRamp); });
}

void setOscSinVol(std::vector<DSP::SynthVoice*> voices, float dB, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [dB, skipRamp] (auto& v) { v->setOscSinVol(dB, skipRamp); });
}

void setOscVol(std::vector<DSP::SynthVoice*> voices, float dB, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [dB, skipRamp] (auto& v) { v->setOscVol(dB, skipRamp); });
}

void setAttTimeVCA(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setAttTimeVCA(ms); });
}

void setDecayTimeVCA(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setDecayTimeVCA(ms); });
}

void setSustainVCA(std::vector<DSP::SynthVoice*> voices, float norm)
{
    std::for_each(voices.begin(), voices.end(), [norm] (auto& v) { v->setSustainVCA(norm); });
}

void setRelTimeVCA(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setRelTimeVCA(ms); });
}

void setAttTimeVCF(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setAttTimeVCF(ms); });
}

void setDecayTimeVCF(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setDecayTimeVCF(ms); });
}

void setSustainVCF(std::vector<DSP::SynthVoice*> voices, float norm)
{
    std::for_each(voices.begin(), voices.end(), [norm] (auto& v) { v->setSustainVCF(norm); });
}

void setRelTimeVCF(std::vector<DSP::SynthVoice*> voices, float ms)
{
    std::for_each(voices.begin(), voices.end(), [ms] (auto& v) { v->setRelTimeVCF(ms); });
}

void setLFOFreqVCF(std::vector<DSP::SynthVoice*> voices, float Hz)
{
    std::for_each(voices.begin(), voices.end(), [Hz] (auto& v) { v->setLFOFreqVCF(Hz); });
}

void setLFOTypeVCF(std::vector<DSP::SynthVoice*> voices, DSP::SynthVoice::LFOType type)
{
    std::for_each(voices.begin(), voices.end(), [type] (auto& v) { v->setLFOTypeVCF(type); });
}

void setEnvAmountVCF(std::vector<DSP::SynthVoice*> voices, float bipolar, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [bipolar, skipRamp] (auto& v) { v->setEnvAmountVCF(bipolar, skipRamp); });
}

void setLFOAmountVCF(std::vector<DSP::SynthVoice*> voices, float bipolar, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [bipolar, skipRamp] (auto& v) { v->setLFOAmountVCF(bipolar, skipRamp); });
}

void setFilterCutoff(std::vector<DSP::SynthVoice*> voices, float Hz, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [Hz, skipRamp] (auto& v) { v->setFilterCutoff(Hz, skipRamp); });
}

void setFilterReso(std::vector<DSP::SynthVoice*> voices, float Q, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [Q, skipRamp] (auto& v) { v->setFilterReso(Q, skipRamp); });
}

void setFilterType(std::vector<DSP::SynthVoice*> voices, DSP::SynthVoice::FilterType type, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [type, skipRamp] (auto& v) { v->setFilterType(type, skipRamp); });
}

void setOutputVol(std::vector<DSP::SynthVoice*> voices, float dB, bool skipRamp)
{
    std::for_each(voices.begin(), voices.end(), [dB, skipRamp] (auto& v) { v->setOutputVol(dB, skipRamp); });
}

static const std::vector<mrta::ParameterInfo> paramVector
{
    { Param::ID::OscillatorSawVol, Param::Name::OscillatorSawVol, Param::Units::dB, -12.f, Param::Ranges::VolMin, Param::Ranges::VolMax, Param::Ranges::VolInc, Param::Ranges::VolSkw },
    { Param::ID::OscillatorTriVol, Param::Name::OscillatorTriVol, Param::Units::dB, -12.f, Param::Ranges::VolMin, Param::Ranges::VolMax, Param::Ranges::VolInc, Param::Ranges::VolSkw },
    { Param::ID::OscillatorSinVol, Param::Name::OscillatorSinVol, Param::Units::dB, -12.f, Param::Ranges::VolMin, Param::Ranges::VolMax, Param::Ranges::VolInc, Param::Ranges::VolSkw },
    { Param::ID::OscillatorVol,    Param::Name::OscillatorVol,    Param::Units::dB,   0.f, Param::Ranges::VolMin, Param::Ranges::VolMax, Param::Ranges::VolInc, Param::Ranges::VolSkw },

    { Param::ID::VCA_AttTime,   Param::Name::VCA_AttTime,   Param::Units::Ms,  50.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },
    { Param::ID::VCA_DecayTime, Param::Name::VCA_DecayTime, Param::Units::Ms,  10.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },
    { Param::ID::VCA_Sustain,   Param::Name::VCA_Sustain,   "",                 0.7f, Param::Ranges::EnvSustainMin, Param::Ranges::EnvSustainMax, Param::Ranges::EnvSustainInc, Param::Ranges::EnvSustainSkw },
    { Param::ID::VCA_RelTime,   Param::Name::VCA_RelTime,   Param::Units::Ms, 100.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },

    { Param::ID::VCF_AttTime,   Param::Name::VCF_AttTime,   Param::Units::Ms,  10.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },
    { Param::ID::VCF_DecayTime, Param::Name::VCF_DecayTime, Param::Units::Ms, 100.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },
    { Param::ID::VCF_Sustain,   Param::Name::VCF_Sustain,   "",                 0.9f, Param::Ranges::EnvSustainMin, Param::Ranges::EnvSustainMax, Param::Ranges::EnvSustainInc, Param::Ranges::EnvSustainSkw },
    { Param::ID::VCF_RelTime,  Param::Name::VCF_RelTime,   Param::Units::Ms, 100.0f, Param::Ranges::EnvTimeMin,    Param::Ranges::EnvTimeMax,    Param::Ranges::EnvTimeInc,    Param::Ranges::EnvTimeSkw },

    { Param::ID::VCF_LFOFreq, Param::Name::VCF_LFOFreq, Param::Units::Hz, 0.5f, Param::Ranges::LFOFreqMin, Param::Ranges::LFOFreqMax, Param::Ranges::LFOFreqInc, Param::Ranges::LFOFreqSkw },
    { Param::ID::VCF_LFOType, Param::Name::VCF_LFOType, Param::Ranges::LFOType, 0 },

    { Param::ID::VCF_Cutoff, Param::Name::VCF_Cutoff, Param::Units::Hz, 2000.f, Param::Ranges::FilterFreqMin, Param::Ranges::FilterFreqMax, Param::Ranges::FilterFreqInc, Param::Ranges::FilterFreqSkw },
    { Param::ID::VCF_Reso,   Param::Name::VCF_Reso,   "",                0.71f, Param::Ranges::FilterResoMin, Param::Ranges::FilterResoMax, Param::Ranges::FilterResoInc, Param::Ranges::FilterResoSkw },
    { Param::ID::VCF_Type,   Param::Name::VCF_Type,   Param::Ranges::FilterType, 0 },

    { Param::ID::VCF_EnvAmount, Param::Name::VCF_EnvAmount, "", 0.f, Param::Ranges::AmountMin, Param::Ranges::AmountMax, Param::Ranges::AmountInc, Param::Ranges::AmountSkw },
    { Param::ID::VCF_LFOAmount, Param::Name::VCF_LFOAmount, "", 0.f, Param::Ranges::AmountMin, Param::Ranges::AmountMax, Param::Ranges::AmountInc, Param::Ranges::AmountSkw },

    { Param::ID::OutputVol, Param::Name::OutputVol, Param::Units::dB, 0.f, Param::Ranges::VolMin, Param::Ranges::VolMax, Param::Ranges::VolInc, Param::Ranges::VolSkw }
};

SynthAudioProcessor::SynthAudioProcessor() :
    paramManager(*this, ProjectInfo::projectName, paramVector)
{
    synth.addSound(new DSP::SynthSound());
    for (size_t i = 0; i < NUM_VOICES; ++i)
    {
        voices.emplace_back(new DSP::SynthVoice());
        synth.addVoice(voices.back());
    }
    synth.setNoteStealingEnabled(false);

    paramManager.registerParameterCallback(Param::ID::OscillatorSawVol, [this] (float value, bool force) { setOscSawVol(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::OscillatorTriVol, [this] (float value, bool force) { setOscTriVol(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::OscillatorSinVol, [this] (float value, bool force) { setOscSinVol(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::OscillatorVol, [this] (float value, bool force) { setOscVol(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::VCA_AttTime, [this] (float value, bool force) { setAttTimeVCA(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCA_DecayTime, [this] (float value, bool force) { setDecayTimeVCA(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCA_Sustain, [this] (float value, bool force) { setSustainVCA(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCA_RelTime, [this] (float value, bool force) { setRelTimeVCA(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_AttTime, [this] (float value, bool force) { setAttTimeVCF(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_DecayTime, [this] (float value, bool force) { setDecayTimeVCF(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_Sustain, [this] (float value, bool force) { setSustainVCF(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_RelTime, [this] (float value, bool force) { setRelTimeVCF(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_LFOFreq, [this] (float value, bool force) { setLFOFreqVCF(voices, value); });
    paramManager.registerParameterCallback(Param::ID::VCF_LFOType, [this] (float value, bool force) { setLFOTypeVCF(voices, static_cast<DSP::SynthVoice::LFOType>(std::round(value))); });
    paramManager.registerParameterCallback(Param::ID::VCF_Cutoff, [this] (float value, bool force) { setFilterCutoff(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::VCF_Reso, [this] (float value, bool force) { setFilterReso(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::VCF_Type, [this] (float value, bool force) { setFilterType(voices, static_cast<DSP::SynthVoice::FilterType>(std::round(value)), force); });
    paramManager.registerParameterCallback(Param::ID::VCF_EnvAmount, [this] (float value, bool force) { setEnvAmountVCF(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::VCF_LFOAmount, [this] (float value, bool force) { setLFOAmountVCF(voices, value, force); });
    paramManager.registerParameterCallback(Param::ID::OutputVol, [this] (float value, bool force) { setOutputVol(voices, value, force); });
}

SynthAudioProcessor::~SynthAudioProcessor()
{
}

void SynthAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    paramManager.updateParameters(true);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioProcessor::releaseResources()
{
}

void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    paramManager.updateParameters();

    buffer.clear();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    paramManager.getStateInformation(destData);
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    paramManager.setStateInformation(data, sizeInBytes);
}

bool SynthAudioProcessor::acceptsMidi() const
{
    return true;
}

//==============================================================================
const juce::String SynthAudioProcessor::getName() const { return JucePlugin_Name; }
bool SynthAudioProcessor::producesMidi() const { return false; }
bool SynthAudioProcessor::isMidiEffect() const { return false; }
double SynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SynthAudioProcessor::getNumPrograms() { return 1; }
int SynthAudioProcessor::getCurrentProgram() { return 0; }
void SynthAudioProcessor::setCurrentProgram(int) { }
const juce::String SynthAudioProcessor::getProgramName(int) { return {}; }
void SynthAudioProcessor::changeProgramName(int, const juce::String&) { }
bool SynthAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* SynthAudioProcessor::createEditor() { return new SynthAudioProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}