/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"
#include "SettingsParameters.h"
//==============================================================================
SChainAudioProcessorEditor::SChainAudioProcessorEditor (SChainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), envelopeCurveComponent(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    

    const auto& audioProcessorParams = audioProcessor.getParameters();
    for (auto p : audioProcessorParams) {
        p->addListener(this);
    }

    const size_t totalPoints = audioProcessor.totalPoints;
    startTimerHz(60);
    //Logo:

    cBtnLogo.setImages(
        false,
        true,
        true,
        logoImage,
        1.0f,
        juce::Colour(0x7FFFFFFFu),
        logoImage,
        0.7f,
        juce::Colour(0x7FFFFFFFu),
        logoImage,
        0.7f,
        juce::Colour(0x7FFFFFFFu)
    );

    addAndMakeVisible(cBtnLogo);
    cHeaderContainer.setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::aqua);
    cHeaderContainer.setText("Header");
    addAndMakeVisible(cHeaderContainer);

    headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    headerLabel.setText("SIDECHAINED", juce::dontSendNotification);
    headerLabel.setFont(juce::Font(Theme::titleFontFamily, Theme::fontSizeh1, juce::Font::plain));
    headerLabel.setJustificationType(juce::Justification::verticallyCentred);
    addAndMakeVisible(headerLabel);

    cEnvelopeContainer.setColour(juce::GroupComponent::ColourIds::outlineColourId, Theme::primaryColor);
    cEnvelopeContainer.setText("Envelope");
    cEnvelopeContainer.setColour(juce::GroupComponent::ColourIds::textColourId, Theme::primaryColor);
    addAndMakeVisible(cEnvelopeContainer, 0);

    cTimeSettingsContainer.setColour(juce::GroupComponent::ColourIds::outlineColourId, Theme::primaryColor);
    cTimeSettingsContainer.setText("Time Interval");
    cTimeSettingsContainer.setColour(juce::GroupComponent::ColourIds::textColourId, Theme::primaryColor);
    addAndMakeVisible(cTimeSettingsContainer);

    cTriggerSContainer.setColour(juce::GroupComponent::ColourIds::outlineColourId, Theme::primaryColor);
    cTriggerSContainer.setColour(juce::GroupComponent::ColourIds::textColourId, Theme::primaryColor);
    cTriggerSContainer.setText("Trigger");
    addAndMakeVisible(cTriggerSContainer);

    // - - - Radio buttons - - - 
    // Time Interval

    const int currentTimeChoice = audioProcessor.timeChoiceParam->getIndex();
    for (size_t i = 0; i < size_t(SettingsParameters::TimeButtonIDs::size); i++) {
        cTimeRadioBtns.push_back(std::make_unique<juce::ToggleButton>(SettingsParameters::timeSettings[i].name));
        cTimeRadioBtns[i].get()->setRadioGroupId(RadioButtonIds::TimeSettings);
        cTimeRadioBtns[i].get()->setLookAndFeel(&rbuttonLookAndFeel);
        cTimeRadioBtns[i].get()->onClick = [this, i] {
            if (cTimeRadioBtns[i].get()->getToggleState()) {
                *audioProcessor.timeChoiceParam = i;

            }
        };
        addAndMakeVisible(cTimeRadioBtns[i].get());

        //TODO: add callback
    }
    cTimeRadioBtns[currentTimeChoice].get()->triggerClick();

    const int currentTriggerChoice = audioProcessor.triggerChoiceParam->getIndex();
    // Trigger Settings
    for (size_t i = 0; i < size_t(SettingsParameters::TriggerButtonIDs::size); i++) {
        cTriggerRadioBtns.push_back(std::make_unique<juce::ToggleButton>(SettingsParameters::triggerChoiceNames[i]));
        cTriggerRadioBtns[i].get()->setRadioGroupId(RadioButtonIds::TriggerSettings);
        cTriggerRadioBtns[i].get()->onClick = [this, i] { 
            if (cTriggerRadioBtns[i].get()->getToggleState()) {
                *audioProcessor.triggerChoiceParam = i;

            }
        };
        addAndMakeVisible(cTriggerRadioBtns[i].get());
    }

    const size_t audioBtnIdx = static_cast<size_t>(SettingsParameters::TriggerButtonIDs::BtnAudio);
    cTriggerRadioBtns[audioBtnIdx].get()->setEnabled(false);
    cTriggerRadioBtns[currentTriggerChoice].get()->triggerClick();
    //Curve Points:
    addAndMakeVisible(envelopeCurveComponent);
    cEnvelopeContainer.toBack();
    envelopeCurveComponent.toFront(true);


    setSize(590, 480);
}

SChainAudioProcessorEditor::~SChainAudioProcessorEditor()
{
    const auto& audioProcessorParams = audioProcessor.getParameters();
    for (auto p : audioProcessorParams) {
        p->removeListener(this);
    }
}

//==============================================================================
void SChainAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Theme::bgColor);
    auto bounds = getLocalBounds();
    g.setColour(Theme::bgOutlineColor);
    g.drawRect(bounds, 7.0f);

}

juce::Point<float> SChainAudioProcessorEditor::getScaledPointAtTime(const juce::Rectangle<int>& bounds, float t)
{
    juce::Path envelopeCurve;
    const double minY = bounds.getBottom();
    const double maxY = bounds.getY();
    const double minX = bounds.getX();
    const double maxX = bounds.getRight();
    auto yMap = [minY, maxY](double t) {
        return juce::jmap<float>(t, minY, maxY);
    };
    auto xMap = [minX, maxX](double t) {
        return juce::jmap<float>(t, minX, maxX);
    };
    juce::Point<float> p;

    float gainFactor = audioProcessor.getGainBezierValue(t);
    p.setXY(xMap(t), yMap(gainFactor));
    return p;
}

void SChainAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int mainHorizontalMargin = 46;
    int VerticalMargin = 2;
    auto mainArea = getLocalBounds();
    auto headerArea = mainArea.removeFromTop(mainArea.getHeight() * 0.16);
    auto headerLabelArea = headerArea.reduced(20);

    mainArea.removeFromLeft(mainHorizontalMargin);
    mainArea.removeFromRight(mainHorizontalMargin);
    mainArea.removeFromTop(VerticalMargin);
    mainArea.removeFromBottom(Theme::containersMargin * 2);

    auto mainHeight = mainArea.getHeight();
    float envelopeProportions = 0.74;
    envelopeCurveArea = mainArea.removeFromTop(mainHeight * envelopeProportions);
    cEnvelopeContainer.setBounds(envelopeCurveArea);

    int margin = 15;
    envelopeCurveArea.removeFromTop(margin);
    envelopeCurveArea.removeFromBottom(margin/1.5);
    envelopeCurveArea.removeFromLeft(margin);
    envelopeCurveArea.removeFromRight(margin);
    //cCurvePoint1.setBounds(envelopeCurveArea);
    envelopeCurveComponent.setBounds(envelopeCurveArea);
    auto controlsArea = mainArea.removeFromTop(mainHeight * (1.0f - envelopeProportions));

    auto controlsAreaWidth = controlsArea.getWidth();
    controlsArea.removeFromTop(Theme::containersMargin);

    auto btnsLeftMargin = 20;

    auto timeSettingsArea = controlsArea.removeFromLeft(controlsAreaWidth * 0.65);
    controlsArea.removeFromLeft(Theme::containersMargin);
    auto triggerSettingsArea = controlsArea;

    // Time settings:
    cTimeSettingsContainer.setBounds(timeSettingsArea);
    timeSettingsArea.removeFromLeft(btnsLeftMargin);
    timeSettingsArea.removeFromRight(btnsLeftMargin);
    auto timeRadioBtnsArea = timeSettingsArea;

    auto btnUnitWidthStep = timeRadioBtnsArea.getWidth() / size_t(TimeButtonIDs::size);
    for (size_t i = 0; i < size_t(TimeButtonIDs::size); i++) {
        auto btnArea = timeRadioBtnsArea.removeFromLeft(btnUnitWidthStep);
        cTimeRadioBtns[i].get()->setBounds(btnArea);
    }

    cTriggerSContainer.setBounds(triggerSettingsArea);
    triggerSettingsArea.removeFromLeft(btnsLeftMargin);
    triggerSettingsArea.removeFromRight(btnsLeftMargin);
    auto triggerRadioBtnsArea = triggerSettingsArea;

    btnUnitWidthStep = triggerRadioBtnsArea.getWidth() / size_t(TriggerButtonIDs::size);
    for (size_t i = 0; i < size_t(TriggerButtonIDs::size); i++) {
        auto btnArea = triggerRadioBtnsArea.removeFromLeft(btnUnitWidthStep);
        cTriggerRadioBtns[i].get()->setBounds(btnArea);
        cTriggerRadioBtns[i].get()->setLookAndFeel(&rbuttonLookAndFeel);
    }
    cBtnLogo.setBounds(headerArea.reduced(20));


}



void SChainAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue) {
    haveParamsChanged.set(true);
}
void SChainAudioProcessorEditor::timerCallback() {
    if (haveParamsChanged.compareAndSetBool(false, true)) 
    {
        const int currentTimeChoice = audioProcessor.timeChoiceParam->getIndex();
        cTimeRadioBtns[currentTimeChoice].get()->triggerClick();
        repaint();
    }
}