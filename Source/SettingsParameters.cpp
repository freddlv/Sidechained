/*
  ==============================================================================

    SettingsParameters.cpp
    Created: 10 Aug 2021 4:32:20pm
    Author:  frebo

  ==============================================================================
*/

#include "SettingsParameters.h"


std::array<SettingsParameters::data, size_t(SettingsParameters::TimeButtonIDs::size)> SettingsParameters::timeSettings = {
{
    {"1/4", float(1) / float(4)},
    {"1/8", float(1) / float(8)},
    {"1/16", float(1) / float(16)},
    {"1/32", float(1)/float(32)}
}};
std::array<juce::String, size_t(SettingsParameters::TriggerButtonIDs::size)> SettingsParameters::triggerChoiceNames = {
    "Auto",
    "Audio"
};
