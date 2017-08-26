/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

Portions (c) Copyright 2014 Sebastian Tauscher, Institute of Mechatronic Systems, Leibniz Universitaet Hannover All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer

=========================================================================auto=*/
#define PI 3.14159265358
// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLIGTLSessionManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLAnnotationTextNode.h"
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include "vtkIGTLToMRMLString.h"
#include <vtkMRMLAnnotationFiducialNode.h>
#include <vtkMRMLAnnotationHierarchyNode.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkCollection.h>
#include <vtkCollectionIterator.h>

#include <vtkCallbackCommand.h>
#include <vtkConeSource.h>
#include <vtkCylinder.h>
#include <vtkCylinderSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkIntArray.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>
#include <vtkAlgorithmOutput.h>
//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLIGTLSessionManagerNode);

//----------------------------------------------------------------------------
vtkMRMLIGTLSessionManagerNode::vtkMRMLIGTLSessionManagerNode()
{
  this->AcknowledgeStringNodeIDInternal = 0;
  this->ConnectorNodeIDInternal = 0;
  this->CommandStringNodeIDInternal = 0;
  this->RegistrationTransformNodeIDInternal = 0;
	
  this->ConnectorNodeReferenceRole = 0;
  this->ConnectorNodeReferenceMRMLAttributeName = 0;
  this->MessageNodeReferenceRole = 0;
  this->MessageNodeReferenceMRMLAttributeName = 0;

  this->SetConnectorNodeReferenceRole("connector");
  this->SetConnectorNodeReferenceMRMLAttributeName("connectorNodeRef");

  this->CurrentVirtualFixtureType = vtkMRMLIGTLSessionManagerNode::PLANE;
  this->ConeAngle = 90.0;
  
  this->DirectionVector[0]= 0;
  this->DirectionVector[1]= 0;
  this->DirectionVector[2]= 1;

  this->StartPointVector[0] = 0;
  this->StartPointVector[1] = 0;
  this->StartPointVector[2] = 0;
  
  this->EndPointVector[0] = 0;
  this->EndPointVector[1] = 0;
  this->EndPointVector[2] = 0;

   this->VirtualFixtureVector[0] = 0;
  this->VirtualFixtureVector[1] = 0;
  this->VirtualFixtureVector[2] = 0;

  this->VirtualFixtureOffset = 50.0;



  this->SetMessageNodeReferenceRole("message");
  this->SetMessageNodeReferenceMRMLAttributeName("messageNodeRef");

  this->HideFromEditors = 0;
  this->AddNodeReferenceRole(this->GetConnectorNodeReferenceRole(),
                             this->GetConnectorNodeReferenceMRMLAttributeName());
  this->AddNodeReferenceRole(this->GetMessageNodeReferenceRole(),
                             this->GetMessageNodeReferenceMRMLAttributeName());

  this->StringMessageConverter = NULL;
  this->UID = 0;
  
}

//----------------------------------------------------------------------------
vtkMRMLIGTLSessionManagerNode::~vtkMRMLIGTLSessionManagerNode()
{

	if(this->StringMessageConverter){
		 this->StringMessageConverter->Delete();
	}
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  const char* connectorNodeID = this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole());

  os << indent << "ConnectorNodeID: " <<
    (connectorNodeID ? connectorNodeID : "(none)") << "\n";
}


//----------------------------------------------------------------------------
const char* vtkMRMLIGTLSessionManagerNode::GetConnectorNodeID()
{
  return this->GetConnectorNodeIDInternal();
}

//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::SetAndObserveConnectorNodeID(const char *connectorNodeID)
{
  // ------------------------------------------------------------
  // Set reference node to start observing events

  if (connectorNodeID == 0 && this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole()) == 0)
    {
    // was NULL and still NULL, nothing to do
    return;
    }

  if (connectorNodeID != 0 && this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole()) != 0 &&
      !strcmp(connectorNodeID, this->GetNodeReferenceID(this->GetConnectorNodeReferenceRole())))
    {
    //the same ID, nothing to do
    return;
    }

  vtkMRMLScene * scene = this->GetScene();
  if (!scene)
    {
    return;
    }

  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(scene->GetNodeByID(connectorNodeID));
  if (!cnode) // There is no connector node with the specified MRML ID
    {
    return;
    }

  // use vtkMRMLNode call to set and observe reference
  vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  this->SetAndObserveNodeReferenceID(this->GetConnectorNodeReferenceRole(), connectorNodeID, events);

  this->SetConnectorNodeIDInternal(connectorNodeID);

  // ------------------------------------------------------------
  // Remove outgoing message nodes currently registered.

  while (cnode->GetNumberOfOutgoingMRMLNodes() > 0)
    {
    vtkMRMLNode* node = cnode->GetOutgoingMRMLNode(0);
    const char* attr = node->GetAttribute("IGTLSessionManager.Created");
    if (strcmp(attr, "1") == 0)
      {
      cnode->UnregisterOutgoingMRMLNode(node);
      scene->RemoveNode(node);
      }
    else
      {
      cnode->UnregisterOutgoingMRMLNode(node);
      }
    }

  // Remove incoming message nodes currently registered.
  while (cnode->GetNumberOfIncomingMRMLNodes() > 0)
    {
		vtkMRMLNode* node = cnode->GetIncomingMRMLNode(0);
		const char* attr = node->GetAttribute("IGTLSessionManager.Created");
		if (strcmp(attr, "1") == 0)
		  {
		  cnode->UnregisterIncomingMRMLNode(node);
		  scene->RemoveNode(node);
		  }
		else
		{
		
		  }
    }

  // ------------------------------------------------------------
  // Register message converter

  if (!this->StringMessageConverter)
    {
    this->StringMessageConverter = vtkIGTLToMRMLString::New();
    }
  cnode->RegisterMessageConverter(this->StringMessageConverter);


  // ------------------------------------------------------------
  // Register message converter

  vtkSmartPointer< vtkMRMLAnnotationTextNode > command = vtkSmartPointer< vtkMRMLAnnotationTextNode >::New();
  std:: stringstream ss;
  ss << UID;
  std::string TmpCmd = "CMD_" + ss.str();
  command->SetName(TmpCmd.c_str());
  command->SetTextLabel("IDLE;");
  scene->AddNode(command);
  cnode->RegisterOutgoingMRMLNode(command);
  this->AddAndObserveMessageNodeID(cnode->GetID());
  this->SetCommandStringNodeIDInternal(command->GetID());

  vtkSmartPointer< vtkMRMLLinearTransformNode > rtrans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  rtrans->SetName("T_CT_Base");
  scene->AddNode(rtrans);
  cnode->RegisterOutgoingMRMLNode(rtrans);
  this->AddAndObserveMessageNodeID(cnode->GetID());
  this->SetRegistrationTransformNodeIDInternal(rtrans->GetID());
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::AddAndObserveMessageNodeID(const char *messageNodeID)
{
  int n = this->GetNumberOfNodeReferences(this->GetMessageNodeReferenceRole());
  for (int i = 0; i < n; i ++)
    {
    const char* id = GetNthNodeReferenceID(this->GetMessageNodeReferenceRole(), i);
    if (strcmp(messageNodeID, id) == 0)
      {
      // Alredy on the list. Remove it.
      return;
      }
    }

  vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  this->AddAndObserveNodeReferenceID(this->GetMessageNodeReferenceRole(), messageNodeID, events);

}

//---------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::ProcessMRMLEvents ( vtkObject *caller,
                                                  unsigned long vtkNotUsed(event),
                                                  void *vtkNotUsed(callData) )
{
  // as retrieving the parent transform node can be costly (browse the scene)
  // do some checks here to prevent retrieving the node for nothing.
	if (caller != NULL)
    {
	 /* if(strcmp(caller->GetClassName(),"vtkMRMLAnnotationTextNode")){

			 vtkMRMLScene* scene = this->GetScene();
		  if (!scene) 
			{
			return;
			}
		  
		  vtkMRMLNode* commandnode = scene->GetNodeByID(this->GetCommandStringNodeIDInternal());
		  
		  vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(commandnode);

		  if (!tnode)
			{
			return;
			}
			this->SendCommand(tnode->GetTextLabel());
						 
		}
	   
		
	}else if (caller == NULL ||
      (event != vtkCommand::ModifiedEvent && 
      event != vtkMRMLIGTLSessionManagerNode::TransformModifiedEvent))
    {
    return;*/
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceAdded(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(reference->GetReferencedNodeID());
  if (!node)
    {
    return;
    }

  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceRemoved(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(reference->GetReferencedNodeID());
  if (!node)
    {
    return;
    }

  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::OnNodeReferenceModified(vtkMRMLNodeReference *reference)
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }


  if (strcmp(reference->GetReferenceRole(), this->GetMessageNodeReferenceRole()) == 0)
    {
    }
  else if (strcmp(reference->GetReferenceRole(), this->GetConnectorNodeReferenceRole()) == 0)
    {
    }
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::SendCommand(std::string CommandString)
{

  if (!this->GetCommandStringNodeIDInternal())
    {
    return;
    }

  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetNodeByID(this->GetCommandStringNodeIDInternal());
  
  vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(node);
  vtkMRMLIGTLConnectorNode* cnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(scene->GetNodeByID(this->GetConnectorNodeIDInternal()));

  if(cnode->GetState()!= 2){
	 return;
 }
  if (!tnode)
    {
    return;
    }
  std::stringstream ss;
  this->UID++;
  ss << this->UID;
  std::string TmpCmd = "CMD_" + ss.str();
  std::cout << "Commmand :  "<<CommandString<<std::endl;
  //tnode->DisableModifiedEventOn();
  tnode->SetName(TmpCmd.c_str());
  //tnode->DisableModifiedEventOff();
  tnode->SetTextLabel(CommandString.data());
  if(UID>999999999999){
	  UID =0;
  }
}


void NodeChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);

int vtkMRMLIGTLSessionManagerNode::ObserveAcknowledgeString()
{
 vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return 0;
    }
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(scene->GetNodeByID(this->ConnectorNodeIDInternal));
  if (!cnode) // There is no connector node with the specified MRML ID
    {
    return 0;
    }
  vtkMRMLNode* node = scene->GetFirstNodeByName("ACK");
  
  vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(node);

  if (!tnode)
  {
    return 0;
  }

  CallBack->SetClientData(this);
  CallBack->SetCallback(NodeChanged);
  tnode->AddObserver( vtkMRMLAnnotationTextNode::ValueModifiedEvent, CallBack); 
  return 1;
  //CallBack->Delete();
}

void NodeChanged(vtkObject* vtk_obj, unsigned long vtkNotUsed(event), void* client_data, void* vtkNotUsed(call_data))
{
	vtkMRMLIGTLSessionManagerNode* thisClass = reinterpret_cast<vtkMRMLIGTLSessionManagerNode*>(client_data);
	vtkMRMLAnnotationTextNode* anode = reinterpret_cast<vtkMRMLAnnotationTextNode*>(vtk_obj);

	if(!anode){
		return;
	}
	if(strcmp(anode->GetName(),"ACK")!=0){
			return;
	}

	vtkMRMLScene* scene = thisClass->GetScene();
	if (!scene) 
	{
		return;
	}
	std::string TmpACK = anode->GetTextLabel();
	std::cout << "Acknowledge :  "<<TmpACK <<std::endl;
	thisClass->SetGlobalWarningDisplay(1);
	int firstpos = TmpACK.find_first_of(";",0);
	std::string AckStateString = TmpACK.substr(0 , firstpos);
	 if (!thisClass->GetCommandStringNodeIDInternal())
	{
		return;
	}
	vtkMRMLNode* node = scene->GetNodeByID(thisClass->GetCommandStringNodeIDInternal());
		  
	vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(node);

	if(strcmp(AckStateString.c_str(),"SHUTDOWN")==0){
		 vtkSmartPointer<vtkMRMLModelDisplayNode> ToolDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		  ToolDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("ToolDisplay"));
		  if(ToolDisplay){
			  ToolDisplay->SetColor(0.4,0.4,0.4);
			  for(int i = 0 ; i<8 ; i++){

					if(i==0){
					   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					   modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("BaseDisplay"));
					   if(modelDisplay){
						modelDisplay->SetColor(0.95,0.95,0.95); // set color (0.95,0.83,0.57 = bone
					   }
					   //modelDisplay->Delete();

					}else{
						std::string name = "Link";
						std::stringstream name_ss;
						name_ss <<name << i<<"Display";
						name = name_ss.str();
						
						vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
						modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName(name.c_str()));
						if(modelDisplay){
							modelDisplay->SetColor(0.95,0.95,0.95);
					   }
						//modelDisplay->Delete();
					}
			  }
		  }
		  thisClass->UID = 0;
		 

		  if (!tnode)
			{
			return;
			}
		  std::string TmpCmd = "IDLE;";
		  tnode->SetTextLabel(TmpCmd.data());
		  //ToolDisplay->Delete();
		  
	}else{
		if(strcmp(AckStateString.c_str(), "IDLE")== 0){
			thisClass->VirtFixOff();
			for(int i = 0 ; i<8 ; i++){

				if(i==0){
				   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
				   modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("BaseDisplay"));
				   if(modelDisplay){
						modelDisplay->SetColor(0.18,0.39,0.514) ; // set color (0.95,0.83,0.57 = bone
					}
				   //modelDisplay->Delete();

				}else{
					std::string name = "Link";
					std::stringstream name_ss;
					name_ss <<name << i<<"Display";
					name = name_ss.str();
					
					vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName(name.c_str()));
					if(modelDisplay){
						modelDisplay->SetColor(0.18,0.39,0.514) ; // set color (0.95,0.83,0.57 = bone
					}
					//modelDisplay->Delete();
				}
			}
		}else if(strcmp(AckStateString.c_str(), "GravComp") == 0){
		 thisClass->VirtFixOff();
		  for(int i = 0 ; i<8 ; i++){

				if(i==0){
				   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
				   modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("BaseDisplay"));
				   if(modelDisplay){
						modelDisplay->SetColor(0.501,0.688,0.501);// set color (0.95,0.83,0.57 = bone
					}
				   //modelDisplay->Delete();

				}else{
					std::string name = "Link";
					std::stringstream name_ss;
					name_ss <<name << i<<"Display";
					name = name_ss.str();
					
					vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName(name.c_str()));
					 if(modelDisplay){
						modelDisplay->SetColor(0.501,0.688,0.501);// set color (0.95,0.83,0.57 = bone
					}
					 //modelDisplay->Delete();
				}
			}
		}else if( strcmp(AckStateString.c_str(), "VirtualFixtures")== 0 || strcmp(AckStateString.c_str(), "PathImp")== 0){
		  
		  for(int i = 0 ; i<8 ; i++){

				if(i==0){
				   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
				   modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("BaseDisplay"));
				   if(modelDisplay){
						modelDisplay->SetColor(0.11,0.433,0.333); // set color (0.95,0.83,0.57 = bone
					}
				   //modelDisplay->Delete();
				  

				}else{
					std::string name = "Link";
					std::stringstream name_ss;
					name_ss <<name << i<<"Display";
					name = name_ss.str();
					
					vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName(name.c_str()));
					if(modelDisplay){
						modelDisplay->SetColor(0.11,0.433,0.333); // set color (0.95,0.83,0.57 = bone
					}
					//modelDisplay->Delete();
				  
				}
			}
		  if(strcmp(AckStateString.c_str(), "VirtualFixtures")== 0 ){
					int pos = TmpACK.find_first_of(";",AckStateString.length()+1);
					if(pos<= TmpACK.length()){
					  std::string VFTypeString = TmpACK.substr(AckStateString.length()+1 , pos - (AckStateString.length()+1));
						vtkSmartPointer<vtkMRMLModelDisplayNode> VirtualFixture =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					  if(strcmp(VFTypeString.c_str(), "plane")== 0){
							thisClass->VirtFixOn("planeDisplay");
							thisClass->VirtFixOn("planeEdgeDisplay");
							VirtualFixture = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("planeDisplay"));
					  }else{
							thisClass->VirtFixOn("coneDisplay");
							VirtualFixture = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("coneDisplay"));
					  }
					  pos = TmpACK.find_first_of(";",AckStateString.length()+1+VFTypeString.length()+1);
					  std::string VirtualFixtureRegion = TmpACK.substr(AckStateString.length()+1+VFTypeString.length()+1 , pos - (AckStateString.length()+1+VFTypeString.length()+1));
					  if(strcmp(VirtualFixtureRegion.c_str(), "0")==0 && VirtualFixture){
						VirtualFixture->SetColor(0,0,1);
						VirtualFixture->SetOpacity(0.2) ;
					  }else if(strcmp(VirtualFixtureRegion.c_str(), "1")== 0 && VirtualFixture){
						VirtualFixture->SetColor(1,0,0);
						VirtualFixture->SetOpacity(0.4) ;
					  }else{
						  if( VirtualFixture){
							  VirtualFixture->SetColor(1,0,0);
							  VirtualFixture->SetOpacity(8.0) ;
						  }
					  }
					 //VirtualFixture->Delete();

					}
			  
		  }else if(strcmp(AckStateString.c_str(), "PathImp") == 0){
			  thisClass->VirtFixOn("pathDisplay");
			   vtkSmartPointer<vtkMRMLModelDisplayNode> Path =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
			   Path = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("pathDisplay"));
			   int pos = TmpACK.find_first_of(";",AckStateString.length()+1);
			  std::string VirtualFixtureRegion = TmpACK.substr(AckStateString.length()+1 , pos-1);
			  
			  if(strcmp(VirtualFixtureRegion.c_str(), "0")== 0 && Path){
				Path->SetColor(0,1,0);
			  }else if(strcmp(VirtualFixtureRegion.c_str(), "1")== 0 && Path){
				  Path->SetColor(0.5,0.5,0);
			  }else if(Path){
				  Path->SetColor(1,0,0);
			  }
			  //Path->Delete();
		  }
	  
	  }else if(strcmp(AckStateString.c_str(), "MoveToPose")== 0){
	
		thisClass->VirtFixOff();
			 for(int i = 0 ; i<8 ; i++){

				if(i==0){
				   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
				   modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName("BaseDisplay"));
				   if(modelDisplay){
						modelDisplay->SetColor(0.688,0.201,0.0); // set color (0.95,0.83,0.57 = bone
				   }
				    //modelDisplay->Delete();

				}else{
					std::string name = "Link";
					std::stringstream name_ss;
					name_ss <<name << i<<"Display";
					name = name_ss.str();
					
					vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
					modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(scene->GetFirstNodeByName(name.c_str()));
					if(modelDisplay){
						modelDisplay->SetColor(0.688,0.201,0.0); // set color (0.95,0.83,0.57 = bone
				   }
					//modelDisplay->Delete();
				}
			}
	  }
	  int Lastpos = TmpACK.find_last_of(";", TmpACK.length());
	  int pos=0;
	  for(int i = 1; i<Lastpos; i++){
			pos = TmpACK.find(";", Lastpos-i);
			if(pos != Lastpos){
				i=Lastpos;
			}
	  }
	  std::string AckUIDString = TmpACK.substr(pos +1 , Lastpos-1);

	  long AckUID = atol(AckUIDString.c_str());
	  if(AckUID == thisClass->UID){
		  vtkMRMLScene* scene = thisClass->GetScene();
		  if (!scene) 
			{
			return;
			}

		  vtkMRMLNode* node = scene->GetNodeByID(thisClass->GetCommandStringNodeIDInternal());
		  
		  vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(node);

		  if (!tnode)
			{
			return;
			}
		  thisClass->SendCommand(tnode->GetTextLabel());
	  }
	}
  }
 
  void vtkMRMLIGTLSessionManagerNode::VirtFixOn(std::string name)
{
	vtkSmartPointer<vtkMRMLModelDisplayNode> VirtualFixture =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	VirtualFixture = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName(name.c_str()));

	if(VirtualFixture){
		VirtualFixture->VisibilityOn();
	}

	 //VirtualFixture->Delete();
}
void vtkMRMLIGTLSessionManagerNode::VirtFixOff()
{
	vtkSmartPointer<vtkMRMLModelDisplayNode> Path =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	Path = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("pathDisplay"));

	if(Path){
		Path->VisibilityOff();
	}
	vtkSmartPointer<vtkMRMLModelDisplayNode> plane =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	plane = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("planeDisplay"));

	if(plane){
		plane->VisibilityOff();
	}
	vtkSmartPointer<vtkMRMLModelDisplayNode> planeBorder =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	planeBorder = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("planeEdgeDisplay"));

	if(planeBorder){
		planeBorder->VisibilityOff();
	}
	vtkSmartPointer<vtkMRMLModelDisplayNode> cone =vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	cone = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("coneDisplay"));

	if(cone){
		cone->VisibilityOff();
	}
}


void vtkMRMLIGTLSessionManagerNode::EndPointFiducialModified(vtkObject* vtk_obj, unsigned long vtkNotUsed(event), void* client_data, void* vtkNotUsed(call_data)) // Mittelung der Fiducialdaten
{
	vtkMRMLIGTLSessionManagerNode* thisClass = reinterpret_cast<vtkMRMLIGTLSessionManagerNode*>(client_data);
	if(vtk_obj->GetClassName(),"vtkMRMLAnnotationFiducialNode"){

		vtkMRMLAnnotationFiducialNode* efiducial = reinterpret_cast<vtkMRMLAnnotationFiducialNode*>(vtk_obj);

		if(!efiducial){
			return;
		}

		if(strcmp(efiducial->GetName(),"EndPoint")!=0){
			return;
		}
		//if(thisClass->EndPointActive)
		//{

			efiducial->GetFiducialCoordinates(thisClass->EndPointVector);

			vtkMRMLAnnotationFiducialNode* sfiducial = vtkMRMLAnnotationFiducialNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("StartPoint"));
			if(!sfiducial){
				return;
			}
			
			sfiducial->GetFiducialCoordinates (thisClass->StartPointVector);
		
			thisClass->DirectionVector[0] = thisClass->StartPointVector[0] - thisClass->EndPointVector[0] ;
			thisClass->DirectionVector[1] = thisClass->StartPointVector[1] - thisClass->EndPointVector[1];
			thisClass->DirectionVector[2] = thisClass->StartPointVector[2] - thisClass->EndPointVector[2];

			double nbetrag = sqrt(pow(thisClass->DirectionVector[0],2)+pow(thisClass->DirectionVector[1],2)+pow(thisClass->DirectionVector[2],2));

			thisClass->DirectionVector[0]/=nbetrag;
			thisClass->DirectionVector[1]/=nbetrag;
			thisClass->DirectionVector[2]/=nbetrag;

			thisClass->VirtualFixtureVector[0]= thisClass->StartPointVector[0]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[0];
			thisClass->VirtualFixtureVector[1]= thisClass->StartPointVector[1]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[1];
			thisClass->VirtualFixtureVector[2]= thisClass->StartPointVector[2]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[2];

		//}
			vtkSmartPointer<vtkTransform> TransPath = vtkSmartPointer<vtkTransform>::New();
		TransPath->Identity();

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_EE = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		T_EE = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
		if (!T_EE)
		{
			std::cout << "ERROR:No Transformnode T_EE found! " << std::endl;
			return;
		}

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_CTBase = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		T_CTBase = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_CT_Base")); // Transformnode festlegen
		if (!T_CTBase)
		{
			std::cout << "ERROR:No Transformnode T_CTBase found! " << std::endl;
			return;
		}
		vtkVector3d nvec = vtkVector3d(-thisClass->DirectionVector[0], -thisClass->DirectionVector[1], -thisClass->DirectionVector[2]);
		nvec= nvec.Normalized();
		vtkSmartPointer<vtkMatrix4x4> T_EE_CT  = vtkSmartPointer<vtkMatrix4x4>::New();
		T_EE->GetMatrixTransformToWorld(T_EE_CT);
		//T_EE_CT->Invert();
		vtkVector3d yvec = vtkVector3d(T_EE_CT->GetElement(0,1),T_EE_CT->GetElement(1,1), T_EE_CT->GetElement(2,1));
		//yvec.SetZ(-yvec.GetY()*nvec.GetY() + yvec.GetZ()*nvec.GetZ()/nvec.GetX());
		yvec = yvec.Normalized();
		vtkVector3d xvec = yvec.Cross(nvec);
		xvec = xvec.Normalized();
		yvec = nvec.Cross(xvec);
		yvec= yvec.Normalized();
		
		#if VTK_MAJOR_VERSION <= 5
			vtkMatrix4x4* T_Tmp_mat = T_CT_Base->GetMatrixTransformToParent();
		#else
			vtkSmartPointer<vtkMatrix4x4> T_Tmp_mat = vtkSmartPointer<vtkMatrix4x4>::New();
		#endif

		T_Tmp_mat->SetElement(0,0,xvec.GetX());
		T_Tmp_mat->SetElement(1,0,xvec.GetY());
		T_Tmp_mat->SetElement(2,0,xvec.GetZ());
		T_Tmp_mat->SetElement(0,1,yvec.GetX());
		T_Tmp_mat->SetElement(1,1,yvec.GetY());
		T_Tmp_mat->SetElement(2,1,yvec.GetZ());
		T_Tmp_mat->SetElement(0,2,nvec.GetX());
		T_Tmp_mat->SetElement(1,2,nvec.GetY());
		T_Tmp_mat->SetElement(2,2,nvec.GetZ());


		//double  beta = asin(nvec[0])* 180.0 / PI;
		//double alpha = -atan2(nvec[1],nvec[2])* 180.0 / PI;
		TransPath->SetMatrix(T_Tmp_mat);
		double EulerAngles[3];
		TransPath->GetOrientation(EulerAngles);
		
		/*std::cerr << "alpha "<<alpha<<std::endl<<"; beta "<<beta<<std::endl;
		TransPath->PostMultiply();
		TransPath->RotateX(alpha);
		TransPath->RotateY(beta);
		TransPath->Translate(thisClass->EndPointVector[0],thisClass->EndPointVector[1], thisClass->EndPointVector[2] );
		TransPath->Update();

		std::cerr << "ERROR:Transform Node " 
			<<TransPath->GetMatrix()->GetElement(0,0)<< "; "<<TransPath->GetMatrix()->GetElement(0,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(0,2)<< ";"<<TransPath->GetMatrix()->GetElement(0,3)<< "; "<< std::endl
			<<TransPath->GetMatrix()->GetElement(1,0)<< "; "<<TransPath->GetMatrix()->GetElement(1,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(1,2)<< "; "<<TransPath->GetMatrix()->GetElement(1,3)<< "; "<< std::endl
			<<TransPath->GetMatrix()->GetElement(2,0)<< "; "<<TransPath->GetMatrix()->GetElement(2,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(2,2)<< "; "<<TransPath->GetMatrix()->GetElement(2,3)<< "; "<< std::endl
			<< std::endl;*/
		

		


		

		vtkSmartPointer<vtkMatrix4x4> Tmp= vtkSmartPointer<vtkMatrix4x4>::New();
		vtkSmartPointer<vtkMatrix4x4> TargetOrientation = vtkSmartPointer<vtkMatrix4x4>::New();

		vtkMatrix4x4::Multiply4x4(T_CTBase->GetMatrixTransformFromParent(),TransPath->GetMatrix(), TargetOrientation); 
		
		
		//vtkMatrix4x4::Multiply4x4(Tmp, TransPath->GetMatrix(), TargetOrientation);

		std::cerr << "ERROR:Transform Node "
			<<TargetOrientation->GetElement(0,0)<< "; "<<TargetOrientation->GetElement(0,1)<< "; "
			<<TargetOrientation->GetElement(0,2)<< ";"<<TargetOrientation->GetElement(0,3)<< "; "<< std::endl
			<<TargetOrientation->GetElement(1,0)<< "; "<<TargetOrientation->GetElement(1,1)<< "; "
			<<TargetOrientation->GetElement(1,2)<< "; "<<TargetOrientation->GetElement(1,3)<< "; "<< std::endl
			<<TargetOrientation->GetElement(2,0)<< "; "<<TargetOrientation->GetElement(2,1)<< "; "
			<<TargetOrientation->GetElement(2,2)<< "; "<<TargetOrientation->GetElement(2,3)<< "; "<< std::endl
			<< std::endl;

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_Ori= vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		if(!thisClass->GetScene()->GetFirstNodeByName("T_Ori")){
			T_Ori->SetName("T_Ori");
			T_Ori->SetScene(thisClass->GetScene());
		}else{
			T_Ori = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_Ori"));

		}
		T_Ori->SetMatrixTransformToParent(TargetOrientation);
		thisClass->GetScene()->AddNode(T_Ori);
		thisClass->UpdateVirtualFixturePreview();

	}
}
//-----------------------------------------------------------------------------
void vtkMRMLIGTLSessionManagerNode::StartPointFiducialModified(vtkObject* vtk_obj, unsigned long vtkNotUsed(event), void* client_data, void* vtkNotUsed(call_data)) // Mittelung der Fiducialdaten
{
	vtkMRMLIGTLSessionManagerNode* thisClass = reinterpret_cast<vtkMRMLIGTLSessionManagerNode*>(client_data);
	if(vtk_obj->GetClassName(),"vtkMRMLAnnotationFiducialNode"){
		vtkMRMLAnnotationFiducialNode* fiducial = reinterpret_cast<vtkMRMLAnnotationFiducialNode*>(vtk_obj);

		if(!fiducial){
			return;
		}

		if(strcmp(fiducial->GetName(),"StartPoint")!=0){
			return;
		}
		
		fiducial->GetFiducialCoordinates (thisClass->StartPointVector);

		vtkMRMLAnnotationFiducialNode *e_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("EndPoint"));
		if(!e_fid){
			return;
		}
		e_fid->GetFiducialCoordinates (thisClass->EndPointVector);
		thisClass->DirectionVector[0] = thisClass->StartPointVector[0] - thisClass->EndPointVector[0];
		thisClass->DirectionVector[1] = thisClass->StartPointVector[1] - thisClass->EndPointVector[1];
		thisClass->DirectionVector[2] = thisClass->StartPointVector[2] - thisClass->EndPointVector[2];

		double nbetrag = sqrt(pow(thisClass->DirectionVector[0],2)+pow(thisClass->DirectionVector[1],2)+pow(thisClass->DirectionVector[2],2));

		thisClass->DirectionVector[0]/=nbetrag;
		thisClass->DirectionVector[1]/=nbetrag;
		thisClass->DirectionVector[2]/=nbetrag;

		thisClass->VirtualFixtureVector[0]= thisClass->StartPointVector[0]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[0];
		thisClass->VirtualFixtureVector[1]= thisClass->StartPointVector[1]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[1];
		thisClass->VirtualFixtureVector[2]= thisClass->StartPointVector[2]+ thisClass->VirtualFixtureOffset*thisClass->DirectionVector[2];

		vtkSmartPointer<vtkTransform> TransPath = vtkSmartPointer<vtkTransform>::New();
		TransPath->Identity();

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_EE = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		T_EE = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
		if (!T_EE)
		{
			std::cout << "ERROR:No Transformnode T_EE found! " << std::endl;
			return;
		}

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_CTBase = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		T_CTBase = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_CT_Base")); // Transformnode festlegen
		if (!T_CTBase)
		{
			std::cout << "ERROR:No Transformnode T_CTBase found! " << std::endl;
			return;
		}
		vtkVector3d nvec = vtkVector3d(-thisClass->DirectionVector[0], -thisClass->DirectionVector[1], -thisClass->DirectionVector[2]);
		nvec= nvec.Normalized();
		vtkSmartPointer<vtkMatrix4x4> T_EE_CT  = vtkSmartPointer<vtkMatrix4x4>::New();
		T_EE->GetMatrixTransformToWorld(T_EE_CT);
		//T_EE_CT->Invert();
		vtkVector3d yvec = vtkVector3d(T_EE_CT->GetElement(0,1),T_EE_CT->GetElement(1,1), T_EE_CT->GetElement(2,1));
		//yvec.SetZ(-yvec.GetY()*nvec.GetY() + yvec.GetZ()*nvec.GetZ()/nvec.GetX());
		yvec = yvec.Normalized();
		vtkVector3d xvec = yvec.Cross(nvec);
		xvec = xvec.Normalized();
		yvec = nvec.Cross(xvec);
		yvec= yvec.Normalized();
		
		#if VTK_MAJOR_VERSION <= 5
			vtkMatrix4x4* T_Tmp_mat = T_CT_Base->GetMatrixTransformToParent();
		#else
			vtkSmartPointer<vtkMatrix4x4> T_Tmp_mat = vtkSmartPointer<vtkMatrix4x4>::New();
		#endif

		T_Tmp_mat->SetElement(0,0,xvec.GetX());
		T_Tmp_mat->SetElement(1,0,xvec.GetY());
		T_Tmp_mat->SetElement(2,0,xvec.GetZ());
		T_Tmp_mat->SetElement(0,1,yvec.GetX());
		T_Tmp_mat->SetElement(1,1,yvec.GetY());
		T_Tmp_mat->SetElement(2,1,yvec.GetZ());
		T_Tmp_mat->SetElement(0,2,nvec.GetX());
		T_Tmp_mat->SetElement(1,2,nvec.GetY());
		T_Tmp_mat->SetElement(2,2,nvec.GetZ());


		//double  beta = asin(nvec[0])* 180.0 / PI;
		//double alpha = -atan2(nvec[1],nvec[2])* 180.0 / PI;
		TransPath->SetMatrix(T_Tmp_mat);
		double EulerAngles[3];
		TransPath->GetOrientation(EulerAngles);
		
		/*std::cerr << "alpha "<<alpha<<std::endl<<"; beta "<<beta<<std::endl;
		TransPath->PostMultiply();
		TransPath->RotateX(alpha);
		TransPath->RotateY(beta);
		TransPath->Translate(thisClass->EndPointVector[0],thisClass->EndPointVector[1], thisClass->EndPointVector[2] );
		TransPath->Update();

		std::cerr << "ERROR:Transform Node " 
			<<TransPath->GetMatrix()->GetElement(0,0)<< "; "<<TransPath->GetMatrix()->GetElement(0,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(0,2)<< ";"<<TransPath->GetMatrix()->GetElement(0,3)<< "; "<< std::endl
			<<TransPath->GetMatrix()->GetElement(1,0)<< "; "<<TransPath->GetMatrix()->GetElement(1,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(1,2)<< "; "<<TransPath->GetMatrix()->GetElement(1,3)<< "; "<< std::endl
			<<TransPath->GetMatrix()->GetElement(2,0)<< "; "<<TransPath->GetMatrix()->GetElement(2,1)<< "; "
			<<TransPath->GetMatrix()->GetElement(2,2)<< "; "<<TransPath->GetMatrix()->GetElement(2,3)<< "; "<< std::endl
			<< std::endl;*/
		

		


		

		vtkSmartPointer<vtkMatrix4x4> Tmp= vtkSmartPointer<vtkMatrix4x4>::New();
		vtkSmartPointer<vtkMatrix4x4> TargetOrientation = vtkSmartPointer<vtkMatrix4x4>::New();

		vtkMatrix4x4::Multiply4x4(T_CTBase->GetMatrixTransformFromParent(),TransPath->GetMatrix(), TargetOrientation); 
		
		
		//vtkMatrix4x4::Multiply4x4(Tmp, TransPath->GetMatrix(), TargetOrientation);

		std::cerr << "ERROR:Transform Node "
			<<TargetOrientation->GetElement(0,0)<< "; "<<TargetOrientation->GetElement(0,1)<< "; "
			<<TargetOrientation->GetElement(0,2)<< ";"<<TargetOrientation->GetElement(0,3)<< "; "<< std::endl
			<<TargetOrientation->GetElement(1,0)<< "; "<<TargetOrientation->GetElement(1,1)<< "; "
			<<TargetOrientation->GetElement(1,2)<< "; "<<TargetOrientation->GetElement(1,3)<< "; "<< std::endl
			<<TargetOrientation->GetElement(2,0)<< "; "<<TargetOrientation->GetElement(2,1)<< "; "
			<<TargetOrientation->GetElement(2,2)<< "; "<<TargetOrientation->GetElement(2,3)<< "; "<< std::endl
			<< std::endl;

		vtkSmartPointer<vtkMRMLLinearTransformNode> T_Ori= vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		if(!thisClass->GetScene()->GetFirstNodeByName("T_Ori")){
			T_Ori->SetName("T_Ori");
			T_Ori->SetScene(thisClass->GetScene());
		}else{
			T_Ori = vtkMRMLLinearTransformNode::SafeDownCast(thisClass->GetScene()->GetFirstNodeByName("T_Ori"));

		}
		T_Ori->SetMatrixTransformToParent(TargetOrientation);
		thisClass->GetScene()->AddNode(T_Ori);
		thisClass->UpdateVirtualFixturePreview();
	}
}


void vtkMRMLIGTLSessionManagerNode::UpdateVirtualFixturePreview(){
	
	vtkSmartPointer<vtkConeSource> cone=vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkRegularPolygonSource> plane=vtkSmartPointer<vtkRegularPolygonSource>::New();
	vtkSmartPointer<vtkCylinderSource> path =vtkSmartPointer<vtkCylinderSource>::New();
	vtkSmartPointer<vtkAlgorithmOutput> VFPolyData=vtkSmartPointer<vtkAlgorithmOutput>::New();
	vtkSmartPointer< vtkMRMLLinearTransformNode > trans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
	if(this->GetScene()->GetFirstNodeByName("T_VF"))
		{
			trans = vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("T_VF")); 
	}else{
		trans->SetName("T_VF");
		trans->SetScene(this->GetScene());
		this->GetScene()->AddNode(trans);
	}
	if(this->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::CONE)
	{
			
		double height = 200; 
		double radius = height*tan(ConeAngle*PI/360); 
		double nbetrag = sqrt(pow(DirectionVector[0],2)+pow(DirectionVector[1],2)+pow(DirectionVector[2],2));
		double cx = height* DirectionVector[0]/(2*nbetrag) + this->VirtualFixtureVector[0];
		double cy = height* DirectionVector[1]/(2*nbetrag) + this->VirtualFixtureVector[1];
		double cz = height* DirectionVector[2]/(2*nbetrag) + this->VirtualFixtureVector[2];

		cone->SetDirection(-1*DirectionVector[0],-1*DirectionVector[1],-1*DirectionVector[2]);
		cone->SetRadius(radius);
		cone->SetHeight(height);
		cone->SetResolution(50); 
		cone->SetCenter(cx,cy,cz); 
		
		VFPolyData = cone->GetOutputPort();		
	}
	else if(this->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::PLANE)
	{
	
		vtkSmartPointer<vtkRegularPolygonSource> planeb=vtkSmartPointer<vtkRegularPolygonSource>::New();	
		
		float size = 500;//  --> Hier Gr\F6\DFe der Fl\E4che anpassen
		planeb->SetNormal(DirectionVector[0],DirectionVector[1],DirectionVector[2]);
		planeb->SetCenter(this->VirtualFixtureVector[0],this->VirtualFixtureVector[1],this->VirtualFixtureVector[2]);
		planeb->SetNumberOfSides(4);
		planeb->SetRadius(size); //
	    
		plane->SetNormal(-1*DirectionVector[0],-1*DirectionVector[1],-1*DirectionVector[2]);
		plane->SetCenter(this->VirtualFixtureVector[0],this->VirtualFixtureVector[1],this->VirtualFixtureVector[2]);
		plane->SetNumberOfSides(4);
		plane->SetRadius(size);

		VFPolyData = plane->GetOutputPort();
		
	}else{
		
	
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->Identity();
		vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
		if (!tnode)
		{
			std::cout << "ERROR:No Transformnode T_EE found! " << std::endl;
			return;
		}
	

		double dPosition[3] = {0.0, 0.0, 0.0};
#if VTK_MAJOR_VERSION <= 5
		vtkMatrix4x4* transformMatrix = tnode->GetMatrixTransformToParent();
		dPosition[0] = transformMatrix->GetElement(0,3);
		dPosition[1] = transformMatrix->GetElement(1,3);
		dPosition[2] = transformMatrix->GetElement(2,3);
#else
		vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		tnode->GetMatrixTransformToWorld(transformMatrix.GetPointer());
		dPosition[0] = transformMatrix->GetElement(0,3);
		dPosition[1] = transformMatrix->GetElement(1,3);
		dPosition[2] = transformMatrix->GetElement(2,3);
#endif

		vtkVector3d CurrentPosition = vtkVector3d(dPosition);
		vtkVector3d TargetPosition = vtkVector3d(this->VirtualFixtureVector[0],this->VirtualFixtureVector[1],this->VirtualFixtureVector[2]);
	
		vtkVector3d uvec = TargetPosition - CurrentPosition;
		vtkVector3d nvec = uvec.Normalized();
		double  beta = -asin(nvec[0])* 180.0 / PI;
		double alpha = atan2(nvec[2],nvec[1])* 180.0 / PI;
		transform->RotateX(alpha);
		transform->RotateZ(beta);
	
		vtkSmartPointer<vtkMatrix4x4> mat = transform->GetMatrix();
		mat = vtkMatrix4x4::SafeDownCast(mat);
		mat->SetElement(0,3,this->VirtualFixtureVector[0]);
		mat->SetElement(1,3,this->VirtualFixtureVector[1]);
		mat->SetElement(2,3,this->VirtualFixtureVector[2] );

		trans->ApplyTransformMatrix(mat);
		
		
		double height =uvec.Norm();  
		double radius = 10;  

		path->SetHeight(height);
		path->SetRadius(radius);
		//Connect to transform...
		path->SetCenter(0, 0 - height/2, 0);
		path->SetResolution(50); // Aufl\F6sung des Kegels

		VFPolyData = path->GetOutputPort();

	}
	// Modelle finden oder erstellen
	if (vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("VF_Temp")))
	{
		vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("VF_Temp"));
		vtkMRMLModelDisplayNode *modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("VF_DisplayTemp"));
		
		model->Reset(0);
		//modelDisplay->Reset(0);

		modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SliceIntersectionVisibilityOn();
		modelDisplay->SetOpacity(0.2) ;
		model->SetName("VF_Temp");
		model->SetScene(this->GetScene());
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		//if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
		//{
			//vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			//T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			//trans->SetAndObserveTransformNodeID( T_CT_Base->GetID());
		if(!(this->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::PATH)){
				trans->SetMatrixTransformToParent(vtkMatrix4x4::New());
			}
			model->SetAndObserveTransformNodeID(trans->GetID());
		//}
		//else{ 
		//	std::cout<<"No Transformation T_CT_Base found!"<<std::endl;
		//	return;
		//}
	
		model->SetPolyDataConnection(VFPolyData);
	}
	else
	{
		vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		model->SetScene(this->GetScene());
		modelDisplay->SetScene(this->GetScene());
		modelDisplay->SetName("VF_DisplayTemp");
		model->SetName("VF_Temp");
		

		modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SetOpacity(0.2) ;
		modelDisplay->SliceIntersectionVisibilityOn();
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());

		
		if(this->GetScene()->GetFirstNodeByName("T_CT_Base"))
		{
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetFirstNodeByName("T_CT_Base"));
			model->SetAndObserveTransformNodeID( T_CT_Base->GetID() );
		}
		else 
			std::cout<<"No Transformation T_CT_Base found!"<<std::endl;	

		model->SetPolyDataConnection(VFPolyData);
	   
		this->GetScene()->AddNode(model);
		this->GetScene()->AddNode(modelDisplay);
	}
	
	

}
