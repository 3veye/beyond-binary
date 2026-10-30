/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SynthUsingMidiInputTutorial
 version:          2.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Synthesiser with midi input.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
struct SineWaveSound   : public juce::SynthesiserSound
{
    SineWaveSound() {}

    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

//==============================================================================
struct SineWaveVoice : public juce::SynthesiserVoice
{
    SineWaveVoice() {}

    //juce::slider* slider = MainContentComponentptr->getSlider(); 

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void setAttack(float value) {
        this->attack = value;
        adsr.setParameters({ attack, decay, sustain, release }); // Set ADSR parameters (attack, decay, sustain, release)


    }
	void setDecay(float value) {
		this->decay = value;
		adsr.setParameters({ attack, decay, sustain, release }); // Set ADSR parameters (attack, decay, sustain, release)
	}
	void setSustain(float value) {
		this->sustain = value;
		adsr.setParameters({ attack, decay, sustain, release }); // Set ADSR parameters (attack, decay, sustain, release)
	}
	void setRelease(float value) {
		this->release = value;
		adsr.setParameters({ attack, decay, sustain, release }); // Set ADSR parameters (attack, decay, sustain, release)
	}
    void setWaveShape(int Value) {
        this->waveShape = Value;

    }

    //void onsliderValueChanged() {
        //attack = attackSlider.getValue(); // Update the attack value based on the slider's value
    //}


    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        adsr.setSampleRate(getSampleRate());
        adsr.setParameters({ attack, decay, sustain, release }); // Set ADSR parameters (attack, decay, sustain, release)
        adsr.noteOn(); // Start the ADSR envelope
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        adsr.reset(); // Reset the ADSR envelope   
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;

        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (waveShape == 1) {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0)
                {
                    auto adsrValue = adsr.getNextSample(); // Get the next ADSR value
                    auto currentSample = (float)(std::sin(currentAngle) * adsrValue * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99; // [8]

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote(); // [9]
                        adsr.noteOff(); // Release the ADSR envelope
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0) // [6]
                {
                    auto adsrValue = adsr.getNextSample(); // Get the next ADSR value
                    auto currentSample =  (float)(std::sin(currentAngle) * adsrValue * level ); //voice generation 

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    } if (waveShape == 3) {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0)
                {
                    auto adsrValue = adsr.getNextSample(); // Get the next ADSR value
                    auto currentSample = (float)((2.0 * (currentAngle / juce::MathConstants<double>::twoPi) - 1.0) * adsrValue * level); //voice generation 


                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    if (currentAngle >= juce::MathConstants<double>::twoPi)
                        currentAngle -= juce::MathConstants<double>::twoPi;
                    ++startSample;

                    tailOff *= 0.99; // [8]

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote(); // [9]
                        adsr.noteOff(); // Release the ADSR envelope
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0) // [6]
                {
                    auto adsrValue = adsr.getNextSample(); // Get the next ADSR value
                    auto currentSample = (float)((2.0 * (currentAngle / juce::MathConstants<double>::twoPi) - 1.0) * adsrValue * level); //voice generation 

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    if (currentAngle >= juce::MathConstants<double>::twoPi)
                        currentAngle -= juce::MathConstants<double>::twoPi;
                    ++startSample;
                }
            }
        }
    }
}


private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::ADSR adsr;
    float attack = 0;
	float decay = 0;
	float sustain = 0;
	float release = 0;
	int waveShape = 1; 
	float sin = 0.0f; 
    //MainContentComponent* MainContentComponentptr; 
   
}; 
    

//==============================================================================
class SynthAudioSource   : public juce::AudioSource
{
public:
    SynthAudioSource (juce::MidiKeyboardState& keyState)
        : keyboardState (keyState)
    {
        for (auto i = 0; i < 4; ++i)
            synth.addVoice (new SineWaveVoice());

        synth.addSound (new SineWaveSound());
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate);
        midiCollector.reset (sampleRate); // [10]
		
    }

    void releaseResources() override {}

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples); // [11]

        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                             bufferToFill.numSamples, true);


        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                               bufferToFill.startSample, bufferToFill.numSamples);
    }

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

	int getNumVoices() const
	{
		return synth.getNumVoices();
	}
	SineWaveVoice*getVoice(int index) const    
	{
		return dynamic_cast<SineWaveVoice*>(synth.getVoice(index));
	}

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    juce::MidiMessageCollector midiCollector;
};

//==============================================================================
class MainContentComponent   : public juce::AudioAppComponent,
                               private juce::Timer,
	public juce::Slider::Listener
{
public:
    MainContentComponent()
        : synthAudioSource  (keyboardState),
          keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible (midiInputListLabel);
        midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
        midiInputListLabel.attachToComponent (&midiInputList, true);

        auto midiInputs = juce::MidiInput::getAvailableDevices();
        addAndMakeVisible (midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");

        juce::StringArray midiInputNames;
        for (auto input : midiInputs)
            midiInputNames.add (input.name);

        midiInputList.addItemList (midiInputNames, 1);
        midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };

        for (auto input : midiInputs)
        {
            if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
            {
                setMidiInput (midiInputs.indexOf (input));
                break;
            }
        }

        if (midiInputList.getSelectedId() == 0)
            setMidiInput (0);

        attackSlider.setSliderStyle(juce::Slider::Rotary);
        attackSlider.setRange(0.1f, 1.0f);
        attackSlider.setValue(0.10);
		attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        addAndMakeVisible(attackSlider);
		
        

		decaySlider.setSliderStyle(juce::Slider::Rotary);
		decaySlider.setRange(0.1f, 1.0f);
		decaySlider.setValue(0.10);
		decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
		addAndMakeVisible(decaySlider);

		sustainSlider.setSliderStyle(juce::Slider::Rotary);
		sustainSlider.setRange(0.1f, 1.0f);
		sustainSlider.setValue(0.10);
		sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
		addAndMakeVisible(sustainSlider);
        
		releaseSlider.setSliderStyle(juce::Slider::Rotary);
		releaseSlider.setRange(0.1f, 1.0f);
		releaseSlider.setValue(0.10);
		releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
		addAndMakeVisible(releaseSlider);

		//attackSlider = new juce::Slider();

		//juce::Slider* getSlider() { return attackSlider; }

		addAndMakeVisible(WaveShapeListLabel);
		addAndMakeVisible(WaveShapeList);
		WaveShapeListLabel.setText("Wave Shape:", juce::dontSendNotification);
		WaveShapeListLabel.attachToComponent(&WaveShapeList, true);
		WaveShapeList.addItem("Sine", 1);
		WaveShapeList.addItem("Square", 2);
		WaveShapeList.addItem("Saw", 3);
		WaveShapeList.addItem("Triangle", 4);
		WaveShapeList.onChange = [this] { WaveShapeListChanged(); };

		

        addAndMakeVisible (keyboardComponent);
        attackSlider.addListener(this);
		decaySlider.addListener(this);
		sustainSlider.addListener(this);
        setAudioChannels (0, 2);

        setSize (600, 600);
        startTimer (400);
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }

    void resized() override
    {
        
		attackSlider.setBounds(10, 10, getWidth() * 0.15, getHeight() * 0.15);
		decaySlider.setBounds(120, 10, getWidth() * 0.15, getHeight() * 0.15);
		sustainSlider.setBounds(230, 10, getWidth() * 0.15, getHeight() * 0.15);
		releaseSlider.setBounds(340, 10, getWidth() * 0.15, getHeight() * 0.15);
        WaveShapeList.setBounds (getWidth() - 100, 10, 50, 20);
        keyboardComponent.setBounds (getWidth() - getWidth() + 10, getHeight() * 0.8, getWidth() - 20, getHeight() * 0.15);
       

    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    }
	void WaveShapeListChanged()
	{
        for (int i = 0; i < synthAudioSource.getNumVoices(); ++i)
        {
            auto voice = synthAudioSource.getVoice(i);
            if (voice)
            {
            voice->setWaveShape(WaveShapeList.getSelectedId());
        }
            
        }

	}

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        synthAudioSource.getNextAudioBlock (bufferToFill);
    }

    void releaseResources() override
    {
        synthAudioSource.releaseResources();
    }

	void sliderValueChanged(juce::Slider* slider) override
	{
		if (slider == &attackSlider)
		{ 
			for (int i = 0; i < synthAudioSource.getNumVoices(); ++i)
			{
				auto voice = synthAudioSource.getVoice(i);
				if (voice)
					voice->setAttack(attackSlider.getValue());
			 
			}
		}
        if (slider == &decaySlider)
        {
            for (int i = 0; i < synthAudioSource.getNumVoices(); ++i)
            {
                auto voice = synthAudioSource.getVoice(i);
                if (voice)
                    voice->setDecay(decaySlider.getValue());

            }
		} if (slider == &sustainSlider)
		{
			for (int i = 0; i < synthAudioSource.getNumVoices(); ++i)
			{
				auto voice = synthAudioSource.getVoice(i);
				if (voice)
					voice->setSustain(sustainSlider.getValue());
			}
		} if (slider == &releaseSlider)
		{
			for (int i = 0; i < synthAudioSource.getNumVoices(); ++i)
			{
				auto voice = synthAudioSource.getVoice(i);
				if (voice)
					voice->setRelease(releaseSlider.getValue());
			}
		}
	}

private:
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }

    void setMidiInput (int index)
    {
        auto list = juce::MidiInput::getAvailableDevices();

        deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier,
                                                     synthAudioSource.getMidiCollector()); // [12]

        auto newInput = list[index];

        if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback (newInput.identifier, synthAudioSource.getMidiCollector()); // [13]
        midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }

    //==========================================================================
    juce::Slider attackSlider;
    juce::Slider decaySlider; 
	juce::Slider sustainSlider;
	juce::Slider releaseSlider;
    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;

    juce::ComboBox midiInputList;
    juce::ComboBox WaveShapeList;
	juce::Label WaveShapeListLabel;
    juce::Label midiInputListLabel;
	juce::Label attackLabel;
	juce::Label decayLabel;
	juce::Label sustainLabel;
    int lastInputIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
