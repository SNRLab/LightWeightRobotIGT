
/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/
/*==============================================================================

  Program: LightWeightRobotIGT

  Portions (c) Copyright 2014 Sebastian Tauscher, Institute of Mechatronic Systems, Leibniz Universitaet Hannover All Rights Reserved.
==============================================================================*/
#define PI 3.14159265358


//c++ includes
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Qt Thread class (for msleep())
#include "vtksys/SystemTools.hxx"

// FooBar Widgets includes
#include "qSlicerLightWeightRobotIGTFooBarWidget.h"
#include "ui_qSlicerLightWeightRobotIGTFooBarWidget.h"
// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerCoreApplication.h"
#include <qSlicerAbstractCoreModule.h>
#include "qSlicerLayoutManager.h"
#include "qSlicerModuleManager.h"
#include "qSlicerModuleSelectorToolBar.h"
//#include "ui_qSlicerRALOModuleWidget.h"

// MRML includes
#include <vtkMRMLIGTLSessionManagerNode.h>
#include <vtkMRMLIGTLConnectorNode.h>
#include <vtkMRMLAnnotationTextNode.h>
#include <vtkMRMLAnnotationFiducialNode.h>
#include <vtkMRMLAnnotationHierarchyNode.h>
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkSlicerModelsLogic.h>
#include <vtkMRMLCameraNode.h> //TF
#include <vtkMRMLSliceNode.h>


// VTK includes
#include <vtkMRMLNode.h>
#include <vtkMRMLModelStorageNode.h> 
#include <vtkMRMLFreeSurferModelStorageNode.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCallbackCommand.h>
#include <vtkClipPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinder.h>
#include <vtkCylinderSource.h>
#include <vtkFeatureEdges.h>
#include <vtkImplicitBoolean.h>
#include <vtkLineSource.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>
#include <vtkRegularPolygonSource.h>//TF
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSlicerAnnotationModuleLogic.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSTLReader.h>
#include <vtkStripper.h>
#include <vtkTransform.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LightWeightRobotIGT
class qSlicerLightWeightRobotIGTFooBarWidgetPrivate
  : public Ui_qSlicerLightWeightRobotIGTFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerLightWeightRobotIGTFooBarWidget);
protected:
  qSlicerLightWeightRobotIGTFooBarWidget* const q_ptr;

public:
  qSlicerLightWeightRobotIGTFooBarWidgetPrivate(
    qSlicerLightWeightRobotIGTFooBarWidget& object);
  virtual void setupUi(qSlicerLightWeightRobotIGTFooBarWidget*);
  vtkSlicerAnnotationModuleLogic* annotationLogic;
  bool VisualActive;
};

// --------------------------------------------------------------------------
qSlicerLightWeightRobotIGTFooBarWidgetPrivate
::qSlicerLightWeightRobotIGTFooBarWidgetPrivate(
  qSlicerLightWeightRobotIGTFooBarWidget& object)
  : q_ptr(&object)
{
	
}

// --------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidgetPrivate
::setupUi(qSlicerLightWeightRobotIGTFooBarWidget* widget)
{
  this->Ui_qSlicerLightWeightRobotIGTFooBarWidget::setupUi(widget);
  
}

//-----------------------------------------------------------------------------
// qSlicerLightWeightRobotIGTFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTFooBarWidget
::qSlicerLightWeightRobotIGTFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLightWeightRobotIGTFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  d->setupUi(this);
  qSlicerAbstractCoreModule* annotationModule = qSlicerCoreApplication::application()->moduleManager()->module("Annotations");
  if (annotationModule)
  {
	d->annotationLogic  = vtkSlicerAnnotationModuleLogic::SafeDownCast(annotationModule->logic());
  }
 // this->ModulesMenu = qSlicerModulesMenu::(QObject::tr("Modules"), parentWidget);
  d->LineEditRobotPath->setText( "C:\\Users\\OptTrack_user\\Documents\\GitHub\\ModellIiwa\\" );

  VisualOptions.COFType = "rob";
  
  this->EndPointActive = false;
  this->StartPointActive = false;
  this->HomePointActive = false;

  MPOptions.A = "180";
  MPOptions.B = "0";
  MPOptions.C = "180";

  VFOptions.VFType = "plane";
  VFOptions.Offset = 15.0;
  VFOptions.phi = "120";


  d->lineEdit_VFphi->setEnabled(false);
  d->VisualActive = false;
  d->GravComp->setEnabled(false);
  d->IDLE->setEnabled(false);
  d->MoveManual->setEnabled(false);
  d->ShutDown->setEnabled(false);
  d->LoadRobot->setEnabled(false);
  d->GetFiducial->setEnabled(false);
  d->MoveToTargetPoint->setEnabled(false);
  d->BackToStart->setEnabled(false);
  d->LeadtoStart->setEnabled(false);
  d->MoveToEntrancePoint->setEnabled(false);
}

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTFooBarWidget
::~qSlicerLightWeightRobotIGTFooBarWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
setSessionManagerNode(vtkMRMLNode *node)
{
  
 
}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
setMRMLScene(vtkMRMLScene *newScene)
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  Superclass::setMRMLScene(newScene);

  d->SessionManagerNodeSelector->setMRMLScene(newScene);
  
 vtkSmartPointer< vtkMRMLIGTLSessionManagerNode> snode = vtkSmartPointer< vtkMRMLIGTLSessionManagerNode>::New() ;
  if (!snode)
    {
    return;
    }
  if (!this->mrmlScene())
    {
    return;
    }
 
  this->mrmlScene()->AddNode(snode);
  snode->SetScene(this->mrmlScene());
  d->SessionManagerNodeSelector->setCurrentNode(snode->GetID());
  if (!snode->GetConnectorNodeID())
    {
    vtkSmartPointer< vtkMRMLIGTLConnectorNode > cnode = vtkSmartPointer< vtkMRMLIGTLConnectorNode >::New();
	cnode->SetTypeClient("172.31.1.147", 49001);
	cnode->SetName("StateControlConnectorNode");
	cnode->SetScene(this->mrmlScene());
    this->mrmlScene()->AddNode(cnode);
    snode->SetAndObserveConnectorNodeID(cnode->GetID());
	//cnode->Delete();
    }
   vtkSmartPointer< vtkMRMLIGTLConnectorNode >  Visualcnode = vtkSmartPointer< vtkMRMLIGTLConnectorNode >::New();
   Visualcnode->SetTypeClient("172.31.1.147", 49002);
   Visualcnode->SetName("VisualizationConnectorNode");
   Visualcnode->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(Visualcnode);
   //Visualcnode->Delete();
   //snode->Delete();
	
  
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
onClickGravComp()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "GravComp;";
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
    snode->SendCommand(CommandString);
    }
  d->BackToStart->setEnabled(false);
  d->MoveToEntrancePoint->setEnabled(false);
  d->MoveToTargetPoint->setEnabled(false);

}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
onClickLeadtoStart()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
 if(!T_CT_Base){
	 return;
 }
   vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if(!snode){
	  return;
  }

vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_BaseOrientation=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
T_CT_BaseOrientation->SetMatrixTransformToParent(T_CT_Base->GetMatrixTransformToParent());;
double  VirtualFixturesPosition[4] = {snode->VirtualFixtureVector[0], snode->VirtualFixtureVector[1], snode->VirtualFixtureVector[2], 1.0};
 double  VirtualFixturesNormal[4] = {snode->DirectionVector[0],snode->DirectionVector[1], snode->DirectionVector[2], 1.0};
 double  *VirtualFixturesPositionBaseFrame;
VirtualFixturesPositionBaseFrame = new double[4];
 double *VirtualFixturesNormalBaseFrame;
 VirtualFixturesNormalBaseFrame = new double[4];

 #if VTK_MAJOR_VERSION <= 5
			vtkMatrix4x4* T_CT_Base_mat = T_CT_Base->GetMatrixTransformToParent();
#else
			vtkSmartPointer<vtkMatrix4x4> T_CT_Base_mat = vtkSmartPointer<vtkMatrix4x4>::New();
			T_CT_Base_mat = T_CT_Base->GetMatrixTransformToParent();
#endif
T_CT_Base_mat->SetElement(0,3,0);
T_CT_Base_mat->SetElement(1,3,0);
T_CT_Base_mat->SetElement(2,3,0);
T_CT_BaseOrientation->SetMatrixTransformToParent(T_CT_Base_mat);
VirtualFixturesPositionBaseFrame = T_CT_Base->GetTransformFromParent()->TransformPoint(VirtualFixturesPosition);
 VirtualFixturesNormalBaseFrame = T_CT_BaseOrientation->GetTransformFromParent()->TransformPoint(VirtualFixturesNormal);
  
  if( snode->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::PLANE){
	  CommandString = "VirtualFixtures;" + VisualOptions.COFType+";plane;" + QString::number(VirtualFixturesPositionBaseFrame[0],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[1],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[2],'f', 8).toStdString() + ";" + QString::number(VirtualFixturesNormalBaseFrame[0],'f', 8).toStdString() + ";" + QString::number(VirtualFixturesNormalBaseFrame[1],'f', 8).toStdString() +";" + QString::number(VirtualFixturesNormalBaseFrame[2],'f', 8).toStdString()+";";



  }
  if(snode->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::CONE )
  {
	CommandString = "VirtualFixtures;" + VisualOptions.COFType+";cone;" + QString::number(VirtualFixturesPositionBaseFrame[0],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[1],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[2],'f', 8).toStdString() + ";" + QString::number(VirtualFixturesNormalBaseFrame[0],'f', 8).toStdString() + ";" + QString::number(VirtualFixturesNormalBaseFrame[1],'f', 8).toStdString() +";" + QString::number(VirtualFixturesNormalBaseFrame[2],'f', 8).toStdString()+";";

  }
  if(snode->CurrentVirtualFixtureType == vtkMRMLIGTLSessionManagerNode::PATH)
  {
	  CommandString = "PathImp;"+VisualOptions.COFType+";" + QString::number(VirtualFixturesPositionBaseFrame[0],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[1],'f', 8).toStdString() + ";" +  QString::number(VirtualFixturesPositionBaseFrame[2],'f', 8).toStdString()+ ";";
	
  }
  //---
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }


  if (snode)
    {
    snode->SendCommand(CommandString);
    }
	
	d->MoveToEntrancePoint->setEnabled(false);  //MoveToPose Button aktivieren
	d->MoveToTargetPoint->setEnabled(false);
	d->BackToStart->setEnabled(false);
	this->HomePointActive =true;
	this->StartPointActive = false;
	this->EndPointActive = false;
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
onClickIDLE()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "IDLE;";
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
    snode->SendCommand(CommandString);
    }
  d->GravComp->setEnabled(true);
  d->MoveManual->setEnabled(true);
  if(vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")) && vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"))) {
	  if(this->EndPointActive){
		  d->MoveToEntrancePoint->setEnabled(true);
		  d->BackToStart->setEnabled(false);
		  d->LeadtoStart->setEnabled(false);
		  d->MoveToTargetPoint->setEnabled(true);
	  }else if(this->StartPointActive){
		  d->LeadtoStart->setEnabled(true);
		  d->BackToStart->setEnabled(true);
		  d->MoveToTargetPoint->setEnabled(true);
		  d->MoveToEntrancePoint->setEnabled(true);
	  }else if(this->HomePointActive){
		  d->LeadtoStart->setEnabled(true);
		  d->BackToStart->setEnabled(true);
		  d->MoveToTargetPoint->setEnabled(false);
		  d->MoveToEntrancePoint->setEnabled(true);
	  }
  }

}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget:: VisualButton()
{
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	
	

	if (!d->VisualActive)
	{
		onClickStartVisual();
		d->StartVisual->setText("Stop Visualization");
		d->VisualActive = true;
	}
	else
	{
		d->StartVisual->setText("Start Visualization");
		onClickStopVisual();
		d->VisualActive = false;
	}

}

void qSlicerLightWeightRobotIGTFooBarWidget:: onClickStartVisual()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "Visual;true;"+ VisualOptions.COFType +";";
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
    snode->SendCommand(CommandString);
    }


}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget:: onClickStopVisual()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "Visual;false;" + VisualOptions.COFType +";";
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
    snode->SendCommand(CommandString);
    }
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickShutdown(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "Shutdown;";
  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
    snode->SendCommand(CommandString);
    }
  d->GravComp->setEnabled(false);
  d->IDLE->setEnabled(false);
  d->MoveManual->setEnabled(false);
  d->ShutDown->setEnabled(false);
  d->LoadRobot->setEnabled(false);
  d->GetFiducial->setEnabled(false);
  d->MoveToTargetPoint->setEnabled(false);
  d->BackToStart->setEnabled(false);
  d->LeadtoStart->setEnabled(false);
  d->MoveToEntrancePoint->setEnabled(false);
  this->HomePointActive=false;
	this->StartPointActive=false;
	this->EndPointActive=false;

	vtkMRMLIGTLConnectorNode* cnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetNodeByID(snode->ConnectorNodeIDInternal));
	if(!cnode){
		return;
	}
	snode->UID = 1;
	cnode->Stop();
	vtkMRMLIGTLConnectorNode* Visualcnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VisualizationConnectorNode"));
	if(!Visualcnode){
		return;
	}
	Visualcnode->Stop();
	
	
	
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickMoveToEntrancePoint(){
Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
 if(!T_CT_Base){
	 return;
 }

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_Ori=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_Ori= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_Ori"));
 if(!T_Ori){
	 return;
 }
   vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if(!snode){
	  return;
  }

vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_BaseOrientation=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
T_CT_BaseOrientation->SetMatrixTransformToParent(T_CT_Base->GetMatrixTransformToParent());;
double  StartPosition[4] = {snode->StartPointVector[0], snode->StartPointVector[1], snode->StartPointVector[2], 1.0};
 double  *StartPositionBaseFrame;

StartPositionBaseFrame = T_CT_Base->GetTransformFromParent()->TransformPoint(StartPosition);
 
CommandString = "MoveToPose;" + VisualOptions.COFType + ";" +  
QString::number(StartPositionBaseFrame[0],'f', 8).toStdString() +";" + 
QString::number(StartPositionBaseFrame[1],'f', 8).toStdString()+ ";" + 
QString::number(StartPositionBaseFrame[2],'f', 8).toStdString() +";" + 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,0),'f', 8).toStdString() +";"+ 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,2),'f', 8).toStdString() +";";




  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  if (snode)
    {
		snode->SendCommand(CommandString);
    }
	d->MoveToTargetPoint->setEnabled(true); 
	d->BackToStart->setEnabled(true);
	d->LeadtoStart->setEnabled(false);
	d->GravComp->setEnabled(false);
	d->MoveManual->setEnabled(false);
	this->EndPointActive=false;
	this->StartPointActive= true;
	this->HomePointActive =false;

}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickMoveToTargetPoint(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
 if(!T_CT_Base){
	 return;
 }
   vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if(!snode){
	  return;
  }

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_Ori=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_Ori= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_Ori"));
 if(!T_Ori){
	 return;
 }

vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_BaseOrientation=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
T_CT_BaseOrientation->SetMatrixTransformToParent(T_CT_Base->GetMatrixTransformToParent());;
double  EndPosition[4] = {snode->EndPointVector[0], snode->EndPointVector[1], snode->EndPointVector[2], 1.0};
 double  *EndPositionBaseFrame;

EndPositionBaseFrame = T_CT_Base->GetTransformFromParent()->TransformPoint(EndPosition);
 
CommandString = "MoveToPose;" + VisualOptions.COFType + ";" +  
QString::number(EndPositionBaseFrame[0],'f', 8).toStdString() +";" + 
QString::number(EndPositionBaseFrame[1],'f', 8).toStdString()+ ";" + 
QString::number(EndPositionBaseFrame[2],'f', 8).toStdString() +";" + 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,0),'f', 8).toStdString() +";"+ 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,2),'f', 8).toStdString() +";";

  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  if (snode)
    {
		snode->SendCommand(CommandString);
    }
  d->BackToStart->setEnabled(false);
  d->LeadtoStart->setEnabled(false);
  d->GravComp->setEnabled(false);
  d->MoveManual->setEnabled(false);
  this->EndPointActive=true;
  this->StartPointActive=false;
  this->HomePointActive=false;

	
}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickBackToStart(){
Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
 if(!T_CT_Base){
	 return;
 }

 vtkSmartPointer<vtkMRMLLinearTransformNode> T_Ori=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
 T_Ori= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_Ori"));
 if(!T_Ori){
	 return;
 }
   vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if(!snode){
	  return;
  }

vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_BaseOrientation=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
T_CT_BaseOrientation->SetMatrixTransformToParent(T_CT_Base->GetMatrixTransformToParent());;

double  StartPosition[4] = {snode->VirtualFixtureVector[0], snode->VirtualFixtureVector[1], snode->VirtualFixtureVector[2], 1.0};
 double  *StartPositionBaseFrame;

StartPositionBaseFrame = T_CT_Base->GetTransformFromParent()->TransformPoint(StartPosition);
 
CommandString = "MoveToPose;" + VisualOptions.COFType + ";" +  
QString::number(StartPositionBaseFrame[0],'f', 8).toStdString() +";" + 
QString::number(StartPositionBaseFrame[1],'f', 8).toStdString()+ ";" + 
QString::number(StartPositionBaseFrame[2],'f', 8).toStdString() +";" + 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,0),'f', 8).toStdString() +";"+ 
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(0,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(1,2),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,0),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,1),'f', 8).toStdString() +";"+
QString::number(T_Ori->GetMatrixTransformFromParent()->GetElement(2,2),'f', 8).toStdString() +";";




  if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  if (snode)
    {
		snode->SendCommand(CommandString);
    }
  d->MoveToTargetPoint->setEnabled(false);
  d->MoveToEntrancePoint->setEnabled(true);
  d->LeadtoStart->setEnabled(false);
  d->GravComp->setEnabled(false);
  d->MoveManual->setEnabled(false);
  this->HomePointActive=true;
  this->StartPointActive =false;
  this->EndPointActive=false;

}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickSetEndPoint(){
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
	vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);

	


	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	//if(EndPointActive)
	//{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(snode);
	   CallBack->SetCallback(snode->EndPointFiducialModified);  

		if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
		{
			double SliceOffset[3];

			for(int i = 0; i<3;i++){
			  if (vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (i, "vtkMRMLSliceNode"))) //target auslesen
				{
				
				vtkMRMLSliceNode *slice = vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (i, "vtkMRMLSliceNode"));
					if(strcmp(slice->GetName(),"Red")==0) {
						SliceOffset[2]= slice->GetSliceOffset();
					}else if(strcmp(slice->GetName(),"Yellow")==0){
						SliceOffset[0]= slice->GetSliceOffset();
					}else if(strcmp(slice->GetName(),"Green")== 0){
						SliceOffset[1]= slice->GetSliceOffset();
					}

						
				}
			}
			SliceOffset[3] = SliceOffset[3]+10;
			vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
			int b = fiducial->SetFiducialCoordinates(SliceOffset);
			fiducial->SetName("EndPoint");
			

			// Neue Fiducialliste erstellen, wenn nicht vorhanden
			if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List")))
			{
			   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
			   if( d->annotationLogic->AddHierarchy())
						 d->annotationLogic->GetActiveHierarchyNode()->SetName("PathPoint_List");
			}
			else
				d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List"))->GetID());

	
		   fiducial->Initialize(this->mrmlScene());
		  
		   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
		   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); 
		   vtkMRMLIGTLConnectorNode* Visualcnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VisualizationConnectorNode"));
		   //add observer to fiducial
		   if(vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")) && Visualcnode->GetState() == 2 ){
				d->LeadtoStart->setEnabled(true); 
		   }
		   

		}
		// Falls End Point bereits vorhanden
		
		else
		 {	   
			   vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
			   fid->RemoveObserver(CallBack);
			   fid->GetFiducialCoordinates (snode->EndPointVector);
			   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack);
			   
		}
	//}
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickSetStartPoint(){
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
	vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	//if(StartPointActive)
	//{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(snode);
	   CallBack->SetCallback(vtkMRMLIGTLSessionManagerNode::StartPointFiducialModified);  

		if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))   
		{
			double SliceOffset[3];

			for(int i = 0; i<3;i++){
			  if (vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (i, "vtkMRMLSliceNode"))) //target auslesen
				{
				
					vtkMRMLSliceNode *slice = vtkMRMLSliceNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (i, "vtkMRMLSliceNode"));
					if(strcmp(slice->GetName(),"Red")==0) {
						SliceOffset[2]= slice->GetSliceOffset();
					}else if(strcmp(slice->GetName(),"Yellow")==0){
						SliceOffset[0]= slice->GetSliceOffset();
					}else if(strcmp(slice->GetName(),"Green")== 0){
						SliceOffset[1]= slice->GetSliceOffset();
					}

					 std::cerr<<"Name: "<<slice->GetName()<< "SliceOffset: " <<slice->GetSliceToRAS()->GetElement(0,3)<< " ; "<<slice->GetSliceToRAS()->GetElement(1,3) << " ; " << slice->GetSliceToRAS()->GetElement(2,3) <<std::endl; 		
				}
			}
			 std::cerr<< "SliceOffset: " <<SliceOffset[0] << " ; "<<SliceOffset[1]  << " ; " << SliceOffset[2] <<std::endl; 		

			vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
			int b = fiducial->SetFiducialCoordinates(SliceOffset);
			fiducial->SetName("StartPoint");
			

			// Neue Fiducialliste erstellen, wenn nicht vorhanden
			if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List")))
			{
			   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
			   if( d->annotationLogic->AddHierarchy())
						 d->annotationLogic->GetActiveHierarchyNode()->SetName("PathPoint_List");
			}
			else
				d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List"))->GetID());


		   fiducial->Initialize(this->mrmlScene());
		  
		   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
		   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); //add observer to fiducial
		   fiducial->SetScene(this->mrmlScene());
		   this->mrmlScene()->AddNode(fiducial);
		// Falls End Point bereits vorhanden
		  
		   vtkMRMLIGTLConnectorNode* Visualcnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VisualizationConnectorNode"));
		  if(vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")) &&  Visualcnode->GetState() == 2 ){
				d->LeadtoStart->setEnabled(true); 
		   }
		
		}else
		 {	   
			   vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
			   fid->RemoveObserver(CallBack);
			   fid->GetFiducialCoordinates (snode->StartPointVector);
			   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack);
		}
	//}
}



//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFphi(QString editText){
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
		vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
	VFOptions.phi = editText.toAscii().data();
	snode->UpdateVirtualFixturePreview();
}
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPx(QString editText){
		
	MPOptions.X = editText.toAscii().data();
    Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(EndPointActive)
	{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(this);
 	   CallBack->SetCallback(EndPointFiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		xyz[0] = editText.toDouble();
		int b = fiducial->SetFiducialCoordinates(xyz);
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
		   if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("PathPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List"))->GetID());
    
	/*if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			{
				vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
				T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
				fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
				
			}*/
	   /*fiducial->Initialize(this->mrmlScene());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());

	   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); //add observer to fiducial
	  d->MoveToTargetPoint->setEnabled(true);  d->MoveToEntrancePoint->setEnabled(true);    

	}
	
	
	else
	 {	   
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);

		   xyz[0] = editText.toDouble();
		   fid->SetFiducialCoordinates(atof(MPOptions.X.c_str()),xyz[1],xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack);
		   
	}
	}
	UpdateVirtualFixturePreview();
}*/

//-----------------------------------------------------------------------------
/*void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPy(QString editText){
	
	MPOptions.Y = editText.toAscii().data();

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(EndPointActive)
	{
		vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
        CallBack->SetClientData(this);
   		CallBack->SetCallback(EndPointFiducialModified); 

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		int b = fiducial->SetFiducialCoordinates(xyz[0],atof(MPOptions.Y.c_str()),xyz[2]);
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
			if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("PathPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List"))->GetID());
	    /*if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
		{
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
			
		}*/
		/*fiducial->Initialize(this->mrmlScene());
		d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());

		 

	    fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); //add observer to fiducial
		d->MoveToTargetPoint->setEnabled(true);  d->MoveToEntrancePoint->setEnabled(true);  //MoveToPose Button aktivieren
	}
	// Falls End Point bereits vorhanden
	
	else
	 {
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);
		   fid->SetFiducialCoordinates(xyz[0],atof(MPOptions.Y.c_str()),xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack);
	}
	}
	UpdateVirtualFixturePreview();
}*/

//-----------------------------------------------------------------------------
/*void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPz(QString editText){
	

	MPOptions.Z = editText.toAscii().data();

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(EndPointActive)
	{
				   
    	vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	    CallBack->SetClientData(this);
   	    CallBack->SetCallback(EndPointFiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		int b = fiducial->SetFiducialCoordinates(xyz[0],xyz[1],atof(MPOptions.Z.c_str()));
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PathPoint_List")))
		{
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());		  
			if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("PathPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List"))->GetID());
	    
		/*if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
		{
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
			
		}*/
		/*fiducial->Initialize(this->mrmlScene());
	    d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());


	    fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); //add observer to fiducial
		d->MoveToTargetPoint->setEnabled(true);  d->MoveToEntrancePoint->setEnabled(true);   //MoveToPose Button aktivieren
	}
	// Falls End Point bereits vorhanden
	
	else
	 {
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);
		   fid->SetFiducialCoordinates(xyz[0],xyz[1],atof(MPOptions.Z.c_str()));
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ControlPointModifiedEvent, CallBack); //add observer to fiducial
	}
	}
	UpdateVirtualFixturePreview();
}*/



//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedVFtype(int index){
		Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
		vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (!snode)
    {
		return;
    }
	switch (index) {
		 case 0:
			 snode->CurrentVirtualFixtureType = vtkMRMLIGTLSessionManagerNode::PLANE;
			d->lineEdit_VFphi->setEnabled(false);
			 break;
		 case 1:
			 snode->CurrentVirtualFixtureType = vtkMRMLIGTLSessionManagerNode::CONE;
			 d->lineEdit_VFphi->setEnabled(true);
			 break;
		 case 2:
			 snode->CurrentVirtualFixtureType = vtkMRMLIGTLSessionManagerNode::PATH;
			 d->lineEdit_VFphi->setEnabled(false);
			 break;
		 default:
			 snode->CurrentVirtualFixtureType = vtkMRMLIGTLSessionManagerNode::PLANE;
			 d->lineEdit_VFphi->setEnabled(false);
			 break;
		 }
	snode->UpdateVirtualFixturePreview();

}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onCheckStatusChangedVFPreview(bool checked)
{
	if(!checked)
	{
		if(vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"))){
			vtkMRMLModelDisplayNode *VFmodelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"));
			VFmodelDisplay->Reset(0);
			VFmodelDisplay->SetOpacity(0);
		}
	}
	else
	{
		if(vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"))){
			vtkMRMLModelDisplayNode *VFmodelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"));
			VFmodelDisplay->SetOpacity(0.2);
		}
	}
}	
//-----------------------------------------------------------------------------

//-------- Tobias F. Create Fiducial mit Mittelung Beginn----------------------------------------------
void TransformChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data);
std::vector<double> fiducialCoordinates(3);  // Vektor mit Fiducial Koordinaten
int steps = 10;	// Anzahl der gemittelten Positionen
vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New(); // Callback erstellen

void qSlicerLightWeightRobotIGTFooBarWidget::CreateFiducial()  
{	
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
    
	// Steps und Koordinatenvektor zurücksetzen
    steps = 0;
	fiducialCoordinates[0] = 0;
	fiducialCoordinates[1] = 0;
	fiducialCoordinates[2] = 0;
	
	// Callback 
	CallBack->SetClientData(this);
    CallBack->SetCallback(TransformChanged);  

	// Pointer auf T_EE erstellen
    vtkMRMLLinearTransformNode* T_EE;
	T_EE = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE"));

	// Abfrage ob T_EE vorhanden ist
    if(!T_EE){
         std::cout<< "T_EE not found"<< std::endl;
         return;
    }
    // Beobachter zu T_EE hinzufügen
    T_EE->AddObserver( vtkMRMLTransformableNode::TransformModifiedEvent, CallBack); //add observer to transformnode
}
//-------------------------------------------------------------------------------------------------------
void TransformChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data) // Mittelung der Fiducialdaten
{
    qSlicerLightWeightRobotIGTFooBarWidget* thisClass = reinterpret_cast<qSlicerLightWeightRobotIGTFooBarWidget*>(client_data);

	if(vtk_obj->GetClassName(),"vtkMRMLLinearTransformNode"){
		vtkMRMLLinearTransformNode* T_EE = reinterpret_cast<vtkMRMLLinearTransformNode*>(vtk_obj);

		if(!T_EE){
			return;
		}

		if(strcmp(T_EE->GetName(),"T_EE")!=0){
			return;
		}
		// T_EE Position auslesen und auf Fiducialvektor addieren
	#if VTK_MAJOR_VERSION <= 5
			vtkMatrix4x4* T_EE_mat = T_EE->GetMatrixTransformToParent();
			fiducialCoordinates[0] += T_EE_mat->GetElement(0,3);
			fiducialCoordinates[1] += T_EE_mat->GetElement(1,3);
			fiducialCoordinates[2] += T_EE_mat->GetElement(2,3);
	#else
			vtkSmartPointer<vtkMatrix4x4> T_EE_mat = vtkSmartPointer<vtkMatrix4x4>::New();
			T_EE_mat = T_EE->GetMatrixTransformToParent();
			fiducialCoordinates[0] += T_EE_mat->GetElement(0,3);
			fiducialCoordinates[1] += T_EE_mat->GetElement(1,3);
			fiducialCoordinates[2] += T_EE_mat->GetElement(2,3);
	#endif

		steps++;

		// Wenn gewünschte Anzahl an Werten aufgenommen wurde:
		if (steps == thisClass->d_ptr->spinBox_MW->value()) // Anzahl der zu mittelnden Messungen angeben
		{
			// Mittelwert bilden
			fiducialCoordinates[0] = fiducialCoordinates[0]/steps;
			fiducialCoordinates[1] = fiducialCoordinates[1]/steps;
			fiducialCoordinates[2] = fiducialCoordinates[2]/steps;

			// Fiducial erstellen
			vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
   			int b = fiducial->SetFiducialCoordinates(fiducialCoordinates[0],fiducialCoordinates[1],fiducialCoordinates[2]); 

			// Falls Fid_list nicht vorhanden ist, diese Liste erstellen
			if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(thisClass->mrmlScene()->GetFirstNodeByName("Fid_List"))) //
			{
				   if( thisClass->d_ptr->annotationLogic->AddHierarchy())
					 thisClass->d_ptr->annotationLogic->GetActiveHierarchyNode()->SetName("Fid_List");

			}
			else{ // Falls es Fid_List, diese als aktive Hierarchy festlegen
				 thisClass->d_ptr->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(thisClass->mrmlScene()->GetFirstNodeByName("Fid_List"))->GetID());
			}
			std::string name =  thisClass->d_ptr->annotationLogic->GetActiveHierarchyNode()->GetName();
			
			// Fiducial in Liste Fid_List eintragen  
			if (name=="Fid_List")
			{
			   fiducial->Initialize(thisClass->mrmlScene());
			   thisClass->d_ptr->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
			}
			
			// Beobachter entfernen
			T_EE->RemoveObserver(CallBack);
			// Leaks vermeiden
			//T_EE_mat->Delete();
		}
	}
}


//-------------------------------------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::OnClickLoadRobot (){
   
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	std::string name;
	std::string filename;
	std::string Tname;


	std::string path = d->LineEditRobotPath->text ().toStdString();

    std::vector<double> a(3);
    std::vector<double> b(3);
    std::vector<double> c(3);

    // read stl Tool

    vtkSmartPointer<vtkSTLReader> reader =vtkSmartPointer<vtkSTLReader>::New();
	
   std::stringstream Tool_ss;
   Tool_ss <<path<<"Tool.stl";
   std::string Toolname = Tool_ss.str();
   reader->SetFileName (Toolname.c_str());
   reader->Update();
   reader->UpdateWholeExtent();
			 
	vtkNew<vtkMRMLModelDisplayNode> display;
	display->SetColor(0.4,0.4,0.4) ;  // set color (0.95,0.83,0.57 = bone)
	display->SetName("ToolDisplay");
	display->SetScene(this->mrmlScene());
	this->mrmlScene()->AddNode(display.GetPointer());

	vtkNew<vtkMRMLModelNode> model;
	model->SetName("Tool");
	model->SetPolyDataConnection(reader->GetOutputPort());
	model->SetAndObserveDisplayNodeID(display->GetID());
	vtkSmartPointer<vtkMRMLLinearTransformNode> T_EE=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
   T_EE = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE"));
    if (!T_EE)
	{
		std::cerr << "ERROR:No Transformnode T_EE found! " << std::endl;
		return;
	}
	vtkSmartPointer<vtkMRMLLinearTransformNode> T_CTBase=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
	T_CTBase = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
	T_EE->SetAndObserveTransformNodeID( T_CTBase->GetID());

	vtkSmartPointer<vtkMRMLLinearTransformNode> T_08=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
   T_08 = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_08"));
    if (!T_08)
	{
		std::cerr << "ERROR:No Transformnode T_08 found! " << std::endl;
		return;
	}
	T_08->SetAndObserveTransformNodeID( T_CTBase->GetID());
	model->SetAndObserveTransformNodeID( T_08->GetID() );
	this->mrmlScene()->AddNode(model.GetPointer());
	reader->Delete();
	model->Delete();
	display->Delete();

   for(int i = 0 ; i<8 ; i++)
    {

		if(i==0){
			//-------------------------------------------------------------------------------------------
			//create model for LBR Socket
			
		   std::stringstream Base_ss;
		   Base_ss <<path<<"Base.stl";
		   std::string Basename = Base_ss.str();

		   // read stl
			vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
			reader->SetFileName (Basename.c_str());
		   reader->Update();
		   reader->UpdateWholeExtent();
			 
			vtkNew<vtkMRMLModelDisplayNode> display;
			display->SetColor(0.95,0.95,0.95) ;  // set color (0.95,0.83,0.57 = bone)
		   display->SetName("BaseDisplay");
		   display->SetScene(this->mrmlScene());
		   this->mrmlScene()->AddNode(display.GetPointer());

			vtkNew<vtkMRMLModelNode> model;
			model->SetName("Base");
			model->SetPolyDataConnection(reader->GetOutputPort());
			model->SetAndObserveDisplayNodeID(display->GetID());
			this->mrmlScene()->AddNode(model.GetPointer());
			
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_Base = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			model->SetAndObserveTransformNodeID( T_Base->GetID());
			this->mrmlScene()->AddNode(model.GetPointer());
		   T_Base->Delete();
			reader->Delete();
			model->Delete();
			display->Delete();

		}else{
			name = "Link";
			std::stringstream name_ss;
			name_ss <<name << i;
			name = name_ss.str();
			std::stringstream filename_ss;
			filename_ss <<path<< name <<".stl";
			filename = filename_ss.str();

			Tname = "T_";
			std::stringstream Tss;
			Tss << Tname << 0<<i;
			Tname = Tss.str();
			
			// read stl
			vtkSmartPointer<vtkSTLReader> reader =
			vtkSmartPointer<vtkSTLReader>::New();
			reader->SetFileName (filename.c_str() );
			reader->Update();
			reader->UpdateWholeExtent();
		

			//connect model to transform
			vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName(Tname.c_str()));
			 if (!transformNode)
			{
				std::cout << "ERROR:No Transformnode"<< Tname.c_str()<<"T_EE found! " << std::endl;
				return;
			}
			transformNode->SetAndObserveTransformNodeID( T_CTBase->GetID());
			

			vtkNew<vtkMRMLModelDisplayNode> display;
			// model attributes
			std::string DisplayName = name + "Display";
			display->SetName(DisplayName.c_str());
			display->SetColor(0.95,0.95,0.95) ;  // set color (0.95,0.83,0.57 = bone)
			display->SetScene(this->mrmlScene());
			this->mrmlScene()->AddNode(display.GetPointer());
			
			vtkNew<vtkMRMLModelNode> model;
			model->SetName(name.c_str());
			model->SetPolyDataConnection(reader->GetOutputPort());
			model->SetAndObserveDisplayNodeID(display->GetID());
			model->SetAndObserveTransformNodeID( transformNode->GetID() );
			this->mrmlScene()->AddNode(model.GetPointer());
			

			transformNode->Delete();
			reader->Delete();
			model->Delete();
			display->Delete();

		}
    }
	T_CTBase->Delete();



}

//---------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickStartCyclic(){

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	 if (!d->SessionManagerNodeSelector)
    {
    return;
    }

  vtkMRMLNode* node = d->SessionManagerNodeSelector->currentNode();
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (snode)
    {
		vtkMRMLIGTLConnectorNode* cnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetNodeByID(snode->ConnectorNodeIDInternal));
		if(!cnode){
			return;
		}
		cnode->Start();
		vtksys::SystemTools::Delay(100);
		vtkMRMLIGTLConnectorNode* Visualcnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VisualizationConnectorNode"));
		if(!Visualcnode){
			return;
		}
		
		Visualcnode->Start();
        vtksys::SystemTools::Delay(100);
		snode->SendCommand("IDLE;");
		vtksys::SystemTools::Delay(500);
		for(int k = 0;k<50; k++){
			cnode->ImportDataFromCircularBuffer();
			if(vtkMRMLNode* node = this->mrmlScene()->GetFirstNodeByName("ACK")){
				vtksys::SystemTools::Delay(50);
				int r = snode->ObserveAcknowledgeString();
				if(r){
					k=50;
					d->GravComp->setEnabled(true);
					d->IDLE->setEnabled(true);
					d->MoveManual->setEnabled(true);
					d->ShutDown->setEnabled(true);
					this->EndPointActive=false;
					this->HomePointActive=false;
					this->StartPointActive=false;
				}

			}else{
				snode->SendCommand("IDLE;");
				vtksys::SystemTools::Delay(50);
			}
		}
		snode->SendCommand("IDLE;");
		vtksys::SystemTools::Delay(50);
		for(int l = 0;l<50; l++){
			Visualcnode->ImportDataFromCircularBuffer();
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_EE=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_EE = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE"));
			if (!T_EE)
			{
				vtksys::SystemTools::Delay(50);
				std::cerr << "ERROR:No Transformnode T_EE found! " << std::endl;
			}else{
				d->LoadRobot->setEnabled(true);
				d->GetFiducial->setEnabled(true);
				if(vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")) && vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"))) {
						d->LeadtoStart->setEnabled(true);
					}
				l=50;
			}
		}

    }

}


//---------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::OnClickShowTCPForce(){
    
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	std::string name;
	std::string filename;
	std::string Tname;


	std::string path = d->LineEditRobotPath->text().toStdString();

    std::vector<double> a(3);
    std::vector<double> b(3);
    std::vector<double> c(3);

    vtkSmartPointer<vtkSTLReader> reader =vtkSmartPointer<vtkSTLReader>::New();
   std::stringstream Target_ss;
   Target_ss <<path<<"Arrow.stl";
   std::string Targetname = Target_ss.str();
   reader->SetFileName (Targetname.c_str());

   //create model

   vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
   
   //connect model to transform
   vtkSmartPointer<vtkMRMLLinearTransformNode> T_TCPForce =vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
   T_TCPForce = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("TCPForce"));
    if (!T_TCPForce)
	{
		std::cerr << "ERROR:No Transformnode TCPForce found! " << std::endl;
		return;
	}
	vtkSmartPointer<vtkMRMLLinearTransformNode> T_CTBase=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
	T_CTBase = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
	 T_TCPForce->SetAndObserveTransformNodeID( T_CTBase->GetID());
	model->SetAndObserveTransformNodeID( T_TCPForce->GetID() );

       // model attributes
   modelDisplay->SetColor(0,0.95,0.0) ;  // set color (0.95,0.83,0.57 = bone)
   modelDisplay->SetName("TCPForceDisplay");
   modelDisplay->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(modelDisplay);

   model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
   model->SetScene(this->mrmlScene());
   model->SetPolyDataConnection(reader->GetOutputPort());
   model->SetName("ForceTCP");

   this->mrmlScene()->AddNode(model);
   reader->Delete();
   model->Delete();
   modelDisplay->Delete();
   T_TCPForce->Delete();
   //polydata->Delete();
}

//---------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickFiducialRegistration(){
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	
	qSlicerAbstractCoreModule* registrationFiducialModule = qSlicerCoreApplication::application()->moduleManager()->module("FiducialRegistration");
	if(!registrationFiducialModule){
		std::cout << "Error finding Module Fiducial Registration module!"<<std::endl;
		return;
	}
	
}
void qSlicerLightWeightRobotIGTFooBarWidget::onCheckStatusChangedAFC(bool checked){

}
