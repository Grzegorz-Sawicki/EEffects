#pragma once

#include <JuceHeader.h>

class EffectUI : public juce::Component
{
public:
    EffectUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String bypassParameter) noexcept : parameters(vts)
	{
		nameComponent = std::make_unique<NameComponent>(effectName);
		controlsComponent = std::make_unique<ControlsComponent>(parameters);
		bypassComponent = std::make_unique<BypassComponent>(parameters, bypassParameter);

		setSize(kComponentW, kComponentH);

		addAndMakeVisible(*nameComponent);
		addAndMakeVisible(*controlsComponent);
		addAndMakeVisible(*bypassComponent);
	}

    ~EffectUI() override = default;

	void paint(juce::Graphics& g) override {
		g.fillAll(juce::Colours::transparentBlack);
	}

	void resized() override {
		auto bounds = getLocalBounds();
		nameComponent->setBounds(bounds.removeFromLeft(bounds.getWidth() / 5));
		bypassComponent->setBounds(bounds.removeFromRight(bounds.getWidth() / 10));
		controlsComponent->setBounds(bounds);
	}

	void addControl(juce::String labelText, juce::String parameterName) {
		juce::Slider slider;
		juce::Label label;
		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

		slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
		slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
		slider.setWantsKeyboardFocus(false);
		controlsComponent->addAndMakeVisible(slider);
		label.setJustificationType(juce::Justification::centred);
		label.setText(labelText, juce::dontSendNotification);
		controlsComponent->addAndMakeVisible(label);
		attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, parameterName, slider);
	}



    

protected:
    juce::AudioProcessorValueTreeState& parameters;

private:
	static constexpr int kComponentW = 500;
	static constexpr int kComponentH = 140;
	static constexpr int kBypassW = 24;
	static constexpr int kBypassH = 24;
	static constexpr int kKnobSz = 80;
	static constexpr int kLabelH = 16;
	static constexpr int kPadding = 8;

	struct NameComponent : public juce::Component
	{
		NameComponent(const juce::String& name)
		{
			label.setText(name, juce::dontSendNotification);
			label.setJustificationType(juce::Justification::centred);
			addAndMakeVisible(label);
		}

		void resized() override
		{
			label.setBounds(getLocalBounds());
		}

	private:
		juce::Label label;
	};

	struct ControlsComponent : public juce::Component
	{
		ControlsComponent(juce::AudioProcessorValueTreeState& vts)
		{
			auto setup = [this](juce::Slider& s)
				{
					s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
					s.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
					s.setWantsKeyboardFocus(false);
					addAndMakeVisible(s);
				};

			setup(wetSlider);
			setup(roomSlider);
			setup(dampingSlider);
			setup(widthSlider);

			wetLabel.setJustificationType(juce::Justification::centred);
			roomLabel.setJustificationType(juce::Justification::centred);
			dampingLabel.setJustificationType(juce::Justification::centred);
			widthLabel.setJustificationType(juce::Justification::centred);

			wetLabel.setText("Wet", juce::dontSendNotification);
			roomLabel.setText("Room", juce::dontSendNotification);
			dampingLabel.setText("Damp", juce::dontSendNotification);
			widthLabel.setText("Width", juce::dontSendNotification);

			addAndMakeVisible(wetLabel);
			addAndMakeVisible(roomLabel);
			addAndMakeVisible(dampingLabel);
			addAndMakeVisible(widthLabel);

			wetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "reverbWet", wetSlider);
			roomAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "reverbRoom", roomSlider);
			dampingAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "reverbDamping", dampingSlider);
			widthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "reverbWidth", widthSlider);
		}

		void resized() override
		{
			auto bounds = getLocalBounds();
			wetSlider.setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));
			roomSlider.setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));
			dampingSlider.setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));
			widthSlider.setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));

			wetLabel.setBounds(wetSlider.getX(), wetSlider.getY(), wetSlider.getWidth(), kLabelH);
			roomLabel.setBounds(roomSlider.getX(), roomSlider.getY(), roomSlider.getWidth(), kLabelH);
			dampingLabel.setBounds(dampingSlider.getX(), dampingSlider.getY(), dampingSlider.getWidth(), kLabelH);
			widthLabel.setBounds(widthSlider.getX(), widthSlider.getY(), widthSlider.getWidth(), kLabelH);
		}

	private:
		juce::Slider wetSlider, roomSlider, dampingSlider, widthSlider;
		juce::Label wetLabel, roomLabel, dampingLabel, widthLabel;
		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetAttach, roomAttach, dampingAttach, widthAttach;
	};

	struct BypassComponent : public juce::Component
	{
		BypassComponent(juce::AudioProcessorValueTreeState& vts, juce::String bypassParameter)
		{
			bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
			bypassButton.setWantsKeyboardFocus(false);
			addAndMakeVisible(bypassButton);

			bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
				vts,
				bypassParameter,
				bypassButton
			);
		}
		void resized() override
		{
			bypassButton.setBounds(getLocalBounds());
		}
	private:
		juce::ToggleButton bypassButton;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
	};

	std::unique_ptr<NameComponent> nameComponent;
	std::unique_ptr<ControlsComponent> controlsComponent;
	std::unique_ptr<BypassComponent> bypassComponent;
};