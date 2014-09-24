/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

Portions (c) Copyright 2014 Sebastian Tauscher, Institute of Mechatronic Systems, Leibniz Universitaet Hannover All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer

=========================================================================auto=*/

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLIGTLSessionManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLAnnotationTextNode.h"
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include "vtkIGTLToMRMLString.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkCollection.h>
#include <vtkCollectionIterator.h>

#include <vtkCallbackCommand.h>
#include <vtkIntArray.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

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
  command->Delete();

  vtkSmartPointer< vtkMRMLLinearTransformNode > rtrans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
  rtrans->SetName("T_CT_Base");
  scene->AddNode(rtrans);
  cnode->RegisterOutgoingMRMLNode(rtrans);
  this->AddAndObserveMessageNodeID(cnode->GetID());
  this->SetRegistrationTransformNodeIDInternal(rtrans->GetID());
  rtrans->Delete();
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
                                                  unsigned long event, 
                                                  void *vtkNotUsed(callData) )
{
  // as retrieving the parent transform node can be costly (browse the scene)
  // do some checks here to prevent retrieving the node for nothing.
	if (caller != NULL)
    {
	  if(strcmp(caller->GetClassName(),"vtkMRMLAnnotationTextNode")){

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
    return;
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

  if (!tnode)
    {
    return;
    }
  std::stringstream ss;
  this->UID++;
  ss << this->UID;
  std::string TmpCmd = "CMD_" + ss.str();
  std::cout << "Commmand :  "<<CommandString<<std::endl;
  tnode->SetName(TmpCmd.c_str());
  tnode->SetTextLabel(CommandString.data());
  if(UID>999999999999){
	  UID =0;
  }
}

vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
void NodeChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);

void vtkMRMLIGTLSessionManagerNode::ObserveAcknowledgeString()
{
  vtkMRMLScene* scene = this->GetScene();
  if (!scene) 
    {
    return;
    }

  vtkMRMLNode* node = scene->GetFirstNodeByName("ACK");;
  
  vtkMRMLAnnotationTextNode* tnode = vtkMRMLAnnotationTextNode::SafeDownCast(node);

  if (!tnode)
  {
    return;
  }
  CallBack->SetClientData(this);
  CallBack->SetCallback(NodeChanged);
  tnode->AddObserver( vtkMRMLAnnotationTextNode::ValueModifiedEvent, CallBack);  
}

void NodeChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data)
{
	vtkMRMLIGTLSessionManagerNode* thisClass = reinterpret_cast<vtkMRMLIGTLSessionManagerNode*>(client_data);
	vtkMRMLAnnotationTextNode* anode = reinterpret_cast<vtkMRMLAnnotationTextNode*>(vtk_obj);
	if (!thisClass->GetCommandStringNodeIDInternal())
    {
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