/*
  ==============================================================================

    analysis_supervisor.h
    Created: 18 Sep 2017 8:19:24pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <numeric>
#include <mutex>
#include "moving_average.h"
#include "phase.h"
#include "fft.h"
#include "constants.h"
#include <algorithm>

class supervisor : public constants, public Timer, public Thread

{

public:

	//Complex vectors use the following format: [0][Re], [1][Imag]

	/*---------------------------------------------------*/

	std::vector<std::vector<double>> composite_ref_complex_vector;
	std::vector<std::vector<double>> composite_system_complex_vector;

	std::vector<std::vector<double>> composite_ref_complex_conjugate;
	std::vector<std::vector<double>> composite_system_complex_conjugate;

	std::vector<double> composite_ref_autospectrum;
	std::vector<double> composite_system_autospectrum;

	std::vector<std::vector<double>> composite_cross_spectrum_complex;

	std::vector<double> composite_cross_spectrum_magnitude;

	std::vector<double> composite_fft_bin_frequencies;

	std::vector<double> system_spectrum_fft_bin_frequencies;
	std::vector<double> system_spectrum_mag_linear;
	std::vector<double> system_spectrum_mag_dB;

	std::vector<std::vector<double>> composite_xfer_function_complex;

	std::vector<double> composite_xfer_function_mag_dB_uncal;
	std::vector<double> composite_xfer_function_mag_dB_cal;
	std::vector<double> composite_xfer_function_phase_deg;

	std::vector<std::vector<double>> composite_ref_autospectrum_history;
	std::vector<double> composite_ref_autospectrum_avg;

	std::vector<std::vector<double>> composite_system_autospectrum_history;
	std::vector<double> composite_system_autospectrum_avg;

	std::vector<std::vector<double>> composite_cross_spectrum_mag_history;
	std::vector<double> composite_cross_spectrum_mag_avg;

	std::vector<std::vector<double>> composite_xfer_function_mag_dB_history;
	std::vector<double> composite_xfer_function_mag_dB_avg;

	std::vector<std::vector<double>> composite_xfer_function_phase_deg_history;
	std::vector<double> composite_xfer_function_phase_deg_avg;
	
	std::vector<double> composite_coherence_value;

	/*---------------------------------------------------*/

	std::deque<double> buffer_ref_samples;
	std::deque<double> buffer_system_samples;

	std::deque<double> current_ref_samples;
	std::deque<double> current_system_samples;

	/*---------------------------------------------------*/

	const int fft_bins = (largest_fft_size / 2) + 1;

	int delay_in_samples = 0;

	/*---------------------------------------------------*/

	std::string mic_cal_path;

	std::vector<double> original_mic_cal_frequencies;
	std::vector<double> original_mic_cal_amplitudes;

	std::vector<double> interpolated_mic_cal_amplitudes;

	/*---------------------------------------------------*/

	std::string system_curve_path;

	std::vector<double> original_system_curve_frequencies;
	std::vector<double> original_system_curve_amplitudes;

	std::vector<double> interpolated_system_curve_amplitudes;

	/*---------------------------------------------------*/

	double average_of_current_ref_samples = 0.0;

	double average_of_current_system_samples = 0.0;

	/*---------------------------------------------------*/

	std::mutex audio_buffer_mtx_supervisor;

	std::mutex plot_data_mtx_supervisor;

	int analyser_update_rate = 1; //in Hz

	bool analysis_cycle_active = false;

	bool analyser_run = false;

	int smoothing_coefficient = 0;

	supervisor(): Thread("Supervisor_Thread",0)
	
	{
		
		composite_ref_complex_vector.resize(2, std::vector<double>(162)); //two rows, each row has as many columns as there are fft bins
		composite_system_complex_vector.resize(2, std::vector<double>(162));

		composite_ref_complex_conjugate.resize(2, std::vector<double>(162));
		composite_system_complex_conjugate.resize(2, std::vector<double>(162));

		composite_ref_autospectrum.resize(162);
		composite_system_autospectrum.resize(162);

		composite_cross_spectrum_complex.resize(2, std::vector<double>(162));

		composite_cross_spectrum_magnitude.resize(162);

		composite_xfer_function_complex.resize(2, std::vector<double>(162));
		composite_xfer_function_mag_dB_uncal.resize(162);
		composite_xfer_function_mag_dB_cal.resize(162);
		composite_xfer_function_phase_deg.resize(162);

		composite_ref_autospectrum_history.resize(4, std::vector<double>(162)); //each row is one history
		composite_ref_autospectrum_avg.resize(162);

		composite_system_autospectrum_history.resize(4, std::vector<double>(162));
		composite_system_autospectrum_avg.resize(162);

		composite_cross_spectrum_mag_history.resize(4, std::vector<double>(162));
		composite_cross_spectrum_mag_avg.resize(162);

		composite_xfer_function_mag_dB_history.resize(4, std::vector<double>(162));
		composite_xfer_function_mag_dB_avg.resize(162);

		composite_xfer_function_phase_deg_history.resize(4, std::vector<double>(162));
		composite_xfer_function_phase_deg_avg.resize(162);

		composite_coherence_value.resize(162);

		composite_fft_bin_frequencies.resize(162);
		
		system_spectrum_fft_bin_frequencies.resize(spectrum_fft_bins);
		system_spectrum_mag_linear.resize(spectrum_fft_bins);
		system_spectrum_mag_dB.resize(spectrum_fft_bins);

		interpolated_mic_cal_amplitudes.resize(162);

		interpolated_system_curve_amplitudes.resize(162);
		
		assemble_composite_fft_bin_frequencies();

		buffer_ref_samples.resize(largest_fft_size + max_delay_samples);
		buffer_system_samples.resize(largest_fft_size);

		current_ref_samples.resize(largest_fft_size);
		current_system_samples.resize(largest_fft_size);
				
		startTimerHz(analyser_update_rate);
		
	};

	~supervisor() {

		delete fft_32k;
		delete fft_16k;
		delete fft_8k;
		delete fft_4k;
		delete fft_2k;
		delete fft_1k;
		delete fft_512;
		delete fft_256;
		delete fft_128;
		delete fft_64;

		stopThread(1000);

	};

	void run() override
	
	{

		while (!threadShouldExit()) {

			run_analysis();

			wait(-1); //thread will stop here until notify() is called from the timer callback

		}

	}

	void run_analysis() {

		analysis_cycle_active = true;

		refresh_supervisor_buffer();

		if (mic_cal_path != "") {
			load_mic_cal();
			interpolate_mic_cal_amplitudes();
		}

		if (system_curve_path != "") {
			load_system_curve();
			interpolate_system_curve_amplitudes();
		}

		fft_32k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_16k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_8k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_4k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_2k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_1k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_512->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_256->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_128->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_64->run_fft_analysis(current_ref_samples, current_system_samples);

		assemble_composite_complex_vectors();
			
		process_fft_data();

		update_histories();

		plot_data_mtx_supervisor.lock();

		calc_system_spectrum();

		smooth_system_spectrum();

		update_averages();

		calculate_coherence();

		plot_data_mtx_supervisor.unlock();
			
		run_calcs_for_SPL_and_meters();

		analysis_cycle_active = false;

	}

	void refresh_supervisor_buffer() {

		audio_buffer_mtx_supervisor.lock();

		std::copy(buffer_ref_samples.begin() + delay_in_samples, buffer_ref_samples.begin() + current_ref_samples.size() + delay_in_samples, current_ref_samples.begin());

		std::copy(buffer_system_samples.begin(), buffer_system_samples.begin() + current_system_samples.size(), current_system_samples.begin());

		audio_buffer_mtx_supervisor.unlock();

	}

	void timerCallback() override

	{

		if (analyser_run == true) {

			if (isThreadRunning() == false) {

				startThread();

			}

			if (analysis_cycle_active == false) { //don't try to call repaint again if there's already a repaint cycle in progress

				notify(); //wake up supervisor thread to run another analysis cycle

			}

		}
		
		if (analyser_run == false) {

			stopThread(10000);

		}

		stopTimer();

		startTimerHz(analyser_update_rate);

	}

private:
	
	fft * fft_32k = new fft(largest_fft_size);
	fft * fft_16k = new fft(largest_fft_size / 2);
	fft * fft_8k = new fft(largest_fft_size / 4);
	fft * fft_4k = new fft(largest_fft_size / 8);
	fft * fft_2k = new fft(largest_fft_size / 16);
	fft * fft_1k = new fft(largest_fft_size / 32);
	fft * fft_512 = new fft(largest_fft_size / 64);
	fft * fft_256 = new fft(largest_fft_size / 128);
	fft * fft_128 = new fft(largest_fft_size / 256);
	fft * fft_64 = new fft(largest_fft_size / 512);

	MovingAverage1DVector composite_data_smoother;

	MovingAverage2DVector complex_data_smoother;

	PhaseWrapUnwrap phase_unwrapper;

	void assemble_composite_complex_vectors() {

		for (int bin = 14; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin - 14] = fft_32k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin - 14] = fft_32k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin - 14] = fft_32k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin - 14] = fft_32k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 2] = fft_16k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 2] = fft_16k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 2] = fft_16k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 2] = fft_16k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 18] = fft_8k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 18] = fft_8k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 18] = fft_8k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 18] = fft_8k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 34] = fft_4k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 34] = fft_4k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 34] = fft_4k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 34] = fft_4k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 50] = fft_2k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 50] = fft_2k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 50] = fft_2k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 50] = fft_2k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 66] = fft_1k->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 66] = fft_1k->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 66] = fft_1k->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 66] = fft_1k->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 82] = fft_512->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 82] = fft_512->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 82] = fft_512->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 82] = fft_512->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 98] = fft_256->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 98] = fft_256->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 98] = fft_256->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 98] = fft_256->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 114] = fft_128->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 114] = fft_128->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 114] = fft_128->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 114] = fft_128->fftw_complex_out_system_vector[1][bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_ref_complex_vector[0][bin + 130] = fft_64->fftw_complex_out_ref_vector[0][bin];
			composite_ref_complex_vector[1][bin + 130] = fft_64->fftw_complex_out_ref_vector[1][bin];

			composite_system_complex_vector[0][bin + 130] = fft_64->fftw_complex_out_system_vector[0][bin];
			composite_system_complex_vector[1][bin + 130] = fft_64->fftw_complex_out_system_vector[1][bin];
		}

	}
	
	void assemble_composite_fft_bin_frequencies() {

		for (int bin = 14; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin - 14] = fft_32k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 2] = fft_16k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 18] = fft_8k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 34] = fft_4k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 50] = fft_2k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 66] = fft_1k->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 82] = fft_512->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 98] = fft_256->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 114] = fft_128->fft_bin_frequencies[bin];
		}

		for (int bin = 16; bin < 32; bin++) {
			composite_fft_bin_frequencies[bin + 130] = fft_64->fft_bin_frequencies[bin];
		}

	}

	void calibrate_xfer_function_mag() {

		for (int x = 0; x < 162; x++) {

			composite_xfer_function_mag_dB_cal[x] =

				composite_xfer_function_mag_dB_uncal[x] -

				interpolated_mic_cal_amplitudes[x] -

				interpolated_system_curve_amplitudes[x];

		}

	}

	void calc_system_spectrum() {

		for (int col = 0; col < spectrum_fft_bins; col++) {

			system_spectrum_fft_bin_frequencies[col] = fft_4k->fft_bin_frequencies[col];

			system_spectrum_mag_linear[col] = 
				
				sqrt(pow(fft_4k->fftw_complex_out_system_vector[0][col], 2) +
					
					pow(fft_4k->fftw_complex_out_system_vector[1][col], 2));

			system_spectrum_mag_dB[col] = 20 * log10(system_spectrum_mag_linear[col]);

		}

	}

	void process_fft_data() {

		complex_conjugate_system();
		complex_conjugate_ref();
		calc_ref_autospectrum();
		calc_system_autospectrum();
		calc_cross_spectrum();
		calc_xfer_function();
		calibrate_xfer_function_mag();
		smooth_xfer_function_magnitude();

	}
	
	void complex_conjugate_ref() {

		std::copy(composite_ref_complex_vector[0].begin(), composite_ref_complex_vector[0].begin() + 162, composite_ref_complex_conjugate[0].begin());

		for (int col = 0; col < 162; col++) {
			composite_ref_complex_conjugate[1][col] = -composite_ref_complex_vector[1][col];
		}

	}

	void complex_conjugate_system() {

		std::copy(composite_system_complex_vector[0].begin(), composite_system_complex_vector[0].begin() + 162, composite_system_complex_conjugate[0].begin());

		for (int col = 0; col < 162; col++) {
			composite_system_complex_conjugate[1][col] = -composite_system_complex_vector[1][col];
		}

	}

	void calc_ref_autospectrum() {

		for (int col = 0; col < 162; col++) {
			composite_ref_autospectrum[col] =
				((composite_ref_complex_vector[0][col])*(composite_ref_complex_conjugate[0][col]) -
				((composite_ref_complex_vector[1][col])*(composite_ref_complex_conjugate[1][col])));
		}
	}

	void calc_system_autospectrum() {

		for (int col = 0; col < 162; col++) {
			composite_system_autospectrum[col] =
				((composite_system_complex_vector[0][col])*(composite_system_complex_conjugate[0][col]) -
				((composite_system_complex_vector[1][col])*(composite_system_complex_conjugate[1][col])));
		}
	}

	void calc_cross_spectrum() {

		for (int col = 0; col < 162; col++) {
			composite_cross_spectrum_complex[0][col] =
				((composite_ref_complex_conjugate[0][col])*(composite_system_complex_vector[0][col]) -
				(composite_ref_complex_conjugate[1][col])*(composite_system_complex_vector[1][col]));
		}

		for (int col = 0; col < 162; col++) {
			composite_cross_spectrum_complex[1][col] =
				((composite_ref_complex_conjugate[0][col])*(composite_system_complex_vector[1][col]) +
				(composite_ref_complex_conjugate[1][col])*(composite_system_complex_vector[0][col]));
		}

		for (int col = 0; col < 162; col++) {
			composite_cross_spectrum_magnitude[col] = sqrt(pow(composite_cross_spectrum_complex[0][col], 2) + pow(composite_cross_spectrum_complex[1][col], 2));
		}
		int x = 0;
	}

	void calc_xfer_function() {

		double xfer_function_magnitude_linear;

		double xfer_function_phase_radians;

		for (int col = 0; col < 162; col++) {
			composite_xfer_function_complex[0][col] = composite_cross_spectrum_complex[0][col] / composite_ref_autospectrum[col];
		}

		for (int col = 0; col < 162; col++) {
			composite_xfer_function_complex[1][col] = composite_cross_spectrum_complex[1][col] / composite_ref_autospectrum[col];
		}

		for (int col = 0; col < 162; col++) {
			xfer_function_magnitude_linear = sqrt(pow(composite_xfer_function_complex[0][col], 2) + pow(composite_xfer_function_complex[1][col], 2));
			composite_xfer_function_mag_dB_uncal[col] = 20 * log10(xfer_function_magnitude_linear);
		}

		smooth_xfer_function_complex_for_phase();

		for (int col = 0; col < 162; col++) {

			xfer_function_phase_radians = -atan2(composite_xfer_function_complex[1][col], composite_xfer_function_complex[0][col]);
			composite_xfer_function_phase_deg[col] = xfer_function_phase_radians / 0.0174533;
		}

	}

	void update_histories() {

		for (int col = 0; col < 162; col++) {

			//using a loop to was found to be more efficient than std::copy for this function

			composite_ref_autospectrum_history[3][col] = composite_ref_autospectrum_history[2][col];
			composite_ref_autospectrum_history[2][col] = composite_ref_autospectrum_history[1][col];
			composite_ref_autospectrum_history[1][col] = composite_ref_autospectrum_history[0][col];
			composite_ref_autospectrum_history[0][col] = composite_ref_autospectrum[col];

			composite_system_autospectrum_history[3][col] = composite_system_autospectrum_history[2][col];
			composite_system_autospectrum_history[2][col] = composite_system_autospectrum_history[1][col];
			composite_system_autospectrum_history[1][col] = composite_system_autospectrum_history[0][col];
			composite_system_autospectrum_history[0][col] = composite_system_autospectrum[col];

			composite_cross_spectrum_mag_history[3][col] = composite_cross_spectrum_mag_history[2][col];
			composite_cross_spectrum_mag_history[2][col] = composite_cross_spectrum_mag_history[1][col];
			composite_cross_spectrum_mag_history[1][col] = composite_cross_spectrum_mag_history[0][col];
			composite_cross_spectrum_mag_history[0][col] = composite_cross_spectrum_magnitude[col];

			composite_xfer_function_mag_dB_history[3][col] = composite_xfer_function_mag_dB_history[2][col];
			composite_xfer_function_mag_dB_history[2][col] = composite_xfer_function_mag_dB_history[1][col];
			composite_xfer_function_mag_dB_history[1][col] = composite_xfer_function_mag_dB_history[0][col];
			composite_xfer_function_mag_dB_history[0][col] = composite_xfer_function_mag_dB_cal[col];

			composite_xfer_function_phase_deg_history[3][col] = composite_xfer_function_phase_deg_history[2][col];
			composite_xfer_function_phase_deg_history[2][col] = composite_xfer_function_phase_deg_history[1][col];
			composite_xfer_function_phase_deg_history[1][col] = composite_xfer_function_phase_deg_history[0][col];
			composite_xfer_function_phase_deg_history[0][col] = composite_xfer_function_phase_deg[col];

		}

	}

	void update_averages() {

		for (int col = 0; col < 162; col++) {

			composite_ref_autospectrum_avg[col] =
				(composite_ref_autospectrum_history[0][col] +
					composite_ref_autospectrum_history[1][col] +
					composite_ref_autospectrum_history[2][col] +
					composite_ref_autospectrum_history[3][col]) / 4;

		}

		for (int col = 0; col < 162; col++) {

			composite_system_autospectrum_avg[col] =
				(composite_system_autospectrum_history[0][col] +
					composite_system_autospectrum_history[1][col] +
					composite_system_autospectrum_history[2][col] +
					composite_system_autospectrum_history[3][col]) / 4;

		}

		for (int col = 0; col < 162; col++) {

			composite_cross_spectrum_mag_avg[col] =
				(composite_cross_spectrum_mag_history[0][col] +
					composite_cross_spectrum_mag_history[1][col] +
					composite_cross_spectrum_mag_history[2][col] +
					composite_cross_spectrum_mag_history[3][col]) / 4;

		}

		for (int col = 0; col < 162; col++) {

			composite_xfer_function_mag_dB_avg[col] =
				(composite_xfer_function_mag_dB_history[0][col] +
					composite_xfer_function_mag_dB_history[1][col] +
					composite_xfer_function_mag_dB_history[2][col] +
					composite_xfer_function_mag_dB_history[3][col]) / 4;

		}

		for (int col = 0; col < 162; col++) {

			composite_xfer_function_phase_deg_avg[col] =
				(composite_xfer_function_phase_deg_history[0][col] +
					composite_xfer_function_phase_deg_history[1][col] +
					composite_xfer_function_phase_deg_history[2][col] +
					composite_xfer_function_phase_deg_history[3][col]) / 4;

		}

	}

	void calculate_coherence() {

		for (int col = 0; col < 162; col++) {

			composite_coherence_value[col] = (pow(composite_cross_spectrum_mag_avg[col], 2)) /
				((composite_ref_autospectrum_avg[col])*(composite_system_autospectrum_avg[col]));

		}
	}

	void smooth_xfer_function_complex_for_phase() {

		composite_data_smoother.configure(162, smoothing_coefficient*2);
		composite_data_smoother.process(composite_xfer_function_complex[0]);
		composite_data_smoother.process(composite_xfer_function_complex[1]);

	}

	void smooth_xfer_function_magnitude() {

		composite_data_smoother.configure(162, smoothing_coefficient * 2);
		composite_data_smoother.process(composite_xfer_function_mag_dB_cal);

	}

	void smooth_system_spectrum() {

		composite_data_smoother.configure(spectrum_fft_bins, smoothing_coefficient * 2);
		composite_data_smoother.process(system_spectrum_mag_dB);

	}

	void load_mic_cal() {

		std::string blank;

		std::string current_line;

		char frequency[8];

		double frequency_double;

		char amplitude[8];

		double amplitude_double;

		int numlines = 0;

		int tab_pos = 0;

		std::ifstream mic_cal(mic_cal_path);

		while (std::getline(mic_cal, blank)) {

			numlines++;

		}

		mic_cal.clear();

		mic_cal.seekg(0, mic_cal.beg);

		for (int x = 0; x < numlines; x++) {

			std::getline(mic_cal, current_line);

			tab_pos = current_line.find('\t');

			current_line.copy(frequency, tab_pos);

			current_line.copy(amplitude, current_line.length() - tab_pos, tab_pos + 1);

			original_mic_cal_frequencies.push_back(atof(frequency));

			original_mic_cal_amplitudes.push_back(atof(amplitude));

		}

	}

	void interpolate_mic_cal_amplitudes() {

		for (int x = 0; x < 162; x++) {

			if (composite_fft_bin_frequencies[x] < original_mic_cal_frequencies[0]) {

				interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[0];

			}

			if (composite_fft_bin_frequencies[x] > original_mic_cal_frequencies[original_mic_cal_frequencies.size() - 1]) {

				interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[original_mic_cal_frequencies.size() - 1];

			}

			if (composite_fft_bin_frequencies[x] >= original_mic_cal_frequencies[0] &&
				composite_fft_bin_frequencies[x] <= original_mic_cal_frequencies[original_mic_cal_frequencies.size() - 1]) {

				int composite_fft_bin_frequency = composite_fft_bin_frequencies[x];

				int current_pos = 0;
				int x0_pos = 0;
				int x1_pos = 0;

				while (1) {

					current_pos++;

					if (current_pos >= original_mic_cal_frequencies.size() - 1) {

						x0_pos = original_mic_cal_frequencies.size() - 1;

						break;

					}

					if (original_mic_cal_frequencies[current_pos] > composite_fft_bin_frequency) {

						x1_pos = current_pos;

						break;

					}

				}

				while (1) {

					current_pos--;

					if (current_pos <= 0) {

						x0_pos = 0;

						break;

					}

					if (original_mic_cal_frequencies[current_pos] < composite_fft_bin_frequency) {

						x0_pos = current_pos;

						break;

					}

				}

				interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[x0_pos] +

					(composite_fft_bin_frequency - original_mic_cal_frequencies[x0_pos])*

					((original_mic_cal_amplitudes[x1_pos] - original_mic_cal_amplitudes[x0_pos]) / (original_mic_cal_frequencies[x1_pos] - original_mic_cal_frequencies[x0_pos]));

			}

		}

	};

	void load_system_curve() {

		std::string blank;

		std::string current_line;

		char frequency[8];

		double frequency_double;

		char amplitude[8];

		double amplitude_double;

		int numlines = 0;

		int tab_pos = 0;

		std::ifstream system_curve(system_curve_path);

		while (std::getline(system_curve, blank)) {

			numlines++;

		}

		system_curve.clear();

		system_curve.seekg(0, system_curve.beg);

		for (int x = 0; x < numlines; x++) {

			std::getline(system_curve, current_line);

			tab_pos = current_line.find('\t');

			current_line.copy(frequency, tab_pos);

			current_line.copy(amplitude, current_line.length() - tab_pos, tab_pos + 1);

			original_system_curve_frequencies.push_back(atof(frequency));

			original_system_curve_amplitudes.push_back(atof(amplitude));

		}

	};

	void interpolate_system_curve_amplitudes() {

		for (int x = 0; x < 162; x++) {

			if (composite_fft_bin_frequencies[x] < original_system_curve_frequencies[0]) {

				interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[0];

			}

			if (composite_fft_bin_frequencies[x] > original_system_curve_frequencies[original_system_curve_frequencies.size() - 1]) {

				interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[original_system_curve_frequencies.size() - 1];

			}

			if (composite_fft_bin_frequencies[x] >= original_system_curve_frequencies[0] &&
				composite_fft_bin_frequencies[x] <= original_system_curve_frequencies[original_system_curve_frequencies.size() - 1]) {

				int composite_fft_bin_frequency = composite_fft_bin_frequencies[x];

				int current_pos = 0;
				int x0_pos = 0;
				int x1_pos = 0;

				while (1) {

					current_pos++;

					if (current_pos >= original_system_curve_frequencies.size() - 1) {

						x0_pos = original_system_curve_frequencies.size() - 1;

						break;

					}

					if (original_system_curve_frequencies[current_pos] > composite_fft_bin_frequency) {

						x1_pos = current_pos;

						break;

					}

				}

				while (1) {

					current_pos--;

					if (current_pos <= 0) {

						x0_pos = 0;

						break;

					}

					if (original_system_curve_frequencies[current_pos] < composite_fft_bin_frequency) {

						x0_pos = current_pos;

						break;

					}

				}

				interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[x0_pos] +

					(composite_fft_bin_frequency - original_system_curve_frequencies[x0_pos])*

					((original_system_curve_amplitudes[x1_pos] - original_system_curve_amplitudes[x0_pos]) / 
					
					(original_system_curve_frequencies[x1_pos] - original_system_curve_frequencies[x0_pos]));

			}

		}
		
	}

	void run_calcs_for_SPL_and_meters() {

		double sum_of_ref_samples = 0.0;

		for (int x = 0; x < current_ref_samples.size(); x++) {

			sum_of_ref_samples = sum_of_ref_samples + abs(current_ref_samples[x]);

		}

		average_of_current_ref_samples = sum_of_ref_samples / current_ref_samples.size();
		

		double sum_of_system_samples = 0.0;
		
		for (int x = 0; x < current_system_samples.size(); x++) {

			sum_of_system_samples = sum_of_system_samples + abs(current_system_samples[x]);

		}

		average_of_current_system_samples = sum_of_system_samples / current_system_samples.size();

	}

	//std::ofstream interpolation;
	//interpolation.open("C:\\interpolation.txt");
	//for (int row = 0; row < 162; row++) {
	//	interpolation << composite_fft_bin_frequencies[row] << "," << interpolated_mic_cal_amplitudes[row] << std::endl;
	//};

	//void dump_analysis_averages() {
	//
	//	std::ofstream analysis;
	//	analysis.open("C:\\analysis.txt");
	//	for (int col = 0; col < fft_bins; col++) {
	//		analysis << fft_bin_frequencies[col] << "," << composite_xfer_function_mag_dB_avg[col] << "," << composite_xfer_function_phase_deg_avg[col] << "," << composite_coherence_value[col] << std::endl;
	//	}

};