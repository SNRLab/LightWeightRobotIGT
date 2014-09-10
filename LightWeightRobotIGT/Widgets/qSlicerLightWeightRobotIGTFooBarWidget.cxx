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
#define PI 3.14159265358


//c++ includes
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
// FooBar Widgets includes
#include "vtkMRMLNode.h"

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
#include "vtkMRMLIGTLSessionManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLAnnotationTextNode.h"
#include <vtkMRMLAnnotationFiducialNode.h>
#include <vtkMRMLAnnotationHierarchyNode.h>
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLCameraNode.h> //TF


// VTK includes
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkClipPolyData.h>
#include <vtkCollectionIterator.h> //TF
#include <vtkConeSource.h> //TF
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
  
  //Setting some default values for the Command Options
  //Registration

  d->LineEditRobotPath->setText( "C:\\Users\\OptTrack_user\\Documents\\GitHub\\RobotModell\\" );
  //Visualization
  VisualOptions.COFType = "rob";
//Path Impedance

  //Move to Pose
  MPOptions.X = "580";
  MPOptions.Y = "-38";
  MPOptions.Z = "200";
  MPOptions.A = "180";
  MPOptions.B = "0";
  MPOptions.C = "180";
  d->lineEdit_MPx->setText(MPOptions.X.c_str());
  d->lineEdit_MPy->setText(MPOptions.Y.c_str());
  d->lineEdit_MPz->setText(MPOptions.Z.c_str());
  d->lineEdit_MPA->setText(MPOptions.A.c_str());
  d->lineEdit_MPB->setText(MPOptions.B.c_str());
  d->lineEdit_MPC->setText(MPOptions.C.c_str());

  
  //Virtual Fixtures
  VFOptions.VFType = "plane";
  PIOptions.X = "560";
  PIOptions.Y = "0";
  PIOptions.Z = "100";
  VFOptions.nX = "0";
  VFOptions.nY = "0";
  VFOptions.nZ = "1";
  VFOptions.phi = "135";
  d->comboBox_VFtype->setCurrentIndex(1);
  d->lineEdit_VFx->setText(PIOptions.X.c_str());
  d->lineEdit_VFy->setText(PIOptions.Y.c_str());
  d->lineEdit_VFz->setText(PIOptions.Z.c_str());
  d->lineEdit_VFnx->setText(VFOptions.nX.c_str());
  d->lineEdit_VFny->setText(VFOptions.nY.c_str());
  d->lineEdit_VFnz->setText(VFOptions.nZ.c_str());
 
  d->comboBox_VFtype->setCurrentIndex(1);
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
  
  //d->TransformNodeSelector->setMRMLScene(newScene);
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
	cnode->Start();
    snode->SetAndObserveConnectorNodeID(cnode->GetID());
    }
   vtkSmartPointer< vtkMRMLIGTLConnectorNode >  Visualcnode = vtkSmartPointer< vtkMRMLIGTLConnectorNode >::New();
   Visualcnode->SetTypeClient("192.168.42.2", 49002);
   Visualcnode->SetName("VisualizationConnectorNode");
   Visualcnode->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(Visualcnode);
   Visualcnode->Start();

  
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
	  if(this->mrmlScene()->GetFirstNodeByName("cone")){
		  this->mrmlScene()->RemoveNode(this->mrmlScene()->GetFirstNodeByName("cone"));
	  }
	  CommandString = CommandString + VFOptions.phi;
		vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
	
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			transformNode= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
	
		vtkSmartPointer<vtkConeSource> cone=vtkSmartPointer<vtkConeSource>::New();
		vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();	
		vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		
		double height = 200;  // Höhe des Kegels  --> Hier Größe anpassen!!
		double radius = height*tan(atof(VFOptions.phi.c_str())*PI/360);  // Radius= tan(Öffnungswinkeln/2) * Höhe
		double cx = height* atof(VFOptions.nX.c_str())/2 + atof(PIOptions.X.c_str());
		double cy = height* atof(VFOptions.nY.c_str())/2 + atof(PIOptions.Y.c_str());;
		double cz = height* atof(VFOptions.nZ.c_str())/2 + atof(PIOptions.Z.c_str());

		cone->SetDirection(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		cone->SetRadius(radius);
		cone->SetHeight(height);
		cone->SetResolution(50); // Auflösung des Kegels
		cone->SetCenter(cx,cy,cz); // center des Kegels liegt mittig auf der Achse ! Verschiebung zur Spitze nötig
		polydata=cone->GetOutput();
	    model->SetScene(this->mrmlScene());
		model->SetAndObservePolyData(polydata);
	    
	    //-----------set model attributes and add them to the scene-----
	    modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SetOpacity(0.3) ;
	    modelDisplay->SetName("coneDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		this->mrmlScene()->AddNode(modelDisplay);
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		model->SetAndObserveTransformNodeID( transformNode->GetID() );
  		modelDisplay->SetInputPolyData(model->GetPolyData());
  		model->SetName("cone");
		this->mrmlScene()->AddNode(model);


  }
  if(VFOptions.VFType == "plane")
  {
	   if(this->mrmlScene()->GetFirstNodeByName("plane")){
		  this->mrmlScene()->RemoveNode(this->mrmlScene()->GetFirstNodeByName("plane"));
	  }
	   if(this->mrmlScene()->GetFirstNodeByName("planeEdge")){
		  this->mrmlScene()->RemoveNode(this->mrmlScene()->GetFirstNodeByName("planeEdge"));
	  }
	  // 2 planes, da nur in Richtung des Normalenvektorssichtbar und von der anderen Seite transparent
		vtkSmartPointer<vtkRegularPolygonSource> plane=vtkSmartPointer<vtkRegularPolygonSource>::New();
		vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();	
		vtkSmartPointer<vtkRegularPolygonSource> planeb=vtkSmartPointer<vtkRegularPolygonSource>::New();
		vtkSmartPointer<vtkPolyData> polydatab=vtkSmartPointer<vtkPolyData>::New();	
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

		polydatab=planeb->GetOutput();
	    modelb->SetScene(this->mrmlScene());
		modelb->SetAndObservePolyData(polydatab);
		modelb->SetAndObserveTransformNodeID( transformNode->GetID() );
	    
	    //-----------set model attributes and add them to the scene-----
	    modelDisplayb->SetColor(0,0,1) ;  
		modelDisplayb->SetOpacity(0.4) ;
	    modelDisplayb->SetName("planeEdgeDisplay");
		modelDisplayb->SetScene(this->mrmlScene());
		this->mrmlScene()->AddNode(modelDisplayb);
		
		modelb->SetAndObserveDisplayNodeID(modelDisplayb->GetID());
  		modelDisplayb->SetInputPolyData(modelb->GetPolyData());
  		modelb->SetName("planeEdge");
		this->mrmlScene()->AddNode(modelb);

		plane->SetNormal(-1*atof(VFOptions.nX.c_str()),-1*atof(VFOptions.nY.c_str()),-1*atof(VFOptions.nZ.c_str()));
		plane->SetCenter(atof(PIOptions.X.c_str()),atof(PIOptions.Y.c_str()),atof(PIOptions.Z.c_str()));
		plane->SetNumberOfSides(4);
		plane->SetRadius(size);

		polydata=plane->GetOutput();
	    model->SetScene(this->mrmlScene());
		model->SetAndObservePolyData(polydata);
	    
	    //-----------set model attributes and add them to the scene-----
	    modelDisplay->SetColor(0,0,1) ;  
		modelDisplay->SetOpacity(0.4) ;
	    modelDisplay->SetName("planeDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		this->mrmlScene()->AddNode(modelDisplay);
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		model->SetAndObserveTransformNodeID( transformNode->GetID() );
  		modelDisplay->SetInputPolyData(model->GetPolyData());
  		model->SetName("plane");
		this->mrmlScene()->AddNode(model);
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


void qSlicerLightWeightRobotIGTFooBarWidget::onClickPathImp(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;

	if(this->mrmlScene()->GetFirstNodeByName("path")){
		this->mrmlScene()->RemoveNode(this->mrmlScene()->GetFirstNodeByName("path"));
	  }
	vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		
		if(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"))
			transformNode= vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));

		vtkSmartPointer<vtkCylinderSource> path =vtkSmartPointer<vtkCylinderSource>::New();
		vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();	
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
		
		vtkVector3d CurrentPosition = vtkVector3d(tnode->GetMatrixTransformToParent()->GetElement(0, 3),tnode->GetMatrixTransformToParent()->GetElement(1, 3), tnode->GetMatrixTransformToParent()->GetElement(2, 3));
		vtkVector3d TargetPosition = vtkVector3d(atof(PIOptions.X.c_str()), atof(PIOptions.Y.c_str()), atof(PIOptions.Z.c_str()));
	
		vtkVector3d uvec = TargetPosition -CurrentPosition;
		vtkVector3d nvec = uvec.Normalized();
		double  beta = -asin(nvec[0])* 180.0 / PI;
		double alpha = atan2(nvec[2],nvec[1])* 180.0 / PI;
		transform->RotateX(alpha);
		transform->RotateZ(beta);
		double* rotation = transform->GetOrientation();
		//transform->Translate( atof(PIOptions.X.c_str()), atof(PIOptions.Y.c_str()), atof(PIOptions.Z.c_str()));
		vtkSmartPointer<vtkMatrix4x4> mat = transform->GetMatrix();
		mat = vtkMatrix4x4::SafeDownCast(mat);
		mat->SetElement(0,3,atof(PIOptions.X.c_str()) );
		mat->SetElement(1,3,atof(PIOptions.Y.c_str()) );
		mat->SetElement(2,3,atof(PIOptions.Z.c_str()) );
		 //transformNode->SetAndObserveImageData(transform);
		trans->ApplyTransformMatrix(mat);
		//transform->Delete();
		this->mrmlScene()->AddNode(trans);
		
		double height = uvec.Norm();  // Höhe des cylinders --> Hier Größe anpassen!!
		double radius = 10;  // Radius= tan(Öffnungswinkeln/2) * Höhe
		
		path->SetHeight(height);
		path->SetRadius(radius);
		//Connect to transform...
		path->SetCenter(0, 0 - height/2, 0);
		path->SetResolution(50); // Auflösung des Kegels
	
		polydata=path->GetOutput();
		model->SetScene(this->mrmlScene());
		model->SetAndObservePolyData(polydata);
	    
		//-----------set model attributes and add them to the scene-----
		modelDisplay->SetColor(0,0,1) ;
		modelDisplay->SetOpacity(0.5) ;
		modelDisplay->SetName("pathDisplay");
		modelDisplay->SetScene(this->mrmlScene());
		this->mrmlScene()->AddNode(modelDisplay);
		
		model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		trans->SetAndObserveTransformNodeID( transformNode->GetID());
		model->SetAndObserveTransformNodeID( trans->GetID() );
		modelDisplay->SetInputPolyData(model->GetPolyData());
		model->SetName("path");
		this->mrmlScene()->AddNode(model);
		 CommandString = "PathImp;"+VisualOptions.COFType+";" +PIOptions.X+";"+ PIOptions.Y +";" + PIOptions.Z+";";
  
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
		std::cout<< CommandString<<std::endl;
    }
	
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFx(QString editText){
	
		/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PrePositioningPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(atof(editText.toAscii().data()),coord[1], coord[2]) ;	*/	
		PIOptions.X = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFy(QString editText){
	/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PrePositioningPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(coord[0], atof(editText.toAscii().data()), coord[2]) ;	*/	
	PIOptions.Y = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFz(QString editText){
	/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("PrePositioningPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(coord[0], coord[1], atof(editText.toAscii().data())) ;	*/
	PIOptions.Z = editText.toAscii().data();


}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFnx(QString editText){
	VFOptions.nX = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFny(QString editText){
	VFOptions.nY = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFnz(QString editText){
	VFOptions.nZ = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPx(QString editText){
		/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("TargetPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(atof(editText.toAscii().data()), coord[1], coord[2]) ;	*/	
	MPOptions.X = editText.toAscii().data();

}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPy(QString editText){
		/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("TargetPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(coord[0], atof(editText.toAscii().data()), coord[2]) ;*/
	MPOptions.Y = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPz(QString editText){
		/*vtkSmartPointer<vtkCollection> tl=vtkSmartPointer<vtkCollection>::New();
		vtkMRMLAnnotationHierarchyNode* tlist =vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("TargetPointList"));
		tlist->GetAssociatedChildrendNodes(tl,"vtkMRMLAnnotationFiducialNode");
		vtkSmartPointer<vtkCollectionIterator> iterator=vtkSmartPointer<vtkCollectionIterator>::New();
		vtkSmartPointer<vtkMRMLAnnotationFiducialNode> target=vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();

		iterator->SetCollection (tl);   //iterator falls später mehrere targets
		iterator->GoToFirstItem ();
		
		target = vtkMRMLAnnotationFiducialNode::SafeDownCast(iterator->GetCurrentObject ());
		double * coord ;
		target->GetFiducialCoordinates(coord);
		target->SetFiducialCoordinates(coord[0], coord[1], atof(editText.toAscii().data())) ;*/

	MPOptions.Z = editText.toAscii().data();
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
	switch (index) {
		 case 0:
			VFOptions.VFType = "plane";
			 break;
		 case 1:
			 VFOptions.VFType = "cone";
			 break;
		 default:
			 VFOptions.VFType = "plane";
			 break;
		 }
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::CreateFiducial()  
{	
	std::cerr << "On Clicked Create Fiducial!!!"<< std::endl;
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
    tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_EE")); // Transformnode festlegen
	if (!tnode)
    {
		std::cout << "ERROR:No Transformnode T_EE found! " << std::endl;
		return;
    }

	// Fiducial erstellen und Koordianten hinzufügen
	vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial = vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New(); 
 	int b = fiducial->SetFiducialCoordinates(tnode->GetMatrixTransformToParent()->GetElement(0, 3),tnode->GetMatrixTransformToParent()->GetElement(1, 3),tnode->GetMatrixTransformToParent()->GetElement(2, 3)); //set coordiantes of the fiducial

	// Neue Fiducialliste erstellen, wenn nicht vorhanden
    if (!vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("Fid_List")))
    {
           if( d->annotationLogic->AddHierarchy())
             d->annotationLogic->GetActiveHierarchyNode()->SetName("Fid_List");

    }
    else
         d->annotationLogic->SetActiveHierarchyNodeID(vtkMRMLAnnotationHierarchyNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("Fid_List"))->GetID());
    
	std::string name =  d->annotationLogic->GetActiveHierarchyNode()->GetName();

	// Fiducial der Szene und der Liste hinzufügen
	 if (name=="Fid_List")
	 {
	   fiducial->Initialize(this->mrmlScene());
       d->annotationLogic->GetActiveHierarchyNode()->SetAssociatedNodeID(fiducial->GetID());
    }

}



void qSlicerLightWeightRobotIGTFooBarWidget::OnClickLoadRobot (){
   
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	std::string name;
	std::string filename;
	std::string Tname;


	std::string path = d->LineEditRobotPath->text ().toStdString();

    std::vector<double> a(3);
    std::vector<double> b(3);
    std::vector<double> c(3);

    //-----Endeffektor auslesen und platzieren---------------------------------------------------------
    // read stl ENDEFFEKTOR

    vtkSmartPointer<vtkSTLReader> reader =vtkSmartPointer<vtkSTLReader>::New();
	
   std::stringstream Tool_ss;
   Tool_ss <<path<<"Tool2.stl";
   std::string Toolname = Tool_ss.str();
   reader->SetFileName (Toolname.c_str());

   //create model
   vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();
   vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
   polydata=reader->GetOutput();
   if (!polydata)
	{
		std::cerr << "ERROR:No polydata for"<<  name.c_str()<<"! " << std::endl;
		return;
	}
   model->SetScene(this->mrmlScene());
   model->SetAndObservePolyData(polydata);
   //connect model to transform
   vtkSmartPointer<vtkMRMLLinearTransformNode> T_1EE=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
   T_1EE = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_08"));
    if (!T_1EE)
	{
		std::cerr << "ERROR:No Transformnode T_EE found! " << std::endl;
		return;
	}
	vtkSmartPointer<vtkMRMLLinearTransformNode> T_CTBase=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
	T_CTBase = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
	T_1EE->SetAndObserveTransformNodeID( T_CTBase->GetID());
	model->SetAndObserveTransformNodeID( T_1EE->GetID() );

       // model attributes
   modelDisplay->SetColor(0.4,0.4,0.4) ;  // set color (0.95,0.83,0.57 = bone)
   modelDisplay->SetName("ToolDisplay");
   modelDisplay->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(modelDisplay);

   model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
   modelDisplay->SetInputPolyData(model->GetPolyData());
   model->SetName("Tool");

   this->mrmlScene()->AddNode(model);

    //--------Gelenke auslesen und platzieren------------------------------------------------------------------
    for(int i = 0 ; i<8 ; i++)
    {

		if(i==0){
			//-------------------------------------------------------------------------------------------
			//create model for LBR Socket
			
		   std::stringstream Base_ss;
		   Base_ss <<path<<"Base.stl";
		   std::string Basename = Base_ss.str();

		   // read stl
			vtkSmartPointer<vtkSTLReader> reader =
			vtkSmartPointer<vtkSTLReader>::New();
			reader->SetFileName (Basename.c_str());

		   vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();
		   vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
		   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
		   polydata=reader->GetOutput();
   			 if (!polydata)
			{
				std::cerr << "ERROR:No polydata for"<<  name.c_str()<<"! " << std::endl;
			return;
			}
		   model->SetScene(this->mrmlScene());
		   model->SetAndObservePolyData(polydata);
		   vtkSmartPointer<vtkMRMLLinearTransformNode> T_Base=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
		   T_Base = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("T_CT_Base"));
		   model->SetAndObserveTransformNodeID( T_Base->GetID());
			   // model attributes
		   modelDisplay->SetColor(0.95,0.95,0.95) ;  // set color (0.95,0.83,0.57 = bone)
		   modelDisplay->SetName("BaseDisplay");
		   modelDisplay->SetScene(this->mrmlScene());
		   this->mrmlScene()->AddNode(modelDisplay);

		   model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
		   modelDisplay->SetInputPolyData(model->GetPolyData());
		   model->SetName("Base");

		   this->mrmlScene()->AddNode(model);

		}else{
			name = "LBRPart";
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
			
			//create model
			vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();
			vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
			vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
			polydata=reader->GetOutput();
			 if (!polydata)
			{
			std::cout << "ERROR:No polydata for"<<  name.c_str()<<"! " << std::endl;
			return;
			}
			model->SetScene(this->mrmlScene());
			model->SetAndObservePolyData(polydata);

			//connect model to transform
			vtkSmartPointer<vtkMRMLLinearTransformNode> transformNode=vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
			transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName(Tname.c_str()));
			 if (!transformNode)
			{
			std::cout << "ERROR:No Transformnode"<< Tname.c_str()<<"T_EE found! " << std::endl;
			return;
			}
			transformNode->SetAndObserveTransformNodeID( T_CTBase->GetID());
			model->SetAndObserveTransformNodeID( transformNode->GetID() );

			// model attributes
			modelDisplay->SetColor(1,1,1) ;  // set color (0.95,0.83,0.57 = bone)
			std::string DisplayName = name + "Display";
			modelDisplay->SetName(DisplayName.c_str());
			modelDisplay->SetScene(this->mrmlScene());
			this->mrmlScene()->AddNode(modelDisplay);

			model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
			modelDisplay->SetInputPolyData(model->GetPolyData());
			model->SetName(name.c_str());

			this->mrmlScene()->AddNode(model);
			if (vtkMRMLCameraNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (0, "vtkMRMLCameraNode"))) //target auslesen
			{
				vtkMRMLCameraNode *camera = vtkMRMLCameraNode::SafeDownCast(this->mrmlScene()->GetNthNodeByClass (0, "vtkMRMLCameraNode"));
			}
			transformNode->Delete();
			T_CTBase->Delete();
			reader->Delete();
			model->Delete();
			modelDisplay->Delete();
			polydata->Delete();
		}
    }
	T_CTBase->Delete();



}
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
		snode->ObserveAcknowledgeString();//TF

    }

  

}

void qSlicerLightWeightRobotIGTFooBarWidget::OnClickShowTCPForce(){
    
	Q_D(qSlicerLightWeightRobotIGTFooBarWidget);

	std::string name;
	std::string filename;
	std::string Tname;


	std::string path = d->LineEditRobotPath->text().toStdString();

    std::vector<double> a(3);
    std::vector<double> b(3);
    std::vector<double> c(3);

    //-----Endeffektor auslesen und platzieren---------------------------------------------------------
    // read stl ENDEFFEKTOR

    vtkSmartPointer<vtkSTLReader> reader =vtkSmartPointer<vtkSTLReader>::New();
   std::stringstream Target_ss;
   Target_ss <<path<<"Pfeil.stl";
   std::string Targetname = Target_ss.str();
   reader->SetFileName (Targetname.c_str());

   //create model
   vtkSmartPointer<vtkPolyData> polydata=vtkSmartPointer<vtkPolyData>::New();
   vtkSmartPointer<vtkMRMLModelNode> model=vtkSmartPointer<vtkMRMLModelNode>::New();
   vtkSmartPointer<vtkMRMLModelDisplayNode> modelDisplay=vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
   polydata=reader->GetOutput();
   	 if (!polydata)
	{
		std::cerr << "ERROR:No polydata for"<<  name.c_str()<<"! " << std::endl;
	return;
	}
	 std::cerr << "Setting Scene model..."<<std::endl;
   model->SetScene(this->mrmlScene());
   model->SetAndObservePolyData(polydata);
   //connect model to transform
   vtkSmartPointer<vtkMRMLLinearTransformNode> TCPForce =vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
   TCPForce = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetFirstNodeByName("TCPForce"));
    if (!TCPForce)
	{
		std::cerr << "ERROR:No Transformnode TCPForce found! " << std::endl;
		return;
	}
	model->SetAndObserveTransformNodeID( TCPForce->GetID() );

       // model attributes
   modelDisplay->SetColor(0,0.95,0.0) ;  // set color (0.95,0.83,0.57 = bone)
   modelDisplay->SetName("TCPForceDisplay");
   modelDisplay->SetScene(this->mrmlScene());
   this->mrmlScene()->AddNode(modelDisplay);

   model->SetAndObserveDisplayNodeID(modelDisplay->GetID());
   modelDisplay->SetInputPolyData(model->GetPolyData());
   model->SetName("TCP Force");

   this->mrmlScene()->AddNode(model);
   reader->Delete();
   model->Delete();
   modelDisplay->Delete();
   polydata->Delete();
}

//-----------------------------------------------------------------------------