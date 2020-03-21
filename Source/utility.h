
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "r8bsrc.h"

struct linear_interpolator
{

	//This template assumes that the input coordinates are vectors which are presorted in ascending order with respect to x
	//If the passed-in x value is outside the range of the x_coord vector, the closest y_value will be returned
	template <typename V>
	static V interpolate(std::vector<V> & x_coord, std::vector<V> & y_coord, double x) {

		if (x <= x_coord.front()) {

			return y_coord.front();

		}

		if (x >= x_coord.back()) {

			return y_coord.back();

		}

		int x1_index = std::distance(x_coord.begin(), std::upper_bound(x_coord.begin(), x_coord.end(), x));
		int x0_index = x1_index - 1;

		return y_coord[x0_index] + (x - x_coord[x0_index])*((y_coord[x1_index] - y_coord[x0_index]) / (x_coord[x1_index] - x_coord[x0_index]));

	}

};

class sample_rate_converter_r8 //high quality conversion using r8brain-free-src
{
public:

	sample_rate_converter_r8() {};
	~sample_rate_converter_r8() {};

	void configure(int source_sample_rate_Hz,
		std::vector<float> *source_samples,
		int destination_sample_rate_Hz,
		std::vector<float> *destination_samples) {

		source_sample_rate = source_sample_rate_Hz;
		dest_sample_rate = destination_sample_rate_Hz;

		source_buffer = source_samples;
		destination_buffer = destination_samples;

		resampler_r8 = r8b_create(source_sample_rate, dest_sample_rate, source_buffer->size(), 3, ER8BResamplerRes::r8brr16);

		source_samples_double.resize(source_buffer->size());

		reset_converter();

	}

	void resample_samples() {

		std::copy(source_buffer->begin(), source_buffer->end(), source_samples_double.begin());

		double * original_samples = source_samples_double.data();
		double * r8_output_buffer;

		int num_resampled_samples{ 0 };

		num_resampled_samples = r8b_process(resampler_r8, original_samples, source_samples_double.size(), r8_output_buffer);

		destination_buffer->resize(num_resampled_samples);

		std::copy(r8_output_buffer, r8_output_buffer + num_resampled_samples, destination_buffer->begin());

	}

	void reset_converter() {

		r8b_clear(resampler_r8);

	}

private:

	JUCE_LEAK_DETECTOR(sample_rate_converter_r8);

	int source_sample_rate{ 0 };
	int dest_sample_rate{ 0 };

	std::vector<float> *source_buffer, *destination_buffer;
	std::vector<double> source_samples_double;

	CR8BResampler resampler_r8;

};

class sample_rate_converter //works but resampling quality is relatively poor
{
public:

	sample_rate_converter() {};
	~sample_rate_converter() {};

	void configure(	int source_sample_rate_Hz, 
					std::vector<float> *source_samples, 
					int destination_sample_rate_Hz, 
					std::vector<float> *destination_samples ) {

		reset_converter();

		source_sample_rate = source_sample_rate_Hz;
		dest_sample_rate = destination_sample_rate_Hz;

		source_buffer = source_samples;
		destination_buffer = destination_samples;

		resample_ratio = source_sample_rate * 1.0 / dest_sample_rate * 1.0;

		needed_destination_samples = floor((source_samples->size() * 1.0) / resample_ratio);

		destination_buffer->resize(needed_destination_samples);

		get_fir_coefficients();
		generate_fir_filter();

		src_aa_spec.sampleRate = source_sample_rate;
		src_aa_spec.maximumBlockSize = source_samples->size();
		src_aa_spec.numChannels = 1;

		src_aa_filter.prepare(src_aa_spec);

	}

	void resample_samples() {

		float * const source_raw_samples = source_buffer->data();
		float * const dest_raw_samples = destination_buffer->data();

		dsp::AudioBlock<float> sample_block(&source_raw_samples, 1, source_buffer->size());
		dsp::ProcessContextReplacing<float> src_aa_context(sample_block);

		src_aa_filter.process(src_aa_context);

		interpolator.process(resample_ratio, source_raw_samples, dest_raw_samples, needed_destination_samples, source_buffer->size(), 0);

	}

	void reset_converter() {

		interpolator.reset();
		src_aa_filter.reset();

	}

private:

	JUCE_LEAK_DETECTOR(sample_rate_converter);
	
	int source_sample_rate{ 0 };
	int dest_sample_rate{ 0 };
	int needed_destination_samples{ 0 };
	double resample_ratio{ 1.0 };
	
	std::vector<float> *source_buffer, *destination_buffer;

	CatmullRomInterpolator interpolator;

	int aa_filter_order{ 64 };
	dsp::FIR::Filter<float> src_aa_filter;
	dsp::FIR::Coefficients<float>::Ptr src_aa_coefficients;
	dsp::ProcessSpec src_aa_spec;

	void get_fir_coefficients() {

		src_aa_coefficients =

			dsp::FilterDesign<float>::designFIRLowpassWindowMethod(
				20000,
				source_sample_rate,
				aa_filter_order,
				dsp::WindowingFunction<float>::hann);

	}

	void generate_fir_filter() {

		src_aa_filter = dsp::FIR::Filter<float>::Filter(src_aa_coefficients);

	}

};

class lanczos_resampler
{
public:
	
	lanczos_resampler() {};
	~lanczos_resampler() {};

private:

};
