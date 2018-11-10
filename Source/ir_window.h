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

		ir_plot.set_plot_properties(-1.0, 10.0, -1.0, 1.0, 1.0, 0.2, "", "");
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

		ir_data.clear_data();

		for (int data_point = 0; data_point < new_ir_data.size(); data_point++) {

			float x_value = ( (data_point*1.0) / constants::sample_rate) * 1000.0;
			float y_value = new_ir_data[data_point];

			ir_data.add_single_data_point(x_value, y_value);

		}

	}

private:

	flexplot ir_plot;

	flexplot_data_object ir_data;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ir_window)
};
