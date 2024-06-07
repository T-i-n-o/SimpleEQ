#pragma once

#include "PluginProcessor.h"

namespace audio_plugin {

struct CustomRotarySlider : juce::Slider {
  CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                       juce::Slider::TextEntryBoxPosition::NoTextBox) {}
};

class SimpleEQEditor : public juce::AudioProcessorEditor {
public:
  explicit SimpleEQEditor(SimpleEQAudioProcessor&);
  ~SimpleEQEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &processorRef;

  CustomRotarySlider peakFreqSlider, peakGainSlider, peakQualitySlider;
  CustomRotarySlider lowCutFreqSlider, highCutFreqSlider;
  CustomRotarySlider lowCutSlopeSlider, highCutSlopeSlider; 

  std::vector<juce::Component*> getComps();

  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

  Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment; 
  Attachment lowCutFreqSliderAttachment, highCutFreqSliderAttachment;
  Attachment lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQEditor)
};
} // namespace audio_plugin
