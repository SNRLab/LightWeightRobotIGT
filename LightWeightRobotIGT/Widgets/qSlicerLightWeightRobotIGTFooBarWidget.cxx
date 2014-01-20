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

// FooBar Widgets includes
#include "vtkMRMLNode.h"

#include "qSlicerLightWeightRobotIGTFooBarWidget.h"
#include "ui_qSlicerLightWeightRobotIGTFooBarWidget.h"

#include "vtkMRMLIGTLSessionManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"


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

  //Setting some default values for the Command Options
  //Registration
  RegOptions.PointstoRegister = "4";

  //Visualization
  VisualOptions.COFType = "rob";

  //Path Impedance
  PIOptions.COFType = "rob";
  PIOptions.X = "500";
  PIOptions.Y = "0";
  PIOptions.Z = "400";

  //Move to Pose
  MPOptions.COFType = "rob";
  MPOptions.X = "500";
  MPOptions.Y = "0";
  MPOptions.Z = "500";
  MPOptions.A = "0";
  MPOptions.B = "0";
  MPOptions.C = "0";
  
  //Virtual Fixtures
  VFOptions.COFType= "rob";
  VFOptions.VFType = "plane";
  VFOptions.X = "500";
  VFOptions.Y = "0";
  VFOptions.Z = "100";
  VFOptions.nX = "0";
  VFOptions.nY = "0";
  VFOptions.nZ = "1";
  VFOptions.phi = "90";

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
  
}


//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::
setSessionManagerNode(vtkMRMLNode *node)
{
  vtkMRMLIGTLSessionManagerNode* snode = vtkMRMLIGTLSessionManagerNode::SafeDownCast(node);
  if (!snode)
    {
    return;
    }
  if (!this->mrmlScene())
    {
    return;
    }
  
  if (!snode->GetConnectorNodeID())
    {
    vtkSmartPointer< vtkMRMLIGTLConnectorNode > cnode = vtkSmartPointer< vtkMRMLIGTLConnectorNode >::New();
    this->mrmlScene()->AddNode(cnode);
    snode->SetAndObserveConnectorNodeID(cnode->GetID());
    }

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
  CommandString = "VirtualFixtures;" + VFOptions.COFType+";"+ VFOptions.VFType +";"  + VFOptions.X +";" + VFOptions.Y +";" + VFOptions.Z + ";"+ VFOptions.nX +";" + VFOptions.nY +";" + VFOptions.nZ + ";" ;
  if(VFOptions.VFType == "cone"){
	  CommandString = CommandString + VFOptions.phi;
  }
  
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

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onClickQuit(){
	 Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "Quit;";
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
  CommandString = "PathImp;"+PIOptions.COFType+";"+PIOptions.X +";"+ PIOptions.Y +";" + PIOptions.Z +";";
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
  CommandString = "MoveToPose;" + MPOptions.COFType + ";" + MPOptions.X +";" + MPOptions.Y + ";" + MPOptions.Z +";" + MPOptions.A +";"+ MPOptions.B +";" + MPOptions.C +";";
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
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFx(QString editText){
	VFOptions.X = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFy(QString editText){
	VFOptions.Y = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedVFz(QString editText){
	VFOptions.Z = editText.toAscii().data();
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
	MPOptions.X = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPy(QString editText){
	MPOptions.Y = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedMPz(QString editText){
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
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedPIx(QString editText){
	PIOptions.X = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedPIy(QString editText){
	PIOptions.Y = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onSelectionChangedPIz(QString editText){
	PIOptions.Z = editText.toAscii().data();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onValueChangedPointsRegister(int Value){


	
	std::string Result;//string which will contain the result

	std::stringstream convert; // stringstream used for the conversion

	convert << Value;//add the value of Number to the characters in the stream

	Result = convert.str();//set Result to the content of the stream

	RegOptions.PointstoRegister = Result;
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedSendDataType(int index){
	switch (index) {
		 case 0:
			SDOptions.Datatype = "point";
			 break;
		 case 1:
			 SDOptions.Datatype = "transform";
			 break;
		 default:
			 SDOptions.Datatype = "point";
			 break;
		 }
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
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedVFCOFrame(int index){
	switch (index) {
		 case 0:
			VFOptions.COFType = "rob";
			 break;
		 case 1:
			 VFOptions.COFType = "img";
			 break;
		 default:
			 VFOptions.COFType = "rob";
			 break;
		 }
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedPICOFrame(int index){
	switch (index) {
		 case 0:
			PIOptions.COFType = "rob";
			 break;
		 case 1:
			 PIOptions.COFType = "img";
			 break;
		 default:
			 PIOptions.COFType = "rob";
			 break;
		 }
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTFooBarWidget::onIndexChangedMPCOFrame(int index){
	switch (index) {
		 case 0:
			MPOptions.COFType = "rob";
			 break;
		 case 1:
			 MPOptions.COFType = "img";
			 break;
		 default:
			MPOptions.COFType = "rob";
			 break;
		 }
}

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
