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

	std::vector<int> composite_fft_bin_frequencies;

	std::vector<double> composite_ref_spectrum_mag_dB;

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

	std::vector<double> current_ref_samples;
	std::vector<double> current_system_samples;

	std::vector<double> current_system_samples_squared;
	double current_system_samples_squared_sum;
	double current_system_samples_squared_mean;
	double current_system_samples_RMS;

	/*---------------------------------------------------*/

	const int fft_bins = (largest_fft_size / 2) + 1;

	int delay_in_samples = 0;

	/*---------------------------------------------------*/

	std::string mic_cal_path;
	std::string mic_cal_path_in_use;

	std::vector<double> original_mic_cal_frequencies;
	std::vector<double> original_mic_cal_amplitudes;

	std::vector<double> interpolated_mic_cal_amplitudes;

	/*---------------------------------------------------*/

	std::string system_curve_path;
	std::string system_curve_path_in_use;

	std::vector<double> original_system_curve_frequencies;
	std::vector<double> original_system_curve_amplitudes;

	std::vector<double> interpolated_system_curve_amplitudes;

	/*---------------------------------------------------*/

	double peak_of_current_ref_samples = 0.0;

	double peak_of_current_system_samples = 0.0;

	/*---------------------------------------------------*/

	std::mutex audio_buffer_mtx_supervisor;

	std::mutex plot_data_mtx_supervisor;

	int analyser_update_rate = 1; //in Hz

	bool analysis_cycle_active = false;

	bool analyser_run = false;

	int smoothing_average_size = 3;

	int smoothing_passes = 0;

	float spectrum_offset_db = 12.0;

	float spectrum_scaling_coefficient = 0.5;

	float spectrum_slope_db_octave = -4.5;

	int curves_only = 0;

	supervisor(): Thread("Supervisor_Thread",0)
	
	{
						
		composite_ref_complex_vector.resize(2, std::vector<double>(composite_fft_bins)); //two rows, each row has as many columns as there are composite fft bins
		composite_system_complex_vector.resize(2, std::vector<double>(composite_fft_bins));

		composite_ref_complex_conjugate.resize(2, std::vector<double>(composite_fft_bins));
		composite_system_complex_conjugate.resize(2, std::vector<double>(composite_fft_bins));

		composite_ref_autospectrum.resize(composite_fft_bins);
		composite_system_autospectrum.resize(composite_fft_bins);

		composite_cross_spectrum_complex.resize(2, std::vector<double>(composite_fft_bins));

		composite_cross_spectrum_magnitude.resize(composite_fft_bins);

		composite_xfer_function_complex.resize(2, std::vector<double>(composite_fft_bins));
		composite_xfer_function_mag_dB_uncal.resize(composite_fft_bins);
		composite_xfer_function_mag_dB_cal.resize(composite_fft_bins);
		composite_xfer_function_phase_deg.resize(composite_fft_bins);

		composite_ref_autospectrum_history.resize(4, std::vector<double>(composite_fft_bins)); //each row is one history
		composite_ref_autospectrum_avg.resize(composite_fft_bins);

		composite_system_autospectrum_history.resize(4, std::vector<double>(composite_fft_bins));
		composite_system_autospectrum_avg.resize(composite_fft_bins);

		composite_cross_spectrum_mag_history.resize(4, std::vector<double>(composite_fft_bins));
		composite_cross_spectrum_mag_avg.resize(composite_fft_bins);

		composite_xfer_function_mag_dB_history.resize(4, std::vector<double>(composite_fft_bins));
		composite_xfer_function_mag_dB_avg.resize(composite_fft_bins);

		composite_xfer_function_phase_deg_history.resize(4, std::vector<double>(composite_fft_bins));
		composite_xfer_function_phase_deg_avg.resize(composite_fft_bins);

		composite_coherence_value.resize(composite_fft_bins);

		composite_fft_bin_frequencies.resize(composite_fft_bins);

		composite_ref_spectrum_mag_dB.resize(composite_fft_bins);

		interpolated_mic_cal_amplitudes.resize(composite_fft_bins);

		interpolated_system_curve_amplitudes.resize(composite_fft_bins);

		buffer_ref_samples.resize(largest_fft_size + max_delay_samples);
		buffer_system_samples.resize(largest_fft_size);

		current_ref_samples.resize(largest_fft_size);
		current_system_samples.resize(largest_fft_size);

		current_system_samples_squared.resize(largest_fft_size);

		assemble_composite_fft_bin_frequencies();
				
		startTimerHz(analyser_update_rate);
		
	};

	~supervisor() {

		delete fft_32k;
		//delete fft_16k;
		delete fft_8k;
		delete fft_4k;
		delete fft_2k;
		delete fft_1k;
		delete fft_512;
		delete fft_256;
		delete fft_128;
		delete fft_64;

		stopThread(5000);

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

		update_mic_system_curves();
		
		fft_32k->run_fft_analysis(current_ref_samples, current_system_samples);
		//fft_16k->run_fft_analysis(current_ref_samples, current_system_samples);
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

		update_averages();

		calculate_coherence();

		calc_ref_spectrum();

		smooth_spectrum_data();
		
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

			if (analysis_cycle_active == false) { //don't try to run a new analysis cycle if there's already one in progress

				notify(); //wake up supervisor thread to run another analysis cycle

			}

		}
		
		if (analyser_run == false) {

			stopThread(1000);

		}

		stopTimer();

		startTimerHz(analyser_update_rate);

	}

private:
	
	fft * fft_32k = new fft(largest_fft_size);
	//fft * fft_16k = new fft(largest_fft_size / 2);
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

	void assemble_composite_complex_vectors() {

		for (int index = 0; index < 70; index++) {

			composite_ref_complex_vector[0][index] = fft_32k->fftw_complex_out_ref_vector[0][index + 13];
			composite_ref_complex_vector[1][index] = fft_32k->fftw_complex_out_ref_vector[1][index + 13];

			composite_system_complex_vector[0][index] = fft_32k->fftw_complex_out_system_vector[0][index + 13];
			composite_system_complex_vector[1][index] = fft_32k->fftw_complex_out_system_vector[1][index + 13];

		}

		for (int index = 0; index < 21; index++) {

			composite_ref_complex_vector[0][index + 70] = fft_8k->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 70] = fft_8k->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 70] = fft_8k->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 70] = fft_8k->fftw_complex_out_system_vector[1][index + 21];


			composite_ref_complex_vector[0][index + 91] = fft_4k->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 91] = fft_4k->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 91] = fft_4k->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 91] = fft_4k->fftw_complex_out_system_vector[1][index + 21];


			composite_ref_complex_vector[0][index + 112] = fft_2k->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 112] = fft_2k->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 112] = fft_2k->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 112] = fft_2k->fftw_complex_out_system_vector[1][index + 21];

			composite_ref_complex_vector[0][index + 133] = fft_1k->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 133] = fft_1k->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 133] = fft_1k->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 133] = fft_1k->fftw_complex_out_system_vector[1][index + 21];


			composite_ref_complex_vector[0][index + 154] = fft_512->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 154] = fft_512->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 154] = fft_512->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 154] = fft_512->fftw_complex_out_system_vector[1][index + 21];


			composite_ref_complex_vector[0][index + 175] = fft_256->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 175] = fft_256->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 175] = fft_256->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 175] = fft_256->fftw_complex_out_system_vector[1][index + 21];

			composite_ref_complex_vector[0][index + 196] = fft_128->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 196] = fft_128->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 196] = fft_128->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 196] = fft_128->fftw_complex_out_system_vector[1][index + 21];

		}

		for (int index = 0; index < 7; index++) {

			composite_ref_complex_vector[0][index + 217] = fft_64->fftw_complex_out_ref_vector[0][index + 21];
			composite_ref_complex_vector[1][index + 217] = fft_64->fftw_complex_out_ref_vector[1][index + 21];

			composite_system_complex_vector[0][index + 217] = fft_64->fftw_complex_out_system_vector[0][index + 21];
			composite_system_complex_vector[1][index + 217] = fft_64->fftw_complex_out_system_vector[1][index + 21];

		}

	}
	
	void assemble_composite_fft_bin_frequencies() {

		int fft32k_bin_frequencies[70] = { 19,21,22,23,25,26,28,
			29,31,32,34,35,37,38,40,41,42,
			44,45,47,48,50,51,53,54,56,57,
			59,60,62,63,64,66,67,69,70,72,
			73,75,76,78,79,81,82,83,85,86,
			88,89,91,92,94,95,97,98,100,101,
			103,104,105,107,108,110,111,113,114,116,
			117,119,120 }; //starting at FFT32k bin 13, ending at FFT32k bin 82

		int fft8k_bin_frequencies[21] = { 123,129,135,141,146,152,158,164,170,
			176,182,188,193,199,205,211,217,223,229,
			234,240 }; //starting at FFT8k bin 21, ending at FFT8k bin 41

		int fft4k_bin_frequencies[21] = { 246,258,270,281,293,305,316,328,340,
			352,363,375,387,398,410,422,434,445,457,
			469,480 }; //starting at FFT4k bin 21, ending at FFT4k bin 41

		int fft2k_bin_frequencies[21] = { 492,516,539,563,586,609,633,656,680,
			703,727,750,773,797,820,844,867,891,914,
			938,961 }; //starting at FFT2k bin 21, ending at FFT2k bin 41

		int fft1k_bin_frequencies[21] = { 984,1031,1078,1125,1172,1219,1266,1313,1359,
			1406,1453,1500,1547,1594,1641,1688,1734,1781,1828,
			1875,1922 }; //starting at FFT1k bin 21, ending at FFT1k bin 41

		int fft512_bin_frequencies[21] = { 1969,2063,2156,2250,2344,2438,2531,2625,2719,
			2813,2906,3000,3094,3188,3281,3375,3469,3563,3656,
			3750,3844 }; //starting at FFT512 bin 21, ending at FFT512 bin 41

		int fft256_bin_frequencies[21] = { 3938,4125,4313,4500,4688,4875,5063,5250,5438,
			5625,5813,6000,6188,6375,6563,6750,6938,7125,7313,
			7500,7688 }; //starting at FFT256 bin 21, ending at FFT256 bin 41

		int fft128_bin_frequencies[21] = { 7875,8250,8625,9000,9375,9750,10125,10500,10875,
			11250,11625,12000,12375,12750,13125,13500,13875,14250,14625,
			15000,15375 }; //starting at FFT128 bin 21, ending at FFT128 bin 41

		int fft64_bin_frequencies[7] = { 15750,16500,17250,18000,18750,19500,20250 };
		//starting at FFT64 bin 21, ending at FFT64 bin 27

		for (int index = 0; index < 70; index++) {

			composite_fft_bin_frequencies[index] = fft32k_bin_frequencies[index];

		}

		for (int index = 0; index < 21; index++) {

			composite_fft_bin_frequencies[index +70] = fft8k_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 91] = fft4k_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 112] = fft2k_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 133] = fft1k_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 154] = fft512_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 175] = fft256_bin_frequencies[index];

			composite_fft_bin_frequencies[index + 196] = fft128_bin_frequencies[index];

		}

		for (int index = 0; index < 7; index++) {

			composite_fft_bin_frequencies[index + 217] = fft64_bin_frequencies[index];

		}

	}

	void apply_mic_and_system_curves(std::vector<double> & source_vector, std::vector<double> & destination_vector) {

		if (curves_only == 0) {

			for (int x = 0; x < composite_fft_bins; x++) {

				destination_vector[x] =

					source_vector[x] -

					interpolated_mic_cal_amplitudes[x] -

					interpolated_system_curve_amplitudes[x];

			}

		}

		else {

			for (int x = 0; x < composite_fft_bins; x++) {

				destination_vector[x] =
					
					interpolated_mic_cal_amplitudes[x] +

					interpolated_system_curve_amplitudes[x];

			}

		}

	}

	//void calc_system_spectrum() {

	//	for (int col = 0; col < spectrum_fft_bins; col++) {

	//		system_spectrum_fft_bin_frequencies[col] = fft_4k->fft_bin_frequencies[col];

	//		system_spectrum_mag_linear[col] = 
	//			
	//			sqrt(pow(fft_4k->fftw_complex_out_system_vector[0][col], 2) +
	//				
	//				pow(fft_4k->fftw_complex_out_system_vector[1][col], 2));

	//		system_spectrum_mag_dB[col] = 20 * log10(system_spectrum_mag_linear[col]);

	//	}

	//}

	void process_fft_data() {

		complex_conjugate_system();
		complex_conjugate_ref();
		calc_ref_autospectrum();
		calc_system_autospectrum();
		calc_cross_spectrum();
		calc_xfer_function();
		apply_mic_and_system_curves(composite_xfer_function_mag_dB_uncal, composite_xfer_function_mag_dB_cal);
		smooth_xfer_function_data();

	}
	
	void complex_conjugate_ref() {

		std::copy(composite_ref_complex_vector[0].begin(), composite_ref_complex_vector[0].begin() + composite_fft_bins, composite_ref_complex_conjugate[0].begin());

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_ref_complex_conjugate[1][col] = -composite_ref_complex_vector[1][col];
		}

	}

	void complex_conjugate_system() {

		std::copy(composite_system_complex_vector[0].begin(), composite_system_complex_vector[0].begin() + composite_fft_bins, composite_system_complex_conjugate[0].begin());

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_system_complex_conjugate[1][col] = -composite_system_complex_vector[1][col];
		}

	}

	void calc_ref_autospectrum() {

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_ref_autospectrum[col] =
				((composite_ref_complex_vector[0][col])*(composite_ref_complex_conjugate[0][col]) -
				((composite_ref_complex_vector[1][col])*(composite_ref_complex_conjugate[1][col])));
		}
	}

	void calc_system_autospectrum() {

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_system_autospectrum[col] =
				((composite_system_complex_vector[0][col])*(composite_system_complex_conjugate[0][col]) -
				((composite_system_complex_vector[1][col])*(composite_system_complex_conjugate[1][col])));
		}
	}

	void calc_cross_spectrum() {

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_cross_spectrum_complex[0][col] =
				((composite_ref_complex_conjugate[0][col])*(composite_system_complex_vector[0][col]) -
				(composite_ref_complex_conjugate[1][col])*(composite_system_complex_vector[1][col]));
		}

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_cross_spectrum_complex[1][col] =
				((composite_ref_complex_conjugate[0][col])*(composite_system_complex_vector[1][col]) +
				(composite_ref_complex_conjugate[1][col])*(composite_system_complex_vector[0][col]));
		}

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_cross_spectrum_magnitude[col] = sqrt(pow(composite_cross_spectrum_complex[0][col], 2) + pow(composite_cross_spectrum_complex[1][col], 2));
		}
		int x = 0;
	}

	void calc_xfer_function() {

		double xfer_function_magnitude_linear;

		double xfer_function_phase_radians;

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_xfer_function_complex[0][col] = composite_cross_spectrum_complex[0][col] / composite_ref_autospectrum[col];
		}

		for (int col = 0; col < composite_fft_bins; col++) {
			composite_xfer_function_complex[1][col] = composite_cross_spectrum_complex[1][col] / composite_ref_autospectrum[col];
		}

		for (int col = 0; col < composite_fft_bins; col++) {
			xfer_function_magnitude_linear = sqrt(pow(composite_xfer_function_complex[0][col], 2) + pow(composite_xfer_function_complex[1][col], 2));
			composite_xfer_function_mag_dB_uncal[col] = 20 * log10(xfer_function_magnitude_linear);
		}

		smooth_xfer_function_data();

		for (int col = 0; col < composite_fft_bins; col++) {

			xfer_function_phase_radians = -atan2(composite_xfer_function_complex[1][col], composite_xfer_function_complex[0][col]);
			composite_xfer_function_phase_deg[col] = xfer_function_phase_radians / 0.0174533;
		}

	}

	void update_histories() {

		for (int col = 0; col < composite_fft_bins; col++) {

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

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_ref_autospectrum_avg[col] =
				(composite_ref_autospectrum_history[0][col] +
					composite_ref_autospectrum_history[1][col] +
					composite_ref_autospectrum_history[2][col] +
					composite_ref_autospectrum_history[3][col]) / 4;

		}

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_system_autospectrum_avg[col] =
				(composite_system_autospectrum_history[0][col] +
					composite_system_autospectrum_history[1][col] +
					composite_system_autospectrum_history[2][col] +
					composite_system_autospectrum_history[3][col]) / 4;

		}

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_cross_spectrum_mag_avg[col] =
				(composite_cross_spectrum_mag_history[0][col] +
					composite_cross_spectrum_mag_history[1][col] +
					composite_cross_spectrum_mag_history[2][col] +
					composite_cross_spectrum_mag_history[3][col]) / 4;

		}

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_xfer_function_mag_dB_avg[col] =
				(composite_xfer_function_mag_dB_history[0][col] +
					composite_xfer_function_mag_dB_history[1][col] +
					composite_xfer_function_mag_dB_history[2][col] +
					composite_xfer_function_mag_dB_history[3][col]) / 4;

		}

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_xfer_function_phase_deg_avg[col] =
				(composite_xfer_function_phase_deg_history[0][col] +
					composite_xfer_function_phase_deg_history[1][col] +
					composite_xfer_function_phase_deg_history[2][col] +
					composite_xfer_function_phase_deg_history[3][col]) / 4;

		}

	}

	void calculate_coherence() {

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_coherence_value[col] = (pow(composite_cross_spectrum_mag_avg[col], 2)) /
				((composite_ref_autospectrum_avg[col])*(composite_system_autospectrum_avg[col]));

		}
	}

	void calc_ref_spectrum() {

		int lowest_composite_frequency = composite_fft_bin_frequencies[0];

		float octaves_distance = 0.0;

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_ref_spectrum_mag_dB[col] = 20 * log10(composite_ref_autospectrum_avg[col]);

			composite_ref_spectrum_mag_dB[col] = (composite_ref_spectrum_mag_dB[col])*(spectrum_scaling_coefficient)+spectrum_offset_db;

			octaves_distance = log(composite_fft_bin_frequencies[col] / lowest_composite_frequency);

			composite_ref_spectrum_mag_dB[col] = (composite_ref_spectrum_mag_dB[col]) + (octaves_distance)*(spectrum_slope_db_octave);
			
		}
		
	}

	void smooth_xfer_function_data() {

		if (smoothing_passes > 0) {

			composite_data_smoother.configure(composite_fft_bins, smoothing_average_size);
			
			for (int x = 0; x < smoothing_passes; x++) {
							
				composite_data_smoother.process(composite_xfer_function_complex[0]);
				composite_data_smoother.process(composite_xfer_function_complex[1]);

				composite_data_smoother.process(composite_xfer_function_mag_dB_cal);

			}

		}

		else {};

	}

	void smooth_spectrum_data() {

		if (smoothing_passes > 0) {

			composite_data_smoother.configure(composite_fft_bins, smoothing_average_size);

			for (int x = 0; x < smoothing_passes; x++) {

				composite_data_smoother.process(composite_ref_spectrum_mag_dB);

			}

		}

		else {};

	}

	void update_mic_system_curves() {

		if (mic_cal_path_in_use != mic_cal_path) {
			mic_cal_path_in_use = mic_cal_path;
			load_curve(mic_cal_path_in_use, original_mic_cal_frequencies, original_mic_cal_amplitudes);
			interpolate_curve(original_mic_cal_frequencies, original_mic_cal_amplitudes, interpolated_mic_cal_amplitudes);
		}

		else {};

		if (system_curve_path_in_use != system_curve_path) {
			system_curve_path_in_use = system_curve_path;
			load_curve(system_curve_path_in_use, original_system_curve_frequencies, original_system_curve_amplitudes);
			interpolate_curve(original_system_curve_frequencies, original_system_curve_amplitudes, interpolated_system_curve_amplitudes);
		}

		else {};
			
	}

	void load_curve(std::string curve_path, std::vector<double> & original_curve_frequencies, std::vector<double> & original_curve_amplitudes) {

		std::string blank;

		std::string current_line;

		char frequency[8];

		double frequency_double;

		char amplitude[8];

		double amplitude_double;

		int numlines = 0;

		int tab_pos = 0;

		std::ifstream curve(curve_path);

		while (std::getline(curve, blank)) {

			numlines++;

		}

		curve.clear();

		curve.seekg(0, curve.beg);

		for (int x = 0; x < numlines; x++) {

			std::getline(curve, current_line);

			tab_pos = current_line.find('\t');

			current_line.copy(frequency, tab_pos);

			current_line.copy(amplitude, current_line.length() - tab_pos, tab_pos + 1);

			original_curve_frequencies.push_back(atof(frequency));

			original_curve_amplitudes.push_back(atof(amplitude));

		}

	}

	void interpolate_curve(std::vector<double> & original_frequencies, std::vector<double> & original_amplitudes, std::vector<double> & interpolated_amplitudes) {

		for (int x = 0; x < composite_fft_bins; x++) {

			if (composite_fft_bin_frequencies[x] < original_frequencies[0]) {

				interpolated_amplitudes[x] = original_amplitudes[0];

			}

			if (composite_fft_bin_frequencies[x] > original_frequencies[original_frequencies.size() - 1]) {

				interpolated_amplitudes[x] = original_amplitudes[original_amplitudes.size() - 1];

			}

			if (composite_fft_bin_frequencies[x] >= original_frequencies[0] &&
				composite_fft_bin_frequencies[x] <= original_frequencies[original_frequencies.size() - 1]) {

				int composite_fft_bin_frequency = composite_fft_bin_frequencies[x];

				int current_pos = 0;
				int x0_pos = 0;
				int x1_pos = 0;

				while (1) {

					current_pos++;

					if (current_pos >= original_frequencies.size() - 1) {

						x0_pos = original_frequencies.size() - 1;

						break;

					}

					if (original_frequencies[current_pos] > composite_fft_bin_frequency) {

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

					if (original_frequencies[current_pos] < composite_fft_bin_frequency) {

						x0_pos = current_pos;

						break;

					}

				}

				interpolated_amplitudes[x] = original_amplitudes[x0_pos] +

					(composite_fft_bin_frequency - original_frequencies[x0_pos])*

					((original_amplitudes[x1_pos] - original_amplitudes[x0_pos]) / (original_frequencies[x1_pos] - original_frequencies[x0_pos]));

			}

		}

	};

	//void interpolate_mic_cal_amplitudes() {

	//	for (int x = 0; x < composite_fft_bins; x++) {

	//		if (composite_fft_bin_frequencies[x] < original_mic_cal_frequencies[0]) {

	//			interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[0];

	//		}

	//		if (composite_fft_bin_frequencies[x] > original_mic_cal_frequencies[original_mic_cal_frequencies.size() - 1]) {

	//			interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[original_mic_cal_frequencies.size() - 1];

	//		}

	//		if (composite_fft_bin_frequencies[x] >= original_mic_cal_frequencies[0] &&
	//			composite_fft_bin_frequencies[x] <= original_mic_cal_frequencies[original_mic_cal_frequencies.size() - 1]) {

	//			int composite_fft_bin_frequency = composite_fft_bin_frequencies[x];

	//			int current_pos = 0;
	//			int x0_pos = 0;
	//			int x1_pos = 0;

	//			while (1) {

	//				current_pos++;

	//				if (current_pos >= original_mic_cal_frequencies.size() - 1) {

	//					x0_pos = original_mic_cal_frequencies.size() - 1;

	//					break;

	//				}

	//				if (original_mic_cal_frequencies[current_pos] > composite_fft_bin_frequency) {

	//					x1_pos = current_pos;

	//					break;

	//				}

	//			}

	//			while (1) {

	//				current_pos--;

	//				if (current_pos <= 0) {

	//					x0_pos = 0;

	//					break;

	//				}

	//				if (original_mic_cal_frequencies[current_pos] < composite_fft_bin_frequency) {

	//					x0_pos = current_pos;

	//					break;

	//				}

	//			}

	//			interpolated_mic_cal_amplitudes[x] = original_mic_cal_amplitudes[x0_pos] +

	//				(composite_fft_bin_frequency - original_mic_cal_frequencies[x0_pos])*

	//				((original_mic_cal_amplitudes[x1_pos] - original_mic_cal_amplitudes[x0_pos]) / (original_mic_cal_frequencies[x1_pos] - original_mic_cal_frequencies[x0_pos]));

	//		}

	//	}

	//};
	//
	//void interpolate_system_curve_amplitudes() {

	//	for (int x = 0; x < composite_fft_bins; x++) {

	//		if (composite_fft_bin_frequencies[x] < original_system_curve_frequencies[0]) {

	//			interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[0];

	//		}

	//		if (composite_fft_bin_frequencies[x] > original_system_curve_frequencies[original_system_curve_frequencies.size() - 1]) {

	//			interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[original_system_curve_frequencies.size() - 1];

	//		}

	//		if (composite_fft_bin_frequencies[x] >= original_system_curve_frequencies[0] &&
	//			composite_fft_bin_frequencies[x] <= original_system_curve_frequencies[original_system_curve_frequencies.size() - 1]) {

	//			int composite_fft_bin_frequency = composite_fft_bin_frequencies[x];

	//			int current_pos = 0;
	//			int x0_pos = 0;
	//			int x1_pos = 0;

	//			while (1) {

	//				current_pos++;

	//				if (current_pos >= original_system_curve_frequencies.size() - 1) {

	//					x0_pos = original_system_curve_frequencies.size() - 1;

	//					break;

	//				}

	//				if (original_system_curve_frequencies[current_pos] > composite_fft_bin_frequency) {

	//					x1_pos = current_pos;

	//					break;

	//				}

	//			}

	//			while (1) {

	//				current_pos--;

	//				if (current_pos <= 0) {

	//					x0_pos = 0;

	//					break;

	//				}

	//				if (original_system_curve_frequencies[current_pos] < composite_fft_bin_frequency) {

	//					x0_pos = current_pos;

	//					break;

	//				}

	//			}

	//			interpolated_system_curve_amplitudes[x] = original_system_curve_amplitudes[x0_pos] +

	//				(composite_fft_bin_frequency - original_system_curve_frequencies[x0_pos])*

	//				((original_system_curve_amplitudes[x1_pos] - original_system_curve_amplitudes[x0_pos]) / 
	//				
	//				(original_system_curve_frequencies[x1_pos] - original_system_curve_frequencies[x0_pos]));

	//		}

	//	}
	//	
	//}

	void run_calcs_for_SPL_and_meters() {

		peak_of_current_ref_samples = *std::max_element(current_ref_samples.begin(), current_ref_samples.end());

		peak_of_current_system_samples = *std::max_element(current_system_samples.begin(), current_system_samples.end());

		current_system_samples_squared = current_system_samples;

		for (int x = 0; x < current_ref_samples.size(); x++) {

			current_system_samples_squared[x] = pow(current_system_samples_squared[x], 2);

		}

		current_system_samples_squared_sum = std::accumulate(current_system_samples_squared.begin(), current_system_samples_squared.end(), 0.0);

		current_system_samples_squared_mean = current_system_samples_squared_sum / current_ref_samples.size();

		current_system_samples_RMS = pow(current_system_samples_squared_mean, 0.5);

	}

};