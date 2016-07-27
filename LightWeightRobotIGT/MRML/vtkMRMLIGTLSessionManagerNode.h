/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#ifndef __vtkMRMLIGTLSessionManagerNode_h
#define __vtkMRMLIGTLSessionManagerNode_h

// MRML includes
#include "vtkMRMLNode.h"
#include "vtkSlicerLightWeightRobotIGTModuleMRMLExport.h"

class vtkIGTLToMRMLString;




/// \brief MRML node to manage OpenIGTLink 
///
/// Configure vtkMRMLIGTLConnectorNode and manage incoming and outgoing
/// data during a OpenIGTLink session.
class VTK_SLICER_LIGHTWEIGHTROBOTIGT_MODULE_MRML_EXPORT vtkMRMLIGTLSessionManagerNode : public vtkMRMLNode
{
public:
  static vtkMRMLIGTLSessionManagerNode *New();

  vtkTypeMacro(vtkMRMLIGTLSessionManagerNode,vtkMRMLNode);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  enum VirtualFixtureType { PLANE, CONE, PATH };

  VirtualFixtureType CurrentVirtualFixtureType;
	
  double ConeAngle;

  double DirectionVector[3];
  double StartPointVector[3];
  double EndPointVector[3];
  double VirtualFixtureVector[3];

  double VirtualFixtureOffset;
  /// 
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// 
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "IGTLSessionManager";};

  ///
  /// Set and configure MRMLIGTLconnector node
  void SetAndObserveConnectorNodeID(const char *connectorNodeID);

  ///
  /// Set a reference to transform node
  void AddAndObserveMessageNodeID(const char *transformNodeID);

  /// 
  /// Associated transform MRML node
  //vtkMRMLTransformNode* GetParentTransformNode();

  /// 
  /// alternative method to propagate events generated in Transform nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );

  //BTX
  virtual void OnNodeReferenceAdded(vtkMRMLNodeReference *reference);

  virtual void OnNodeReferenceRemoved(vtkMRMLNodeReference *reference);

  virtual void OnNodeReferenceModified(vtkMRMLNodeReference *reference);
  //ETX

  enum
    {
      TransformModifiedEvent = 15000
    };

  /// Get referenced connector node id
  const char *GetConnectorNodeID();
  unsigned long UID;
  virtual void SendCommand(std::string CommandString);
  int ObserveAcknowledgeString();
  static void StartPointFiducialModified(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);
  static void EndPointFiducialModified(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);
  //void NodeChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);
  void UpdateVirtualFixturePreview();
  void VirtFixOn(std::string name);
  void VirtFixOff();
  bool finish;
    
	vtkMRMLIGTLSessionManagerNode();
  ~vtkMRMLIGTLSessionManagerNode();
  vtkMRMLIGTLSessionManagerNode(const vtkMRMLIGTLSessionManagerNode&);
  void operator=(const vtkMRMLIGTLSessionManagerNode&);

  //----------------------------------------------------------------
  // Reference role strings
  //----------------------------------------------------------------
  char* ConnectorNodeReferenceRole;
  char* ConnectorNodeReferenceMRMLAttributeName;

  vtkSetStringMacro(ConnectorNodeReferenceRole);
  vtkGetStringMacro(ConnectorNodeReferenceRole);

  vtkSetStringMacro(ConnectorNodeReferenceMRMLAttributeName);
  vtkGetStringMacro(ConnectorNodeReferenceMRMLAttributeName);

  char* MessageNodeReferenceRole;
  char* MessageNodeReferenceMRMLAttributeName;

  vtkSetStringMacro(MessageNodeReferenceRole);
  vtkGetStringMacro(MessageNodeReferenceRole);

  vtkSetStringMacro(MessageNodeReferenceMRMLAttributeName);
  vtkGetStringMacro(MessageNodeReferenceMRMLAttributeName);


  char* ConnectorNodeIDInternal;
  vtkSetStringMacro(ConnectorNodeIDInternal);
  vtkGetStringMacro(ConnectorNodeIDInternal);

 
   char* CommandStringNodeIDInternal;
  vtkSetStringMacro(CommandStringNodeIDInternal);
  vtkGetStringMacro(CommandStringNodeIDInternal);

   char* RegistrationTransformNodeIDInternal;
  vtkSetStringMacro(RegistrationTransformNodeIDInternal);
  vtkGetStringMacro(RegistrationTransformNodeIDInternal);
 
  char* SensorTransformNodeIDInternal;
  vtkSetStringMacro(SensorTransformNodeIDInternal);
  vtkGetStringMacro(SensorTransformNodeIDInternal);

  char* AcknowledgeStringNodeIDInternal;
  vtkSetStringMacro(AcknowledgeStringNodeIDInternal);
  vtkGetStringMacro(AcknowledgeStringNodeIDInternal);


  vtkIGTLToMRMLString * StringMessageConverter;

};

#endif
