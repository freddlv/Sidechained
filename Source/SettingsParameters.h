/*
  ==============================================================================

    SettingsParameters.h
    Created: 10 Aug 2021 4:32:20pm
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <string>
class SettingsParameters {
public:
    enum class TimeButtonIDs {
        Btn1_4,
        Btn1_8,
        Btn1_16,
        BtnCustom,
        size
    };
    enum class TriggerButtonIDs {
        BtnAuto,
        BtnAudio,
        size
    };
    struct data {
        std::string name;
        float duration; //1/4th note = 0.25, 1/8th = 0.125

    };

    static std::array<data, size_t(TimeButtonIDs::size)> timeSettings;

    static std::array<juce::String, size_t(TriggerButtonIDs::size)> triggerChoiceNames;

};

enum class Points {
    leftAnchor,
    Control1,
    Control2,
    LineStart,
    LineEnd,
    Control3,
    Control4,
    rightAnchor
};