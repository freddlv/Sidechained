/*
  ==============================================================================

    Utils.h
    Created: 7 Aug 2021 12:59:01pm
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <stdio.h>

template <typename... Args>
void DBGF(juce::String format, Args... args)
{
    juce::String output;
    char buffer[256];
    sprintf(buffer, format.toStdString().c_str(), args...);
    DBG(buffer);
}
