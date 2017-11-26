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
class delay_indicator    : public Component, public constants, public Button::Listener
{
public:

	Rectangle<int> delay_indicator_remaining_outline;
	Rectangle<int> delay_indicator_display_area;
	Rectangle<int> delay_indicator_display_black_area;
	Rectangle<int> delay_indicator_controls_area;
	Rectangle<int> delay_indicator_controls_division1;
	Rectangle<int> delay_indicator_controls_division2;
	Rectangle<int> delay_indicator_controls_division3;
	Rectangle<int> delay_indicator_controls_division4;
	Rectangle<int> delay_indicator_controls_division5;

	int delay_in_samples = 0;

	float delay_in_milliseconds = 0.0;

	bool repaint_active = false;

    delay_indicator()
    {
		addAndMakeVisible(increase_1000);
		addAndMakeVisible(decrease_1000);
		addAndMakeVisible(increase_100);
		addAndMakeVisible(decrease_100);
		addAndMakeVisible(increase_10);
		addAndMakeVisible(decrease_10);
		addAndMakeVisible(increase_1);
		addAndMakeVisible(decrease_1);
		addAndMakeVisible(reset_to_0);
		//addAndMakeVisible(launch_delay_finder);

		reset_to_0.setButtonText("Reset to 0");
		//launch_delay_finder.setButtonText("Delay Finder");

		increase_1000.addListener(this);
		decrease_1000.addListener(this);
		increase_100.addListener(this);
		decrease_100.addListener(this);
		increase_10.addListener(this);
		decrease_10.addListener(this);
		increase_1.addListener(this);
		decrease_1.addListener(this);
		reset_to_0.addListener(this);
		//launch_delay_finder.addListener(this);
    }

    ~delay_indicator()
    {
    }

    void paint (Graphics& g) override
    {
		repaint_active = true;
		
		delay_in_milliseconds = ((delay_in_samples*1.0) / (sample_rate*1.0))*1000.0;
		std::string delay_in_milliseconds_str = std::to_string(delay_in_milliseconds);
		
		std::string delay_in_milliseconds_str_short;
		
		if (delay_in_milliseconds < 10.0) {
			delay_in_milliseconds_str_short = delay_in_milliseconds_str.substr(0, 4);
		}

		if (delay_in_milliseconds >= 10.0 && delay_in_milliseconds < 100.0) {
			delay_in_milliseconds_str_short = delay_in_milliseconds_str.substr(0, 5);
		}

		if (delay_in_milliseconds >= 100.0) {
			delay_in_milliseconds_str_short = delay_in_milliseconds_str.substr(0, 6);
		}

		g.setColour(Colours::black);
		g.fillRect(delay_indicator_display_black_area);

		g.setColour(Colours::white);
		g.setFont(delay_indicator_display_black_area.getHeight()*0.5);
		g.drawFittedText(std::to_string(delay_in_samples), delay_indicator_display_black_area, Justification::centredTop, 0, 1.0f);
		g.drawFittedText(delay_in_milliseconds_str_short, delay_indicator_display_black_area, Justification::centredBottom, 0, 1.0f);

		g.setFont(delay_indicator_controls_division1.getHeight()*0.75);

		g.drawFittedText("Reference Delay",
			delay_indicator_controls_division1.getX(),
			delay_indicator_controls_division1.getY(),
			delay_indicator_controls_division1.getWidth(),
			delay_indicator_controls_division1.getHeight(),
			Justification::centred, 0, 1.0f);

		g.setFont(delay_indicator_controls_division2.getHeight()*0.5);

		g.drawFittedText("x1000",
			delay_indicator_controls_division2.getX(),
			delay_indicator_controls_division2.getY(),
			delay_indicator_controls_division2.getWidth()*0.25,
			delay_indicator_controls_division2.getHeight(),
			Justification::left, 0, 1.0f);

		g.drawFittedText("x100",
			delay_indicator_controls_division3.getX(),
			delay_indicator_controls_division3.getY(),
			delay_indicator_controls_division3.getWidth()*0.25,
			delay_indicator_controls_division3.getHeight(),
			Justification::left, 0, 1.0f);

		g.drawFittedText("x10",
			delay_indicator_controls_division4.getX(),
			delay_indicator_controls_division4.getY(),
			delay_indicator_controls_division4.getWidth()*0.25,
			delay_indicator_controls_division4.getHeight(),
			Justification::left, 0, 1.0f);

		g.drawFittedText("x1",
			delay_indicator_controls_division5.getX(),
			delay_indicator_controls_division5.getY(),
			delay_indicator_controls_division5.getWidth()*0.25,
			delay_indicator_controls_division5.getHeight(),
			Justification::left, 0, 1.0f);

        //g.setColour (Colours::grey);
		//g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

		repaint_active = false;
    }

    void resized() override
    {
		delay_indicator_remaining_outline = getLocalBounds();
		delay_indicator_display_area = delay_indicator_remaining_outline.removeFromLeft(getWidth()*0.50);
		delay_indicator_display_black_area = delay_indicator_display_area;
		delay_indicator_display_black_area.reduce((delay_indicator_display_area.getWidth()*0.05),(delay_indicator_display_area.getHeight()*0.1));

		delay_indicator_controls_area = delay_indicator_remaining_outline;
		delay_indicator_controls_division1 = delay_indicator_controls_area.removeFromTop(getHeight()*0.2);
		delay_indicator_controls_division2 = delay_indicator_controls_area.removeFromTop(getHeight()*0.175);
		delay_indicator_controls_division3 = delay_indicator_controls_area.removeFromTop(getHeight()*0.175);
		delay_indicator_controls_division4 = delay_indicator_controls_area.removeFromTop(getHeight()*0.175);
		delay_indicator_controls_division5 = delay_indicator_controls_area.removeFromTop(getHeight()*0.175);

		reset_to_0.setBounds(
			delay_indicator_controls_division2.getX() + delay_indicator_controls_division2.getWidth()*0.4,
			delay_indicator_controls_division2.getY() + delay_indicator_controls_division2.getHeight()*0.35,
			delay_indicator_controls_division2.getWidth() *0.5,
			delay_indicator_controls_division2.getHeight() *1.0);

		/*launch_delay_finder.setBounds(
			delay_indicator_controls_division4.getX() + delay_indicator_controls_division4.getWidth()*0.4,
			delay_indicator_controls_division4.getY() + delay_indicator_controls_division4.getHeight()*0.35,
			delay_indicator_controls_division4.getWidth() *0.5,
			delay_indicator_controls_division4.getHeight() *1.0);*/
		
		increase_1000.setBounds(
			delay_indicator_controls_division2.getX() + delay_indicator_controls_division2.getWidth()*0.2,
			delay_indicator_controls_division2.getY() + delay_indicator_controls_division2.getHeight()*0.35,
			delay_indicator_controls_division2.getHeight() *0.5,
			delay_indicator_controls_division2.getHeight() *0.5);

		decrease_1000.setBounds(
			delay_indicator_controls_division2.getX() + delay_indicator_controls_division2.getWidth()*0.275,
			delay_indicator_controls_division2.getY() + delay_indicator_controls_division2.getHeight()*0.35,
			delay_indicator_controls_division2.getHeight() *0.5,
			delay_indicator_controls_division2.getHeight() *0.5);

		increase_100.setBounds(
			delay_indicator_controls_division3.getX() + delay_indicator_controls_division3.getWidth()*0.2,
			delay_indicator_controls_division3.getY() + delay_indicator_controls_division3.getHeight()*0.35,
			delay_indicator_controls_division3.getHeight() *0.5,
			delay_indicator_controls_division3.getHeight() *0.5);

		decrease_100.setBounds(
			delay_indicator_controls_division3.getX() + delay_indicator_controls_division3.getWidth()*0.275,
			delay_indicator_controls_division3.getY() + delay_indicator_controls_division3.getHeight()*0.35,
			delay_indicator_controls_division3.getHeight() *0.5,
			delay_indicator_controls_division3.getHeight() *0.5);

		increase_10.setBounds(
			delay_indicator_controls_division4.getX() + delay_indicator_controls_division4.getWidth()*0.2,
			delay_indicator_controls_division4.getY() + delay_indicator_controls_division4.getHeight()*0.35,
			delay_indicator_controls_division4.getHeight() *0.5,
			delay_indicator_controls_division4.getHeight() *0.5);

		decrease_10.setBounds(
			delay_indicator_controls_division4.getX() + delay_indicator_controls_division4.getWidth()*0.275,
			delay_indicator_controls_division4.getY() + delay_indicator_controls_division4.getHeight()*0.35,
			delay_indicator_controls_division4.getHeight() *0.5,
			delay_indicator_controls_division4.getHeight() *0.5);

		increase_1.setBounds(
			delay_indicator_controls_division5.getX() + delay_indicator_controls_division5.getWidth()*0.2,
			delay_indicator_controls_division5.getY() + delay_indicator_controls_division5.getHeight()*0.35,
			delay_indicator_controls_division5.getHeight() *0.5,
			delay_indicator_controls_division5.getHeight() *0.5);

		decrease_1.setBounds(
			delay_indicator_controls_division5.getX() + delay_indicator_controls_division5.getWidth()*0.275,
			delay_indicator_controls_division5.getY() + delay_indicator_controls_division5.getHeight()*0.35,
			delay_indicator_controls_division5.getHeight() *0.5,
			delay_indicator_controls_division5.getHeight() *0.5);

    }

	void buttonClicked(Button* button) override {

		if (button == &increase_1000)
		{
			adjust_delay(1000);
		}

		if (button == &decrease_1000)
		{
			adjust_delay(-1000);
		}

		if (button == &increase_100)
		{
			adjust_delay(100);
		}

		if (button == &decrease_100)
		{
			adjust_delay(-100);
		}

		if (button == &increase_10)
		{
			adjust_delay(10);
		}

		if (button == &decrease_10)
		{
			adjust_delay(-10);
		}

		if (button == &increase_1)
		{
			adjust_delay(1);
		}

		if (button == &decrease_1)
		{
			adjust_delay(-1);
		}

		if (button == &reset_to_0)
		{
			delay_in_samples = 0;
		}

	}

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

	void try_repaint()

	{

		if (repaint_active == false) { //don't try to call repaint again if there's already a repaint cycle in progress

			repaint();

		}

	}

private:

	ArrowButton increase_1000{"increase_1000", 0.75, Colours::white};
	ArrowButton decrease_1000{ "decrease_1000", 0.25, Colours::white };
	ArrowButton increase_100{ "increase_100", 0.75, Colours::white };
	ArrowButton decrease_100{ "decrease_100", 0.25, Colours::white };
	ArrowButton increase_10{ "increase_10", 0.75, Colours::white };
	ArrowButton decrease_10{ "decrease_10", 0.25, Colours::white };
	ArrowButton increase_1{ "increase_1", 0.75, Colours::white };
	ArrowButton decrease_1{ "decrease_1", 0.25, Colours::white };
	TextButton reset_to_0;
	/*TextButton launch_delay_finder;*/
		
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (delay_indicator)
};
