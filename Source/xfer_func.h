#pragma once

#include <vector>
#include <functional>
#include <algorithm>

//Note: many of the equations in this class come from the excellent reference "Dual Channel FFT Analysis (Part I)",
//published by Bruel & Kjaer 

class xfer_func

{

public:

	//Complex vectors use the following format: [0][Real Value], [1][Imaginary Value]

	xfer_func(int freq_bins) :  num_freq_bins(freq_bins)
	{
		/*ref_complex_conjugate.resize(2, std::vector<double>(num_freq_bins));*/
		ref_autospectrum.resize(num_freq_bins);
		system_autospectum.resize(num_freq_bins);
		ref_system_cross_spectrum_complex.resize(2, std::vector<double>(num_freq_bins));
		ref_system_cross_spectrum_magnitude.resize(num_freq_bins);
		ref_system_xfer_func_complex.resize(2, std::vector<double>(num_freq_bins));
	};

	~xfer_func() {};

	void run_xfer_func(std::vector<std::vector<double>> &input_complex_freq_ref, 
						std::vector<std::vector<double>> &input_complex_freq_system,
						std::vector<std::vector<double>> &output_complex_freq_response) 
	{

		/*calc_complex_conjugate(input_complex_freq_ref, ref_complex_conjugate);*/
		
		calc_autospectrum(input_complex_freq_ref, ref_autospectrum);
		calc_autospectrum(input_complex_freq_system, system_autospectum);

		calc_cross_spectrum(input_complex_freq_ref, input_complex_freq_system, ref_system_cross_spectrum_complex, ref_system_cross_spectrum_magnitude);

		calc_xfer_func(ref_system_cross_spectrum_complex, ref_autospectrum, output_complex_freq_response);
	}

	std::vector<double> ref_autospectrum;
	std::vector<double> system_autospectum;
	std::vector<std::vector<double>> ref_system_cross_spectrum_complex;
	std::vector<double> ref_system_cross_spectrum_magnitude;
	std::vector<std::vector<double>> ref_system_xfer_func_complex;

private:

	int num_freq_bins{ 0 };

	/*std::vector<std::vector<double>> ref_complex_conjugate;*/
		
	/*void calc_complex_conjugate(std::vector<std::vector<double>> & input_complex_freq,
								std::vector<std::vector<double>> & output_complex_conjugate) 
	
	{

		std::copy(input_complex_freq[0].begin(), input_complex_freq[0].end(), output_complex_conjugate.begin());
		std::transform(input_complex_freq[1].begin(), input_complex_freq[1].end(), output_complex_conjugate.begin(), std::negate<double>());

	}*/

	void calc_autospectrum(	std::vector<std::vector<double>> & input_complex_freq, 
							std::vector<double> & output_autospectrum)
		
		//Multiplying complex number by its conujugate: (a+bi)(a-bi) = a^2 + b^2 

		//per BK reference, the autospectrum should be multiplied by 2 (see eq. 2.9)
	{

		for (int value = 0; value < output_autospectrum.size(); value++) {

			output_autospectrum[value] = (pow(input_complex_freq[0][value], 2.0) + pow(input_complex_freq[1][value], 2.0)) * 2.0;

		}

	}

	void calc_cross_spectrum(	std::vector<std::vector<double>> & input_ref_complex_freq,
								std::vector<std::vector<double>> & input_sytem_complex_freq,
								std::vector<std::vector<double>> & output_ref_system_cross_spectrum_complex,
								std::vector<double> &output_ref_system_cross_spectrum_magnitude)
	{

		for (int value = 0; value < output_ref_system_cross_spectrum_complex[0].size(); value++) {
			
			//Product of two complex numbers: (x + yi)(u + vi) = (x*u - y*v) + (x*v + y*u)i

			//per BK reference, the cross spectrum should be multiplied by 2 (see eq. 2.9)

			//instead of calculating the complex conjugate of the ref signal separately, we are simpy negating the value of the 
			//imaginary part of the ref signal when it is accessed in these equations

			output_ref_system_cross_spectrum_complex[0][value] =((input_ref_complex_freq[0][value] * input_sytem_complex_freq[0][value]) -
												((-input_ref_complex_freq[1][value]) * input_sytem_complex_freq[1][value])) * 2.0;

			output_ref_system_cross_spectrum_complex[1][value] =((input_ref_complex_freq[0][value] * input_sytem_complex_freq[1][value]) +
												((-input_ref_complex_freq[1][value]) * input_sytem_complex_freq[0][value])) * 2.0;

			output_ref_system_cross_spectrum_magnitude[value] = sqrt(pow(output_ref_system_cross_spectrum_complex[0][value], 2) + 
																pow(output_ref_system_cross_spectrum_complex[1][value], 2));

		}

	}

	void calc_xfer_func(std::vector<std::vector<double>> &input_ref_system_cross_spectrum_complex,
						std::vector<double> &input_ref_autospectrum,
						std::vector<std::vector<double>> &output_ref_system_xfer_func_complex)
	{

		for (int value = 0; value < output_ref_system_xfer_func_complex[0].size(); value++) {

			output_ref_system_xfer_func_complex[0][value] = input_ref_system_cross_spectrum_complex[0][value] / input_ref_autospectrum[value];
			output_ref_system_xfer_func_complex[1][value] = input_ref_system_cross_spectrum_complex[1][value] / input_ref_autospectrum[value];

		}

	}

};