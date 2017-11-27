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

	//Rectangle<int> load_indicator_region;
	//Rectangle<int> ui_load_label_outline;
	//Rectangle<int> analyser_load_label_outline;

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

	double current_composite_fft_bin_frequencies[162] = { 0.0 };

	double current_composite_xfer_function_mag_dB[162] = { 0.0 };
	double display_composite_xfer_function_mag_dB[162] = { 0.0 };

	double current_composite_xfer_function_phase_deg[162] = { 0.0 };
	double display_composite_xfer_function_phase_deg[162] = { 0.0 };

	double current_composite_coherence_value[162] = { 0.0 };
	double display_composite_coherence_value[162] = { 0.0 };
	
	double system_spectrum_bin_frequencies[spectrum_fft_bins] = { 0.0 };
	double current_system_spectrum_mag_db[spectrum_fft_bins] = { 0.0 };
	double display_system_spectrum_mag_db[spectrum_fft_bins] = { 0.0 };

	double saved_fft_bin_frequencies[162] = { 0.0 };
	double saved_xfer_function_mag_dB_avg[162] = { 0.0 };
	double saved_xfer_function_phase_deg_avg[162] = { 0.0 };
	double saved_coherence_value[162] = { 0.0 };

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

	//int ui_idle_time = 100; //in ms

	//int analyser_idle_time = 100; //in ms

	//String ui_idle_time_string;

	//String analyser_idle_time_string;
	
    plots()
    {
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

		magnitude_visible_button.setButtonText("M");
		magnitude_active_button.setButtonText("M");

		phase_visible_button.setButtonText("P");
		phase_active_button.setButtonText("P");

		coherence_visible_button.setButtonText("C");
		coherence_active_button.setButtonText("C");

		spectrum_visible_button.setButtonText("S");
		spectrum_active_button.setButtonText("S");

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

    }

    ~plots()
    {
    }

    void paint (Graphics& g) override
	{
		
		repaint_active = true;

		Colour very_dark_grey(20, 20, 20);
		g.fillAll(very_dark_grey);

		g.setColour(Colours::white);
		
		
		/*g.drawRect(ui_load_label_outline, 1);
		g.drawRect(analyser_load_label_outline, 1);

		g.setFont(ui_load_label_outline.getHeight()*0.8);
		
		ui_idle_time_string = "UI Idle: ";
		ui_idle_time_string += String(ui_idle_time);
		ui_idle_time_string += " ms";
		g.drawFittedText(ui_idle_time_string, ui_load_label_outline, Justification::centred, 1, 0);

		analyser_idle_time_string = "FFT Idle: ";
		analyser_idle_time_string += String(analyser_idle_time);
		analyser_idle_time_string += " ms";
		g.drawFittedText(analyser_idle_time_string, analyser_load_label_outline, Justification::centred, 1, 0);*/
		
		calc_display_values();
		
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

		//==============================================================================

		g.saveState();
		
		g.reduceClipRegion(plot_clip_region);

		g.setColour(Colours::black);
		
		g.fillRect(plot_actual_region);

		g.restoreState();

		g.setColour(Colours::white);
		
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

				plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*amp_to_y(display_composite_xfer_function_mag_dB[0]));


			for (int x = 1; x < 162; x++) {

				mag_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[x]),

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

				plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*phase_to_y(display_composite_xfer_function_phase_deg[0]));


			for (int x = 1; x < 162; x++) {

				phase_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*phase_to_y(display_composite_xfer_function_phase_deg[x]));

			}

			phase_smoothed_trace = phase_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::red);

			g.strokePath(phase_smoothed_trace, PathStrokeType(live_trace_thickness));

		};

		if (coherence_visible == 1) {

			Path coh_trace;
			Path coh_smoothed_trace;

			coh_trace.startNewSubPath(

				plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[0]),

				plot_actual_region_y + plot_actual_region_height*(1.0-display_composite_coherence_value[0]));

			for (int x = 1; x < 162; x++) {

				coh_trace.lineTo(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(current_composite_fft_bin_frequencies[x]),

					plot_actual_region_y + plot_actual_region_height*(1.0-display_composite_coherence_value[x]));

			}

			coh_smoothed_trace = coh_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

			g.setColour(Colours::blue);

			g.strokePath(coh_smoothed_trace, PathStrokeType(live_trace_thickness));

		}

		if (spectrum_visible == 1) {

			g.setColour(Colours::magenta);

			int start_x = 0;
			int start_y = 0;
			int end_x = 0;
			int end_y = 0;

			for (int x = 1; x < spectrum_fft_bins; x++) { //start at index 1, as fft bin zero just has DC offset

				start_x = plot_actual_region_x + plot_actual_region_width*freq_to_x(system_spectrum_bin_frequencies[x]);
				start_y = plot_actual_region_y + plot_actual_region_height*spectrum_mag_to_y(display_system_spectrum_mag_db[x]);
				end_x = plot_actual_region_x + plot_actual_region_width*freq_to_x(system_spectrum_bin_frequencies[x+1]);
				end_y = plot_actual_region_y + plot_actual_region_height*spectrum_mag_to_y(display_system_spectrum_mag_db[x+1]);

				g.drawLine(start_x, start_y, end_x, end_y, live_trace_thickness);

			}

		};

		if (saved_traces_visible == 1) {

			if (magnitude_visible == 1) {

				Path mag_trace;
				Path mag_smoothed_trace;

				mag_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*amp_to_y(saved_xfer_function_mag_dB_avg[0]));


				for (int x = 1; x < 162; x++) {

					mag_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*amp_to_y(saved_xfer_function_mag_dB_avg[x]));

				}

				mag_smoothed_trace = mag_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(0, 80, 0));

				g.strokePath(mag_smoothed_trace, PathStrokeType(saved_trace_thickness));

			};

			if (phase_visible == 1) {

				Path phase_trace;
				Path phase_smoothed_trace;

				phase_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*phase_to_y(saved_xfer_function_phase_deg_avg[0]));


				for (int x = 1; x < 162; x++) {

					phase_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*phase_to_y(saved_xfer_function_phase_deg_avg[x]));

				}

				phase_smoothed_trace = phase_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(80, 0, 0));

				g.strokePath(phase_smoothed_trace, PathStrokeType(saved_trace_thickness));

			};

			if (coherence_visible == 1) {

				Path coh_trace;
				Path coh_smoothed_trace;

				coh_trace.startNewSubPath(

					plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[0]),

					plot_actual_region_y + plot_actual_region_height*(1.0 - saved_coherence_value[0]));

				for (int x = 1; x < 162; x++) {

					coh_trace.lineTo(

						plot_actual_region_x + plot_actual_region_width*freq_to_x(saved_fft_bin_frequencies[x]),

						plot_actual_region_y + plot_actual_region_height*(1.0 - saved_coherence_value[x]));

				}

				coh_smoothed_trace = coh_trace.createPathWithRoundedCorners(trace_path_smoothing_radius);

				g.setColour(Colour(0, 0, 80));

				g.strokePath(coh_smoothed_trace, PathStrokeType(saved_trace_thickness));

			}

		}
		
		g.restoreState();

		repaint_active = false;

	}

    void resized() override
    {
		plot_outline = getLocalBounds();

		plot_clip_region.setBounds(plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.1, plot_outline.getWidth()*0.85, plot_outline.getHeight()*0.8);
		
		top_title_region.setBounds(0, 0, plot_outline.getWidth(), plot_outline.getHeight()*0.05);
		top_slider_region.setBounds(plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.05, plot_outline.getWidth()*0.85, plot_outline.getHeight()*0.05);
				
		right_slider_region.setBounds(plot_outline.getWidth()*0.95, plot_outline.getHeight()*0.1, plot_outline.getWidth()*0.05, plot_outline.getHeight()*0.8);
		
		bottom_label_region.setBounds(plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.9, plot_outline.getWidth()*0.85, plot_outline.getHeight()*0.05);
		bottom_slider_region.setBounds(plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.95, plot_outline.getWidth()*0.85, plot_outline.getHeight()*0.05);

		left_slider_region.setBounds(0, plot_outline.getHeight()*0.1, plot_outline.getWidth()*0.05, plot_outline.getHeight()*0.8);
		left_label_region.setBounds(plot_outline.getWidth()*0.05, plot_outline.getHeight()*0.1, plot_outline.getWidth()*0.05, plot_outline.getHeight()*0.8);
		
		bottom_label_clip_region = bottom_label_region.expanded(bottom_label_region.getHeight()*axis_label_scale*0.85, 0);
		left_label_clip_region = left_label_region.expanded(0, bottom_label_region.getHeight()*axis_label_scale*0.5);

		/*load_indicator_region.setBounds(0, plot_outline.getHeight()*0.95, plot_outline.getWidth()*0.1, plot_outline.getHeight()*0.05);
		ui_load_label_outline = load_indicator_region.removeFromTop(load_indicator_region.getHeight()*0.5);
		analyser_load_label_outline = load_indicator_region;*/

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

		magnitude_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 4, top_title_region.getY(), button_height, button_width);
		phase_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 3, top_title_region.getY(), button_height, button_width);
		coherence_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width * 2, top_title_region.getY(), button_height, button_width);
		spectrum_active_button.setBounds(top_title_region.getX() + top_title_region.getWidth() - button_width, top_title_region.getY(), button_height, button_width);

		set_plot_zoom_center_from_slider();
		
    }
	
	void set_plot_zoom_center_from_slider() {

		plot_horizontal_zoom = horizontal_zoom_slider.getValue();
		plot_vertical_zoom = vertical_zoom_slider.getValue();
		plot_actual_region.setSize(plot_clip_region.getWidth()*plot_horizontal_zoom, plot_clip_region.getHeight()*plot_vertical_zoom);

		plot_horizontal_center_offset = ((horizontal_center_slider.getValue())*(plot_actual_region.getWidth() - plot_clip_region.getWidth())*0.5);
		plot_vertical_center_offset = ((vertical_center_slider.getValue())*(plot_actual_region.getHeight() - plot_clip_region.getHeight())*0.5);
		plot_actual_region.setCentre(plot_clip_region.getCentreX() + plot_horizontal_center_offset, plot_clip_region.getCentreY() + plot_vertical_center_offset);
				
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
			if (magnitude_visible == 1) {
				magnitude_visible = 0;
				repaint();
			}

			else {
				magnitude_visible = 1;
				repaint();
			}
		}

		if (button == &phase_visible_button)
		{
			if (phase_visible == 1) {
				phase_visible = 0;
				repaint();
			}

			else {
				phase_visible = 1;
				repaint();
			}
		}

		if (button == &coherence_visible_button)
		{
			if (coherence_visible == 1) {
				coherence_visible = 0;
				repaint();
			}

			else {
				coherence_visible = 1;
				repaint();
			}
		}

		if (button == &spectrum_visible_button)
		{
			if (spectrum_visible == 1) {
				spectrum_visible = 0;
				repaint();
			}

			else {
				spectrum_visible = 1;
				repaint();
			}
		}

		if (button == &magnitude_active_button)
		{
			active_grid_mode = 0;
			repaint();
		}

		if (button == &phase_active_button)
		{
			active_grid_mode = 1;
			repaint();
		}

		if (button == &coherence_active_button)
		{
			active_grid_mode = 2;
			repaint();
		}

		if (button == &spectrum_active_button)
		{
			active_grid_mode = 3;
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

	void calc_display_values() {

		std::vector<double *> mag_arrays = {display_composite_xfer_function_mag_dB, current_composite_xfer_function_mag_dB};
		std::vector<double *> phase_arrays = { display_composite_xfer_function_phase_deg, current_composite_xfer_function_phase_deg };
		std::vector<double *> coherence_arrays = { display_composite_coherence_value, current_composite_coherence_value };
		std::vector<double *> spectrum_arrays = { display_system_spectrum_mag_db, current_system_spectrum_mag_db };

		std::vector<std::vector<double *>> array_types = { mag_arrays ,phase_arrays, coherence_arrays, spectrum_arrays};

		std::vector<double *> snap_ranges = { &magnitude_snap_range, &phase_snap_range, &coherence_snap_range, &spectrum_snap_range };
		std::vector<double *> rates_of_change = { &magnitude_rate_of_change, &phase_rate_of_change, &coherence_rate_of_change, &spectrum_rate_of_change};

		for (int x = 0; x < 162; x++) {

			for (int type = 0; type < 3; type++) {
										
				if ((array_types[type][0][x] - array_types[type][1][x]) > *snap_ranges[type]) {

					array_types[type][0][x] =

						array_types[type][0][x] -

						(abs(array_types[type][0][x] - array_types[type][1][x]) * *rates_of_change[type]);

				}

				if ((array_types[type][0][x] - array_types[type][1][x]) < -*snap_ranges[type]) {

					array_types[type][0][x] =

						array_types[type][0][x] +

						(abs(array_types[type][0][x] - array_types[type][1][x])* *rates_of_change[type]);

				}

				if (abs(array_types[type][0][x] - array_types[type][1][x]) <= *snap_ranges[type]) {

					array_types[type][0][x] = array_types[type][1][x];

				}

			}
			
		}

		for (int x = 0; x < spectrum_fft_bins; x++) {

				if ((display_system_spectrum_mag_db[x] - current_system_spectrum_mag_db[x]) > *snap_ranges[3]) {

					display_system_spectrum_mag_db[x] =

						display_system_spectrum_mag_db[x] -

						(abs(display_system_spectrum_mag_db[x] - current_system_spectrum_mag_db[x]) * *rates_of_change[3]);

				}

				if ((display_system_spectrum_mag_db[x] - current_system_spectrum_mag_db[x]) < -*snap_ranges[3]) {

					display_system_spectrum_mag_db[x] =

						display_system_spectrum_mag_db[x] +

						(abs(display_system_spectrum_mag_db[x] - current_system_spectrum_mag_db[x])* *rates_of_change[3]);

				}

				if (abs(display_system_spectrum_mag_db[x] - current_system_spectrum_mag_db[x]) <= *snap_ranges[3]) {

					display_system_spectrum_mag_db[x] = current_system_spectrum_mag_db[x];

				}

			}

	}
	
	float freq_to_x(double freq) {
		
		double raw_distance = log10(freq);

		double zeroed_distance = raw_distance - log10(grid_frequencies[0]);

		double normalized_distance = zeroed_distance / 3;	// the zeroed distance at 20 kHz is 3, which is the largest distance. 
															//This put all distance values on the scale of 0 to 1

		return normalized_distance;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (plots)
};
