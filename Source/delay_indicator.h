/*
  ==============================================================================

    delay_indicator.h
    Created: 23 Sep 2017 11:37:56am
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "constants.h"
#include <string>

//==============================================================================
/*
*/
class delay_indicator    : public Component, public constants, public Button::Listener, public ActionBroadcaster
{
public:
	
	int delay_in_samples{ 0 };
	float* ir_peak_msec_ptr;

    delay_indicator()
    {
		addAndMakeVisible(increase_1000_button);
		addAndMakeVisible(decrease_1000_button);
		addAndMakeVisible(increase_100_button);
		addAndMakeVisible(decrease_100_button);
		addAndMakeVisible(increase_10_button);
		addAndMakeVisible(decrease_10_button);
		addAndMakeVisible(increase_1_button);
		addAndMakeVisible(decrease_1_button);
		addAndMakeVisible(reset_to_0_button);
		addAndMakeVisible(change_1000_label);
		addAndMakeVisible(change_100_label);
		addAndMakeVisible(change_10_label);
		addAndMakeVisible(change_1_label);

		addAndMakeVisible(delay_samples_label);
		addAndMakeVisible(delay_msec_label);

		addAndMakeVisible(control_area_label);

		addAndMakeVisible(manual_delay_entry_box);
		addAndMakeVisible(set_delay_manual_button);
		addAndMakeVisible(set_delay_auto_button);

		reset_to_0_button.setButtonText("Reset to 0");
		reset_to_0_button.setColour(reset_to_0_button.buttonColourId, Colours::transparentBlack);

		delay_samples_label.setColour(delay_samples_label.backgroundColourId, Colours::black);
		delay_samples_label.setJustificationType(Justification::centred);
		
		delay_msec_label.setColour(delay_msec_label.backgroundColourId, Colours::black);
		delay_msec_label.setJustificationType(Justification::centred);

		control_area_label.setJustificationType(Justification::centred);
		control_area_label.setText("Reference Delay", dontSendNotification);

		manual_delay_entry_box.setColour(manual_delay_entry_box.backgroundColourId, Colours::black);
		manual_delay_entry_box.setEditable(true, false, false);
		
		set_delay_manual_button.setColour(set_delay_manual_button.buttonColourId, Colours::transparentBlack);
		set_delay_manual_button.setButtonText("S");

		set_delay_auto_button.setColour(set_delay_auto_button.buttonColourId, Colours::transparentBlack);
		set_delay_auto_button.setButtonText("A");

		change_1000_label.setText("x1000", dontSendNotification);

		change_100_label.setText("x100", dontSendNotification);

		change_10_label.setText("x10", dontSendNotification);

		change_1_label.setText("x1", dontSendNotification);

		increase_1000_button.addListener(this);
		decrease_1000_button.addListener(this);
		increase_100_button.addListener(this);
		decrease_100_button.addListener(this);
		increase_10_button.addListener(this);
		decrease_10_button.addListener(this);
		increase_1_button.addListener(this);
		decrease_1_button.addListener(this);
		reset_to_0_button.addListener(this);
		set_delay_manual_button.addListener(this);
		set_delay_auto_button.addListener(this);

    }

    ~delay_indicator()
    {
    }

    void paint (Graphics& g) override
    {
		
		selected_delay_in_milliseconds = ((delay_in_samples*1.0) / (sample_rate*1.0))*1000.0;

		std::string delay_in_milliseconds_str = std::to_string(selected_delay_in_milliseconds);
		
		if (selected_delay_in_milliseconds < 10.0) {
			delay_in_milliseconds_str = delay_in_milliseconds_str.substr(0, 4);
		}

		if (selected_delay_in_milliseconds >= 10.0 && selected_delay_in_milliseconds < 100.0) {
			delay_in_milliseconds_str = delay_in_milliseconds_str.substr(0, 5);
		}

		if (selected_delay_in_milliseconds >= 100.0) {
			delay_in_milliseconds_str = delay_in_milliseconds_str.substr(0, 6);
		}

		delay_samples_label.setText(std::to_string(delay_in_samples), dontSendNotification);
		delay_msec_label.setText(delay_in_milliseconds_str, dontSendNotification);

    }

    void resized() override
    {
		component_outline = getLocalBounds();

		meter_area_outline = component_outline.removeFromLeft(getWidth()*0.5);
		meter_area_active = meter_area_outline.reduced(padding_pix);
		meter_area_upper_label = meter_area_active.removeFromTop(meter_area_active.getHeight()*0.5);
		meter_area_lower_label = meter_area_active;

		control_area_outline = component_outline;
		control_area_active = control_area_outline.reduced(padding_pix);
		control_area_top_label = control_area_active.removeFromTop(getHeight()*0.2);
		control_area_left_column = control_area_active.removeFromLeft(control_area_active.getWidth()*0.5);
		control_area_right_column = control_area_active;
		
		int control_area_left_column_height = control_area_left_column.getHeight();
		
		control_area_left_column_row_1 = control_area_left_column.removeFromTop(control_area_left_column_height*0.25);
		control_area_left_column_row_2 = control_area_left_column.removeFromTop(control_area_left_column_height*0.25);
		control_area_left_column_row_3 = control_area_left_column.removeFromTop(control_area_left_column_height*0.25);
		control_area_left_column_row_4 = control_area_left_column;

		change_1000_label_area = control_area_left_column_row_1.removeFromLeft(control_area_left_column_row_1.getWidth()*0.5);
		increase_1000_button_area = control_area_left_column_row_1.removeFromLeft(control_area_left_column_row_1.getWidth()*0.5);
		decrease_1000_button_area = control_area_left_column_row_1;

		change_100_label_area = control_area_left_column_row_2.removeFromLeft(control_area_left_column_row_2.getWidth()*0.5);
		increase_100_button_area = control_area_left_column_row_2.removeFromLeft(control_area_left_column_row_2.getWidth()*0.5);
		decrease_100_button_area = control_area_left_column_row_2;

		change_10_label_area = control_area_left_column_row_3.removeFromLeft(control_area_left_column_row_3.getWidth()*0.5);
		increase_10_button_area = control_area_left_column_row_3.removeFromLeft(control_area_left_column_row_3.getWidth()*0.5);
		decrease_10_button_area = control_area_left_column_row_3;

		change_1_label_area = control_area_left_column_row_4.removeFromLeft(control_area_left_column_row_4.getWidth()*0.5);
		increase_1_button_area = control_area_left_column_row_4.removeFromLeft(control_area_left_column_row_4.getWidth()*0.5);
		decrease_1_button_area = control_area_left_column_row_4;
		
		int control_area_right_column_height = control_area_right_column.getHeight();

		control_area_right_column_row_1 = control_area_right_column.removeFromTop(control_area_right_column_height / 3);
		control_area_right_column_row_2 = control_area_right_column.removeFromTop(control_area_right_column_height / 3);
		control_area_right_column_row_3_left = control_area_right_column.removeFromLeft(control_area_right_column.getWidth() * 0.5);
		control_area_right_column_row_3_right = control_area_right_column;

		delay_samples_label.setBounds(meter_area_upper_label);
		delay_msec_label.setBounds(meter_area_lower_label);
		control_area_label.setBounds(control_area_top_label);

		change_1000_label.setBounds(change_1000_label_area);
		increase_1000_button.setBounds(increase_1000_button_area.reduced(increase_1000_button_area.getHeight()*0.25));
		decrease_1000_button.setBounds(decrease_1000_button_area.reduced(decrease_1000_button_area.getHeight()*0.25));

		change_100_label.setBounds(change_100_label_area);
		increase_100_button.setBounds(increase_100_button_area.reduced(increase_100_button_area.getHeight()*0.25));
		decrease_100_button.setBounds(decrease_100_button_area.reduced(decrease_100_button_area.getHeight()*0.25));

		change_10_label.setBounds(change_10_label_area);
		increase_10_button.setBounds(increase_10_button_area.reduced(increase_10_button_area.getHeight()*0.25));
		decrease_10_button.setBounds(decrease_10_button_area.reduced(decrease_10_button_area.getHeight()*0.25));

		change_1_label.setBounds(change_1_label_area);
		increase_1_button.setBounds(increase_1_button_area.reduced(increase_1_button_area.getHeight()*0.25));
		decrease_1_button.setBounds(decrease_1_button_area.reduced(decrease_1_button_area.getHeight()*0.25));

		reset_to_0_button.setBounds(control_area_right_column_row_1.reduced(padding_pix));
		manual_delay_entry_box.setBounds(control_area_right_column_row_2.reduced(padding_pix));
		set_delay_manual_button.setBounds(control_area_right_column_row_3_left.reduced(padding_pix));
		set_delay_auto_button.setBounds(control_area_right_column_row_3_right.reduced(padding_pix));
		delay_samples_label.setFont(delay_samples_label.getHeight()*0.9);
		delay_msec_label.setFont(delay_msec_label.getHeight()*0.9);
		control_area_label.setFont(control_area_label.getHeight()*0.9);

    }

	void buttonClicked(Button* button) override {

		if (button == &increase_1000_button)
		{
			adjust_delay(1000);
		}

		if (button == &decrease_1000_button)
		{
			adjust_delay(-1000);
		}

		if (button == &increase_100_button)
		{
			adjust_delay(100);
		}

		if (button == &decrease_100_button)
		{
			adjust_delay(-100);
		}

		if (button == &increase_10_button)
		{
			adjust_delay(10);
		}

		if (button == &decrease_10_button)
		{
			adjust_delay(-10);
		}

		if (button == &increase_1_button)
		{
			adjust_delay(1);
		}

		if (button == &decrease_1_button)
		{
			adjust_delay(-1);
		}

		if (button == &reset_to_0_button)
		{
			delay_in_samples = 0;
		}

		if (button == &set_delay_manual_button)
		{
			int new_delay_samples = round(manual_delay_entry_box.getText(true).getFloatValue() * (constants::sample_rate / 1000.0));

			adjust_delay(new_delay_samples - delay_in_samples);
		}

		if (button == &set_delay_auto_button)
		{
			int ir_peak_samples = round(*ir_peak_msec_ptr * (constants::sample_rate / 1000.0));

			adjust_delay(ir_peak_samples - delay_in_samples);
		}

		sendActionMessage("cmd_update_supervisor");

		repaint();

	}
	
private:
	
	float selected_delay_in_milliseconds{ 0.0 };

	int padding_pix{ 5 };

	Rectangle<int> component_outline;

	Rectangle<int> meter_area_outline;
	Rectangle<int> meter_area_active;
	Rectangle<int> meter_area_upper_label;
	Rectangle<int> meter_area_lower_label;
	
	Rectangle<int> control_area_outline;
	Rectangle<int> control_area_active;
	Rectangle<int> control_area_top_label;
	Rectangle<int> control_area_left_column;
	Rectangle<int> control_area_left_column_row_1;
	Rectangle<int> control_area_left_column_row_2;
	Rectangle<int> control_area_left_column_row_3;
	Rectangle<int> control_area_left_column_row_4;
	Rectangle<int> control_area_right_column;
	Rectangle<int> control_area_right_column_row_1;
	Rectangle<int> control_area_right_column_row_2;
	Rectangle<int> control_area_right_column_row_3_left;
	Rectangle<int> control_area_right_column_row_3_right;

	Rectangle<int> change_1000_label_area;
	Rectangle<int> increase_1000_button_area;
	Rectangle<int> decrease_1000_button_area;

	Rectangle<int> change_100_label_area;
	Rectangle<int> increase_100_button_area;
	Rectangle<int> decrease_100_button_area;

	Rectangle<int> change_10_label_area;
	Rectangle<int> increase_10_button_area;
	Rectangle<int> decrease_10_button_area;

	Rectangle<int> change_1_label_area;
	Rectangle<int> increase_1_button_area;
	Rectangle<int> decrease_1_button_area;
	
	ArrowButton increase_1000_button{"increase_1000", 0.75, Colours::white};
	ArrowButton decrease_1000_button{ "decrease_1000", 0.25, Colours::white };
	ArrowButton increase_100_button{ "increase_100", 0.75, Colours::white };
	ArrowButton decrease_100_button{ "decrease_100", 0.25, Colours::white };
	ArrowButton increase_10_button{ "increase_10", 0.75, Colours::white };
	ArrowButton decrease_10_button{ "decrease_10", 0.25, Colours::white };
	ArrowButton increase_1_button{ "increase_1", 0.75, Colours::white };
	ArrowButton decrease_1_button{ "decrease_1", 0.25, Colours::white };

	TextButton reset_to_0_button;
	TextButton set_delay_manual_button;
	TextButton set_delay_auto_button;

	Label change_1000_label;
	Label change_100_label;
	Label change_10_label;
	Label change_1_label;
	Label delay_samples_label;
	Label delay_msec_label;
	Label control_area_label;
	Label manual_delay_entry_box;
	
	void adjust_delay(int delay_change) {

		int requested_delay = delay_in_samples;

		requested_delay = requested_delay + delay_change;

		if (requested_delay > max_delay_samples) {

			requested_delay = max_delay_samples;

		}

		if (requested_delay < 0) {

			requested_delay = 0;

		}

		delay_in_samples = requested_delay;

	}
		
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (delay_indicator)
};
