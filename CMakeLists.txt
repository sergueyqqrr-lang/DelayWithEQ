cmake_minimum_required(VERSION 3.22)

project(DelayWithEQ VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE (you need to have JUCE as a submodule or installed)
# Option 1: If JUCE is a git submodule in the folder "JUCE"
add_subdirectory(JUCE)

# Option 2: If you prefer FetchContent (uncomment if needed)
# include(FetchContent)
# FetchContent_Declare(
#     JUCE
#     GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
#     GIT_TAG        8.0.0   # or the version you want
# )
# FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DelayWithEQ
    COMPANY_NAME "YourName"
    PLUGIN_MANUFACTURER_CODE YrNm
    PLUGIN_CODE DlEQ
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "DelayWithEQ"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(DelayWithEQ
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(DelayWithEQ
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DelayWithEQ
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
