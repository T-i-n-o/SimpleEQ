#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace audio_plugin {

enum Slope {
  Slope_12,
  Slope_24,
  Slope_36,
  Slope_48
};

/**
 * @brief The ChainSettings struct holds the settings for the filter chain
*/
struct ChainSettings {
  float peakFreq {0}, peakGainInDecibels {0}, peakQuality {1.f};
  float lowCutFreq {0}, highCutFreq {0};
  Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
};

/**
 * @brief Get the Chain Settings object
 * @param apvts The AudioProcessorValueTreeState object
 * @return The ChainSettings object
*/
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts);

/**
 * @brief The Audio Processor class for the SimpleEQ plugin
*/
class SimpleEQAudioProcessor : public juce::AudioProcessor 
{
public:
  SimpleEQAudioProcessor();
  ~SimpleEQAudioProcessor() override;

  /**
   * @brief prepareToPlay is called before the plugin starts processing audio
  */
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  /**
   * @brief processBlock is called to process the audio
   * @param buffer The audio buffer
   * @param midiMessages The midi buffer
  */
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

  /**
   * @brief Get the state information of all plugin parameters
  */
  void getStateInformation(juce::MemoryBlock &destData) override;

  /**
   * @brief Set the state information of all plugin parameters
  */
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

  /**
   * @brief Enum for the Chain positions in the filter chain
  */
  enum ChainPositions {
    LowCut,
    Peak,
    HighCut
  }; 

  /**
   * @brief Update the Peak filter
  */
  void updatePeakFilter(const ChainSettings &chainSettings);

  /**
   * @brief Alias for the Coefficients object
  */
  using Coefficients = Filter::CoefficientsPtr;

  static void updateCoefficients(Coefficients &old, const Coefficients &replacements);

  /**
   * @brief Update coefficients for the filter chain 
   * @param Index The index of the filter in the chain
   * @param chain The filter chain
   * @param coefficients The coefficients for the filter
   **/
  template <int Index, typename ChainType, typename CoefficientType> 
  void update(ChainType &chain, const CoefficientType &coefficients)
  {
    updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
    chain.template setBypassed<Index>(false);
  }

  /**
   * @brief Update the filters in the filter chain
   * @param ChainType The type of the filter chain
   * @param CoefficientType The data type of the coefficients
  */
  template <typename ChainType, typename CoefficientType>
  void updateCutFilters(ChainType &chain, const CoefficientType &coefficients, const Slope &slope)
  {
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch (slope) {
      case Slope_48:
        update<3>(chain, coefficients);
      case Slope_36:
        update<2>(chain, coefficients);
      case Slope_24:
        update<1>(chain, coefficients);
      case Slope_12:
        update<0>(chain, coefficients);
        break;
    };
  }

  void updateLowCutFilters(const ChainSettings &chainSettings);
  void updateHighCutFilters(const ChainSettings &chainSettings);

  void updateFilters();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessor)
};
} // namespace audio_plugin
