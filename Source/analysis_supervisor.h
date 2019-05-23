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
#include "ifft.h"
#include "constants.h"
#include "data_history.h"
#include <algorithm>
#include "xfer_func.h"

class supervisor : public constants, public Timer, public Thread, public ActionBroadcaster

{

public:

	//Complex vectors use the following format: [0][Re], [1][Imag]

	/*---------------------------------------------------*/

	std::vector<double> composite_fft_bin_frequencies;

	std::vector<std::vector<double>> composite_ref_complex_vector, composite_system_complex_vector;

	std::vector<double> composite_cross_spectrum_magnitude;

	std::vector<std::vector<double>> composite_xfer_function_complex;

	std::vector<double> composite_ref_autospectrum_avg, composite_system_autospectrum_avg;

	std::vector<double> composite_cross_spectrum_mag_avg;

	std::vector<double> composite_xfer_function_real_data_avg, composite_xfer_function_imag_data_avg;
	
	std::vector<double> composite_xfer_function_mag_dB_uncal, composite_xfer_function_mag_dB_cal;
	
	std::vector<double> composite_xfer_function_phase_deg;
		
	std::vector<double> composite_coherence_value;

	std::vector<double> composite_system_spectrum_mag_linear, composite_system_spectrum_mag_dB;

	std::vector<std::vector<double>> impulse_response_complex_freq_samples;

	std::vector<float> impulse_response_time_samples;

	/*---------------------------------------------------*/

	std::deque<double> buffer_ref_samples, buffer_system_samples;

	std::vector<double> current_ref_samples, undelayed_ref_samples, current_system_samples;
	
	std::vector<double> current_ref_samples_squared;
	double current_ref_samples_squared_sum;
	double current_ref_samples_squared_mean;
	double current_ref_samples_RMS;

	std::vector<double> current_system_samples_squared;
	double current_system_samples_squared_sum;
	double current_system_samples_squared_mean;
	double current_system_samples_RMS;

	/*---------------------------------------------------*/

	int delay_in_samples{ 0 };

	/*---------------------------------------------------*/

	std::string mic_cal_path, mic_cal_path_in_use;

	std::vector<double> original_mic_cal_frequencies, 
						original_mic_cal_amplitudes, 
						interpolated_mic_cal_amplitudes;

	/*---------------------------------------------------*/

	std::string system_curve_path, system_curve_path_in_use;

	std::vector<double> original_system_curve_frequencies, 
						original_system_curve_amplitudes, 
						interpolated_system_curve_amplitudes;

	/*---------------------------------------------------*/

	std::mutex audio_buffer_mtx_supervisor, plot_data_mtx_supervisor;

	int analyser_update_rate_hz{ 1 };

	bool analysis_cycle_active{ false }, analyser_run{ false };

	int smoothing_average_size{ 3 }, smoothing_passes{ 0 };

	float spectrum_offset_db{ 12.0 }, spectrum_scaling_coefficient{ 0.5 }, spectrum_slope_db_octave = -4.5;

	bool curves_only{ false };

	int num_averages{ 2 };

	supervisor(): Thread("Supervisor_Thread",0)
	
	{
						
		composite_ref_complex_vector.resize(2, std::vector<double>(composite_fft_bins)); //two rows, each row has as many columns as there are composite fft bins
		composite_system_complex_vector.resize(2, std::vector<double>(composite_fft_bins));

		composite_xfer_function_complex.resize(2, std::vector<double>(composite_fft_bins));
		composite_xfer_function_mag_dB_uncal.resize(composite_fft_bins);
		composite_xfer_function_mag_dB_cal.resize(composite_fft_bins);
		composite_xfer_function_phase_deg.resize(composite_fft_bins);

		composite_ref_autospectrum_avg.resize(composite_fft_bins);

		composite_system_autospectrum_avg.resize(composite_fft_bins);

		composite_cross_spectrum_mag_avg.resize(composite_fft_bins);

		composite_xfer_function_real_data_avg.resize(composite_fft_bins);
		composite_xfer_function_imag_data_avg.resize(composite_fft_bins);

		composite_coherence_value.resize(composite_fft_bins);

		composite_fft_bin_frequencies.resize(composite_fft_bins);

		composite_system_spectrum_mag_linear.resize(composite_fft_bins);
		composite_system_spectrum_mag_dB.resize(composite_fft_bins);

		impulse_response_complex_freq_samples.resize(2, std::vector<double>((largest_fft_size / 2) + 1));
		impulse_response_time_samples.resize(largest_fft_size);

		interpolated_mic_cal_amplitudes.resize(composite_fft_bins);

		interpolated_system_curve_amplitudes.resize(composite_fft_bins);

		buffer_ref_samples.resize(largest_fft_size + max_delay_samples);
		buffer_system_samples.resize(largest_fft_size);

		current_ref_samples.resize(largest_fft_size);
		undelayed_ref_samples.resize(largest_fft_size);
		current_system_samples.resize(largest_fft_size);

		current_ref_samples_squared.resize(largest_fft_size);
		current_system_samples_squared.resize(largest_fft_size);

		assemble_composite_fft_bin_frequencies();
				
		startTimer(1000/analyser_update_rate_hz);

		composite_ref_autospectrum_data_history.set_num_histories(num_averages);

		composite_system_autospectrum_data_history.set_num_histories(num_averages);

		composite_xfer_function_real_data_history.set_num_histories(num_averages);

		composite_xfer_function_imag_data_history.set_num_histories(num_averages);

		composite_cross_spectrum_mag_data_history.set_num_histories(num_averages);
		
	};

	~supervisor() {

		this->signalThreadShouldExit();
		this->notify();
		this->waitForThreadToExit(5000);
		
		delete fft_32k;
		delete fft_8k;
		delete fft_4k;
		delete fft_2k;
		delete fft_1k;
		delete fft_512;
		delete fft_256;
		delete fft_128;
		delete fft_64;
		
	};

	void run() override
	
	{

		while (!threadShouldExit()) {

			run_analysis();

			wait(-1); //thread will stop here until notify() is called

		}

	}

	void run_analysis() {

		analysis_cycle_active = true;

		refresh_supervisor_buffer();

		update_mic_system_curves();

		fft_32k_ir->run_fft_analysis(undelayed_ref_samples, current_system_samples);
		
		fft_32k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_8k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_4k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_2k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_1k->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_512->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_256->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_128->run_fft_analysis(current_ref_samples, current_system_samples);
		fft_64->run_fft_analysis(current_ref_samples, current_system_samples);

		assemble_composite_complex_vectors();

		plot_data_mtx_supervisor.lock();
			
		process_fft_data();

		calculate_impulse_response();
		
		calculate_coherence();

		calc_system_spectrum();

		smooth_spectrum_data();
		
		plot_data_mtx_supervisor.unlock();
			
		run_calcs_for_SPL_and_meters();

		analysis_cycle_active = false;

		sendActionMessage("cmd_supervisor_cycle_done");

	}

	void refresh_supervisor_buffer() {

		audio_buffer_mtx_supervisor.lock();

		std::copy(buffer_ref_samples.begin() + delay_in_samples, 
			buffer_ref_samples.begin() + current_ref_samples.size() + delay_in_samples, 
			current_ref_samples.begin());

		std::copy(buffer_ref_samples.begin(),
			buffer_ref_samples.begin() + undelayed_ref_samples.size(),
			undelayed_ref_samples.begin());

		std::copy(buffer_system_samples.begin(), 
			buffer_system_samples.begin() + current_system_samples.size(), 
			current_system_samples.begin());

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

		if (getTimerInterval() != (1000 / analyser_update_rate_hz))
		{

			startTimer(1000 / analyser_update_rate_hz);

		}
		
	}

private:

	fft * fft_32k_ir = new fft(largest_fft_size);
	
	fft * fft_32k = new fft(largest_fft_size);
	fft * fft_8k = new fft(largest_fft_size / 4);
	fft * fft_4k = new fft(largest_fft_size / 8);
	fft * fft_2k = new fft(largest_fft_size / 16);
	fft * fft_1k = new fft(largest_fft_size / 32);
	fft * fft_512 = new fft(largest_fft_size / 64);
	fft * fft_256 = new fft(largest_fft_size / 128);
	fft * fft_128 = new fft(largest_fft_size / 256);
	fft * fft_64 = new fft(largest_fft_size / 512);

	xfer_func composite_xfer_func{ composite_fft_bins };

	xfer_func impulse_reponse_transfer_func{ (largest_fft_size / 2) + 1 };

	ifft impulse_response_calculator{ largest_fft_size };

	MovingAverage1DVector composite_data_smoother;

	MovingAverage2DVector complex_data_smoother;

	data_history composite_ref_autospectrum_data_history{ composite_fft_bins };

	data_history composite_system_autospectrum_data_history{ composite_fft_bins };

	data_history composite_xfer_function_real_data_history{ composite_fft_bins };

	data_history composite_xfer_function_imag_data_history{ composite_fft_bins };

	data_history composite_cross_spectrum_mag_data_history{ composite_fft_bins };

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

	void process_fft_data() {

		composite_xfer_func.run_xfer_func(	composite_ref_complex_vector, 
											composite_system_complex_vector, 
											composite_xfer_function_complex);

		update_histories();
		update_averages();
		
		process_xfer_function_data();
		
		apply_mic_and_system_curves(composite_xfer_function_mag_dB_uncal, composite_xfer_function_mag_dB_cal);

	}

	void calculate_impulse_response() {

		impulse_reponse_transfer_func.run_xfer_func(fft_32k_ir->fftw_complex_out_ref_vector, fft_32k_ir->fftw_complex_out_system_vector, impulse_response_complex_freq_samples);

		impulse_response_calculator.set_freq_response_data(impulse_response_complex_freq_samples);

		impulse_response_calculator.run_ifft();

		impulse_response_calculator.get_output_samples(impulse_response_time_samples);

	}

	void process_xfer_function_data() {
	
		smooth_xfer_function_data(); //complex smoothing

		double xfer_function_magnitude_linear;

		double xfer_function_phase_radians;

		for (int col = 0; col < composite_fft_bins; col++) {
			
			xfer_function_magnitude_linear = sqrt(pow(composite_xfer_function_real_data_avg[col], 2) + pow(composite_xfer_function_imag_data_avg[col], 2));
			composite_xfer_function_mag_dB_uncal[col] = 20 * log10(xfer_function_magnitude_linear);

			xfer_function_phase_radians = -atan2(composite_xfer_function_imag_data_avg[col], composite_xfer_function_real_data_avg[col]);
			composite_xfer_function_phase_deg[col] = xfer_function_phase_radians / 0.0174533;
		}

	}

	void update_histories() {

		composite_ref_autospectrum_data_history.add_latest_values(composite_xfer_func.ref_autospectrum);

		composite_system_autospectrum_data_history.add_latest_values(composite_xfer_func.system_autospectum);

		composite_xfer_function_real_data_history.add_latest_values(composite_xfer_function_complex[0]);

		composite_xfer_function_imag_data_history.add_latest_values(composite_xfer_function_complex[1]);

		composite_cross_spectrum_mag_data_history.add_latest_values(composite_xfer_func.ref_system_cross_spectrum_magnitude);

	}

	void update_averages() {

		composite_ref_autospectrum_data_history.get_data_average(composite_ref_autospectrum_avg);

		composite_system_autospectrum_data_history.get_data_average(composite_system_autospectrum_avg);

		composite_cross_spectrum_mag_data_history.get_data_average(composite_cross_spectrum_mag_avg);

		composite_xfer_function_real_data_history.get_data_average(composite_xfer_function_real_data_avg);

		composite_xfer_function_imag_data_history.get_data_average(composite_xfer_function_imag_data_avg);

	}

	void calculate_coherence() {

		for (int col = 0; col < composite_fft_bins; col++) {

			composite_coherence_value[col] = (pow(composite_cross_spectrum_mag_avg[col], 2)) /
				((composite_ref_autospectrum_avg[col])*(composite_system_autospectrum_avg[col]));

		}
	}

	void calc_system_spectrum() {

		for (int index = 0; index < 70; index++) {

			composite_system_spectrum_mag_linear[index] = sqrt(pow(composite_system_complex_vector[0][index], 2) + pow(composite_system_complex_vector[1][index], 2)) * 1.0; //from fft_32k

		}

		/* The final constants used on the right side of the equations in the following lines was calculated with the following equation:
		
		constant = sqrt(N1/N2), where N1 is the largest fft size (currently 32768), and N2 is the size of the fft used for that
		region of the composite amplitude spectrum. 

		The constants correct for the usage of different fft sizes when assembling a composite amplitude spectrum. The equation
		used to calculate the constants was determined empirically.
		
		*/

		for (int index = 0; index < 21; index++) {

			composite_system_spectrum_mag_linear[index + 70] = sqrt(pow(composite_system_complex_vector[0][index + 70], 2) + pow(composite_system_complex_vector[1][index + 70], 2)) * 2.0; //from fft_8k
			
			composite_system_spectrum_mag_linear[index + 91] = sqrt(pow(composite_system_complex_vector[0][index + 91], 2) + pow(composite_system_complex_vector[1][index + 91], 2)) * 2.82842712474619; //from fft_4k
			
			composite_system_spectrum_mag_linear[index + 112] = sqrt(pow(composite_system_complex_vector[0][index + 112], 2) + pow(composite_system_complex_vector[1][index + 112], 2)) * 4.0; //from fft_2k

			composite_system_spectrum_mag_linear[index + 133] = sqrt(pow(composite_system_complex_vector[0][index + 133], 2) + pow(composite_system_complex_vector[1][index + 133], 2)) * 5.65685424949238; //from fft_1k
			
			composite_system_spectrum_mag_linear[index + 154] = sqrt(pow(composite_system_complex_vector[0][index + 154], 2) + pow(composite_system_complex_vector[1][index + 154], 2)) * 8.0; //from fft_512

			composite_system_spectrum_mag_linear[index + 175] = sqrt(pow(composite_system_complex_vector[0][index + 175], 2) + pow(composite_system_complex_vector[1][index + 175], 2)) * 11.31370849898476; //from fft_256

			composite_system_spectrum_mag_linear[index + 196] = sqrt(pow(composite_system_complex_vector[0][index + 196], 2) + pow(composite_system_complex_vector[1][index + 196], 2)) * 16.0; //from fft_128

		}

		for (int index = 0; index < 7; index++) {

			composite_system_spectrum_mag_linear[index + 217] = sqrt(pow(composite_system_complex_vector[0][index + 217], 2) + pow(composite_system_complex_vector[1][index + 217], 2)) * 22.62741699796952; //from fft_64

		}
		
		for (int index = 0; index < composite_fft_bins; index++) {

			composite_system_spectrum_mag_dB[index] = 20 * log10(composite_system_spectrum_mag_linear[index]) - 62.0;

		}

	}

	void smooth_xfer_function_data() {

		if (smoothing_passes > 0) {

			composite_data_smoother.configure(composite_fft_bins, smoothing_average_size);

			for (int x = 0; x < smoothing_passes; x++) {

				composite_data_smoother.process(composite_xfer_function_real_data_avg);
				composite_data_smoother.process(composite_xfer_function_imag_data_avg);

				//composite_data_smoother.process(composite_xfer_function_mag_dB_cal);

			}

		}

		else {};

	}

	void smooth_spectrum_data() {

		if (smoothing_passes > 0) {

			composite_data_smoother.configure(composite_fft_bins, smoothing_average_size);

			for (int x = 0; x < smoothing_passes; x++) {

				composite_data_smoother.process(composite_system_spectrum_mag_dB);

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

		int numlines = 0;

		int tab_pos = 0;

		original_curve_frequencies.clear();
		original_curve_amplitudes.clear();

		std::ifstream curve(curve_path);

		while (std::getline(curve, blank)) {

			numlines++;

		}

		curve.clear();

		curve.seekg(0, curve.beg);

		for (int x = 0; x < numlines; x++) {

			std::getline(curve, current_line);
			
			if (current_line.find_first_of("0123456789") == 0) {
				
				tab_pos = current_line.find('\t');

				std::string frequency = current_line.substr(0, tab_pos);

				original_curve_frequencies.push_back(std::stod(frequency, NULL));

				std::string amplitude = current_line.substr(tab_pos + 1, current_line.length() - tab_pos - 1);
				
				original_curve_amplitudes.push_back(std::stod(amplitude, NULL));

			}

			else {};

		}
			
	}

	void interpolate_curve(
		
		std::vector<double> & original_frequencies, 
		std::vector<double> & original_amplitudes, 
		std::vector<double> & interpolated_amplitudes) {

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

					if (original_frequencies[current_pos] > composite_fft_bin_frequency) {

						x1_pos = current_pos;

						break;

					}
					
					if (current_pos >= original_frequencies.size() - 1) {

						x1_pos = original_frequencies.size() - 1;

						break;

					}
					
				}

				while (1) {

					current_pos--;

					if (original_frequencies[current_pos] < composite_fft_bin_frequency) {

						x0_pos = current_pos;

						break;

					}

					if (current_pos <= 0) {

						x0_pos = 0;

						break;

					}
					
				}

				interpolated_amplitudes[x] = original_amplitudes[x0_pos] +

					(composite_fft_bin_frequency - original_frequencies[x0_pos])*

					((original_amplitudes[x1_pos] - original_amplitudes[x0_pos]) / (original_frequencies[x1_pos] - original_frequencies[x0_pos]));

			}

		}

	};

	void run_calcs_for_SPL_and_meters() {

		for (int x = 0; x < current_system_samples_squared.size(); x++) {

			current_ref_samples_squared[x] = pow(current_ref_samples[x], 2);
			current_system_samples_squared[x] = pow(current_system_samples[x], 2);
			
		}

		current_ref_samples_squared_sum = std::accumulate(current_ref_samples_squared.begin(), current_ref_samples_squared.end(), 0.0);
		current_system_samples_squared_sum = std::accumulate(current_system_samples_squared.begin(), current_system_samples_squared.end(), 0.0);


		current_system_samples_squared_mean = current_system_samples_squared_sum / current_system_samples_squared.size();
		current_ref_samples_squared_mean = current_ref_samples_squared_sum / current_ref_samples_squared.size();

		current_ref_samples_RMS = std::sqrt(current_ref_samples_squared_mean);
		current_system_samples_RMS = std::sqrt(current_system_samples_squared_mean);

	}

};