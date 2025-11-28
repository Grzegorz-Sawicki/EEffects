#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>

class EffectUI : public juce::Component
{
public:
    EffectUI(juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) noexcept
        : parameters(vts)
    {
        nameComponent = std::make_unique<NameComponent>(effectName);
        controlsComponent = std::make_unique<ControlsComponent>(parameters);
        bypassComponent = std::make_unique<BypassComponent>(parameters, mixParameter, bypassParameter);

        //setSize(kComponentW, kComponentH);

        addAndMakeVisible(*nameComponent);
        addAndMakeVisible(*controlsComponent);
        addAndMakeVisible(*bypassComponent);
    }

    ~EffectUI() override = default;

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::transparentBlack);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        nameComponent->setBounds(bounds.removeFromLeft(bounds.getWidth() / 5));
        bypassComponent->setBounds(bounds.removeFromRight(bounds.getWidth() / 4));
        controlsComponent->setBounds(bounds);
    }

    void addControl(juce::String labelText, juce::String parameterName)
    {
        controlsComponent->addControl(labelText, parameterName);
    }

protected:
    juce::AudioProcessorValueTreeState& parameters;

private:
    static constexpr int kComponentW = 500;
    static constexpr int kComponentH = 140;
    static constexpr int kBypassW = 32;
    static constexpr int kBypassH = 32;
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
            : vtsRef(vts) {}

        void addControl(const juce::String& labelText, const juce::String& parameterName)
        {
            auto slider = std::make_unique<juce::Slider>();
            slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            slider->setWantsKeyboardFocus(false);

            auto label = std::make_unique<juce::Label>();
            label->setJustificationType(juce::Justification::centred);
            label->setText(labelText, juce::dontSendNotification);

            using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
            auto attachment = std::make_unique<SliderAttachment>(vtsRef, parameterName, *slider);

            controls.emplace_back();
            auto& item = controls.back();
            item.slider = std::move(slider);
            item.label = std::move(label);
            item.attachment = std::move(attachment);
            item.paramId = parameterName;

            addAndMakeVisible(*item.slider);
            addAndMakeVisible(*item.label);

            resized();
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            for each (auto& item in controls)
            {
                item.slider->setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));
                item.label->setBounds(item.slider->getX(), item.slider->getY(), item.slider->getWidth(), kLabelH);
			}

            
        }

    private:
        struct ControlItem
        {
            std::unique_ptr<juce::Slider> slider;
            std::unique_ptr<juce::Label> label;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
            juce::String paramId;
        };

        juce::AudioProcessorValueTreeState& vtsRef;
        std::vector<ControlItem> controls;
    };

    struct BypassComponent : public juce::Component
    {
        BypassComponent(juce::AudioProcessorValueTreeState& vts, const juce::String& mixParameter, const juce::String& bypassParameter) 
			: vtsRef(vts), mixParameter(mixParameter.toStdString()), bypassParameter(bypassParameter.toStdString())
        {
            bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
            bypassButton.setWantsKeyboardFocus(false);
            addAndMakeVisible(bypassButton);

            bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                vts,
                this->bypassParameter,
                bypassButton
            );

			if(this->mixParameter != "") addMixControl("Mix", mixParameter);
        }

        void addMixControl(const juce::String& labelText, const juce::String& parameterName)
        {
            auto slider = std::make_unique<juce::Slider>();
            slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            slider->setWantsKeyboardFocus(false);

            auto label = std::make_unique<juce::Label>();
            label->setJustificationType(juce::Justification::centred);
            label->setText(labelText, juce::dontSendNotification);

            using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
            auto attachment = std::make_unique<SliderAttachment>(vtsRef, parameterName, *slider);

			mixControl = ControlItem{};
            mixControl.slider = std::move(slider);
            mixControl.label = std::move(label);
            mixControl.attachment = std::move(attachment);
            mixControl.paramId = parameterName;

            addAndMakeVisible(*mixControl.slider);
            addAndMakeVisible(*mixControl.label);

            resized();
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            bypassButton.setBounds(bounds.removeFromRight(kBypassW).reduced(4));
            if (this->mixParameter != "") {
                mixControl.slider->setBounds(bounds.removeFromRight(kKnobSz).reduced(4));
                mixControl.label->setBounds(mixControl.slider->getX(), mixControl.slider->getY(), mixControl.slider->getWidth(), kLabelH);
            }
			
        }

    private:
        struct ControlItem
        {
            std::unique_ptr<juce::Slider> slider;
            std::unique_ptr<juce::Label> label;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
            juce::String paramId;
        };

        juce::ToggleButton bypassButton;
        ControlItem mixControl;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
        juce::AudioProcessorValueTreeState& vtsRef;

        std::string mixParameter;
        std::string bypassParameter;
    };

    std::unique_ptr<NameComponent> nameComponent;
    std::unique_ptr<ControlsComponent> controlsComponent;
    std::unique_ptr<BypassComponent> bypassComponent;
};