/*
==============================================================================

level_meters.h
Created: 14 Sep 2017 10:17:55pm
Author:  John

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>
#include <numeric>

//==============================================================================
/*
*/
class level_meters : public Component
{
public:

	Rectangle<int> component_outline;
	Rectangle<int> text_region;
	Rectangle<int> ref_meter_text_region;
	Rectangle<int> system_meter_text_region;
	Rectangle<int> graphics_region;
	Rectangle<int> ref_meter_graphics_region;
	Rectangle<int> system_meter_graphics_region;
	Rectangle<int> ref_meter_graphic_background;
	Rectangle<int> ref_meter_graphic_mask;
	Rectangle<int> system_meter_graphic_background;
	Rectangle<int> system_meter_graphic_mask;
	Rectangle<int> numeric_display_region;
	Rectangle<int> ref_numeric_display_region;
	Rectangle<int> system_numeric_display_region;

	double raw_ref_input_level = 0.0;
	
	double current_ref_input_level_dBFS = -96.0;

	double display_ref_input_level_dBFS = -96.0;

	double raw_system_input_level = 0.0;

	double current_system_input_level_dBFS = -96.0;

	double display_system_input_level_dBFS = -96.0;

	double meter_rate_of_change = 0.1;

	bool repaint_active = false;

	level_meters()
	{
	}

	~level_meters()
	{
	}

	void paint(Graphics& g) override
	{

		repaint_active = true;
		
		calc_input_levels();
		
		update_display_levels();

		g.fillAll(Colour(30, 30, 30));

		g.setColour(Colours::white);

		g.setFont(ref_meter_text_region.getHeight()*0.8);

		g.drawFittedText("Reference Input Level", ref_meter_text_region, Justification::centred, 1);

		g.drawFittedText("System Input Level", system_meter_text_region, Justification::centred, 1);

		std::string display_ref_input_level_str = std::to_string(display_ref_input_level_dBFS);

		std::string display_system_input_level_str = std::to_string(display_system_input_level_dBFS);

		display_ref_input_level_str = display_ref_input_level_str.substr(0, 5);

		display_system_input_level_str = display_system_input_level_str.substr(0, 5);

		g.drawFittedText(display_ref_input_level_str, ref_numeric_display_region, Justification::centred, 1);

		g.drawFittedText(display_system_input_level_str, system_numeric_display_region, Justification::centred, 1);

		//Two ways to draw these meters are below. Will investigate which is less expensive

		ref_meter_graphic_mask = ref_meter_graphic_background.translated(ref_meter_graphic_background.getWidth() *
			dbFS_to_mask_offset_percent(display_ref_input_level_dBFS),
			0.0);

		system_meter_graphic_mask.setX(
			
			system_meter_graphic_background.getX()
			
			+ system_meter_graphic_background.getWidth() * dbFS_to_mask_offset_percent(display_system_input_level_dBFS));

		g.setColour(Colour(66, 162, 200));

		g.fillRect(ref_meter_graphic_background);
		g.fillRect(system_meter_graphic_background);

		g.setColour(Colour(38, 50, 56));

		g.saveState();

		g.reduceClipRegion(ref_meter_graphic_background);

		g.fillRect(ref_meter_graphic_mask);

		g.restoreState();

		g.saveState();

		g.reduceClipRegion(system_meter_graphic_background);

		g.fillRect(system_meter_graphic_mask);

		g.restoreState();

		repaint_active = false;

	}

	void resized() override
	{

		component_outline = getLocalBounds();

		text_region = component_outline.removeFromLeft(getLocalBounds().getWidth()*0.15);

		ref_meter_text_region = text_region.removeFromTop(text_region.getHeight()*0.5);

		system_meter_text_region = text_region;

		graphics_region = component_outline.removeFromLeft(getLocalBounds().getWidth()*0.80);

		ref_meter_graphics_region = graphics_region.removeFromTop(graphics_region.getHeight()*0.5);

		system_meter_graphics_region = graphics_region;

		ref_meter_graphic_background = ref_meter_graphics_region.reduced(ref_meter_graphics_region.getWidth()*0.01, 
			
			ref_meter_graphics_region.getHeight()*0.25);

		system_meter_graphic_background = system_meter_graphics_region.reduced(system_meter_graphics_region.getWidth()*0.01, 
			
			system_meter_graphics_region.getHeight()*0.25);

		ref_meter_graphic_mask = ref_meter_graphic_background;

		system_meter_graphic_mask = system_meter_graphic_background;

		numeric_display_region = component_outline;

		ref_numeric_display_region = numeric_display_region.removeFromTop(numeric_display_region.getHeight()*0.5);

		system_numeric_display_region = numeric_display_region;

	}

	double dbFS_to_mask_offset_percent(double dBFS_level) { //this assumes that -96 dbFS is 0% on the meter

		return (100 + (dBFS_level * (1 / 0.96))) / 100;

	}

	void calc_input_levels() {

		current_ref_input_level_dBFS = Decibels::gainToDecibels(raw_ref_input_level, -96.0);

		current_system_input_level_dBFS = Decibels::gainToDecibels(raw_system_input_level, -96.0);

	}

	void update_display_levels() {

		if (display_ref_input_level_dBFS > current_ref_input_level_dBFS) {

			display_ref_input_level_dBFS = display_ref_input_level_dBFS - 
				
				(abs(display_ref_input_level_dBFS - current_ref_input_level_dBFS)*meter_rate_of_change);

		}

		if (display_ref_input_level_dBFS < current_ref_input_level_dBFS) {

			display_ref_input_level_dBFS = current_ref_input_level_dBFS;

		}

		if (display_system_input_level_dBFS > current_system_input_level_dBFS) {

			display_system_input_level_dBFS = display_system_input_level_dBFS - 
				
				(abs(display_system_input_level_dBFS - current_system_input_level_dBFS)*meter_rate_of_change);

		}

		if (display_system_input_level_dBFS < current_system_input_level_dBFS) {

			display_system_input_level_dBFS = current_system_input_level_dBFS;

		}

	}

	void try_repaint()

	{

		if (repaint_active == false) { //don't try to call repaint again if there's already a repaint cycle in progress

			repaint();

		}

	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(level_meters)

};
