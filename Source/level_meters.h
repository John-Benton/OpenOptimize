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
	Rectangle<int> ref_meter_graphic_rms_level;
	Rectangle<int> system_meter_graphic_background;
	Rectangle<int> system_meter_graphic_rms_level;
	Rectangle<int> numeric_display_region;
	Rectangle<int> ref_numeric_display_region;
	Rectangle<int> system_numeric_display_region;

	double ref_rms_input_level_linear = 0.0;
	double ref_peak_input_level_linear = 0.0;
	double current_ref_rms_input_level_dBFS = -96.0;
	double current_ref_peak_input_level_dBFS = -96.0;

	double system_rms_input_level_linear = 0.0;
	double system_peak_input_level_linear = 0.0;
	double current_system_rms_input_level_dBFS = -96.0;
	double current_system_peak_input_level_dBFS = -96.0;

	level_meters()
	{
	}

	~level_meters()
	{
	}

	void paint(Graphics& g) override
	{

		calc_input_levels();

		g.fillAll(Colour(30, 30, 30));

		g.setColour(Colours::white);

		g.setFont(ref_meter_text_region.getHeight()*0.8);

		g.drawFittedText("Reference Input Level", ref_meter_text_region, Justification::centred, 1);

		g.drawFittedText("System Input Level", system_meter_text_region, Justification::centred, 1);

		std::string display_ref_input_level_str = std::to_string(current_ref_rms_input_level_dBFS);

		std::string display_system_input_level_str = std::to_string(current_system_rms_input_level_dBFS);

		display_ref_input_level_str = display_ref_input_level_str.substr(0, 5);

		display_system_input_level_str = display_system_input_level_str.substr(0, 5);

		g.drawFittedText(display_ref_input_level_str, ref_numeric_display_region, Justification::centred, 1);

		g.drawFittedText(display_system_input_level_str, system_numeric_display_region, Justification::centred, 1);

		g.setColour(Colour(38, 50, 56));

		g.fillRect(ref_meter_graphic_background);
		g.fillRect(system_meter_graphic_background);

		g.setColour(Colour(66, 162, 200));
		
		double ref_rms_level_proportion = 1 - (current_ref_rms_input_level_dBFS / -96.0);
		ref_meter_graphic_rms_level = ref_meter_graphic_background.withWidth(ref_meter_graphic_background.getWidth()*ref_rms_level_proportion);
		g.fillRect(ref_meter_graphic_rms_level);

		double system_rms_level_proportion = 1 - (current_system_rms_input_level_dBFS / -96.0);
		system_meter_graphic_rms_level = system_meter_graphic_background.withWidth(system_meter_graphic_background.getWidth()*system_rms_level_proportion);
		g.fillRect(system_meter_graphic_rms_level);
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

		numeric_display_region = component_outline;

		ref_numeric_display_region = numeric_display_region.removeFromTop(numeric_display_region.getHeight()*0.5);

		system_numeric_display_region = numeric_display_region;

	}

	double dbFS_to_mask_offset_percent(double dBFS_level) { //this assumes that -96 dbFS is 0% on the meter

		return (100 + (dBFS_level * (1 / 0.96))) / 100;

	}

	void calc_input_levels() {

		current_ref_rms_input_level_dBFS = Decibels::gainToDecibels(ref_rms_input_level_linear, -96.0);
		current_ref_peak_input_level_dBFS = Decibels::gainToDecibels(ref_peak_input_level_linear, -96.0);

		current_system_rms_input_level_dBFS = Decibels::gainToDecibels(system_rms_input_level_linear, -96.0);
		current_system_peak_input_level_dBFS = Decibels::gainToDecibels(system_peak_input_level_linear, -96.0);

	}
	
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(level_meters)

};
