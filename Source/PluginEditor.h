/*
  ==============================================================================
    DelayWithEQ - Editor
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class DelayWithEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    DelayWithEQAudioProcessorEditor (DelayWithEQAudioProcessor&);
    ~DelayWithEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DelayWithEQAudioProcessor& audioProcessor;

    //==============================================================================
    // Delay controls
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider mixSlider;

    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label mixLabel;

    // EQ controls
    juce::Slider lowShelfFreqSlider, lowShelfGainSlider;
    juce::Slider peakFreqSlider, peakGainSlider, peakQSlider;
    juce::Slider highShelfFreqSlider, highShelfGainSlider;

    juce::Label lowShelfFreqLabel, lowShelfGainLabel;
    juce::Label peakFreqLabel, peakGainLabel, peakQLabel;
    juce::Label highShelfFreqLabel, highShelfGainLabel;

    juce::Label delayTitle;
    juce::Label eqTitle;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> delayTimeAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    std::unique_ptr<SliderAttachment> lowShelfFreqAttachment;
    std::unique_ptr<SliderAttachment> lowShelfGainAttachment;
    std::unique_ptr<SliderAttachment> peakFreqAttachment;
    std::unique_ptr<SliderAttachment> peakGainAttachment;
    std::unique_ptr<SliderAttachment> peakQAttachment;
    std::unique_ptr<SliderAttachment> highShelfFreqAttachment;
    std::unique_ptr<SliderAttachment> highShelfGainAttachment;

    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayWithEQAudioProcessorEditor)
};
