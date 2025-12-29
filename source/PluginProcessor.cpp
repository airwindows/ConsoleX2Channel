#include "PluginProcessor.h"
#include "PluginEditor.h"
#ifndef M_PI
#  define M_PI (3.14159265358979323846)
#endif
#ifndef M_PI_2
#  define M_PI_2 (1.57079632679489661923)
#endif

//==============================================================================
PluginProcessor::PluginProcessor():AudioProcessor (
                    BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
){
    for (int x = 0; x < biq_total; x++) {
        highA[x] = 0.0;
        highB[x] = 0.0;
        highC[x] = 0.0;
        midA[x] = 0.0;
        midB[x] = 0.0;
        midC[x] = 0.0;
        lowA[x] = 0.0;
        lowB[x] = 0.0;
        lowC[x] = 0.0;
    }
    highLIIR = 0.0;
    highRIIR = 0.0;
    midLIIR = 0.0;
    midRIIR = 0.0;
    lowLIIR = 0.0;
    lowRIIR = 0.0;
    //SmoothEQ2
    
    for (int x = 0; x < bez_total; x++) bezComp[x] = 0.0;
    bezComp[bez_cycle] = 1.0; bezMax = 0.0; bezMin = 0.0;
    bezGate = 2.0;
    //Dynamics3
    
    for(int count = 0; count < 22; count++) {
        iirHPositionL[count] = 0.0;
        iirHAngleL[count] = 0.0;
        iirHPositionR[count] = 0.0;
        iirHAngleR[count] = 0.0;
    }
    hBypass = false;
    
    for(int count = 0; count < 14; count++) {
        iirLPositionL[count] = 0.0;
        iirLAngleL[count] = 0.0;
        iirLPositionR[count] = 0.0;
        iirLAngleR[count] = 0.0;
    }
    lBypass = false;
    //Cabs2
    
    for(int count = 0; count < dscBuf+2; count++) {
        dBaL[count] = 0.0;
        dBaR[count] = 0.0;
    }
    dBaPosL = 0.0;
    dBaPosR = 0.0;
    dBaXL = 1;
    dBaXR = 1;
    //Discontapeity
    
    for (int x = 0; x < 33; x++) {avg32L[x] = 0.0; avg32R[x] = 0.0;}
    for (int x = 0; x < 17; x++) {avg16L[x] = 0.0; avg16R[x] = 0.0;}
    for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; avg8R[x] = 0.0;}
    for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; avg4R[x] = 0.0;}
    for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; avg2R[x] = 0.0;}
    avgPos = 0;
    lastSlewL = 0.0; lastSlewR = 0.0;
    lastSlewpleL = 0.0; lastSlewpleR = 0.0;
    //preTapeHack
    
    lFreqA = 1.0; lFreqB = 1.0;
    hFreqA = 0.0; hFreqB = 0.0;
    panA = 0.5; panB = 0.5;
    inTrimA = 0.5; inTrimB = 0.5;

    fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
    fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
    //this is reset: values being initialized only once. Startup values, whatever they are.

    // (internal ID, how it's shown in DAW generic view, {min, max}, default)
    addParameter(params[KNOBTRM] = new juce::AudioParameterFloat("trim", "Trim", {0.0f, 1.0f}, 0.25f));            params[KNOBTRM]->addListener(this);
    addParameter(params[KNOBMOR] = new juce::AudioParameterFloat("more", "More", {0.0f, 1.0f}, 0.0f));             params[KNOBMOR]->addListener(this);
    addParameter(params[KNOBHIG] = new juce::AudioParameterFloat("high", "High", {0.0f, 1.0f}, 0.5f));             params[KNOBHIG]->addListener(this);
    addParameter(params[KNOBHMG] = new juce::AudioParameterFloat("hmid", "HMid", {0.0f, 1.0f}, 0.5f));             params[KNOBHMG]->addListener(this);
    addParameter(params[KNOBLMG] = new juce::AudioParameterFloat("lmid", "LMid", {0.0f, 1.0f}, 0.5f));             params[KNOBLMG]->addListener(this);
    addParameter(params[KNOBBSG] = new juce::AudioParameterFloat("bass", "Bass", {0.0f, 1.0f}, 0.5f));             params[KNOBBSG]->addListener(this);
    addParameter(params[KNOBHIF] = new juce::AudioParameterFloat("highf", "HighF", {0.0f, 1.0f}, 0.5f));           params[KNOBHIF]->addListener(this);
    addParameter(params[KNOBHMF] = new juce::AudioParameterFloat("hmidf", "HMidF", {0.0f, 1.0f}, 0.5f));           params[KNOBHMF]->addListener(this);
    addParameter(params[KNOBLMF] = new juce::AudioParameterFloat("lmidf", "LMidF", {0.0f, 1.0f}, 0.5f));           params[KNOBLMF]->addListener(this);
    addParameter(params[KNOBBSF] = new juce::AudioParameterFloat("bassf", "BassF", {0.0f, 1.0f}, 0.5f));           params[KNOBBSF]->addListener(this);
    addParameter(params[KNOBTHR] = new juce::AudioParameterFloat("thresh", "Threshold", {0.0f, 1.0f}, 1.0f));      params[KNOBTHR]->addListener(this);
    addParameter(params[KNOBATK] = new juce::AudioParameterFloat("attack", "Attack", {0.0f, 1.0f}, 0.5f));         params[KNOBATK]->addListener(this);
    addParameter(params[KNOBRLS] = new juce::AudioParameterFloat("release", "Release", {0.0f, 1.0f}, 0.5f));       params[KNOBRLS]->addListener(this);
    addParameter(params[KNOBGAT] = new juce::AudioParameterFloat("gate", "Gate", {0.0f, 1.0f}, 0.0f));             params[KNOBGAT]->addListener(this);
    addParameter(params[KNOBLOP] = new juce::AudioParameterFloat("lowpass", "Lowpass", {0.0f, 1.0f}, 1.0f));       params[KNOBLOP]->addListener(this);
    addParameter(params[KNOBHIP] = new juce::AudioParameterFloat("highpass", "Highpass", {0.0f, 1.0f}, 0.0f));     params[KNOBHIP]->addListener(this);
    addParameter(params[KNOBPAN] = new juce::AudioParameterFloat("pan", "Pan", {0.0f, 1.0f}, 0.5f));               params[KNOBPAN]->addListener(this);
    addParameter(params[KNOBFAD] = new juce::AudioParameterFloat("fader", "Fader", {0.0f, 1.0f}, 0.5f));           params[KNOBFAD]->addListener(this);
}

PluginProcessor::~PluginProcessor() {}
void PluginProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
    AudioToUIMessage msg;
    msg.what = AudioToUIMessage::NEW_VALUE;
    msg.which = (PluginProcessor::Parameters)parameterIndex;
    msg.newValue = params[parameterIndex]->convertFrom0to1(newValue);
    audioToUI.push(msg);
}
void PluginProcessor::parameterGestureChanged(int parameterIndex, bool starting) {}
const juce::String PluginProcessor::getName() const {return JucePlugin_Name;}
bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::supportsDoublePrecisionProcessing() const
{
   #if JucePlugin_SupportsDoublePrecisionProcessing
    return true;
   #else
    return true;
    //note: I don't know whether that config option is set, so I'm hardcoding it
    //knowing I have enabled such support: keeping the boilerplate stuff tho
    //in case I can sort out where it's enabled as a flag
   #endif
}
bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double PluginProcessor::getTailLengthSeconds() const {return 0.0;}
int PluginProcessor::getNumPrograms() {return 1;}
int PluginProcessor::getCurrentProgram() {return 0;}
void PluginProcessor::setCurrentProgram (int index) {juce::ignoreUnused (index);}
const juce::String PluginProcessor::getProgramName (int index) {juce::ignoreUnused (index); return {};}
void PluginProcessor::changeProgramName (int index, const juce::String& newName) {juce::ignoreUnused (index, newName);}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {juce::ignoreUnused (sampleRate, samplesPerBlock);}
void PluginProcessor::releaseResources() {}
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this metering code we only support stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

//==============================================================================

bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("consolex2channel"));
    xml->setAttribute("streamingVersion", (int)8524);

    for (unsigned long i = 0; i < n_params; ++i)
    {
        juce::String nm = juce::String("awcx2c_") + std::to_string(i);
        float val = 0.0f; if (i < n_params) val = *(params[i]);
        xml->setAttribute(nm, val);
    }
    if (pluginWidth < 8 || pluginWidth > 16386) pluginWidth = 618;
    xml->setAttribute(juce::String("awcx2c_width"), pluginWidth);
    if (pluginHeight < 8 || pluginHeight > 16386) pluginHeight = 375;
    xml->setAttribute(juce::String("awcx2c_height"), pluginHeight);
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("consolex2channel"))
        {
            for (unsigned long i = 0; i < n_params; ++i)
            {
                juce::String nm = juce::String("awcx2c_") + std::to_string(i);
                auto f = xmlState->getDoubleAttribute(nm);
                params[i]->setValueNotifyingHost((float)f);
            }
            auto w = xmlState->getIntAttribute(juce::String("awcx2c_width"));
            if (w < 8 || w > 16386) w = 618;
            auto h = xmlState->getIntAttribute(juce::String("awcx2c_height"));
            if (h < 8 || h > 16386) h = 375;
            updatePluginSize(w, h);
        }
        updateHostDisplay();
    }
    //These functions are adapted (simplified) from baconpaul's airwin2rack and all thanks to getting
    //it working shall go there, though sudara or anyone could've spotted that I hadn't done these.
    //baconpaul pointed me to the working versions in airwin2rack, that I needed to see.
}

void PluginProcessor::updateTrackProperties(const TrackProperties& properties)
{
    trackProperties = properties;
    // call the version in the editor to update there
    if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        editor->updateTrackProperties();
}

void PluginProcessor::updatePluginSize(int w, int h)
{
    pluginWidth = w;
    pluginHeight = h;
    // call the version in the editor to update there
    if (auto* editor = dynamic_cast<PluginEditor*> (getActiveEditor()))
        editor->updatePluginSize();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}


void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (!(getBus(false, 0)->isEnabled() && getBus(true, 0)->isEnabled())) return;
    auto mainOutput = getBusBuffer(buffer, false, 0); //if we have audio busses at all,
    auto mainInput = getBusBuffer(buffer, true, 0); //they're now mainOutput and mainInput.
    
    UIToAudioMessage uim;
    while (uiToAudio.pop(uim)) {
        switch (uim.what) {
        case UIToAudioMessage::NEW_VALUE: params[uim.which]->setValueNotifyingHost(params[uim.which]->convertTo0to1(uim.newValue)); break;
        case UIToAudioMessage::BEGIN_EDIT: params[uim.which]->beginChangeGesture(); break;
        case UIToAudioMessage::END_EDIT: params[uim.which]->endChangeGesture(); break;
        }
    } //Handle inbound messages from the UI thread

    double rmsSize = (1881.0 / 44100.0)*getSampleRate(); //higher is slower with larger RMS buffers
    double zeroCrossScale = (1.0 / getSampleRate())*44100.0;
    int inFramesToProcess = buffer.getNumSamples(); //vst doesn't give us this as a separate variable so we'll make it
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();
	int spacing = floor(overallscale*2.0);
	if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
    
    double moreTapeHack = (params[KNOBMOR]->get()*2.0)+1.0;
    bool tapehackOff = (params[KNOBMOR]->get() == 0.0);
    switch ((int)(params[KNOBTRM]->get()*4.0)){
        case 0: moreTapeHack *= 0.5; break;
        case 1: break;
        case 2: moreTapeHack *= 2.0; break;
        case 3: moreTapeHack *= 4.0; break;
        case 4: moreTapeHack *= 8.0; break;
    }
    double moreDiscontinuity = fmax(pow(params[KNOBMOR]->get()*0.42,3.0)*overallscale,0.00001);
    //Discontapeity
    
    double trebleGain = (params[KNOBHIG]->get()-0.5)*2.0;
    trebleGain = 1.0+(trebleGain*fabs(trebleGain)*fabs(trebleGain));
    double highmidGain = (params[KNOBHMG]->get()-0.5)*2.0;
    highmidGain = 1.0+(highmidGain*fabs(highmidGain)*fabs(highmidGain));
    double lowmidGain = (params[KNOBLMG]->get()-0.5)*2.0;
    lowmidGain = 1.0+(lowmidGain*fabs(lowmidGain)*fabs(lowmidGain));
    double bassGain = (params[KNOBBSG]->get()-0.5)*2.0;
    bassGain = 1.0+(bassGain*fabs(bassGain)*fabs(bassGain));
    double highCoef = 0.0;
    double midCoef = 0.0;
    double lowCoef = 0.0;
    
    bool eqOff = (trebleGain == 1.0 && highmidGain == 1.0 && lowmidGain == 1.0 && bassGain == 1.0);
    //we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
    //it cancels out to bit-identical anyhow, but we get to skip the calculation
    if (!eqOff) {
        double trebleRef = params[KNOBHIF]->get()-0.5;
        double highmidRef = params[KNOBHMF]->get()-0.5;
        double lowmidRef = params[KNOBLMF]->get()-0.5;
        double bassRef = params[KNOBBSF]->get()-0.5;
        double highF = 0.75 + ((trebleRef+trebleRef+trebleRef+highmidRef)*0.125);
        double bassF = 0.25 + ((lowmidRef+bassRef+bassRef+bassRef)*0.125);
        double midF = (highF*0.5) + (bassF*0.5) + ((highmidRef+lowmidRef)*0.125);
        
        double highQ = fmax(fmin(1.0+(highmidRef-trebleRef),4.0),0.125);
        double midQ = fmax(fmin(1.0+(lowmidRef-highmidRef),4.0),0.125);
        double lowQ = fmax(fmin(1.0+(bassRef-lowmidRef),4.0),0.125);
        
        highA[biq_freq] = ((pow(highF,3)*20000.0)/getSampleRate());
        highC[biq_freq] = highB[biq_freq] = highA[biq_freq] = fmax(fmin(highA[biq_freq],0.4999),0.00025);
        double highFreq = pow(highF,3)*20000.0;
        double omega = 2.0*M_PI*(highFreq/getSampleRate());
        double biqK = 2.0-cos(omega);
        highCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        highA[biq_reso] = 2.24697960 * highQ;
        highB[biq_reso] = 0.80193774 * highQ;
        highC[biq_reso] = 0.55495813 * highQ;
        
        midA[biq_freq] = ((pow(midF,3)*20000.0)/getSampleRate());
        midC[biq_freq] = midB[biq_freq] = midA[biq_freq] = fmax(fmin(midA[biq_freq],0.4999),0.00025);
        double midFreq = pow(midF,3)*20000.0;
        omega = 2.0*M_PI*(midFreq/getSampleRate());
        biqK = 2.0-cos(omega);
        midCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        midA[biq_reso] = 2.24697960 * midQ;
        midB[biq_reso] = 0.80193774 * midQ;
        midC[biq_reso] = 0.55495813 * midQ;
        
        lowA[biq_freq] = ((pow(bassF,3)*20000.0)/getSampleRate());
        lowC[biq_freq] = lowB[biq_freq] = lowA[biq_freq] = fmax(fmin(lowA[biq_freq],0.4999),0.00025);
        double lowFreq = pow(bassF,3)*20000.0;
        omega = 2.0*M_PI*(lowFreq/getSampleRate());
        biqK = 2.0-cos(omega);
        lowCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        lowA[biq_reso] = 2.24697960 * lowQ;
        lowB[biq_reso] = 0.80193774 * lowQ;
        lowC[biq_reso] = 0.55495813 * lowQ;
        
        biqK = tan(M_PI * highA[biq_freq]);
        double norm = 1.0 / (1.0 + biqK / highA[biq_reso] + biqK * biqK);
        highA[biq_a0] = biqK * biqK * norm;
        highA[biq_a1] = 2.0 * highA[biq_a0];
        highA[biq_a2] = highA[biq_a0];
        highA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highA[biq_b2] = (1.0 - biqK / highA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * highB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / highB[biq_reso] + biqK * biqK);
        highB[biq_a0] = biqK * biqK * norm;
        highB[biq_a1] = 2.0 * highB[biq_a0];
        highB[biq_a2] = highB[biq_a0];
        highB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highB[biq_b2] = (1.0 - biqK / highB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * highC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / highC[biq_reso] + biqK * biqK);
        highC[biq_a0] = biqK * biqK * norm;
        highC[biq_a1] = 2.0 * highC[biq_a0];
        highC[biq_a2] = highC[biq_a0];
        highC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highC[biq_b2] = (1.0 - biqK / highC[biq_reso] + biqK * biqK) * norm;
        
        biqK = tan(M_PI * midA[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midA[biq_reso] + biqK * biqK);
        midA[biq_a0] = biqK * biqK * norm;
        midA[biq_a1] = 2.0 * midA[biq_a0];
        midA[biq_a2] = midA[biq_a0];
        midA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midA[biq_b2] = (1.0 - biqK / midA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * midB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midB[biq_reso] + biqK * biqK);
        midB[biq_a0] = biqK * biqK * norm;
        midB[biq_a1] = 2.0 * midB[biq_a0];
        midB[biq_a2] = midB[biq_a0];
        midB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midB[biq_b2] = (1.0 - biqK / midB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * midC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midC[biq_reso] + biqK * biqK);
        midC[biq_a0] = biqK * biqK * norm;
        midC[biq_a1] = 2.0 * midC[biq_a0];
        midC[biq_a2] = midC[biq_a0];
        midC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midC[biq_b2] = (1.0 - biqK / midC[biq_reso] + biqK * biqK) * norm;
        
        biqK = tan(M_PI * lowA[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowA[biq_reso] + biqK * biqK);
        lowA[biq_a0] = biqK * biqK * norm;
        lowA[biq_a1] = 2.0 * lowA[biq_a0];
        lowA[biq_a2] = lowA[biq_a0];
        lowA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowA[biq_b2] = (1.0 - biqK / lowA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * lowB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowB[biq_reso] + biqK * biqK);
        lowB[biq_a0] = biqK * biqK * norm;
        lowB[biq_a1] = 2.0 * lowB[biq_a0];
        lowB[biq_a2] = lowB[biq_a0];
        lowB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowB[biq_b2] = (1.0 - biqK / lowB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * lowC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowC[biq_reso] + biqK * biqK);
        lowC[biq_a0] = biqK * biqK * norm;
        lowC[biq_a1] = 2.0 * lowC[biq_a0];
        lowC[biq_a2] = lowC[biq_a0];
        lowC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowC[biq_b2] = (1.0 - biqK / lowC[biq_reso] + biqK * biqK) * norm;
    }
    //SmoothEQ2
    
    double bezThresh = pow(1.0-params[KNOBTHR]->get(), 4.0) * 8.0;
    double bezRez = pow(1.0-params[KNOBATK]->get(), 4.0) / overallscale;
    double sloRez = pow(1.0-params[KNOBRLS]->get(), 4.0) / overallscale;
    double gate = pow(params[KNOBGAT]->get(),4.0);
    bezRez = fmin(fmax(bezRez,0.0001),1.0);
    sloRez = fmin(fmax(sloRez,0.0001),1.0);
    //Dynamics3
    
    lFreqA = lFreqB; lFreqB = pow(fmax(params[KNOBLOP]->get(),0.002),overallscale); //the lowpass
    hFreqA = hFreqB; hFreqB = pow(params[KNOBHIP]->get(),overallscale+2.0); //the highpass
    //Cabs2
    
    panA = panB; panB = params[KNOBPAN]->get()*1.57079633;
    inTrimA = inTrimB; inTrimB = params[KNOBFAD]->get()*2.0;
    //Console
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto outL = mainOutput.getWritePointer(0, i);
        auto outR = mainOutput.getWritePointer(1, i);
        auto inL = mainInput.getReadPointer(0, i); //in isBussesLayoutSupported, we have already
        auto inR = mainInput.getReadPointer(1, i); //specified that we can only be stereo and never mono
        long double inputSampleL = *inL;
        long double inputSampleR = *inR;
        if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
        //NOTE: I don't yet know whether counting the buffer backwards means that gainA and gainB must be reversed.
        //If the audio flow is in fact reversed we must swap the temp and (1.0-temp) and I have done this provisionally
        //Original VST2 is counting DOWN with -- operator, but this counts UP with ++
        //If this doesn't create zipper noise on sine processing then it's correct
		
        inputSampleL *= moreTapeHack;
        inputSampleR *= moreTapeHack;
        //trim control gets to work even when MORE is off
        
        if (!tapehackOff) {
            double darkSampleL = inputSampleL;
            double darkSampleR = inputSampleR;
            if (avgPos > 31) avgPos = 0;
            if (spacing > 31) {
                avg32L[avgPos] = darkSampleL; avg32R[avgPos] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {darkSampleL += avg32L[x]; darkSampleR += avg32R[x];}
                darkSampleL /= 32.0; darkSampleR /= 32.0;
            } if (spacing > 15) {
                avg16L[avgPos%16] = darkSampleL; avg16R[avgPos%16] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {darkSampleL += avg16L[x]; darkSampleR += avg16R[x];}
                darkSampleL /= 16.0; darkSampleR /= 16.0;
            } if (spacing > 7) {
                avg8L[avgPos%8] = darkSampleL; avg8R[avgPos%8] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x]; darkSampleR += avg8R[x];}
                darkSampleL /= 8.0; darkSampleR /= 8.0;
            } if (spacing > 3) {
                avg4L[avgPos%4] = darkSampleL; avg4R[avgPos%4] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x]; darkSampleR += avg4R[x];}
                darkSampleL /= 4.0; darkSampleR /= 4.0;
            } if (spacing > 1) {
                avg2L[avgPos%2] = darkSampleL; avg2R[avgPos%2] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x]; darkSampleR += avg2R[x];}
                darkSampleL /= 2.0; darkSampleR /= 2.0;
            } avgPos++;
            lastSlewL += fabs(lastSlewpleL-inputSampleL); lastSlewpleL = inputSampleL;
            double avgSlewL = fmin(lastSlewL*lastSlewL*(0.0635-(overallscale*0.0018436)),1.0);
            lastSlewL = fmax(lastSlewL*0.78,2.39996322972865332223);
            lastSlewR += fabs(lastSlewpleR-inputSampleR); lastSlewpleR = inputSampleR;
            double avgSlewR = fmin(lastSlewR*lastSlewR*(0.0635-(overallscale*0.0018436)),1.0);
            lastSlewR = fmax(lastSlewR*0.78,2.39996322972865332223); //look up Golden Angle, it's cool
            inputSampleL = (inputSampleL*(1.0-avgSlewL)) + (darkSampleL*avgSlewL);
            inputSampleR = (inputSampleR*(1.0-avgSlewR)) + (darkSampleR*avgSlewR);
            //begin Discontinuity section
            inputSampleL *= moreDiscontinuity;
            dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
            dBaPosL = fmin(dBaPosL,1.0);
            int dBdly = floor(dBaPosL*dscBuf);
            double dBi = (dBaPosL*dscBuf)-dBdly;
            inputSampleL = dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
            dBdly++; inputSampleL += dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
            dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
            inputSampleL /= moreDiscontinuity;
            //end Discontinuity section, begin TapeHack section
            inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
            double addtwo = inputSampleL * inputSampleL;
            double empower = inputSampleL * addtwo; // inputSampleL to the third power
            inputSampleL -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleL += (empower / 69.0);
            empower *= addtwo; //seventh
            inputSampleL -= (empower / 2530.08);
            empower *= addtwo; //ninth
            inputSampleL += (empower / 224985.6);
            empower *= addtwo; //eleventh
            inputSampleL -= (empower / 9979200.0f);
            //this is a degenerate form of a Taylor Series to approximate sin()
            //end TapeHack section
            //begin Discontinuity section
            inputSampleR *= moreDiscontinuity;
            dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25)-0.5))*0.5);
            dBaPosR = fmin(dBaPosR,1.0);
            dBdly = floor(dBaPosR*dscBuf);
            dBi = (dBaPosR*dscBuf)-dBdly;
            inputSampleR = dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
            dBdly++; inputSampleR += dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
            dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
            inputSampleR /= moreDiscontinuity;
            //end Discontinuity section, begin TapeHack section
            inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
            addtwo = inputSampleR * inputSampleR;
            empower = inputSampleR * addtwo; // inputSampleR to the third power
            inputSampleR -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleR += (empower / 69.0);
            empower *= addtwo; //seventh
            inputSampleR -= (empower / 2530.08);
            empower *= addtwo; //ninth
            inputSampleR += (empower / 224985.6);
            empower *= addtwo; //eleventh
            inputSampleR -= (empower / 9979200.0f);
            //this is a degenerate form of a Taylor Series to approximate sin()
            //end TapeHack section
            //Discontapeity
        }
        
        if (!eqOff) {
            double trebleL = inputSampleL;
            double outSample = (trebleL * highA[biq_a0]) + highA[biq_sL1];
            highA[biq_sL1] = (trebleL * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sL2];
            highA[biq_sL2] = (trebleL * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midA[biq_a0]) + midA[biq_sL1];
            midA[biq_sL1] = (highmidL * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sL2];
            midA[biq_sL2] = (highmidL * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowA[biq_a0]) + lowA[biq_sL1];
            lowA[biq_sL1] = (lowmidL * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sL2];
            lowA[biq_sL2] = (lowmidL * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //first stage of three crossovers
            
            outSample = (trebleL * highB[biq_a0]) + highB[biq_sL1];
            highB[biq_sL1] = (trebleL * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sL2];
            highB[biq_sL2] = (trebleL * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midB[biq_a0]) + midB[biq_sL1];
            midB[biq_sL1] = (highmidL * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sL2];
            midB[biq_sL2] = (highmidL * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowB[biq_a0]) + lowB[biq_sL1];
            lowB[biq_sL1] = (lowmidL * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sL2];
            lowB[biq_sL2] = (lowmidL * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //second stage of three crossovers
            
            outSample = (trebleL * highC[biq_a0]) + highC[biq_sL1];
            highC[biq_sL1] = (trebleL * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sL2];
            highC[biq_sL2] = (trebleL * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midC[biq_a0]) + midC[biq_sL1];
            midC[biq_sL1] = (highmidL * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sL2];
            midC[biq_sL2] = (highmidL * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowC[biq_a0]) + lowC[biq_sL1];
            lowC[biq_sL1] = (lowmidL * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sL2];
            lowC[biq_sL2] = (lowmidL * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //third stage of three crossovers
            
            highLIIR = (highLIIR*highCoef) + (trebleL*(1.0-highCoef));
            highmidL = highLIIR; trebleL -= highmidL;
            
            midLIIR = (midLIIR*midCoef) + (highmidL*(1.0-midCoef));
            lowmidL = midLIIR; highmidL -= lowmidL;
            
            lowLIIR = (lowLIIR*lowCoef) + (lowmidL*(1.0-lowCoef));
            bassL = lowLIIR; lowmidL -= bassL;
            
            inputSampleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //fourth stage of three crossovers is the exponential filters
            
            
            double trebleR = inputSampleR;
            outSample = (trebleR * highA[biq_a0]) + highA[biq_sR1];
            highA[biq_sR1] = (trebleR * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sR2];
            highA[biq_sR2] = (trebleR * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midA[biq_a0]) + midA[biq_sR1];
            midA[biq_sR1] = (highmidR * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sR2];
            midA[biq_sR2] = (highmidR * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowA[biq_a0]) + lowA[biq_sR1];
            lowA[biq_sR1] = (lowmidR * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sR2];
            lowA[biq_sR2] = (lowmidR * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //first stage of three crossovers
            
            outSample = (trebleR * highB[biq_a0]) + highB[biq_sR1];
            highB[biq_sR1] = (trebleR * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sR2];
            highB[biq_sR2] = (trebleR * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midB[biq_a0]) + midB[biq_sR1];
            midB[biq_sR1] = (highmidR * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sR2];
            midB[biq_sR2] = (highmidR * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowB[biq_a0]) + lowB[biq_sR1];
            lowB[biq_sR1] = (lowmidR * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sR2];
            lowB[biq_sR2] = (lowmidR * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //second stage of three crossovers
            
            outSample = (trebleR * highC[biq_a0]) + highC[biq_sR1];
            highC[biq_sR1] = (trebleR * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sR2];
            highC[biq_sR2] = (trebleR * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midC[biq_a0]) + midC[biq_sR1];
            midC[biq_sR1] = (highmidR * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sR2];
            midC[biq_sR2] = (highmidR * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowC[biq_a0]) + lowC[biq_sR1];
            lowC[biq_sR1] = (lowmidR * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sR2];
            lowC[biq_sR2] = (lowmidR * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //third stage of three crossovers
            
            highRIIR = (highRIIR*highCoef) + (trebleR*(1.0-highCoef));
            highmidR = highRIIR; trebleR -= highmidR;
            
            midRIIR = (midRIIR*midCoef) + (highmidR*(1.0-midCoef));
            lowmidR = midRIIR; highmidR -= lowmidR;
            
            lowRIIR = (lowRIIR*lowCoef) + (lowmidR*(1.0-lowCoef));
            bassR = lowRIIR; lowmidR -= bassR;
            
            inputSampleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //fourth stage of three crossovers is the exponential filters
        }
        //SmoothEQ2
        
        if (baseComp < fabs(inputSampleR*64.0)) baseComp = fmin(fabs(inputSampleR*64.0),1.0);
        if (baseComp < fabs(inputSampleL*64.0)) baseComp = fmin(fabs(inputSampleL*64.0),1.0);
        if (baseGate < fabs(inputSampleR*64.0)) baseGate = fmin(fabs(inputSampleR*64.0),1.0);
        if (baseGate < fabs(inputSampleL*64.0)) baseGate = fmin(fabs(inputSampleL*64.0),1.0);
        //here we make the comp and gate lights light up anytime there is signal,
        //and either compression or gating DIM the lights in question.
        
        if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
        else bezGate = fmax(0.000001, bezGate-fmin(bezRez,sloRez));
        if (maxGate > bezGate) maxGate = bezGate;

        double ctrl = fmax(fabs(inputSampleL),fabs(inputSampleR));
        maxAttack = fmax(fmin(maxAttack+((ctrl>bezThresh)?-bezRez:bezRez),1.0),0.0);
        maxRelease = fmax(fmin(maxRelease+((ctrl>bezThresh)?-sloRez:maxAttack),1.0),0.0);
        //metering blinkenlights
        
        if (bezThresh > 0.0) {
            inputSampleL *= (bezThresh+1.0);
            inputSampleR *= (bezThresh+1.0);
            ctrl = fmax(fabs(inputSampleL),fabs(inputSampleR));
            bezMax = fmax(bezMax,ctrl);
            bezMin = fmax(bezMin-sloRez,ctrl);
            bezComp[bez_cycle] += bezRez;
            bezComp[bez_Ctrl] += (bezMin * bezRez);
            if (bezComp[bez_cycle] > 1.0) {
                if (bezGate < 1.0) bezComp[bez_Ctrl] /= bezGate;
                bezComp[bez_cycle] -= 1.0;
                bezComp[bez_C] = bezComp[bez_B];
                bezComp[bez_B] = bezComp[bez_A];
                bezComp[bez_A] = bezComp[bez_Ctrl];
                bezComp[bez_Ctrl] = 0.0;
                bezMax = 0.0;
            }
            double CB = (bezComp[bez_C]*(1.0-bezComp[bez_cycle]))+(bezComp[bez_B]*bezComp[bez_cycle]);
            double BA = (bezComp[bez_B]*(1.0-bezComp[bez_cycle]))+(bezComp[bez_A]*bezComp[bez_cycle]);
            double CBA = (bezComp[bez_B]+(CB*(1.0-bezComp[bez_cycle]))+(BA*bezComp[bez_cycle]))*0.5;
            inputSampleL *= 1.0-(fmin(CBA*bezThresh,1.0));
            inputSampleR *= 1.0-(fmin(CBA*bezThresh,1.0));
            if (maxComp > 1.0-pow(CBA,2.0)) maxComp = 1.0-pow(CBA,2.0);
        } else bezComp[bez_Ctrl] = 0.0;
        //Dynamics3
        
        const double temp = (double)i/inFramesToProcess;
        const double hFreq = (hFreqA*temp)+(hFreqB*(1.0-temp));
        if (hFreq > 0.0) {
            double lowSampleL = inputSampleL;
            double lowSampleR = inputSampleR;
            for(int count = 0; count < 21; count++) {
                iirHAngleL[count] = (iirHAngleL[count]*(1.0-hFreq))+((lowSampleL-iirHPositionL[count])*hFreq);
                lowSampleL = ((iirHPositionL[count]+(iirHAngleL[count]*hFreq))*(1.0-hFreq))+(lowSampleL*hFreq);
                iirHPositionL[count] = ((iirHPositionL[count]+(iirHAngleL[count]*hFreq))*(1.0-hFreq))+(lowSampleL*hFreq);
                inputSampleL -= (lowSampleL * (1.0/21.0));//left
                iirHAngleR[count] = (iirHAngleR[count]*(1.0-hFreq))+((lowSampleR-iirHPositionR[count])*hFreq);
                lowSampleR = ((iirHPositionR[count]+(iirHAngleR[count]*hFreq))*(1.0-hFreq))+(lowSampleR*hFreq);
                iirHPositionR[count] = ((iirHPositionR[count]+(iirHAngleR[count]*hFreq))*(1.0-hFreq))+(lowSampleR*hFreq);
                inputSampleR -= (lowSampleR * (1.0/21.0));//right
            } //the highpass
            hBypass = false;
        } else {
            if (!hBypass) {
                hBypass = true;
                for(int count = 0; count < 22; count++) {
                    iirHPositionL[count] = 0.0;
                    iirHAngleL[count] = 0.0;
                    iirHPositionR[count] = 0.0;
                    iirHAngleR[count] = 0.0;
                }
            } //blank out highpass if jut switched off
        }
        const double lFreq = (lFreqA*temp)+(lFreqB*(1.0-temp));
        if (lFreq < 1.0) {
            for(int count = 0; count < 13; count++) {
                iirLAngleL[count] = (iirLAngleL[count]*(1.0-lFreq))+((inputSampleL-iirLPositionL[count])*lFreq);
                inputSampleL = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0-lFreq))+(inputSampleL*lFreq);
                iirLPositionL[count] = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0-lFreq))+(inputSampleL*lFreq);//left
                iirLAngleR[count] = (iirLAngleR[count]*(1.0-lFreq))+((inputSampleR-iirLPositionR[count])*lFreq);
                inputSampleR = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0-lFreq))+(inputSampleR*lFreq);
                iirLPositionR[count] = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0-lFreq))+(inputSampleR*lFreq);//right
            } //the lowpass
            lBypass = false;
        } else {
            if (!lBypass) {
                lBypass = true;
                for(int count = 0; count < 14; count++) {
                    iirLPositionL[count] = 0.0;
                    iirLAngleL[count] = 0.0;
                    iirLPositionR[count] = 0.0;
                    iirLAngleR[count] = 0.0;
                }
            } //blank out lowpass if just switched off
        }
        //Cabs2
        
        double gainR = (panA*temp)+(panB*(1.0-temp));
        double gainL = 1.57079633-gainR;
        gainR = sin(gainR); gainL = sin(gainL);
        
        double gain = (inTrimA*temp)+(inTrimB*(1.0-temp));
        if (gain > 1.0) gain *= gain;
        if (gain < 1.0) gain = 1.0-pow(1.0-gain,2);
        
        inputSampleL = inputSampleL * gainL * gain;
        inputSampleR = inputSampleR * gainR * gain;
        //applies pan section, and smoothed fader gain
        
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
        
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
        
        //begin bar display section
        if ((fabs(inputSampleL-previousLeft)/28000.0f)*getSampleRate() > slewLeft) slewLeft =  (fabs(inputSampleL-previousLeft)/28000.0f)*getSampleRate();
        if ((fabs(inputSampleR-previousRight)/28000.0f)*getSampleRate() > slewRight) slewRight = (fabs(inputSampleR-previousRight)/28000.0f)*getSampleRate();
        previousLeft = inputSampleL; previousRight = inputSampleR; //slew measurement is NOT rectified
        double rectifiedL = fabs(inputSampleL);
        double rectifiedR = fabs(inputSampleR);
        if (rectifiedL > peakLeft) peakLeft = rectifiedL;
        if (rectifiedR > peakRight) peakRight = rectifiedR;
        rmsLeft += (rectifiedL * rectifiedL);
        rmsRight += (rectifiedR * rectifiedR);
        rmsCount++;
        zeroLeft += zeroCrossScale;
        if (longestZeroLeft < zeroLeft) longestZeroLeft = zeroLeft;
        if (wasPositiveL && inputSampleL < 0.0) {
            wasPositiveL = false;
            zeroLeft = 0.0;
        } else if (!wasPositiveL && inputSampleL > 0.0) {
            wasPositiveL = true;
            zeroLeft = 0.0;
        }
        zeroRight += zeroCrossScale;
        if (longestZeroRight < zeroRight) longestZeroRight = zeroRight;
        if (wasPositiveR && inputSampleR < 0.0) {
            wasPositiveR = false;
            zeroRight = 0.0;
        } else if (!wasPositiveR && inputSampleR > 0.0) {
            wasPositiveR = true;
            zeroRight = 0.0;
        } //end bar display section
        
        //begin 32 bit stereo floating point dither
        int expon; frexpf((float)inputSampleL, &expon);
        fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
        inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
        frexpf((float)inputSampleR, &expon);
        fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
        inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
        //end 32 bit stereo floating point dither
        
        *outL = inputSampleL;
        *outR = inputSampleR;
    }


    if (rmsCount > rmsSize)
    {
        AudioToUIMessage msg; //define the thing we're telling JUCE
        msg.what = AudioToUIMessage::SLEW_LEFT; msg.newValue = (float)slewLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::SLEW_RIGHT; msg.newValue = (float)slewRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_LEFT; msg.newValue = (float)sqrt(peakLeft); audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_RIGHT; msg.newValue = (float)sqrt(peakRight); audioToUI.push(msg);
        msg.what = AudioToUIMessage::RMS_LEFT; msg.newValue = (float)sqrt(sqrt(rmsLeft/rmsCount)); audioToUI.push(msg);
        msg.what = AudioToUIMessage::RMS_RIGHT; msg.newValue = (float)sqrt(sqrt(rmsRight/rmsCount)); audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_LEFT; msg.newValue = (float)longestZeroLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_RIGHT; msg.newValue = (float)longestZeroRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_COMP; msg.newValue = (float)maxComp; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_GATE; msg.newValue = (float)1.0-maxGate; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_ATK; msg.newValue = (float)maxAttack; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_RLS; msg.newValue = (float)maxRelease; audioToUI.push(msg);
        msg.what = AudioToUIMessage::INCREMENT; msg.newValue = 1200.0f; audioToUI.push(msg);
        slewLeft = 0.0;
        slewRight = 0.0;
        peakLeft = 0.0;
        peakRight = 0.0;
        rmsLeft = 0.0;
        rmsRight = 0.0;
        zeroLeft = 0.0;
        zeroRight = 0.0;
        longestZeroLeft = 0.0;
        longestZeroRight = 0.0;
        maxComp = baseComp;
        maxGate = baseGate;
        baseComp = 0.0;
        baseGate = 0.0;
        rmsCount = 0;
    }
}

//==============================================================================

void PluginProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    if (!(getBus(false, 0)->isEnabled() && getBus(true, 0)->isEnabled())) return;
    auto mainOutput = getBusBuffer(buffer, false, 0); //if we have audio busses at all,
    auto mainInput = getBusBuffer(buffer, true, 0); //they're now mainOutput and mainInput.
    UIToAudioMessage uim;
    while (uiToAudio.pop(uim)) {
        switch (uim.what) {
        case UIToAudioMessage::NEW_VALUE: params[uim.which]->setValueNotifyingHost(params[uim.which]->convertTo0to1(uim.newValue)); break;
        case UIToAudioMessage::BEGIN_EDIT: params[uim.which]->beginChangeGesture(); break;
        case UIToAudioMessage::END_EDIT: params[uim.which]->endChangeGesture(); break;
        }
    } //Handle inbound messages from the UI thread
    
    double rmsSize = (1881.0 / 44100.0)*getSampleRate(); //higher is slower with larger RMS buffers
    double zeroCrossScale = (1.0 / getSampleRate())*44100.0;
    int inFramesToProcess = buffer.getNumSamples(); //vst doesn't give us this as a separate variable so we'll make it
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();
	int spacing = floor(overallscale*2.0);
	if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
    
    double moreTapeHack = (params[KNOBMOR]->get()*2.0)+1.0;
    bool tapehackOff = (params[KNOBMOR]->get() == 0.0);
    switch ((int)(params[KNOBTRM]->get()*4.0)){
        case 0: moreTapeHack *= 0.5; break;
        case 1: break;
        case 2: moreTapeHack *= 2.0; break;
        case 3: moreTapeHack *= 4.0; break;
        case 4: moreTapeHack *= 8.0; break;
    }
    double moreDiscontinuity = fmax(pow(params[KNOBMOR]->get()*0.42,3.0)*overallscale,0.00001);
    //Discontapeity
    
    double trebleGain = (params[KNOBHIG]->get()-0.5)*2.0;
    trebleGain = 1.0+(trebleGain*fabs(trebleGain)*fabs(trebleGain));
    double highmidGain = (params[KNOBHMG]->get()-0.5)*2.0;
    highmidGain = 1.0+(highmidGain*fabs(highmidGain)*fabs(highmidGain));
    double lowmidGain = (params[KNOBLMG]->get()-0.5)*2.0;
    lowmidGain = 1.0+(lowmidGain*fabs(lowmidGain)*fabs(lowmidGain));
    double bassGain = (params[KNOBBSG]->get()-0.5)*2.0;
    bassGain = 1.0+(bassGain*fabs(bassGain)*fabs(bassGain));
    double highCoef = 0.0;
    double midCoef = 0.0;
    double lowCoef = 0.0;
    
    bool eqOff = (trebleGain == 1.0 && highmidGain == 1.0 && lowmidGain == 1.0 && bassGain == 1.0);
    //we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
    //it cancels out to bit-identical anyhow, but we get to skip the calculation
    if (!eqOff) {
        double trebleRef = params[KNOBHIF]->get()-0.5;
        double highmidRef = params[KNOBHMF]->get()-0.5;
        double lowmidRef = params[KNOBLMF]->get()-0.5;
        double bassRef = params[KNOBBSF]->get()-0.5;
        double highF = 0.75 + ((trebleRef+trebleRef+trebleRef+highmidRef)*0.125);
        double bassF = 0.25 + ((lowmidRef+bassRef+bassRef+bassRef)*0.125);
        double midF = (highF*0.5) + (bassF*0.5) + ((highmidRef+lowmidRef)*0.125);
        
        double highQ = fmax(fmin(1.0+(highmidRef-trebleRef),4.0),0.125);
        double midQ = fmax(fmin(1.0+(lowmidRef-highmidRef),4.0),0.125);
        double lowQ = fmax(fmin(1.0+(bassRef-lowmidRef),4.0),0.125);
        
        highA[biq_freq] = ((pow(highF,3)*20000.0)/getSampleRate());
        highC[biq_freq] = highB[biq_freq] = highA[biq_freq] = fmax(fmin(highA[biq_freq],0.4999),0.00025);
        double highFreq = pow(highF,3)*20000.0;
        double omega = 2.0*M_PI*(highFreq/getSampleRate());
        double biqK = 2.0-cos(omega);
        highCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        highA[biq_reso] = 2.24697960 * highQ;
        highB[biq_reso] = 0.80193774 * highQ;
        highC[biq_reso] = 0.55495813 * highQ;
        
        midA[biq_freq] = ((pow(midF,3)*20000.0)/getSampleRate());
        midC[biq_freq] = midB[biq_freq] = midA[biq_freq] = fmax(fmin(midA[biq_freq],0.4999),0.00025);
        double midFreq = pow(midF,3)*20000.0;
        omega = 2.0*M_PI*(midFreq/getSampleRate());
        biqK = 2.0-cos(omega);
        midCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        midA[biq_reso] = 2.24697960 * midQ;
        midB[biq_reso] = 0.80193774 * midQ;
        midC[biq_reso] = 0.55495813 * midQ;
        
        lowA[biq_freq] = ((pow(bassF,3)*20000.0)/getSampleRate());
        lowC[biq_freq] = lowB[biq_freq] = lowA[biq_freq] = fmax(fmin(lowA[biq_freq],0.4999),0.00025);
        double lowFreq = pow(bassF,3)*20000.0;
        omega = 2.0*M_PI*(lowFreq/getSampleRate());
        biqK = 2.0-cos(omega);
        lowCoef = -sqrt((biqK*biqK)-1.0)+biqK;
        lowA[biq_reso] = 2.24697960 * lowQ;
        lowB[biq_reso] = 0.80193774 * lowQ;
        lowC[biq_reso] = 0.55495813 * lowQ;
        
        biqK = tan(M_PI * highA[biq_freq]);
        double norm = 1.0 / (1.0 + biqK / highA[biq_reso] + biqK * biqK);
        highA[biq_a0] = biqK * biqK * norm;
        highA[biq_a1] = 2.0 * highA[biq_a0];
        highA[biq_a2] = highA[biq_a0];
        highA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highA[biq_b2] = (1.0 - biqK / highA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * highB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / highB[biq_reso] + biqK * biqK);
        highB[biq_a0] = biqK * biqK * norm;
        highB[biq_a1] = 2.0 * highB[biq_a0];
        highB[biq_a2] = highB[biq_a0];
        highB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highB[biq_b2] = (1.0 - biqK / highB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * highC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / highC[biq_reso] + biqK * biqK);
        highC[biq_a0] = biqK * biqK * norm;
        highC[biq_a1] = 2.0 * highC[biq_a0];
        highC[biq_a2] = highC[biq_a0];
        highC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highC[biq_b2] = (1.0 - biqK / highC[biq_reso] + biqK * biqK) * norm;
        
        biqK = tan(M_PI * midA[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midA[biq_reso] + biqK * biqK);
        midA[biq_a0] = biqK * biqK * norm;
        midA[biq_a1] = 2.0 * midA[biq_a0];
        midA[biq_a2] = midA[biq_a0];
        midA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midA[biq_b2] = (1.0 - biqK / midA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * midB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midB[biq_reso] + biqK * biqK);
        midB[biq_a0] = biqK * biqK * norm;
        midB[biq_a1] = 2.0 * midB[biq_a0];
        midB[biq_a2] = midB[biq_a0];
        midB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midB[biq_b2] = (1.0 - biqK / midB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * midC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / midC[biq_reso] + biqK * biqK);
        midC[biq_a0] = biqK * biqK * norm;
        midC[biq_a1] = 2.0 * midC[biq_a0];
        midC[biq_a2] = midC[biq_a0];
        midC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        midC[biq_b2] = (1.0 - biqK / midC[biq_reso] + biqK * biqK) * norm;
        
        biqK = tan(M_PI * lowA[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowA[biq_reso] + biqK * biqK);
        lowA[biq_a0] = biqK * biqK * norm;
        lowA[biq_a1] = 2.0 * lowA[biq_a0];
        lowA[biq_a2] = lowA[biq_a0];
        lowA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowA[biq_b2] = (1.0 - biqK / lowA[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * lowB[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowB[biq_reso] + biqK * biqK);
        lowB[biq_a0] = biqK * biqK * norm;
        lowB[biq_a1] = 2.0 * lowB[biq_a0];
        lowB[biq_a2] = lowB[biq_a0];
        lowB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowB[biq_b2] = (1.0 - biqK / lowB[biq_reso] + biqK * biqK) * norm;
        biqK = tan(M_PI * lowC[biq_freq]);
        norm = 1.0 / (1.0 + biqK / lowC[biq_reso] + biqK * biqK);
        lowC[biq_a0] = biqK * biqK * norm;
        lowC[biq_a1] = 2.0 * lowC[biq_a0];
        lowC[biq_a2] = lowC[biq_a0];
        lowC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowC[biq_b2] = (1.0 - biqK / lowC[biq_reso] + biqK * biqK) * norm;
    }
    //SmoothEQ2
    
    double bezThresh = pow(1.0-params[KNOBTHR]->get(), 4.0) * 8.0;
    double bezRez = pow(1.0-params[KNOBATK]->get(), 4.0) / overallscale;
    double sloRez = pow(1.0-params[KNOBRLS]->get(), 4.0) / overallscale;
    double gate = pow(params[KNOBGAT]->get(),4.0);
    bezRez = fmin(fmax(bezRez,0.0001),1.0);
    sloRez = fmin(fmax(sloRez,0.0001),1.0);
    //Dynamics3
    
    lFreqA = lFreqB; lFreqB = pow(fmax(params[KNOBLOP]->get(),0.002),overallscale); //the lowpass
    hFreqA = hFreqB; hFreqB = pow(params[KNOBHIP]->get(),overallscale+2.0); //the highpass
    //Cabs2
    
    panA = panB; panB = params[KNOBPAN]->get()*1.57079633;
    inTrimA = inTrimB; inTrimB = params[KNOBFAD]->get()*2.0;
    //Console
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto outL = mainOutput.getWritePointer(0, i);
        auto outR = mainOutput.getWritePointer(1, i);
        auto inL = mainInput.getReadPointer(0, i); //in isBussesLayoutSupported, we have already
        auto inR = mainInput.getReadPointer(1, i); //specified that we can only be stereo and never mono
        long double inputSampleL = *inL;
        long double inputSampleR = *inR;
        if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
        //NOTE: I don't yet know whether counting the buffer backwards means that gainA and gainB must be reversed.
        //If the audio flow is in fact reversed we must swap the temp and (1.0-temp) and I have done this provisionally
        //Original VST2 is counting DOWN with -- operator, but this counts UP with ++
        //If this doesn't create zipper noise on sine processing then it's correct
		
        
        inputSampleL *= moreTapeHack;
        inputSampleR *= moreTapeHack;
        //trim control gets to work even when MORE is off
        
        if (!tapehackOff) {
            double darkSampleL = inputSampleL;
            double darkSampleR = inputSampleR;
            if (avgPos > 31) avgPos = 0;
            if (spacing > 31) {
                avg32L[avgPos] = darkSampleL; avg32R[avgPos] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {darkSampleL += avg32L[x]; darkSampleR += avg32R[x];}
                darkSampleL /= 32.0; darkSampleR /= 32.0;
            } if (spacing > 15) {
                avg16L[avgPos%16] = darkSampleL; avg16R[avgPos%16] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {darkSampleL += avg16L[x]; darkSampleR += avg16R[x];}
                darkSampleL /= 16.0; darkSampleR /= 16.0;
            } if (spacing > 7) {
                avg8L[avgPos%8] = darkSampleL; avg8R[avgPos%8] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x]; darkSampleR += avg8R[x];}
                darkSampleL /= 8.0; darkSampleR /= 8.0;
            } if (spacing > 3) {
                avg4L[avgPos%4] = darkSampleL; avg4R[avgPos%4] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x]; darkSampleR += avg4R[x];}
                darkSampleL /= 4.0; darkSampleR /= 4.0;
            } if (spacing > 1) {
                avg2L[avgPos%2] = darkSampleL; avg2R[avgPos%2] = darkSampleR;
                darkSampleL = 0.0; darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x]; darkSampleR += avg2R[x];}
                darkSampleL /= 2.0; darkSampleR /= 2.0;
            } avgPos++;
            lastSlewL += fabs(lastSlewpleL-inputSampleL); lastSlewpleL = inputSampleL;
            double avgSlewL = fmin(lastSlewL*lastSlewL*(0.0635-(overallscale*0.0018436)),1.0);
            lastSlewL = fmax(lastSlewL*0.78,2.39996322972865332223);
            lastSlewR += fabs(lastSlewpleR-inputSampleR); lastSlewpleR = inputSampleR;
            double avgSlewR = fmin(lastSlewR*lastSlewR*(0.0635-(overallscale*0.0018436)),1.0);
            lastSlewR = fmax(lastSlewR*0.78,2.39996322972865332223); //look up Golden Angle, it's cool
            inputSampleL = (inputSampleL*(1.0-avgSlewL)) + (darkSampleL*avgSlewL);
            inputSampleR = (inputSampleR*(1.0-avgSlewR)) + (darkSampleR*avgSlewR);
            //begin Discontinuity section
            inputSampleL *= moreDiscontinuity;
            dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25)-0.5))*0.5);
            dBaPosL = fmin(dBaPosL,1.0);
            int dBdly = floor(dBaPosL*dscBuf);
            double dBi = (dBaPosL*dscBuf)-dBdly;
            inputSampleL = dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
            dBdly++; inputSampleL += dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
            dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
            inputSampleL /= moreDiscontinuity;
            //end Discontinuity section, begin TapeHack section
            inputSampleL = fmax(fmin(inputSampleL,2.305929007734908),-2.305929007734908);
            double addtwo = inputSampleL * inputSampleL;
            double empower = inputSampleL * addtwo; // inputSampleL to the third power
            inputSampleL -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleL += (empower / 69.0);
            empower *= addtwo; //seventh
            inputSampleL -= (empower / 2530.08);
            empower *= addtwo; //ninth
            inputSampleL += (empower / 224985.6);
            empower *= addtwo; //eleventh
            inputSampleL -= (empower / 9979200.0f);
            //this is a degenerate form of a Taylor Series to approximate sin()
            //end TapeHack section
            //begin Discontinuity section
            inputSampleR *= moreDiscontinuity;
            dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25)-0.5))*0.5);
            dBaPosR = fmin(dBaPosR,1.0);
            dBdly = floor(dBaPosR*dscBuf);
            dBi = (dBaPosR*dscBuf)-dBdly;
            inputSampleR = dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0-dBi);
            dBdly++; inputSampleR += dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
            dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
            inputSampleR /= moreDiscontinuity;
            //end Discontinuity section, begin TapeHack section
            inputSampleR = fmax(fmin(inputSampleR,2.305929007734908),-2.305929007734908);
            addtwo = inputSampleR * inputSampleR;
            empower = inputSampleR * addtwo; // inputSampleR to the third power
            inputSampleR -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleR += (empower / 69.0);
            empower *= addtwo; //seventh
            inputSampleR -= (empower / 2530.08);
            empower *= addtwo; //ninth
            inputSampleR += (empower / 224985.6);
            empower *= addtwo; //eleventh
            inputSampleR -= (empower / 9979200.0f);
            //this is a degenerate form of a Taylor Series to approximate sin()
            //end TapeHack section
            //Discontapeity
        }
        
        if (!eqOff) {
            double trebleL = inputSampleL;
            double outSample = (trebleL * highA[biq_a0]) + highA[biq_sL1];
            highA[biq_sL1] = (trebleL * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sL2];
            highA[biq_sL2] = (trebleL * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midA[biq_a0]) + midA[biq_sL1];
            midA[biq_sL1] = (highmidL * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sL2];
            midA[biq_sL2] = (highmidL * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowA[biq_a0]) + lowA[biq_sL1];
            lowA[biq_sL1] = (lowmidL * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sL2];
            lowA[biq_sL2] = (lowmidL * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //first stage of three crossovers
            
            outSample = (trebleL * highB[biq_a0]) + highB[biq_sL1];
            highB[biq_sL1] = (trebleL * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sL2];
            highB[biq_sL2] = (trebleL * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midB[biq_a0]) + midB[biq_sL1];
            midB[biq_sL1] = (highmidL * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sL2];
            midB[biq_sL2] = (highmidL * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowB[biq_a0]) + lowB[biq_sL1];
            lowB[biq_sL1] = (lowmidL * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sL2];
            lowB[biq_sL2] = (lowmidL * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //second stage of three crossovers
            
            outSample = (trebleL * highC[biq_a0]) + highC[biq_sL1];
            highC[biq_sL1] = (trebleL * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sL2];
            highC[biq_sL2] = (trebleL * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidL = outSample; trebleL -= highmidL;
            
            outSample = (highmidL * midC[biq_a0]) + midC[biq_sL1];
            midC[biq_sL1] = (highmidL * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sL2];
            midC[biq_sL2] = (highmidL * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidL = outSample; highmidL -= lowmidL;
            
            outSample = (lowmidL * lowC[biq_a0]) + lowC[biq_sL1];
            lowC[biq_sL1] = (lowmidL * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sL2];
            lowC[biq_sL2] = (lowmidL * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassL = outSample; lowmidL -= bassL;
            
            trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //third stage of three crossovers
            
            highLIIR = (highLIIR*highCoef) + (trebleL*(1.0-highCoef));
            highmidL = highLIIR; trebleL -= highmidL;
            
            midLIIR = (midLIIR*midCoef) + (highmidL*(1.0-midCoef));
            lowmidL = midLIIR; highmidL -= lowmidL;
            
            lowLIIR = (lowLIIR*lowCoef) + (lowmidL*(1.0-lowCoef));
            bassL = lowLIIR; lowmidL -= bassL;
            
            inputSampleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
            //fourth stage of three crossovers is the exponential filters
            
            
            double trebleR = inputSampleR;
            outSample = (trebleR * highA[biq_a0]) + highA[biq_sR1];
            highA[biq_sR1] = (trebleR * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sR2];
            highA[biq_sR2] = (trebleR * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midA[biq_a0]) + midA[biq_sR1];
            midA[biq_sR1] = (highmidR * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sR2];
            midA[biq_sR2] = (highmidR * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowA[biq_a0]) + lowA[biq_sR1];
            lowA[biq_sR1] = (lowmidR * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sR2];
            lowA[biq_sR2] = (lowmidR * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //first stage of three crossovers
            
            outSample = (trebleR * highB[biq_a0]) + highB[biq_sR1];
            highB[biq_sR1] = (trebleR * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sR2];
            highB[biq_sR2] = (trebleR * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midB[biq_a0]) + midB[biq_sR1];
            midB[biq_sR1] = (highmidR * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sR2];
            midB[biq_sR2] = (highmidR * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowB[biq_a0]) + lowB[biq_sR1];
            lowB[biq_sR1] = (lowmidR * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sR2];
            lowB[biq_sR2] = (lowmidR * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //second stage of three crossovers
            
            outSample = (trebleR * highC[biq_a0]) + highC[biq_sR1];
            highC[biq_sR1] = (trebleR * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sR2];
            highC[biq_sR2] = (trebleR * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidR = outSample; trebleR -= highmidR;
            
            outSample = (highmidR * midC[biq_a0]) + midC[biq_sR1];
            midC[biq_sR1] = (highmidR * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sR2];
            midC[biq_sR2] = (highmidR * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidR = outSample; highmidR -= lowmidR;
            
            outSample = (lowmidR * lowC[biq_a0]) + lowC[biq_sR1];
            lowC[biq_sR1] = (lowmidR * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sR2];
            lowC[biq_sR2] = (lowmidR * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassR = outSample; lowmidR -= bassR;
            
            trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //third stage of three crossovers
            
            highRIIR = (highRIIR*highCoef) + (trebleR*(1.0-highCoef));
            highmidR = highRIIR; trebleR -= highmidR;
            
            midRIIR = (midRIIR*midCoef) + (highmidR*(1.0-midCoef));
            lowmidR = midRIIR; highmidR -= lowmidR;
            
            lowRIIR = (lowRIIR*lowCoef) + (lowmidR*(1.0-lowCoef));
            bassR = lowRIIR; lowmidR -= bassR;
            
            inputSampleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
            //fourth stage of three crossovers is the exponential filters
        }
        //SmoothEQ2
        
        if (baseComp < fabs(inputSampleR*64.0)) baseComp = fmin(fabs(inputSampleR*64.0),1.0);
        if (baseComp < fabs(inputSampleL*64.0)) baseComp = fmin(fabs(inputSampleL*64.0),1.0);
        if (baseGate < fabs(inputSampleR*64.0)) baseGate = fmin(fabs(inputSampleR*64.0),1.0);
        if (baseGate < fabs(inputSampleL*64.0)) baseGate = fmin(fabs(inputSampleL*64.0),1.0);
        //here we make the comp and gate lights light up anytime there is signal,
        //and either compression or gating DIM the lights in question.
        
        if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
        else bezGate = fmax(0.000001, bezGate-fmin(bezRez,sloRez));
        if (maxGate > bezGate) maxGate = bezGate;

        double ctrl = fmax(fabs(inputSampleL),fabs(inputSampleR));
        maxAttack = fmax(fmin(maxAttack+((ctrl>bezThresh)?-bezRez:bezRez),1.0),0.0);
        maxRelease = fmax(fmin(maxRelease+((ctrl>bezThresh)?-sloRez:maxAttack),1.0),0.0);
        //metering blinkenlights
        
        if (bezThresh > 0.0) {
            inputSampleL *= (bezThresh+1.0);
            inputSampleR *= (bezThresh+1.0);
            ctrl = fmax(fabs(inputSampleL),fabs(inputSampleR));
            bezMax = fmax(bezMax,ctrl);
            bezMin = fmax(bezMin-sloRez,ctrl);
            bezComp[bez_cycle] += bezRez;
            bezComp[bez_Ctrl] += (bezMin * bezRez);
            if (bezComp[bez_cycle] > 1.0) {
                if (bezGate < 1.0) bezComp[bez_Ctrl] /= bezGate;
                bezComp[bez_cycle] -= 1.0;
                bezComp[bez_C] = bezComp[bez_B];
                bezComp[bez_B] = bezComp[bez_A];
                bezComp[bez_A] = bezComp[bez_Ctrl];
                bezComp[bez_Ctrl] = 0.0;
                bezMax = 0.0;
            }
            double CB = (bezComp[bez_C]*(1.0-bezComp[bez_cycle]))+(bezComp[bez_B]*bezComp[bez_cycle]);
            double BA = (bezComp[bez_B]*(1.0-bezComp[bez_cycle]))+(bezComp[bez_A]*bezComp[bez_cycle]);
            double CBA = (bezComp[bez_B]+(CB*(1.0-bezComp[bez_cycle]))+(BA*bezComp[bez_cycle]))*0.5;
            inputSampleL *= 1.0-(fmin(CBA*bezThresh,1.0));
            inputSampleR *= 1.0-(fmin(CBA*bezThresh,1.0));
            if (maxComp > 1.0-pow(CBA,2.0)) maxComp = 1.0-pow(CBA,2.0);
        } else bezComp[bez_Ctrl] = 0.0;
        //Dynamics3
        
        const double temp = (double)i/inFramesToProcess;
        const double hFreq = (hFreqA*temp)+(hFreqB*(1.0-temp));
        if (hFreq > 0.0) {
            double lowSampleL = inputSampleL;
            double lowSampleR = inputSampleR;
            for(int count = 0; count < 21; count++) {
                iirHAngleL[count] = (iirHAngleL[count]*(1.0-hFreq))+((lowSampleL-iirHPositionL[count])*hFreq);
                lowSampleL = ((iirHPositionL[count]+(iirHAngleL[count]*hFreq))*(1.0-hFreq))+(lowSampleL*hFreq);
                iirHPositionL[count] = ((iirHPositionL[count]+(iirHAngleL[count]*hFreq))*(1.0-hFreq))+(lowSampleL*hFreq);
                inputSampleL -= (lowSampleL * (1.0/21.0));//left
                iirHAngleR[count] = (iirHAngleR[count]*(1.0-hFreq))+((lowSampleR-iirHPositionR[count])*hFreq);
                lowSampleR = ((iirHPositionR[count]+(iirHAngleR[count]*hFreq))*(1.0-hFreq))+(lowSampleR*hFreq);
                iirHPositionR[count] = ((iirHPositionR[count]+(iirHAngleR[count]*hFreq))*(1.0-hFreq))+(lowSampleR*hFreq);
                inputSampleR -= (lowSampleR * (1.0/21.0));//right
            } //the highpass
            hBypass = false;
        } else {
            if (!hBypass) {
                hBypass = true;
                for(int count = 0; count < 22; count++) {
                    iirHPositionL[count] = 0.0;
                    iirHAngleL[count] = 0.0;
                    iirHPositionR[count] = 0.0;
                    iirHAngleR[count] = 0.0;
                }
            } //blank out highpass if jut switched off
        }
        const double lFreq = (lFreqA*temp)+(lFreqB*(1.0-temp));
        if (lFreq < 1.0) {
            for(int count = 0; count < 13; count++) {
                iirLAngleL[count] = (iirLAngleL[count]*(1.0-lFreq))+((inputSampleL-iirLPositionL[count])*lFreq);
                inputSampleL = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0-lFreq))+(inputSampleL*lFreq);
                iirLPositionL[count] = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0-lFreq))+(inputSampleL*lFreq);//left
                iirLAngleR[count] = (iirLAngleR[count]*(1.0-lFreq))+((inputSampleR-iirLPositionR[count])*lFreq);
                inputSampleR = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0-lFreq))+(inputSampleR*lFreq);
                iirLPositionR[count] = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0-lFreq))+(inputSampleR*lFreq);//right
            } //the lowpass
            lBypass = false;
        } else {
            if (!lBypass) {
                lBypass = true;
                for(int count = 0; count < 14; count++) {
                    iirLPositionL[count] = 0.0;
                    iirLAngleL[count] = 0.0;
                    iirLPositionR[count] = 0.0;
                    iirLAngleR[count] = 0.0;
                }
            } //blank out lowpass if just switched off
        }
        //Cabs2
        
        double gainR = (panA*temp)+(panB*(1.0-temp));
        double gainL = 1.57079633-gainR;
        gainR = sin(gainR); gainL = sin(gainL);
        
        double gain = (inTrimA*temp)+(inTrimB*(1.0-temp));
        if (gain > 1.0) gain *= gain;
        if (gain < 1.0) gain = 1.0-pow(1.0-gain,2);
        
        inputSampleL = inputSampleL * gainL * gain;
        inputSampleR = inputSampleR * gainR * gain;
        //applies pan section, and smoothed fader gain
        
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        else if (inputSampleL > 0.0) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        else if (inputSampleL < 0.0) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
        
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        else if (inputSampleR > 0.0) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        else if (inputSampleR < 0.0) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
        
        //begin bar display section
        if ((fabs(inputSampleL-previousLeft)/28000.0f)*getSampleRate() > slewLeft) slewLeft =  (fabs(inputSampleL-previousLeft)/28000.0f)*getSampleRate();
        if ((fabs(inputSampleR-previousRight)/28000.0f)*getSampleRate() > slewRight) slewRight = (fabs(inputSampleR-previousRight)/28000.0f)*getSampleRate();
        previousLeft = inputSampleL; previousRight = inputSampleR; //slew measurement is NOT rectified
        double rectifiedL = fabs(inputSampleL);
        double rectifiedR = fabs(inputSampleR);
        if (rectifiedL > peakLeft) peakLeft = rectifiedL;
        if (rectifiedR > peakRight) peakRight = rectifiedR;
        rmsLeft += (rectifiedL * rectifiedL);
        rmsRight += (rectifiedR * rectifiedR);
        rmsCount++;
        zeroLeft += zeroCrossScale;
        if (longestZeroLeft < zeroLeft) longestZeroLeft = zeroLeft;
        if (wasPositiveL && inputSampleL < 0.0) {
            wasPositiveL = false;
            zeroLeft = 0.0;
        } else if (!wasPositiveL && inputSampleL > 0.0) {
            wasPositiveL = true;
            zeroLeft = 0.0;
        }
        zeroRight += zeroCrossScale;
        if (longestZeroRight < zeroRight) longestZeroRight = zeroRight;
        if (wasPositiveR && inputSampleR < 0.0) {
            wasPositiveR = false;
            zeroRight = 0.0;
        } else if (!wasPositiveR && inputSampleR > 0.0) {
            wasPositiveR = true;
            zeroRight = 0.0;
        } //end bar display section
        
        //begin 64 bit stereo floating point dither
        int expon; frexp((double)inputSampleL, &expon);
        fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
        inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
        frexp((double)inputSampleR, &expon);
        fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
        inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
        //end 64 bit stereo floating point dither
        
        *outL = inputSampleL;
        *outR = inputSampleR;
    }

    if (rmsCount > rmsSize)
    {
        AudioToUIMessage msg; //define the thing we're telling JUCE
        msg.what = AudioToUIMessage::SLEW_LEFT; msg.newValue = (float)slewLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::SLEW_RIGHT; msg.newValue = (float)slewRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_LEFT; msg.newValue = (float)sqrt(peakLeft); audioToUI.push(msg);
        msg.what = AudioToUIMessage::PEAK_RIGHT; msg.newValue = (float)sqrt(peakRight); audioToUI.push(msg);
        msg.what = AudioToUIMessage::RMS_LEFT; msg.newValue = (float)sqrt(sqrt(rmsLeft/rmsCount)); audioToUI.push(msg);
        msg.what = AudioToUIMessage::RMS_RIGHT; msg.newValue = (float)sqrt(sqrt(rmsRight/rmsCount)); audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_LEFT; msg.newValue = (float)longestZeroLeft; audioToUI.push(msg);
        msg.what = AudioToUIMessage::ZERO_RIGHT; msg.newValue = (float)longestZeroRight; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_COMP; msg.newValue = (float)maxComp; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_GATE; msg.newValue = (float)1.0f-maxGate; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_ATK; msg.newValue = (float)maxAttack; audioToUI.push(msg);
        msg.what = AudioToUIMessage::BLINKEN_RLS; msg.newValue = (float)maxRelease; audioToUI.push(msg);
        msg.what = AudioToUIMessage::INCREMENT; msg.newValue = 1200.0f; audioToUI.push(msg);
        slewLeft = 0.0;
        slewRight = 0.0;
        peakLeft = 0.0;
        peakRight = 0.0;
        rmsLeft = 0.0;
        rmsRight = 0.0;
        zeroLeft = 0.0;
        zeroRight = 0.0;
        longestZeroLeft = 0.0;
        longestZeroRight = 0.0;
        maxComp = baseComp;
        maxGate = baseGate;
        baseComp = 0.0;
        baseGate = 0.0;
        rmsCount = 0;
    }
}

