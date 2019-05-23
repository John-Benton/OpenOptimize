/*
  ==============================================================================

    MainComponent.h
    Created: 4 Sep 2018 1:02:58pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "settings_bar.h"
#include "level_meters.h"
#include "plots.h"
#include "fft.h"
#include "constants.h"
#include "analysis_supervisor.h"
#include "ir_window.h"

class MainContentComponent : public AudioAppComponent, public constants, public Timer, public ActionListener
{

public:
	//==============================================================================

	int num_samples_stored = 0;

	int update_rate = 30; //In Hz

	MainContentComponent();

	~MainContentComponent();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

	void getNextAudioBlock(const AudioSourceChannelInfo& audio_device_buffer) override;

	void releaseResources() override;

	//==============================================================================
	void paint(Graphics& g) override;

	void resized() override;

	void update_current_plot_data();

	void update_loaded_plot_data();

	void update_controls_trace_arrays_for_saving();

	void update_meters();

	void open_audio_io_window();

	void open_ir_window();

	void update_supervisor_parameters();

	void timerCallback() override;

	void actionListenerCallback(const String &message) override;

private:
	//==============================================================================

	supervisor * supervisor1 = new supervisor;

	settings_bar main_settings_bar;

	plots main_plot;

	level_meters main_level_meters;

	AudioDeviceManager::AudioDeviceSetup current_audio_device_setup; //only used when checking sample rate

	AudioDeviceSelectorComponent audio_device_selector_component{ this->deviceManager, 2, 2, 0, 0, 0, 0, 0, 0 };

	int reported_xruns{ 0 };

	DialogWindow::LaunchOptions audio_io_config_window;

	ir_window ir_window;

	DialogWindow::LaunchOptions ir_window_config;

	bool data_update_active{ false }, sample_rate_correct{ true };

	OpenGLContext gl_renderer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

};

Component* createMainContentComponent() { return new MainContentComponent(); }