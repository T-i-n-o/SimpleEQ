#include "SimpleEQ/PluginEditor.h"
#include "SimpleEQ/PluginProcessor.h"

namespace audio_plugin {

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor &p) : processorRef(p) 
{
  const auto& params = processorRef.getParameters();
  for(auto param : params) {
    param->addListener(this);
  }

  startTimerHz(60);
  parametersChanged.set(true);
}

ResponseCurveComponent::~ResponseCurveComponent() 
{
  const auto& params = processorRef.getParameters();
  for(auto param : params) {
    param->removeListener(this);
  }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue) 
{
  // ignore unused parameterIndex
  juce::ignoreUnused(parameterIndex);
  juce::ignoreUnused(newValue);
  
  parametersChanged.set(true);
}

void ResponseCurveComponent::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) 
{
  // ignore unused parameterIndex
  juce::ignoreUnused(parameterIndex);

  if(!gestureIsStarting) {
    parametersChanged.set(false);
  }
}

void ResponseCurveComponent::timerCallback() 
{
  if(parametersChanged.compareAndSetBool(false, true)) {
    // update the UI
    auto chainSettings = getChainSettings(processorRef.apvts);
    auto sampleRate = processorRef.getSampleRate();

    auto peakCoefficients = makePeakFilter(chainSettings, sampleRate);

    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

    auto lowCutCoefficients = makeLowCutFilter(chainSettings, sampleRate);
    auto highCutCoefficients = makeHighCutFilter(chainSettings, sampleRate);

    updateCutFilters(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilters(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);

    repaint();
  }

}

void ResponseCurveComponent::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  using namespace juce;
  g.fillAll(Colours::black);

  auto responseArea = getLocalBounds();

  auto w = responseArea.getWidth();

  auto& lowCut = monoChain.get<ChainPositions::LowCut>();
  auto& peak = monoChain.get<ChainPositions::Peak>();
  auto& highCut = monoChain.get<ChainPositions::HighCut>();

  auto sampleRate = processorRef.getSampleRate();

  std::vector<double> mags;

  mags.resize(static_cast<size_t>(w));

  // compute maknitude of the Eq Curve for every pixel of the width
  for(int i = 0; i < w; i++) {
    // 1.0 is the default magnitude
    double mag = 1.f;
    auto freq = mapToLog10<double>(double(i) / double(w), 20, 20000);

    // compute the magnitude of the filter chain
    // peak filter
    if(!monoChain.isBypassed<ChainPositions::Peak>()) {
      mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    // low cut filter
    if(!lowCut.isBypassed<0>()) {
      mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!lowCut.isBypassed<1>()) {
      mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!lowCut.isBypassed<2>()) {
      mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!lowCut.isBypassed<3>()) {
      mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    // high cut filter
    if(!highCut.isBypassed<0>()) {
      mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!highCut.isBypassed<1>()) {
      mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!highCut.isBypassed<2>()) {
      mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }
    if(!highCut.isBypassed<3>()) {
      mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    }

    mags[static_cast<size_t>(i)] = Decibels::gainToDecibels(mag);
  }

  Path responseCurve;

  const double outputMin = responseArea.getBottom();
  const double outputMax = responseArea.getY();

  // Map decibels to Screen coordinates
  auto map = [outputMin, outputMax](double input) {
    return jmap(input, -24.0, 24.0, outputMin, outputMax);
  };
  
  responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

  for(int i = 1; i < static_cast<int>(mags.size()); i++) {
    responseCurve.lineTo(responseArea.getX() + i, map(mags[static_cast<size_t>(i)]));
  }

  g.setColour(Colours::grey);
  g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);

  g.setColour(Colours::white);
  g.strokePath(responseCurve, PathStrokeType(2.f));
}

SimpleEQEditor::SimpleEQEditor(SimpleEQAudioProcessor &p) : AudioProcessorEditor(&p), 
  processorRef(p), 
  responseCurveComponent(p),
  peakFreqSlider(*processorRef.apvts.getParameter("Peak Freq"), "Hz"),
  peakGainSlider(*processorRef.apvts.getParameter("Peak Gain"), "dB"),
  peakQualitySlider(*processorRef.apvts.getParameter("Peak Quality"), ""),
  lowCutFreqSlider(*processorRef.apvts.getParameter("LowCut Freq"), "Hz"),
  lowCutSlopeSlider(*processorRef.apvts.getParameter("LowCut Slope"), "dB/Oct"),
  highCutFreqSlider(*processorRef.apvts.getParameter("HighCut Freq"), "Hz"),
  highCutSlopeSlider(*processorRef.apvts.getParameter("HighCut Slope"), "dB/Oct"),
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
  setSize(800, 600);
}

SimpleEQEditor::~SimpleEQEditor() {}

void SimpleEQEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  auto bounds = getLocalBounds();
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

  responseCurveComponent.setBounds(responseArea);

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

void SimpleEQEditor::paint(juce::Graphics &g) {
  // Fill the whole window black
  g.fillAll(juce::Colours::black);
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
      &highCutSlopeSlider,
      &responseCurveComponent
  };
}

} // namespace audio_plugin
