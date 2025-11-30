#pragma once

#include <JuceHeader.h>
#include "EffectInfo.h"

class EffectsListUI : 
    public juce::Component, 
    public juce::ListBoxModel, 
    public juce::DragAndDropContainer,
    public juce::DragAndDropTarget
{
public:
    EffectsListUI(std::vector<EffectInfo> effectsInfo) :
        effectsInfo(effectsInfo)
    {
        listBox.setModel(this);
        listBox.setRowHeight(rowHeight);
        addAndMakeVisible(listBox);
    }

    ~EffectsListUI() override = default;

    void setEffectsInfo(std::vector<EffectInfo>& effectsInfo)
    {
        this->effectsInfo = effectsInfo;
        listBox.updateContent();
        listBox.repaint();
    }

    std::function<void(int, bool)> onToggleChanged; // (index, state)
    std::function<void(int, int)> onRowMoved;       // (from, to)

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
    }

    void resized() override
    {
        listBox.setBounds(getLocalBounds());
    }

    var getDragSourceDescription(const SparseSet<int>& selectedRows) override
    {
        String row;

		row = String(selectedRows[0]);

        return row;
    }

    //Drag and drop

    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& /*dragSourceDetails*/) override
    {
        return true;
	}

    void itemDragEnter(const SourceDetails& /*dragSourceDetails*/) override
    {
        repaint();
    }

    void itemDragMove(const SourceDetails& /*dragSourceDetails*/) override
    {

    }

    void itemDragExit(const SourceDetails& /*dragSourceDetails*/) override
    {
        repaint();
    }

    void itemDropped(const SourceDetails& dragSourceDetails) override
    {
        auto desc = dragSourceDetails.description.toString();
        if (! desc.containsOnly ("0123456789"))
            return;

        const int src = desc.getIntValue();
        if (src < 0 || src >= (int) effectsInfo.size())
            return;

        juce::Point<int> p = listBox.getLocalPoint (this, dragSourceDetails.localPosition.toInt());

        int dst = listBox.getInsertionIndexForPosition (p.x, p.y);

        dst = juce::jlimit (0, (int) effectsInfo.size(), dst);

        if (dst == src || dst == src + 1)
        {
            repaint();
            return;
        }

        if (dst > src)
        {           
            int newDst = dst - 1;
            if (onRowMoved) onRowMoved (src, newDst);
        }
        else // dst < src
        {
            if (onRowMoved) onRowMoved (src, dst);
        }

        repaint();
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
            auto nameArea = area.removeFromLeft(area.getWidth() / 3);
            auto toggleArea = area.removeFromRight(24);
            nameLabel.setBounds(nameArea);
            toggle.setBounds(toggleArea);
        }

        juce::Label nameLabel;
        juce::ToggleButton toggle;

        std::function<void(int)> onClicked;
        std::function<void(int, bool)> onToggled;

    private:
        int rowIndex = -1;
        juce::Point<int> dragStart;
    };

    int getNumRows() override
    {
        return static_cast<int> (effectsInfo.size());
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(findColour(juce::ListBox::backgroundColourId).brighter(0.05f));
    }

    juce::Component* refreshComponentForRow(int rowNumber, bool /*isRowSelected*/, juce::Component* existingComponentToUpdate) override
    {
        if (rowNumber < 0 || rowNumber >= (int)effectsInfo.size())
            return existingComponentToUpdate;

        RowComponent* rc = dynamic_cast<RowComponent*>(existingComponentToUpdate);
        if (rc == nullptr)
            rc = new RowComponent();

        rc->setIndex(rowNumber);
        rc->setName(effectsInfo[(size_t)rowNumber].name);
        rc->setActive(effectsInfo[(size_t)rowNumber].active);

        rc->onToggled = [this](int idx, bool state)
            {
                if (isPositiveAndBelow(idx, (int)effectsInfo.size()))
                    effectsInfo[(size_t)idx].active = state;

                if (onToggleChanged) onToggleChanged(idx, state);
            };

        return rc;
    }

    juce::ListBox listBox{ "EffectsList", this };
    std::vector<EffectInfo> effectsInfo;

    static constexpr int rowHeight = 28;
};