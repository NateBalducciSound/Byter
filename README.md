# Byter (JUCE Plugin)

Byter is an audio effect plugin built with the JUCE framework. It combines a classic bitcrusher with an integrated envelope follower, allowing the incoming audio signal's amplitude to dynamically modulate the degradation effects.

## Features

* Dynamic Bitcrushing: Standard bit depth reduction and sample rate decimation for classic digital distortion.
* Integrated Envelope Follower: Extracts the amplitude envelope of the incoming audio signal to modulate the bitcrusher parameters in real-time.
* Modulation Controls: Fully adjustable Attack and Release parameters for the envelope follower, giving you tight control over how quickly the degradation reacts to transients and tails.
* Depth Control: Scale exactly how much the envelope follower influences the bit depth and sample rate.
* Cross-Platform: Available as a VST3, AU, and Standalone application for Windows and macOS.

## Prerequisites

To build Byter from the source code, you will need:

* JUCE Framework (v7.0 or higher recommended)
* A C++ IDE (Visual Studio for Windows, Xcode for macOS)
* Projucer (included with JUCE) or CMake

## Build Instructions

### Using Projucer
1. Clone this repository to your local machine.
2. Open the `Byter.jucer` file using the Projucer application.
3. Ensure your Global Paths in Projucer are pointing to your local JUCE modules folder.
4. Click the "Save and Open in IDE" button for your target platform (Visual Studio or Xcode).
5. Build the project in your IDE (Make sure to build in 'Release' mode for optimal performance).

### Using CMake
1. Clone this repository.
2. Open a terminal or command prompt in the repository root.
3. Run `cmake -B build` to configure the project.
4. Run `cmake --build build --config Release` to compile the plugin.

## Usage

Once built, the compiled plugin files (.vst3, .component, etc.) will be located in the `Builds` folder. Copy the appropriate file to your system's plugin directory:

* Windows VST3: `C:\Program Files\Common Files\VST3`
* macOS VST3: `/Library/Audio/Plug-Ins/VST3`
* macOS AU: `/Library/Audio/Plug-Ins/Components`

Open your preferred Digital Audio Workstation (DAW), scan for new plugins, and load Byter onto an audio track to start degrading your sound.

## Acknowledgments

* Built using the JUCE framework.
