/*
  ==============================================================================
    DelayWithEQ - Delay plugin with EQ only on the wet/delayed signal
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayWithEQAudioProcessor::DelayWithEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    delayTimeParam  = apvts.getRawParameterValue ("delayTime");
    feedbackParam   = apvts.getRawParameterValue ("feedback");
    mixParam        = apvts.getRawParameterValue ("mix");

    lowShelfFreq    = apvts.getRawParameterValue ("lowShelfFreq");
    lowShelfGain    = apvts.getRawParameterValue ("lowShelfGain");
    peakFreq        = apvts.getRawParameterValue ("peakFreq");
    peakGain        = apvts.getRawParameterValue ("peakGain");
    peakQ           = apvts.getRawParameterValue ("peakQ");
    highShelfFreq   = apvts.getRawParameterValue ("highShelfFreq");
    highShelfGain   = apvts.getRawParameterValue ("highShelfGain");
}

DelayWithEQAudioProcessor::~DelayWithEQAudioProcessor()
{
}

//==============================================================================
const juce::String DelayWithEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayWithEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayWithEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayWithEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayWithEQAudioProcessor::getTailLengthSeconds() const
{
    return 4.0; // max delay time
}

int DelayWithEQAudioProcessor::getNumPrograms()
{
    return 1;
}

int DelayWithEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayWithEQAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String DelayWithEQAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void DelayWithEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void DelayWithEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = 2;

    delayLine.prepare (spec);
    delayLine.reset();
    delayLine.setMaximumDelayInSamples (static_cast<int> (sampleRate * 4.0)); // 4 seconds max

    // Smoothing (50 ms)
    delayTimeSmoothed.reset (sampleRate, 0.05);
    feedbackSmoothed.reset  (sampleRate, 0.05);
    mixSmoothed.reset       (sampleRate, 0.05);

    // EQ filters
    lowShelfL.prepare (spec);   lowShelfR.prepare (spec);
    peakL.prepare (spec);       peakR.prepare (spec);
    highShelfL.prepare (spec);  highShelfR.prepare (spec);

    lowShelfL.reset();   lowShelfR.reset();
    peakL.reset();       peakR.reset();
    highShelfL.reset();  highShelfR.reset();

    wetBuffer.setSize (2, samplesPerBlock);
    wetBuffer.clear();

    updateEQCoefficients();
}

void DelayWithEQAudioProcessor::releaseResources()
{
    // When playback stops, release resources
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayWithEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayWithEQAudioProcessor::updateEQCoefficients()
{
    const double sr = getSampleRate();
    if (sr <= 0.0)
        return;

    // Low Shelf
    *lowShelfL.coefficients = *Coefficients::makeLowShelf (
        sr,
        static_cast<float> (*lowShelfFreq),
        0.707f,
        juce::Decibels::decibelsToGain (static_cast<float> (*lowShelfGain)));
    *lowShelfR.coefficients = *lowShelfL.coefficients;

    // Peak / Bell
    *peakL.coefficients = *Coefficients::makePeakFilter (
        sr,
        static_cast<float> (*peakFreq),
        static_cast<float> (*peakQ),
        juce::Decibels::decibelsToGain (static_cast<float> (*peakGain)));
    *peakR.coefficients = *peakL.coefficients;

    // High Shelf
    *highShelfL.coefficients = *Coefficients::makeHighShelf (
        sr,
        static_cast<float> (*highShelfFreq),
        0.707f,
        juce::Decibels::decibelsToGain (static_cast<float> (*highShelfGain)));
    *highShelfR.coefficients = *highShelfL.coefficients;
}

void DelayWithEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = juce::jmin (buffer.getNumChannels(), 2);

    // Clear extra channels if any
    for (auto i = numChannels; i < buffer.getNumChannels(); ++i)
        buffer.clear (i, 0, numSamples);

    // Update EQ coefficients (can be optimized later to only when params change)
    updateEQCoefficients();

    // Update smoothed values
    delayTimeSmoothed.setTargetValue (static_cast<float> (*delayTimeParam) * 0.001f * static_cast<float> (getSampleRate()));
    feedbackSmoothed.setTargetValue  (static_cast<float> (*feedbackParam));
    mixSmoothed.setTargetValue       (static_cast<float> (*mixParam));

    // Copy dry signal into wetBuffer (we will overwrite it with delayed signal)
    wetBuffer.makeCopyOf (buffer, true);

    //==============================================================================
    // Process WET path only: Delay + Feedback + EQ
    //==============================================================================
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* wet = wetBuffer.getWritePointer (channel);

        auto& filterLow  = (channel == 0) ? lowShelfL  : lowShelfR;
        auto& filterPeak = (channel == 0) ? peakL      : peakR;
        auto& filterHigh = (channel == 0) ? highShelfL : highShelfR;

        for (int i = 0; i < numSamples; ++i)
        {
            const float delayInSamples = delayTimeSmoothed.getNextValue();
            const float feedback       = feedbackSmoothed.getNextValue();

            // Read delayed sample
            float delayedSample = delayLine.popSample (channel, delayInSamples);

            // Apply EQ ONLY to the delayed signal
            delayedSample = filterLow.processSample  (delayedSample);
            delayedSample = filterPeak.processSample (delayedSample);
            delayedSample = filterHigh.processSample (delayedSample);

            // Write into delay line: current input + feedback of filtered delay
            const float inputSample = wet[i];
            delayLine.pushSample (channel, inputSample + delayedSample * feedback);

            // Store the processed delayed sample
            wet[i] = delayedSample;
        }
    }

    //==============================================================================
    // Mix Dry + Wet
    //==============================================================================
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* dry = buffer.getWritePointer (channel);
        auto* wet = wetBuffer.getReadPointer (channel);

        for (int i = 0; i < numSamples; ++i)
        {
            const float mix = mixSmoothed.getNextValue();
            dry[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
        }
    }
}

//==============================================================================
bool DelayWithEQAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DelayWithEQAudioProcessor::createEditor()
{
    return new DelayWithEQAudioProcessorEditor (*this);
}

//==============================================================================
void DelayWithEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DelayWithEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout DelayWithEQAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Delay parameters
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delayTime", 1 },
        "Delay Time",
        juce::NormalisableRange<float> (1.0f, 2000.0f, 0.1f, 0.4f),
        350.0f,
        juce::AudioParameterFloatAttributes().withLabel ("ms")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "feedback", 1 },
        "Feedback",
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f),
        0.40f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.35f));

    // EQ - Low Shelf
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "lowShelfFreq", 1 },
        "Low Shelf Freq",
        juce::NormalisableRange<float> (30.0f, 500.0f, 1.0f, 0.3f),
        120.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "lowShelfGain", 1 },
        "Low Shelf Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    // EQ - Peak
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "peakFreq", 1 },
        "Peak Freq",
        juce::NormalisableRange<float> (200.0f, 5000.0f, 1.0f, 0.3f),
        1000.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "peakGain", 1 },
        "Peak Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "peakQ", 1 },
        "Peak Q",
        juce::NormalisableRange<float> (0.1f, 10.0f, 0.01f, 0.3f),
        0.707f));

    // EQ - High Shelf
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "highShelfFreq", 1 },
        "High Shelf Freq",
        juce::NormalisableRange<float> (2000.0f, 16000.0f, 1.0f, 0.3f),
        8000.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "highShelfGain", 1 },
        "High Shelf Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayWithEQAudioProcessor();
}
