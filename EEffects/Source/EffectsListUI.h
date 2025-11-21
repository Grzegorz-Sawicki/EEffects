#pragma once

#include <JuceHeader.h>

class EffectsListUI : 
    public juce::Component, 
    public juce::ListBoxModel, 
    public juce::DragAndDropContainer,
    public juce::DragAndDropTarget
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
        // Read source index from drag description (digits)
        auto desc = dragSourceDetails.description.toString();
        if (! desc.containsOnly ("0123456789"))
            return;

        const int src = desc.getIntValue();
        if (src < 0 || src >= (int) names.size())
            return;

        // Convert drop position to listBox coordinates
        juce::Point<int> p = listBox.getLocalPoint (this, dragSourceDetails.localPosition.toInt());

        // Get insertion index for the position
        int dst = listBox.getInsertionIndexForPosition (p.x, p.y);
        DBG("DST 1 = " + juce::String(dst));

        // Clamp dst to valid range [0..names.size()]
        dst = juce::jlimit (0, (int) names.size(), dst);
        DBG("DST 2 = " + juce::String(dst));

        if (dst == src || dst == src + 1)
        {
            repaint();
            return;
        }

        // Use std::rotate to move element safely (avoids iterator/proxy issues with vector<bool>)
        if (dst > src)
        {
            // move element at src to position dst-1
            std::rotate(names.begin() + src, names.begin() + src + 1, names.begin() + dst);
            std::rotate(activesVec.begin() + src, activesVec.begin() + src + 1, activesVec.begin() + dst);
            int newDst = dst - 1;
            listBox.updateContent();
            listBox.repaint();
            if (onRowMoved) onRowMoved (src, newDst);
        }
        else // dst < src
        {
            // move element at src to position dst
            std::rotate(names.begin() + dst, names.begin() + src, names.begin() + src + 1);
            std::rotate(activesVec.begin() + dst, activesVec.begin() + src, activesVec.begin() + src + 1);
            listBox.updateContent();
            listBox.repaint();
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
            auto nameArea = area.removeFromLeft(area.getWidth() / 4);
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