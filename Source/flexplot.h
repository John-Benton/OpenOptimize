#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "utility.h"

class flexplot_trace //holds the data and properties for a single trace that can be rendered in a flexplot
{
public:

	flexplot_trace() {};
	~flexplot_trace() {};

	void set_trace_appearance(int trace_thickness_pix, Colour trace_color)
	{

		trace_line_thickess_pix = trace_thickness_pix;
		trace_line_color = trace_color;

	}

	void set_trace_visible(bool is_visible) {

		trace_visible = is_visible;

	}

	void set_trace_master(bool is_master) {

		master_trace = is_master;

	}

	void set_trace_plotting_boundaries(float plotted_x_min, float plotted_x_max, float plotted_y_min, float plotted_y_max) {

		trace_plotted_x_min_value = plotted_x_min;
		trace_plotted_x_max_value = plotted_x_max;
		trace_plotted_y_min_value = plotted_y_min;
		trace_plotted_y_max_value = plotted_y_max;

	}

	void set_trace_gridlines(std::vector<float> & x_gridlines, std::vector<String> & x_gridline_labels, String x_unit, 
		std::vector<float> & y_gridlines, std::vector<String> & y_gridline_labels, String y_unit) {

		trace_x_gridline_coord = x_gridlines;
		trace_x_gridline_labels = x_gridline_labels;
		trace_x_unit = x_unit;
		trace_y_gridline_coord = y_gridlines;
		trace_y_gridline_labels = y_gridline_labels;
		trace_y_unit = y_unit;

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
	std::vector<String> trace_x_gridline_labels, trace_y_gridline_labels;
	String trace_x_unit, trace_y_unit;

	std::vector<float> data_x_coord, data_y_coord; //the x and y data
	
};

class flexplot    : public Component, public Slider::Listener
{
public:
    flexplot()
    {
		addAndMakeVisible(x_center_slider),
			x_center_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			x_center_slider.setRange(0.0, 1.0, 0.0),
			x_center_slider.setValue(0.5, dontSendNotification),
			x_center_slider_value = x_center_slider.getValue(),
			x_center_slider.setDoubleClickReturnValue(true, 0.5),
			x_center_slider.addListener(this);
		addAndMakeVisible(y_zoom_slider),
			y_zoom_slider.setSliderStyle(Slider::SliderStyle::LinearVertical),
			y_zoom_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			y_zoom_slider.setRange(1.0, max_vert_zoom, 0.0),
			y_zoom_slider.setValue(1.0, dontSendNotification),
			y_zoom_slider_value = y_zoom_slider.getValue(),
			y_zoom_slider.setDoubleClickReturnValue(true, 1.0),
			y_zoom_slider.addListener(this);
		addAndMakeVisible(y_center_slider),
			y_center_slider.setSliderStyle(Slider::SliderStyle::LinearVertical),
			y_center_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			y_center_slider.setRange(0.0, 1.0, 0.0),
			y_center_slider.setValue(0.5, dontSendNotification),
			y_center_slider_value = y_center_slider.getValue(),
			y_center_slider.setDoubleClickReturnValue(true, 0.5),
			y_center_slider.addListener(this);
		addAndMakeVisible(x_zoom_slider),
			x_zoom_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, 1, 60, 30),
			x_zoom_slider.setRange(1.0, max_hor_zoom, 0.0),
			x_zoom_slider.setValue(1.0, dontSendNotification),
			x_zoom_slider_value = x_zoom_slider.getValue(),
			x_zoom_slider.setDoubleClickReturnValue(true, 1.0),
			x_zoom_slider.addListener(this);
		
    }

    ~flexplot()
    {
    }
	
	virtual void draw_custom_overlay(Graphics& g) {}

	void set_axis_modes(bool linear_x_axis, bool linear_y_axis) {

		plot_x_axis_linear = linear_x_axis;
		plot_y_axis_linear = linear_y_axis;

	}

	//the input to this function is the on-screen x position of the mouse pointer 
	//over the visible area of the plot, reported as a proportion of the range 0 to 1, 
	//where 0 is the left edge of the visible plot region and 1 is the right edge
	void set_screen_to_data_x_position_proportion(float x_proportion) {

		screen_to_data_x_position_proportion = x_proportion;

	}

	std::vector<std::pair<float, float>> get_screen_to_data_pairs() {

		return screen_to_data_return_values;

	}

	/*void paint(Graphics& g) override
	{

	};*/

    void paint (Graphics& g) override
    {
		//g.fillAll(Colour(20, 20, 20));

		g.setColour(Colours::black);
		g.fillRect(plot_area);

		//g.setColour(Colours::white);
		//g.drawRect(top_region, 1.0);
		//g.drawRect(left_region, 1.0);
		//g.drawRect(y_zoom_slider_area, 1.0);
		//g.drawRect(left_label_area, 1.0);
		//g.drawRect(left_label_moving_center, 1.0);
		//g.drawRect(plot_area, 1.0);
		//g.drawRect(y_center_slider_area, 1.0);
		//g.drawRect(bottom_region, 1.0);
		//g.drawRect(bottom_label_area, 1.0);
		//g.drawRect(bottom_label_moving_center, 1.0);
		//g.drawRect(x_zoom_slider_area, 1.0);
		//g.drawRect(upper_left_corner, 1.0);
		//g.drawRect(upper_right_corner, 1.0);
		//g.drawRect(bottom_left_corner, 1.0);
		//g.drawRect(bottom_right_corner, 1.0);

		//g.setColour(Colours::orange);
		//g.drawRect(y_max_fixed_label_outline, 1.0);
		//g.drawRect(y_min_fixed_label_outline, 1.0);
		//g.drawRect(x_min_fixed_label_outline, 1.0);
		//g.drawRect(x_max_fixed_label_outline, 1.0);

		screen_to_data_return_values.clear();
		
		for (int data_set = 0; data_set < data_sets.size(); data_set++) {

			//Get the extremes of plotting from current data set.

			original_plotted_x_min = data_sets[data_set]->trace_plotted_x_min_value;
			original_plotted_x_max = data_sets[data_set]->trace_plotted_x_max_value;
			original_plotted_y_min = data_sets[data_set]->trace_plotted_y_min_value;
			original_plotted_y_max = data_sets[data_set]->trace_plotted_y_max_value;
			calculate_actual_min_max(); //Transform the original plotting extremes to new ones based on zoom and pan controls
			
			bool render_gridlines_labels{ false };

			if (data_sets[data_set]->master_trace == true) { //The master trace defines the gridlines and axis labeling

				plot_x_gridline_coord = data_sets[data_set]->trace_x_gridline_coord;
				plot_x_gridline_labels = data_sets[data_set]->trace_x_gridline_labels;
				plot_y_gridline_coord = data_sets[data_set]->trace_y_gridline_coord;
				plot_y_gridline_labels = data_sets[data_set]->trace_y_gridline_labels;

				unit_x_string = data_sets[data_set]->trace_x_unit;
				unit_y_string = data_sets[data_set]->trace_y_unit;

				set_fixed_label_text();
				render_gridlines_labels = true; //Only render the gridlines and labels once, when the master trace is reached

			}
			
			if (render_gridlines_labels == true) {

				jassert(plot_x_gridline_coord.size() == plot_x_gridline_labels.size()); //there must be a label for each x gridline
				jassert(plot_y_gridline_coord.size() == plot_y_gridline_labels.size()); //there must be a label for each y gridline

				int text_label_height_pix = bottom_label_area.getHeight()*0.6;
				g.setFont(text_label_height_pix);

				int x_min_fixed_label_text_width = g.getCurrentFont().getStringWidth(x_min_fixed_label_text);
				x_min_fixed_label_outline.setBounds(
					bottom_label_area.getX(),
					bottom_label_area.getY(),
					x_min_fixed_label_text_width,
					bottom_label_area.getHeight()
				);

				int x_max_fixed_label_text_width = g.getCurrentFont().getStringWidth(x_max_fixed_label_text);
				x_max_fixed_label_outline.setBounds(
					bottom_label_area.getRight() - x_max_fixed_label_text_width,
					bottom_label_area.getY(),
					x_max_fixed_label_text_width,
					bottom_label_area.getHeight()
				);
								
				y_min_fixed_label_outline.setBounds(
					left_label_area.getX(),
					left_label_area.getBottom() - text_label_height_pix * 1.5,
					left_label_area.getWidth(),
					text_label_height_pix * 1.5
				);

				y_max_fixed_label_outline.setBounds(
					left_label_area.getX(),
					left_label_area.getY(),
					left_label_area.getWidth(),
					text_label_height_pix * 1.5
				);
				
				for (int x_index = 0; x_index < plot_x_gridline_coord.size(); x_index++) {
										
					if (plot_x_gridline_coord[x_index] >= actual_plotted_x_min && plot_x_gridline_coord[x_index] <= actual_plotted_x_max) {

						g.setColour(Colour(50, 50, 50));

						g.fillRect(
							data_coord_to_plot_screen_coord(plot_x_gridline_coord[x_index], 0).first,
							plot_screen_y_min,
							gridline_thickness_pix,
							plot_screen_y_max - plot_screen_y_min);

						g.setColour(Colours::white);
						int x_floating_label_width_pix = g.getCurrentFont().getStringWidth(plot_x_gridline_labels[x_index]);
						
						Rectangle<int> x_floating_label_outline{
							data_coord_to_plot_screen_coord(plot_x_gridline_coord[x_index], 0.0).first - (x_floating_label_width_pix / 2),
							bottom_label_area.getCentreY() - (text_label_height_pix / 2),
							x_floating_label_width_pix,
							text_label_height_pix
						};

						if (x_floating_label_outline.intersects(x_min_fixed_label_outline) == false &&
							x_floating_label_outline.intersects(x_max_fixed_label_outline) == false) {

							g.drawText(
								plot_x_gridline_labels[x_index],
								x_floating_label_outline,
								Justification::centred,
								false);

							//g.drawRect(x_floating_label_outline, 1.0);

						}

					}

				}

				for (int y_index = 0; y_index < plot_y_gridline_coord.size(); y_index++) {

					if (plot_y_gridline_coord[y_index] >= actual_plotted_y_min && plot_y_gridline_coord[y_index] <= actual_plotted_y_max) {
						
						if(plot_y_gridline_coord[y_index] != 0.0){ g.setColour(Colour(50, 50, 50)); }
						if (plot_y_gridline_coord[y_index] == 0.0) { g.setColour(Colours::white); }

						g.fillRect(
							plot_screen_x_min,
							data_coord_to_plot_screen_coord(0, plot_y_gridline_coord[y_index]).second,
							plot_screen_x_max - plot_screen_x_min,
							gridline_thickness_pix);

						g.setColour(Colours::white);
						int y_floating_label_width_pix = g.getCurrentFont().getStringWidth(plot_y_gridline_labels[y_index]);

						Rectangle<int> y_floating_label_outline{
							left_label_area.getCentreX() - (y_floating_label_width_pix / 2),
							data_coord_to_plot_screen_coord(0.0, plot_y_gridline_coord[y_index]).second - (text_label_height_pix / 2),
							y_floating_label_width_pix,
							text_label_height_pix
						};

						if (y_floating_label_outline.intersects(y_min_fixed_label_outline) == false &&
							y_floating_label_outline.intersects(y_max_fixed_label_outline) == false) {

							g.drawText(
								plot_y_gridline_labels[y_index],
								y_floating_label_outline,
								Justification::centred,
								false);

							//g.drawRect(y_floating_label_outline, 1.0);

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
				bool first_plotted_datapoint{ false };

				for (int coordinate = 0; coordinate < data_sets[data_set]->number_data_points() - 1; coordinate++) {

					float current_x_value = data_sets[data_set]->return_x_data_point(coordinate);

					if (current_x_value < actual_plotted_x_min || current_x_value > actual_plotted_x_max) { continue; }
					
					float current_y_value = data_sets[data_set]->return_y_data_point(coordinate);

					if (current_y_value < actual_plotted_y_min) {

						current_y_value = actual_plotted_y_min;

					}

					if (current_y_value > actual_plotted_y_max) {

						current_y_value = actual_plotted_y_max;

					}

					std::pair<int, int> current_point_screen_coord;

					current_point_screen_coord = data_coord_to_plot_screen_coord(current_x_value, current_y_value);

					if (first_plotted_datapoint == false) {

						current_trace.startNewSubPath(current_point_screen_coord.first, current_point_screen_coord.second);
						first_plotted_datapoint = true;

					}

					if (first_plotted_datapoint == true) {

						current_trace.lineTo(current_point_screen_coord.first, current_point_screen_coord.second);

					}

				}

				g.setColour(data_sets[data_set]->trace_line_color);
				current_trace = current_trace.createPathWithRoundedCorners(5.0);
				g.strokePath(current_trace, PathStrokeType(data_sets[data_set]->trace_line_thickess_pix));

			}

			screen_to_data_return_values.push_back(
				visible_plot_screen_proportion_to_trace_coord(data_sets[data_set], screen_to_data_x_position_proportion));

		}
						
		draw_custom_overlay(g);

    }
	
    void resized() override
    {
		component_outline = getLocalBounds();

		int original_width = component_outline.getWidth(), original_height = component_outline.getHeight();

		top_region = component_outline.removeFromTop(original_height*0.05);
		upper_left_corner = top_region.removeFromLeft(original_width*0.15);
		upper_right_corner = top_region.removeFromRight(original_width*0.05);
		x_center_slider_area = top_region;
		bottom_region = component_outline.removeFromBottom(original_height*0.10);
		bottom_left_corner = bottom_region.removeFromLeft(original_width*0.15);
		bottom_right_corner = bottom_region.removeFromRight(original_width*0.05);
		bottom_label_area = bottom_region.removeFromTop(original_height*0.05);
		bottom_label_moving_center = bottom_label_area;
		x_zoom_slider_area = bottom_region.removeFromTop(original_height*0.05);
		left_region = component_outline.removeFromLeft(original_width*0.15);
		y_zoom_slider_area = left_region.removeFromLeft(original_width*0.05);
		left_label_area = left_region.removeFromLeft(original_width*0.10);
		left_label_moving_center = left_label_area;
		plot_area = component_outline.removeFromLeft(original_width*0.8);
		y_center_slider_area = component_outline;

		plot_screen_x_min = plot_area.getX();
		plot_screen_x_max = plot_screen_x_min + plot_area.getWidth();
		plot_screen_y_min = plot_area.getY();
		plot_screen_y_max = plot_screen_y_min + plot_area.getHeight();

		x_center_slider.setBounds(x_center_slider_area);
		x_zoom_slider.setBounds(x_zoom_slider_area);
		y_center_slider.setBounds(y_center_slider_area);
		y_zoom_slider.setBounds(y_zoom_slider_area);
		
    }

	void set_plot_max_zooms(int horizontal_max, int vertical_max) {

		max_hor_zoom = horizontal_max;
		max_vert_zoom = vertical_max;

		y_zoom_slider.setRange(1.0, max_vert_zoom, 0.0);
		x_zoom_slider.setRange(1.0, max_hor_zoom, 0.0);

	}

	void set_y_center_slider_default(double new_value) {

		y_center_slider.setValue(new_value);
		y_center_slider.setDoubleClickReturnValue(true, new_value);

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
					x_center_slider_area,
					left_region,
					y_zoom_slider_area,
					left_label_area,
					y_max_fixed_label_outline,
					left_label_moving_center,
					y_min_fixed_label_outline,
					plot_area,
					y_center_slider_area,
					bottom_region,
					bottom_label_area,
					x_min_fixed_label_outline,
					bottom_label_moving_center,
					x_max_fixed_label_outline,
					x_zoom_slider_area,
					upper_left_corner,
					upper_right_corner,
					bottom_left_corner,
					bottom_right_corner;

	Slider	x_center_slider,
			y_zoom_slider,
			y_center_slider,
			x_zoom_slider;

	String unit_x_string, unit_y_string;

	String 	x_min_fixed_label_text,
			x_max_fixed_label_text,
			y_min_fixed_label_text,
			y_max_fixed_label_text;

	std::vector<flexplot_trace*> data_sets; //holds pointers to all data sets that have been added to the plot;

	float original_plotted_x_min, original_plotted_x_max, original_plotted_y_min, original_plotted_y_max; //data coordinates, not screen coordinates

	int plot_screen_x_min, plot_screen_x_max, plot_screen_y_min, plot_screen_y_max; //screen cordinates

	float actual_plotted_x_min, actual_plotted_x_max, actual_plotted_y_min, actual_plotted_y_max; //boundary of zoomed in area of plot, data coordinates
	
	int max_hor_zoom{ 10 }, max_vert_zoom{ 5 };

	float x_grid_spacing, y_grid_spacing;

	std::vector<float> plot_x_gridline_coord, plot_y_gridline_coord; //position of x and y gridlines, in data coordinates
	std::vector<String> plot_x_gridline_labels, plot_y_gridline_labels; 

	int gridline_thickness_pix{ 1 };

	int trace_thickness_pix{ 2 };

	float x_zoom_slider_value{ 1.0 }, x_center_slider_value{ 0.0 }, y_zoom_slider_value{ 1.0 }, y_center_slider_value{ 0.0 };

	bool plot_x_axis_linear{ true }; //if false, then the x-axis will be base-10 logarithmic
	bool plot_y_axis_linear{ true }; //if false, then the y-axis will be base-10 logarithmic

	//This is the x position that will be used by "visible_plot_screen_proportion_to_trace_coord()"
	float screen_to_data_x_position_proportion{ 0.0};
	
	//Each of the pairs in this vector corresponds to the x and y value of a trace at the position
	//calculated by "visible_plot_screen_proportion_to_trace_coord"
	//The order of the pairs in this vector matches the order of the flexplot_trace objects in "data_sets"
	std::vector<std::pair<float,float>> screen_to_data_return_values;

	//========================================//

	void sliderValueChanged(Slider* slider) override {

		if (slider == &x_center_slider) {

			x_center_slider_value = x_center_slider.getValue();

		}

		else if (slider == &y_zoom_slider) {

			y_zoom_slider_value = y_zoom_slider.getValue();
			
		}

		else if (slider == &y_center_slider) {

			y_center_slider_value = y_center_slider.getValue();

		}

		else if (slider == &x_zoom_slider) {

			x_zoom_slider_value = x_zoom_slider.getValue();

		}

		else {}

		//repaint();

	}

	void calculate_actual_min_max() {
		
		if (plot_x_axis_linear == true) {

			float original_x_plotting_range = original_plotted_x_max - original_plotted_x_min;
			float visible_x_plotting_range = original_x_plotting_range / x_zoom_slider_value;
			float invisible_x_plotting_range = original_x_plotting_range - visible_x_plotting_range;
			float visible_x_plotting_range_start = original_plotted_x_min + invisible_x_plotting_range * x_center_slider_value;

			actual_plotted_x_min = visible_x_plotting_range_start;
			actual_plotted_x_max = visible_x_plotting_range_start + visible_x_plotting_range;
			
		}

		if (plot_x_axis_linear == false) {

			jassert(original_plotted_x_min > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(original_plotted_x_max > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis

			float original_x_plotting_range_log = log10(original_plotted_x_max) - log10(original_plotted_x_min);
			float visible_x_plotting_range_log = original_x_plotting_range_log / x_zoom_slider_value;
			float invisible_x_plotting_range_log = original_x_plotting_range_log - visible_x_plotting_range_log;
			float visible_x_plotting_range_start_log = log10(original_plotted_x_min) + invisible_x_plotting_range_log * x_center_slider_value;

			actual_plotted_x_min = pow(10, visible_x_plotting_range_start_log);
			actual_plotted_x_max = pow(10, visible_x_plotting_range_start_log + visible_x_plotting_range_log);

		}

		if (plot_y_axis_linear == true) {

			float original_y_plotting_range = original_plotted_y_max - original_plotted_y_min;
			float visible_y_plotting_range = original_y_plotting_range / y_zoom_slider_value;
			float invisible_y_plotting_range = original_y_plotting_range - visible_y_plotting_range;
			float visible_y_plotting_range_start = original_plotted_y_min + invisible_y_plotting_range * y_center_slider_value;

			actual_plotted_y_min = visible_y_plotting_range_start;
			actual_plotted_y_max = visible_y_plotting_range_start + visible_y_plotting_range;

		}

		if (plot_y_axis_linear == false) {

			jassert(original_plotted_y_min > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(original_plotted_y_max > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis

			float original_y_plotting_range_log = log10(original_plotted_y_max) - log10(original_plotted_y_min);
			float visible_y_plotting_range_log = original_y_plotting_range_log / y_zoom_slider_value;
			float invisible_y_plotting_range_log = original_y_plotting_range_log - visible_y_plotting_range_log;
			float visible_y_plotting_range_start_log = log10(original_plotted_y_min) + invisible_y_plotting_range_log * y_center_slider_value;

			actual_plotted_y_min = pow(10, visible_y_plotting_range_start_log);
			actual_plotted_y_max = pow(10, visible_y_plotting_range_start_log + visible_y_plotting_range_log);

		}
		
	}

	void set_fixed_label_text() {

		x_min_fixed_label_text = String(actual_plotted_x_min,1,false) + " " + unit_x_string;
		x_max_fixed_label_text = String(actual_plotted_x_max,1, false) + " " + unit_x_string;
		
		y_min_fixed_label_text = String(actual_plotted_y_min,1,false) + " " + unit_y_string;
		y_max_fixed_label_text = String(actual_plotted_y_max,1,false) + " " + unit_y_string;

		if (actual_plotted_x_min > 0.0) { x_min_fixed_label_text = "+" + x_min_fixed_label_text;}
		if (actual_plotted_x_max > 0.0) { x_max_fixed_label_text = "+" + x_max_fixed_label_text;}
		
		if (actual_plotted_y_min > 0.0) { y_min_fixed_label_text = "+" + y_min_fixed_label_text;}
		if (actual_plotted_y_max > 0.0) { y_max_fixed_label_text = "+" + y_max_fixed_label_text;}
		
	}

	std::pair<int,int> data_coord_to_plot_screen_coord(float x_data_coord, float y_data_coord) {

		float x_position_proportion{ 0.0 };
		float y_position_proportion{ 0.0 };

		if (plot_x_axis_linear == true) {

			//the following line calculates the position of the input data x coordinate as a proportion from 0 to 1,
			//where 0 is the left edge of the visible plot region and 1 is the right edge, for a linear x-axis
			x_position_proportion = (x_data_coord - actual_plotted_x_min) / (actual_plotted_x_max - actual_plotted_x_min);

		}

		if (plot_y_axis_linear == true) {

			//the following line calculates the position of the input data y coordinate as a proportion from 0 to 1,
			//where 0 is the lower edge of the visible plot region and 1 is the upper edge.
			y_position_proportion = (y_data_coord - actual_plotted_y_min) / (actual_plotted_y_max - actual_plotted_y_min);

		}

		if (plot_x_axis_linear == false) {

			jassert(actual_plotted_x_min > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(actual_plotted_x_max > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(x_data_coord >= 0.0); //negative x-coordinates not allowed on logarithmic x-axis

			if (x_data_coord > 0.0) {

				//the following lines calculate the position of the input data x coordinate as a proportion from 0 to 1,
				//where 0 is the left edge of the visible plot region and 1 is the right edge, for a base10 log x-axis
				float x_axis_max_log = log10(actual_plotted_x_max);
				float x_axis_min_log = log10(actual_plotted_x_min);
				float x_data_coord_log = log10(x_data_coord);

				x_position_proportion = (x_data_coord_log - x_axis_min_log) / (x_axis_max_log - x_axis_min_log);

			}
			
			if (x_data_coord == 0.0){

				x_position_proportion = 0.0;

			}

		}

		if (plot_y_axis_linear == false) {

			jassert(actual_plotted_y_min > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(actual_plotted_y_max > 0.0); //the extrema of the plotting range must be greater than 0 on a log axis
			jassert(y_data_coord >= 0.0); //negative y-coordinates not allowed on logarithmic x-axis

			if (y_data_coord > 0.0) {

				//the following lines calculate the position of the input data y coordinate as a proportion from 0 to 1,
				//where 0 is the bottom edge of the visible plot region and 1 is the top edge, for a base10 log y-axis
				float y_axis_max_log = log10(actual_plotted_y_max);
				float y_axis_min_log = log10(actual_plotted_y_min);
				float y_data_coord_log = log10(y_data_coord);

				y_position_proportion = (y_data_coord_log - y_axis_min_log) / (y_axis_max_log - y_axis_min_log);

			}

			if (y_data_coord == 0.0){

				y_position_proportion = 0.0;

			}

		}
		
		//the following two lines calculate the width, in pixels, of the x and y axes of the visible plot region. 
		int plot_screen_x_span = plot_screen_x_max - plot_screen_x_min;
		int plot_screen_y_span = plot_screen_y_max - plot_screen_y_min;

		int x_screen_cord = plot_screen_x_min + (x_position_proportion * plot_screen_x_span);
		int y_screen_cord = plot_screen_y_max - (y_position_proportion * plot_screen_y_span);

		std::pair<int,int> screen_cord;

		screen_cord.first = x_screen_cord, screen_cord.second = y_screen_cord;

		return screen_cord;

	}

	//it is assumed that the input x position is given as a fraction of the visible plot area, where 0 
	//is the left side of the plot and 1 is the right
	std::pair<float,float> visible_plot_screen_proportion_to_trace_coord(flexplot_trace* trace, float plot_x_visible_screen_proportion) {
		
		if (plot_x_axis_linear == false) {

			float x_coord_log = ((log10(actual_plotted_x_max) - log10(actual_plotted_x_min)) * plot_x_visible_screen_proportion)
				+ log10(actual_plotted_x_min);
			float x_coord = pow(10, x_coord_log);

			return std::pair<float, float>{x_coord, linear_interpolator::interpolate(trace->data_x_coord, trace->data_y_coord, x_coord)};

		}

		else{

			float x_coord = ((actual_plotted_x_max - actual_plotted_x_min) * plot_x_visible_screen_proportion) + actual_plotted_x_min;

			return std::pair<float, float>{x_coord, linear_interpolator::interpolate(trace->data_x_coord, trace->data_y_coord, x_coord)};

		}

	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (flexplot)
};
