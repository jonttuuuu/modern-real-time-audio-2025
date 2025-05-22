#include "PluginEditor.h"

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    oscParamEditor(p.getParamManager(), PARAM_HEIGHT, { Param::ID::OscillatorSawVol, Param::ID::OscillatorTriVol, Param::ID::OscillatorSinVol, Param::ID::OscillatorVol, Param::ID::OutputVol }),
    vcaEnvParamEditor(p.getParamManager(), PARAM_HEIGHT, { Param::ID::VCA_AttTime, Param::ID::VCA_DecayTime, Param::ID::VCA_Sustain, Param::ID::VCA_RelTime }),
    vcfEnvParamEditor(p.getParamManager(), PARAM_HEIGHT, { Param::ID::VCF_AttTime, Param::ID::VCF_DecayTime, Param::ID::VCF_Sustain, Param::ID::VCF_RelTime }),
    lfoParamEditor(p.getParamManager(), PARAM_HEIGHT, { Param::ID::VCF_LFOFreq, Param::ID::VCF_LFOType }),
    filterParamEditor(p.getParamManager(), PARAM_HEIGHT, { Param::ID::VCF_Cutoff, Param::ID::VCF_Reso, Param::ID::VCF_Type, Param::ID::VCF_EnvAmount, Param::ID::VCF_LFOAmount }),
    oscLabel("", "Oscillators"),
    vcaEnvLabel("", "Amplitude Envelope"),
    vcfEnvLabel("", "Filter Envelope"),
    lfoLabel("", "Filter LFO"),
    filterLabel("", "Filter")
{
    addAndMakeVisible(oscParamEditor);
    addAndMakeVisible(vcaEnvParamEditor);
    addAndMakeVisible(vcfEnvParamEditor);
    addAndMakeVisible(lfoParamEditor);
    addAndMakeVisible(filterParamEditor);

    setupLabel(oscLabel);
    setupLabel(vcaEnvLabel);
    setupLabel(vcfEnvLabel);
    setupLabel(lfoLabel);
    setupLabel(filterLabel);

    setSize(NUM_SECTIONS * SECTION_WIDTH + (NUM_SECTIONS - 1) * SECTION_SPACER_WIDTH, LABEL_HEIGHT + PARAM_HEIGHT * MAX_PARAM_COUNT);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
}

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    for (int i = 1; i < NUM_SECTIONS; ++i)
        g.fillRect(i * (SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) - 1, 0, 2, getHeight());
}

void SynthAudioProcessorEditor::resized()
{
    auto bounds { getLocalBounds() };

    {
        auto secBounds { bounds.removeFromLeft(SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) };
        oscLabel.setBounds(secBounds.removeFromTop(LABEL_HEIGHT));
        oscParamEditor.setBounds(secBounds.withSizeKeepingCentre(SECTION_WIDTH, secBounds.getHeight()));
    }

    {
        auto secBounds { bounds.removeFromLeft(SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) };
        vcaEnvLabel.setBounds(secBounds.removeFromTop(LABEL_HEIGHT));
        vcaEnvParamEditor.setBounds(secBounds.withSizeKeepingCentre(SECTION_WIDTH, secBounds.getHeight()));
    }

    {
        auto secBounds { bounds.removeFromLeft(SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) };
        filterLabel.setBounds(secBounds.removeFromTop(LABEL_HEIGHT));
        filterParamEditor.setBounds(secBounds.withSizeKeepingCentre(SECTION_WIDTH, secBounds.getHeight()));
    }

    {
        auto secBounds { bounds.removeFromLeft(SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) };
        vcfEnvLabel.setBounds(secBounds.removeFromTop(LABEL_HEIGHT));
        vcfEnvParamEditor.setBounds(secBounds.withSizeKeepingCentre(SECTION_WIDTH, secBounds.getHeight()));
    }

    {
        auto secBounds { bounds.removeFromLeft(SECTION_WIDTH + SECTION_SPACER_WIDTH / 2) };
        lfoLabel.setBounds(secBounds.removeFromTop(LABEL_HEIGHT));
        lfoParamEditor.setBounds(secBounds.withSizeKeepingCentre(SECTION_WIDTH, secBounds.getHeight()));
    }
}

void SynthAudioProcessorEditor::setupLabel(juce::Label& label)
{
    label.setFont(juce::FontOptions((25.f)));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}