/*
  ==============================================================================

    EnvelopeCurve.h
    Created: 7 Aug 2021 12:58:00pm
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Theme.h"
#include "CurvePoint.h"
//==============================================================================
/*
*/
class EnvelopeCurve  : public juce::Component, juce::Timer
{
public:
    EnvelopeCurve(SChainAudioProcessor& p);
    ~EnvelopeCurve() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    juce::Rectangle<int> getEnvelopeGraphBounds();
    void timerCallback() override;

private:
    SChainAudioProcessor& audioProcessor;

    std::vector<std::unique_ptr<CurvePoint>> curvePoints;
    juce::Atomic<bool> haveParamsChanged;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeCurve)
};
