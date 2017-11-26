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

//==============================================================================
/*
*/
class controls    : public Component, public Button::Listener, public Slider::Listener
{
public:

	Rectangle<int> controls_outline;
	
	Rectangle<int> left_column;
	Rectangle<int> center_column;
	Rectangle<int> right_column;

	Rectangle<int> left_column_row_1;
	Rectangle<int> left_column_row_2;
	Rectangle<int> left_column_row_3;
	Rectangle<int> left_column_row_4;

	Rectangle<int> center_column_row_1;
	Rectangle<int> center_column_row_2;
	Rectangle<int> center_column_row_3;
	Rectangle<int> center_column_row_4;

	Rectangle<int> right_column_row_1;
	Rectangle<int> right_column_row_2;
	Rectangle<int> right_column_row_3;
	Rectangle<int> right_column_row_4;
		
	int analysis_status = 0; //0 is stopped, 1 is running

	int saved_traces_visible = 0; //0 is non-visible, 1 is visible

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

	double newest_fft_bin_frequencies[162] = { 0 };
	double newest_xfer_function_mag_dB_avg_cal[162] = { 0 };
	double newest_xfer_function_phase_deg_avg[162] = { 0 };
	double newest_coherence_value[162] = { 0 };

	double saved_fft_bin_frequencies[162] = { 0 };
	double saved_xfer_function_mag_dB_avg_cal[162] = { 0 };
	double saved_xfer_function_phase_deg_avg[162] = { 0 };
	double saved_coherence_value[162] = { 0 };

	int open_audio_io_config_window = 0;
		
    controls()
    {

		addAndMakeVisible(analysis_onoff_button);
		analysis_onoff_button.setButtonText("Analyser Run");
		analysis_onoff_button.setClickingTogglesState(true);
		analysis_onoff_button.addListener(this);
		analysis_onoff_button.setColour(analysis_onoff_button.buttonOnColourId, Colour(160,79,0));

		addAndMakeVisible(mic_cal_filename_label);
		mic_cal_filename_label.setColour(Label::backgroundColourId, Colours::black);
		
		addAndMakeVisible(choose_mic_cal);
		choose_mic_cal.setButtonText("Mic Cal");
		choose_mic_cal.addListener(this);

		addAndMakeVisible(system_curve_filename_label);
		system_curve_filename_label.setColour(Label::backgroundColourId, Colours::black);

		addAndMakeVisible(choose_system_curve);
		choose_system_curve.setButtonText("System Curve");
		choose_system_curve.addListener(this);

		addAndMakeVisible(refresh_rate_slider);
		refresh_rate_slider.setRange(1, 20, 1);
		refresh_rate_slider.addListener(this);

		addAndMakeVisible(smoothing_slider);
		smoothing_slider.setRange(0, 5, 1);
		smoothing_slider.addListener(this);

		addAndMakeVisible(capture_traces_button);
		capture_traces_button.setButtonText("Capture");
		capture_traces_button.addListener(this);

		addAndMakeVisible(load_saved_traces_button);
		load_saved_traces_button.setButtonText("Load");
		load_saved_traces_button.addListener(this);

		addAndMakeVisible(saved_traces_visible_onoff_button);
		saved_traces_visible_onoff_button.setButtonText("Visible");
		saved_traces_visible_onoff_button.setClickingTogglesState(true);
		saved_traces_visible_onoff_button.addListener(this);
		saved_traces_visible_onoff_button.setColour(saved_traces_visible_onoff_button.buttonOnColourId, Colour(160, 79, 0));
		
		addAndMakeVisible(saved_traces_filename_label);
		saved_traces_filename_label.setColour(Label::backgroundColourId, Colours::black);

		addAndMakeVisible(config_audio_io_button);
		config_audio_io_button.setButtonText("Audio I/O Setup");
		config_audio_io_button.addListener(this);
		
    }

    ~controls()
    {
	
	}

    void paint (Graphics& g) override
    {

		//g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

        //g.setColour (Colours::grey);
        //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (Colours::white);

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

		
		left_column = controls_outline.removeFromLeft(controls_width * (1.0/3.0));
		left_column_row_1 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_2 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_3 = left_column.removeFromTop(controls_height * 0.25);
		left_column_row_4 = left_column.removeFromTop(controls_height * 0.25);


		center_column = controls_outline.removeFromLeft(controls_width * (1.0 / 3.0));
		center_column_row_1 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_2 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_3 = center_column.removeFromTop(controls_height * 0.25);
		center_column_row_4 = center_column.removeFromTop(controls_height * 0.25);


		right_column = controls_outline.removeFromLeft(controls_width * (1.0 / 3.0));
		right_column_row_1 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_2 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_3 = right_column.removeFromTop(controls_height * 0.25);
		right_column_row_4 = right_column.removeFromTop(controls_height * 0.25);

		analysis_onoff_button.setBounds(left_column_row_1.getX(), left_column_row_1.getY(), left_column_row_1.getWidth(), left_column_row_1.getHeight());

		mic_cal_filename_label.setBounds(left_column_row_2.getX(), left_column_row_2.getY(), left_column_row_2.getWidth()*0.60, left_column_row_2.getHeight());

		choose_mic_cal.setBounds(left_column_row_2.getX() + left_column_row_2.getWidth()*0.60, left_column_row_2.getY(), left_column_row_2.getWidth()*0.40, left_column_row_2.getHeight());

		system_curve_filename_label.setBounds(left_column_row_3.getX(), left_column_row_3.getY(), left_column_row_3.getWidth()*0.60, left_column_row_3.getHeight());

		choose_system_curve.setBounds(left_column_row_3.getX() + left_column_row_3.getWidth()*0.60, left_column_row_3.getY(), left_column_row_3.getWidth()*0.40, left_column_row_3.getHeight());

		refresh_rate_slider.setBounds(center_column_row_2.getX(), center_column_row_2.getY(), center_column_row_2.getWidth(), center_column_row_2.getHeight());

		smoothing_slider.setBounds(center_column_row_4.getX(), center_column_row_4.getY(), center_column_row_4.getWidth(), center_column_row_4.getHeight());

		capture_traces_button.setBounds(right_column_row_2.getX(), right_column_row_2.getY(), right_column_row_2.getWidth()*0.33, right_column_row_2.getHeight());

		load_saved_traces_button.setBounds(right_column_row_2.getX() + right_column_row_2.getWidth()*0.33, right_column_row_2.getY(), right_column_row_2.getWidth()*0.33, right_column_row_2.getHeight());

		saved_traces_visible_onoff_button.setBounds(right_column_row_2.getX() + right_column_row_2.getWidth()*0.66, right_column_row_2.getY(), right_column_row_2.getWidth()*0.33, right_column_row_2.getHeight());

		saved_traces_filename_label.setBounds(right_column_row_3.getX(), right_column_row_3.getY(), right_column_row_3.getWidth(), right_column_row_3.getHeight());

		config_audio_io_button.setBounds(left_column_row_4.getX(), left_column_row_4.getY(), left_column_row_4.getWidth(), left_column_row_4.getHeight());
    
	}

	void buttonClicked(Button* button) override {
		
		analysis_status = analysis_onoff_button.getToggleState();

		saved_traces_visible = saved_traces_visible_onoff_button.getToggleState();
		
		if (button == &choose_mic_cal)
	
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

		if (button == &choose_system_curve)

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

		for (int x = 0; x < 162; x++) {

			save_traces << newest_fft_bin_frequencies[x] << "," << newest_xfer_function_mag_dB_avg_cal[x] << "," << newest_xfer_function_phase_deg_avg[x] << "," << newest_coherence_value[x] << std::endl;

		}

		save_traces.close();

		for (int x = 0; x < 162; x++) {

			saved_fft_bin_frequencies[x] = newest_fft_bin_frequencies[x];

			saved_xfer_function_mag_dB_avg_cal[x] = newest_xfer_function_mag_dB_avg_cal[x];

			saved_xfer_function_phase_deg_avg[x] = newest_xfer_function_phase_deg_avg[x];

			saved_coherence_value[x] = newest_coherence_value[x];

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

			saved_fft_bin_frequencies[x] = stod(frequency_string);

			std::getline(load_traces, amplitude_string, ',');

			saved_xfer_function_mag_dB_avg_cal[x] = stod(amplitude_string);

			std::getline(load_traces, phase_string, ',');

			saved_xfer_function_phase_deg_avg[x] = stod(phase_string);

			std::getline(load_traces, coherence_string);

			saved_coherence_value[x] = stod(coherence_string);

		}

		saved_traces_visible_onoff_button.setToggleState(true, dontSendNotification);
		saved_traces_visible = 1;

	}

private:

	TextButton analysis_onoff_button;

	TextButton choose_mic_cal;

	Label mic_cal_filename_label;

	Slider refresh_rate_slider;

	Slider smoothing_slider;

	TextButton choose_system_curve;

	Label system_curve_filename_label;

	TextButton capture_traces_button;

	TextButton load_saved_traces_button;

	TextButton saved_traces_visible_onoff_button;

	Label saved_traces_filename_label;

	TextButton config_audio_io_button;
					
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (controls)

};
