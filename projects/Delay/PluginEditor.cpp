#include "MeterComponent.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager()),
    meterComponent(audioProcessor.getMeter())
{
    unsigned int numParams { static_cast<unsigned int>(audioProcessor.getParameterManager().getParameters().size()) };
    unsigned int paramHeight { static_cast<unsigned int>(genericParameterEditor.parameterWidgetHeight) };

    addAndMakeVisible(meterComponent);
    addAndMakeVisible(genericParameterEditor);
    genericParameterEditor.setLookAndFeel(&laf);
    setSize(300 + METER_WIDTH, numParams * paramHeight);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    genericParameterEditor.setLookAndFeel(nullptr);
}

void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DelayAudioProcessorEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    meterComponent.setBounds(area.removeFromRight(METER_WIDTH));
    genericParameterEditor.setBounds(area);
}