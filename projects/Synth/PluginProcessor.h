#pragma once

#include <JuceHeader.h>
#include "Synth.h"

namespace Param
{
    namespace ID
    {
        static const juce::String OscillatorSawVol { "oscillator_saw_vol" };
        static const juce::String OscillatorTriVol { "oscillator_tri_vol" };
        static const juce::String OscillatorSinVol { "oscillator_sin_vol" };
        static const juce::String OscillatorVol { "oscillator_volume" };
        static const juce::String OutputVol { "output_vol" };

        static const juce::String VCA_AttTime { "vca_att_time" };
        static const juce::String VCA_DecayTime { "vca_decay_time" };
        static const juce::String VCA_Sustain { "vca_sustain_level" };
        static const juce::String VCA_RelTime { "vca_rel_time" };

        static const juce::String VCF_AttTime { "vcf_att_time" };
        static const juce::String VCF_DecayTime { "vcf_decay_time" };
        static const juce::String VCF_Sustain { "vcf_sustain" };
        static const juce::String VCF_RelTime { "vcf_rel_time" };

        static const juce::String VCF_LFOFreq { "vcf_lfo_freq" };
        static const juce::String VCF_LFOType { "vcf_lfo_type" };

        static const juce::String VCF_Cutoff { "vcf_cutoff" };
        static const juce::String VCF_Reso { "vcf_reso" };
        static const juce::String VCF_Type { "vcf_type" };
        static const juce::String VCF_EnvAmount { "vcf_env_amount" };
        static const juce::String VCF_LFOAmount { "vcf_lfo_amount" };
    }

    namespace Name
    {
        static const juce::String OscillatorSawVol { "Osc. Saw Vol." };
        static const juce::String OscillatorTriVol { "Osc. Tri Vol." };
        static const juce::String OscillatorSinVol { "Osc. Sin Vol." };
        static const juce::String OscillatorVol { "Osc. Vol." };
        static const juce::String OutputVol { "Output Vol." };

        static const juce::String VCA_AttTime { "VCA Attack Time" };
        static const juce::String VCA_DecayTime { "VCA Decay Time" };
        static const juce::String VCA_Sustain { "VCA Sustain" };
        static const juce::String VCA_RelTime { "VCA Release Time" };

        static const juce::String VCF_AttTime { "VCF Attack Time" };
        static const juce::String VCF_DecayTime { "VCF Decay Time" };
        static const juce::String VCF_Sustain { "VCF Sustain" };
        static const juce::String VCF_RelTime { "VCF Release Time" };

        static const juce::String VCF_LFOFreq { "VCF LFO Freq." };
        static const juce::String VCF_LFOType { "VCF LFO Type" };

        static const juce::String VCF_Cutoff { "VCF Cutoff" };
        static const juce::String VCF_Reso { "VCF Resonance" };
        static const juce::String VCF_Type { "VCF Type" };

        static const juce::String VCF_EnvAmount { "VCF Env. Amount" };
        static const juce::String VCF_LFOAmount { "VCF LFO Amount" };
    }

    namespace Ranges
    {
        static constexpr float VolMin { -60.f };
        static constexpr float VolMax { 12.f };
        static constexpr float VolInc { 0.1f };
        static constexpr float VolSkw { 2.8f };

        static constexpr float EnvTimeMin { 1.f };
        static constexpr float EnvTimeMax { 1000.f };
        static constexpr float EnvTimeInc { 1.f };
        static constexpr float EnvTimeSkw { 0.5f };

        static constexpr float EnvSustainMin { 0.f };
        static constexpr float EnvSustainMax { 1.f };
        static constexpr float EnvSustainInc { 0.001f };
        static constexpr float EnvSustainSkw { 1.f };

        static constexpr float LFOFreqMin { 0.1f };
        static constexpr float LFOFreqMax { 50.f };
        static constexpr float LFOFreqInc { 0.1f };
        static constexpr float LFOFreqSkw { 0.25f };

        static constexpr float FilterFreqMin { 20.0f };
        static constexpr float FilterFreqMax { 20000.f };
        static constexpr float FilterFreqInc { 1.f };
        static constexpr float FilterFreqSkw { 0.5f };

        static constexpr float FilterResoMin { 0.5f };
        static constexpr float FilterResoMax { 10.f };
        static constexpr float FilterResoInc { 0.01f };
        static constexpr float FilterResoSkw { 0.5f };

        static constexpr float AmountMin { -1.f };
        static constexpr float AmountMax { 1.f };
        static constexpr float AmountInc { 0.001f };
        static constexpr float AmountSkw { 1.f };

        static const juce::StringArray LFOType { "Sin", "Tri" };
        static const juce::StringArray FilterType { "Low Pass", "Band Pass", "High Pass" };
    }

    namespace Units
    {
        static const juce::String Hz { "Hz" };
        static const juce::String dB { "dB" };
        static const juce::String Ms { "ms" };
    }
}

class SynthAudioProcessor : public juce::AudioProcessor
{
public:
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParamManager() { return paramManager; }

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
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    //==============================================================================

    static constexpr size_t NUM_VOICES { 8 };

private:
    mrta::ParameterManager paramManager;
    std::vector<DSP::SynthVoice*> voices;
    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessor)
};