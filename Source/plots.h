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

//==============================================================================
/*
*/
class plots : public Component, public constants, public Slider::Listener, public Button::Listener
{
public:

	Rectangle<int> plot_outline;
	Rectangle<int> top_title_region;
	Rectangle<int> mod_region; //mod = mouse over display
	Rectangle<int> frequency_mod_bounds;
	Rectangle<int> magnitude_mod_bounds;
	Rectangle<int> phase_mod_bounds;
	Rectangle<int> coherence_mod_bounds;
	Rectangle<int> spectrum_mod_bounds;
	Rectangle<int> top_slider_region;
	Rectangle<int> left_slider_region;
	Rectangle<int> left_label_region;
	Rectangle<int> left_label_clip_region;
	Rectangle<int> plot_actual_region;
	Rectangle<int> plot_clip_region;
	Rectangle<int> right_slider_region;
	Rectangle<int> bottom_label_region;
	Rectangle<int> bottom_label_clip_region;
	Rectangle<int> bottom_slider_region;
	
	std::vector<int> grid_frequencies{20, 30, 40, 50, 60, 70, 80, 90, 
										100, 200, 300, 400, 500, 600, 700, 800, 900,
										1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
										10000, 20000};

	std::vector<std::string> grid_frequencies_labels{ "20", "30", "", "50", "", "70", "", "",
		"100", "200", "300", "", "500", "", "700", "", "",
		"1k", "2k", "3k", "", "5k", "", "7k", "", "",
		"10k", "20k"};

	std::vector<std::string> magnitude_labels{"+24", "+18", "+12", "+6", "0 dB", "-6", "-12", "-18", "-24"};

	std::vector<std::string> phase_labels{ "+180", "+135", "+90", "+45", "0 DEG", "-45", "-90", "-135", "-180"};

	std::vector<std::string> coherence_labels{ "1.0", "0.9", "0.8", "0.7", "0.6", "0.5", "0.4", "0.3", "0.2", "0.1", "0.0"};

	std::vector<std::string> spectrum_labels{ "0 dBFS", "-12", "-24", "-36", "-48", "-60", "-72", "-84", "-96" };

	double plot_horizontal_zoom = 1.0;
	double plot_horizontal_center_offset = 0.0;
	double plot_vertical_zoom = 1.0;
	double plot_vertical_center_offset = 0.0;

	double horizontal_center_span = 0.0;

	std::vector<double> current_composite_fft_bin_frequencies;

	std::vector<double> current_composite_xfer_function_mag_dB;
	std::vector<double> display_composite_xfer_function_mag_dB;

	std::vector<double> current_composite_xfer_function_phase_deg;
	std::vector<double> display_composite_xfer_function_phase_deg;

	std::vector<double> current_composite_coherence_value;
	std::vector<double> display_composite_coherence_value;

	std::vector<double> current_composite_system_spectrum_mag_dB;
	std::vector<double> display_composite_system_spectrum_mag_dB;

	std::vector<double> loaded_composite_fft_bin_frequencies;
	std::vector<double> loaded_composite_xfer_function_mag_dB_avg_cal;
	std::vector<double> loaded_composite_xfer_function_phase_deg_avg;
	std::vector<double> loaded_composite_coherence_value;

	float grid_line_thickness = 0.25;

	int axis_label_width = 50;

	double axis_label_scale = 0.5;
	
	int plot_label_width = 100;
	int plot_label_height = 1;

	float live_trace_thickness = 3.0;

	float saved_trace_thickness = 3.0;

	float trace_path_smoothing_radius = 10.0;

	float horizontal_grid_label_right_padding = 0.005;

	int active_grid_mode = 0; //0 = magnitude, 1 = phase, 2 = coherence, 3 = spectrum

	int magnitude_visible = 1;

	int phase_visible = 0;

	int coherence_visible = 0;

	int spectrum_visible = 0;

	int saved_traces_visible = 0;

	bool repaint_active = false;

	double magnitude_rate_of_change = 0.1;
	double phase_rate_of_change = 0.1;
	double coherence_rate_of_change = 0.1;
	double spectrum_rate_of_change = 0.05;

	double magnitude_snap_range = 0.0;
	double phase_snap_range = 0.0;
	double coherence_snap_range = 0.0;
	double spectrum_snap_range = 0.0;

	int mod_region_width = 0;

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
		
		addAndMakeVisible(horizontal_zoom_slider);
		addAndMakeVisible(horizontal_center_slider);
		
		addAndMakeVisible(vertical_zoom_slider);
		addAndMakeVisible(vertical_center_slider);

		addAndMakeVisible(magnitude_visible_button);
		addAndMakeVisible(magnitude_active_button);

		addAndMakeVisible(phase_visible_button);
		addAndMakeVisible(phase_active_button);

		addAndMakeVisible(coherence_visible_button);
		addAndMakeVisible(coherence_active_button);

		addAndMakeVisible(spectrum_visible_button);
		addAndMakeVisible(spectrum_active_button);

		addAndMakeVisible(frequency_mod);
		addAndMakeVisible(magnitude_mod);
		addAndMakeVisible(phase_mod);
		addAndMakeVisible(coherence_mod);
		addAndMakeVisible(spectrum_mod);

		addAndMakeVisible(sample_rate_incorrect_warning_label);
		sample_rate_incorrect_warning_label.setJustificationType(Justification::centred);
		sample_rate_incorrect_warning_label.setText(
			"SAMPLE RATE INCORRECT. PLEASE SET SAMPLE RATE TO 48000 Hz USING AUDIO INPUT SETUP WINDOW.", 
			dontSendNotification);

		frequency_mod.setJustificationType(Justification::centred);
		magnitude_mod.setJustificationType(Justification::centred);
		phase_mod.setJustificationType(Justification::centred);
		coherence_mod.setJustificationType(Justification::centred);
		spectrum_mod.setJustificationType(Justification::centred);

		frequency_mod.setColour(frequency_mod.textColourId, Colours::white);
		magnitude_mod.setColour(magnitude_mod.textColourId, Colours::green);
		phase_mod.setColour(phase_mod.textColourId, Colours::red);
		coherence_mod.setColour(coherence_mod.textColourId, Colours::cyan);
		spectrum_mod.setColour(spectrum_mod.textColourId, Colours::magenta);

		magnitude_visible_button.setButtonText("M");
		magnitude_active_button.setButtonText("M");

		phase_visible_button.setButtonText("P");
		phase_active_button.setButtonText("P");

		coherence_visible_button.setButtonText("C");
		coherence_active_button.setButtonText("C");

		spectrum_visible_button.setButtonText("S");
		spectrum_active_button.setButtonText("S");

		magnitude_visible_button.setColour(magnitude_visible_button.buttonColourId, Colours::transparentBlack);
		magnitude_visible_button.setColour(magnitude_visible_button.buttonOnColourId, Colours::darkgreen);
		
		
		magnitude_active_button.setColour(magnitude_active_button.buttonColourId, Colours::transparentBlack);
		magnitude_active_button.setColour(magnitude_active_button.buttonOnColourId, Colours::darkgreen);

		
		phase_visible_button.setColour(phase_visible_button.buttonColourId, Colours::transparentBlack);
		phase_visible_button.setColour(phase_visible_button.buttonOnColourId, Colours::darkred);
				
		
		phase_active_button.setColour(phase_active_button.buttonColourId, Colours::transparentBlack);
		phase_active_button.setColour(phase_active_button.buttonOnColourId, Colours::darkred);

		
		coherence_visible_button.setColour(coherence_visible_button.buttonColourId, Colours::transparentBlack);
		coherence_visible_button.setColour(coherence_visible_button.buttonOnColourId, Colours::darkcyan);
		
		
		coherence_active_button.setColour(coherence_active_button.buttonColourId, Colours::transparentBlack);
		coherence_active_button.setColour(coherence_active_button.buttonOnColourId, Colours::darkcyan);

		
		spectrum_visible_button.setColour(spectrum_visible_button.buttonColourId, Colours::transparentBlack);
		spectrum_visible_button.setColour(spectrum_visible_button.buttonOnColourId, Colours::darkmagenta);
		
		
		spectrum_active_button.setColour(spectrum_active_button.buttonColourId, Colours::transparentBlack);
		spectrum_active_button.setColour(spectrum_active_button.buttonOnColourId, Colours::darkmagenta);

		
		magnitude_active_button.setToggleState(1, dontSendNotification);
		magnitude_visible_button.setToggleState(1, dontSendNotification);

		horizontal_zoom_slider.setRange(1, 10);
		horizontal_center_slider.setRange(-1, 1);
		
		vertical_zoom_slider.setRange(1, 10);
		vertical_center_slider.setRange(-1, 1);

		vertical_zoom_slider.setSliderStyle(Slider::SliderStyle::LinearVertical);
		vertical_center_slider.setSliderStyle(Slider::SliderStyle::LinearVertical);

		horizontal_zoom_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 0, 0);
		horizontal_center_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 0, 0);
		
		vertical_zoom_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 0, 0);
		vertical_center_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 0, 0);

		horizontal_zoom_slider.setDoubleClickReturnValue(1, 1);
		horizontal_center_slider.setDoubleClickReturnValue(1, 0);
		
		vertical_zoom_slider.setDoubleClickReturnValue(1, 1);
		vertical_center_slider.setDoubleClickReturnValue(1, 0);

		horizontal_zoom_slider.addListener(this);
		horizontal_center_slider.addListener(this);
		
		vertical_zoom_slider.addListener(this);
		vertical_center_slider.addListener(this);

		magnitude_visible_button.addListener(this);
		magnitude_active_button.addListener(this);

		phase_visible_button.addListener(this);
		phase_active_button.addListener(this);

		coherence_visible_button.addListener(this);
		coherence_active_button.addListener(this);

		spectrum_visible_button.addListener(this);
		spectrum_active_button.addListener(this);

		std::fill(loaded_composite_fft_bin_frequencies.begin(), loaded_composite_fft_bin_frequencies.end(), 0.0);
		std::fill(loaded_composite_xfer_function_mag_dB_avg_cal.begin(), loaded_composite_xfer_function_mag_dB_avg_cal.end(), 0.0);
		std::fill(loaded_composite_xfer_function_phase_deg_avg.begin(), loaded_composite_xfer_function_phase_deg_avg.end(), 0.0);
		std::fill(loaded_composite_coherence_value.begin(), loaded_composite_coherence_value.end(), 0.0);

    }

    ~plots()
    {
    }

    void paint (Graphics& g) override
	{
		repaint_active = true;
				
		calc_display_values(display_composite_xfer_function_mag_dB, current_composite_xfer_function_mag_dB, 0);

		calc_display_values(display_composite_xfer_function_phase_deg, current_composite_xfer_function_phase_deg, 1);

		calc_display_values(display_composite_coherence_value, current_composite_coherence_value, 2);

		calc_display_values(display_composite_system_spectrum_mag_dB, current_composite_system_spectrum_mag_dB, 3);

		mouse_current_position = getMouseXYRelative();

		if (plot_clip_region.contains(mouse_current_position) == true) {

			mod_line_visible = true;

			update_mod_values();

		}

		else {

			mod_line_visible = false;

		}
		
		frequency_mod.setText(frequency_mod_value_string, dontSendNotification);
		magnitude_mod.setText(magnitude_mod_value_string, dontSendNotification);
		phase_mod.setText(phase_mod_value_string, dontSendNotification);
		coherence_mod.setText(coherence_mod_value_string, dontSendNotification);
		spectrum_mod.setText(spectrum_mod_value_string, dontSendNotification);
		
		magnitude_visible = magnitude_visible_button.getToggleState();

		phase_visible = phase_visible_button.getToggleState();

		coherence_visible = coherence_visible_button.getToggleState();

		spectrum_visible = spectrum_visible_button.getToggleState();
		
		if (magnitude_active_button.getToggleState() == true) {

			active_grid_mode = 0;

		}

		else if (phase_active_button.getToggleState() == true) {

			active_grid_mode = 1;

		}

		else if (coherence_active_button.getToggleState() == true) {

			active_grid_mode = 2;

		}

		else {

			active_grid_mode = 3;

		}

		g.setColour(Colours::white);
		
		//g.drawRect(top_title_region, 1);
		//g.drawRect(top_slider_region, 1);
		//g.drawRect(left_slider_region, 1);
		//g.drawRect(left_label_region, 1);
		//g.drawRect(left_label_clip_region, 1);
		//g.drawRect(plot_actual_region, 1);
		//g.drawRect(right_slider_region, 1);
		//g.drawRect(bottom_label_region, 1);
		//g.drawRect(bottom_label_clip_region, 1);
		//g.drawRect(bottom_slider_region, 1);
		//g.drawRect(frequency_mod_bounds, 1.0);
		//g.drawRect(magnitude_mod_bounds, 1.0);
		//g.drawRect(phase_mod_bounds, 1.0);
		//g.drawRect(coherence_mod_bounds, 1.0);
		//g.drawRect(spectrum_mod_bounds, 1.0);

		//==============================================================================

		g.saveState();
		
		g.reduceClipRegion(plot_clip_region);

		g.setColour(Colours::black);
		
		g.fillRect(plot_actual_region);

		g.setColour(Colours::white);

		g.restoreState();

		if (sample_rate_incorrect_warning == true) {

			sample_rate_incorrect_warning_label.setVisible(true);

			goto endofpaint;

		}

		else { sample_rate_incorrect_warning_label.setVisible(false); }
				
		//==============================================================================

		g.saveState();

		for (int x = 0; x < 28; x++) {
			
			double raw_distance = log10(grid_frequencies[x]);

			double zeroed_distance = raw_distance - log10(grid_frequencies[0]);

			double normalized_distance = zeroed_distance / 3;	// the zeroed distance at 20 kHz is 3, which is the largest distance. 
																//This put all distance values on the scale of 0 to 1

			double x_pos = plot_actual_region.getX() + normalized_distance * plot_actual_region.getWidth();

			g.reduceClipRegion(plot_clip_region);
			
			g.drawRect(x_pos, plot_outline.getHeight()*0.1, grid_line_thickness, float(plot_actual_region.getHeight()));

		}

		g.restoreState();

		//==============================================================================

		g.saveState();

		double axis_label_font_height = bottom_label_region.getHeight()*axis_label_scale;
		
		g.setFont(axis_label_font_height);

		double horizontal_axis_label_width = plot_clip_region.getWidth();

		for (int x = 0; x < 28; x++) {

			double raw_distance = log10(grid_frequencies[x]);

			double zeroed_distance = raw_distance - log10(grid_frequencies[0]);

			double normalized_distance = zeroed_distance / 3;	// the zeroed distance at 20 kHz is 3, which is the largest distance. 
																//This put all distance values on the scale of 0 to 1

			double x_pos = plot_actual_region.getX() + normalized_distance * plot_actual_region.getWidth();

			g.reduceClipRegion(bottom_label_clip_region);

			g.drawText(
				
				grid_frequencies_labels[x], x_pos - (horizontal_axis_label_width / 2),
				
				float(bottom_label_region.getCentreY()) - (axis_label_font_height / 2), 
				
				float(horizontal_axis_label_width), axis_label_font_height, 
				
				Justification::centred);

		}

		g.restoreState();

		//==============================================================================

		g.saveState();

		g.reduceClipRegion(plot_clip_region);

		switch (active_grid_mode)
		{
		case 0: case 1:
			
			for (int x = 0; x < 9; x++) {
				
				g.drawRect(

					float(plot_actual_region.getX()),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*12.5) / 100))),

					float(plot_actual_region.getWidth()),
						
					grid_line_thickness);
		
			}

			g.drawRect( //draw an extra thick gridline at 0 dB

				float(plot_actual_region.getX()),

				plot_actual_region.getY() + ((plot_actual_region.getHeight()*((4*12.5) / 100))),

				float(plot_actual_region.getWidth()),

				grid_line_thickness*3);

			break;

		case 2:

			for (int x = 0; x < 11; x++) {

				g.drawRect(

					float(plot_actual_region.getX()),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*10.0) / 100))),

					float(plot_actual_region.getWidth()),

					grid_line_thickness);

			}

			break;

		case 3:

			for (int x = 0; x < 9; x++) {

				g.drawRect(

					float(plot_actual_region.getX()),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*12.5) / 100))),

					float(plot_actual_region.getWidth()),

					grid_line_thickness);

			}

			break;
		
		default:
			break;
		}

		g.restoreState();

		//==============================================================================

		g.saveState();

		g.reduceClipRegion(left_label_clip_region);

		g.setFont(axis_label_font_height);

		switch (active_grid_mode)
		{
		case 0:

			for (int x = 0; x < 9; x++) {

				g.drawText(
					
					magnitude_labels[x],

					left_label_region.getX(),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*12.5) / 100))) - (axis_label_font_height/2),

					left_label_region.getWidth(),

					axis_label_font_height,
					
					Justification::centred);

			}

			break;

		case 1:
			
			for (int x = 0; x < 9; x++) {

				g.drawText(

					phase_labels[x],

					left_label_region.getX(),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*12.5) / 100))) - (axis_label_font_height / 2),

					left_label_region.getWidth(),

					axis_label_font_height,

					Justification::centred);

			}

			break;

		case 2:

			for (int x = 0; x < 11; x++) {

				g.drawText(

					coherence_labels[x],

					left_label_region.getX(),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*10.0) / 100))) - (axis_label_font_height / 2),

					left_label_region.getWidth(),

					axis_label_font_height,

					Justification::centred);

			}

			break;

		case 3:

			for (int x = 0; x < 9; x++) {

				g.drawText(

					spectrum_labels[x],

					left_label_region.getX(),

					plot_actual_region.getY() + ((plot_actual_region.getHeight()*((x*12.5) / 100))) - (axis_label_font_height / 2),

					left_label_region.getWidth(),

					axis_label_font_height,

					Justification::centred);

			}

			break;

		default:
			break;
		}

		g.restoreState();

		//==============================================================================

		g.saveState();

		g.reduceClipRegion(plot_clip_region);

		double plot_actual_region_x = plot_actual_region.getX();

		double plot_actual_region_y = plot_actual_region.getY();

		double plot_actual_region_width = plot_actual_region.getWidth();

		double plot_actual_region_height = plot_actual_region.getHeight();

		if (magnitude_visible == 1) {

			Path mag_trace;
			Path mag_smoothed_trace;

			mag_trace.startNewSubPath(

				plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*amp_to_y(display_composite_xfer_function_mag_dB[0]));


			for (int x = 1; x < composite_fft_bins; x++) {

				mag_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*amp_to_y(display_composite_xfer_function_mag_dB[x]));

			}

			mag_smoothed_trace = mag_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::green);

			g.strokePath(mag_smoothed_trace, PathStrokeType(live_trace_thickness));

		};

		if (phase_visible == 1) {

			Path phase_trace;
			Path phase_smoothed_trace;

			phase_trace.startNewSubPath(

				plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*phase_to_y(display_composite_xfer_function_phase_deg[0]));


			for (int x = 1; x < composite_fft_bins; x++) {

				phase_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*phase_to_y(display_composite_xfer_function_phase_deg[x]));

			}

			phase_smoothed_trace = phase_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::darkred);

			g.strokePath(phase_smoothed_trace, PathStrokeType(live_trace_thickness));

		};

		if (coherence_visible == 1) {

			Path coh_trace;
			Path coh_smoothed_trace;

			coh_trace.startNewSubPath(

				plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*(1.0-display_composite_coherence_value[0]));

			for (int x = 1; x < composite_fft_bins; x++) {

				coh_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*(1.0-display_composite_coherence_value[x]));

			}

			coh_smoothed_trace = coh_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::darkcyan);

			g.strokePath(coh_smoothed_trace, PathStrokeType(live_trace_thickness));

		}

		if (spectrum_visible == 1) {

			Path spectrum_trace;
			Path spectrum_smoothed_trace;

			spectrum_trace.startNewSubPath(

				plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*spectrum_mag_to_y(display_composite_system_spectrum_mag_dB[0]));


			for (int x = 1; x < composite_fft_bins; x++) {

				spectrum_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*spectrum_mag_to_y(display_composite_system_spectrum_mag_dB[x]));

			}

			spectrum_smoothed_trace = spectrum_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::darkmagenta);

			g.strokePath(spectrum_smoothed_trace, PathStrokeType(live_trace_thickness));

		};

		if (saved_traces_visible == 1 && loaded_composite_fft_bin_frequencies[0] != 0.0) {

			if (magnitude_visible == 1) {

				Path mag_trace;
				Path mag_smoothed_trace;

				mag_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*amp_to_y(loaded_composite_xfer_function_mag_dB_avg_cal[0]));


				for (int x = 1; x < composite_fft_bins; x++) {

					mag_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*amp_to_y(loaded_composite_xfer_function_mag_dB_avg_cal[x]));

				}

				mag_smoothed_trace = mag_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(0, 80, 0));

				g.strokePath(mag_smoothed_trace, PathStrokeType(saved_trace_thickness));

			};

			if (phase_visible == 1) {

				Path phase_trace;
				Path phase_smoothed_trace;

				phase_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*phase_to_y(loaded_composite_xfer_function_phase_deg_avg[0]));


				for (int x = 1; x < composite_fft_bins; x++) {

					phase_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*phase_to_y(loaded_composite_xfer_function_phase_deg_avg[x]));

				}

				phase_smoothed_trace = phase_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(80, 0, 0));

				g.strokePath(phase_smoothed_trace, PathStrokeType(saved_trace_thickness));

			};

			if (coherence_visible == 1) {

				Path coh_trace;
				Path coh_smoothed_trace;

				coh_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*(1.0 - loaded_composite_coherence_value[0]));

				for (int x = 1; x < composite_fft_bins; x++) {

					coh_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_normalized_distance(loaded_composite_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*(1.0 - loaded_composite_coherence_value[x]));

				}

				coh_smoothed_trace = coh_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(0, 0, 80));

				g.strokePath(coh_smoothed_trace, PathStrokeType(saved_trace_thickness));

			}

		}
		
		g.restoreState();

		//==============================================================================

		if (mod_line_visible == true) {

			g.saveState();

			g.reduceClipRegion(plot_clip_region);

			g.setColour(Colours::white);

			g.drawRect(mouse_current_position.getX(), plot_actual_region.getY(), mod_crosshair_thickness, plot_actual_region.getHeight(), 1.0);

			g.restoreState();

		}

		else {};

		//==============================================================================

		endofpaint: //painting will jump to here if sample rate is incorrect

		repaint_active = false;

	}

    void resized() override
    {
		plot_outline = getLocalBounds();

		plot_clip_region.setBounds(plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.1, 
			
			plot_outline.getWidth()*0.85, plot_outline.getHeight()*0.8);

		sample_rate_incorrect_warning_label.setBounds(plot_clip_region.reduced(plot_clip_region.getHeight()*0.25));
		sample_rate_incorrect_warning_label.setFont(plot_clip_region.getHeight()*0.075);
		
		top_title_region.setBounds(0, 0, plot_outline.getWidth(), plot_outline.getHeight()*0.05);

		mod_region.setBounds(top_title_region.getX() + top_title_region.getWidth() / 2 - top_title_region.getWidth()*0.15, 
			
			top_title_region.getY(), top_title_region.getWidth()*0.3, top_title_region.getHeight());
				
		mod_region_width = mod_region.getWidth();
		
		frequency_mod_bounds = mod_region.removeFromLeft(mod_region_width*0.2);
		magnitude_mod_bounds = mod_region.removeFromLeft(mod_region_width*0.2);
		phase_mod_bounds = mod_region.removeFromLeft(mod_region_width*0.2);
		coherence_mod_bounds = mod_region.removeFromLeft(mod_region_width*0.2);
		spectrum_mod_bounds = mod_region;

		frequency_mod.setBounds(frequency_mod_bounds);
		magnitude_mod.setBounds(magnitude_mod_bounds);
		phase_mod.setBounds(phase_mod_bounds);
		coherence_mod.setBounds(coherence_mod_bounds);
		spectrum_mod.setBounds(spectrum_mod_bounds);

		frequency_mod.setFont(frequency_mod.getHeight()*0.5);
		magnitude_mod.setFont(frequency_mod.getHeight()*0.5);
		phase_mod.setFont(frequency_mod.getHeight()*0.5);
		coherence_mod.setFont(frequency_mod.getHeight()*0.5);
		spectrum_mod.setFont(frequency_mod.getHeight()*0.5);
		
		top_slider_region.setBounds(plot_outline.getWidth()*0.1, 
			plot_outline.getHeight()*0.05, 
			plot_outline.getWidth()*0.85, 
			plot_outline.getHeight()*0.05);
				
		right_slider_region.setBounds(plot_outline.getWidth()*0.95, 
			plot_outline.getHeight()*0.1, 
			plot_outline.getWidth()*0.05, 
			plot_outline.getHeight()*0.8);
		
		bottom_label_region.setBounds(plot_outline.getWidth()*0.1, 
			plot_outline.getHeight()*0.9, 
			plot_outline.getWidth()*0.85, 
			plot_outline.getHeight()*0.05);
		
		bottom_slider_region.setBounds(plot_outline.getWidth()*0.1, 
			plot_outline.getHeight()*0.95, 
			plot_outline.getWidth()*0.85, 
			plot_outline.getHeight()*0.05);

		left_slider_region.setBounds(0, 
			plot_outline.getHeight()*0.1, 
			plot_outline.getWidth()*0.05,
			plot_outline.getHeight()*0.8);
		
		left_label_region.setBounds(plot_outline.getWidth()*0.05, 
			plot_outline.getHeight()*0.1, 
			plot_outline.getWidth()*0.05, 
			plot_outline.getHeight()*0.8);
		
		bottom_label_clip_region = bottom_label_region.expanded(bottom_label_region.getHeight()*axis_label_scale*0.85, 0);
		left_label_clip_region = left_label_region.expanded(0, bottom_label_region.getHeight()*axis_label_scale*0.5);

		horizontal_zoom_slider.setBounds(bottom_slider_region);
		horizontal_center_slider.setBounds(top_slider_region);
		vertical_zoom_slider.setBounds(left_slider_region);
		vertical_center_slider.setBounds(right_slider_region);

		int button_height = top_title_region.getHeight();

		int button_width = button_height * 1.0; 

		magnitude_visible_button.setBounds(top_title_region.getX(), top_title_region.getY(), button_height, button_width);
		phase_visible_button.setBounds(top_title_region.getX() + button_width, top_title_region.getY(), button_height, button_width);
		coherence_visible_button.setBounds(top_title_region.getX() + button_width*2, top_title_region.getY(), button_height, button_width);
		spectrum_visible_button.setBounds(top_title_region.getX() + button_width * 3, top_title_region.getY(), button_height, button_width);

		magnitude_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 4, 
			top_title_region.getY(), 
			button_height, 
			button_width);

		phase_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 3, 
			top_title_region.getY(), 
			button_height, 
			button_width);

		coherence_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 2, 
			top_title_region.getY(), 
			button_height, 
			button_width);
		
		spectrum_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width, 
			top_title_region.getY(), 
			button_height, 
			button_width);

		set_plot_zoom_center_from_slider();
		
    }
	
	void set_plot_zoom_center_from_slider() {

		plot_horizontal_zoom = horizontal_zoom_slider.getValue();
		plot_vertical_zoom = vertical_zoom_slider.getValue();
		plot_actual_region.setSize(plot_clip_region.getWidth()*plot_horizontal_zoom, plot_clip_region.getHeight()*plot_vertical_zoom);

		plot_horizontal_center_offset = ((horizontal_center_slider.getValue())*(plot_actual_region.getWidth() - plot_clip_region.getWidth())*0.5);
		plot_vertical_center_offset = ((vertical_center_slider.getValue())*(plot_actual_region.getHeight() - plot_clip_region.getHeight())*0.5);
		plot_actual_region.setCentre(plot_clip_region.getCentreX() + plot_horizontal_center_offset, 
			plot_clip_region.getCentreY() + plot_vertical_center_offset);
				
		repaint();

	}

	void sliderValueChanged(Slider* slider) override
	{
		if (slider == &horizontal_zoom_slider) {
			set_plot_zoom_center_from_slider();
		}

		if (slider == &horizontal_center_slider) {
			set_plot_zoom_center_from_slider();
		}

		if (slider == &vertical_zoom_slider) {
			set_plot_zoom_center_from_slider();
		}

		if (slider == &vertical_center_slider) {
			set_plot_zoom_center_from_slider();
		}

	}

	void buttonClicked(Button* button) override {

		if (button == &magnitude_visible_button)
		{
			
			magnitude_visible_button.setToggleState(!magnitude_visible_button.getToggleState(), dontSendNotification);

			repaint();

		}

		if (button == &phase_visible_button)
		{

			phase_visible_button.setToggleState(!phase_visible_button.getToggleState(), dontSendNotification);

			repaint();

		}

		if (button == &coherence_visible_button)
		{

			coherence_visible_button.setToggleState(!coherence_visible_button.getToggleState(), dontSendNotification);

			repaint();

		}

		if (button == &spectrum_visible_button)
		{

			spectrum_visible_button.setToggleState(!spectrum_visible_button.getToggleState(), dontSendNotification);

			repaint();

		}

		if (button == &magnitude_active_button)
		{

			magnitude_active_button.setToggleState(1, dontSendNotification);
			phase_active_button.setToggleState(0, dontSendNotification);
			coherence_active_button.setToggleState(0, dontSendNotification);
			spectrum_active_button.setToggleState(0, dontSendNotification);

			repaint();

		}

		if (button == &phase_active_button)
		{

			magnitude_active_button.setToggleState(0, dontSendNotification);
			phase_active_button.setToggleState(1, dontSendNotification);
			coherence_active_button.setToggleState(0, dontSendNotification);
			spectrum_active_button.setToggleState(0, dontSendNotification);

			repaint();

		}

		if (button == &coherence_active_button)
		{

			magnitude_active_button.setToggleState(0, dontSendNotification);
			phase_active_button.setToggleState(0, dontSendNotification);
			coherence_active_button.setToggleState(1, dontSendNotification);
			spectrum_active_button.setToggleState(0, dontSendNotification);

			repaint();

		}

		if (button == &spectrum_active_button)
		{

			magnitude_active_button.setToggleState(0, dontSendNotification);
			phase_active_button.setToggleState(0, dontSendNotification);
			coherence_active_button.setToggleState(0, dontSendNotification);
			spectrum_active_button.setToggleState(1, dontSendNotification);

			repaint();

		}

}

	void try_repaint()

	{

		if (repaint_active == false) { //don't try to call repaint again if there's already a repaint cycle in progress

			repaint();

		}

	}

private:

	Slider horizontal_zoom_slider;
	Slider horizontal_center_slider;
	Slider vertical_zoom_slider;
	Slider vertical_center_slider;

	TextButton magnitude_visible_button;
	TextButton magnitude_active_button;

	TextButton phase_visible_button;
	TextButton phase_active_button;

	TextButton coherence_visible_button;
	TextButton coherence_active_button;

	TextButton spectrum_visible_button;
	TextButton spectrum_active_button;

	Label frequency_mod;
	Label magnitude_mod;
	Label phase_mod;
	Label coherence_mod;
	Label spectrum_mod;

	Point<int> mouse_current_position;

	Label sample_rate_incorrect_warning_label;

	void calc_display_values(std::vector<double> &display_value_vector, std::vector<double> &current_value_vector, int plot_type) {

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

	void update_mod_values() {

		frequency_mod_value = mouse_x_to_freq(mouse_current_position.getX());

		if (frequency_mod_value < 20) {

			frequency_mod_value = 20;
			
		}

		else if (frequency_mod_value > 20000) {

			frequency_mod_value = 20000;

		}

		else {};

		frequency_mod_value_string = String(frequency_mod_value);
		frequency_mod_value_string += " Hz";

		magnitude_mod_value = frequency_to_value(frequency_mod_value, 
			current_composite_fft_bin_frequencies, 
			current_composite_xfer_function_mag_dB);
		magnitude_mod_value_string = String(magnitude_mod_value);
		magnitude_mod_value_string = magnitude_mod_value_string.substring(0, magnitude_mod_value_string.indexOfChar('.') + 2);
		magnitude_mod_value_string += " dB";

		phase_mod_value = frequency_to_value(frequency_mod_value, 
			current_composite_fft_bin_frequencies, 
			current_composite_xfer_function_phase_deg);
		phase_mod_value_string = String(phase_mod_value);
		phase_mod_value_string = phase_mod_value_string.substring(0, phase_mod_value_string.indexOfChar('.'));
		phase_mod_value_string += " Deg";

		coherence_mod_value = frequency_to_value(frequency_mod_value, 
			current_composite_fft_bin_frequencies, 
			current_composite_coherence_value);
		coherence_mod_value_string = String(coherence_mod_value);
		coherence_mod_value_string = coherence_mod_value_string.substring(0, coherence_mod_value_string.indexOfChar('.') + 3);

		spectrum_mod_value = frequency_to_value(frequency_mod_value, 
			current_composite_fft_bin_frequencies, 
			current_composite_system_spectrum_mag_dB);
		spectrum_mod_value_string = String(spectrum_mod_value);
		spectrum_mod_value_string = spectrum_mod_value_string.substring(0, spectrum_mod_value_string.indexOfChar('.'));
		spectrum_mod_value_string += " dB";

	}
	
	float freq_to_normalized_distance(double freq) {
		
		double raw_distance = log10(freq);

		double zeroed_distance = raw_distance - log10(grid_frequencies[0]);

		double normalized_distance = zeroed_distance / 3;	// the zeroed distance at 20 kHz is 3, which is the largest distance. 
															//This put all distance values on the scale of 0 to 1

		return normalized_distance;

	}

	int mouse_x_to_freq(int mouse_x) {

		mouse_x -= plot_actual_region.getX() - getX(); //mouse_x argument is relative to component, not the actuat plot region. This compensates for that. 

		double normalized_distance = mouse_x*1.0 / plot_actual_region.getWidth();
	
		double zeroed_distance = normalized_distance * 3;

		double raw_distance = zeroed_distance + log10(grid_frequencies[0]);

		return pow(10, raw_distance);
	
	}

	float amp_to_y(double amp) {
		double amp_percentage_y = 0.50 + (amp * (-1.0/48)); //0dB on amplitude trace is at 50% of plot component height. 6dB of height on amplitude trace is 10% of plot component height
		return amp_percentage_y;
	}

	float phase_to_y(double phase) {
		double phase_percentage_y = 0.50 + (phase * (-1.0 / 360)); //0 deg on phase trace is at 50% of plot component height. 45 deg of height on phase trace is 10% of plot component height
		return phase_percentage_y;
	}

	float spectrum_mag_to_y(double spectrum_mag) {
		double spectrum_mag_percentage_y = -(spectrum_mag * (0.01 / 0.96)); 
		return spectrum_mag_percentage_y;
	}

	double frequency_to_value(int specified_frequency, std::vector<double> &frequency_vector, std::vector<double> &value_vector) {

		double x0 = 0;
		double x1 = 0;
		double y0 = 0;
		double y1 = 0;
		
		int index = 0;

		while (true) {

			if (specified_frequency > frequency_vector[index]) {

				index++;

			}

			else { break; }

		}

		x0 = frequency_vector[index-1];
		x1 = frequency_vector[index];

		y0 = value_vector[index - 1];
		y1 = value_vector[index];

		return y0 + (specified_frequency - x0)*((y1 - y0) / (x1 - x0));

	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (plots)
};
