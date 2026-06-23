/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    

    const int numParams = audioProcessor.audioParams.size();
    

    sliders.reserve(numParams);
    labels.reserve(numParams);
    attachments.reserve(numParams);
 
    for (int i{0}; i < audioProcessor.audioParams.size(); ++i){
        // this can be customized later by adding more functionality to each audio parameter's description itself
        // we are essentially assuming that every parameter requires a slider, and has a label, and the value is float for simplicity
        // customizability can always be added later
        
        auto& param = audioProcessor.audioParams[i];

        // Create and configure the Slider
        auto slider = std::make_unique<juce::Slider>();
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider->setRange(param.min, param.max, param.step);
        slider->setValue(param.default_value);
        addAndMakeVisible(*slider);

        // Create and configure the Label
        auto label = std::make_unique<juce::Label>();
        label->setText(param.name, juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*label);

        // Create the APVTS Attachment
        auto attachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, param.name, *slider);

        // Move the unique pointers into the vectors for safe keeping
        sliders.push_back(std::move(slider));
        labels.push_back(std::move(label));
        attachments.push_back(std::move(attachment));
    }
    setSize (400, 300); // we can use the number of parameters we have to determine the size
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int numParams = sliders.size();
    if (numParams == 0) return;

    const int cols = std::ceil (std::sqrt (numParams));
    const int rows = std::ceil ( static_cast<float>(numParams) / cols);
    const int cellWidth = getWidth() / cols;
    const int cellHeight = getHeight() / rows;

    for (int i{0}; i < numParams; ++i){
        const int currentCol = i % cols;
        const int currentRow = i / cols;

        juce::Rectangle<int> cellBounds (currentCol * cellWidth, currentRow * cellHeight, cellWidth, cellHeight);
        cellBounds.reduce(5,5);
        labels[i]->setBounds (cellBounds.removeFromTop(24));
        sliders[i]->setBounds (cellBounds);
    }
}
