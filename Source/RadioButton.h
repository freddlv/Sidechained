/*
  ==============================================================================

    RadioButton.h
    Created: 10 Aug 2021 11:05:24am
    Author:  frebo

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "Theme.h"
#include "PluginEditor.h"

/*

class RadioButton : public juce::ToggleButon
{
public:
    RadioButton();
    ~CurvePoint() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    //Coordinate system has origin (0, 0) at leftBottom of localBounds()
    //x: {0, 1}
    //y: {0, 1}
    void setNormalizedPosition(float x, float y, juce::Rectangle<int> frameArea);
    void setFrameArea(juce::Rectangle<int> area);

    //juce::Point<float> getPosition();
    //void setPosition(juce::Point<float> position);
    juce::ComponentDragger myDragger;

    void mouseDown(const juce::MouseEvent& e);
    void mouseDrag(const juce::MouseEvent& e);
    void mouseUp(const juce::MouseEvent& e);

private:
    juce::Atomic<float> xPos;
    juce::Atomic<float> yPos;
    juce::Atomic<bool>& repaintSignal;

    float pointRadius = 6.0;
    SChainAudioProcessor& audioProcessor;
    juce::Rectangle<int> frameArea;
    size_t pointID;
    juce::Atomic<bool> isDragging;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CurvePoint);


};
*/