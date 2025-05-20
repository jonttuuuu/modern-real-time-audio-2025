#pragma once

#include <JuceHeader.h>

namespace Param
{
    namespace ID
    {
        static const juce::String OscType { "osc_type" };
        static const juce::String Osc2Type { "osc2_type" };
        static const juce::String OscMix { "osc_mix" };
        
        static const juce::String FilterEnabled { "filter_enabled" };
        static const juce::String FilterCutoff { "filter_cutoff" };
        static const juce::String FilterResonance { "filter_resonance" };
        static const juce::String FilterMode { "filter_mode" };

        static const juce::String Attack { "attack" };
        static const juce::String Decay { "decay" };
        static const juce::String Sustain { "sustain" };
        static const juce::String Release { "release" };
        
        static const juce::String MasterGain { "master_gain" };
    }

    namespace Name
    {

        static const juce::String OscType { "Oscillator 1 Type" };
        static const juce::String Osc2Type { "Oscillator 2 Type" };
        static const juce::String OscMix { "Oscillator Mix" };
        

        static const juce::String FilterEnabled { "Filter Enabled" };
        static const juce::String FilterCutoff { "Filter Cutoff" };
        static const juce::String FilterResonance { "Filter Resonance" };
        static const juce::String FilterMode { "Filter Mode" };
        
        static const juce::String Attack { "Attack" };
        static const juce::String Decay { "Decay" };
        static const juce::String Sustain { "Sustain" };
        static const juce::String Release { "Release" };
        

        static const juce::String MasterGain { "Master Gain" };
    }
}

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
    void setOscType(int type);
    void setOsc2Type(int type);
    void setOscMix(float mix);
    void setFilterEnabled(bool enabled);
    void setFilterCutoff(float freq);
    void setFilterResonance(float res);
    void setFilterMode(int mode);

    void setADSRParameters(float attack, float decay, float sustain, float release);    
    void getADSRParameters(float& attack, float& decay, float& sustain, float& release) const;
    void getADSRForAttack(float& decay, float& sustain, float& release) const;
    void getADSRForDecay(float& attack, float& sustain, float& release) const;
    void getADSRForSustain(float& attack, float& decay, float& release) const;
    void getADSRForRelease(float& attack, float& decay, float& sustain) const;
    void setMasterGain(float gain);
    
private:
    void configureOscillatorWaveform(juce::dsp::Oscillator<float>& oscillator, int type);

private:
    bool noteOn { false };
    int midiNote { 0 };
    float level { 0.0f };
    
    juce::dsp::Oscillator<float> oscillator1;
    juce::dsp::Oscillator<float> oscillator2;
    float oscMix { 0.5f };
    
    juce::dsp::LadderFilter<float> filter;
    bool filterEnabled { true };

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    juce::dsp::Gain<float> masterGain;
};

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() {}
    
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    //==============================================================================

private:
    mrta::ParameterManager parameterManager;
    juce::Synthesiser synth;
    const int numVoices { 8 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessor)
};
