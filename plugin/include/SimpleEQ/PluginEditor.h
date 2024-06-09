#pragma once

#include "PluginProcessor.h"

namespace audio_plugin {

struct CustomRotarySlider : juce::Slider {
  CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                       juce::Slider::TextEntryBoxPosition::NoTextBox) {}
};

class SimpleEQEditor : public juce::AudioProcessorEditor, 
juce::AudioProcessorParameter::Listener,
juce::Timer
{
public:
  explicit SimpleEQEditor(SimpleEQAudioProcessor&);
  ~SimpleEQEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  /** Receives a callback when a parameter has been changed.
    IMPORTANT NOTE: This will be called synchronously when a parameter changes, and
    many audio processors will change their parameter during their audio callback.
    This means that not only has your handler code got to be completely thread-safe,
    but it's also got to be VERY fast, and avoid blocking. If you need to handle
    this event on your message thread, use this callback to trigger an AsyncUpdater
    or ChangeBroadcaster which you can respond to on the message thread.
*/
void parameterValueChanged(int parameterIndex, float newValue) override;

/** Indicates that a parameter change gesture has started.
    E.g. if the user is dragging a slider, this would be called with gestureIsStarting
    being true when they first press the mouse button, and it will be called again with
    gestureIsStarting being false when they release it.

    IMPORTANT NOTE: This will be called synchronously, and many audio processors will
    call it during their audio callback. This means that not only has your handler code
    got to be completely thread-safe, but it's also got to be VERY fast, and avoid
    blocking. If you need to handle this event on your message thread, use this callback
    to trigger an AsyncUpdater or ChangeBroadcaster which you can respond to later on the
    message thread.
*/
void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

void timerCallback() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &processorRef;

  juce::Atomic<bool> parametersChanged {false};

  CustomRotarySlider peakFreqSlider, peakGainSlider, peakQualitySlider;
  CustomRotarySlider lowCutFreqSlider, highCutFreqSlider;
  CustomRotarySlider lowCutSlopeSlider, highCutSlopeSlider; 

  std::vector<juce::Component*> getComps();

  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

  Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment; 
  Attachment lowCutFreqSliderAttachment, highCutFreqSliderAttachment;
  Attachment lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;

  MonoChain monoChain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQEditor)
};
} // namespace audio_plugin
