/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
	
MainContentComponent::MainContentComponent()
{
	setSize(1024, 768);

	addAndMakeVisible(main_settings_bar);
	addAndMakeVisible(main_level_meters);
	addAndMakeVisible(main_plot);

	startTimerHz(update_rate);
		
	update_current_plot_data();
		
    setAudioChannels (2, 0);

	main_settings_bar.main_controls.addActionListener(this);
	main_settings_bar.main_delay_indicator.addActionListener(this);
	supervisor1->addActionListener(this);
						
}

MainContentComponent::~MainContentComponent()
{
    shutdownAudio();
	delete supervisor1;
}

//==============================================================================
void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

	this->deviceManager.setCurrentAudioDeviceType("DirectSound", true);
		
	this->deviceManager.getAudioDeviceSetup(audio_device_setup);

	audio_device_setup.sampleRate = 48000;

	this->deviceManager.setAudioDeviceSetup(audio_device_setup, true);

}

void MainContentComponent::getNextAudioBlock(const AudioSourceChannelInfo& audio_device_buffer)
{
	const float* ref_in_buffer = audio_device_buffer.buffer->getReadPointer(0);
	const float* system_in_buffer = audio_device_buffer.buffer->getReadPointer(1);
	double ref_sample_value = 0;
	double system_sample_value = 0;
		
	if (audio_device_setup.inputChannels.toInteger() == 3) {

		supervisor1->audio_buffer_mtx_supervisor.lock();

		for (int sample = 0; sample < audio_device_buffer.numSamples; ++sample) {

			ref_sample_value = ref_in_buffer[sample];
			system_sample_value = system_in_buffer[sample];

			supervisor1->buffer_ref_samples.push_front(ref_sample_value);
			supervisor1->buffer_ref_samples.pop_back();

			supervisor1->buffer_system_samples.push_front(system_sample_value);
			supervisor1->buffer_system_samples.pop_back();

			num_samples_stored++;

		}

		supervisor1->audio_buffer_mtx_supervisor.unlock();

	}

	else {}; //there must be exactly two input channels (JUCE thinks there are 3 on Focusrite 2i2) for this audio callback to run correctly

}
	
void MainContentComponent::releaseResources()
{
}

    //==============================================================================
void MainContentComponent::paint (Graphics& g)
{
		
	Colour very_dark_grey(20, 20, 20);

	g.fillAll(very_dark_grey);

}

void MainContentComponent::resized()
{
		
	Rectangle<int> main_window(getLocalBounds());

	int settings_bar_height = getHeight()*0.14;
	main_settings_bar.setBounds(main_window.removeFromTop(settings_bar_height));

	int level_meters_height = getHeight()*0.05;
	main_level_meters.setBounds(main_window.removeFromBottom(level_meters_height));

	main_plot.setBounds(main_window);

}

void MainContentComponent::update_current_plot_data() {

	supervisor1->plot_data_mtx_supervisor.lock();

	std::copy(supervisor1->composite_fft_bin_frequencies.begin(), 
		supervisor1->composite_fft_bin_frequencies.end(), 
		main_plot.current_composite_fft_bin_frequencies.begin());
				
	std::copy(supervisor1->composite_xfer_function_mag_dB_cal.begin(),
		supervisor1->composite_xfer_function_mag_dB_cal.end(),
		main_plot.current_composite_xfer_function_mag_dB.begin());

	std::copy(supervisor1->composite_xfer_function_phase_deg.begin(),
		supervisor1->composite_xfer_function_phase_deg.end(),
		main_plot.current_composite_xfer_function_phase_deg.begin());

	std::copy(supervisor1->composite_coherence_value.begin(),
		supervisor1->composite_coherence_value.end(),
		main_plot.current_composite_coherence_value.begin());

	std::copy(supervisor1->composite_ref_spectrum_mag_dB.begin(),
		supervisor1->composite_ref_spectrum_mag_dB.end(),
		main_plot.current_composite_ref_spectrum_mag_dB.begin());

	supervisor1->plot_data_mtx_supervisor.unlock();

}

void MainContentComponent::update_loaded_plot_data() {

	std::copy(main_settings_bar.main_controls.loaded_composite_fft_bin_frequencies.begin(),
		main_settings_bar.main_controls.loaded_composite_fft_bin_frequencies.end(),
		main_plot.loaded_composite_fft_bin_frequencies.begin());

	std::copy(main_settings_bar.main_controls.loaded_composite_xfer_function_mag_dB_avg_cal.begin(),
		main_settings_bar.main_controls.loaded_composite_xfer_function_mag_dB_avg_cal.end(),
		main_plot.loaded_composite_xfer_function_mag_dB_avg_cal.begin());

	std::copy(main_settings_bar.main_controls.loaded_composite_xfer_function_phase_deg_avg.begin(),
		main_settings_bar.main_controls.loaded_composite_xfer_function_phase_deg_avg.end(),
		main_plot.loaded_composite_xfer_function_phase_deg_avg.begin());

	std::copy(main_settings_bar.main_controls.loaded_composite_coherence_value.begin(),
		main_settings_bar.main_controls.loaded_composite_coherence_value.end(),
		main_plot.loaded_composite_coherence_value.begin());

	main_plot.saved_traces_visible = main_settings_bar.main_controls.saved_traces_visible;

}

void MainContentComponent::update_controls_trace_arrays_for_saving(){

	std::copy(main_plot.current_composite_fft_bin_frequencies.begin(),
		main_plot.current_composite_fft_bin_frequencies.end(),
		main_settings_bar.main_controls.composite_fft_bin_frequencies_for_save.begin());

	std::copy(main_plot.current_composite_xfer_function_mag_dB.begin(),
		main_plot.current_composite_xfer_function_mag_dB.end(),
		main_settings_bar.main_controls.composite_xfer_function_mag_dB_avg_cal_for_save.begin());

	std::copy(main_plot.current_composite_xfer_function_phase_deg.begin(),
		main_plot.current_composite_xfer_function_phase_deg.end(),
		main_settings_bar.main_controls.composite_xfer_function_phase_deg_avg_for_save.begin());

	std::copy(main_plot.current_composite_coherence_value.begin(),
		main_plot.current_composite_coherence_value.end(),
		main_settings_bar.main_controls.composite_coherence_value_for_save.begin());

}

void MainContentComponent::update_meters() {

	main_settings_bar.main_spl_meter.raw_input_level = supervisor1->current_system_samples_RMS;

	main_level_meters.raw_ref_input_level = supervisor1->peak_of_current_ref_samples;

	main_level_meters.raw_system_input_level = supervisor1->peak_of_current_system_samples;

}

void MainContentComponent::open_audio_io_window() {
		
	OptionalScopedPointer<Component> audio_io_config_window_content_ptr{ &audio_device_selector_component, 0 };

	audio_io_config_window.dialogTitle = "Audio I/O Configuration";

	audio_io_config_window.dialogBackgroundColour = Colours::darkgrey;

	audio_io_config_window.content = audio_io_config_window_content_ptr;

	audio_io_config_window.componentToCentreAround = NULL;

	audio_io_config_window.escapeKeyTriggersCloseButton = true;

	audio_io_config_window.useNativeTitleBar = true;

	audio_io_config_window.useBottomRightCornerResizer = false;

	audio_device_selector_component.setSize(640, 480);

	audio_io_config_window.launchAsync();

}

void MainContentComponent::update_supervisor_parameters() {

	supervisor1->analyser_run = main_settings_bar.main_controls.analysis_status;
	supervisor1->delay_in_samples = main_settings_bar.main_delay_indicator.delay_in_samples;
	supervisor1->mic_cal_path = main_settings_bar.main_controls.fixed_mic_cal_file_path;
	supervisor1->system_curve_path = main_settings_bar.main_controls.fixed_system_curve_file_path;
	supervisor1->analyser_update_rate = main_settings_bar.main_controls.refresh_rate_slider_value;
	supervisor1->smoothing_passes = main_settings_bar.main_controls.smoothing_slider_value;
	supervisor1->curves_only = main_settings_bar.main_controls.curves_only;

}

void MainContentComponent::timerCallback()
{
				
	main_settings_bar.main_spl_meter.try_repaint();

	main_plot.try_repaint();

	main_level_meters.try_repaint();
		
}

void MainContentComponent::actionListenerCallback(const String &message)
{

	if (message == "cmd_open_audio_io") 
	{

		open_audio_io_window();

	}

	else if (message == "cmd_update_supervisor") 
	{

		update_supervisor_parameters();
		
	}

	else if (message == "cmd_supervisor_cycle_done" && data_update_active==false)
	{

		data_update_active = true;
		
		update_current_plot_data();

		update_loaded_plot_data();

		update_controls_trace_arrays_for_saving();

		update_meters();

		data_update_active = false;

	}

	else {}

}