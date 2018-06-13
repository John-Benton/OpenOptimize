/*
  ==============================================================================

    controls.h
    Created: 23 Sep 2017 2:21:19pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <string>
#include <fstream>
#include <vector>
#include "constants.h"

//==============================================================================
/*
*/
class controls    : public constants, public Component, public Button::Listener, public Slider::Listener
{
public:

	Rectangle<int> controls_outline;
	
	Rectangle<int> left_column;
	Rectangle<int> center_column;
	Rectangle<int> right_column;

	Rectangle<int> left_column_row_1;
	Rectangle<int> left_column_row_1_active;
	Rectangle<int> left_column_row_2;
	Rectangle<int> left_column_row_2_active;
	Rectangle<int> left_column_row_2_active_left;
	Rectangle<int> left_column_row_2_active_right;
	Rectangle<int> left_column_row_3;
	Rectangle<int> left_column_row_3_active;
	Rectangle<int> left_column_row_3_active_left;
	Rectangle<int> left_column_row_3_active_right;
	Rectangle<int> left_column_row_4;
	Rectangle<int> left_column_row_4_active;

	Rectangle<int> center_column_row_1;
	Rectangle<int> center_column_row_1_active;
	Rectangle<int> center_column_row_2;
	Rectangle<int> center_column_row_2_active;
	Rectangle<int> center_column_row_3;
	Rectangle<int> center_column_row_3_active;
	Rectangle<int> center_column_row_4;
	Rectangle<int> center_column_row_4_active;

	Rectangle<int> right_column_row_1;
	Rectangle<int> right_column_row_1_active;
	Rectangle<int> right_column_row_2;
	Rectangle<int> right_column_row_2_active;
	Rectangle<int> right_column_row_2_active_far_left;
	Rectangle<int> right_column_row_2_active_middle;
	Rectangle<int> right_column_row_2_active_far_right;
	Rectangle<int> right_column_row_3;
	Rectangle<int> right_column_row_3_active;
	Rectangle<int> right_column_row_3_active_left;
	Rectangle<int> right_column_row_3_active_right;
	Rectangle<int> right_column_row_4;
	Rectangle<int> right_column_row_4_left;
	Rectangle<int> right_column_row_4_right;
	Rectangle<int> right_column_row_4_left_active;
	Rectangle<int> right_column_row_4_right_active;
			
	int analysis_status = 0; //0 is stopped, 1 is running

	int saved_traces_visible = 0; //0 is non-visible, 1 is visible

	int curves_only = 0; //0 is regular mode, 1 is curves only mode

	//==========//

	String raw_mic_cal_file_path;

	String mic_cal_file_name;

	std::string fixed_mic_cal_file_path;

	//==========//

	String raw_system_curve_file_path;

	String system_curve_file_name;

	std::string fixed_system_curve_file_path;

	//==========//

	String raw_saved_traces_save_path;

	std::string fixed_saved_traces_save_path;

	//==========//

	String raw_saved_traces_load_path;

	String saved_traces_file_name;

	std::string fixed_saved_traces_load_path;

	//==========//

	int refresh_rate_slider_value = 1;

	int smoothing_slider_value = 0;

	std::vector<double> composite_fft_bin_frequencies_for_save;
	std::vector<double> composite_xfer_function_mag_dB_avg_cal_for_save;
	std::vector<double> composite_xfer_function_phase_deg_avg_for_save;
	std::vector<double> composite_coherence_value_for_save;

	std::vector<double> loaded_composite_fft_bin_frequencies;
	std::vector<double> loaded_composite_xfer_function_mag_dB_avg_cal;
	std::vector<double> loaded_composite_xfer_function_phase_deg_avg;
	std::vector<double> loaded_composite_coherence_value;
	
	int open_audio_io_config_window = 0;

	float active_horizontal_padding_factor = 0.025;

	float active_vertical_padding_factor = 0.1;
		
    controls()
    {

		addAndMakeVisible(analysis_onoff_button);
		analysis_onoff_button.setButtonText("Analyser Run");
		analysis_onoff_button.setClickingTogglesState(true);
		analysis_onoff_button.addListener(this);
		analysis_onoff_button.setColour(analysis_onoff_button.buttonColourId, Colours::transparentBlack);
		analysis_onoff_button.setColour(analysis_onoff_button.buttonOnColourId, Colour(160,79,0));

		addAndMakeVisible(mic_cal_filename_label);
		mic_cal_filename_label.setColour(Label::backgroundColourId, Colours::black);
		
		addAndMakeVisible(choose_mic_cal_button);
		choose_mic_cal_button.setButtonText("Mic Cal");
		choose_mic_cal_button.addListener(this);
		choose_mic_cal_button.setColour(choose_mic_cal_button.buttonColourId, Colours::transparentBlack);

		addAndMakeVisible(system_curve_filename_label);
		system_curve_filename_label.setColour(Label::backgroundColourId, Colours::black);

		addAndMakeVisible(choose_system_curve_button);
		choose_system_curve_button.setButtonText("System Curve");
		choose_system_curve_button.addListener(this);
		choose_system_curve_button.setColour(choose_system_curve_button.buttonColourId, Colours::transparentBlack);

		addAndMakeVisible(refresh_rate_slider);
		refresh_rate_slider.setRange(1, 20, 1);
		refresh_rate_slider.addListener(this);

		addAndMakeVisible(smoothing_slider);
		smoothing_slider.setRange(0, 4, 1);
		smoothing_slider.addListener(this);

		addAndMakeVisible(capture_traces_button);
		capture_traces_button.setButtonText("Capture");
		capture_traces_button.addListener(this);
		capture_traces_button.setColour(capture_traces_button.buttonColourId, Colours::transparentBlack);

		addAndMakeVisible(load_saved_traces_button);
		load_saved_traces_button.setButtonText("Load");
		load_saved_traces_button.addListener(this);
		load_saved_traces_button.setColour(load_saved_traces_button.buttonColourId, Colours::transparentBlack);

		addAndMakeVisible(saved_traces_visible_onoff_button);
		saved_traces_visible_onoff_button.setButtonText("Visible");
		saved_traces_visible_onoff_button.setClickingTogglesState(true);
		saved_traces_visible_onoff_button.addListener(this);
		saved_traces_visible_onoff_button.setColour(saved_traces_visible_onoff_button.buttonColourId, Colours::transparentBlack);
		saved_traces_visible_onoff_button.setColour(saved_traces_visible_onoff_button.buttonOnColourId, Colour(160, 79, 0));
		
		addAndMakeVisible(saved_traces_filename_label);
		saved_traces_filename_label.setColour(Label::backgroundColourId, Colours::black);

		addAndMakeVisible(config_audio_io_button);
		config_audio_io_button.setButtonText("Audio I/O Setup");
		config_audio_io_button.addListener(this);
		config_audio_io_button.setColour(config_audio_io_button.buttonColourId, Colours::transparentBlack);

		composite_fft_bin_frequencies_for_save.resize(composite_fft_bins);
		composite_xfer_function_mag_dB_avg_cal_for_save.resize(composite_fft_bins);
		composite_xfer_function_phase_deg_avg_for_save.resize(composite_fft_bins);
		composite_coherence_value_for_save.resize(composite_fft_bins);

		loaded_composite_fft_bin_frequencies.resize(composite_fft_bins);
		loaded_composite_xfer_function_mag_dB_avg_cal.resize(composite_fft_bins);
		loaded_composite_xfer_function_phase_deg_avg.resize(composite_fft_bins);
		loaded_composite_coherence_value.resize(composite_fft_bins);

		addAndMakeVisible(curves_only_button);
		curves_only_button.setClickingTogglesState(true);
		curves_only_button.setButtonText("Curves Only");
		curves_only_button.addListener(this);
		curves_only_button.setColour(curves_only_button.buttonColourId, Colours::transparentBlack);
		curves_only_button.setColour(curves_only_button.buttonOnColourId, Colour(160, 79, 0));

		addAndMakeVisible(ir_window_button);
		ir_window_button.setButtonText("IR Window");
		ir_window_button.addListener(this);
		ir_window_button.setColour(ir_window_button.buttonColourId, Colours::transparentBlack);
		
    }

    ~controls()
    {
	
	}

    void paint (Graphics& g) override
    {
		g.setColour (Colours::black);

		g.drawRect (getLocalBounds(), 1.0);

		g.setColour(Colours::white);

		//g.drawRect(left_column, 1);
		//g.drawRect(left_column_row_1, 1);
		//g.drawRect(left_column_row_2, 1);
		//g.drawRect(left_column_row_3, 1);
		//g.drawRect(left_column_row_4, 1);

		//g.drawRect(center_column, 1);
		//g.drawRect(center_column_row_1, 1);
		//g.drawRect(center_column_row_2, 1);
		//g.drawRect(center_column_row_3, 1);
		//g.drawRect(center_column_row_4, 1);
		//
		//g.drawRect(right_column, 1);
		//g.drawRect(right_column_row_1, 1);
		//g.drawRect(right_column_row_2, 1);
		//g.drawRect(right_column_row_3, 1);
		//g.drawRect(right_column_row_4, 1);

		//g.drawRect(right_column_row_4, 1.0);
		//g.drawRect(right_column_row_4_left, 1.0);
		//g.drawRect(right_column_row_4_right, 1.0);
		//g.drawRect(right_column_row_4_left_active, 1.0);
		//g.drawRect(right_column_row_4_right_active, 1.0);

		mic_cal_filename_label.setFont(left_column_row_1.getHeight()*0.8);

		mic_cal_filename_label.setText(mic_cal_file_name, dontSendNotification);

		system_curve_filename_label.setFont(left_column_row_1.getHeight()*0.8);

		system_curve_filename_label.setText(system_curve_file_name, dontSendNotification);

		saved_traces_filename_label.setFont(left_column_row_1.getHeight()*0.8);

		saved_traces_filename_label.setText(saved_traces_file_name, dontSendNotification);

		g.setFont(left_column_row_1.getHeight()*0.8);

		g.drawFittedText("Update Rate (Hz)",
			center_column_row_1.getX(),
			center_column_row_1.getY(),
			center_column_row_1.getWidth(),
			center_column_row_1.getHeight(),
			Justification::centred, 0, 1.0f);

		g.drawFittedText("Smoothing Level",
			center_column_row_3.getX(),
			center_column_row_3.getY(),
			center_column_row_3.getWidth(),
			center_column_row_3.getHeight(),
			Justification::centred, 0, 1.0f);

		g.drawFittedText("Saved Traces",
			right_column_row_1.getX(),
			right_column_row_1.getY(),
			right_column_row_1.getWidth(),
			right_column_row_1.getHeight(),
			Justification::centred, 0, 1.0f);
		
    }

    void resized() override
  	{
		
		controls_outline = getLocalBounds();

		int controls_width = controls_outline.getWidth();
		int controls_height = controls_outline.getHeight();

		//
		
		left_column = controls_outline.removeFromLeft(controls_width * (1.0/3.0));

		left_column_row_1 = left_column.removeFromTop(controls_height * 0.25);
		int active_horizontal_padding_pixels = left_column_row_1.getWidth()*active_horizontal_padding_factor;
		int active_vertical_padding_pixels = left_column_row_1.getHeight()*active_vertical_padding_factor;
		left_column_row_1_active = left_column_row_1.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);
		
		left_column_row_2 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_2_active = left_column_row_2.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);
		left_column_row_2_active_left = left_column_row_2_active.removeFromLeft(left_column_row_2_active.getWidth() * 0.6);
		left_column_row_2_active_right = left_column_row_2_active;
		
		left_column_row_3 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_3_active = left_column_row_3.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);
		left_column_row_3_active_left = left_column_row_3_active.removeFromLeft(left_column_row_3_active.getWidth() * 0.6);
		left_column_row_3_active_right = left_column_row_3_active;

		left_column_row_4 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_4_active = left_column_row_4.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		//

		center_column = controls_outline.removeFromLeft(controls_width * (1.0 / 3.0));

		center_column_row_1 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_1_active = center_column_row_1.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		center_column_row_2 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_2_active = center_column_row_2.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		center_column_row_3 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_3_active = center_column_row_3.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		center_column_row_4 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_4_active = center_column_row_4.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		//

		right_column = controls_outline.removeFromLeft(controls_width * (1.0 / 3.0));

		right_column_row_1 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_1_active = right_column_row_1.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		right_column_row_2 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_2_active = right_column_row_2.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);
		int right_column_row_2_active_width = right_column_row_2_active.getWidth();
		right_column_row_2_active_far_left = right_column_row_2_active.removeFromLeft(right_column_row_2_active_width * 0.33);
		right_column_row_2_active_middle = right_column_row_2_active.removeFromLeft(right_column_row_2_active_width * 0.33);
		right_column_row_2_active_far_right = right_column_row_2_active;

		right_column_row_3 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_3_active = right_column_row_3.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		right_column_row_4 = right_column;
		right_column_row_4_left = right_column_row_4.removeFromLeft(right_column_row_4.getWidth()*0.5);
		right_column_row_4_right = right_column_row_4;
		right_column_row_4_left_active = right_column_row_4_left.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);
		right_column_row_4_right_active = right_column_row_4_right.reduced(active_horizontal_padding_pixels, active_vertical_padding_pixels);

		
		//

		analysis_onoff_button.setBounds(left_column_row_1_active);
		
		mic_cal_filename_label.setBounds(left_column_row_2_active_left);

		choose_mic_cal_button.setBounds(left_column_row_2_active_right);

		system_curve_filename_label.setBounds(left_column_row_3_active_left);

		choose_system_curve_button.setBounds(left_column_row_3_active_right);
		
		config_audio_io_button.setBounds(left_column_row_4_active);
		
		refresh_rate_slider.setBounds(center_column_row_2_active);

		smoothing_slider.setBounds(center_column_row_4_active);

		capture_traces_button.setBounds(right_column_row_2_active_far_left);

		load_saved_traces_button.setBounds(right_column_row_2_active_middle);

		saved_traces_visible_onoff_button.setBounds(right_column_row_2_active_far_right);

		saved_traces_filename_label.setBounds(right_column_row_3_active);

		curves_only_button.setBounds(right_column_row_4_left_active);

		ir_window_button.setBounds(right_column_row_4_right_active);
    
	}

	void buttonClicked(Button* button) override {
		
		analysis_status = analysis_onoff_button.getToggleState();

		saved_traces_visible = saved_traces_visible_onoff_button.getToggleState();

		curves_only = curves_only_button.getToggleState();
		
		if (button == &choose_mic_cal_button)
	
		{
			
			FileChooser mic_cal_file("Select Microphone Calibration File", File(), "", true, false);

			mic_cal_file.browseForFileToOpen();

			mic_cal_file_name = mic_cal_file.getResult().getFileName();

			raw_mic_cal_file_path = mic_cal_file.getResult().getFullPathName();

			fixed_mic_cal_file_path = raw_mic_cal_file_path.toStdString();
			
			char bad_char = '\\';
			
			char good_char = '/';
			
			replace(fixed_mic_cal_file_path.begin(), fixed_mic_cal_file_path.end(), bad_char, good_char);

			repaint();
			
		}

		if (button == &choose_system_curve_button)

		{

			FileChooser system_curve_file("Select System Curve File", File(), "", true, false);

			system_curve_file.browseForFileToOpen();

			system_curve_file_name = system_curve_file.getResult().getFileName();

			raw_system_curve_file_path = system_curve_file.getResult().getFullPathName();

			fixed_system_curve_file_path = raw_system_curve_file_path.toStdString();

			char bad_char = '\\';

			char good_char = '/';

			replace(fixed_system_curve_file_path.begin(), fixed_system_curve_file_path.end(), bad_char, good_char);

			repaint();

		}

		if (button == &capture_traces_button)

		{
			
			save_traces_to_file();

		}

		if (button == &load_saved_traces_button)

		{

			load_traces_from_file();

		}

		if (button == &config_audio_io_button)

		{

			open_audio_io_config_window = 1;

		}

	}

	void sliderValueChanged(Slider* slider) override
	{
		refresh_rate_slider_value = refresh_rate_slider.getValue();
		smoothing_slider_value = smoothing_slider.getValue();
	}

	void save_traces_to_file() {

		FileChooser save_traces_path_finder("Select File to Save Traces To", File(), ".txt", true, false);

		save_traces_path_finder.browseForFileToSave(1);

		saved_traces_file_name = save_traces_path_finder.getResult().getFileName();

		raw_saved_traces_save_path = save_traces_path_finder.getResult().getFullPathName();

		fixed_saved_traces_save_path = raw_saved_traces_save_path.toStdString();

		char bad_char = '\\';

		char good_char = '/';

		replace(fixed_saved_traces_save_path.begin(), fixed_saved_traces_save_path.end(), bad_char, good_char);

		std::ofstream save_traces;

		save_traces.open(fixed_saved_traces_save_path);

		for (int x = 0; x < composite_fft_bins; x++) {

			save_traces 
				
				<< composite_fft_bin_frequencies_for_save[x] 
				
				<< "," << composite_xfer_function_mag_dB_avg_cal_for_save[x] 
				
				<< "," << composite_xfer_function_phase_deg_avg_for_save[x] 
				
				<< "," << composite_coherence_value_for_save[x] 
				
				<< std::endl;

		}

		save_traces.close();

		for (int x = 0; x < composite_fft_bins; x++) {

			loaded_composite_fft_bin_frequencies[x] = composite_fft_bin_frequencies_for_save[x];

			loaded_composite_xfer_function_mag_dB_avg_cal[x] = composite_xfer_function_mag_dB_avg_cal_for_save[x];

			loaded_composite_xfer_function_phase_deg_avg[x] = composite_xfer_function_phase_deg_avg_for_save[x];

			loaded_composite_coherence_value[x] = composite_coherence_value_for_save[x];

		}

		saved_traces_visible_onoff_button.setToggleState(true, dontSendNotification);
		saved_traces_visible = 1;

	}

	void load_traces_from_file() {

		std::string current_line;

		FileChooser traces_loader("Select Traces to Load", File(), "", true, false);

		traces_loader.browseForFileToOpen();

		saved_traces_file_name = traces_loader.getResult().getFileName();

		raw_saved_traces_load_path = traces_loader.getResult().getFullPathName();

		fixed_saved_traces_load_path = raw_saved_traces_load_path.toStdString();

		char bad_char = '\\';

		char good_char = '/';

		replace(fixed_saved_traces_load_path.begin(), fixed_saved_traces_load_path.end(), bad_char, good_char);

		std::ifstream load_traces(fixed_saved_traces_load_path);

		for (int x = 0; x < 162; x++) {

			std::string frequency_string;

			std::string amplitude_string;

			std::string phase_string;

			std::string coherence_string;

			std::getline(load_traces, frequency_string, ',');

			loaded_composite_fft_bin_frequencies[x] = stod(frequency_string);

			std::getline(load_traces, amplitude_string, ',');

			loaded_composite_xfer_function_mag_dB_avg_cal[x] = stod(amplitude_string);

			std::getline(load_traces, phase_string, ',');

			loaded_composite_xfer_function_phase_deg_avg[x] = stod(phase_string);

			std::getline(load_traces, coherence_string);

			loaded_composite_coherence_value[x] = stod(coherence_string);

		}

		saved_traces_visible_onoff_button.setToggleState(true, dontSendNotification);
		saved_traces_visible = 1;

	}

private:

	TextButton analysis_onoff_button;

	TextButton choose_mic_cal_button;

	Label mic_cal_filename_label;

	Slider refresh_rate_slider;

	Slider smoothing_slider;

	TextButton choose_system_curve_button;

	Label system_curve_filename_label;

	TextButton capture_traces_button;

	TextButton load_saved_traces_button;

	TextButton saved_traces_visible_onoff_button;

	Label saved_traces_filename_label;

	TextButton config_audio_io_button;

	TextButton curves_only_button;

	TextButton ir_window_button;
					
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (controls)

};
