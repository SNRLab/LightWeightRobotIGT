/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

#ifndef __vtkIGTLToMRMLPoint_h
#define __vtkIGTLToMRMLPoint_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerLightWeightRobotIGTModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlPointMessage.h>

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class VTK_SLICER_LIGHTWEIGHTROBOTIGT_MODULE_MRML_EXPORT vtkIGTLToMRMLPoint : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLPoint *New();
  vtkTypeMacro(vtkIGTLToMRMLPoint,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POINT"; };
  virtual const char*  GetMRMLName() { return "IGTLPoint"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLPoint();
  ~vtkIGTLToMRMLPoint();
  vtkIGTLToMRMLPoint(const vtkIGTLToMRMLPoint&);
  void operator=(const vtkIGTLToMRMLPoint&);

 protected:
  igtl::PointMessage::Pointer OutPointMsg;

};


#endif //__vtkIGTLToMRMLPoint_h


