/*
  ==============================================================================

    TriggerParameters.h
    Created: 11 Aug 2021 11:05:57am
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TriggerParameters  : public juce::Component
{
public:
    TriggerParameters();
    ~TriggerParameters() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerParameters)
};
