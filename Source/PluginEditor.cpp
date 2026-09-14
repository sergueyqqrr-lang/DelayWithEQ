/*
  ==============================================================================
    DelayWithEQ - Editor
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayWithEQAudioProcessorEditor::DelayWithEQAudioProcessorEditor (DelayWithEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (520, 420);

    // Titles
    delayTitle.setText ("DELAY", juce::dontSendNotification);
    delayTitle.setFont (juce::Font (18.0f, juce::Font::bold));
    delayTitle.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (delayTitle);

    eqTitle.setText ("EQ (only on Delay)", juce::dontSendNotification);
    eqTitle.setFont (juce::Font (18.0f, juce::Font::bold));
    eqTitle.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (eqTitle);

    // Delay sliders
    setupSlider (delayTimeSlider, delayTimeLabel, "Time (ms)");
    setupSlider (feedbackSlider,  feedbackLabel,  "Feedback");
    setupSlider (mixSlider,       mixLabel,       "Mix");

    // EQ sliders
    setupSlider (lowShelfFreqSlider,  lowShelfFreqLabel,  "Low Freq");
    setupSlider (lowShelfGainSlider,  lowShelfGainLabel,  "Low Gain");
    setupSlider (peakFreqSlider,      peakFreqLabel,      "Peak Freq");
    setupSlider (peakGainSlider,      peakGainLabel,      "Peak Gain");
    setupSlider (peakQSlider,         peakQLabel,         "Peak Q");
    setupSlider (highShelfFreqSlider, highShelfFreqLabel, "High Freq");
    setupSlider (highShelfGainSlider, highShelfGainLabel, "High Gain");

    // Attachments
    delayTimeAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "delayTime", delayTimeSlider);
    feedbackAttachment  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "feedback",  feedbackSlider);
    mixAttachment       = std::make_unique<SliderAttachment> (audioProcessor.apvts, "mix",       mixSlider);

    lowShelfFreqAttachment  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "lowShelfFreq",  lowShelfFreqSlider);
    lowShelfGainAttachment  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "lowShelfGain",  lowShelfGainSlider);
    peakFreqAttachment      = std::make_unique<SliderAttachment> (audioProcessor.apvts, "peakFreq",      peakFreqSlider);
    peakGainAttachment      = std::make_unique<SliderAttachment> (audioProcessor.apvts, "peakGain",      peakGainSlider);
    peakQAttachment         = std::make_unique<SliderAttachment> (audioProcessor.apvts, "peakQ",         peakQSlider);
    highShelfFreqAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "highShelfFreq", highShelfFreqSlider);
    highShelfGainAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "highShelfGain", highShelfGainSlider);
}

DelayWithEQAudioProcessorEditor::~DelayWithEQAudioProcessorEditor()
{
}

//==============================================================================
void DelayWithEQAudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    addAndMakeVisible (label);
}

//==============================================================================
void DelayWithEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1e1e2e));

    // Section backgrounds
    g.setColour (juce::Colour (0xff2a2a3e));
    g.fillRoundedRectangle (15.0f, 40.0f, 490.0f, 120.0f, 8.0f);   // Delay section
    g.fillRoundedRectangle (15.0f, 190.0f, 490.0f, 210.0f, 8.0f);  // EQ section
}

void DelayWithEQAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    // Titles
    delayTitle.setBounds (bounds.removeFromTop (25));
    bounds.removeFromTop (10);

    // Delay row
    auto delayRow = bounds.removeFromTop (100);
    const int delaySliderWidth = delayRow.getWidth() / 3;

    delayTimeSlider.setBounds (delayRow.removeFromLeft (delaySliderWidth).reduced (15));
    feedbackSlider.setBounds  (delayRow.removeFromLeft (delaySliderWidth).reduced (15));
    mixSlider.setBounds       (delayRow.reduced (15));

    bounds.removeFromTop (25);
    eqTitle.setBounds (bounds.removeFromTop (25));
    bounds.removeFromTop (10);

    // EQ first row (Low + Peak)
    auto eqRow1 = bounds.removeFromTop (90);
    const int eqSliderWidth = eqRow1.getWidth() / 4;

    lowShelfFreqSlider.setBounds (eqRow1.removeFromLeft (eqSliderWidth).reduced (10));
    lowShelfGainSlider.setBounds (eqRow1.removeFromLeft (eqSliderWidth).reduced (10));
    peakFreqSlider.setBounds     (eqRow1.removeFromLeft (eqSliderWidth).reduced (10));
    peakGainSlider.setBounds     (eqRow1.reduced (10));

    bounds.removeFromTop (15);

    // EQ second row (Peak Q + High)
    auto eqRow2 = bounds.removeFromTop (90);
    peakQSlider.setBounds         (eqRow2.removeFromLeft (eqSliderWidth).reduced (10));
    highShelfFreqSlider.setBounds (eqRow2.removeFromLeft (eqSliderWidth).reduced (10));
    highShelfGainSlider.setBounds (eqRow2.removeFromLeft (eqSliderWidth).reduced (10));
}
