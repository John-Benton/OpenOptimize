#pragma once

#include <vector>
#include <functional>
#include <algorithm>

//Note: many of the equations in this structure come from the excellent reference "Dual Channel FFT Analysis (Part I)",
//published by Bruel & Kjaer 

struct xfer_func
{

	static void calc_autospectrum(	std::vector<std::vector<double>> & input_complex_freq, 
									std::vector<double> & output_autospectrum)
		
		//Multiplying complex number by its conujugate: (a+bi)(a-bi) = a^2 + b^2 

		//per BK reference, the autospectrum should be multiplied by 2 (see eq. 2.9),
		//in order to get the single-sided spectra
	{

		for (int value = 0; value < output_autospectrum.size(); value++) {

			output_autospectrum[value] = (pow(input_complex_freq[0][value], 2.0) + pow(input_complex_freq[1][value], 2.0)) * 2.0;

		}

	}

	static void calc_cross_spectrum(	std::vector<std::vector<double>> & input_ref_complex_freq,
										std::vector<std::vector<double>> & input_sytem_complex_freq,
										std::vector<std::vector<double>> & output_ref_system_cross_spectrum_complex,
										std::vector<double> &output_ref_system_cross_spectrum_magnitude)
	{

		for (int value = 0; value < output_ref_system_cross_spectrum_complex[0].size(); value++) {
			
			//Product of two complex numbers: (x + yi)(u + vi) = (x*u - y*v) + (x*v + y*u)i

			//per BK reference, the cross spectrum should be multiplied by 2 (see eq. 2.9),
			//in order to get the single sided spectra

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

	static void calc_xfer_func(	std::vector<std::vector<double>> &input_ref_system_cross_spectrum_complex,
								std::vector<double> &input_ref_autospectrum,
								std::vector<std::vector<double>> &output_ref_system_xfer_func_complex)
	{

		for (int value = 0; value < output_ref_system_xfer_func_complex[0].size(); value++) {

			output_ref_system_xfer_func_complex[0][value] = input_ref_system_cross_spectrum_complex[0][value] / input_ref_autospectrum[value];
			output_ref_system_xfer_func_complex[1][value] = input_ref_system_cross_spectrum_complex[1][value] / input_ref_autospectrum[value];

		}

	}

};