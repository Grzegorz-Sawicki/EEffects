#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>

class EffectUI : public juce::Component
{
public:
    EffectUI(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String effectName, juce::String mixParameter, juce::String bypassParameter) noexcept
        : parameters(vts)
    {
        nameComponent = std::make_unique<NameComponent>(effectName);
        controlsComponent = std::make_unique<ControlsComponent>(editorIn, parameters);
        bypassComponent = std::make_unique<BypassComponent>(editorIn, parameters, mixParameter, bypassParameter);

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

    struct SliderTest : public juce::Slider
    {
        SliderTest(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, juce::String paramId)
            : editor(editorIn), vtsRef(vts), paramId(paramId)
        {
        }

        void mouseUp(const juce::MouseEvent& e) override
        {
            if (e.mods.isRightButtonDown())
            {
                if (auto* c = editor.getHostContext())
                {
                    if (auto menuInfo = c->getContextMenuForParameter(vtsRef.getParameter(paramId)))
                    {
                        menuInfo->getEquivalentPopupMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(this)
                            .withMousePosition());
                        DBG("Showing context menu for parameter: " + paramId);
                    }
                }
            }
        }

        juce::String paramId;
        juce::AudioProcessorValueTreeState& vtsRef;
        juce::AudioProcessorEditor& editor;
    };

    struct ControlItem : public juce::Component
    {
        ControlItem() {}

        std::unique_ptr<SliderTest> slider;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

        juce::String paramId;
    };

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
        ControlsComponent(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts)
            : editor(editorIn), vtsRef(vts) {}

        void addControl(const juce::String& labelText, const juce::String& parameterName)
        {
            auto slider = std::make_unique<SliderTest>(editor, vtsRef, parameterName);
            slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            slider->setWantsKeyboardFocus(false);

            auto label = std::make_unique<juce::Label>();
            label->setJustificationType(juce::Justification::centred);
            label->setText(labelText, juce::dontSendNotification);

            using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
            auto attachment = std::make_unique<SliderAttachment>(vtsRef, parameterName, *slider);

            auto item = std::make_unique<ControlItem>();
            item->slider = std::move(slider);
            item->label = std::move(label);
            item->attachment = std::move(attachment);

			addAndMakeVisible(*item->slider);
			addAndMakeVisible(*item->label);

            controls.emplace_back(std::move(item));

            resized();
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            for (auto& itemPtr : controls)
            {
                if (! itemPtr) continue;
                itemPtr->slider->setBounds(bounds.removeFromLeft(kKnobSz).reduced(4));
                itemPtr->label->setBounds(itemPtr->slider->getX(), itemPtr->slider->getY(), itemPtr->slider->getWidth(), kLabelH);
            }
        }

    private:
        juce::AudioProcessorValueTreeState& vtsRef;
        juce::AudioProcessorEditor& editor;
        std::vector<std::unique_ptr<ControlItem>> controls;
    };

    struct BypassComponent : public juce::Component
    {
        BypassComponent(AudioProcessorEditor& editorIn, juce::AudioProcessorValueTreeState& vts, const juce::String& mixParameter, const juce::String& bypassParameter)
            : editor(editorIn), vtsRef(vts), mixParameter(mixParameter.toStdString()), bypassParameter(bypassParameter.toStdString())
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
            auto slider = std::make_unique<SliderTest>(editor, vtsRef, parameterName);
            slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
            slider->setWantsKeyboardFocus(false);

            auto label = std::make_unique<juce::Label>();
            label->setJustificationType(juce::Justification::centred);
            label->setText(labelText, juce::dontSendNotification);

            using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
            auto attachment = std::make_unique<SliderAttachment>(vtsRef, parameterName, *slider);

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
        juce::ToggleButton bypassButton;
        ControlItem mixControl;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
        juce::AudioProcessorValueTreeState& vtsRef;
		juce::AudioProcessorEditor& editor;

        std::string mixParameter;
        std::string bypassParameter;
    };

    std::unique_ptr<NameComponent> nameComponent;
    std::unique_ptr<ControlsComponent> controlsComponent;
    std::unique_ptr<BypassComponent> bypassComponent;
};