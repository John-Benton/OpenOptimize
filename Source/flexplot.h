#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class flexplot_trace //holds the data and properties for a single trace that can be rendered in a flexplot
{
public:

	flexplot_trace() {};
	~flexplot_trace() {};

	void set_trace_appearance(bool is_visible, int trace_thickness_pix, Colour trace_color)
	{

		trace_visible = is_visible;
		trace_line_thickess_pix = trace_thickness_pix;
		trace_line_color = trace_color;

	}

	void clear_data() {

		data_x_cord.clear(), data_y_cord.clear();

	}

	void append_single_data_point(float x_value, float y_value) //appends a single data point to end of data set
	{

		data_x_cord.push_back(x_value), data_y_cord.push_back(y_value);

	}

	void set_data(std::vector<float> & x_values, std::vector<float> & y_values) //clears object data set and refills it with data from vectors
																			//that are passed to this function. Both x and y vectors should be
																			//of the same size. If not, data will only be added until the end
																			//of the shorter vector is reached
	{

		clear_data();

		int data_size = std::min(x_values.size(), y_values.size());

		data_x_cord.resize(data_size), data_y_cord.resize(data_size);

		std::copy(x_values.begin(), x_values.begin() + data_size, data_x_cord.begin());
		std::copy(y_values.begin(), y_values.begin() + data_size, data_y_cord.begin());

	}

	int number_data_points() {

		return std::max(data_x_cord.size(), data_y_cord.size());

	}

	float return_x_data_point(int index) {

		return data_x_cord[index];

	}

	float return_y_data_point(int index) {

		return data_y_cord[index];

	}

protected:

	friend class flexplot;
	
	bool trace_visible{ true };
	int trace_line_thickess_pix{ 1 };
	Colour trace_line_color{ Colours::white };

	std::vector<float> data_x_cord, data_y_cord; //the x and y data
	
};

class flexplot    : public Component, public Slider::Listener
{
public:
    flexplot()
    {
		addAndMakeVisible(top_slider),
			top_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			top_slider.setRange(-1.0, 1.0, 0.0),
			top_slider.setValue(0.0, dontSendNotification),
			top_slider.setDoubleClickReturnValue(true, 0.0);
			top_slider.addListener(this);
		addAndMakeVisible(left_slider),
			left_slider.setSliderStyle(Slider::SliderStyle::LinearVertical),
			left_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			left_slider.setRange(1.0, max_vert_zoom, 0.0),
			left_slider.setValue(1.0, dontSendNotification),
			left_slider.setDoubleClickReturnValue(true, 1.0);
			left_slider.addListener(this);
		addAndMakeVisible(right_slider),
			right_slider.setSliderStyle(Slider::SliderStyle::LinearVertical),
			right_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			right_slider.setRange(-1.0, 1.0, 0.0),
			right_slider.setValue(0.0, dontSendNotification),
			right_slider.setDoubleClickReturnValue(true, 0.0);
			right_slider.addListener(this);
		addAndMakeVisible(bottom_slider),
			bottom_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			bottom_slider.setRange(1.0, max_hor_zoom, 0.0),
			bottom_slider.setValue(1.0, dontSendNotification),
			bottom_slider.setDoubleClickReturnValue(true, 1.0);
			bottom_slider.addListener(this);
		
    }

    ~flexplot()
    {
    }
	
	virtual void draw_custom_overlay(Graphics& g) {



	}

    void paint (Graphics& g) override
    {
		g.fillAll(Colour(20, 20, 20));

		g.setColour(Colours::black);

		g.fillRect(plot_area);

		g.setColour(Colours::white);

		//g.drawRect(top_region, 1.0);
		//g.drawRect(left_region, 1.0);
		//g.drawRect(left_slider_area, 1.0);
		//g.drawRect(left_label_area, 1.0);
		//g.drawRect(y_max_fixed_label_outline, 1.0);
		//g.drawRect(left_label_moving_center, 1.0);
		//g.drawRect(y_min_fixed_label_outline, 1.0);
		//g.drawRect(plot_area, 1.0);
		//g.drawRect(right_slider_area, 1.0);
		//g.drawRect(bottom_region, 1.0);
		//g.drawRect(bottom_label_area, 1.0);
		//g.drawRect(x_min_fixed_label_outline, 1.0);
		//g.drawRect(bottom_label_moving_center, 1.0);
		//g.drawRect(x_max_fixed_label_outline, 1.0);
		//g.drawRect(bottom_slider_area, 1.0);
		//g.drawRect(upper_left_corner, 1.0);
		//g.drawRect(upper_right_corner, 1.0);
		//g.drawRect(bottom_left_corner, 1.0);
		//g.drawRect(bottom_right_corner, 1.0);

		g.drawFittedText(x_min_fixed_label_text, x_min_fixed_label_outline, Justification::left, 1, 0.0);
		g.drawFittedText(x_max_fixed_label_text, x_max_fixed_label_outline, Justification::right, 1, 0.0);
		g.drawFittedText(y_min_fixed_label_text, y_min_fixed_label_outline, Justification::centredBottom, 1, 0.0);
		g.drawFittedText(y_max_fixed_label_text, y_max_fixed_label_outline, Justification::centredTop, 1, 0.0);

		g.saveState();

		g.reduceClipRegion(bottom_label_area);

			for (int label = 0; label < x_floating_label_strings.size(); label++) {

				int x_label_width_pix{ 100 };

				g.drawText(
					x_floating_label_strings[label],
					data_cord_to_plot_screen_cord(x_grid_cord[label], 0.0).first - (x_label_width_pix/2),
					bottom_label_area.getY(),
					x_label_width_pix,
					bottom_label_area.getHeight(),
					Justification::centred,
					false);

			}

		g.restoreState();

		g.saveState();

		g.reduceClipRegion(left_label_area);

			for (int label = 0; label < y_floating_label_strings.size(); label++) {

				int y_label_height_pix{ 50 };

				g.drawText(
					y_floating_label_strings[label],
					left_label_area.getX(),
					data_cord_to_plot_screen_cord(0.0, y_grid_cord[label]).second - (y_label_height_pix / 2),
					left_label_area.getWidth(),
					y_label_height_pix,
					Justification::centred,
					false);

			}

		g.restoreState();

		g.saveState();

		g.reduceClipRegion(plot_area);

			g.setColour(Colour(50, 50, 50));

			for (int line = 0; line < x_grid_cord.size(); line++) {

				g.fillRect(
					data_cord_to_plot_screen_cord(x_grid_cord[line], 0).first,
					plot_screen_y_min,
					gridline_thickness_pix,
					plot_screen_y_max - plot_screen_y_min);
			}

			for (int line = 0; line < y_grid_cord.size(); line++) {

				g.fillRect(
					plot_screen_x_min,
					data_cord_to_plot_screen_cord(0, y_grid_cord[line]).second,
					plot_screen_x_max - plot_screen_x_min,
					gridline_thickness_pix);
			}

			for (int data_set = 0; data_set < data_sets.size(); data_set++) {

				if (data_sets[data_set]->trace_visible == true) {

					Path current_trace;
					std::pair<int, int> x_axis;
					x_axis = data_cord_to_plot_screen_cord(0.0, 0.0);

					for (int coordinate = 0; coordinate < data_sets[data_set]->number_data_points() - 1; coordinate++) {

						float current_x_value = data_sets[data_set]->return_x_data_point(coordinate);
						float current_y_value = data_sets[data_set]->return_y_data_point(coordinate);

						std::pair<int, int> current_point_screen_coord;

						current_point_screen_coord = data_cord_to_plot_screen_cord(current_x_value, current_y_value);

						if (coordinate == 0) {

							current_trace.startNewSubPath(current_point_screen_coord.first, current_point_screen_coord.second);

						}

						if (coordinate > 0) {

							current_trace.lineTo(current_point_screen_coord.first, current_point_screen_coord.second);

						}

					}

					g.setColour(data_sets[data_set]->trace_line_color);
					current_trace = current_trace.createPathWithRoundedCorners(5.0);
					g.strokePath(current_trace, PathStrokeType(data_sets[data_set]->trace_line_thickess_pix));

				}

			}

		g.restoreState();

		draw_custom_overlay(g);

    }

    void resized() override
    {
		component_outline = getBounds();

		int original_width = component_outline.getWidth(), original_height = component_outline.getHeight();

		top_region = component_outline.removeFromTop(original_height*0.05);
		upper_left_corner = top_region.removeFromLeft(original_width*0.15);
		upper_right_corner = top_region.removeFromRight(original_width*0.05);
		top_slider_area = top_region;
		bottom_region = component_outline.removeFromBottom(original_height*0.10);
		bottom_left_corner = bottom_region.removeFromLeft(original_width*0.15);
		bottom_right_corner = bottom_region.removeFromRight(original_width*0.05);
		bottom_label_area = bottom_region.removeFromTop(original_height*0.05);
		x_min_fixed_label_outline = bottom_label_area.removeFromLeft(original_width*0.05);
		x_max_fixed_label_outline = bottom_label_area.removeFromRight(original_width*0.05);
		bottom_label_moving_center = bottom_label_area;
		bottom_slider_area = bottom_region.removeFromTop(original_height*0.05);
		left_region = component_outline.removeFromLeft(original_width*0.15);
		left_slider_area = left_region.removeFromLeft(original_width*0.05);
		left_label_area = left_region.removeFromLeft(original_width*0.10);
		y_max_fixed_label_outline = left_label_area.removeFromTop(original_height*0.025);
		y_min_fixed_label_outline = left_label_area.removeFromBottom(original_height*0.025);
		left_label_moving_center = left_label_area;
		plot_area = component_outline.removeFromLeft(original_width*0.8);

		plot_screen_x_min = plot_area.getX();
		plot_screen_x_max = plot_screen_x_min + plot_area.getWidth();
		plot_screen_y_min = plot_area.getY();
		plot_screen_y_max = plot_screen_y_min + plot_area.getHeight();

		right_slider_area = component_outline;
		top_slider.setBounds(top_slider_area);
		left_slider.setBounds(left_slider_area);
		right_slider.setBounds(right_slider_area);
		bottom_slider.setBounds(bottom_slider_area);

		recalc_plot_layout();

    }

	void set_plot_properties(float plot_x_min, float plot_x_max, float plot_y_min, float plot_y_max, float x_gridline_spacing, float y_gridline_spacing, String x_unit, String y_unit) {

		set_x_min = plot_x_min;
		set_x_max = plot_x_max;
		set_y_min = plot_y_min;
		set_y_max = plot_y_max;

		x_grid_spacing = x_gridline_spacing;
		y_grid_spacing = y_gridline_spacing;

		unit_x = x_unit;

		unit_y = y_unit;
		
		recalc_plot_layout();

	}

	void set_plot_max_zooms(int horizontal_max, int vertical_max) {

		max_hor_zoom = horizontal_max;
		max_vert_zoom = vertical_max;

		left_slider.setRange(1.0, max_vert_zoom, 0.0);
		bottom_slider.setRange(1.0, max_hor_zoom, 0.0);

		recalc_plot_layout();

	}

	void add_data_set(flexplot_trace* new_data_set) { //adds a flexplot_trace to the plot

		data_sets.push_back(new_data_set);

	}

	void clear_data_sets() {

		data_sets.clear();

	}

protected:

	Rectangle<int>	component_outline,
					top_region,
					top_slider_area,
					left_region,
					left_slider_area,
					left_label_area,
					y_max_fixed_label_outline,
					left_label_moving_center,
					y_min_fixed_label_outline,
					plot_area,
					right_slider_area,
					bottom_region,
					bottom_label_area,
					x_min_fixed_label_outline,
					bottom_label_moving_center,
					x_max_fixed_label_outline,
					bottom_slider_area,
					upper_left_corner,
					upper_right_corner,
					bottom_left_corner,
					bottom_right_corner;

	Slider	top_slider,
			left_slider,
			right_slider,
			bottom_slider;

	String unit_x, unit_y;

	String 	x_min_fixed_label_text,
			x_max_fixed_label_text,
			y_min_fixed_label_text,
			y_max_fixed_label_text;

	std::vector<String> x_floating_label_strings;
	std::vector<String> y_floating_label_strings;

	std::vector<flexplot_trace*> data_sets; //holds pointers to all data sets that have been added to the plot;

	float set_x_min, set_x_max, set_y_min, set_y_max; //as configured by set_plot_properties() - data coordinates, not screen coordinates

	int plot_screen_x_min, plot_screen_x_max, plot_screen_y_min, plot_screen_y_max; //screen cordinates

	float actual_x_min, actual_x_max, actual_y_min, actual_y_max; //boundary of zoomed in area of plot, data coordinates
	
	int max_hor_zoom{ 2 }, max_vert_zoom{ 2 };

	float x_grid_spacing, y_grid_spacing;

	std::vector<float> x_grid_cord, y_grid_cord; //data coordinates

	int gridline_thickness_pix{ 2 };

	int trace_thickness_pix{ 2 };

	float x_zoom{ 1.0 }, x_offset{ 0.0 }, y_zoom{ 1.0 }, y_offset{ 0.0 };

	void sliderValueChanged(Slider* slider) override {

		if (slider == &top_slider) {

			x_offset = top_slider.getValue();

		}

		else if (slider == &left_slider) {

			y_zoom = left_slider.getValue();
			
		}

		else if (slider == &right_slider) {

			y_offset = right_slider.getValue();

		}

		else if (slider == &bottom_slider) {

			x_zoom = bottom_slider.getValue();

		}

		else {}

		recalc_plot_layout();

	}

	void recalc_plot_layout() {

		calculate_actual_min_max();

		calculate_gridlines();

		calculate_floating_label_strings();

		set_fixed_label_text();

		repaint();

	}

	void calculate_actual_min_max() {

		float set_x_span = set_x_max - set_x_min;

		float zoomed_x_span = set_x_span / x_zoom;

		float zoomed_x_span_total_buffer = set_x_span - zoomed_x_span;

		float right_buffer_proportion = 0.5 - (x_offset*0.5);

		float left_buffer_proportion = 1 - right_buffer_proportion;

		float right_buffer = zoomed_x_span_total_buffer * right_buffer_proportion;

		float left_buffer = zoomed_x_span_total_buffer - right_buffer;

		actual_x_min = set_x_min + left_buffer;
		actual_x_max = actual_x_min + zoomed_x_span;

		//

		float set_y_span = set_y_max - set_y_min;

		float zoomed_y_span = set_y_span / y_zoom;

		float zoomed_y_span_total_buffer = set_y_span - zoomed_y_span;

		float top_buffer_proportion = 0.5 - (y_offset*0.5);

		float bottom_buffer_proportion = 1 - top_buffer_proportion;

		float top_buffer = zoomed_y_span_total_buffer * top_buffer_proportion;

		float bottom_buffer = zoomed_y_span_total_buffer - top_buffer;

		actual_y_min = set_y_min + bottom_buffer;
		actual_y_max = actual_y_min + zoomed_y_span;

	}

	void set_fixed_label_text() {

		x_min_fixed_label_text = String(int(actual_x_min)) + " " + unit_x;
		x_max_fixed_label_text = String(int(actual_x_max)) + " " + unit_x;
		y_min_fixed_label_text = String(actual_y_min) + " " + unit_y;
		y_max_fixed_label_text = String(actual_y_max) + " " + unit_y;
		
	}

	void calculate_gridlines() {

		x_grid_cord.clear();

		x_grid_cord.push_back(set_x_min);

		int set_x_span = set_x_max - set_x_min;

		int remaining_x_gridlines = floor(set_x_span / x_grid_spacing);

		for (int x = 1; x <= remaining_x_gridlines; x++) {

			int possible_gridline = set_x_min + (x_grid_spacing*x);

			if (possible_gridline >= actual_x_min && possible_gridline <= actual_x_max) {

				x_grid_cord.push_back(possible_gridline);

			}

			else {};

		}

		//

		y_grid_cord.clear();

		y_grid_cord.push_back(set_y_min);

		float set_y_span = set_y_max - set_y_min;

		int remaining_y_gridlines = floor(set_y_span / y_grid_spacing);

		for (int y = 1; y <= remaining_y_gridlines; y++) {

			float possible_gridline = set_y_min + (y_grid_spacing*y);

			if (possible_gridline >= actual_y_min && possible_gridline <= actual_y_max) {

				y_grid_cord.push_back(possible_gridline);

			}

			else {};

		}

	}

	void calculate_floating_label_strings() {

		x_floating_label_strings.clear();

		for (int label = 0; label < x_grid_cord.size(); label++) {

			x_floating_label_strings.push_back(String(x_grid_cord[label]) + " "+ unit_x);

		}

		//

		y_floating_label_strings.clear();

		for (int label = 0; label < y_grid_cord.size(); label++) {

			y_floating_label_strings.push_back(String(y_grid_cord[label]) + unit_y);

		}

	}

	std::pair<int,int> data_cord_to_plot_screen_cord(float x_data_cord, float y_data_cord) {

		float x_position = (x_data_cord - actual_x_min) / (actual_x_max - actual_x_min);
		float y_position = (y_data_cord - actual_y_min) / (actual_y_max - actual_y_min);

		int plot_screen_x_span = plot_screen_x_max - plot_screen_x_min;

		int x_screen_cord = plot_screen_x_min + (x_position * plot_screen_x_span);

		int plot_screen_y_span = plot_screen_y_max - plot_screen_y_min;

		int y_screen_cord = plot_screen_y_max - (y_position * plot_screen_y_span);

		std::pair<int,int> screen_cord;

		screen_cord.first = x_screen_cord, screen_cord.second = y_screen_cord;

		return screen_cord;

	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (flexplot)
};
