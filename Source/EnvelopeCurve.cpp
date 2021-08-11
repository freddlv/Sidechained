/*
  ==============================================================================

    EnvelopeCurve.cpp
    Created: 7 Aug 2021 12:58:00pm
    Author:  frebo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EnvelopeCurve.h"

//==============================================================================
EnvelopeCurve::EnvelopeCurve(SChainAudioProcessor& p): audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    const size_t totalPoints = audioProcessor.totalPoints;
    for (size_t i = 0; i < totalPoints; i++)
    {
        curvePoints.push_back(std::make_unique<CurvePoint>(i, audioProcessor, haveParamsChanged));
        addAndMakeVisible(curvePoints[i].get());
    }
    startTimerHz(60);

}

EnvelopeCurve::~EnvelopeCurve()
{
}


juce::Rectangle<int> EnvelopeCurve::getEnvelopeGraphBounds(){
    const int margin = 10;
    auto envelopeArea = getLocalBounds();
    envelopeArea.removeFromBottom(margin);
    envelopeArea.removeFromRight(margin);
    envelopeArea.removeFromTop(margin);
    envelopeArea.removeFromLeft(margin);
    return envelopeArea;
}
void EnvelopeCurve::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    auto envelopeArea = getEnvelopeGraphBounds();
    const int margin = 10;
    size_t sidechainSamples = audioProcessor.getSidechainSampleLength();
    juce::Path envelopeCurve;
    const double minY = envelopeArea.getBottom();
    const double maxY = envelopeArea.getY();
    const double minX = envelopeArea.getX();
    const double maxX = envelopeArea.getRight();
    auto yMap = [minY, maxY](double t) {
        return juce::jmap(t, minY, maxY);
    };
    auto xMap = [minX, maxX](double t) {
        return juce::jmap(t, minX, maxX);
    };
    envelopeCurve.startNewSubPath(minX, minY);
    for (size_t i = 0; i < sidechainSamples; i++)
    {
        float t = NormalizeTimeInterval(0, sidechainSamples, i);
        float gainFactor = audioProcessor.getGainBezierValue(t);

        envelopeCurve.lineTo(xMap(t), yMap(gainFactor));
    }

    g.setColour(juce::Colours::white);
    g.strokePath(envelopeCurve, juce::PathStrokeType(3.f));

    //Drawing point lines
    const size_t totalPoints = audioProcessor.totalPoints;
    juce::Path pointLinesPath;
    Point p0 = audioProcessor.getPointAtID(0);
    pointLinesPath.startNewSubPath(xMap(p0.X), yMap(p0.Y));

    for (size_t i = 1; i < totalPoints; i++)
    {
        Point p = audioProcessor.getPointAtID(i);
        pointLinesPath.lineTo(xMap(p.X), yMap(p.Y));
    }
    g.setColour(juce::Colours::seagreen);
    g.strokePath(pointLinesPath, juce::PathStrokeType(0.5f, juce::PathStrokeType::JointStyle::mitered));
}

void EnvelopeCurve::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    const size_t totalPoints = audioProcessor.totalPoints;
    float step = 1.0 / float(totalPoints);
    auto bounds = getEnvelopeGraphBounds();
    for (size_t i = 0; i < totalPoints; i++)
    {
        const int pointDiameter = Theme::pointRadius * 2;
        curvePoints[i].get()->setBounds(0, 0, pointDiameter, pointDiameter);
        curvePoints[i].get()->setFrameArea(bounds);
        Point p = audioProcessor.getPointAtID(i);
        curvePoints[i].get()->setNormalizedPosition(p.X, p.Y, bounds);
    }

}

void EnvelopeCurve::timerCallback() {
    
}