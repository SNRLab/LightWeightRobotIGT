project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

# Additional directories to include
set(${KIT}_INCLUDED_DIRECTORIES
  ${Slicer_Base_INCLUDE_DIRS}
  ${vtkSlicerLightWeightRobotIGTModuleMRML_SOURCE_DIR}
  ${vtkSlicerLightWeightRobotIGTModuleMRML_BINARY_DIR}
  ${vtkSlicerAnnotationsModuleLogic_BINARY_DIR}
  ${vtkSlicerAnnotationsModuleLogic_SOURCE_DIR}
  )

# Source files
set(${KIT}_SRCS
  vtkMRMLIGTLSessionManagerNode.cxx
  vtkMRMLIGTLSessionManagerNode.h
  vtkIGTLToMRMLPoint.cxx
  vtkIGTLToMRMLString.cxx
  )

# Additional Target libraries
set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${MRML_LIBRARIES}
  vtkSlicerAnnotationsModuleLogic
  vtkSlicerAnnotationsModuleMRML
  vtkSlicerOpenIGTLinkIFModuleMRML
  )

SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
