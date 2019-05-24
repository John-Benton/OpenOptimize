#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "flexplot.h"
#include "constants.h"

class ir_plot : public flexplot
{
public:

	ir_plot() {};
	~ir_plot() {};

	void draw_custom_overlay(Graphics& g) override 
	{
		g.saveState();

		Rectangle<int> ir_max_indicator_outline(plot_area.getX() + 5,
			plot_area.getY() + 5,
			plot_area.getWidth()*0.20,
			plot_area.getHeight()*0.05);

		g.setColour(Colours::black);
		g.fillRect(ir_max_indicator_outline);

		g.setColour(Colours::white);
		g.setFont(ir_max_indicator_outline.getHeight()*0.8);
		g.drawFittedText("IR Peak: " + String(ir_max_time_msec), ir_max_indicator_outline, Justification::centred, 1);

		g.restoreState();
	}

	float ir_max_time_msec{ 0.0 };

};

class ir_window    : public Component
{
public:

	ir_plot main_ir_plot;

    ir_window()
    {
		
		Desktop::getInstance();
		Rectangle<int> display_outline = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
		int component_width = display_outline.getWidth()*0.5;
		int component_height = display_outline.getHeight()*0.5;
		this->setSize(component_width, component_height);

		ir_data_history.set_num_histories(10);

		original_ir_data.resize(num_ir_samples);
		original_ir_data_avg.resize(num_ir_samples);
				
		main_ir_plot.set_plot_max_zooms(10, 10);
		main_ir_plot.add_data_set(&ir_data);

		ir_data.set_trace_appearance(true, 2, Colours::white);
		ir_data.set_trace_geometry(0, 500, -1.2, 1.2);
		std::vector<float> ir_x_gridlines{ 0.0f,100.0f,200.0f,300.0f,400.0f,500.0f };
		std::vector<float> ir_y_gridlines{-1.2f,-1.0f,-0.8f,-0.6f,-0.4f,-0.2f,0.0f,0.2f,0.4f,0.6f,0.8f,1.0f,1.2f};
		ir_data.set_trace_gridlines(ir_x_gridlines, ir_y_gridlines);

		addAndMakeVisible(main_ir_plot);
		main_ir_plot.setOpaque(true);

    }

    ~ir_window()
    {
    }

    void paint (Graphics& g) override
    {

		main_ir_plot.repaint();

    }

    void resized() override
    {

		main_ir_plot.setBounds(getLocalBounds());
		
    }

	void update_ir_plot_data(std::vector<float> &new_ir_data) {

		std::copy(new_ir_data.begin(), new_ir_data.begin() + num_ir_samples, original_ir_data.begin());
		
		ir_data_history.add_latest_values(original_ir_data);
		ir_data_history.get_data_average(original_ir_data_avg);
		ir_data.clear_data();

		auto original_ir_data_avg_max_it = std::max_element(original_ir_data_avg.begin(), original_ir_data_avg.end());
		original_ir_data_avg_max_index = std::distance(original_ir_data_avg.begin(), original_ir_data_avg_max_it);
		main_ir_plot.ir_max_time_msec = (original_ir_data_avg_max_index * 1.0 / constants::sample_rate * 1.0) * 1000.0;
		
		int skipped_data_points{ 0 };
		int peaks_detected{ 0 };
		bool excessive_peaks{ false };
		decimated_avg_ir_sample_amplitudes.clear();
		decimated_avg_ir_sample_times_msec.clear();
		
		for (int index = 0; index < original_ir_data_avg.size(); index++) {

			if (peaks_detected >= 100) {

				excessive_peaks = true;

			}

			if (original_ir_data_avg[index] > 0.5 && excessive_peaks == false) {

				decimated_avg_ir_sample_amplitudes.push_back(original_ir_data_avg[index]);
				decimated_avg_ir_sample_times_msec.push_back((1000.0 / constants::sample_rate*1.0) * index);
				peaks_detected++;
				skipped_data_points = 0;
				continue;

			}

			if (skipped_data_points >= 20) {

				decimated_avg_ir_sample_amplitudes.push_back(original_ir_data_avg[index]);
				decimated_avg_ir_sample_times_msec.push_back((1000.0 / constants::sample_rate*1.0) * index);
				skipped_data_points = 0;
				continue;

			}

			skipped_data_points++;

		}

		auto decimated_avg_ir_samples_max = std::max_element(decimated_avg_ir_sample_amplitudes.begin(), decimated_avg_ir_sample_amplitudes.end());
		FloatVectorOperations::multiply(&decimated_avg_ir_sample_amplitudes[0], 1.0 / *decimated_avg_ir_samples_max, decimated_avg_ir_sample_amplitudes.size());

		ir_data.set_data(decimated_avg_ir_sample_times_msec, decimated_avg_ir_sample_amplitudes);

		main_ir_plot.repaint();

	}

private:
	
	int num_ir_samples = constants::sample_rate * (500.0 / 1000.0); //for 500 msec of samples
	
	std::vector<float> original_ir_data;
	std::vector<float> original_ir_data_avg;

	std::vector<float> decimated_avg_ir_sample_amplitudes;
	std::vector<float> decimated_avg_ir_sample_times_msec;

	int original_ir_data_avg_max_index{ 0 };
	
	data_history ir_data_history{ num_ir_samples };

	flexplot_trace ir_data;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ir_window)

};
