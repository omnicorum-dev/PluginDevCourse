cmake_minimum_required(VERSION 3.22)

project(asdMonoSynthV3 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================ #

set(JUCE_PATH "/Users/nicorusso/Development/JUCE/"
    CACHE PATH "Path to an external JUCE checkout")

# ============================================================================ #

if(CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
    message(WARNING
        "Generator '${CMAKE_GENERATOR}' does not support "
        "CMAKE_EXPORT_COMPILE_COMMANDS. Configure a separate build "
        "directory with -G Ninja (or 'Unix Makefiles') if you need "
        "compile_commands.json for clangd.")
endif()

if(JUCE_PATH)
    add_subdirectory(${JUCE_PATH} JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# ============================================================================ #

juce_add_plugin(asdMonoSynthV3
    PRODUCT_NAME "[asd] Mono Synth v3"
    COMPANY_NAME "Omnicorum"

    PLUGIN_MANUFACTURER_CODE Omni
    PLUGIN_CODE msv3

    FORMATS AU VST3 Standalone

    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    COPY_PLUGIN_AFTER_BUILD TRUE

    VST3_CATEGORIES Instrument Synth
    AU_MAIN_TYPE kAudioUnitType_MusicDevice
)

juce_generate_juce_header(asdMonoSynthV3)

target_sources(asdMonoSynthV3
    PRIVATE
        src/processor.cpp
)

target_include_directories(asdMonoSynthV3
    PRIVATE
        include/
)

target_compile_definitions(asdMonoSynthV3
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(asdMonoSynthV3
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_devices
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)


