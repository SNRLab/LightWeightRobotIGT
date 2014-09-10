/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// LightWeightRobotIGT Logic includes
#include <vtkSlicerLightWeightRobotIGTLogic.h>

// LightWeightRobotIGT includes
#include "qSlicerLightWeightRobotIGTModule.h"
#include "qSlicerLightWeightRobotIGTModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerLightWeightRobotIGTModule, qSlicerLightWeightRobotIGTModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLightWeightRobotIGTModulePrivate
{
public:
  qSlicerLightWeightRobotIGTModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLightWeightRobotIGTModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTModulePrivate
::qSlicerLightWeightRobotIGTModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLightWeightRobotIGTModule methods

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTModule
::qSlicerLightWeightRobotIGTModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLightWeightRobotIGTModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLightWeightRobotIGTModule::~qSlicerLightWeightRobotIGTModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLightWeightRobotIGTModule::helpText()const
{
  return "The LightWeightRobotIGT module allows for communication with and light weight robot iiwa (KUKA Laboratories GmbH) via an OpenIGTLink based interfaces for state control and visualization. ";
}

//-----------------------------------------------------------------------------
QString qSlicerLightWeightRobotIGTModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1 and by KUKA Laboratories GmbH (Augsburg, Germany)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLightWeightRobotIGTModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Junichi Tokuda");
  moduleContributors << QString("Sebastian Tauscher");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerLightWeightRobotIGTModule::icon()const
{
  return QIcon(":/Icons/LightWeightRobotIGT.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLightWeightRobotIGTModule::categories() const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLightWeightRobotIGTModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerLightWeightRobotIGTModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerLightWeightRobotIGTModule
::createWidgetRepresentation()
{
  return new qSlicerLightWeightRobotIGTModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLightWeightRobotIGTModule::createLogic()
{
  return vtkSlicerLightWeightRobotIGTLogic::New();
}
