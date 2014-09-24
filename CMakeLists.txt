cmake_minimum_required(VERSION 2.8.9)

project(LightWeightRobotIGT)

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/LightWeightRobotIGT")
set(EXTENSION_CATEGORY "IGT")
set(EXTENSION_CONTRIBUTORS "Sebastian Tauscher (LUH), Junichi Tokuda (BWH)")
set(EXTENSION_DESCRIPTION "3D Slicer module for communication between 3D Slicer and LightWeight robot.")
set(EXTENSION_ICONURL "http://www.slicer.org/slicerWiki/images/5/58/LightWeightRobotIGT.png")
set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-LightWeightRobotIGT-screenshot.png")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(LightWeightRobotIGT)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
