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

	void set_trace_geometry(float plotted_x_min, float plotted_x_max, float plotted_y_min, float plotted_y_max) {

		trace_plotted_x_min_value = plotted_x_min;
		trace_plotted_x_max_value = plotted_x_max;
		trace_plotted_y_min_value = plotted_y_min;
		trace_plotted_y_max_value = plotted_y_max;

	}

	void set_trace_gridlines(std::vector<float> & x_gridlines, std::vector<float> & y_gridlines) {

		trace_x_gridline_coord = x_gridlines;
		trace_y_gridline_coord = y_gridlines;

	}

	void clear_data() {

		data_x_coord.clear(), data_y_coord.clear();

	}

	void append_single_data_point(float x_value, float y_value) //appends a single data point to end of data set
	{

		data_x_coord.push_back(x_value), data_y_coord.push_back(y_value);

	}

	void set_data(std::vector<float> & x_values, std::vector<float> & y_values) //clears object data set and refills it with data from vectors
																			//that are passed to this function. Both x and y vectors should be
																			//of the same size. If not, data will only be added until the end
																			//of the shorter vector is reached
	{

		clear_data();

		int data_size = std::min(x_values.size(), y_values.size());

		data_x_coord.resize(data_size), data_y_coord.resize(data_size);

		std::copy(x_values.begin(), x_values.begin() + data_size, data_x_coord.begin());
		std::copy(y_values.begin(), y_values.begin() + data_size, data_y_coord.begin());

	}

	int number_data_points() {

		return std::max(data_x_coord.size(), data_y_coord.size());

	}

	float return_x_data_point(int index) {

		return data_x_coord[index];

	}

	std::vector<float> return_x_data() {

		return data_x_coord;

	}

	float return_y_data_point(int index) {

		return data_y_coord[index];

	}

	std::vector<float> return_y_data() {

		return data_y_coord;

	}

	std::vector<float> return_data_extremes() {
				
		auto x_minmax = std::minmax_element(data_x_coord.begin(), data_x_coord.end());
		auto y_minmax = std::minmax_element(data_y_coord.begin(), data_y_coord.end());

		std::vector<float> data_extremes{ *x_minmax.first, *x_minmax.second, *y_minmax.first, *y_minmax.second };
		return data_extremes;

	}

protected:

	friend class flexplot;

	bool master_trace{ true };
	
	bool trace_visible{ true };
	int trace_line_thickess_pix{ 1 };
	Colour trace_line_color{ Colours::white };

	float trace_plotted_x_min_value{ 0 }, trace_plotted_x_max_value{ 1 }, trace_plotted_y_min_value{ 0 }, trace_plotted_y_max_value{ 0 };

	std::vector<float> trace_x_gridline_coord, trace_y_gridline_coord;

	std::vector<float> data_x_coord, data_y_coord; //the x and y data
	
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

		//g.setColour(Colours::white);
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

		for (int data_set = 0; data_set < data_sets.size(); data_set++) {

			//Get the extremes of plotting from current data set.

			original_plotted_x_min = data_sets[data_set]->trace_plotted_x_min_value;
			original_plotted_x_max = data_sets[data_set]->trace_plotted_x_max_value;
			original_plotted_y_min = data_sets[data_set]->trace_plotted_y_min_value;
			original_plotted_y_max = data_sets[data_set]->trace_plotted_y_max_value;

			calculate_actual_min_max(); //Transform the original plotting extremes to new ones based on zoom and pan controls
			bool render_gridlines_labels{ false };

			if (data_sets[data_set]->master_trace == true) { //The master trace defines the gridlines and axis labeling

				plot_x_grid_coord = data_sets[data_set]->trace_x_gridline_coord;
				plot_y_grid_coord = data_sets[data_set]->trace_y_gridline_coord;
				calculate_floating_label_strings();
				set_fixed_label_text();
				render_gridlines_labels = true; //Only render the gridlines and labels once, when the master trace is reached

			}
			
			if (render_gridlines_labels == true) {

				for (int x_index = 0; x_index < plot_x_grid_coord.size(); x_index++) {
										
					if (plot_x_grid_coord[x_index] >= actual_plotted_x_min && plot_x_grid_coord[x_index] <= actual_plotted_x_max) {

						g.setColour(Colour(50, 50, 50));

						g.fillRect(
							data_cord_to_plot_screen_cord(plot_x_grid_coord[x_index], 0).first,
							plot_screen_y_min,
							gridline_thickness_pix,
							plot_screen_y_max - plot_screen_y_min);

						g.setColour(Colours::white);
						int x_label_width_pix{ 100 };

						Rectangle<int> x_floating_label_outline{
							data_cord_to_plot_screen_cord(plot_x_grid_coord[x_index], 0.0).first - (x_label_width_pix / 2),
							bottom_label_area.getY(),
							x_label_width_pix,
							bottom_label_area.getHeight()
						};

						if (x_floating_label_outline.intersects(x_min_fixed_label_outline) == false &&
							x_floating_label_outline.intersects(x_max_fixed_label_outline) == false) {

							g.drawText(
								x_floating_label_strings[x_index],
								x_floating_label_outline,
								Justification::centred,
								false);

						}

					}

				}

				for (int y_index = 0; y_index < plot_y_grid_coord.size(); y_index++) {

					if (plot_y_grid_coord[y_index] >= actual_plotted_y_min && plot_y_grid_coord[y_index] <= actual_plotted_y_max) {

						g.setColour(Colour(50, 50, 50));

						g.fillRect(
							plot_screen_x_min,
							data_cord_to_plot_screen_cord(0, plot_y_grid_coord[y_index]).second,
							plot_screen_x_max - plot_screen_x_min,
							gridline_thickness_pix);

						g.setColour(Colours::white);
						int y_label_height_pix{ 50 };

						Rectangle<int> y_floating_label_outline{
							left_label_area.getX(),
							data_cord_to_plot_screen_cord(0.0, plot_y_grid_coord[y_index]).second - (y_label_height_pix / 2),
							left_label_area.getWidth(),
							y_label_height_pix
						};

						if (y_floating_label_outline.intersects(y_min_fixed_label_outline) == false &&
							y_floating_label_outline.intersects(y_max_fixed_label_outline) == false) {

							g.drawText(
								y_floating_label_strings[y_index],
								y_floating_label_outline,
								Justification::centred,
								false);

						}

					}

				}

				g.drawFittedText(x_min_fixed_label_text, x_min_fixed_label_outline, Justification::left, 1, 0.0);
				g.drawFittedText(x_max_fixed_label_text, x_max_fixed_label_outline, Justification::right, 1, 0.0);
				g.drawFittedText(y_min_fixed_label_text, y_min_fixed_label_outline, Justification::centredBottom, 1, 0.0);
				g.drawFittedText(y_max_fixed_label_text, y_max_fixed_label_outline, Justification::centredTop, 1, 0.0);
				
			}

			if (data_sets[data_set]->trace_visible == true) {

				Path current_trace;
				bool path_beginning_set{ false };

				for (int coordinate = 0; coordinate < data_sets[data_set]->number_data_points() - 1; coordinate++) {

					float current_x_value = data_sets[data_set]->return_x_data_point(coordinate);

					if (current_x_value < actual_plotted_x_min || current_x_value > actual_plotted_x_max) { continue; }

					float current_y_value = data_sets[data_set]->return_y_data_point(coordinate);

					std::pair<int, int> current_point_screen_coord;

					current_point_screen_coord = data_cord_to_plot_screen_cord(current_x_value, current_y_value);

					if (path_beginning_set == false) {

						current_trace.startNewSubPath(current_point_screen_coord.first, current_point_screen_coord.second);
						path_beginning_set = true;

					}

					if (path_beginning_set == true) {

						current_trace.lineTo(current_point_screen_coord.first, current_point_screen_coord.second);

					}

				}

				g.setColour(data_sets[data_set]->trace_line_color);
				current_trace = current_trace.createPathWithRoundedCorners(5.0);
				g.strokePath(current_trace, PathStrokeType(data_sets[data_set]->trace_line_thickess_pix));

			}

		}
				
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

    }

	void set_plot_max_zooms(int horizontal_max, int vertical_max) {

		max_hor_zoom = horizontal_max;
		max_vert_zoom = vertical_max;

		left_slider.setRange(1.0, max_vert_zoom, 0.0);
		bottom_slider.setRange(1.0, max_hor_zoom, 0.0);

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

	float original_plotted_x_min, original_plotted_x_max, original_plotted_y_min, original_plotted_y_max; //as configured by set_plot_properties() - data coordinates, not screen coordinates

	int plot_screen_x_min, plot_screen_x_max, plot_screen_y_min, plot_screen_y_max; //screen cordinates

	float actual_plotted_x_min, actual_plotted_x_max, actual_plotted_y_min, actual_plotted_y_max; //boundary of zoomed in area of plot, data coordinates
	
	int max_hor_zoom{ 2 }, max_vert_zoom{ 2 };

	float x_grid_spacing, y_grid_spacing;

	std::vector<float> plot_x_grid_coord, plot_y_grid_coord; //position of x and y gridlines, in data coordinates

	int gridline_thickness_pix{ 2 };

	int trace_thickness_pix{ 2 };

	float x_zoom{ 1.0 }, x_offset{ 0.0 }, y_zoom{ 1.0 }, y_offset{ 0.0 };

	int plot_x_mode{ 0 }; //0 is linear, 1 is base-10 logarithmic

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

		repaint();

	}

	void calculate_actual_min_max() {

		float original_plotted_x_span = original_plotted_x_max - original_plotted_x_min;

		float zoomed_plotted_x_span = original_plotted_x_span / x_zoom;

		float zoomed_plotted_x_span_total_buffer = original_plotted_x_span - zoomed_plotted_x_span;

		float right_buffer_proportion = 0.5 - (x_offset*0.5);

		float left_buffer_proportion = 1 - right_buffer_proportion;

		float right_buffer = zoomed_plotted_x_span_total_buffer * right_buffer_proportion;

		float left_buffer = zoomed_plotted_x_span_total_buffer - right_buffer;

		actual_plotted_x_min = original_plotted_x_min + left_buffer;
		actual_plotted_x_max = actual_plotted_x_min + zoomed_plotted_x_span;

		//

		float original_plotted_y_span = original_plotted_y_max - original_plotted_y_min;

		float zoomed_plotted_y_span = original_plotted_y_span / y_zoom;

		float zoomed_plotted_y_span_total_buffer = original_plotted_y_span - zoomed_plotted_y_span;

		float top_buffer_proportion = 0.5 - (y_offset*0.5);

		float bottom_buffer_proportion = 1 - top_buffer_proportion;

		float top_buffer = zoomed_plotted_y_span_total_buffer * top_buffer_proportion;

		float bottom_buffer = zoomed_plotted_y_span_total_buffer - top_buffer;

		actual_plotted_y_min = original_plotted_y_min + bottom_buffer;
		actual_plotted_y_max = actual_plotted_y_min + zoomed_plotted_y_span;

	}

	void set_fixed_label_text() {

		x_min_fixed_label_text = String(int(actual_plotted_x_min)) + " " + unit_x;
		x_max_fixed_label_text = String(int(actual_plotted_x_max)) + " " + unit_x;
		y_min_fixed_label_text = String(actual_plotted_y_min) + " " + unit_y;
		y_max_fixed_label_text = String(actual_plotted_y_max) + " " + unit_y;
		
	}

	//void calculate_gridlines() {

	//	plot_x_grid_coord.clear();

	//	plot_x_grid_coord.push_back(original_plotted_x_min);

	//	int original_plotted_x_span = original_plotted_x_max - original_plotted_x_min;

	//	int remaining_x_gridlines = floor(original_plotted_x_span / x_grid_spacing);

	//	for (int x = 1; x <= remaining_x_gridlines; x++) {

	//		int possible_gridline = original_plotted_x_min + (x_grid_spacing*x);

	//		if (possible_gridline >= actual_plotted_x_min && possible_gridline <= actual_plotted_x_max) {

	//			plot_x_grid_coord.push_back(possible_gridline);

	//		}

	//		else {};

	//	}

	//	//

	//	plot_y_grid_coord.clear();

	//	plot_y_grid_coord.push_back(original_plotted_y_min);

	//	float original_plotted_y_span = original_plotted_y_max - original_plotted_y_min;

	//	int remaining_y_gridlines = floor(original_plotted_y_span / y_grid_spacing);

	//	for (int y = 1; y <= remaining_y_gridlines; y++) {

	//		float possible_gridline = original_plotted_y_min + (y_grid_spacing*y);

	//		if (possible_gridline >= actual_plotted_y_min && possible_gridline <= actual_plotted_y_max) {

	//			plot_y_grid_coord.push_back(possible_gridline);

	//		}

	//		else {};

	//	}

	//}

	void calculate_floating_label_strings() {

		x_floating_label_strings.clear();

		for (int label = 0; label < plot_x_grid_coord.size(); label++) {

			x_floating_label_strings.push_back(String(plot_x_grid_coord[label]) + " "+ unit_x);

		}

		//

		y_floating_label_strings.clear();

		for (int label = 0; label < plot_y_grid_coord.size(); label++) {

			y_floating_label_strings.push_back(String(plot_y_grid_coord[label]) + unit_y);

		}

	}

	std::pair<int,int> data_cord_to_plot_screen_cord(float x_data_cord, float y_data_cord) {

		float x_position = (x_data_cord - actual_plotted_x_min) / (actual_plotted_x_max - actual_plotted_x_min);
		float y_position = (y_data_cord - actual_plotted_y_min) / (actual_plotted_y_max - actual_plotted_y_min);

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
