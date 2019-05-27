/*
  ==============================================================================

    spl_meter.h
    Created: 23 Sep 2017 11:37:43am
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "spl_cal.h"

//==============================================================================
/*
*/
class spl_meter    : public Component, public Button::Listener
{
public:

	int spl_meter_weighting = 0; //0 is Z, 1 is C, 2 is A,
	
	double raw_input_level = 0.0;

	double raw_spl_dB = 0.0;

	double calibrated_spl_dB = 0.0;

	double display_spl_dB = 0.0;

	const double dBFS_offset = 0.0;

	double spl_calibration_offset = 0.0;

	const double meter_snap_range = 0.1;

	const double spl_meter_fast_rate_of_change = 0.10;

	const double spl_meter_slow_rate_of_change = 0.01;

	double spl_meter_rate_of_change;

	Rectangle<int> component_bounds;
	Rectangle<int> left_column_row_1;
	Rectangle<int> left_column_row_1_active;
	Rectangle<int> right_column_row_1;
	Rectangle<int> right_column_row_1_active;
	Rectangle<int> right_column_row_2;
	Rectangle<int> right_column_row_2_active;
	Rectangle<int> right_column_row_3;
	Rectangle<int> right_column_row_3_active;
	Rectangle<int> right_column_row_4;
	Rectangle<int> right_column_row_4_active;

	int horizontal_padding_pixels = 5;

	int vertical_padding_pixels = 5;

	spl_meter()
    {
		/*addAndMakeVisible(z_weight_button);
		addAndMakeVisible(c_weight_button);
		addAndMakeVisible(a_weight_button);*/

		addAndMakeVisible(cal_spl_button);
		
		addAndMakeVisible(slow_time_button);
		addAndMakeVisible(fast_time_button);

		addAndMakeVisible(spl_meter_label);

		addAndMakeVisible(spl_meter_reading_text);

		addAndMakeVisible(spl_uncal_label);

		addAndMakeVisible(spl_weight_z_label);
		
		/*z_weight_button.setButtonText("Z Weighting");
		c_weight_button.setButtonText("C Weighting");
		a_weight_button.setButtonText("A Weighting");*/
		
		cal_spl_button.setButtonText("Calibrate");
		
		slow_time_button.setButtonText("Slower");
		fast_time_button.setButtonText("Faster");

		spl_meter_label.setText("SPL", dontSendNotification);
		
		spl_uncal_label.setText("UNCALIBRATED", dontSendNotification);

		spl_weight_z_label.setText("dB(Z)", dontSendNotification);

		spl_meter_reading_text.setJustificationType(Justification::centred);
		spl_meter_label.setJustificationType(Justification::centred);

		spl_uncal_label.setJustificationType(Justification::centred);

		spl_weight_z_label.setJustificationType(Justification::centred);

		spl_meter_reading_text.setColour(Label::backgroundColourId, Colours::black);

		spl_uncal_label.setColour(Label::textColourId, Colours::red);
				
		/*z_weight_button.addListener(this);
		c_weight_button.addListener(this);
		a_weight_button.addListener(this);*/

		cal_spl_button.addListener(this);
		
		slow_time_button.addListener(this);
		fast_time_button.addListener(this);

		/*z_weight_button.setColour(z_weight_button.buttonOnColourId, Colours::green);
		c_weight_button.setColour(c_weight_button.buttonOnColourId, Colours::blue);
		a_weight_button.setColour(a_weight_button.buttonOnColourId, Colours::red);*/

		cal_spl_button.setColour(cal_spl_button.buttonColourId, Colours::transparentBlack);
		cal_spl_button.setColour(cal_spl_button.buttonOnColourId, Colours::darkblue);
		
		slow_time_button.setColour(slow_time_button.buttonColourId, Colours::transparentBlack);
		slow_time_button.setColour(slow_time_button.buttonOnColourId, Colours::darkred);

		fast_time_button.setColour(fast_time_button.buttonColourId, Colours::transparentBlack);
		fast_time_button.setColour(fast_time_button.buttonOnColourId, Colours::darkgreen);

		fast_time_button.setToggleState(1, dontSendNotification);

    }

    ~spl_meter()
    {
		/*z_weight_button.removeListener(this);
		c_weight_button.removeListener(this);
		a_weight_button.removeListener(this);*/
		cal_spl_button.removeListener(this);
		slow_time_button.removeListener(this);
		fast_time_button.removeListener(this);
    }

    void paint (Graphics& g) override
    {

		if (fast_time_button.getToggleState() == true) {

			spl_meter_rate_of_change = spl_meter_fast_rate_of_change;

		}

		else {

			spl_meter_rate_of_change = spl_meter_slow_rate_of_change;

		}
		
		calc_uncalibrated_spl();

		calc_calibrated_spl();
		
		update_display_spl();

		spl_display_value_string = String(display_spl_dB);

		if (display_spl_dB < 0.0) {
			spl_display_value_string = spl_display_value_string.substring(0, 5);
		}
		
		if (display_spl_dB < 100.0 && display_spl_dB >= 0.0) {
			spl_display_value_string = spl_display_value_string.substring(0, 4);
		}

		if (display_spl_dB >= 100.0) {
			spl_display_value_string = spl_display_value_string.substring(0, 3);
		}

		spl_meter_reading_text.setText(spl_display_value_string, dontSendNotification);
		
		if (spl_calibration_offset != 0.0) {

			cal_spl_button.setToggleState(1, dontSendNotification);
			spl_uncal_label.setVisible(false);

		}

		else {

			cal_spl_button.setToggleState(0, dontSendNotification);
			spl_uncal_label.setVisible(true);

		}

		g.setColour(Colours::black);
		g.drawRect(getLocalBounds(), 1.0);

    }

    void resized() override
    {
				
		component_bounds = getLocalBounds();

		int component_height = component_bounds.getHeight();
		int component_width = component_bounds.getWidth();

		left_column_row_1 = component_bounds.removeFromLeft(component_width*0.75);
				
		right_column_row_1 = component_bounds.removeFromTop(component_height*0.25);
		right_column_row_2 = component_bounds.removeFromTop(component_height*0.25);
		right_column_row_3 = component_bounds.removeFromTop(component_height*0.25);
		right_column_row_4 = component_bounds;

		left_column_row_1_active = left_column_row_1.reduced(horizontal_padding_pixels, vertical_padding_pixels);
		right_column_row_1_active = right_column_row_1.reduced(horizontal_padding_pixels, vertical_padding_pixels);
		right_column_row_2_active = right_column_row_2.reduced(horizontal_padding_pixels, vertical_padding_pixels);
		right_column_row_3_active = right_column_row_3.reduced(horizontal_padding_pixels, vertical_padding_pixels);
		right_column_row_4_active = right_column_row_4.reduced(horizontal_padding_pixels, vertical_padding_pixels);

		spl_meter_reading_text.setBounds(left_column_row_1_active);

		spl_uncal_label.setBounds(
			left_column_row_1_active.getX(),
			left_column_row_1_active.getY(),
			left_column_row_1_active.getWidth(),
			left_column_row_1_active.getHeight() / 5);

		spl_weight_z_label.setBounds(
			left_column_row_1_active.getX(),
			left_column_row_1_active.getY() + left_column_row_1_active.getHeight() * (4.0 / 5.0),
			left_column_row_1_active.getWidth() / 3,
			left_column_row_1_active.getHeight() / 5
		);
		
		spl_meter_label.setBounds(right_column_row_1_active);
		cal_spl_button.setBounds(right_column_row_2_active);
		slow_time_button.setBounds(right_column_row_3_active);
		fast_time_button.setBounds(right_column_row_4_active);

		spl_meter_reading_text.setFont(spl_meter_reading_text.getHeight()*0.75);
		spl_meter_label.setFont(spl_meter_label.getHeight());

		spl_uncal_label.setFont(spl_uncal_label.getHeight()*0.75);
		spl_weight_z_label.setFont(spl_weight_z_label.getHeight()*0.75);
		
    }

	void buttonClicked(Button* button) override {

		/*if (button == &z_weight_button)
		{
			spl_meter_weighting = 0;
			z_weight_button.setToggleState(1, dontSendNotification);
			c_weight_button.setToggleState(0, dontSendNotification);
			a_weight_button.setToggleState(0, dontSendNotification);
		}

		if (button == &c_weight_button)
		{
			spl_meter_weighting = 1;
			z_weight_button.setToggleState(0, dontSendNotification);
			c_weight_button.setToggleState(1, dontSendNotification);
			a_weight_button.setToggleState(0, dontSendNotification);
		}

		if (button == &a_weight_button)
		{
			spl_meter_weighting = 2;
			z_weight_button.setToggleState(0, dontSendNotification);
			c_weight_button.setToggleState(0, dontSendNotification);
			a_weight_button.setToggleState(1, dontSendNotification);
		}*/

		if (button == &cal_spl_button)
		{
			open_spl_cal_window();
		}

		if (button == &slow_time_button)
		{
			spl_meter_rate_of_change = 0.01;
			slow_time_button.setToggleState(1, dontSendNotification);
			fast_time_button.setToggleState(0, dontSendNotification);
		}

		if (button == &fast_time_button)
		{
			spl_meter_rate_of_change = 0.08;
			slow_time_button.setToggleState(0, dontSendNotification);
			fast_time_button.setToggleState(1, dontSendNotification);
		}

	}

	void calc_uncalibrated_spl() {

		raw_spl_dB = Decibels::gainToDecibels(raw_input_level, -96.0) + dBFS_offset;

	}

	void calc_calibrated_spl() {

		calibrated_spl_dB = raw_spl_dB + spl_calibration_offset;

	}

	void update_display_spl() {
		
		if ((display_spl_dB - calibrated_spl_dB) > meter_snap_range) {

			display_spl_dB = display_spl_dB - (abs(display_spl_dB - calibrated_spl_dB)*spl_meter_rate_of_change);

		}

		if ((display_spl_dB - calibrated_spl_dB) < -meter_snap_range) {

			display_spl_dB = display_spl_dB + (abs(display_spl_dB - calibrated_spl_dB)*spl_meter_rate_of_change);

		}

		if (abs(display_spl_dB - calibrated_spl_dB) <= meter_snap_range) {

			display_spl_dB = calibrated_spl_dB;

		}
	
	}

	void open_spl_cal_window() {

		spl_calibration_component.set_offset_locations(&spl_calibration_offset, &raw_spl_dB);

		OptionalScopedPointer<Component> spl_calibration_component_ptr{ &spl_calibration_component, 0 };

		spl_calibration_window.dialogTitle = "SPL Calibration";

		spl_calibration_window.dialogBackgroundColour = Colours::darkgrey;

		spl_calibration_window.content = spl_calibration_component_ptr;

		spl_calibration_window.componentToCentreAround = NULL;

		spl_calibration_window.escapeKeyTriggersCloseButton = true;

		spl_calibration_window.useNativeTitleBar = true;

		spl_calibration_window.useBottomRightCornerResizer = false;

		spl_calibration_component.setSize(640, 480);

		spl_calibration_window.launchAsync();
		
	}

private:

	/*TextButton z_weight_button;
	TextButton c_weight_button;
	TextButton a_weight_button;*/
	TextButton cal_spl_button;
	TextButton slow_time_button;
	TextButton fast_time_button;
	DialogWindow::LaunchOptions spl_calibration_window;
	spl_cal spl_calibration_component;
	Label spl_meter_reading_text;
	Label spl_meter_label;
	Label spl_uncal_label;
	Label spl_weight_z_label;
	String spl_display_value_string;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (spl_meter)

};
