
#pragma once

#include <fftw3.h>
#include <array>
#include "constants.h"
#include <vector>
#include <deque>

class ifft : public constants
{

public:

	ifft(int ifft_size) : local_ifft_size(ifft_size) //size is specified as number of output samples, not number of freq bins
	{
		
		ifft_num_output_samples = ifft_size;
		
		ifft_num_input_bins = (ifft_num_output_samples / 2) + 1;
		
		output_samples = new double[ifft_num_output_samples];

		input_freq_response = fftw_alloc_complex(ifft_num_input_bins);

		plan = fftw_plan_dft_c2r_1d(ifft_num_output_samples, input_freq_response, output_samples, FFTW_MEASURE);

	};

	~ifft() {

		fftw_destroy_plan(plan);
		fftw_free(input_freq_response);
		delete[]output_samples;

	};

	void set_freq_response_data(std::vector<std::vector<double>> & complex_freq_response_data) {
		
		for (int bin = 0; bin < ifft_num_input_bins; bin++) {

			input_freq_response[bin][0] = complex_freq_response_data[0][bin]; //real part
			input_freq_response[bin][1] = complex_freq_response_data[1][bin]; //imaginary part

		}

		for (int bin = 0; bin < ifft_num_input_bins; bin++) { //scaled per instructions in 
			                                                 //The Scientist and Engineer's Guide to Digital Signal Processing

			input_freq_response[bin][0] = input_freq_response[bin][0] / (ifft_num_output_samples / 2);
			input_freq_response[bin][1] = -input_freq_response[bin][1] / (ifft_num_output_samples / 2);
			
		}

		input_freq_response[0][0] = input_freq_response[0][0] / 2;
		input_freq_response[ifft_num_input_bins-1][0] = input_freq_response[ifft_num_input_bins - 1][0] / 2;

	}

	void run_ifft() {

		fftw_execute(plan);

		//normalize_output_samples();

	}

	void get_output_samples(std::vector<float> &destination_vector) {

		for (int sample = 0; sample < ifft_num_output_samples; sample++) {

			destination_vector[sample] = output_samples[sample];

		}

	}

private:

	JUCE_LEAK_DETECTOR(ifft);

	int local_ifft_size;
	
	int ifft_num_output_samples, ifft_num_input_bins;

	fftw_complex *input_freq_response;

	double *output_samples;

	fftw_plan plan;

	void prep_input() {



	}

	void normalize_output_samples() {

		float max_value{ 0 };
		float min_value{ 0 };
		float largest_value{ 0 };

		for (int sample = 0; sample < ifft_num_output_samples; sample++) {

			if(output_samples[sample] > max_value){
				
				max_value = output_samples[sample];
			
			}

			else if (output_samples[sample] < min_value) {

				min_value = output_samples[sample];

			}

			else {}

		}

		largest_value = std::max(max_value, abs(min_value));

		for (int sample = 0; sample < ifft_num_output_samples; sample++) {

			output_samples[sample] = output_samples[sample] / largest_value;

		}

	}

};