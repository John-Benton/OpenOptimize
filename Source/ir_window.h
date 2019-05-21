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

		Rectangle<int> ir_max_indicator_outline(plot_area.getX(),
			plot_area.getY(),
			plot_area.getWidth()*0.20,
			plot_area.getHeight()*0.05);

		g.setColour(Colours::black);
		g.fillRect(ir_max_indicator_outline);

		g.setColour(Colours::white);
		g.setFont(ir_max_indicator_outline.getHeight()*0.8);
		g.drawFittedText("IR Peak: " + String(ir_max_index), ir_max_indicator_outline, Justification::centred, 1);

		g.restoreState();
	}

	int ir_max_index{ 0 };

};

class ir_window    : public Component
{
public:
    ir_window()
    {
		
		Desktop::getInstance();
		Rectangle<int> display_outline = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
		int component_width = display_outline.getWidth()*0.5;
		int component_height = display_outline.getHeight()*0.5;
		this->setSize(component_width, component_height);

		ir_data_history.set_num_histories(10);

		decimated_ir_data.resize(num_decimated_ir_data_points);

		decimated_ir_data_avg.resize(num_decimated_ir_data_points);

		main_ir_plot.set_plot_properties(0.0, 500.0, -1.2, 1.2, 50, 0.2, "", "");
		main_ir_plot.set_plot_max_zooms(10, 10);
		main_ir_plot.add_data_set(&ir_data);

		addAndMakeVisible(main_ir_plot);

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

		int raw_data_point{ 0 };

		for (int decimated_data_point = 0; decimated_data_point < num_decimated_ir_data_points; decimated_data_point++) {

			decimated_ir_data[decimated_data_point] = new_ir_data[raw_data_point];

			raw_data_point += constants::sample_rate / 1000; //this skips over samples between each whole milisecond

		}
		
		ir_data_history.add_latest_values(decimated_ir_data);
		ir_data_history.get_data_average(decimated_ir_data_avg);
		ir_data.clear_data();

		auto decimated_ir_data_avg_max_it = std::max_element(decimated_ir_data_avg.begin(), decimated_ir_data_avg.end());
		main_ir_plot.ir_max_index = std::distance(decimated_ir_data_avg.begin(), decimated_ir_data_avg_max_it);
		FloatVectorOperations::multiply(&decimated_ir_data_avg[0], 1.0 / *decimated_ir_data_avg_max_it, decimated_ir_data_avg.size());

		for (int data_point = 0; data_point < decimated_ir_data_avg.size(); data_point++) {

			float x_value = data_point;
			float y_value = decimated_ir_data_avg[data_point];

			ir_data.append_single_data_point(x_value, y_value);

		}

	}
	
private:

	int num_decimated_ir_data_points = 500; //data points will be at 1 msec intervals.

	std::vector<float> decimated_ir_data;

	std::vector<float> decimated_ir_data_avg;

	ir_plot main_ir_plot;

	data_history ir_data_history{ num_decimated_ir_data_points };

	flexplot_data_object ir_data;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ir_window)
};
