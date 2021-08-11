/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CurvePoint.h"
#include "EnvelopeCurve.h"
#include "Theme.h"


class RButtonLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawTickBox(juce::Graphics& g,
        juce::Component&,
        float 	x,
        float 	y,
        float 	w,
        float 	h,
        bool 	ticked,
        bool 	isEnabled,
        bool 	shouldDrawButtonAsHighlighted,
        bool 	shouldDrawButtonAsDown
    ) {
        g.setColour(juce::Colour(0xFF19E6DAu));
        g.drawEllipse(x, y, w, w, 2.3f);
        if (ticked) {
            g.setColour(juce::Colour(0xFF94E2CFu));
            const float radiusFactor = 0.90f;
            g.fillEllipse(x+ radiusFactor/2.0f, y+radiusFactor/2.0f, w * radiusFactor, w * radiusFactor);
        }
    }
   
};

float NormalizeTimeInterval(float startT, float endT, float t);

struct DraggablePoint: public juce::ShapeButton
{
    juce::ComponentDragger myDragger;
    juce::ComponentBoundsConstrainer compBoundsConstrainer;
    DraggablePoint(juce::String strId) : 
        juce::ShapeButton(strId, juce::Colours::aqua, juce::Colours::beige, juce::Colours::antiquewhite) {
        
    };

    void mouseDown(const juce::MouseEvent& e)
    {
        myDragger.startDraggingComponent(this, e);
    }

    void mouseDrag(const juce::MouseEvent& e)
    {
        myDragger.dragComponent(this, e, &compBoundsConstrainer);
    }
};

//==============================================================================
/**
*/

class SChainAudioProcessorEditor  : public juce::AudioProcessorEditor,
    juce::AudioProcessorParameter::Listener,
    juce::Timer

{
public:
    SChainAudioProcessorEditor (SChainAudioProcessor&);
    ~SChainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SChainAudioProcessor& audioProcessor;
    RButtonLookAndFeel rbuttonLookAndFeel;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::ButtonAttachment;

    juce::Slider sliderP0;
    std::vector<std::unique_ptr<juce::Slider>> pointSlidersX;
    std::vector<std::unique_ptr<juce::Slider>> pointSlidersY;
    std::vector<std::unique_ptr<APVTS::SliderAttachment>> pointSlidersAttachments;

    juce::Point<float> getScaledPointAtTime(const juce::Rectangle<int>& bounds, float t);
    
    juce::Atomic<bool> haveParamsChanged{ false };

    // Components (Containers)
    juce::GroupComponent cHeaderContainer;
    juce::GroupComponent cDsplayContainer;
    juce::GroupComponent cControlsContainer;
    juce::Label headerLabel;
    juce::GroupComponent cTimeSettingsContainer;
    juce::GroupComponent cTriggerSContainer;
    juce::GroupComponent cEnvelopeContainer;
    juce::Image logoImage{ juce::ImageFileFormat::loadFrom(juce::File::getCurrentWorkingDirectory().getChildFile(Theme::logoPath)) };
    juce::ImageButton cBtnLogo;

    juce::Rectangle<int> envelopeCurveArea{ getLocalBounds() };


    enum RadioButtonIds {
        TimeSettings = 421,
        TriggerSettings = 422
    };

    enum class TimeButtonIDs : size_t {
        Btn1_4,
        Btn1_16,
        Btn1_32,
        BtnCustom,
        size
    };
    enum class TriggerButtonIDs {
        BtnAuto,
        BtnAudio,
        size
    };
    std::array<juce::String, size_t(TimeButtonIDs::size)> timeRadioBtnsNames{
        "1/4",
        "1/16",
        "1/32",
        "Custom"
    };

    std::array<juce::String, size_t(TriggerButtonIDs::size)> triggerRadioBtnsNames{
        "Auto",
        "Audio"
    };

    // Radio Buttons and Inputs

    // Time Interval
    std::vector<std::unique_ptr<juce::ToggleButton>> cTimeRadioBtns;
    std::vector<std::unique_ptr<APVTS::ButtonAttachment>> cTimeRadioBtnsAttachments;


    juce::TextEditor cTimeTxtbox;

    // Trigger Settings
    std::vector<std::unique_ptr<juce::ToggleButton>> cTriggerRadioBtns;
    std::vector<std::unique_ptr<APVTS::ButtonAttachment>> cTriggerRadioBtnsAttachments;
    

    juce::TextButton cBtnAudioTriggerSelect{ "Select" };

    EnvelopeCurve envelopeCurveComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SChainAudioProcessorEditor)
};
