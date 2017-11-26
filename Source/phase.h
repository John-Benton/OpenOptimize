/*
  ==============================================================================

    phase.h
    Created: 10 Nov 2017 10:46:57am
    Author:  John

	NOTE: The phase unwrapping and re-wrapping algorithms used in the class are
	based upon the algorithms discussed in the paper "One-Dimensional Phase Unwrapping Problem"
	by Dr. Munther Gdeisat and Dr. Francis Lilley

  ==============================================================================
*/

#pragma once
#include <vector>

class PhaseWrapUnwrap 
{

public:

	std::vector<double> intermediate_phase;
	int num_samples;
	double wrap_theshold = 90.0; //degrees

	PhaseWrapUnwrap()
	{
	}

	~PhaseWrapUnwrap()
	{
	}

	void configure(int samples) {

		intermediate_phase.resize(samples);

	}

	void unwrap(std::vector<double> & source_phase) {

		unwrap_source_phase(source_phase);
		copy_output_to_source(source_phase);

	}

	void wrap(std::vector<double> & source_phase) {

		wrap_source_phase(source_phase);
		copy_output_to_source(source_phase);

	}

private:

	void unwrap_source_phase(std::vector<double> & source_phase) {

		std::copy(source_phase.begin(), source_phase.end(), intermediate_phase.begin());

		double difference;

		for (int sample = 1; sample < intermediate_phase.size(); sample++) {

			difference = intermediate_phase[sample] - intermediate_phase[sample - 1];

			if (difference > wrap_theshold) {

				for (int index = sample; index < intermediate_phase.size(); index++) {

					intermediate_phase[index] -= 360.0;

				}

			}

			if (difference < -wrap_theshold) {

				for (int index = sample; index < intermediate_phase.size(); index++) {

					intermediate_phase[index] += 360.0;

				}

			}

			else {};

		}

	}

	void wrap_source_phase(std::vector<double> & source_phase) {

		std::copy(source_phase.begin(), source_phase.end(), intermediate_phase.begin());

		for (int sample = 0; sample < intermediate_phase.size(); sample++) {

			if (intermediate_phase[sample] > 180.0) {

				for (int index = sample; index < intermediate_phase.size(); index++) {

					intermediate_phase[index] -= 360;

				}

			}

			if (intermediate_phase[sample] < -180.0) {

				for (int index = sample; index < intermediate_phase.size(); index++) {

					intermediate_phase[index] += 360;

				}

			}

		}

	}

	void copy_output_to_source(std::vector<double> & source_phase) {

		std::copy(intermediate_phase.begin(), intermediate_phase.end(), source_phase.begin());

	}

};
