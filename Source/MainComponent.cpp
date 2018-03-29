/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "settings_bar.h"
#include "level_meters.h"
#include "plots.h"
#include "fft.h"
#include "constants.h"
#include "analysis_supervisor.h"

class MainContentComponent   : public AudioAppComponent, public constants, public Timer
{

public:
    //==============================================================================
		
	int num_samples_stored = 0;

	int buffer_ready = 0;

	int update_rate = 30; //In Hz
	
	MainContentComponent()
    {
		setSize(1024, 768);

		addAndMakeVisible(main_settings_bar);
		addAndMakeVisible(main_level_meters);
		addAndMakeVisible(main_plot);

		startTimerHz(update_rate);
		
		update_current_plot_data();
		
        setAudioChannels (2, 0);
						
    }

    ~MainContentComponent()
    {
        shutdownAudio();
		delete supervisor1;
    }

    //==============================================================================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{

		this->deviceManager.setCurrentAudioDeviceType("DirectSound", true);
		
		this->deviceManager.getAudioDeviceSetup(audio_device_setup);

		audio_device_setup.sampleRate = 48000;

		//audio_device_setup.bufferSize = 480;

		this->deviceManager.setAudioDeviceSetup(audio_device_setup, true);

	}

	void getNextAudioBlock(const AudioSourceChannelInfo& audio_device_buffer) override
	{
		const float* ref_in_buffer = audio_device_buffer.buffer->getReadPointer(0);
		const float* system_in_buffer = audio_device_buffer.buffer->getReadPointer(1);
		double ref_sample_value = 0;
		double system_sample_value = 0;
		
		if (audio_device_setup.inputChannels.toInteger() != 3) {} //there must be exactly two input channels (JUCE thinks there are 3 on Focusrite 2i2) for this audio callback to run correctly

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

		if (num_samples_stored >= 100000) {
			buffer_ready = 1;
		}

	}
	
    void releaseResources() override
    {
   	}

    //==============================================================================
    void paint (Graphics& g) override
    {
		
		Colour very_dark_grey(20, 20, 20);

		g.fillAll(very_dark_grey);

    }

    void resized() override
    {
		
		Rectangle<int> main_window(getLocalBounds());

		int settings_bar_height = getHeight()*0.14;
		main_settings_bar.setBounds(main_window.removeFromTop(settings_bar_height));

		int level_meters_height = getHeight()*0.05;
		main_level_meters.setBounds(main_window.removeFromBottom(level_meters_height));

		main_plot.setBounds(main_window);

    }

	void update_current_plot_data() {

		supervisor1->plot_data_mtx_supervisor.lock();
		
		for (int col = 0; col < composite_fft_bins; col++) {
						
			
			main_plot.current_composite_fft_bin_frequencies[col] = supervisor1->composite_fft_bin_frequencies[col];
			main_plot.current_composite_xfer_function_mag_dB[col] = supervisor1->composite_xfer_function_mag_dB_avg[col];
			main_plot.current_composite_xfer_function_phase_deg[col] = supervisor1->composite_xfer_function_phase_deg_avg[col];
			main_plot.current_composite_coherence_value[col] = supervisor1->composite_coherence_value[col];
					
		}

		for (int col = 0; col < spectrum_fft_bins; col++) {

			main_plot.current_system_spectrum_mag_db[col] = supervisor1->system_spectrum_mag_dB[col];
			main_plot.system_spectrum_bin_frequencies[col] = supervisor1->system_spectrum_fft_bin_frequencies[col];

		}

		supervisor1->plot_data_mtx_supervisor.unlock();

	}

	void update_captured_plot_data() {

		for (int col = 0; col < composite_fft_bins; col++) {

			main_plot.saved_fft_bin_frequencies[col] = main_settings_bar.main_controls.saved_fft_bin_frequencies[col];
			main_plot.saved_xfer_function_mag_dB_avg[col] = main_settings_bar.main_controls.saved_xfer_function_mag_dB_avg_cal[col];
			main_plot.saved_xfer_function_phase_deg_avg[col] = main_settings_bar.main_controls.saved_xfer_function_phase_deg_avg[col];
			main_plot.saved_coherence_value[col] = main_settings_bar.main_controls.saved_coherence_value[col];

			main_plot.saved_traces_visible = main_settings_bar.main_controls.saved_traces_visible;

		}

	}

	void update_saved_trace_arrays(){

		for (int col = 0; col < composite_fft_bins; col++) {

			main_settings_bar.main_controls.newest_fft_bin_frequencies[col] = main_plot.current_composite_fft_bin_frequencies[col];
			main_settings_bar.main_controls.newest_xfer_function_mag_dB_avg_cal[col] = main_plot.current_composite_xfer_function_mag_dB[col];
			main_settings_bar.main_controls.newest_xfer_function_phase_deg_avg[col] = main_plot.current_composite_xfer_function_phase_deg[col];
			main_settings_bar.main_controls.newest_coherence_value[col] = main_plot.current_composite_coherence_value[col];

		}

	}

	void update_meters() {

		main_settings_bar.main_spl_meter.raw_input_level = supervisor1->peak_of_current_system_samples;

		main_level_meters.raw_ref_input_level = supervisor1->peak_of_current_ref_samples;

		main_level_meters.raw_system_input_level = supervisor1->peak_of_current_system_samples;

	}

	void open_audio_io_window() {
		
		OptionalScopedPointer<Component> audio_io_config_window_content_ptr{ &audio_device_selector_component, 0 };

		audio_io_config_window_launch_options.dialogTitle = "Audio I/O Configuration";

		audio_io_config_window_launch_options.dialogBackgroundColour = Colours::darkgrey;

		audio_io_config_window_launch_options.content = audio_io_config_window_content_ptr;

		audio_io_config_window_launch_options.componentToCentreAround = NULL;

		audio_io_config_window_launch_options.escapeKeyTriggersCloseButton = true;

		audio_io_config_window_launch_options.useNativeTitleBar = true;

		audio_io_config_window_launch_options.useBottomRightCornerResizer = false;

		audio_device_selector_component.setSize(640, 480);

		audio_io_config_window_launch_options.launchAsync();

	}

	void update_supervisor_parameters() {

		supervisor1->analyser_run = main_settings_bar.main_controls.analysis_status;
		supervisor1->delay_in_samples = main_settings_bar.main_delay_indicator.delay_in_samples;
		supervisor1->mic_cal_path = main_settings_bar.main_controls.fixed_mic_cal_file_path;
		supervisor1->system_curve_path = main_settings_bar.main_controls.fixed_system_curve_file_path;
		supervisor1->analyser_update_rate = main_settings_bar.main_controls.refresh_rate_slider_value;
		supervisor1->smoothing_coefficient = main_settings_bar.main_controls.smoothing_slider_value;

	}

	void repaint_ui() {

		main_settings_bar.main_spl_meter.try_repaint();

		main_plot.try_repaint();

		main_level_meters.try_repaint();

		main_settings_bar.main_delay_indicator.try_repaint();

	}

	void timerCallback() override
	{
		
		if (main_settings_bar.main_controls.open_audio_io_config_window == 1) {

			main_settings_bar.main_controls.open_audio_io_config_window = 0;

			open_audio_io_window();
			
		}

		update_supervisor_parameters();
				
		update_current_plot_data();

		update_captured_plot_data();
			
		update_saved_trace_arrays();
		
		update_meters();

		repaint_ui();
		
	}
	
private:
    //==============================================================================
	
	supervisor* supervisor1 = new supervisor;
	
	settings_bar main_settings_bar;

	plots main_plot;

	level_meters main_level_meters;

	AudioDeviceManager::AudioDeviceSetup audio_device_setup;
	
	AudioDeviceSelectorComponent audio_device_selector_component{ this->deviceManager, 2, 2, 0, 0, 0, 0, 0, 0 };

	DialogWindow::LaunchOptions audio_io_config_window_launch_options;
				
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
		
};

Component* createMainContentComponent()     { return new MainContentComponent(); }
