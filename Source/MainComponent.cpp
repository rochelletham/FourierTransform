#include "MainComponent.h"
#include "MainApplication.h"


MainComponent::MainComponent()
   //Initialises an object for performing forward and inverse FFT with the given size.
   : forwardFFT(fftOrder),

   //Image(format, width, height, clearImage)
   spectrogramImage (Image::ARGB, 300, 300, true)

{

   // set up for audio
   setAudioChannels (2, 2);

   //cpuLabel
   cpuLabel.attachToComponent(&cpuUsage, true);
   cpuLabel.setJustificationType(Justification::centredRight);
   cpuLabel.setColour(Label::textColourId, Colours::black);
   addAndMakeVisible(cpuLabel);

   cpuUsage.setColour(Label::textColourId, Colours::black);
   addAndMakeVisible(cpuUsage);

   //stopButton
   stopButton.setRadioGroupId(1);
   stopButton.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnRight);
   stopButton.addListener(this);
   addAndMakeVisible(stopButton);

   //playButton
   playButton.setRadioGroupId(1);
   playButton.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnLeft);
   playButton.addListener(this);
   addAndMakeVisible(playButton);

   //settingsButton
   addAndMakeVisible(settingsButton);
   settingsButton.addListener(this);

   //saturation
   addAndMakeVisible(saturationLabel);
   saturationLabel.setText("Saturation:", dontSendNotification);
   saturationLabel.setColour(Label::textColourId, Colours::black);
   saturationLabel.attachToComponent(&saturationSlider, true);
   saturationLabel.setJustificationType(1);


   addAndMakeVisible(saturationSlider);
   saturationSlider.setValue(saturation);
   saturationSlider.setRange(0.0, 1.0);
   saturationSlider.setTextBoxStyle(Slider::TextBoxLeft, true, 0, 0);
   saturationSlider.setSliderStyle(Slider::LinearHorizontal);
   saturationSlider.addListener(this);

   //Starts the timer with an interval specified in Hertz.
   startTimerHz (60);
   setSize (700, 500);
}


MainComponent::~MainComponent() {
   AudioAppComponent::shutdownAudio();
}

void MainComponent::paint (Graphics& gr) {
   gr.fillAll(Colours::lightyellow);

   auto area = getLocalBounds().toFloat();
   area.reduce(50,50);
   gr.drawImage (spectrogramImage, area);
}

void MainComponent::resized() {
   auto area = getLocalBounds();
   area.reduce(15, 15);
   auto topLine = area.removeFromTop(30);
   topLine.removeFromLeft(30);

   openButton.setBounds(topLine.removeFromLeft(80));
   topLine.removeFromLeft(10);

   settingsButton.setBounds(topLine.removeFromLeft(80));
   stopButton.setBounds(topLine.removeFromLeft(80));
   playButton.setBounds(topLine.removeFromLeft(80));


   topLine.removeFromLeft(30);
   saturationLabel.setBounds(topLine.removeFromLeft(100));
   saturationSlider.setBounds(topLine.removeFromLeft(130));

   auto bottomLine = area.removeFromBottom(20);
   cpuLabel.setBounds(bottomLine.removeFromRight(66));
   cpuUsage.setBounds(bottomLine.removeFromRight(66));

}

void MainComponent::openAudioSettings() {
   auto devComp = std::make_unique<AudioDeviceSelectorComponent>(deviceManager,0,2,0,2,true, false, true, false);
   DialogWindow::LaunchOptions dw;
   devComp->setSize(500, 270);
   dw.dialogTitle = "Audio Settings";
   dw.useNativeTitleBar = true;
   dw.resizable = false;
   dw.dialogBackgroundColour = this->getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
   dw.DialogWindow::LaunchOptions::content.setOwned(devComp.release());
   dw.DialogWindow::LaunchOptions::launchAsync();

}

void MainComponent::sliderValueChanged (Slider *slider) {
   if (slider == &saturationSlider) {
      saturation = saturationSlider.getValue();
      std::cout << "saturationSlider = "  << saturation<<std::endl;
   }
}


void MainComponent::buttonClicked (Button *button) {
   if (button == &stopButton) {
      std::cout << "stop button" <<std::endl;
      // stopping timer stops spectrogram functions
      stopTimer();

   } else if (button == &playButton) {
      std::cout << "play button" <<std::endl;
      // resume timer to start spectrogram functions again
      startTimerHz(60);
   } else if (button == &settingsButton) {
      std::cout << "settings button" <<std::endl;
      openAudioSettings();
   }
}
//from juce tutorial
// pushes the given sample into audioData array
void MainComponent::pushNextSampleIntoAudioData (float sample) {
   // if the audioData contains enough data (should be 1024),
   // able to process audioData to perform fourier transform
   if (audioDataIndex == fftSize)
   {
      if (!nextBlockReady)
      {
         /** Fills a block of memory with zeros. */
         zeromem (fftData, sizeof (fftData));

         // memcpy -- copy a block of memory from a location to another
         // takes content from audioData and copies into fftData
         memcpy (fftData, audioData, sizeof (audioData));
         nextBlockReady = true;
      }
      // reset back to 0 to start initialising audioData again
      audioDataIndex = 0;
   }
   // keep updating audioData to have sample
   audioData[audioDataIndex++] = sample;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
   //iterate through the channels
   for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
      // ptr to first sample
      const float * channelData = bufferToFill.buffer->getReadPointer (chan, bufferToFill.startSample);
      //iterate ptr through all samples, apply pushNextSampleIntoAudioData
      for (auto i = 0; i < bufferToFill.numSamples; ++i) {
         pushNextSampleIntoAudioData (channelData[i]);
      }
   }
   // clears buffer if the source is not producing any data.
   bufferToFill.clearActiveBufferRegion();
}

// updates the image after wave transformed
void MainComponent::timerCallback() {
   if (nextBlockReady)
   {
      String text = String(deviceManager.getCpuUsage() * 100, 2) + "%";
      cpuUsage.setText(text, dontSendNotification);
      drawNextLineOfSpectrogram();
      nextBlockReady = false;
      // need repaint to update spectrogram display
      repaint();
   }
}

void MainComponent::drawNextLineOfSpectrogram()
{
   // less than actual width to compensate for moving image across window
   auto rightEdge = spectrogramImage.getWidth() - 1;
   auto imageHeight = spectrogramImage.getHeight();

   //moves image over by 1 to the left -- change x-value
   spectrogramImage.moveImageSection (0, 0, 1, 0, rightEdge, imageHeight);

   // then render FFT data
   // Takes fftData array and transforms it to the magnitude frequency response spectrum.
   forwardFFT.performFrequencyOnlyForwardTransform (fftData);

   // Finds the minimum and maximum values in the given fftData array and gives range
   auto range = FloatVectorOperations::findMinAndMax (fftData, fftSize / 2);

   for (auto yValue = 1; yValue < imageHeight; ++yValue)
   {
      // for every pixel in the image, need to calculate level proportional to sample set
      // use log scale to adjust yValue --> better representation of frequencies
      auto adjustedY = 1.0f - std::exp (std::log (yValue / (float) imageHeight) * 0.2f);
      //jlimit Constrains a value to keep it within a given range.
      auto fftDataIndex = jlimit (0, fftSize / 2, (int) (adjustedY * (int) fftSize / 2));

      // Remaps a value from source range to target range
      auto level = jmap (fftData[fftDataIndex], 0.0f, jmax (range.getEnd(), 1e-5f), 0.0f, 1.0f);

      // setting pixel color
      // fromHSV: using floating point hue, saturation and brightness values, and an 8-bit alpha
      spectrogramImage.setPixelAt (rightEdge, yValue, Colour::fromHSV (level, saturation, level, 1.0f));
   }
}

