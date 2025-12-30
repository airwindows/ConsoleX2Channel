#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    setResizable(true, true);
    setLookAndFeel(&airwindowsLookAndFeel);
    if (hostTrackColour != juce::Colour()) {
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, hostTrackColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, hostTrackColour);
    } else {
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour);
    }
    updateTrackProperties();
    updatePluginSize();

    idleTimer = std::make_unique<IdleTimer>(this);
    idleTimer->startTimer(1000/30); //space between UI screen updates. Larger is slower updates to screen

    meter.setOpaque(true);
    meter.resetArrays();
    meter.displayTrackName = hostTrackName;
    addAndMakeVisible(meter);

    if (airwindowsLookAndFeel.knobMode == 0) {
        TRMKnob.setSliderStyle(juce::Slider::Rotary);
        MORKnob.setSliderStyle(juce::Slider::Rotary);
        HIGKnob.setSliderStyle(juce::Slider::Rotary);
        HMGKnob.setSliderStyle(juce::Slider::Rotary);
        LMGKnob.setSliderStyle(juce::Slider::Rotary);
        BSGKnob.setSliderStyle(juce::Slider::Rotary);
        HIFKnob.setSliderStyle(juce::Slider::Rotary);
        HMFKnob.setSliderStyle(juce::Slider::Rotary);
        LMFKnob.setSliderStyle(juce::Slider::Rotary);
        BSFKnob.setSliderStyle(juce::Slider::Rotary);
        THRKnob.setSliderStyle(juce::Slider::Rotary);
        ATKKnob.setSliderStyle(juce::Slider::Rotary);
        RLSKnob.setSliderStyle(juce::Slider::Rotary);
        GATKnob.setSliderStyle(juce::Slider::Rotary);
        LOPKnob.setSliderStyle(juce::Slider::Rotary);
        HIPKnob.setSliderStyle(juce::Slider::Rotary);
        PANKnob.setSliderStyle(juce::Slider::Rotary);
        FADKnob.setSliderStyle(juce::Slider::Rotary);
    }

    if (airwindowsLookAndFeel.knobMode == 1) {
        TRMKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HIGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        GATKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    }

    if (airwindowsLookAndFeel.knobMode == 2) {
        TRMKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HIGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        GATKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    }

    if (airwindowsLookAndFeel.knobMode == 3) {
        TRMKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        MORKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HIGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LMGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        BSGKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HIFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LMFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        BSFKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        THRKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        ATKKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        RLSKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        GATKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        LOPKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        HIPKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        PANKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        FADKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    }

    TRMKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    TRMKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    TRMKnob.setRange(0.0f, 1.0f);
    TRMKnob.setValue(processorRef.params[PluginProcessor::KNOBTRM]->get(), juce::NotificationType::dontSendNotification);
    TRMKnob.addListener(this);
    addAndMakeVisible(TRMKnob);

    MORKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    MORKnob.setRange(0.0f, 1.0f);
    MORKnob.setValue(processorRef.params[PluginProcessor::KNOBMOR]->get(), juce::NotificationType::dontSendNotification);
    MORKnob.addListener(this);
    addAndMakeVisible(MORKnob);
    MORKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(1.0f, 0.486f, 0.129f, 1.0f)); //fixed thumb color orange

    
    HIGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIGKnob.setRange(0.0f, 1.0f);
    HIGKnob.setValue(processorRef.params[PluginProcessor::KNOBHIG]->get(), juce::NotificationType::dontSendNotification);
    HIGKnob.addListener(this);
    addAndMakeVisible(HIGKnob);
    HIGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.698f, 0.152f, 0.164f, 1.0f)); //fixed thumb color red
    
    HMGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HMGKnob.setRange(0.0f, 1.0f);
    HMGKnob.setValue(processorRef.params[PluginProcessor::KNOBHMG]->get(), juce::NotificationType::dontSendNotification);
    HMGKnob.addListener(this);
    addAndMakeVisible(HMGKnob);
    HMGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.2f, 0.466f, 0.2f, 1.0f)); //fixed thumb color green
    
    LMGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LMGKnob.setRange(0.0f, 1.0f);
    LMGKnob.setValue(processorRef.params[PluginProcessor::KNOBLMG]->get(), juce::NotificationType::dontSendNotification);
    LMGKnob.addListener(this);
    addAndMakeVisible(LMGKnob);
    LMGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.078f, 0.466f, 0.662f, 1.0f)); //fixed thumb color blue

    BSGKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    BSGKnob.setRange(0.0f, 1.0f);
    BSGKnob.setValue(processorRef.params[PluginProcessor::KNOBBSG]->get(), juce::NotificationType::dontSendNotification);
    BSGKnob.addListener(this);
    addAndMakeVisible(BSGKnob);
    BSGKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.513f, 0.27f, 0.153f, 1.0f)); //fixed thumb color brown

    HIFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIFKnob.setRange(0.0f, 1.0f);
    HIFKnob.setValue(processorRef.params[PluginProcessor::KNOBHIF]->get(), juce::NotificationType::dontSendNotification);
    HIFKnob.addListener(this);
    addAndMakeVisible(HIFKnob);
    HIFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.698f, 0.152f, 0.164f, 1.0f)); //fixed thumb color red
    
    HMFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HMFKnob.setRange(0.0f, 1.0f);
    HMFKnob.setValue(processorRef.params[PluginProcessor::KNOBHMF]->get(), juce::NotificationType::dontSendNotification);
    HMFKnob.addListener(this);
    addAndMakeVisible(HMFKnob);
    HMFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.2f, 0.466f, 0.2f, 1.0f)); //fixed thumb color green
    
    LMFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LMFKnob.setRange(0.0f, 1.0f);
    LMFKnob.setValue(processorRef.params[PluginProcessor::KNOBLMF]->get(), juce::NotificationType::dontSendNotification);
    LMFKnob.addListener(this);
    addAndMakeVisible(LMFKnob);
    LMFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.078f, 0.466f, 0.662f, 1.0f)); //fixed thumb color blue

    BSFKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    BSFKnob.setRange(0.0f, 1.0f);
    BSFKnob.setValue(processorRef.params[PluginProcessor::KNOBBSF]->get(), juce::NotificationType::dontSendNotification);
    BSFKnob.addListener(this);
    addAndMakeVisible(BSFKnob);
    BSFKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.513f, 0.27f, 0.153f, 1.0f)); //fixed thumb color brown

    THRKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    THRKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    THRKnob.setRange(0.0f, 1.0f);
    THRKnob.setValue(processorRef.params[PluginProcessor::KNOBTHR]->get(), juce::NotificationType::dontSendNotification);
    THRKnob.addListener(this);
    addAndMakeVisible(THRKnob);
    THRKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //black

    ATKKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    ATKKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    ATKKnob.setRange(0.0f, 1.0f);
    ATKKnob.setValue(processorRef.params[PluginProcessor::KNOBATK]->get(), juce::NotificationType::dontSendNotification);
    ATKKnob.addListener(this);
    addAndMakeVisible(ATKKnob);
    ATKKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //black

    RLSKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    RLSKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    RLSKnob.setRange(0.0f, 1.0f);
    RLSKnob.setValue(processorRef.params[PluginProcessor::KNOBRLS]->get(), juce::NotificationType::dontSendNotification);
    RLSKnob.addListener(this);
    addAndMakeVisible(RLSKnob);
    RLSKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //black

    GATKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    GATKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    GATKnob.setRange(0.0f, 1.0f);
    GATKnob.setValue(processorRef.params[PluginProcessor::KNOBGAT]->get(), juce::NotificationType::dontSendNotification);
    GATKnob.addListener(this);
    addAndMakeVisible(GATKnob);
    GATKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f)); //black

    LOPKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    LOPKnob.setRange(0.0f, 1.0f);
    LOPKnob.setValue(processorRef.params[PluginProcessor::KNOBLOP]->get(), juce::NotificationType::dontSendNotification);
    LOPKnob.addListener(this);
    addAndMakeVisible(LOPKnob);
    LOPKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.466f, 0.313f, 0.505f, 1.0f)); //fixed thumb color purple

    
    HIPKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    HIPKnob.setRange(0.0f, 1.0f);
    HIPKnob.setValue(processorRef.params[PluginProcessor::KNOBHIP]->get(), juce::NotificationType::dontSendNotification);
    HIPKnob.addListener(this);
    addAndMakeVisible(HIPKnob);
    HIPKnob.setColour(juce::Slider::thumbColourId, juce::Colour().fromFloatRGBA(0.949f, 0.913f, 0.117f, 1.0f)); //fixed thumb color yellow

    
    PANKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    PANKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    PANKnob.setRange(0.0f, 1.0f);
    PANKnob.setValue(processorRef.params[PluginProcessor::KNOBPAN]->get(), juce::NotificationType::dontSendNotification);
    PANKnob.addListener(this);
    addAndMakeVisible(PANKnob);

    FADKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    FADKnob.setRange(0.0f, 1.0f);
    FADKnob.setValue(processorRef.params[PluginProcessor::KNOBFAD]->get(), juce::NotificationType::dontSendNotification);
    FADKnob.addListener(this);
    addAndMakeVisible(FADKnob);

    setSize (airwindowsLookAndFeel.userWidth, airwindowsLookAndFeel.userHeight);
    // Make sure that before the constructor has finished, you've set the editor's size to whatever you need it to be.
}

PluginEditor::~PluginEditor(){
    setLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    if (airwindowsLookAndFeel.blurImage == juce::Image()) {
        g.fillAll (airwindowsLookAndFeel.defaultColour);
        if (hostTrackColour != juce::Colour()) {
            g.setFillType(juce::FillType(hostTrackColour)); g.setOpacity(airwindowsLookAndFeel.applyTrackColour); g.fillAll();
        }
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour.interpolatedWith (hostTrackColour, airwindowsLookAndFeel.applyTrackColour));
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour.interpolatedWith (hostTrackColour, airwindowsLookAndFeel.applyTrackColour));
    } else {
        if (airwindowsLookAndFeel.usingNamedImage) {
            g.drawImageWithin(airwindowsLookAndFeel.backgroundImage, 0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight(), 0);
        } else {
            g.setTiledImageFill(airwindowsLookAndFeel.backgroundImage, 0, 0, 1.0f); g.fillAll();
        }
                
        if (hostTrackColour != juce::Colour()) {
            g.setFillType(juce::FillType(hostTrackColour)); g.setOpacity(airwindowsLookAndFeel.applyTrackColour); g.fillAll();
        }
        airwindowsLookAndFeel.defaultColour = juce::Colour::fromRGBA(airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getRed(),airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getGreen(),airwindowsLookAndFeel.blurImage.getPixelAt(1,1).getBlue(),1.0);
        airwindowsLookAndFeel.setColour(juce::ResizableWindow::backgroundColourId, airwindowsLookAndFeel.defaultColour);
        airwindowsLookAndFeel.setColour(juce::Slider::thumbColourId, airwindowsLookAndFeel.defaultColour);
    } //find the color of the background tile or image, if there is one. Please use low-contrast stuff, but I'm not your mom :)
    
    g.setFont(juce::FontOptions(airwindowsLookAndFeel.newFont, g.getCurrentFont().getHeight(), 0));
    auto linewidth = getLocalBounds().getWidth(); if (getLocalBounds().getHeight() > linewidth) linewidth = getLocalBounds().getHeight();  linewidth = (int)cbrt(linewidth/2)/2;
    if ((hostTrackName == juce::String()) || (hostTrackName.length() < 1.0f)) hostTrackName = juce::String("ConsoleHChannel");
    meter.displayTrackName = hostTrackName; //if not track name, then name of plugin. To be displayed on the actual peakmeter
    meter.displayFont = airwindowsLookAndFeel.newFont; //in the custom font, if we're using one
        
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::white, 0.618f));
    g.fillRect(0, 0, getLocalBounds().getWidth(), linewidth); g.fillRect(0, 0, linewidth, getLocalBounds().getHeight());
    g.setColour (findColour(juce::ResizableWindow::backgroundColourId).interpolatedWith (juce::Colours::black, 0.382f));
    g.fillRect(linewidth, getLocalBounds().getHeight()-linewidth, getLocalBounds().getWidth(), linewidth); g.fillRect(getLocalBounds().getWidth()-linewidth, linewidth, linewidth, getLocalBounds().getHeight()-linewidth);
    g.setColour (juce::Colours::black); g.drawRect(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
    //draw global bevel effect, either from the color or from the color of the blurred texture, and a black border
}

void PluginEditor::resized()
{
    
    auto area = getLocalBounds(); // this is a huge huge routine, but not all of it runs at all times!
    processorRef.pluginWidth = airwindowsLookAndFeel.userWidth = area.getWidth();
    processorRef.pluginHeight = airwindowsLookAndFeel.userHeight = area.getHeight();
    inspectButton.setBounds(0, 0, 10, 10);
    auto linewidth = (int)fmax(area.getHeight(),area.getWidth());
    linewidth = (int)cbrt(linewidth/2)-1;
    area.reduce(linewidth, linewidth);
    meter.displayWidth = (int)area.getWidth()-((linewidth/2)-2);
    meter.displayHeight = (int)(area.getHeight()*0.3819661f); //meter is inverse, controls are golden ratio
    if (meter.displayHeight > (float)meter.displayWidth*0.6180339f) meter.displayHeight = (int)(meter.displayWidth*0.6180339f);
    meter.setBounds(linewidth,linewidth,meter.displayWidth,meter.displayHeight);
    area.removeFromTop(meter.displayHeight); //remaining area is for controls. getProportion sets first start X and Y placement, then size X and Y placement
    float aspectRatio = (float)area.getWidth() / (float)area.getHeight(); //Larger than 1: horisontal. Smaller than 1: vertical
    // 12h-1w = 0.11  (0.26)  6h-2w = 0.41   4h-3w = 1.0    3h-4w = 1.8    2h-6w = 3.85     1h-12w = 15.42
    //float skew = airwindowsLookAndFeel.applyTilt; //this is the amount of tilt the knobs experience at top and bottom. MAX 0.5 becomes full tilt
    float lilKnob = 0.35f; float bigKnob = 0.65f; //float hugeKnob = 0.81f;
    float sliderW = 0.99f; float sliderH = 0.25f; //these are knob sizes scaled to the size of the block
    //these are always the same for all aspect ratios, but panels and offsets are unique to the aspect ratios, as are the sequence of control blocks
    
    //float A1x = 0.18f; float A1y = 0.05f; float A2x = 0.06f; float A3x = 0.59f; float A23y = 0.68f; //A is big knob and two smaller knobs below
    
    float B1x = 0.1f; float B1y = 0.33f; float B2x = 0.62f; float B2y = 0.1f; //B is big and small knob above right
    
    float C1234x = 0.01f; float C1y = 0.07f; float C2y = 0.29f; float C3y = 0.51f; float C4y = 0.73f; //C is the four slider dynamics bank, evenly spaced
    
    float D1x = 0.01f; float D1y = 0.07f; float D2x = 0.18f; float D2y = 0.35f; //D is one slider with a BigKnob under it: trim/more or pan/fader
    
    //float S1x = 0.1f; float S1y = 0.15f; //S is single knob
    
    if (aspectRatio >= 0.0f && aspectRatio < 0.25f) { //7h-1w
        float pY = (float)area.getHeight()/7.0f; //size of each movable block
        float pX = (float)area.getWidth()/1.0f;
        float offsetY = 0.0f; float offsetX = 0.0f; float panelTilt = 0.5f; //1 and 2 wide don't have tilted knobs
        HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
        offsetY = pY; //put between vertical SECTIONS (not knobs)
        
        HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
        offsetY = pY*2.0f; //put between vertical SECTIONS (not knobs)
        
        LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
        offsetY = pY*3.0f; //put between vertical SECTIONS (not knobs)
        
        BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
        offsetY = pY*4.0f; //put between vertical SECTIONS (not knobs)
        
        LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Lowpass Highpass B block
        offsetY = pY*5.0f; //put between vertical SECTIONS (not knobs)

       THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       GATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //Dynamics C block
       offsetY = pY*6.0f; //put between vertical SECTIONS (not knobs)

        PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
        FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D2y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()})); //Pan Fader D block
    } //7h-1w
    
    if (aspectRatio >= 0.25f && aspectRatio < 0.99f) { //43h-2w
        float pY = (float)area.getHeight()/4.0f; //left bank is 4 knobs tall
        float offsetY = 0.0f; float panelTilt = 0.5f; //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
        float pX = (float)area.getWidth()*0.45f;//left section width of 1/4 square
        float offsetX = 0.0f;
        HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
        offsetY += pY; //put between vertical SECTIONS (not knobs)

        HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
        offsetY += pY; //put between vertical SECTIONS (not knobs)
        
        LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
        offsetY += pY; //put between vertical SECTIONS (not knobs)
        
        BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block

        offsetX += pX; //put between vertical SECTIONS (not knobs)
        pX = (float)area.getWidth()*0.55f; //left section width of 1/3 square
        pY = (float)area.getHeight()/3.0f; //right bank is only 3 knobs tall
        offsetX = (float)area.getWidth()*0.45f; //begin where left section stops
        offsetY = 0.0f; //put between horizontal SECTIONS (not knobs)

        LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Lowpass Highpass B block
        offsetY += pY; //put between vertical SECTIONS (not knobs)

       THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       GATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //Dynamics C block
        offsetY += pY; //put between vertical SECTIONS (not knobs)

        PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
        FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D2y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()})); //Pan Fader D block
    } //43h-2w

    if (aspectRatio >= 0.99f && aspectRatio < 4.0f) { //2h-43w
        float pX = (float)area.getWidth()/4.0f; //top bank is 4 knobs wide
        float offsetX = 0.0f; float panelTilt = 0.5f; //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
        float pY = (float)area.getHeight()*0.45f;//top section height of 1/4 square
        float offsetY = 0.0f;
        BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)
        
        LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)
        
        HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)
        
        HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)
        pY = (float)area.getHeight()*0.55f; //bottom section height of 1/3 square
        pX = (float)area.getWidth()/3.0f; //bottom bank is only 3 knobs wide
        offsetY = (float)area.getHeight()*0.45f; //begin where top section stops
        offsetX = 0.0f; //put between vertical SECTIONS (not knobs)

        LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Lowpass Highpass B block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)

       THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       GATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //Dynamics C block
        offsetX += pX; //put between horizontal SECTIONS (not knobs)

        PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
        FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D2y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()})); //Pan Fader D block
    } //2h-43w
    
    
    if (aspectRatio >= 4.0f && aspectRatio < 999999.0f) { //1h-7w
        float pY = (float)area.getHeight()/1.0f; //size of each movable block
        float pX = (float)area.getWidth()/7.0f;
        float offsetY = 0.0f; float offsetX = 0.0f; float panelTilt = 0.5f; //update the new panel tilt each time offsetY is updated. 0.0 is top panel, seen from underneath. 1.0 is bottom panel, seen from above
        BSGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        BSFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        BSFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Bass and BassF B block
        offsetX = pX; //put between vertical SECTIONS (not knobs)
        
        LMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        LMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //LowMid and LowMidF B block
        offsetX = pX*2.0f; //put between vertical SECTIONS (not knobs)
        
        HMGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HMFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HMFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //HighMid and HighMidF B block
        offsetX = pX*3.0f; //put between vertical SECTIONS (not knobs)
        
        HIGKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIGKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIFKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIFKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //High and HighF B block
        offsetX = pX*4.0f; //put between vertical SECTIONS (not knobs)
        
        LOPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        LOPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B1x))/area.getWidth(), (offsetY+(pY*B1y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()}));
        HIPKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        HIPKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*B2x))/area.getWidth(), (offsetY+(pY*B2y))/area.getHeight(), (pX*lilKnob)/area.getWidth(), (pY*lilKnob)/area.getHeight()})); //Lowpass Highpass B block
        offsetX = pX*5.0f; //put between vertical SECTIONS (not knobs)

       THRKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       ATKKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C2y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       RLSKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C3y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
       GATKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*C1234x))/area.getWidth(), (offsetY+(pY*C4y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()})); //Dynamics C block
        offsetX = pX*6.0f; //put between vertical SECTIONS (not knobs)

        PANKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D1x))/area.getWidth(), (offsetY+(pY*D1y))/area.getHeight(), (pX*sliderW)/area.getWidth(), (pY*sliderH)/area.getHeight()}));
        FADKnob.setColour(juce::Slider::backgroundColourId, juce::Colour().fromFloatRGBA(panelTilt, 0.0f, 0.0f, 1.0f));
        FADKnob.setBounds(area.getProportion(juce::Rectangle{(offsetX+(pX*D2x))/area.getWidth(), (offsetY+(pY*D2y))/area.getHeight(), (pX*bigKnob)/area.getWidth(), (pY*bigKnob)/area.getHeight()})); //Pan Fader D block
    } //1h-7w
}

void PluginEditor::sliderValueChanged(juce::Slider *s)
{
    if (s == &TRMKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTRM;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MORKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMOR;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSGKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSG;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSFKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSF;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &THRKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTHR;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &ATKKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBATK;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RLSKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRLS;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &GATKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBGAT;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LOPKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLOP;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIPKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIP;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &PANKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBPAN;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
    if (s == &FADKnob)
    {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = PluginProcessor::UIToAudioMessage::NEW_VALUE;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBFAD;
        msg.newValue = (float)s->getValue();
        processorRef.uiToAudio.push(msg);
    }
}

void PluginEditor::sliderDragStarted(juce::Slider *s) {sliderDragInternal(s, true);}
void PluginEditor::sliderDragEnded(juce::Slider *s) {sliderDragInternal(s, false);}
void PluginEditor::sliderDragInternal(juce::Slider *s, bool bv)
{
    if (s == &TRMKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTRM;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &MORKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBMOR;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSGKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSG;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HMFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHMF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LMFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLMF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &BSFKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBBSF;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &THRKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBTHR;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &ATKKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBATK;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &RLSKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBRLS;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &GATKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBGAT;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &LOPKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBLOP;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &HIPKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBHIP;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &PANKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBPAN;
        processorRef.uiToAudio.push(msg);
    }
    if (s == &FADKnob) {
        PluginProcessor::UIToAudioMessage msg;
        msg.what = bv ? PluginProcessor::UIToAudioMessage::BEGIN_EDIT : PluginProcessor::UIToAudioMessage::END_EDIT;
        msg.which = (PluginProcessor::Parameters)PluginProcessor::KNOBFAD;
        processorRef.uiToAudio.push(msg);
    }
}

void PluginEditor::updateTrackProperties() {
    auto opt = processorRef.trackProperties.colour;
    if (opt.has_value()) hostTrackColour = *opt;
    auto optB = processorRef.trackProperties.name;
    if (optB.has_value()) hostTrackName = *optB;
    repaint();
}

void PluginEditor::updatePluginSize() {
    airwindowsLookAndFeel.userWidth = processorRef.pluginWidth;
    airwindowsLookAndFeel.userHeight = processorRef.pluginHeight;
    if (airwindowsLookAndFeel.userWidth < 8 || airwindowsLookAndFeel.userWidth > 16386) airwindowsLookAndFeel.userWidth = 618;
    if (airwindowsLookAndFeel.userHeight < 8 || airwindowsLookAndFeel.userHeight > 16386) airwindowsLookAndFeel.userHeight = 375;
    repaint();
}



void PluginEditor::idle()
{
    PluginProcessor::AudioToUIMessage msg;
    bool repaintTS{false}; //we don't redraw interface just for getting data into the GUI section
    while (processorRef.audioToUI.pop(msg)) {
        switch (msg.what) {
        case PluginProcessor::AudioToUIMessage::NEW_VALUE:
                if (msg.which == PluginProcessor::KNOBTRM) {TRMKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBMOR) {MORKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHIG) {HIGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHMG) {HMGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLMG) {LMGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBBSG) {BSGKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHIF) {HIFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHMF) {HMFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLMF) {LMFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBBSF) {BSFKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBTHR) {THRKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBATK) {ATKKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBRLS) {RLSKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBGAT) {GATKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBLOP) {LOPKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBHIP) {HIPKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBPAN) {PANKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                if (msg.which == PluginProcessor::KNOBFAD) {FADKnob.setValue(msg.newValue, juce::NotificationType::dontSendNotification); break;}
                break; //this can grab the knobs away from the user! Should cause the knob to repaint, too.
                
        case PluginProcessor::AudioToUIMessage::RMS_LEFT: meter.pushA(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::RMS_RIGHT: meter.pushB(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::PEAK_LEFT: meter.pushC(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::PEAK_RIGHT: meter.pushD(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::SLEW_LEFT: meter.pushE(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::SLEW_RIGHT: meter.pushF(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::ZERO_LEFT: meter.pushG(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::ZERO_RIGHT: meter.pushH(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::BLINKEN_COMP: meter.pushI(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::BLINKEN_GATE: meter.pushJ(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::BLINKEN_ATK: meter.pushK(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::BLINKEN_RLS: meter.pushL(msg.newValue); break;
        case PluginProcessor::AudioToUIMessage::INCREMENT: //Increment is running at 24 FPS and giving the above calculations
                meter.pushIncrement(); repaintTS = true; //we will repaint GUI after doing the following
                
                THRKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenComp*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                ATKKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenAttack*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenAttack*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenAttack*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                RLSKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenRelease*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenRelease*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenRelease*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));
                GATKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.blinkenGate*airwindowsLookAndFeel.LEDColour.getFloatRed(), meter.blinkenGate*airwindowsLookAndFeel.LEDColour.getFloatGreen(), meter.blinkenGate*airwindowsLookAndFeel.LEDColour.getFloatBlue(), 1.0f));

                //User color LEDS are done like this: choose the same meter.data selection for each, and then the color will always be the user color
                //SGTKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA (meter.inputRMSL, meter.inputRMSL, meter.inputRMSL, 1.0f));
                //for an RGB or specified color blinken-knob, we don't reference user color, instead we just use the multiple meter.data directly without bringing in LEDColour
                
                //here is where we can make any control's thumb be a continuous blinkenlight with any value in meter. It runs at about 24fps.
                //We can do if statements etc. here, only thing we can NOT do is instantiate new variables. Do it as shown above and it works.
                //Also, this defaults to colors flashing against black, like LEDs: that's a good way to distinguish blinken-knobs from knobs
                break;
        default: break;
        } //end of switch statement for msg.what
    } if (repaintTS) meter.repaint();
}
