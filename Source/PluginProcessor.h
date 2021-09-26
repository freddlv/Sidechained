/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <chrono>
#include <vector>
#include "SettingsParameters.h"
using namespace std::chrono;

class Point {
public:
    float X;
    float Y;
    Point() : X(0.0f), Y(0.0f) {}
    Point(float x, float y) : X(x), Y(y) { }

    static Point bezier4(Point P1, Point P2, Point P3, Point P4, float t) {
        Point p;
        p.X = P1.X * (1 - t) * (1 - t) * (1 - t) + P2.X * 3 * (1 - t) *
            (1 - t) * t + P3.X * 3 * (1 - t) * t * t + P4.X * t * t * t;

        p.Y = P1.Y * (1 - t) * (1 - t) * (1 - t) + P2.Y * 3 * (1 - t) *
            (1 - t) * t + P3.Y * 3 * (1 - t) * t * t + P4.Y * t * t * t;

        return p;
    }
    static Point bezier3(Point P1, Point P2, Point P3, float t) {
        Point p;
        p.X = P1.X * (1 - t) * (1 - t) + P2.X * 2 * (1 - t) * t + P3.X * t * t;
        p.Y = P1.Y * (1 - t) * (1 - t) + P2.Y * 2 * (1 - t) * t + P3.Y * t * t;
        return p;
    }
    static Point bezier2(Point P1, Point P2, float t) {
        Point p;
        p.X = P1.X * (1.0f - t) + P2.X * t;
        p.Y = P1.Y * (1.0f - t) + P2.Y * t;
        return p;
    }
};

float NormalizeTimeInterval(float startT, float endT, float t);


//==============================================================================
/**
*/
class SChainAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SChainAudioProcessor();
    ~SChainAudioProcessor() override;

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

    void addParameterPoints(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    
    void resetCurSamplesProcessedPerChannel(size_t channelID);

    steady_clock::time_point startTime;
    std::vector<juce::int64> curSamplesProcessedPerChannel;

    const size_t totalPoints = 8;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{
        *this, nullptr, "Parameters", createParameterLayout()
    };

    std::vector<juce::RangedAudioParameter*> pointParametersX;
    std::vector<juce::RangedAudioParameter*> pointParametersY;

    juce::AudioParameterChoice* timeChoiceParam{ nullptr };
    juce::AudioParameterChoice* triggerChoiceParam{ nullptr };
    Point getPointAtID(size_t pointID);

    std::pair<juce::RangedAudioParameter*, juce::RangedAudioParameter*> getPointPtr(size_t pointID) {
        std::pair< juce::RangedAudioParameter*, juce::RangedAudioParameter*> p;
        p.first = pointParametersX[pointID];
        p.second = pointParametersY[pointID];
    }
    bool setPointAtID(size_t pointID, Point point);

    // t must be normalized to the range [0, 1]
    float getGainBezierValue(float t);
    size_t getSamplesPerBeat();
    juce::int64 getSidechainSampleLength();




private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SChainAudioProcessor)
};
