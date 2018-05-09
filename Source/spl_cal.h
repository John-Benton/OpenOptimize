/*
  ==============================================================================

    spl_cal.h
    Created: 8 May 2018 4:06:27pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class spl_cal    : public Component, public Button::Listener
{
public:

	double* spl_offset_ptr;

	double* current_spl_ptr;
    	
	spl_cal()
    {

		addAndMakeVisible(window_instructions);
		addAndMakeVisible(meter_spl_entry_area);
		addAndMakeVisible(calibrate_button);
		addAndMakeVisible(calibrate_button_label);
		
		window_instructions.setText("Enter the current reading on your SPL meter in the black box below, then click CALIBRATE", dontSendNotification);
		window_instructions.setJustificationType(Justification::centred);

		meter_spl_entry_area.setColour(Label::backgroundColourId, Colours::black);
		meter_spl_entry_area.setEditable(true, true, false);
		meter_spl_entry_area.setJustificationType(Justification::centred);
		
		calibrate_button.addListener(this);
		calibrate_button.setButtonText("CALIBRATE");
		calibrate_button.setColour(calibrate_button.buttonColourId, Colours::transparentBlack);

		calibrate_button.setButtonText("CALIBRATE");
		
	}

    ~spl_cal()
    {
    }

    void paint (Graphics& g) override
    {
    		
		g.fillAll(Colour(30, 30, 30));
		
    }

    void resized() override
    {

		component_bounds = getLocalBounds();

		int component_height = component_bounds.getHeight();

		row1 = component_bounds.removeFromTop(component_height / 3);

		row2 = component_bounds.removeFromTop(component_height / 3);

		row3 = component_bounds;

		row1_active = row1.reduced(horizontal_padding_pixels, vertical_padding_pixels);

		row2_active = row2.reduced(horizontal_padding_pixels, vertical_padding_pixels);

		row3_active = row3.reduced(horizontal_padding_pixels, vertical_padding_pixels);

		window_instructions.setBounds(row1_active);

		meter_spl_entry_area.setBounds(row2_active);

		meter_spl_entry_area.setFont(row3_active.getHeight()*0.5);

		calibrate_button.setBounds(row3_active);

		window_instructions.setFont(row1_active.getHeight()*0.2);

    }

	void set_offset_locations(double* spl_offset_location, double* current_spl_location) {

		spl_offset_ptr = spl_offset_location;

		current_spl_ptr = current_spl_location;

	}

private:

	int horizontal_padding_pixels = 5;

	int vertical_padding_pixels = 5;

	Rectangle<int> component_bounds;
	Rectangle<int> row1;
	Rectangle<int> row1_active;
	Rectangle<int> row2;
	Rectangle<int> row2_active;
	Rectangle<int> row3;
	Rectangle<int> row3_active;

	Label window_instructions;

	Label meter_spl_entry_area;

	TextButton calibrate_button;

	Label calibrate_button_label;

	String entered_spl_value;

	void buttonClicked(Button* button) override {

		if (button == &calibrate_button){
			
			entered_spl_value = meter_spl_entry_area.getText(true);
			
			*spl_offset_ptr = entered_spl_value.getFloatValue() - *current_spl_ptr;

		}

	}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(spl_cal)

};
	