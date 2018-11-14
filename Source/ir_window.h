/*
  ==============================================================================

    ir_window.h
    Created: 5 Nov 2018 4:14:37pm
    Author:  John

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "flexplot.h"
#include "constants.h"

//==============================================================================
/*
*/
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

		ir_data_avg.resize(constants::largest_fft_size);

		ir_plot.set_plot_properties(-100.0, 1000.0, -1.0, 1.0, 50, 0.2, "", "");
		ir_plot.set_plot_max_zooms(10, 10);
		ir_plot.add_data_set(&ir_data);

		addAndMakeVisible(ir_plot);

    }

    ~ir_window()
    {
    }

    void paint (Graphics& g) override
    {

		ir_plot.repaint();

    }

    void resized() override
    {

		ir_plot.setBounds(getLocalBounds());
        
    }

	void update_ir_plot_data(std::vector<float> &new_ir_data) {

		ir_data_history.add_latest_values(new_ir_data);

		ir_data_history.get_data_average(ir_data_avg);
		
		ir_data.clear_data();

		for (int data_point = 0; data_point < ir_data_avg.size(); data_point++) {

			float x_value = ( (data_point*1.0) / constants::sample_rate) * 1000.0;
			float y_value = ir_data_avg[data_point];

			ir_data.add_single_data_point(x_value, y_value);

		}

	}

private:

	std::vector<float> ir_data_avg;

	flexplot ir_plot;

	data_history ir_data_history{ constants::largest_fft_size };

	flexplot_data_object ir_data;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ir_window)
};
