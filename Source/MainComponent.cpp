
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

	main_settings_bar.main_delay_indicator.ir_peak_msec_ptr = &ir_window.main_ir_plot.ir_max_time_msec;
							
}

MainContentComponent::~MainContentComponent()
{
	supervisor1->signalThreadShouldExit();
	supervisor1->notify();
	shutdownAudio();
	delete supervisor1;
}

//==============================================================================
void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{}

void MainContentComponent::getNextAudioBlock(const AudioSourceChannelInfo& audio_device_buffer)
{
	this->deviceManager.getAudioDeviceSetup(current_audio_device_setup);

	if (current_audio_device_setup.sampleRate == 48000) {

		sample_rate_correct = true;

	}

	else { sample_rate_correct = false; }
	
	const float* ref_in_buffer = audio_device_buffer.buffer->getReadPointer(0);
	const float* system_in_buffer = audio_device_buffer.buffer->getReadPointer(1);
	
	supervisor1->audio_buffer_mtx_supervisor.lock();

	for (int sample = 0; sample < audio_device_buffer.numSamples; ++sample) {

		supervisor1->buffer_ref_samples.push_front(ref_in_buffer[sample]);
		supervisor1->buffer_ref_samples.pop_back();

		supervisor1->buffer_system_samples.push_front(system_in_buffer[sample]);
		supervisor1->buffer_system_samples.pop_back();

		num_samples_stored++;

	}

	supervisor1->audio_buffer_mtx_supervisor.unlock();

	reported_xruns = this->deviceManager.getXRunCount();

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
	int main_window_height = main_window.getHeight();

	main_settings_bar.setBounds(main_window.removeFromTop(main_window_height * 0.15));

	main_plot.setBounds(main_window.removeFromTop(main_window_height * 0.80));
	
	main_level_meters.setBounds(main_window);
	
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

	std::copy(supervisor1->composite_system_spectrum_mag_dB.begin(),
		supervisor1->composite_system_spectrum_mag_dB.end(),
		main_plot.current_composite_system_spectrum_mag_dB.begin());

	supervisor1->plot_data_mtx_supervisor.unlock();

	ir_window.update_ir_plot_data(supervisor1->impulse_response_time_samples);

	main_plot.sample_rate_incorrect_warning = !sample_rate_correct;

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

	//main_plot.saved_traces_visible = main_settings_bar.main_controls.saved_traces_visible;

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

	main_level_meters.raw_ref_input_level = supervisor1->current_ref_samples_RMS;

	main_level_meters.raw_system_input_level = supervisor1->current_system_samples_RMS;

}

void MainContentComponent::open_audio_io_window() {
		
	OptionalScopedPointer<Component> audio_io_config_window_content_ptr{ &audio_device_selector_component, 0 };

	audio_io_config_window.dialogTitle = "Audio Input Setup";

	audio_io_config_window.dialogBackgroundColour = Colours::darkgrey;

	audio_io_config_window.content = audio_io_config_window_content_ptr;

	audio_io_config_window.componentToCentreAround = NULL;

	audio_io_config_window.escapeKeyTriggersCloseButton = true;

	audio_io_config_window.useNativeTitleBar = true;

	audio_io_config_window.useBottomRightCornerResizer = false;

	audio_device_selector_component.setSize(640, 480);

	audio_io_config_window.launchAsync();

}

void MainContentComponent::open_ir_window()
{

	OptionalScopedPointer<Component> ir_window_content_ptr{ & ir_window, 0 };

	ir_window_config.dialogTitle = "System Impulse Response";

	ir_window_config.content = ir_window_content_ptr;

	ir_window_config.componentToCentreAround = NULL;

	ir_window_config.escapeKeyTriggersCloseButton = true;

	ir_window_config.useNativeTitleBar = true;

	ir_window_config.useBottomRightCornerResizer = false;

	ir_window_config.launchAsync();

}

void MainContentComponent::update_supervisor_parameters() {

	supervisor1->analyser_run = main_settings_bar.main_controls.analysis_status;
	supervisor1->delay_in_samples = main_settings_bar.main_delay_indicator.delay_in_samples;
	supervisor1->mic_cal_path = main_settings_bar.main_controls.fixed_mic_cal_file_path;
	supervisor1->system_curve_path = main_settings_bar.main_controls.fixed_system_curve_file_path;
	supervisor1->analyser_update_rate_hz = main_settings_bar.main_controls.refresh_rate_slider_value;
	supervisor1->smoothing_passes = main_settings_bar.main_controls.smoothing_slider_value;
	supervisor1->curves_only = main_settings_bar.main_controls.curves_only;

}

void MainContentComponent::timerCallback()
{
				
	main_settings_bar.main_spl_meter.repaint();

	main_plot.repaint();

	main_level_meters.repaint();
		
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

	else if (message == "cmd_open_ir_window") {

		open_ir_window();

	}

	else {}

}