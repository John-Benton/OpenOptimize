/*
  ==============================================================================

    settings_bar.h
    Created: 14 Sep 2017 10:04:59pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "spl_meter.h"
#include "delay_indicator.h"
#include "controls.h"

//==============================================================================
/*
*/
class settings_bar    : public Component
{
public:

	controls main_controls;

    settings_bar()
    {
		addAndMakeVisible(main_spl_meter);
		addAndMakeVisible(main_delay_indicator);
		addAndMakeVisible(main_controls);
    }

    ~settings_bar()
    {
    }

    void paint (Graphics& g) override
    {
		
		g.fillAll(Colour(30, 30, 30));
		 
    }

    void resized() override
    {
		Rectangle<int> settings_bar_outline(getLocalBounds());

		int spl_meter_width = getWidth()*0.25;
		main_spl_meter.setBounds(settings_bar_outline.removeFromLeft(spl_meter_width));

		int delay_indicator_width = getWidth()*0.25;
		main_delay_indicator.setBounds(settings_bar_outline.removeFromLeft(delay_indicator_width));

		main_controls.setBounds(settings_bar_outline);

    }

	spl_meter main_spl_meter;

	delay_indicator main_delay_indicator;

private:
		
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (settings_bar)
};
