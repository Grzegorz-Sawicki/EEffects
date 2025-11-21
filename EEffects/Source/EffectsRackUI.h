#pragma once

#include <JuceHeader.h>
#include <vector>
#include "EffectUI.h"

//TODO: rethink list and rack setup

class EffectsRackUI : 
    public juce::Component,
    public juce::ListBoxModel
{
public:
    EffectsRackUI() 
    {
		listBox.setModel(this);
		listBox.setRowHeight(rowHeight);
		addAndMakeVisible(listBox);
    }

	~EffectsRackUI() override = default;

    void addEffectUI(std::unique_ptr<EffectUI> effectUI)
    {
        if (!effectUI) return;
        auto* p = effectUI.get();
        effectUIs.push_back(std::move(p));
    }

    void update() {
        listBox.updateContent();
		listBox.repaint();
    }

    void moveEffectUI(int src, int dst)
    {
        if (src < 0 || src >= (int)effectUIs.size() ||
            dst < 0 || dst >= (int)effectUIs.size() ||
            src == dst)
            return;

        if (dst > src) {
            int newDst = dst + 1;
			std::rotate(effectUIs.begin() + src, effectUIs.begin() + src + 1, effectUIs.begin() + newDst);
            listBox.updateContent();
            listBox.repaint();
        }
        else // dst < src
        {
            std::rotate(effectUIs.begin() + dst,  effectUIs.begin() + src, effectUIs.begin() + src + 1);
            listBox.updateContent();
            listBox.repaint();
		}
	}   

    void setEffectUIs(const std::vector<EffectUI*>& effectUIsIn)
    {
        effectUIs = effectUIsIn;
        listBox.updateContent();
		listBox.repaint();
	}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::grey);
    }

    void resized() override
    {
        listBox.setBounds(getLocalBounds());
	}

private:
    int getNumRows() override
    {
        return static_cast<int>(effectUIs.size());
	}

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        // No custom painting needed as we use components
	}

    juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override
    {
        if (isPositiveAndBelow(rowNumber, (int)effectUIs.size()))
        {
            EffectUI* effectUI = effectUIs[(size_t)rowNumber];
            return effectUI;

        }
        return nullptr;
	}

    juce::ListBox listBox{ "EffectsRack" , this };
	std::vector<EffectUI*> effectUIs;

	static constexpr int rowHeight = 140;
};