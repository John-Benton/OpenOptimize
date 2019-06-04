/*
  ==============================================================================

    plots.h
    Created: 14 Sep 2017 10:17:15pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "constants.h"
#include <vector>
#include <string>
#include "flexplot.h"

//==============================================================================
/*
*/

class xfer_func_plot : public flexplot
{
public:

	xfer_func_plot(){}

	~xfer_func_plot(){}

	void draw_custom_overlay(Graphics& g) override
	{
		if (sample_rate_incorrect == true) {

			g.setColour(Colours::black);
			g.fillRect(plot_area);

			g.setColour(Colours::white);
			g.setFont(plot_area.getHeight()*0.05);
			g.drawFittedText(
				"SAMPLE RATE INCORRECT. PLEASE SET SAMPLE RATE TO 48000 Hz USING AUDIO INPUT SETUP WINDOW.",
				plot_area.reduced(plot_area.getWidth()*0.10),
				Justification::centred,
				2);

		}

		if (mod_line_visible == true) {

			Rectangle<int> mod_rect{
				int(plot_area.getWidth()*mod_line_plot_x_propotion + plot_area.getX()),
				plot_area.getY(),
				mod_line_width,
				plot_area.getHeight()
			};

			g.setColour(Colours::white);
			g.fillRect(mod_rect);

		}

	}

	//In the returned vector, 
	//[0] is the x_coordinate of the left edge of the plot,
	//[1] is the x_coordinate of the right edge of the plot,
	//[2] is the y_coordinate of the bottom edge of the plot,
	//[3] is the y_coordinate of the top edge of the plot
	std::vector<int> get_plot_edges_screen_pix()
	{
		
		int left_edge_x = getScreenX() + plot_area.getX();
		int top_edge_y = getScreenY() + plot_area.getY();
		int right_edge_x = left_edge_x + plot_area.getWidth();
		int bottom_edge_y = top_edge_y + plot_area.getHeight();

		return std::vector<int> {left_edge_x, right_edge_x, bottom_edge_y, top_edge_y};
	}
	
	bool sample_rate_incorrect{ true };

	bool mod_line_visible{ false };

	int mod_line_width{ 1 };
	float mod_line_plot_x_propotion{ 0.0 };

};

class plots : public Component, public constants, public Button::Listener
{
public:

	std::vector<float> current_composite_fft_bin_frequencies;
	std::vector<float> current_composite_xfer_function_mag_dB;
	std::vector<float> current_composite_xfer_function_phase_deg;
	std::vector<float> current_composite_coherence_value;
	std::vector<float> current_composite_system_spectrum_mag_dB;

	std::vector<float> loaded_composite_fft_bin_frequencies;
	std::vector<float> loaded_composite_xfer_function_mag_dB_avg_cal;
	std::vector<float> loaded_composite_xfer_function_phase_deg_avg;
	std::vector<float> loaded_composite_coherence_value;

	bool sample_rate_incorrect_warning{ false };
	
    plots()
    {
		current_composite_fft_bin_frequencies.resize(composite_fft_bins);

		current_composite_xfer_function_mag_dB.resize(composite_fft_bins);
		display_composite_xfer_function_mag_dB.resize(composite_fft_bins);

		current_composite_xfer_function_phase_deg.resize(composite_fft_bins);
		display_composite_xfer_function_phase_deg.resize(composite_fft_bins);

		current_composite_coherence_value.resize(composite_fft_bins);
		display_composite_coherence_value.resize(composite_fft_bins);

		current_composite_system_spectrum_mag_dB.resize(composite_fft_bins);
		display_composite_system_spectrum_mag_dB.resize(composite_fft_bins);

		loaded_composite_fft_bin_frequencies.resize(composite_fft_bins);
		loaded_composite_xfer_function_mag_dB_avg_cal.resize(composite_fft_bins);
		loaded_composite_xfer_function_phase_deg_avg.resize(composite_fft_bins);
		loaded_composite_coherence_value.resize(composite_fft_bins);

		addAndMakeVisible(magnitude_visible_button);
		magnitude_visible_button.setButtonText("M");
		magnitude_visible_button.setColour(magnitude_visible_button.buttonColourId, Colours::transparentBlack);
		magnitude_visible_button.setColour(magnitude_visible_button.buttonOnColourId, Colours::darkgreen);
		magnitude_visible_button.setToggleState(1, dontSendNotification);
		magnitude_visible_button.setClickingTogglesState(true);
		magnitude_visible_button.addListener(this);
		
		addAndMakeVisible(magnitude_master_button);
		magnitude_master_button.setButtonText("M");
		magnitude_master_button.setColour(magnitude_master_button.buttonColourId, Colours::transparentBlack);
		magnitude_master_button.setColour(magnitude_master_button.buttonOnColourId, Colours::darkgreen);
		magnitude_master_button.setToggleState(1, dontSendNotification);
		magnitude_master_button.addListener(this);

		addAndMakeVisible(phase_visible_button);
		phase_visible_button.setButtonText("P");
		phase_visible_button.setColour(phase_visible_button.buttonColourId, Colours::transparentBlack);
		phase_visible_button.setColour(phase_visible_button.buttonOnColourId, Colours::darkred);
		phase_visible_button.setClickingTogglesState(true);
		phase_visible_button.addListener(this);
		
		addAndMakeVisible(phase_master_button);
		phase_master_button.setButtonText("P");
		phase_master_button.setColour(phase_master_button.buttonColourId, Colours::transparentBlack);
		phase_master_button.setColour(phase_master_button.buttonOnColourId, Colours::darkred);
		phase_master_button.addListener(this);

		addAndMakeVisible(coherence_visible_button);
		coherence_visible_button.setButtonText("C");
		coherence_visible_button.setColour(coherence_visible_button.buttonColourId, Colours::transparentBlack);
		coherence_visible_button.setColour(coherence_visible_button.buttonOnColourId, Colours::darkcyan);
		coherence_visible_button.setClickingTogglesState(true);
		coherence_visible_button.addListener(this);
		
		addAndMakeVisible(coherence_master_button);
		coherence_master_button.setButtonText("C");
		coherence_master_button.setColour(coherence_master_button.buttonColourId, Colours::transparentBlack);
		coherence_master_button.setColour(coherence_master_button.buttonOnColourId, Colours::darkcyan);
		coherence_master_button.addListener(this);

		addAndMakeVisible(spectrum_visible_button);
		spectrum_visible_button.setButtonText("S");
		spectrum_visible_button.setColour(spectrum_visible_button.buttonColourId, Colours::transparentBlack);
		spectrum_visible_button.setColour(spectrum_visible_button.buttonOnColourId, Colours::darkmagenta);
		spectrum_visible_button.setClickingTogglesState(true);
		spectrum_visible_button.addListener(this);
		
		addAndMakeVisible(spectrum_master_button);
		spectrum_master_button.setButtonText("S");
		spectrum_master_button.setColour(spectrum_master_button.buttonColourId, Colours::transparentBlack);
		spectrum_master_button.setColour(spectrum_master_button.buttonOnColourId, Colours::darkmagenta);
		spectrum_master_button.addListener(this);
		
		std::fill(loaded_composite_fft_bin_frequencies.begin(), loaded_composite_fft_bin_frequencies.end(), 0.0);
		std::fill(loaded_composite_xfer_function_mag_dB_avg_cal.begin(), loaded_composite_xfer_function_mag_dB_avg_cal.end(), 0.0);
		std::fill(loaded_composite_xfer_function_phase_deg_avg.begin(), loaded_composite_xfer_function_phase_deg_avg.end(), 0.0);
		std::fill(loaded_composite_coherence_value.begin(), loaded_composite_coherence_value.end(), 0.0);

		addAndMakeVisible(xfer_function_plot);
		xfer_function_plot.setOpaque(true);
		xfer_function_plot.set_axis_modes(false, true);

		//==================================================// set up all the traces for the plot

		flexplot_trace live_magnitude_trace;
		live_magnitude_trace.set_trace_appearance(live_trace_thickness, Colours::green);
		live_magnitude_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", magnitude_gridline_coord, magnitude_gridline_labels, "dB");
		live_magnitude_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, -24.0f, 24.0f);
		xfer_function_traces.push_back(live_magnitude_trace);
		
		flexplot_trace live_phase_trace;
		live_phase_trace.set_trace_appearance(live_trace_thickness, Colours::red);
		live_phase_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", phase_gridline_coord, phase_gridline_labels, "DEG");
		live_phase_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, -180.0f, 180.0f);
		xfer_function_traces.push_back(live_phase_trace);
		
		flexplot_trace live_coherence_trace;
		live_coherence_trace.set_trace_appearance(live_trace_thickness, Colours::blue);
		live_coherence_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", coherence_gridline_coord, coherence_gridline_labels, "");
		live_coherence_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, 0.0f, 1.0f);
		xfer_function_traces.push_back(live_coherence_trace);

		flexplot_trace live_spectrum_trace;
		live_spectrum_trace.set_trace_appearance(live_trace_thickness, Colours::magenta);
		live_spectrum_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", spectrum_gridline_coord, spectrum_gridline_labels, "dBFS");
		live_spectrum_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, -96.0f, 0.0f);
		xfer_function_traces.push_back(live_spectrum_trace);

		flexplot_trace loaded_magnitude_trace;
		loaded_magnitude_trace.set_trace_appearance(loaded_trace_thickness, Colours::darkgreen);
		loaded_magnitude_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", magnitude_gridline_coord, magnitude_gridline_labels, "dB");
		loaded_magnitude_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, -24.0f, 24.0f);
		xfer_function_traces.push_back(loaded_magnitude_trace);

		flexplot_trace loaded_phase_trace;
		loaded_phase_trace.set_trace_appearance(loaded_trace_thickness, Colours::darkred);
		loaded_phase_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", phase_gridline_coord, phase_gridline_labels, "DEG");
		loaded_phase_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, -180.0f, 180.0f);
		xfer_function_traces.push_back(loaded_phase_trace);

		flexplot_trace loaded_coherence_trace;
		loaded_coherence_trace.set_trace_appearance(loaded_trace_thickness, Colours::darkblue);
		loaded_coherence_trace.set_trace_gridlines(x_gridline_coord, x_gridline_labels, "", coherence_gridline_coord, coherence_gridline_labels, "");
		loaded_coherence_trace.set_trace_plotting_boundaries(20.0f, 20000.0f, 0.0f, 1.0f);
		xfer_function_traces.push_back(loaded_coherence_trace);

		xfer_function_plot.add_data_set(&xfer_function_traces[0]);
		xfer_function_plot.add_data_set(&xfer_function_traces[1]);
		xfer_function_plot.add_data_set(&xfer_function_traces[2]);
		xfer_function_plot.add_data_set(&xfer_function_traces[3]);
		xfer_function_plot.add_data_set(&xfer_function_traces[4]);
		xfer_function_plot.add_data_set(&xfer_function_traces[5]);
		xfer_function_plot.add_data_set(&xfer_function_traces[6]);

    }

    ~plots()
    {
    }

    void paint (Graphics& g) override
	{

		calc_display_values(display_composite_xfer_function_mag_dB, current_composite_xfer_function_mag_dB, 0);
		calc_display_values(display_composite_xfer_function_phase_deg, current_composite_xfer_function_phase_deg, 1);
		calc_display_values(display_composite_coherence_value, current_composite_coherence_value, 2);
		calc_display_values(display_composite_system_spectrum_mag_dB, current_composite_system_spectrum_mag_dB, 3);

		xfer_function_traces[0].set_data(current_composite_fft_bin_frequencies, display_composite_xfer_function_mag_dB);
		xfer_function_traces[1].set_data(current_composite_fft_bin_frequencies, display_composite_xfer_function_phase_deg);
		xfer_function_traces[2].set_data(current_composite_fft_bin_frequencies, display_composite_coherence_value);
		xfer_function_traces[3].set_data(current_composite_fft_bin_frequencies, display_composite_system_spectrum_mag_dB);
		xfer_function_traces[4].set_data(loaded_composite_fft_bin_frequencies, loaded_composite_xfer_function_mag_dB_avg_cal);
		xfer_function_traces[5].set_data(loaded_composite_fft_bin_frequencies, loaded_composite_xfer_function_phase_deg_avg);
		xfer_function_traces[6].set_data(loaded_composite_fft_bin_frequencies, loaded_composite_coherence_value);

		xfer_function_traces[0].set_trace_visible(magnitude_visible_button.getToggleState());
		xfer_function_traces[1].set_trace_visible(phase_visible_button.getToggleState());
		xfer_function_traces[2].set_trace_visible(coherence_visible_button.getToggleState());
		xfer_function_traces[3].set_trace_visible(spectrum_visible_button.getToggleState());
		xfer_function_traces[4].set_trace_visible(magnitude_visible_button.getToggleState());
		xfer_function_traces[5].set_trace_visible(phase_visible_button.getToggleState());
		xfer_function_traces[6].set_trace_visible(coherence_visible_button.getToggleState());

		for (int trace = 0; trace < xfer_function_traces.size(); trace++) {

			if (trace == master_trace) { xfer_function_traces[trace].set_trace_master(true); }
			if (trace != master_trace) { xfer_function_traces[trace].set_trace_master(false); }

		}

		mouse_relative_position_pix = getMouseXYRelative();
		int mouse_absolute_x_pix = mouse_relative_position_pix.getX() + getScreenX();
		int mouse_absolute_y_pix = mouse_relative_position_pix.getY() + getScreenY();

		std::vector<int> plot_edges_screen_pix = xfer_function_plot.get_plot_edges_screen_pix();

		int plot_left_screen_absolute_x_pix = plot_edges_screen_pix[0];
		int plot_right_screen_absolute_x_pix = plot_edges_screen_pix[1];
		int plot_bottom_screen_absolute_y_pix = plot_edges_screen_pix[2];
		int plot_top_screen_absolute_y_pix = plot_edges_screen_pix[3];

		float mouse_x_position_plot_proportion = 
			(mouse_absolute_x_pix - plot_left_screen_absolute_x_pix)*1.0 / 
			(plot_right_screen_absolute_x_pix - plot_left_screen_absolute_x_pix)*1.0;

		if (mouse_x_position_plot_proportion >= 0.0 
			&& mouse_x_position_plot_proportion <= 1.0
			&& mouse_absolute_y_pix >= plot_top_screen_absolute_y_pix
			&& mouse_absolute_y_pix <= plot_bottom_screen_absolute_y_pix) {

			xfer_function_plot.set_screen_to_data_x_position_proportion(mouse_x_position_plot_proportion);
			xfer_function_plot.mod_line_plot_x_propotion = mouse_x_position_plot_proportion;
			xfer_function_plot.mod_line_visible = true;

		}

		else {

			xfer_function_plot.mod_line_visible = false;

		}
		
		xfer_function_plot.sample_rate_incorrect = sample_rate_incorrect_warning;
		xfer_function_plot.repaint();

		fetch_screen_to_data_values();
		update_mod_strings();
		
		g.setFont(frequency_mod_outline.getHeight() * 0.8);
		g.setColour(Colours::white);
		g.drawText(frequency_mod_value_string, frequency_mod_outline, Justification::centred);
		g.setColour(Colours::darkgreen);
		g.drawText(magnitude_mod_value_string, magnitude_mod_outline, Justification::centred);
		g.setColour(Colours::darkred);
		g.drawText(phase_mod_value_string, phase_mod_outline, Justification::centred);
		g.setColour(Colours::darkcyan);
		g.drawText(coherence_mod_value_string, coherence_mod_outline, Justification::centred);
		g.setColour(Colours::darkmagenta);
		g.drawText(spectrum_mod_value_string, spectrum_mod_outline, Justification::centred);
		
	}

    void resized() override
    {
		plot_outline = getLocalBounds();
		top_title_region_outline = plot_outline.removeFromTop(plot_outline.getHeight() * 0.05);
		top_title_region_active_area = top_title_region_outline.reduced(3);
		xfer_function_plot.setBounds(plot_outline);

		int top_title_region_active_area_width = top_title_region_active_area.getWidth();
		float button_width_proportional = 0.05;
		magnitude_visible_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		phase_visible_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		coherence_visible_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		spectrum_visible_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		
		mod_region = top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * 0.60);
		int mod_region_width = mod_region.getWidth();
		frequency_mod_outline = mod_region.removeFromLeft(mod_region_width*0.2);
		magnitude_mod_outline = mod_region.removeFromLeft(mod_region_width*0.2);
		phase_mod_outline = mod_region.removeFromLeft(mod_region_width*0.2);
		coherence_mod_outline = mod_region.removeFromLeft(mod_region_width*0.2);
		spectrum_mod_outline = mod_region;
		
		magnitude_master_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		phase_master_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		coherence_master_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		spectrum_master_button.setBounds(top_title_region_active_area.removeFromLeft(top_title_region_active_area_width * button_width_proportional));
		
    }

private:

	xfer_func_plot xfer_function_plot;
	std::vector<flexplot_trace> xfer_function_traces;
	int live_trace_thickness = 3;
	int loaded_trace_thickness = 3;
	
	Rectangle<int> plot_outline;
	Rectangle<int> top_title_region_outline;
	Rectangle<int> top_title_region_active_area;
	Rectangle<int> mod_region; //mod = mouse over display
	Rectangle<int> frequency_mod_outline;
	Rectangle<int> magnitude_mod_outline;
	Rectangle<int> phase_mod_outline;
	Rectangle<int> coherence_mod_outline;
	Rectangle<int> spectrum_mod_outline;

	TextButton magnitude_visible_button;
	TextButton magnitude_master_button;

	TextButton phase_visible_button;
	TextButton phase_master_button;

	TextButton coherence_visible_button;
	TextButton coherence_master_button;

	TextButton spectrum_visible_button;
	TextButton spectrum_master_button;

	int master_trace{ 0 };

	std::vector<float> x_gridline_coord{ 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f,
		100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f,
		1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
		10000.0f, 20000.0f };

	std::vector<String> x_gridline_labels{ "20", "30", "", "50", "", "70", "", "",
		"100", "200", "300", "", "500", "", "700", "", "",
		"1k", "2k", "3k", "", "5k", "", "7k", "", "",
		"10k", "20k" };

	std::vector<float> magnitude_gridline_coord{ 24.0f,18.0f,12.0f,6.0f,0.0f,-6.0f,-12.0f,-18.0f,-24.0f };
	std::vector<String> magnitude_gridline_labels{ "+24", "+18", "+12", "+6", "0", "-6", "-12", "-18", "-24" };

	std::vector<float> phase_gridline_coord{ 180.0f,135.0f,90.0f,45.0f,0.0f,-45.0f,-90.0f,-135.0f,-180.0f };
	std::vector<String> phase_gridline_labels{ "+180", "+135", "+90", "+45", "0", "-45", "-90", "-135", "-180" };

	std::vector<float> coherence_gridline_coord{ 1.0f,0.9f,0.8f,0.7f,0.6f,0.5f,0.4f,0.3f,0.2f,0.1f,0.0f };
	std::vector<String> coherence_gridline_labels{ "1.0", "0.9", "0.8", "0.7", "0.6", "0.5", "0.4", "0.3", "0.2", "0.1", "0.0" };

	std::vector<float> spectrum_gridline_coord{ 0.0f,-12.0f,-24.0f,-36.0f,-48.0f,-60.0f,-72.0f,-84.0f,-96.0f };
	std::vector<String> spectrum_gridline_labels{ "0", "-12", "-24", "-36", "-48", "-60", "-72", "-84", "-96" };

	std::vector<float> display_composite_xfer_function_mag_dB;
	std::vector<float> display_composite_xfer_function_phase_deg;
	std::vector<float> display_composite_coherence_value;
	std::vector<float> display_composite_system_spectrum_mag_dB;

	double magnitude_rate_of_change = 0.1;
	double phase_rate_of_change = 0.1;
	double coherence_rate_of_change = 0.1;
	double spectrum_rate_of_change = 0.05;

	double magnitude_snap_range = 0.0;
	double phase_snap_range = 0.0;
	double coherence_snap_range = 0.0;
	double spectrum_snap_range = 0.0;
	
	Point<int> mouse_relative_position_pix;

	int mod_crosshair_thickness = 1.0;
	bool mod_line_visible = false;

	int frequency_mod_value = 0;
	double magnitude_mod_value = 0.0;
	double phase_mod_value = 0.0;
	double coherence_mod_value = 0.0;
	double spectrum_mod_value = 0.0;

	String frequency_mod_value_string;
	String magnitude_mod_value_string;
	String phase_mod_value_string;
	String coherence_mod_value_string;
	String spectrum_mod_value_string;
		
	void buttonClicked(Button* button) override {

		if (button == &magnitude_master_button) {master_trace = 0;}

		if (button == &phase_master_button) {master_trace = 1;}

		if (button == &coherence_master_button) {master_trace = 2;}

		if (button == &spectrum_master_button) {master_trace = 3;}

		configure_master_trace_buttons();

	}

	void configure_master_trace_buttons() {

		if (master_trace == 0) {

			magnitude_master_button.setToggleState(true, dontSendNotification);
			phase_master_button.setToggleState(false, dontSendNotification);
			coherence_master_button.setToggleState(false, dontSendNotification);
			spectrum_master_button.setToggleState(false, dontSendNotification);

		}

		if (master_trace == 1) {

			magnitude_master_button.setToggleState(false, dontSendNotification);
			phase_master_button.setToggleState(true, dontSendNotification);
			coherence_master_button.setToggleState(false, dontSendNotification);
			spectrum_master_button.setToggleState(false, dontSendNotification);

		}

		if (master_trace == 2) {

			magnitude_master_button.setToggleState(false, dontSendNotification);
			phase_master_button.setToggleState(false, dontSendNotification);
			coherence_master_button.setToggleState(true, dontSendNotification);
			spectrum_master_button.setToggleState(false, dontSendNotification);

		}

		if (master_trace == 3) {

			magnitude_master_button.setToggleState(false, dontSendNotification);
			phase_master_button.setToggleState(false, dontSendNotification);
			coherence_master_button.setToggleState(false, dontSendNotification);
			spectrum_master_button.setToggleState(true, dontSendNotification);

		}

	}

	void calc_display_values(std::vector<float> &display_value_vector, std::vector<float> &current_value_vector, int plot_type) {

		float snap_range = 0.0;

		float rate_of_change = 0.0;
		
		if (plot_type == 0) {

			snap_range = magnitude_snap_range;

			rate_of_change = magnitude_rate_of_change;

		}

		if (plot_type == 1) {

			snap_range = phase_snap_range;

			rate_of_change = phase_rate_of_change;

		}

		if (plot_type == 2) {

			snap_range = coherence_snap_range;

			rate_of_change = coherence_rate_of_change;

		}

		if (plot_type == 3) {

			snap_range = spectrum_snap_range;

			rate_of_change = spectrum_rate_of_change;

		}

		for (int x = 0; x < composite_fft_bins; x++) {

			if ((display_value_vector[x] - current_value_vector[x]) > snap_range) {

				display_value_vector[x] =

					display_value_vector[x] -

					(abs(display_value_vector[x] - current_value_vector[x]) * rate_of_change);

			}

			if ((display_value_vector[x] - current_value_vector[x]) < -snap_range) {

				display_value_vector[x] =

					display_value_vector[x] +

					(abs(display_value_vector[x] - current_value_vector[x])* rate_of_change);

			}

			if (abs(display_value_vector[x] - current_value_vector[x]) <= snap_range) {

				display_value_vector[x] = current_value_vector[x];

			}
			
		}

	}

	void fetch_screen_to_data_values() {
		
		std::vector<std::pair<float, float>> screen_to_data_pairs = xfer_function_plot.get_screen_to_data_pairs();

		if (screen_to_data_pairs.size() == 4) {

			frequency_mod_value = screen_to_data_pairs[0].first;
			magnitude_mod_value = screen_to_data_pairs[0].second;
			phase_mod_value = screen_to_data_pairs[1].second;
			coherence_mod_value = screen_to_data_pairs[2].second;
			spectrum_mod_value = screen_to_data_pairs[3].second;

		}

	}

	void update_mod_strings() {

		frequency_mod_value_string = String(frequency_mod_value);
		frequency_mod_value_string += " Hz";

		magnitude_mod_value_string = String(magnitude_mod_value);
		magnitude_mod_value_string = magnitude_mod_value_string.substring(0, magnitude_mod_value_string.indexOfChar('.') + 2);
		magnitude_mod_value_string += " dB";
				
		phase_mod_value_string = String(phase_mod_value);
		phase_mod_value_string = phase_mod_value_string.substring(0, phase_mod_value_string.indexOfChar('.'));
		phase_mod_value_string += " Deg";
				
		coherence_mod_value_string = String(coherence_mod_value);
		coherence_mod_value_string = coherence_mod_value_string.substring(0, coherence_mod_value_string.indexOfChar('.') + 3);
				
		spectrum_mod_value_string = String(spectrum_mod_value);
		spectrum_mod_value_string = spectrum_mod_value_string.substring(0, spectrum_mod_value_string.indexOfChar('.'));
		spectrum_mod_value_string += " dB";

	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (plots)
};
