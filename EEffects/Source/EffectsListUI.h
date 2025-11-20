#pragma once

#include <JuceHeader.h>

class EffectsListUI : public juce::Component, public juce::ListBoxModel
{
public:
    EffectsListUI()
    {
        listBox.setModel(this);
        listBox.setRowHeight(rowHeight);
        addAndMakeVisible(listBox);
    }

    ~EffectsListUI() override = default;

    void setEffects(const std::vector<juce::String>& namesIn, const std::vector<bool>* actives = nullptr)
    {
        names = namesIn;
        activesVec.clear();
        if (actives != nullptr)
            activesVec = *actives;
        else
            activesVec.assign(names.size(), true);

        listBox.updateContent();
        listBox.repaint();
    }

    void setRowActive(int index, bool active)
    {
        if (isPositiveAndBelow(index, (int)activesVec.size()))
        {
            activesVec[(size_t)index] = active;
            if (auto* rc = dynamic_cast<RowComponent*> (listBox.getComponentForRowNumber(index)))
                rc->setActive(active);
            listBox.repaintRow(index);
        }
    }

    void setRowName(int index, const juce::String& name)
    {
        if (isPositiveAndBelow(index, (int)names.size()))
        {
            names[(size_t)index] = name;
            if (auto* rc = dynamic_cast<RowComponent*> (listBox.getComponentForRowNumber(index)))
                rc->setName(name);
            listBox.repaintRow(index);
        }
    }

    std::function<void(int, bool)> onToggleChanged; // (index, state)
    std::function<void(int)> onRowClicked;         // (index)
    std::function<void(int, int)> onRowMoved;       // (from, to) - not used here yet

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
    }

    void resized() override
    {
        listBox.setBounds(getLocalBounds());
    }

private:
    struct RowComponent : public juce::Component
    {
        RowComponent()
        {
            this->setInterceptsMouseClicks(false, true);

            nameLabel.setJustificationType(juce::Justification::centredLeft);
            toggle.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);

            addAndMakeVisible(nameLabel);
            addAndMakeVisible(toggle);

            toggle.onClick = [this]()
                {
                    if (onToggled)
                        onToggled(rowIndex, toggle.getToggleState());
                };
        }

        void setIndex(int i) noexcept { rowIndex = i; }
        void setName(const juce::String& t) { nameLabel.setText(t, juce::dontSendNotification); }
        void setActive(bool a) { toggle.setToggleState(a, juce::dontSendNotification); }

        void resized() override
        {
            auto area = getLocalBounds();
            auto nameArea = area.removeFromLeft(area.getWidth() / 4);
            auto toggleArea = area.removeFromRight(24);
            nameLabel.setBounds(nameArea);
            toggle.setBounds(toggleArea);
        }

        void mouseDown(const juce::MouseEvent&) override
        {
            if (onClicked)
                onClicked(rowIndex);
        }

        juce::Label nameLabel;
        juce::ToggleButton toggle;

        std::function<void(int)> onClicked;
        std::function<void(int, bool)> onToggled;

    private:
        int rowIndex = -1;
    };

    int getNumRows() override
    {
        return static_cast<int> (names.size());
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(findColour(juce::ListBox::backgroundColourId).brighter(0.05f));
    }

    juce::Component* refreshComponentForRow(int rowNumber, bool /*isRowSelected*/, juce::Component* existingComponentToUpdate) override
    {
        if (rowNumber < 0 || rowNumber >= (int)names.size())
            return existingComponentToUpdate;

        RowComponent* rc = dynamic_cast<RowComponent*>(existingComponentToUpdate);
        if (rc == nullptr)
            rc = new RowComponent();

        rc->setIndex(rowNumber);
        rc->setName(names[(size_t)rowNumber]);
        const bool active = (rowNumber < (int)activesVec.size()) ? activesVec[(size_t)rowNumber] : true;
        rc->setActive(active);

        rc->onClicked = [this](int idx) { if (onRowClicked) onRowClicked(idx); };
        rc->onToggled = [this](int idx, bool state)
            {
                if (isPositiveAndBelow(idx, (int)activesVec.size()))
                    activesVec[(size_t)idx] = state;

                if (onToggleChanged) onToggleChanged(idx, state);
            };

        return rc;
    }

    juce::ListBox listBox{ "EffectsList", this };
    std::vector<juce::String> names;
    std::vector<bool> activesVec;

    static constexpr int rowHeight = 28;
};