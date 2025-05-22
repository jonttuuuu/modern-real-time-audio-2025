#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MeterComponent.h"
#include "MrtaLAF.h"

class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor(DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    static constexpr int METER_WIDTH { 40 };

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DelayAudioProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;
    GUI::MeterComponent meterComponent;
    GUI::MrtaLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};