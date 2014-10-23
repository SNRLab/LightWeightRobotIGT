/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLPoint.h"

// OpenIGTLink includes
#include <igtlPointMessage.h>
#include <igtlWin32Header.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkNew.h>
#include <vtkCollection.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// MRML includes
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLAnnotationHierarchyNode.h"
#include "vtkMRMLAnnotationFiducialNode.h"
//#include "vtkSlicerAnnotationModuleLogic.h"
          
//#include "qSlicerCoreApplication.h"
//#include "qSlicerAbstractCoreModule.h"
//#include "qSlicerModuleManager.h"




//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLPoint);
//vtkCxxRevisionMacro(vtkIGTLToMRMLPoint, "$Revision: 15552 $");

//---------------------------------------------------------------------------
vtkIGTLToMRMLPoint::vtkIGTLToMRMLPoint()
{
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLPoint::~vtkIGTLToMRMLPoint()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLPoint::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLPoint::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLAnnotationHierarchyNode* pointNode;

  pointNode = vtkMRMLAnnotationHierarchyNode::New();
  pointNode->SetName(name);
  pointNode->SetDescription("Received by OpenIGTLink");

  vtkMRMLNode* n = scene->AddNode(pointNode);
  pointNode->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLPoint::GetNodeEvents()
{
  return NULL;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLPoint::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkIGTLToMRMLBase::IGTLToMRML(buffer, node);

  // Create a message buffer to receive transform data
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->Copy(buffer);  // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = pointMsg->Unpack(this->CheckCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  if (node == NULL)
    {
    return 0;
    }

  vtkMRMLAnnotationHierarchyNode* hierarchyNode =
    vtkMRMLAnnotationHierarchyNode::SafeDownCast(node);
  
  vtkNew<vtkCollection> collection;
  hierarchyNode->GetDirectChildren(collection.GetPointer());
  int nItems = collection->GetNumberOfItems();
  collection->InitTraversal();
  int nElements = pointMsg->GetNumberOfPointElement();
  
 
  for (int i = 0; i < nElements; i ++)
    {
    igtl::PointElement::Pointer pointElement;
    pointMsg->GetPointElement(i, pointElement);
    igtlFloat32 pos[3];
    pointElement->GetPosition(pos);
    const char* pointName = pointElement->GetName();

    // Check if there is a fiducial node with the same name
    // TODO: Currently, this will pick up the first one. Maybe we need to check
    // if there is more than one fiducial nodes with the same name.
    int found = 0;
    for (int j = 0; j < nItems; j ++)
      {
      vtkMRMLAnnotationFiducialNode* fnode;
      fnode = vtkMRMLAnnotationFiducialNode::SafeDownCast(collection->GetNextItemAsObject());
      if (fnode)
        {
        if (strcmp(fnode->GetName(), pointName) == 0)
          {
          fnode->SetFiducialCoordinates(pos[0], pos[1], pos[2]);
          //igtlUint8 rgba[4];
          //pointElement->GetRGBA(rgba);
          //std::cerr << " GroupName : " << pointElement->GetGroupName() << std::endl;
          //std::cerr << " Radius    : " << std::fixed << pointElement->GetRadius() << std::endl;
          //std::cerr << " Owner     : " << pointElement->GetOwner() << std::endl;
          found = 1;
          }
        }
      }
    if (!found)
      {
      vtkMRMLAnnotationFiducialNode* fnode = vtkMRMLAnnotationFiducialNode::New();
      fnode->SetName(pointName);
      fnode->SetFiducialCoordinates(pos[0], pos[1], pos[2]);
      hierarchyNode->GetScene()->AddNode(fnode);
      fnode->CreateAnnotationTextDisplayNode();
      fnode->CreateAnnotationPointDisplayNode();
      //fnode->SetTextScale(textScale);
      //fnode->GetAnnotationPointDisplayNode()->SetGlyphScale(symbolScale);
      //fnode->GetAnnotationPointDisplayNode()->SetGlyphType(glyphType);
      //fnode->GetAnnotationPointDisplayNode()->SetColor(color);
      //fnode->GetAnnotationPointDisplayNode()->SetSelectedColor(selColor);
      //fnode->GetAnnotationTextDisplayNode()->SetColor(color);
      //fnode->GetAnnotationTextDisplayNode()->SetSelectedColor(selColor);
      //fnode->SetDisplayVisibility(vis);
      fnode->Delete();
      }
    }
  
  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLPoint::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (mrmlNode && event == vtkMRMLNode::HierarchyModifiedEvent)
    {
    vtkMRMLAnnotationHierarchyNode* hierarchyNode =
      vtkMRMLAnnotationHierarchyNode::SafeDownCast(mrmlNode);
    
    if (!hierarchyNode)
      {
      return 0;
      }
    //igtl::PointMessage::Pointer OutPointMsg;
    if (this->OutPointMsg.IsNull())
      {
      this->OutPointMsg = igtl::PointMessage::New();
      }

    this->OutPointMsg->SetDeviceName(hierarchyNode->GetName());

    // TODO: Change number of elements instead of clearing up and creating new points
    this->OutPointMsg->ClearPointElement();
    
    vtkNew<vtkCollection> collection;
    hierarchyNode->GetDirectChildren(collection.GetPointer());
    int nItems = collection->GetNumberOfItems();
    collection->InitTraversal();

    for (int i = 0; i < nItems; i ++)
      {
      vtkMRMLAnnotationFiducialNode* fnode;
      fnode = vtkMRMLAnnotationFiducialNode::SafeDownCast(collection->GetNextItemAsObject());
      if (fnode)
        {
        double pos[3];
        fnode->GetFiducialCoordinates(pos);

        igtl::PointElement::Pointer pointElem;
        pointElem = igtl::PointElement::New();
        pointElem->SetName(fnode->GetName());
        pointElem->SetRGBA(0x00, 0x00, 0xFF, 0xFF);
        pointElem->SetPosition(pos[0], pos[1], pos[2]);
        this->OutPointMsg->AddPointElement(pointElem);
        }
      }
    
    this->OutPointMsg->Pack();

    *size = this->OutPointMsg->GetPackSize();
    *igtlMsg = (void*)this->OutPointMsg->GetPackPointer();

    return 1;
    }

  return 0;
}


