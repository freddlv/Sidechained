/*
  ==============================================================================

    TimeParameters.h
    Created: 11 Aug 2021 11:05:39am
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TimeParameters  : public juce::Component
{
public:
    TimeParameters();
    ~TimeParameters() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeParameters)
};
