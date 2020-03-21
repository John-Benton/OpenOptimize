/*
  ==============================================================================

    constants.h
    Created: 18 Sep 2017 6:42:54pm
    Author:  John

  ==============================================================================
*/

#pragma once

class constants
{
public:

	JUCE_LEAK_DETECTOR(constants);

	static const int largest_fft_size = 32768;
	static const int sample_rate = 48000;
	static const int fft_bins = (largest_fft_size / 2) + 1; //the adddition of one accounts for the DC component bin
	static const int max_delay_samples = 48000; //this will allow for up to one second of delay between ref and system signals
	static const int composite_fft_bins = 224;

};
