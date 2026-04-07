cmake_minimum_required(VERSION 3.22)
project(Byter VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(/Users/nathanbalducci/JUCE /Users/nathanbalducci/JUCE/Byter/Byter/build/JUCE)

juce_add_plugin(Byter
    COMPANY_NAME "HSDX"
    BUNDLE_ID "com.hsdx.byter"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Hsdx
    PLUGIN_CODE Bytr
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Byter"
)

juce_generate_juce_header(Byter)

target_sources(Byter PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(Byter PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(Byter PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
