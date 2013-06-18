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
onClickRegistration()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "Registration;4;";
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
onClickReceiveRegistrationData()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "SendData;";
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
onClickWaitForTransform()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "WaitforTCTBase;";
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
onClickNavGravComp()
{
  Q_D(qSlicerLightWeightRobotIGTFooBarWidget);
  std::string CommandString;
  CommandString = "NavGravComp;";
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
  CommandString = "VirtualFixtures;plane;0.0;0.0;15.0;0.0;0.0;1.0;";
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