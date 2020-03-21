/*
  ==============================================================================

    moving_average.h
    Created: 5 Nov 2017 8:33:15am
    Author:  John

  ==============================================================================
*/

#pragma once
#include <deque>
#include <vector>

class MovingAverage1DVector
{
public:

	double first_data_point_row0 = 0.0;
	double last_data_point_row0 = 0.0;

	double num_elements = 0.0; //the number of elements in each row of the original data set
	double average_size = 0.0; //the number of points on either side of the current point that are included in the average

	int iter_start_offset = 0;
	double averaging_sum_row0 = 0.0;

	std::deque<double> intermediate_row0;

	std::vector<double> output_row0;

	MovingAverage1DVector()
	{
	}

	~MovingAverage1DVector()
	{
	}

	void configure(int elements, int size) {

		num_elements = elements;
		average_size = size;

		intermediate_row0.resize(num_elements + 2 * average_size);

		output_row0.resize(num_elements);

	}

	void process(std::vector<double> & source) {

		find_first_last_source(source);
		copy_source_to_intermediate(source);
		pad_intermediate();
		calc_average();
		copy_output_to_source(source);

	}

private:

	JUCE_LEAK_DETECTOR(MovingAverage1DVector);

	void find_first_last_source(std::vector<double> & source) {

		first_data_point_row0 = source.front();
		last_data_point_row0 = source.back();

	}

	void copy_source_to_intermediate(std::vector<double> & source) {

		std::copy(source.begin(), source.end(), intermediate_row0.begin() + average_size);

	}

	void pad_intermediate() {

		for (int x = 0; x < average_size; x++) {

			intermediate_row0[x] = first_data_point_row0;
			intermediate_row0[x + average_size + num_elements] = last_data_point_row0;

		}

	}

	void calc_average() {

		for (int center = average_size; center < average_size + num_elements; center++) {

			averaging_sum_row0 = 0.0;
			iter_start_offset = center - average_size;

			for (int iter = 0; iter <= 2 * average_size; iter++) {

				averaging_sum_row0 += intermediate_row0[iter + iter_start_offset];

			}

			output_row0[center - average_size] = averaging_sum_row0 / (2 * average_size + 1);

		}

	}

	void copy_output_to_source(std::vector<double> & source) {

		std::copy(output_row0.begin(), output_row0.end(), source.begin());

	}

};

class MovingAverage2DVector
{
public:

	double first_data_point_row0 = 0.0;
	double first_data_point_row1 = 0.0;
	double last_data_point_row0 = 0.0;
	double last_data_point_row1 = 0.0;

	double num_elements = 0.0; //the number of elements in each row of the original data set
	double average_size = 0.0; //the number of points on either side of the current point that are included in the average

	int iter_start_offset = 0;
	double averaging_sum_row0 = 0.0;
	double averaging_sum_row1 = 0.0;
	
	std::deque<double> intermediate_row0;
	std::deque<double> intermediate_row1;

	std::vector<double> output_row0;
	std::vector<double> output_row1;
	
	MovingAverage2DVector()
	{
	}
	
	~MovingAverage2DVector()
	{
	}

	void configure(int elements, int size) {

		num_elements = elements;
		average_size = size;

		intermediate_row0.resize(num_elements + 2 * average_size);
		intermediate_row1.resize(num_elements + 2 * average_size);

		output_row0.resize(num_elements);
		output_row1.resize(num_elements);

	}

	void process(std::vector<std::vector<double>> & source) {

		find_first_last_source(source);
		copy_source_to_intermediate(source);
		pad_intermediate();
		calc_average();
		copy_output_to_source(source);

	}

private:

	JUCE_LEAK_DETECTOR(MovingAverage2DVector);
	
	void find_first_last_source(std::vector<std::vector<double>> & source) {

		first_data_point_row0 = source[0].front();
		first_data_point_row1 = source[1].front();

		last_data_point_row0 = source[0].back();
		last_data_point_row1 = source[0].back();

	}

	void copy_source_to_intermediate(std::vector<std::vector<double>> & source) {

		std::copy(source[0].begin(), source[0].end(), intermediate_row0.begin() + average_size);
		std::copy(source[1].begin(), source[1].end(), intermediate_row1.begin() + average_size);

	}

	void pad_intermediate() {

		for (int x = 0; x < average_size; x++) {

			intermediate_row0[x] = first_data_point_row0;
			intermediate_row0[x + average_size + num_elements] = last_data_point_row0;

			intermediate_row1[x] = first_data_point_row1;
			intermediate_row1[x + average_size + num_elements] = last_data_point_row1;

		}

	}

	void calc_average() {
		
		for (int center = average_size; center < average_size + num_elements; center++) {

			averaging_sum_row0 = 0.0;
			averaging_sum_row1 = 0.0;
			iter_start_offset = center - average_size;

			for (int iter = 0; iter <= 2 * average_size; iter++) {

				averaging_sum_row0 += intermediate_row0[iter + iter_start_offset];
				averaging_sum_row1 += intermediate_row1[iter + iter_start_offset];

			}

			output_row0[center-average_size] = averaging_sum_row0 / (2 * average_size + 1);
			output_row1[center-average_size] = averaging_sum_row1 / (2 * average_size + 1);
			
		}

	}

	void copy_output_to_source(std::vector<std::vector<double>> & source) {

		std::copy(output_row0.begin(), output_row0.end(), source[0].begin());
		std::copy(output_row1.begin(), output_row1.end(), source[1].begin());

	}
	
};