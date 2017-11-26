# OpenOptimize

Welcome to the repository for the OpenOptimize project. The goal of this project is to create a free and open source dual-channel FFT-based audio analyser. Using OpenOptimize, a person should be able to measure the electro-acoustic transfer function of a sound sytem and use those measurments to make informed adjustments to the system. 

OpenOptimize uses [FFTW](http://www.fftw.org/) to perform a fourier transform on the system and reference audio signals. Audio I/O and the application's UI are handled by [JUCE](https://www.juce.com/tutorials)

Visual Studio 2017 is recommended for making code edits and building OpenOptimize. In addition, you will need to have JUCE v5 installed on your system.
