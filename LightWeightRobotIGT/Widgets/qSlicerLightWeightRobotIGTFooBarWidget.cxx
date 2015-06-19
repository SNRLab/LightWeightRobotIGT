
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
  

  d->LineEditRobotPath->setText( "C:\\Users\\OptTrack_user\\Documents\\GitHub\\ModellIiwa\\" );
  //Visualization
  VisualOptions.COFType = "rob";
//Path Impedance

  //Move to Pose
  //MPOptions.X = "580";  wenn einkommentiert, Crash zu Beginn durch Funktion onSelectionChangedMPx
 // MPOptions.Y = "-38";
//  MPOptions.Z = "200";
  MPOptions.A = "180";
  MPOptions.B = "0";
  MPOptions.C = "180";
  //d->lineEdit_MPx->setText(MPOptions.X.c_str());
  //d->lineEdit_MPy->setText(MPOptions.Y.c_str());
 // d->lineEdit_MPz->setText(MPOptions.Z.c_str());
  d->lineEdit_MPA->setText(MPOptions.A.c_str());
  d->lineEdit_MPB->setText(MPOptions.B.c_str());
  d->lineEdit_MPC->setText(MPOptions.C.c_str());

  
  //Virtual Fixtures
  VFOptions.VFType = "plane";
 // PIOptions.X = "560";
  //PIOptions.Y = "0";
  //PIOptions.Z = "100";
  VFOptions.nX = "0";
  VFOptions.nY = "0";
  VFOptions.nZ = "1";
  VFOptions.phi = "135";
  //d->comboBox_VFtype->setCurrentIndex(0);
  //d->lineEdit_VFx->setText(PIOptions.X.c_str());
  //d->lineEdit_VFy->setText(PIOptions.Y.c_str());
  //d->lineEdit_VFz->setText(PIOptions.Z.c_str());
  //d->lineEdit_VFnx->setText(VFOptions.nX.c_str());
  //d->lineEdit_VFny->setText(VFOptions.nY.c_str());
  //d->lineEdit_VFnz->setText(VFOptions.nZ.c_str());
  //d->lineEdit_VFphi->setText(VFOptions.phi.c_str());

  d->lineEdit_VFphi->setEnabled(false);
  d->VisualActive = false;
  d->MoveToPose->setEnabled(false);
  //d->checkBox_VF_Preview->setChecked(true);
}

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTFooBarWidget
::~qSlicerLightWeightRobotIGTFooBarWidget()
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
	cnode->SetTypeClient("192.168.42.2", 49001);
	cnode->SetName("StateControlConnectorNode");
	cnode->SetScene(this->mrmlScene());
    this->mrmlScene()->AddNode(cnode);
    snode->SetAndObserveConnectorNodeID(cnode->GetID());
	//cnode->Delete();
    }
   vtkSmartPointer< vtkMRMLIGTLConnectorNode >  Visualcnode = vtkSmartPointer< vtkMRMLIGTLConnectorNode >::New();
   Visualcnode->SetTypeClient("192.168.42.2", 49002);
   Visualcnode->SetName("VisualizationConnectorNode");
   Visualcnode->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(Visualcnode);
   //Visualcnode->Delete();
   //snode->Delete();
	
  
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
setSessionManagerNode(vtkMRMLNode *node)
{
  
 
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

}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
onClickVirtualFixtures()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "VirtualFixtures;" + VisualOptions.COFType+";"+ VFOptions.VFType +";" + PIOptions.X + ";" +  PIOptions.Y + ";" +  PIOptions.Z  + ";" + VFOptions.nX + ";" + VFOptions.nY +";" + VFOptions.nZ;
  

  
  
  if(VFOptions.VFType == "cone"){
	  vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
	  vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
	  
	  CommandString = CommandString + VFOptions.phi;
		vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
	
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base")){
			transformNode= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
		}
	
		vtkSmartPointer<vtkConeSource> cone=vtkSmartPointer<vtkConeSource>::New();	
		
		
		double height = 200; 
		double radius = height*tan(atof(VFOptions.phi.c_str())*PI/360); 
		double nbetrag = sqrt(pow(atof(VFOptions.nX.c_str()),2)+pow(atof(VFOptions.nY.c_str()),2)+pow(atof(VFOptions.nZ.c_str()),2));
		double cx = height* atof(VFOptions.nX.c_str())/(2*nbetrag) + atof(PIOptions.X.c_str());
		double cy = height* atof(VFOptions.nY.c_str())/(2*nbetrag) + atof(PIOptions.Y.c_str());;
		double cz = height* atof(VFOptions.nZ.c_str())/(2*nbetrag) + atof(PIOptions.Z.c_str());

		cone->SetDirection(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		cone->SetRadius(radius);
		cone->SetHeight(height);
		cone->SetResolution(50); 
		cone->SetCenter(cx,cy,cz); 
		if(this->mrmlScene()->GetFirstNodeByName("cone")){
			model = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("cone"));
			modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("coneDisplay"));
			model->Reset();
			modelDisplay->Reset();
		}

		model->SetScene(this->mrmlScene());
	    
		//-----------set model attributes and add them to the scene-----
		modelDisplay->SetColor(0,1,0) ;
		modelDisplay->SetOpacity(0.2) ;
		modelDisplay->SetName("coneDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		if(!this->mrmlScene()->GetFirstNodeByName("coneDisplay")){
			this->mrmlScene()->AddNode(modelDisplay);
		}

		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		model->SetAndObserveTransformNodeID( transformNode->GetID() );
		model->SetPolyDataConnection(cone->GetOutputPort());
		model->SetName("cone");
		if(!this->mrmlScene()->GetFirstNodeByName("cone")){
			this->mrmlScene()->AddNode(model);
		}

		//transformNode->Delete();
		//model->Delete();
		//modelDisplay->Delete();
		//cone->Delete();


  }
  if(VFOptions.VFType == "plane")
  {
		vtkSmartPointer<vtkRegularPolygonSource> plane=vtkSmartPointer<vtkRegularPolygonSource>::New();
	
		vtkSmartPointer<vtkRegularPolygonSource> planeb=vtkSmartPointer<vtkRegularPolygonSource>::New();	
		vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		vtkSmartPointer<vtkMRMLModelNode> modelb=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplayb=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		
		vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			transformNode= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));

		float size = 500;//  --> Hier Größe der Fläche anpassen
		planeb->SetNormal(atof(VFOptions.nX.c_str()),atof(VFOptions.nY.c_str()),atof(VFOptions.nZ.c_str()));
		planeb->SetCenter(atof(PIOptions.X.c_str()),atof(PIOptions.Y.c_str()),atof(PIOptions.Z.c_str()));
		planeb->SetNumberOfSides(4);
		planeb->SetRadius(size); //

		if(this->mrmlScene()->GetFirstNodeByName("planeEdge")){
			modelb = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("planeEdge"));
			modelDisplayb = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("planeEdgeDisplay"));
			modelb->Reset();
			modelDisplayb->Reset();
		}
	    modelb->SetScene(this->mrmlScene());
		modelb->SetAndObserveTransformNodeID( transformNode->GetID() );
	    
	    //-----------set model attributes and add them to the scene-----
	    modelDisplayb->SetColor(0,1,0) ;  
		modelDisplayb->SetOpacity(0.2) ;
	    modelDisplayb->SetName("planeEdgeDisplay");
		modelDisplayb->SetScene(this->mrmlScene());
		if(!this->mrmlScene()->GetFirstNodeByName("planeEdgeDisplay")){
			this->mrmlScene()->AddNode(modelDisplayb);
		}
		
		modelb->SetAndObserveDisplayNodeID(modelDisplayb->GetID());
		modelb->SetPolyDataConnection(planeb->GetOutputPort());
  		modelb->SetName("planeEdge");
		if(!this->mrmlScene()->GetFirstNodeByName("planeEdge")){
			this->mrmlScene()->AddNode(modelb);
		}

		plane->SetNormal(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		plane->SetCenter(atof(PIOptions.X.c_str()),atof(PIOptions.Y.c_str()),atof(PIOptions.Z.c_str()));
		plane->SetNumberOfSides(4);
		plane->SetRadius(size);
		if(this->mrmlScene()->GetFirstNodeByName("plane")){
			model = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("plane"));
			modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("planeDisplay"));
			model->Reset();
			modelDisplay->Reset();
		}
	    model->SetScene(this->mrmlScene());
	    
	    //-----------set model attributes and add them to the scene-----
	    modelDisplay->SetColor(0,1,0) ;  
		modelDisplay->SetOpacity(0.2) ;
	    modelDisplay->SetName("planeDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		if(!this->mrmlScene()->GetFirstNodeByName("planeDisplay")){
			this->mrmlScene()->AddNode(modelDisplay);
		}
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		model->SetAndObserveTransformNodeID( transformNode->GetID() );
  		model->SetPolyDataConnection(plane->GetOutputPort());
  		model->SetName("plane");
		if(!this->mrmlScene()->GetFirstNodeByName("plane")){
			this->mrmlScene()->AddNode(model);
		}

  }

  //---
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

}
//----------------------Tobias F. Beginn --------------------------------------
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
// --------------------Tobias F. Ende------------------------------------------
//-----------------------------------------------------------------------------
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

}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickPathImp(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;


	vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			transformNode= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));

		vtkSmartPointer<vtkCylinderSource> path =vtkSmartPointer<vtkCylinderSource>::New();
		vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		vtkSmartPointer< vtkMRMLLinearTransformNode > trans = vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
		trans->SetName("T_path");

		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->Identity();
		vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
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
		tnode->GetMatrixTransformToParent(transformMatrix.GetPointer());
		dPosition[0] = transformMatrix->GetElement(0,3);
		dPosition[1] = transformMatrix->GetElement(1,3);
		dPosition[2] = transformMatrix->GetElement(2,3);
#endif

		vtkVector3d CurrentPosition = vtkVector3d(dPosition);
		vtkVector3d TargetPosition = vtkVector3d(atof(PIOptions.X.c_str()), atof(PIOptions.Y.c_str()), atof(PIOptions.Z.c_str()));
	
		vtkVector3d uvec = TargetPosition -CurrentPosition;
		vtkVector3d nvec = uvec.Normalized();
		double  beta = -asin(nvec[0])* 180.0 / PI;
		double alpha = atan2(nvec[2],nvec[1])* 180.0 / PI;
		transform->RotateX(alpha);
		transform->RotateZ(beta);
		double* rotation = transform->GetOrientation();
	
		vtkSmartPointer<vtkMatrix4x4> mat = transform->GetMatrix();
		mat = vtkMatrix4x4::SafeDownCast(mat);
		mat->SetElement(0,3,atof(PIOptions.X.c_str()) );
		mat->SetElement(1,3,atof(PIOptions.Y.c_str()) );
		mat->SetElement(2,3,atof(PIOptions.Z.c_str()) );

		trans->ApplyTransformMatrix(mat);
		this->mrmlScene()->AddNode(trans);
		
		double height = uvec.Norm();  
		double radius = 10;  
		
		path->SetHeight(height);
		path->SetRadius(radius);
		//Connect to transform...
		path->SetCenter(0, 0 - height/2, 0);
		path->SetResolution(50); // Auflösung des Kegels
		if(this->mrmlScene()->GetFirstNodeByName("path")){
			model = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("path"));
			modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("pathDisplay"));
			model->Reset();
			modelDisplay->Reset();
		}
		model->SetScene(this->mrmlScene());
	    
		//-----------set model attributes and add them to the scene-----
		modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SetOpacity(0.5) ;
		modelDisplay->SetName("pathDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		if(!this->mrmlScene()->GetFirstNodeByName("pathDisplay")){
			this->mrmlScene()->AddNode(modelDisplay);
		}
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		trans->SetAndObserveTransformNodeID( transformNode->GetID());
		model->SetAndObserveTransformNodeID( trans->GetID() );
		model->SetPolyDataConnection(path->GetOutputPort());
		model->SetName("path");

		if(!this->mrmlScene()->GetFirstNodeByName("path")){
			this->mrmlScene()->AddNode(model);
		}


		CommandString = "PathImp;"+VisualOptions.COFType+";" +PIOptions.X+";"+ PIOptions.Y +";" + PIOptions.Z+";";
		//model->Delete();
		//trans->Delete();
		//path->Delete();
		//transform->Delete();
		//tnode->Delete();
		//transformMatrix->Delete();
		//modelDisplay->Delete();
		//transformNode->Delete();
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
void qSlicerLightWeightRobotIGTFooBarWidget::onClickMoveToPose(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  double*view;


  if (vtkMRMLCameraNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (0, "vtkMRMLCameraNode"))) //target auslesen
	{
	
	vtkMRMLCameraNode *camera = vtkMRMLCameraNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (0, "vtkMRMLCameraNode"));
	view=camera->GetViewUp();
			
	}




  CommandString = "MoveToPose;" + VisualOptions.COFType + ";" +  MPOptions.X + ";" + MPOptions.Y + ";" + MPOptions.Z +";" + MPOptions.A  +";"+ MPOptions.B  +";" + MPOptions.C+";";



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
bool activeA = true;
double  d_xyz[3] = {0,0,1};

void d_FiducialModified(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data) // Mittelung der Fiducialdaten
{
    qSlicerLightWeightRobotIGTFooBarWidget* thisClass = reinterpret_cast<qSlicerLightWeightRobotIGTFooBarWidget*>(client_data);
    vtkMRMLAnnotationFiducialNode* d_fiducial = reinterpret_cast<vtkMRMLAnnotationFiducialNode*>(vtk_obj);

	if(activeA)
	{
	activeA = false;
	double  xyz[3] = {0};
	d_fiducial->GetFiducialCoordinates (d_xyz);

	vtkMRMLAnnotationFiducialNode* fiducial = vtkMRMLAnnotationFiducialNode::SafeDownCast(thisClass->mrmlScene()->GetFirstNodeByName("StartPoint"));
	fiducial->GetFiducialCoordinates (xyz);
	
	d_xyz[0] = xyz[0] - d_xyz[0] ;
	d_xyz[1] = xyz[1] - d_xyz[1];
	d_xyz[2] = xyz[2] - d_xyz[2];

	double nbetrag = sqrt(pow(d_xyz[0],2)+pow(d_xyz[1],2)+pow(d_xyz[2],2));

	d_xyz[0]/=nbetrag;
	d_xyz[1]/=nbetrag;
	d_xyz[2]/=nbetrag;

	thisClass->d_ptr->lineEdit_VFnx->setText(QString::number(d_xyz[0]));
	thisClass->d_ptr->lineEdit_VFny->setText(QString::number(d_xyz[1]));
	thisClass->d_ptr->lineEdit_VFnz->setText(QString::number(d_xyz[2]));
	
	activeA = true;
	thisClass->MoveVF();
	
	}
}
//-----------------------------------------------------------------------------
void FiducialModifiedA(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data) // Mittelung der Fiducialdaten
{
    qSlicerLightWeightRobotIGTFooBarWidget* thisClass = reinterpret_cast<qSlicerLightWeightRobotIGTFooBarWidget*>(client_data);
    vtkMRMLAnnotationFiducialNode* fiducial = reinterpret_cast<vtkMRMLAnnotationFiducialNode*>(vtk_obj);
	activeA = false;
	double  xyz[3] = {0};
	fiducial->GetFiducialCoordinates (xyz);

	thisClass->d_ptr->lineEdit_VFx->setText(QString::number(xyz[0]));
	thisClass->d_ptr->lineEdit_VFy->setText(QString::number(xyz[1]));
	thisClass->d_ptr->lineEdit_VFz->setText(QString::number(xyz[2]));
	
	//if (thisClass->d_ptr->checkBox_VF_Preview->isChecked())
  
	vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(thisClass->mrmlScene()->GetFirstNodeByName("DirectionPoint"));

	xyz[0] -=10*d_xyz[0];
	xyz[1] -=10*d_xyz[1];
	xyz[2] -=10*d_xyz[2];
    d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);

    activeA = true;
	thisClass->MoveVF();
}



//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::MoveVF(){
	
	vtkSmartPointer<vtkConeSource> cone=vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkRegularPolygonSource> plane=vtkSmartPointer<vtkRegularPolygonSource>::New();
	vtkSmartPointer<vtkAlgorithmOutput> VFPolyData=vtkSmartPointer<vtkAlgorithmOutput>::New();
	
	if(VFOptions.VFType == "cone")
	{
			
		double height = 200; 
		double radius = height*tan(atof(VFOptions.phi.c_str())*PI/360); 
		double nbetrag = sqrt(pow(atof(VFOptions.nX.c_str()),2)+pow(atof(VFOptions.nY.c_str()),2)+pow(atof(VFOptions.nZ.c_str()),2));
		double cx = height* atof(VFOptions.nX.c_str())/(2*nbetrag) + atof(PIOptions.X.c_str());
		double cy = height* atof(VFOptions.nY.c_str())/(2*nbetrag) + atof(PIOptions.Y.c_str());;
		double cz = height* atof(VFOptions.nZ.c_str())/(2*nbetrag) + atof(PIOptions.Z.c_str());

		cone->SetDirection(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		cone->SetRadius(radius);
		cone->SetHeight(height);
		cone->SetResolution(50); 
		cone->SetCenter(cx,cy,cz); 
		
		VFPolyData = cone->GetOutputPort();		
	}
	else // if(VFOptions.VFType == "plane")
	{
	
		vtkSmartPointer<vtkRegularPolygonSource> planeb=vtkSmartPointer<vtkRegularPolygonSource>::New();	
		
		float size = 500;//  --> Hier Größe der Fläche anpassen
		planeb->SetNormal(atof(VFOptions.nX.c_str()),atof(VFOptions.nY.c_str()),atof(VFOptions.nZ.c_str()));
		planeb->SetCenter(atof(PIOptions.X.c_str()),atof(PIOptions.Y.c_str()),atof(PIOptions.Z.c_str()));
		planeb->SetNumberOfSides(4);
		planeb->SetRadius(size); //
	    
		plane->SetNormal(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		plane->SetCenter(atof(PIOptions.X.c_str()),atof(PIOptions.Y.c_str()),atof(PIOptions.Z.c_str()));
		plane->SetNumberOfSides(4);
		plane->SetRadius(size);

		VFPolyData = plane->GetOutputPort();
		
	}
	// Modelle finden oder erstellen
	if (vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_Temp")))
	{
		vtkMRMLModelNode *model = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_Temp"));
		vtkMRMLModelDisplayNode *modelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"));

		modelDisplay->SetColor(0,0,1) ;
		//modelDisplay->SetOpacity(0.2) ;
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());

		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
		{
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			model->SetAndObserveTransformNodeID( T_CT_Base->GetID() );
		}
		else 
			std::cout<<"No Transformation T_CT_Base found!"<<std::endl;	
			
			
		model->SetPolyDataConnection(VFPolyData);
	}
	else
	{
		vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		model->SetScene(this->mrmlScene());
		modelDisplay->SetScene(this->mrmlScene());
		modelDisplay->SetName("VF_DisplayTemp");
		model->SetName("VF_Temp");
		

		modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SetOpacity(0.2) ;
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());

		
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
		{
			vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
			model->SetAndObserveTransformNodeID( T_CT_Base->GetID() );
		}
		else 
			std::cout<<"No Transformation T_CT_Base found!"<<std::endl;	
		
		
		

		model->SetPolyDataConnection(VFPolyData);
	   
		this->mrmlScene()->AddNode(model);
		this->mrmlScene()->AddNode(modelDisplay);
	}
	
	

}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFx(QString editText){
	
		PIOptions.X = editText.toAscii().data();
		Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(activeA)
	{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(this);
 	   CallBack->SetCallback(FiducialModifiedA);  

	   vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   d_CallBack->SetClientData(this);
 	   d_CallBack->SetCallback(d_FiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		xyz[0] = editText.toDouble();
		int b = fiducial->SetFiducialCoordinates(xyz);
		fiducial->SetName("StartPoint");

		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> d_fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		d_fiducial->SetName("DirectionPoint");
		xyz[2] +=10 ;
		int a = d_fiducial->SetFiducialCoordinates(xyz);
		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
		   if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("StartPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List"))->GetID());
		
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			{
				vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
				T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
				fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
				d_fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
			}

	   fiducial->Initialize(this->mrmlScene());
	   d_fiducial->Initialize(this->mrmlScene());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(d_fiducial->GetID());
	   
	   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
	   d_fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack); //add observer to fiducial

	   d->MoveToPose->setEnabled(true);    //MoveToPose Button aktivieren

	}
	// Falls End Point bereits vorhanden
	
	else
	 {	   
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);

		   xyz[0] = editText.toDouble();
		   fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack);

		   vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
		   d_fid->RemoveObserver(d_CallBack);
		   
			xyz[0] +=10*d_xyz[0];
			xyz[1] +=10*d_xyz[1];
			xyz[2] +=10*d_xyz[2];
		   d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);


		   
		   
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFy(QString editText){

	PIOptions.Y = editText.toAscii().data();
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
		double  xyz[3] = {0};
// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(activeA)
	{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(this);
 	   CallBack->SetCallback(FiducialModifiedA);  

	   vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   d_CallBack->SetClientData(this);
 	   d_CallBack->SetCallback(d_FiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		xyz[1] = editText.toDouble();
		int b = fiducial->SetFiducialCoordinates(xyz);
		fiducial->SetName("StartPoint");
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> d_fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		d_fiducial->SetName("DirectionPoint");
		xyz[2] +=10 ;
		int a = d_fiducial->SetFiducialCoordinates(xyz);

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
		   if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("StartPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List"))->GetID());
	
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			{
				vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
				T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
				fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
				d_fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
			}

	   fiducial->Initialize(this->mrmlScene());
	   d_fiducial->Initialize(this->mrmlScene());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(d_fiducial->GetID());
	   
	   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
	   d_fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack); //add observer to fiducial

	   d->MoveToPose->setEnabled(true);    //MoveToPose Button aktivieren

	}
	// Falls End Point bereits vorhanden
	
	else
	 {	   
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);

		   xyz[1] = editText.toDouble();
		   fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack);

		    vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
		   d_fid->RemoveObserver(d_CallBack);
		   
			xyz[0] +=10*d_xyz[0];
			xyz[1] +=10*d_xyz[1];
			xyz[2] +=10*d_xyz[2];
		   d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);
		   
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFz(QString editText){

	PIOptions.Z = editText.toAscii().data();

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
		double  xyz[3] = {0};
// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(activeA)
	{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(this);
 	   CallBack->SetCallback(FiducialModifiedA);  

	   vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   d_CallBack->SetClientData(this);
 	   d_CallBack->SetCallback(d_FiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		xyz[2] = editText.toDouble();
		int b = fiducial->SetFiducialCoordinates(xyz);
		fiducial->SetName("StartPoint");
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> d_fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		d_fiducial->SetName("DirectionPoint");
		xyz[2] +=10 ;
		int a = d_fiducial->SetFiducialCoordinates(xyz);

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
		   if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("StartPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint_List"))->GetID());
	    
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			{
				vtkSmartPointer<vtkMRMLLinearTransformNode> T_CT_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
				T_CT_Base= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
				fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
				d_fiducial->SetAndObserveTransformNodeID(T_CT_Base->GetID());
			}

	   fiducial->Initialize(this->mrmlScene());
	   d_fiducial->Initialize(this->mrmlScene());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(d_fiducial->GetID());
	   
	   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
	   d_fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack); //add observer to fiducial

	   d->MoveToPose->setEnabled(true);    //MoveToPose Button aktivieren
	}
	// Falls End Point bereits vorhanden
	
	else
	 {	   
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);

		   xyz[2] = editText.toDouble();
		   fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack);

		    vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
		   d_fid->RemoveObserver(d_CallBack);
		   
			xyz[0] +=10*d_xyz[0];
			xyz[1] +=10*d_xyz[1];
			xyz[2] +=10*d_xyz[2];
		   d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
		   d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);
		   
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFnx(QString editText){
	VFOptions.nX = editText.toAscii().data();
	if(activeA && vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))
	{	
	activeA = false;
	double  xyz[3] = {0};
	vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
	fid->GetFiducialCoordinates (xyz);

    vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
	 vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	d_CallBack->SetClientData(this);
 	d_CallBack->SetCallback(d_FiducialModified);  
	d_fid->RemoveObserver(d_CallBack);
	xyz[0] +=10*d_xyz[0];
	xyz[1] +=10*d_xyz[1];
	xyz[2] +=10*d_xyz[2];
	d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
	d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);
	activeA = true;
	}
	MoveVF();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFny(QString editText){
	VFOptions.nY = editText.toAscii().data();
	if(activeA && vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))
	{	
	activeA = false;
	double  xyz[3] = {0};
	vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
	fid->GetFiducialCoordinates (xyz);
	
    vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
    vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	d_CallBack->SetClientData(this);
 	d_CallBack->SetCallback(d_FiducialModified);  
	d_fid->RemoveObserver(d_CallBack);
	xyz[0] +=10*d_xyz[0];
	xyz[1] +=10*d_xyz[1];
	xyz[2] +=10*d_xyz[2];
	d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
	d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);
	activeA = true;
	}
	MoveVF();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFnz(QString editText){
	VFOptions.nZ = editText.toAscii().data();
	
	if(activeA && vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint")))
	{
	activeA = false;
	double  xyz[3] = {0};
	vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("StartPoint"));
	fid->GetFiducialCoordinates (xyz);

    vtkMRMLAnnotationFiducialNode *d_fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("DirectionPoint"));
    vtkSmartPointer<vtkCallbackCommand> d_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	d_CallBack->SetClientData(this);
 	d_CallBack->SetCallback(d_FiducialModified);  
	d_fid->RemoveObserver(d_CallBack);
	xyz[0] +=10*d_xyz[0];
	xyz[1] +=10*d_xyz[1];
	xyz[2] +=10*d_xyz[2];
	d_fid->SetFiducialCoordinates(xyz[0],xyz[1],xyz[2]);
	d_fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, d_CallBack);
	activeA = true;
	}
	MoveVF();
}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFphi(QString editText){
	VFOptions.phi = editText.toAscii().data();
	MoveVF();
}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickPlaceEndPointManually()
{

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	d->annotationLogic->StartPlaceMode(false);


}
//-----------------------------------------------------------------------------
bool active = true;
void FiducialModified(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data) // Mittelung der Fiducialdaten
{
    qSlicerLightWeightRobotIGTFooBarWidget* thisClass = reinterpret_cast<qSlicerLightWeightRobotIGTFooBarWidget*>(client_data);
    vtkMRMLAnnotationFiducialNode* fiducial = reinterpret_cast<vtkMRMLAnnotationFiducialNode*>(vtk_obj);
	active = false;
	double  xyz[3] = {0};
	fiducial->GetFiducialCoordinates (xyz);

	thisClass->d_ptr->lineEdit_MPx->setText(QString::number(xyz[0]));
	thisClass->d_ptr->lineEdit_MPy->setText(QString::number(xyz[1]));
	thisClass->d_ptr->lineEdit_MPz->setText(QString::number(xyz[2]));
	active = true;
}
//---------------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPx(QString editText){
		
	MPOptions.X = editText.toAscii().data();
    Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(active)
	{
	   vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	   CallBack->SetClientData(this);
 	   CallBack->SetCallback(FiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		xyz[0] = editText.toDouble();
		int b = fiducial->SetFiducialCoordinates(xyz);
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
		   if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("EndPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List"))->GetID());
	    
	   fiducial->Initialize(this->mrmlScene());
	   d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());

	   fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
	   d->MoveToPose->setEnabled(true);    //MoveToPose Button aktivieren

	}
	// Falls End Point bereits vorhanden
	
	else
	 {	   
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);

		   xyz[0] = editText.toDouble();
		   fid->SetFiducialCoordinates(atof(MPOptions.X.c_str()),xyz[1],xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack);
		   
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPy(QString editText){
	
	MPOptions.Y = editText.toAscii().data();

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(active)
	{
		vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
        CallBack->SetClientData(this);
   		CallBack->SetCallback(FiducialModified); 

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		int b = fiducial->SetFiducialCoordinates(xyz[0],atof(MPOptions.Y.c_str()),xyz[2]);
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List")))
		{
		   d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());
			if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("EndPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List"))->GetID());
	    
		fiducial->Initialize(this->mrmlScene());
		d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());

		 

	    fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
		d->MoveToPose->setEnabled(true);  d->MoveToPose->setEnabled(true);  //MoveToPose Button aktivieren
	}
	// Falls End Point bereits vorhanden
	
	else
	 {
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);
		   fid->SetFiducialCoordinates(xyz[0],atof(MPOptions.Y.c_str()),xyz[2]);
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack);
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPz(QString editText){
	

	MPOptions.Z = editText.toAscii().data();

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	double  xyz[3] = {0};
	// Fiducial auswählen, falls vorhanden und Koordianten aktualisieren
	if(active)
	{
				   
    	vtkSmartPointer<vtkCallbackCommand> CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	    CallBack->SetClientData(this);
   	    CallBack->SetCallback(FiducialModified);  

	if (!vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint")))   
	{
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
		int b = fiducial->SetFiducialCoordinates(xyz[0],xyz[1],atof(MPOptions.Z.c_str()));
		fiducial->SetName("EndPoint");

		// Neue Fiducialliste erstellen, wenn nicht vorhanden
		if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List")))
		{
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("All Annotations"))->GetID());		  
			if( d->annotationLogic->AddHierarchy())
					 d->annotationLogic->GetActiveHierarchyNode()->SetName("EndPoint_List");
		}
		else
			d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint_List"))->GetID());
	    
		fiducial->Initialize(this->mrmlScene());
	    d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());


	    fiducial->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
		d->MoveToPose->setEnabled(true);  //MoveToPose Button aktivieren
	}
	// Falls End Point bereits vorhanden
	
	else
	 {
           vtkMRMLAnnotationFiducialNode *fid = vtkMRMLAnnotationFiducialNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("EndPoint"));
		   fid->RemoveObserver(CallBack);
		   fid->GetFiducialCoordinates (xyz);
		   fid->SetFiducialCoordinates(xyz[0],xyz[1],atof(MPOptions.Z.c_str()));
		   fid->AddObserver(vtkMRMLAnnotationFiducialNode::ValueModifiedEvent, CallBack); //add observer to fiducial
	}
	}
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPA(QString editText){
	MPOptions.A = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPB(QString editText){
	MPOptions.B = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPC(QString editText){
	MPOptions.C = editText.toAscii().data();
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedVFtype(int index){
		Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
	switch (index) {
		 case 0:
			VFOptions.VFType = "plane";
			d->lineEdit_VFphi->setEnabled(false);
			MoveVF();
			 break;
		 case 1:
			 VFOptions.VFType = "cone";
			 d->lineEdit_VFphi->setEnabled(true);
			 MoveVF();
			 break;
		 default:
			 VFOptions.VFType = "plane";
			 d->lineEdit_VFphi->setEnabled(false);
			 MoveVF();
			 break;
		 }

}
//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onCheckStatusChangedVFPreview(bool checked)
{
	if(!checked)
	{
		if(vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"))){
			vtkMRMLModelDisplayNode *VFmodelDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VF_DisplayTemp"));
			VFmodelDisplay->Reset();
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

/*//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedVisualCOFrame(int index){
	switch (index) {
		 case 0:
			VisualOptions.COFType = "rob";
			 break;
		 case 1:
			 VisualOptions.COFType = "img";
			 break;
		 case 2:
			 VisualOptions.COFType = "jnt";
			 break;
		 default:
			 VisualOptions.COFType = "img";
			 break;
		 }
}
*/

//-----------------------------------------------------------------------------
/*void qSlicerLightWeightRobotIGTFooBarWidget::CreateFiducial()  
{	

	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
    tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
	if (!tnode)
    {
		std::cout << "ERROR:No Transformnode T_EE found! " << std::endl;
		return;
    }

	// create fiducial
	vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
	double fiducialCoordinates[3] = {0.0, 0.0, 0.0};
#if VTK_MAJOR_VERSION <= 5
	vtkMatrix4x4* transformMatrix = tnode->GetMatrixTransformToParent();
	fiducialCoordinates[0] = transformMatrix->GetElement(0,3);
	fiducialCoordinates[1] = transformMatrix->GetElement(1,3);
	fiducialCoordinates[2] = transformMatrix->GetElement(2,3);
#else
	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	tnode->GetMatrixTransformToParent(transformMatrix.GetPointer());
	fiducialCoordinates[0] = transformMatrix->GetElement(0,3);
	fiducialCoordinates[1] = transformMatrix->GetElement(1,3);
	fiducialCoordinates[2] = transformMatrix->GetElement(2,3);
#endif

 	int b = fiducial->SetFiducialCoordinates(fiducialCoordinates); //set coordiantes of the fiducial

	// create new fiducial list
    if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("Fid_List")))
    {
           if( d->annotationLogic->AddHierarchy())
             d->annotationLogic->GetActiveHierarchyNode()->SetName("Fid_List");

    }
    else
         d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("Fid_List"))->GetID());
    
	std::string name =  d->annotationLogic->GetActiveHierarchyNode()->GetName();

	 if (name=="Fid_List")
	 {
	   fiducial->Initialize(this->mrmlScene());
       d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
    }
	 //tnode->Delete();
	 //fiducial->Delete();
	 //transformMatrix->Delete();


}
*/
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
	vtkMRMLLinearTransformNode* PivotPoint;
	PivotPoint = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PivotPoint"));

}
//-------------------------------------------------------------------------------------------------------
void TransformChanged(vtkObject* vtk_obj, unsigned long event, void* client_data, void* call_data) // Mittelung der Fiducialdaten
{
    qSlicerLightWeightRobotIGTFooBarWidget* thisClass = reinterpret_cast<qSlicerLightWeightRobotIGTFooBarWidget*>(client_data);
    vtkMRMLLinearTransformNode* T_EE = reinterpret_cast<vtkMRMLLinearTransformNode*>(vtk_obj);

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
		else // Falls es Fid_List, diese als aktive Hierarchy festlegen
			 thisClass->d_ptr->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(thisClass->mrmlScene()->GetFirstNodeByName("Fid_List"))->GetID());
	    
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
		T_EE_mat->Delete();
	}
}
//-------- Tobias F. Create Fiducial mit Mittelung Ende----------------------------------------------
//---------------------------------------------------------------

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
		vtkMRMLIGTLConnectorNode* Visualcnode =  vtkMRMLIGTLConnectorNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("VisualizationConnectorNode"));
		if(!Visualcnode){
			return;
		}
		Visualcnode->Start();
                vtksys::SystemTools::Delay(500);
		snode->SendCommand("IDLE;;");
                vtksys::SystemTools::Delay(50);
		snode->SendCommand("IDLE;;");
                vtksys::SystemTools::Delay(100);
		cnode->ImportDataFromCircularBuffer();
		int i = cnode->GetNumberOfIncomingMRMLNodes();
                vtksys::SystemTools::Delay(50);
		snode->ObserveAcknowledgeString();


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
