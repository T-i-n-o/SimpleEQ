#pragma once

#include "PluginProcessor.h"

namespace audio_plugin {

class SimpleEQEditor : public juce::AudioProcessorEditor {
public:
  explicit SimpleEQEditor(SimpleEQ &);
  ~SimpleEQEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQ &processorRef;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQEditor)
};
} // namespace audio_plugin
