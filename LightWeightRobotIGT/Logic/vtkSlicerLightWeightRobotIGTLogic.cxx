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

// LightWeightRobotIGT Logic includes
#include "vtkMRMLScene.h"
#include "vtkSlicerLightWeightRobotIGTLogic.h"
#include "vtkMRMLIGTLSessionManagerNode.h"
#include <vtkMRMLLinearTransformNode.h>

// MRML includes

// VTK includes
#include <vtkNew.h>
// VTK includes
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLightWeightRobotIGTLogic);

//----------------------------------------------------------------------------
vtkSlicerLightWeightRobotIGTLogic::vtkSlicerLightWeightRobotIGTLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerLightWeightRobotIGTLogic::~vtkSlicerLightWeightRobotIGTLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene * scene = this->GetMRMLScene();
  scene->RegisterNodeClass(vtkNew< vtkMRMLIGTLSessionManagerNode >().GetPointer());
  std::cerr << "Registering Connector Nodes" << std::endl;
  scene->RegisterNodeClass(vtkNew< vtkMRMLLinearTransformNode >().GetPointer());
  std::cerr << "Registering Transform Nodes" << std::endl;
}

//---------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerLightWeightRobotIGTLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

