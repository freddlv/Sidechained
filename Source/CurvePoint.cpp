/*
  ==============================================================================

    CurvePoint.cpp
    Created: 7 Aug 2021 12:58:40pm
    Author:  frebo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CurvePoint.h"
#include "Utils.h"
#include "Theme.h"

//==============================================================================
CurvePoint::CurvePoint(size_t pID, SChainAudioProcessor& p, juce::Atomic<bool>& repaintSignal):
    ShapeButton("shapeBtn", Theme::controlPointColor, Theme::controlPointColor, Theme::controlPointColor),
pointID(pID), audioProcessor(p),
    repaintSignal(repaintSignal)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    pointXAttachment = std::make_unique<juce::ParameterAttachment>(*p.pointParametersX[pointID], onPointXUpdated);
    pointYAttachment = std::make_unique<juce::ParameterAttachment>(*p.pointParametersY[pointID], onPointYUpdated);
    pointXAttachment.get()->sendInitialUpdate();
    pointYAttachment.get()->sendInitialUpdate();
}

CurvePoint::~CurvePoint()
{
}

void CurvePoint::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    //g.fillAll(Theme::controlPointColor);
    g.setColour(Theme::controlPointColor);
    g.fillEllipse(getLocalBounds().toFloat());
    
}

void CurvePoint::setNormalizedPosition(float x, float y, juce::Rectangle<int> frameArea) {
    const float minY = float(frameArea.getBottom());
    const float minX = float(frameArea.getX());
    float posX = minX  + float(frameArea.getWidth()) * x;
    float posY = minY - float(frameArea.getHeight()) * y;
    juce::Rectangle<int> bounds = getLocalBounds();
    setBounds(posX, posY, bounds.getHeight(), bounds.getWidth());
}
void CurvePoint::resized()
{
}


void CurvePoint::mouseDown(const juce::MouseEvent& e)
{
    //audioProcessor.pointParametersX[pointID]->beginChangeGesture();
    pointXAttachment.get()->beginGesture();
    pointYAttachment.get()->beginGesture();
    isDragging.set(true);

    myDragger.startDraggingComponent(this, e);
}

void CurvePoint::mouseDrag(const juce::MouseEvent& e)
{
    compBoundsConstrainer.setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);


    myDragger.dragComponent(this, e, &compBoundsConstrainer);
    juce::Point<int> pos = getPosition();
    Point p;
    float curvePosY = float(frameArea.getBottom() - pos.getY());
    p.X = NormalizeTimeInterval(frameArea.getX(), frameArea.getRight(), float(pos.getX()));
    p.Y = NormalizeTimeInterval(frameArea.getY(), frameArea.getBottom(), curvePosY);
    pointXAttachment.get()->setValueAsPartOfGesture(p.X);
    pointYAttachment.get()->setValueAsPartOfGesture(p.Y);
}
void CurvePoint::mouseUp(const juce::MouseEvent& e) {
    if (isDragging.compareAndSetBool(false, true)) 
    {
        pointXAttachment.get()->endGesture();
        pointYAttachment.get()->endGesture();
    }
    
}
void CurvePoint::setFrameArea(juce::Rectangle<int> area) {
    frameArea = area;
}

void CurvePoint::onPointParameterChangedX(float newValue) {

}

