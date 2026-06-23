cmake_minimum_required(VERSION 3.22)  # JUCE requires 3.22 minimum
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
project(audio-engine VERSION 1.0.0)


# C++ Standard


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Default Build Type


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified. Defaulting to Release.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


# JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.13
)

FetchContent_MakeAvailable(JUCE)


# Platform-Specific Dependencies (sndfile)


if(WIN32)
    find_package(SndFile CONFIG REQUIRED HINTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/sndfile")
    add_library(SndFileTarget INTERFACE)
    target_link_libraries(SndFileTarget INTERFACE SndFile::sndfile)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
    add_library(SndFileTarget INTERFACE)
    target_link_libraries(SndFileTarget INTERFACE PkgConfig::SNDFILE)
endif()


# Fetch kissfft


FetchContent_Declare(
    kissfft
    GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
    GIT_TAG 7bce415
)

set(KISSFFT_DATATYPE float CACHE STRING "" FORCE)
set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)
set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(kissfft)


# Audio Engine Library
# Add the paths of the .cpp files you wish to link for the audio engine inside add_library


add_library(AudioEngineLib STATIC
    src/audio/AudioEngine.cpp
)

target_include_directories(AudioEngineLib
    PUBLIC
        src
        ${kissfft_SOURCE_DIR}
)

target_link_libraries(AudioEngineLib
    PUBLIC
        SndFileTarget
        kissfft
)

# Modify the parameters of the plugin-generation as per your needs
# Set your plugin name here.
set(PLUGIN_NAME UserPlugin)

juce_add_plugin(${PLUGIN_NAME}
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    FORMATS VST3 Standalone
    PRODUCT_NAME ${PLUGIN_NAME}

)

#Set your Effect name here, which is also the directory where your PluginProcessor.h, etc. live in.
set(EFFECT_NAME EffectName)
if(EFFECT_NAME STREQUAL "")
    message(FATAL_ERROR "EFFECT_NAME is empty. Set your effect folder name.")
endif()

if(EFFECT_NAME STREQUAL "EffectName")
    message(WARNING "Default EFFECT_NAME is still set")
endif()

target_sources(
    ${PLUGIN_NAME} PRIVATE
    src/${EFFECT_NAME}/PluginProcessor.cpp
    src/${EFFECT_NAME}/PluginProcessor.h
    src/${EFFECT_NAME}/PluginEditor.h
    src/${EFFECT_NAME}/PluginEditor.cpp
)

# Add effect-specific source files here

add_library(
    EffectLib STATIC
    src/effects/Dummy.cpp

)

target_link_libraries(
    ${PLUGIN_NAME} PRIVATE
    EffectLib

    juce::juce_audio_utils
    juce::juce_audio_processors
    juce_recommended_config_flags
    juce_recommended_lto_flags
    juce_recommended_warning_flags
)
target_compile_definitions(${PLUGIN_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0 
)
juce_generate_juce_header(${PLUGIN_NAME})

# Main / Benchmark Applications

add_executable(mainApp src/app/main_engine.cpp)
target_link_libraries(mainApp PRIVATE AudioEngineLib)

add_executable(benchmarkApp src/app/benchmark_main.cpp)
target_link_libraries(benchmarkApp PRIVATE AudioEngineLib)
