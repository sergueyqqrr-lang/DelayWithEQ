/*
  ==============================================================================
    DelayWithEQ - Delay plugin with EQ only on the wet/delayed signal
    Built with JUCE
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class DelayWithEQAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayWithEQAudioProcessor();
    ~DelayWithEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    // Delay
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 192000 };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> delayTimeSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> feedbackSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmoothed;

    // EQ only on wet signal (3 bands)
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;

    Filter lowShelfL, lowShelfR;
    Filter peakL, peakR;
    Filter highShelfL, highShelfR;

    // Temporary buffer for wet signal
    juce::AudioBuffer<float> wetBuffer;

    // Parameter pointers
    std::atomic<float>* delayTimeParam  = nullptr;
    std::atomic<float>* feedbackParam   = nullptr;
    std::atomic<float>* mixParam        = nullptr;

    std::atomic<float>* lowShelfFreq    = nullptr;
    std::atomic<float>* lowShelfGain    = nullptr;
    std::atomic<float>* peakFreq        = nullptr;
    std::atomic<float>* peakGain        = nullptr;
    std::atomic<float>* peakQ           = nullptr;
    std::atomic<float>* highShelfFreq   = nullptr;
    std::atomic<float>* highShelfGain   = nullptr;

    void updateEQCoefficients();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayWithEQAudioProcessor)
};
