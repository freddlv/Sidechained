/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SChainAudioProcessor::SChainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (size_t i = 0; i < totalPoints; i++)
    {
        juce::String pStr_x = "Px";
        pStr_x += i;
        juce::String pStr_y = "Py";
        pStr_y += i;
        pointParametersX.push_back(apvts.getParameter(pStr_x));
        pointParametersY.push_back(apvts.getParameter(pStr_y));
    }
    triggerChoiceParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Trigger"));
    timeChoiceParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("TimeInterval"));
    // Initialize Points
    // Will connect points using 3rd order bezier curves

    setPointAtID(0, Point(0.0f, 0.0f));
    setPointAtID(1, Point(0.1f, 0.0f));
    setPointAtID(2, Point(0.1f, 1.0f));
    setPointAtID(3, Point(0.73f, 1.0f));
    setPointAtID(4, Point(0.90f, 1.0f));
    setPointAtID(5, Point(0.93f, 1.0f));
    setPointAtID(6, Point(0.95f, 1.0f));
    setPointAtID(7, Point(1.0f, 0.0f));
}

SChainAudioProcessor::~SChainAudioProcessor()
{
}

//==============================================================================
const juce::String SChainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SChainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SChainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SChainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SChainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SChainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SChainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SChainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SChainAudioProcessor::getProgramName (int index)
{
    return {};
}

void SChainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SChainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    curSamplesProcessedPerChannel.resize(getTotalNumInputChannels());
    for (int i = 0; i < curSamplesProcessedPerChannel.size(); i++) {
        curSamplesProcessedPerChannel[i] = 0;
    }
}

void SChainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.


}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SChainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SChainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float duration = 0.25f;

    int timeIntervalChoice = timeChoiceParam->getIndex();
    if (timeIntervalChoice < SettingsParameters::timeSettings.size() && timeIntervalChoice > 0) {
        duration = SettingsParameters::timeSettings[timeIntervalChoice].duration;
    }
    juce::int64 currentPositionInSamples = 0;
    float bpm = 120;

    int numTimeSig = 4;
    playHead = this->getPlayHead();
    if (playHead != nullptr) {
        playHead->getCurrentPosition(currentPositionInfo);
        currentPositionInSamples = currentPositionInfo.timeInSamples;
        bpm = static_cast<float>(currentPositionInfo.bpm);
        numTimeSig = currentPositionInfo.timeSigNumerator;
    }
    float beatDurationSecs = 60.0f / bpm; 

    double sampleRate = getSampleRate();
    if (sampleRate == 0) {
        sampleRate = 44100;
    }
    const juce::int64 samplesPerBeat = static_cast<juce::int64>(beatDurationSecs * sampleRate);
    const juce::int64 samplesPerBar = samplesPerBeat * numTimeSig;
    juce::int64 schainLengthInSamples = static_cast<size_t>(static_cast<double>(samplesPerBar) * duration);
    const juce::int64 totalSamples = buffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < totalSamples; i++)
        {
            float t = NormalizeTimeInterval(0, schainLengthInSamples, curSamplesProcessedPerChannel[channel]);
            float gainFactor = getGainBezierValue(t);
            if (gainFactor > 1.0f) {
                gainFactor = 1.0f;
            }
            else if (gainFactor < 0.0f) {
                gainFactor = 0.0f;
            }
            else if(gainFactor)
            channelData[i] *= gainFactor;
            curSamplesProcessedPerChannel[channel]++;
            curSamplesProcessedPerChannel[channel] = currentPositionInSamples % schainLengthInSamples;
        }
    }
}

juce::int64 SChainAudioProcessor::getSidechainSampleLength() {
    float bpm = 120;
    int numTimeSig = 4;
    playHead = this->getPlayHead();
    if (playHead != nullptr) {
        playHead->getCurrentPosition(currentPositionInfo);
        bpm = static_cast<float>(currentPositionInfo.bpm);
        numTimeSig = currentPositionInfo.timeSigNumerator;
    }
    float duration = 0.25f;
    const float beatDurationSecs = 60.0f / bpm;
    const juce::int64 samplesPerBeat = static_cast<juce::int64>(beatDurationSecs * getSampleRate());
    const juce::int64 samplesPerBar = samplesPerBeat * numTimeSig;
    const juce::int64 schainLengthInSamples = static_cast<size_t>(static_cast<double>(samplesPerBar) * duration);
    return schainLengthInSamples;
}

size_t SChainAudioProcessor::getSamplesPerBeat() 
{
    int bpm = 120;
    playHead = this->getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo curPosInfo;
    if (playHead != nullptr) {
        playHead->getCurrentPosition(curPosInfo);
        bpm = static_cast<float>(curPosInfo.bpm);
    }
    float beatDurationSecs = 60.0f / bpm;
    size_t samplesPerBeat = static_cast<size_t>(beatDurationSecs * getSampleRate());
    return samplesPerBeat;
}
void SChainAudioProcessor::resetCurSamplesProcessedPerChannel(size_t channelID)
{
    size_t currentPositionInSamples = 0;
    float bpm = 120;
    double sampleRate = getSampleRate();

    playHead = this->getPlayHead();
    if (playHead != nullptr) {
        playHead->getCurrentPosition(currentPositionInfo);
        currentPositionInSamples = currentPositionInfo.timeInSamples;
        bpm = static_cast<float>(currentPositionInfo.bpm);
    }
    float beatDurationSecs = 60.0f / bpm;
    size_t samplesPerBeat = static_cast<size_t>(beatDurationSecs * sampleRate);
    size_t sampleValue = currentPositionInSamples % samplesPerBeat;
    curSamplesProcessedPerChannel[channelID] = sampleValue;
}
//==============================================================================
bool SChainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SChainAudioProcessor::createEditor()
{
    return new SChainAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SChainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SChainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto vTree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (vTree.isValid()) {
        apvts.replaceState(vTree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SChainAudioProcessor();
}



void SChainAudioProcessor::addParameterPoints(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    for (size_t i = 0; i < totalPoints; i++)
    {
        juce::String pStr_x = "Px";
        pStr_x += i;
        juce::String pStr_y = "Py";
        pStr_y += i;
        //x coordinate
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            pStr_x, pStr_x,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f));
        //y coordinate
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            pStr_y, pStr_y,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), 0.0f));

    }
}


Point SChainAudioProcessor::getPointAtID(size_t pointID) {

    jassert(pointID < pointParametersX.size());
    float x = pointParametersX[pointID]->getValue();
    float y = pointParametersY[pointID]->getValue();
    Point p(x, y);
    return p;
}
bool SChainAudioProcessor::setPointAtID(size_t pointID, Point point) {
    jassert(pointID < pointParametersX.size());
    pointParametersX[pointID]->setValue(point.X);
    pointParametersY[pointID]->setValue(point.Y);
    return true;

}
float SChainAudioProcessor::getGainBezierValue(float t)
{
    jassert(t >= 0 && t <= 1);
    jassert(t != NAN);
    //Movable Points
    Point leftMostAnchor = getPointAtID(static_cast<size_t>(Points::leftAnchor)); //fixedx, variable y
    Point rightMostAnchor = getPointAtID(static_cast<size_t>(Points::rightAnchor)); //fixedx, variable y

    Point lineStartPoint = getPointAtID(static_cast<size_t>(Points::LineStart));
    Point lineEndPoint = getPointAtID(static_cast<size_t>(Points::LineEnd));

    //Control Points
    Point controlLeft1 = getPointAtID(static_cast<size_t>(Points::Control1));
    Point controlLeft2 = getPointAtID(static_cast<size_t>(Points::Control2));
    Point controlRight1 = getPointAtID(static_cast<size_t>(Points::Control3));
    Point controlRight2 = getPointAtID(static_cast<size_t>(Points::Control4));
    float gain = 0.0;
    //Attack part of bezier curve:
    if (t > leftMostAnchor.X && t < lineStartPoint.X) // Attack part
    {
        float curveTime = NormalizeTimeInterval(leftMostAnchor.X, lineStartPoint.X, t);
        gain = Point::bezier4(leftMostAnchor, controlLeft1, controlLeft2, lineStartPoint, curveTime).Y;
    }
    else if (t >= lineStartPoint.X && t < lineEndPoint.X) // Straight line
    {
        float curveTime = NormalizeTimeInterval(lineStartPoint.X, lineEndPoint.X, t);
        gain = Point::bezier2(lineStartPoint, lineEndPoint, curveTime).Y;
    }
    else if (t >= lineEndPoint.X && t <= rightMostAnchor.X) // Decay part
    {
        float curveTime = NormalizeTimeInterval(lineEndPoint.X, rightMostAnchor.X, t);
        gain = Point::bezier4(lineEndPoint, controlRight1, controlRight2, rightMostAnchor, curveTime).Y;
    }
    return gain;
}

juce::AudioProcessorValueTreeState::ParameterLayout SChainAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const size_t timeParametersSize = static_cast<size_t>(SettingsParameters::TimeButtonIDs::size);
    const size_t triggerParametersSize = static_cast<size_t>(SettingsParameters::TriggerButtonIDs::size);
    juce::StringArray timeChoices;
    for (size_t i = 0; i < timeParametersSize; i++) {
        timeChoices.add(SettingsParameters::timeSettings[i].name);
    }


    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "TimeInterval", "TimeInterval",
        timeChoices, static_cast<int>(SettingsParameters::TimeButtonIDs::Btn1_4)));

    juce::StringArray triggerChoices;
    for (size_t i = 0; i < triggerParametersSize; i++) {
        triggerChoices.add(SettingsParameters::triggerChoiceNames[i]);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "Trigger", "Trigger",
        triggerChoices, static_cast<int>(SettingsParameters::TriggerButtonIDs::BtnAuto)));

    addParameterPoints(layout);
    return layout;
}

float NormalizeTimeInterval(float startT, float endT, float t)
{
    return (startT - t) / (startT - endT);
}

