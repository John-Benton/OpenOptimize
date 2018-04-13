/*
  ==============================================================================

    spl_meter.h
    Created: 23 Sep 2017 11:37:43am
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <string>
#include <deque>
#include <numeric>
#include <fstream>
#include <vector>

//==============================================================================
/*
*/
class spl_meter    : public Component, public Button::Listener
{
public:

	int spl_meter_weighting = 0; //0 is Z, 1 is C, 2 is A,
	
	double raw_input_level = 0.0;

	double unweighted_spl = 0.0;

	double weighted_spl = 0.0;

	double display_spl = 0.0;

	const double dBFS_offset = 120;

	const double meter_snap_range = 0.1;

	const double spl_meter_fast_rate_of_change = 0.05;

	const double spl_meter_slow_rate_of_change = 0.25;

	double spl_meter_rate_of_change;
		
	Rectangle<int> spl_meter_remaining_outline;
	Rectangle<int> spl_meter_display_area;
	Rectangle<int> spl_meter_display_black_area;
	Rectangle<int> spl_meter_controls_area;
	Rectangle<int> spl_meter_controls_division1;
	Rectangle<int> spl_meter_controls_division2;
	Rectangle<int> spl_meter_controls_division3;
	Rectangle<int> spl_meter_controls_division4;

	bool repaint_active = false;

	spl_meter()
    {
		/*addAndMakeVisible(z_weight_button);
		addAndMakeVisible(c_weight_button);
		addAndMakeVisible(a_weight_button);
		addAndMakeVisible(cal_spl_button);*/
		
		addAndMakeVisible(slow_time_button);
		addAndMakeVisible(fast_time_button);
		
		/*z_weight_button.setButtonText("Z Weighting");
		c_weight_button.setButtonText("C Weighting");
		a_weight_button.setButtonText("A Weighting");
		cal_spl_button.setButtonText("Calibrate");*/
		
		slow_time_button.setButtonText("Slower");
		fast_time_button.setButtonText("Faster");

		/*z_weight_button.addListener(this);
		c_weight_button.addListener(this);
		a_weight_button.addListener(this);
		cal_spl_button.addListener(this);*/
		
		slow_time_button.addListener(this);
		fast_time_button.addListener(this);

		/*z_weight_button.setColour(z_weight_button.buttonOnColourId, Colours::green);
		c_weight_button.setColour(c_weight_button.buttonOnColourId, Colours::blue);
		a_weight_button.setColour(a_weight_button.buttonOnColourId, Colours::red);
		cal_spl_button.setColour(cal_spl_button.buttonColourId, Colour(66, 162, 200));*/
		
		slow_time_button.setColour(a_weight_button.buttonColourId, Colours::transparentBlack);
		slow_time_button.setColour(a_weight_button.buttonOnColourId, Colours::darkred);

		fast_time_button.setColour(a_weight_button.buttonColourId, Colours::transparentBlack);
		fast_time_button.setColour(a_weight_button.buttonOnColourId, Colours::darkgreen);

		fast_time_button.setToggleState(1, dontSendNotification);

    }

    ~spl_meter()
    {
		z_weight_button.removeListener(this);
		c_weight_button.removeListener(this);
		a_weight_button.removeListener(this);
		cal_spl_button.removeListener(this);
		slow_time_button.removeListener(this);
		fast_time_button.removeListener(this);
    }

    void paint (Graphics& g) override
    {
		
		repaint_active = true;

		if (fast_time_button.getToggleState() == true) {

			spl_meter_rate_of_change = 0.05;

		}

		else {

			spl_meter_rate_of_change = 0.25;

		}
		
		calc_unweighted_spl();

		calc_weighted_spl();
		
		update_display_spl();
				
		g.setColour(Colours::black);
		
		g.fillRect(spl_meter_display_black_area);

		g.setColour(Colours::white);

		g.setFont(spl_meter_controls_division1.getHeight()*0.75);

		g.drawFittedText("SPL",
			spl_meter_controls_division1.getX(),
			spl_meter_controls_division1.getY(),
			spl_meter_controls_division1.getWidth(),
			spl_meter_controls_division1.getHeight(),
			Justification::centred, 0, 1.0f);

		std::string display_spl_str = std::to_string(display_spl);

		std::string display_spl_str_short;

		if (weighted_spl < 100.0) {
			display_spl_str_short = display_spl_str.substr(0, 4);
		}

		if (weighted_spl >= 100.0) {
			display_spl_str_short = display_spl_str.substr(0, 3);
		}

		g.setFont(spl_meter_display_black_area.getHeight()*0.60);
		g.drawFittedText(display_spl_str_short, spl_meter_display_black_area, Justification::centred, 0, 1.0f);

		repaint_active = false;

    }

    void resized() override
    {
		
		spl_meter_remaining_outline = getLocalBounds();
		spl_meter_display_area = spl_meter_remaining_outline.removeFromLeft(getWidth()*0.50);
		spl_meter_display_black_area = spl_meter_display_area;
		spl_meter_display_black_area.reduce((spl_meter_display_area.getWidth()*0.05), (spl_meter_display_area.getHeight()*0.1));

		spl_meter_controls_area = spl_meter_remaining_outline;
		spl_meter_controls_division1 = spl_meter_controls_area.removeFromTop(getHeight()*0.2);
		spl_meter_controls_area.removeFromTop(getHeight()*0.05);
		spl_meter_controls_division2 = spl_meter_controls_area.removeFromTop(getHeight()*0.175);
		spl_meter_controls_area.removeFromTop(getHeight()*0.05);
		spl_meter_controls_division3 = spl_meter_controls_area.removeFromTop(getHeight()*0.175);
		spl_meter_controls_area.removeFromTop(getHeight()*0.05);
		spl_meter_controls_division4 = spl_meter_controls_area.removeFromTop(getHeight()*0.175);
		
		int w = getWidth();
		int h = getHeight();

		z_weight_button.setBounds(
			spl_meter_controls_division2.getX(),
			spl_meter_controls_division2.getY(),
			spl_meter_controls_division2.getWidth()*0.45,
			spl_meter_controls_division2.getHeight());

		c_weight_button.setBounds(
			spl_meter_controls_division3.getX(),
			spl_meter_controls_division3.getY(),
			spl_meter_controls_division3.getWidth()*0.45,
			spl_meter_controls_division3.getHeight());

		a_weight_button.setBounds(
			spl_meter_controls_division4.getX(),
			spl_meter_controls_division4.getY(),
			spl_meter_controls_division4.getWidth()*0.45,
			spl_meter_controls_division4.getHeight());

		cal_spl_button.setBounds(
			spl_meter_controls_division2.getX() + spl_meter_controls_division2.getWidth()*0.55,
			spl_meter_controls_division2.getY(),
			spl_meter_controls_division2.getWidth()*0.45,
			spl_meter_controls_division2.getHeight());

		slow_time_button.setBounds(
			spl_meter_controls_division3.getX() + spl_meter_controls_division3.getWidth()*0.55,
			spl_meter_controls_division3.getY(),
			spl_meter_controls_division3.getWidth()*0.45,
			spl_meter_controls_division3.getHeight());

		fast_time_button.setBounds(
			spl_meter_controls_division4.getX() + spl_meter_controls_division4.getWidth()*0.55,
			spl_meter_controls_division4.getY(),
			spl_meter_controls_division4.getWidth()*0.45,
			spl_meter_controls_division4.getHeight());

    }

	void buttonClicked(Button* button) override {

		if (button == &z_weight_button)
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

	void calc_unweighted_spl() {

		unweighted_spl = Decibels::gainToDecibels(raw_input_level, -96.0) + dBFS_offset;

	}

	void calc_weighted_spl() {

		weighted_spl = unweighted_spl;

	}

	void update_display_spl() {
		
		if ((display_spl - weighted_spl) > meter_snap_range) {

			display_spl = display_spl - (abs(display_spl - weighted_spl)*spl_meter_rate_of_change);

		}

		if ((display_spl - weighted_spl) < -meter_snap_range) {

			display_spl = display_spl + (abs(display_spl - weighted_spl)*spl_meter_rate_of_change);

		}

		if (abs(display_spl - weighted_spl) <= meter_snap_range) {

			display_spl = weighted_spl;

		}
	
	}

	void try_repaint()
	
	{

		if (repaint_active == false) { //don't try to call repaint again if there's already a repaint cycle in progress

			repaint();

		}
		
	}

private:

	TextButton z_weight_button;
	TextButton c_weight_button;
	TextButton a_weight_button;
	TextButton cal_spl_button;
	TextButton slow_time_button;
	TextButton fast_time_button;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (spl_meter)
};
