#pragma once

#include "AirwindowsUI.h"
#include "PluginProcessor.h"
#include "BinaryData.h"



//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, juce::Slider::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider *) override;
    void sliderDragStarted(juce::Slider *) override;
    void sliderDragEnded(juce::Slider *) override;
    void sliderDragInternal(juce::Slider *, bool started);
    void updateTrackProperties();
    void updatePluginSize();
    void idle();
 
    struct IdleTimer : juce::Timer
        {
            IdleTimer(PluginEditor *ed) : ed(ed) {}
            ~IdleTimer() = default;
            void timerCallback() override { ed->idle(); }
            PluginEditor *ed;
        };


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef; //quick ref
    
    std::unique_ptr<IdleTimer> idleTimer;
    AirwindowsLookAndFeel airwindowsLookAndFeel;
    AirwindowsMeter meter;
    
    juce::Slider TRMKnob { "Trim" };
    juce::Slider MORKnob { "More" };
    juce::Slider HIGKnob { "High" };
    juce::Slider HMGKnob { "HMid" };
    juce::Slider LMGKnob { "LMid" };
    juce::Slider BSGKnob { "Bass" };
    juce::Slider HIFKnob { "HighF" };
    juce::Slider HMFKnob { "HMidF" };
    juce::Slider LMFKnob { "LMidF" };
    juce::Slider BSFKnob { "BassF" };
    juce::Slider THRKnob { "Comp" };
    juce::Slider ATKKnob { "Attack" };
    juce::Slider RLSKnob { "Release" };
    juce::Slider GATKnob { "Gate" };
    juce::Slider LOPKnob { "Lowpass" };
    juce::Slider HIPKnob { "Hipass" };
    juce::Slider PANKnob { "Pan" };
    juce::Slider FADKnob { "Fader" };

    juce::Colour hostTrackColour = juce::Colours::lightgrey;
    juce::String hostTrackName = juce::String();
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
