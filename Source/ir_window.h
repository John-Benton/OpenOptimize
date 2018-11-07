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

		ir_plot.set_plot_properties(0.0, 683.0, -1.0, 1.0, 50.0, 0.2, "", "");
		ir_plot.set_plot_max_zooms(50, 10);

		addAndMakeVisible(ir_plot);

    }

    ~ir_window()
    {
    }

    void paint (Graphics& g) override
    {
        
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (Colours::white);
        g.setFont (14.0f);
        g.drawText ("ir_window", getLocalBounds(),
                    Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {

		ir_plot.setBounds(getLocalBounds());
        
    }

private:

	flexplot ir_plot;

	flexplot_data_object ir_data;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ir_window)
};
