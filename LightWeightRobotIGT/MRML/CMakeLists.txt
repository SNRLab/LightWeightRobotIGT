project(vtkSlicer${MODULE_NAME}ModuleMRML)

#
# See CMake/SlicerMacroBuildModuleLogic.cmake for details
#


set(module_mrml_name "${PROJECT_NAME}")

# The header '${PROJECT_NAME}Export.h' will be automatically configured.
set(module_mrml_export_directive "VTK_SLICER_LIGHTWEIGHTROBOTIGT_MODULE_MRML_EXPORT")

# Additional directories to include
set(module_mrml_include_directories
  ${Slicer_Base_INCLUDE_DIRS}
  )

# Source files
set(module_mrml_SRCS
  vtkMRMLIGTLSessionManagerNode.cxx
  vtkIGTLToMRMLString.cxx
  )

# Additional Target libraries
set(module_mrml_target_libraries
  ${OpenIGTLink_LIBRARIES}
  ${MRML_LIBRARIES}
  SlicerBaseLogic
  vtkSlicerOpenIGTLinkIFModuleMRML
  vtkSlicerAnnotationsModuleMRML
  )

SlicerMacroBuildModuleMRML(
  NAME ${module_mrml_name}
  EXPORT_DIRECTIVE ${module_mrml_export_directive}
  INCLUDE_DIRECTORIES ${module_mrml_include_directories}
  SRCS ${module_mrml_SRCS}
  TARGET_LIBRARIES ${module_mrml_target_libraries}
  )
