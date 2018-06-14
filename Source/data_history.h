/*
  ==============================================================================

    data_history.h
    Created: 13 Jun 2018 1:45:10pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include <vector>
#include <deque>

class data_history
{

	std::vector<double> latest_data_values;

	std::vector<double> average;

	double value;

	std::deque<std::vector<double>> data_value_history;

	int number_of_data_points;
		
public:

	data_history(int dp) : number_of_data_points(dp)
	{

		latest_data_values.resize(number_of_data_points);

		average.resize(number_of_data_points);

	}

	~data_history()
	{

	}

	void set_num_histories(int num_histories) {

		data_value_history.resize(num_histories);

		for (int passes = 0; passes < data_value_history.size(); passes++) {

			data_value_history.push_front(latest_data_values);

			data_value_history.pop_back();

		}

	}

	void add_latest_values(std::vector<double> &latest_value_source) {

		std::copy(latest_value_source.begin(), latest_value_source.end(), latest_data_values.begin());

		data_value_history.push_front(latest_data_values);

		data_value_history.pop_back();

	}

	void get_data_average(std::vector<double> &destination_vector) {

		for (int index = 0; index < number_of_data_points; index++) {
			
			value = 0;

			for (int history = 0; history < data_value_history.size(); history++) {

				value += data_value_history[history][index];

			}

			average[index] = value / data_value_history.size()*1.0;
			
		}

		std::copy(average.begin(), average.end(), destination_vector.begin());

	}

private:
	
};
