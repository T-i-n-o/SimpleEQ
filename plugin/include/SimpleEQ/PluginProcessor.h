#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace audio_plugin {

class SimpleEQ : public juce::AudioProcessor 
{
public:
  SimpleEQ();
  ~SimpleEQ() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  /*** User defined functions ***/ 

  /**
   * @brief Create the parameter layout for the plugin, all later used parameters are defined here
   * @return The parameter layout
  */
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  /**
   * @brief The AudioProcessorValueTreeState object that manages the parameters
  */
  juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:
  /**
   * @brief Alias for filter object
  */
  using Filter = juce::dsp::IIR::Filter<float>;

  /**
   * @brief Alias for filter chain holding differen type of filters
  */
  using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

  /**
   * @brief Alias for the mono chain holding the Signal filter chain
  */
  using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

  /**
   * @brief The Stereo Chain Objects
  */
  MonoChain leftChain, rightChain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQ)
};
} // namespace audio_plugin
