#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include <map>
#include "EffectUI.h"
#include "ReverbUI.h"
#include "DelayUI.h"
#include "FilterUI.h"
#include "ConvolutionUI.h"

class EffectsRackUI :
    public juce::Component
{
public:
    EffectsRackUI(juce::AudioProcessorValueTreeState& vts, std::vector<EffectInfo> effectsInfo)
        : effectsInfo(std::move(effectsInfo))
    {
        addAndMakeVisible(viewport);
        viewport.setViewedComponent(&content, false);
        viewport.setScrollBarsShown(true, true);        
        viewport.setScrollBarThickness(12);            

        ownedEffectUIs["Reverb"] = std::make_unique<ReverbUI>(vts, "Reverb", ID::reverbMix, ID::reverbBypass);
        ownedEffectUIs["Delay"]  = std::make_unique<DelayUI> (vts, "Delay",  ID::delayMix, ID::delayBypass);
        ownedEffectUIs["Filter"] = std::make_unique<FilterUI>(vts, "Filter", "", ID::filterBypass);
		ownedEffectUIs["Convolution"] = std::make_unique<ConvolutionUI>(vts, "Convolution", ID::convolutionMix, ID::convolutionBypass);

        rebuildVisibleComponents();
    }

    ~EffectsRackUI() override = default;

    void setEffectsInfo(std::vector<EffectInfo>& effectsInfoIn)
    {
        effectsInfo = effectsInfoIn;
        rebuildVisibleComponents();
        resized();
        repaint();
    }

    void update()
    {
        rebuildVisibleComponents();
        resized();
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::grey);
    }

    void resized() override
    {
        viewport.setBounds(getLocalBounds());

        int totalH = static_cast<int>(visibleRowIndices.size()) * rowHeight;
        content.setBounds(0, 0, getWidth(), totalH);

        for (size_t i = 0; i < visibleRowIndices.size(); ++i)
        {
            int realIndex = visibleRowIndices[i];
            const auto& name = effectsInfo[(size_t)realIndex].name;
            auto it = ownedEffectUIs.find(name.toStdString());
            if (it != ownedEffectUIs.end() && it->second)
            {
                auto* comp = it->second.get();
                comp->setBounds(0, static_cast<int>(i) * rowHeight, getWidth(), rowHeight);
            }
        }
    }

private:
    void rebuildVisibleComponents()
    {
        visibleRowIndices.clear();
        visibleRowIndices.reserve(effectsInfo.size());

        for (size_t i = 0; i < effectsInfo.size(); ++i)
        {
            if (effectsInfo[i].active)
                visibleRowIndices.push_back(static_cast<int>(i));
        }

        content.removeAllChildren();

        for (int realIndex : visibleRowIndices)
        {
            const auto& name = effectsInfo[(size_t)realIndex].name;
            auto it = ownedEffectUIs.find(name.toStdString());
            if (it != ownedEffectUIs.end() && it->second)
            {
                content.addAndMakeVisible(it->second.get());
            }
            else
            {
                
            }
        }
    }

    juce::Viewport viewport { "EffectsViewport" };
    juce::Component content; 

    std::map<std::string, std::unique_ptr<EffectUI>> ownedEffectUIs;

    std::vector<int> visibleRowIndices;

    std::vector<EffectInfo> effectsInfo;

    static constexpr int rowHeight = 140;
};