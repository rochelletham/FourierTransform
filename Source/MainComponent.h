#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent :
//   public Component,
   public AudioAppComponent,
   public Button::Listener,
   public Slider::Listener,
   public Timer {

public:
   //constructor
   MainComponent();

   ~MainComponent();

   //==============================================================================
   // Component overrides

   void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
   void paint (Graphics& g) override;
   void resized() override;
   void buttonClicked (Button *button) override;
//   void comboBoxChanged (ComboBox *menu) override;
   void sliderValueChanged (Slider *slider) override;
   void timerCallback() override;
   void pushNextSampleIntoAudioData (float sample);
   void drawNextLineOfSpectrogram();

   void openAudioSettings();
   void prepareToPlay (int /*samplesPerBlockExpected*/, double /*newSampleRate*/) override
   {
      // (nothing to do here)
   }

   void releaseResources() override
   {
      // (nothing to do here)
   }

   enum
   {
      // order of 10 makes an FFT with (2^10) points. caulc 2 ^ power of order = 1024
      fftOrder = 10,
      //left bit shift operator, shift 10 times to left
      fftSize  = 1 << fftOrder
   };

private:
   float saturation = 0.0f;
   Label saturationLabel;
   Slider saturationSlider;

   TextButton settingsButton {"Audio Settings"};
   TextButton playButton {"Play"};
   TextButton stopButton {"Stop"};
   TextButton openButton {"Open File"};
   /// A label that displays the text "CPU:"
   Label cpuLabel {"", "CPU Usage: "};
   /// A label that is updated by a timer to show the current cpu usage.
   Label cpuUsage {"0.00", "%"};

//   /// A reference to the app's audio device manager.
//   AudioDeviceManager& deviceManager;
   //dsp::FFT object which we will use to do FFT with
   dsp::FFT forwardFFT;
   //image used for display
   Image spectrogramImage;

   //temp bool indicating if next FFT block is ready to be rendered
   bool nextBlockReady = false;

   //array with prior initialized size 1024.
   //contains incoming audio data in samples
   float audioData [fftSize];

   //count of amount of samples in fifo
   int audioDataIndex = 0;
   //fftData float array with size 2048 (double fftSize).
   // contains results of FFT calculations
   float fftData [2 * fftSize];

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

