cmake_minimum_required(VERSION 3.15)
project(AtmosSpatialAudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source files
set(DSP_SOURCES
    src/dsp/SpatialEngine.cpp
    src/dsp/CenterExtractor.cpp
    src/dsp/SurroundGen.cpp
    src/dsp/HeightGen.cpp
    src/dsp/SpectralAccent.cpp
    src/dsp/HRTFRenderer.cpp
    src/dsp/EarlyReflections.cpp
    src/dsp/FDNReverb.cpp
    src/dsp/BassManager.cpp
    src/dsp/HarmonicBass.cpp
    src/dsp/MasterEQ.cpp
    src/dsp/DialogueEnhancer.cpp
    src/dsp/Crossfeed.cpp
    src/dsp/LoudnessNormalizer.cpp
    src/dsp/TruePeakLimiter.cpp
    src/dsp/Biquad.cpp
    src/dsp/DelayLine.cpp
    src/dsp/ContentClassifier.cpp
    src/dsp/AirAbsorption.cpp
    src/dsp/ParamSmooth.cpp
)

set(AU_SOURCES
    src/au/AtmosAU.cpp
    src/au/AtmosAUEntry.cpp
    src/au/PresetManager.cpp
    src/au/ParameterDefs.cpp
)

set(ALL_SOURCES ${DSP_SOURCES} ${AU_SOURCES})

# Include directories
include_directories(include)

# Platform-specific build
if(APPLE)
    # Find AudioUnit framework
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    
    # Create AU component bundle
    set(AU_BUNDLE "AtmosAU.component")
    set(AU_BUNDLE_DIR "${CMAKE_BINARY_DIR}/${AU_BUNDLE}")
    set(AU_CONTENTS_DIR "${AU_BUNDLE_DIR}/Contents")
    set(AU_MACOS_DIR "${AU_CONTENTS_DIR}/MacOS")
    set(AU_RESOURCES_DIR "${AU_CONTENTS_DIR}/Resources")
    
    # Create bundle structure
    file(MAKE_DIRECTORY "${AU_MACOS_DIR}")
    file(MAKE_DIRECTORY "${AU_RESOURCES_DIR}")
    
    # Build the shared library
    add_library(AtmosAU SHARED ${ALL_SOURCES})
    
    target_link_libraries(AtmosAU 
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
    
    target_compile_definitions(AtmosAU PRIVATE
        __MACOSX_CORE__
        JUCE_USE_CORE_AUDIO=1
    )
    
    set_target_properties(AtmosAU PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${AU_MACOS_DIR}"
        XCODE_GENERATE_SCHEME OFF
        MACOSX_RPATH ON
        BUILD_WITH_INSTALL_RPATH ON
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
    
    # Copy Info.plist to bundle
    add_custom_command(TARGET AtmosAU POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/presets/Info.plist"
            "${AU_CONTENTS_DIR}/Info.plist"
        COMMAND ${CMAKE_COMMAND} -E echo "APPL????" > "${AU_CONTENTS_DIR}/PkgInfo"
    )
    
    # Install target
    install(DIRECTORY "${AU_BUNDLE_DIR}"
        DESTINATION "~/Library/Audio/Plug-Ins/Components"
        USE_SOURCE_PERMISSIONS
    )
    
    message(STATUS "Building AU component for macOS")
    message(STATUS "Output: ${AU_BUNDLE_DIR}")
    
else()
    # Non-macOS build (for testing compilation only)
    message(STATUS "Non-macOS platform detected - building for testing only")
    
    add_library(AtmosAU SHARED ${ALL_SOURCES})
    
    target_compile_definitions(AtmosAU PRIVATE
        TEST_BUILD=1
    )
    
    # Stub out macOS-specific headers for testing
    target_include_directories(AtmosAU PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# Compiler warnings
target_compile_options(AtmosAU PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

# Optimization for Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(AtmosAU PRIVATE -O3 -DNDEBUG)
endif()
