/*
  ==============================================================================

    fft.h
    Created: 17 Sep 2017 2:57:29pm
    Author:  John

  ==============================================================================
*/

//Complex arrays use the following format: [Re][0], [Imag][1]

//Complex vectors use the following format: [0][Re], [1][Imag]

#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <fftw3.h>
#include <array>
#include "constants.h"
#include <vector>
#include <deque>

class fft : public constants
{

public:

	const int local_fft_size;
	int local_fft_bins;

	std::vector<double> hann_window_weights;

	/*---------------------------------------------------*/

	std::vector<double> local_ref_samples;
	std::vector<double> local_system_samples;

	/*---------------------------------------------------*/

	double *fftw_samples_dynamic;

	fftw_complex *out;

	fftw_plan plan;

	/*---------------------------------------------------*/

	std::vector<std::vector<double>> fftw_complex_out_ref_vector;
	std::vector<std::vector<double>> fftw_complex_out_system_vector;

	/*---------------------------------------------------*/

	fft(int fft_size) : local_fft_size(fft_size)
	{
		local_fft_bins = (local_fft_size / 2) + 1;

		/*---------------------------------------------------*/ //set sizes of vectors

		hann_window_weights.resize(local_fft_size);
		local_ref_samples.resize(local_fft_size);
		local_system_samples.resize(local_fft_size);

		fftw_complex_out_ref_vector.resize(2, std::vector<double>(local_fft_bins)); //two rows, each row has as many columns as there are fft bins
		fftw_complex_out_system_vector.resize(2, std::vector<double>(local_fft_bins));

		/*---------------------------------------------------*/

		generate_hann_window_weights_array();

		/*---------------------------------------------------*/

		fftw_samples_dynamic = new double[local_fft_size];

		out = fftw_alloc_complex(local_fft_size);
		plan = fftw_plan_dft_r2c_1d(local_fft_size, fftw_samples_dynamic, out, FFTW_MEASURE);

	};

	~fft() {

		fftw_destroy_plan(plan);
		fftw_free(out);
		delete[]fftw_samples_dynamic;

	};

	void run_fft_analysis(std::vector<double> &ref_samples_in, std::vector<double> &system_samples_in) {

		std::copy(ref_samples_in.begin(), ref_samples_in.begin() + local_fft_size, local_ref_samples.begin());
		std::copy(system_samples_in.begin(), system_samples_in.begin() + local_fft_size, local_system_samples.begin());

		window_samples(local_ref_samples);
		window_samples(local_system_samples);

		run_fftw(local_ref_samples, fftw_complex_out_ref_vector);
		run_fftw(local_system_samples, fftw_complex_out_system_vector);

		normalize_fftw_output();

	}

private:

	void generate_hann_window_weights_array() {

		for (int n = 0; n < local_fft_size; n++) {
			hann_window_weights[n] = 0.5*(1 - cos((2 * 3.141592654*n) / (local_fft_size - 1)));
		}

	}

	void window_samples(std::vector<double> &samples) {

		for (int n = 0; n < local_fft_size; n++) {
			samples[n] = samples[n] * hann_window_weights[n];
		}

	}

	void run_fftw(std::vector<double> &samples, std::vector<std::vector<double>> & destinaton_vector) {

		std::copy(samples.begin(), samples.begin() + local_fft_size, fftw_samples_dynamic);

		fftw_execute(plan);

		for (int row = 0; row < 2; row++) {
			for (int col = 0; col < local_fft_bins; col++) {
				destinaton_vector[row][col] = out[col][row];
			}
		}
	}

	void normalize_fftw_output() {

		for (int col = 0; col < local_fft_bins; col++) {

			fftw_complex_out_ref_vector[0][col] = fftw_complex_out_ref_vector[0][col] / local_fft_size;
			fftw_complex_out_ref_vector[1][col] = fftw_complex_out_ref_vector[1][col] / local_fft_size;
			fftw_complex_out_system_vector[0][col] = fftw_complex_out_system_vector[0][col] / local_fft_size;
			fftw_complex_out_system_vector[1][col] = fftw_complex_out_system_vector[1][col] / local_fft_size;

		}

	}

};