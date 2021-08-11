/*
  ==============================================================================

    CurvePoint.h
    Created: 7 Aug 2021 12:58:40pm
    Author:  frebo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Theme.h"
#include "Utils.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class CurvePoint  : public juce::ShapeButton
{
public:
    CurvePoint(size_t pID, SChainAudioProcessor& p, juce::Atomic<bool>& repaintSignal);
    ~CurvePoint() override;

    void paint (juce::Graphics&) override;
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

    void onPointParameterChangedX(float);
    void onPointParameterChangedY(float newValue);

private:
    juce::Atomic<float> xPos;
    juce::Atomic<float> yPos;
    juce::Atomic<bool>& repaintSignal;

    float pointRadius = 6.0;
    SChainAudioProcessor& audioProcessor;
    std::unique_ptr<juce::ParameterAttachment> pointXAttachment;
    std::unique_ptr<juce::ParameterAttachment> pointYAttachment;
    std::function<void(float)> onPointXUpdated = [this](float newValue) {
        if (!isDragging.get()) {
            Point p = audioProcessor.getPointAtID(pointID);
            setNormalizedPosition(newValue, p.Y, frameArea);

        }
    };
    std::function<void(float)> onPointYUpdated = [this](float newValue) {
        if (!isDragging.get()) {
            Point p = audioProcessor.getPointAtID(pointID);
            setNormalizedPosition(p.X, newValue, frameArea);

        }
    };

    juce::Rectangle<int> frameArea;
    juce::Rectangle<int> constraintBounds;
    size_t pointID;
    juce::ComponentBoundsConstrainer compBoundsConstrainer;
    juce::Atomic<bool> isDragging;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CurvePoint);


};
