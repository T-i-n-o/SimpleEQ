#include "SimpleEQ/PluginEditor.h"
#include "SimpleEQ/PluginProcessor.h"

namespace audio_plugin {
SimpleEQEditor::SimpleEQEditor(SimpleEQAudioProcessor &p) : AudioProcessorEditor(&p), processorRef(p), 
  peakFreqSliderAttachment(processorRef.apvts, "Peak Freq", peakFreqSlider),
  peakGainSliderAttachment(processorRef.apvts, "Peak Gain", peakGainSlider),
  peakQualitySliderAttachment(processorRef.apvts, "Peak Quality", peakQualitySlider),
  lowCutFreqSliderAttachment(processorRef.apvts, "LowCut Freq", lowCutFreqSlider),
  highCutFreqSliderAttachment(processorRef.apvts, "HighCut Freq", highCutFreqSlider),
  lowCutSlopeSliderAttachment(processorRef.apvts, "LowCut Slope", lowCutSlopeSlider),
  highCutSlopeSliderAttachment(processorRef.apvts, "HighCut Slope", highCutSlopeSlider)
{
  juce::ignoreUnused(processorRef);
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  for (auto *comp : getComps()) {
    addAndMakeVisible(comp);
  }

  setSize(600, 400);
}

SimpleEQEditor::~SimpleEQEditor() {}

void SimpleEQEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  // g.setColour(juce::Colours::white);
  // g.setFont(15.0f);
  // g.drawFittedText("Hello World!", getLocalBounds(),
  //                  juce::Justification::centred, 1);
}

void SimpleEQEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  auto bounds = getLocalBounds();
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

  auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
  auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
  
  lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.66));
  lowCutSlopeSlider.setBounds(lowCutArea);

  highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.66));
  highCutSlopeSlider.setBounds(highCutArea);

  peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.66));
  peakGainSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() * 0.5)); 
  peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component *> SimpleEQEditor::getComps() 
{
  return 
  {
      &peakFreqSlider, 
      &peakGainSlider, 
      &peakQualitySlider,
      &lowCutFreqSlider, 
      &highCutFreqSlider,
      &lowCutSlopeSlider,
      &highCutSlopeSlider
  };
}

} // namespace audio_plugin
