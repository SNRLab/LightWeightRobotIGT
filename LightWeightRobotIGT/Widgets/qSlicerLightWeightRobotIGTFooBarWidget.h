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

#ifndef __qSlicerLightWeightRobotIGTFooBarWidget_h
#define __qSlicerLightWeightRobotIGTFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerLightWeightRobotIGTModuleWidgetsExport.h"
#include "qSlicerWidget.h"

class qSlicerLightWeightRobotIGTFooBarWidgetPrivate;
class vtkMRMLNode;

//Structs for the Options for the different States
//VirtualFixtures Options
// - position of the plane/cone X,Y,Z
// - normal vector of the plane/cone nX, nY, nZ
// - Virtual Fixture type VFType =plane/cone;
struct VirtualFixturesOptions{
	std::string X;
	std::string Y;
	std::string Z;
	std::string nX;
	std::string nY;
	std::string nZ;
	std::string VFType;
	std::string COFType;
	std::string phi;

};

//MoveToPose Options
// - position of end point X,Y,Z
// - Orientation of endpoint (RPY) A, B, C
// - Coordinate System used COFType=Imagespace/robbase;
struct MoveToPoseOptions{
	std::string X;
	std::string Y;
	std::string Z;
	std::string A;
	std::string B;
	std::string C;
	std::string COFType;

};
//PathImp Options
// - position of end point X,Y,Z
// - Coordinate System used COFType=Imagespace/robbase;
struct PathImpOptions{
	std::string X;
	std::string Y;
	std::string Z;
	std::string COFType;

};
//Visualization Options
// - Coordinate System used COFType=Imagespace/robbase/Jointspace;
struct VisualizationOptions{
	std::string COFType;

};

//Registration Options
// - Number of pointsto register PointstoRegister;
struct RegistrationOptions{
	std::string PointstoRegister;

};

//Registration Options
// - Number of pointsto register PointstoRegister;
struct SendDataOptions{
	std::string Datatype;

};
/// \ingroup Slicer_QtModules_LightWeightRobotIGT
class Q_SLICER_MODULE_LIGHTWEIGHTROBOTIGT_WIDGETS_EXPORT qSlicerLightWeightRobotIGTFooBarWidget
  : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLightWeightRobotIGTFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerLightWeightRobotIGTFooBarWidget();
  
  RegistrationOptions RegOptions;
  VisualizationOptions VisualOptions;
  PathImpOptions PIOptions;
  MoveToPoseOptions MPOptions;
  VirtualFixturesOptions VFOptions;
  SendDataOptions SDOptions;


public slots:
  virtual void setMRMLScene(vtkMRMLScene *newScene);
  virtual void setSessionManagerNode(vtkMRMLNode *node);
  virtual void onClickGravComp();
  virtual void onClickVirtualFixtures();
  virtual void onClickIDLE();
  virtual void onClickStartVisual();
  virtual void onClickStopVisual();
  virtual void onClickShutdown();
  virtual void onClickQuit();
  virtual void onClickPathImp();
  virtual void onClickMoveToPose();
  virtual void onSelectionChangedVFx(QString);
  virtual void onSelectionChangedVFy(QString);
  virtual void onSelectionChangedVFz(QString);
  virtual void onSelectionChangedVFnx(QString);
  virtual void onSelectionChangedVFny(QString);
  virtual void onSelectionChangedVFnz(QString);
  virtual void onSelectionChangedMPx(QString);
  virtual void onSelectionChangedMPy(QString);
  virtual void onSelectionChangedMPz(QString);
  virtual void onSelectionChangedMPA(QString);
  virtual void onSelectionChangedMPB(QString);
  virtual void onSelectionChangedMPC(QString);
  virtual void onSelectionChangedPIx(QString);
  virtual void onSelectionChangedPIy(QString);
  virtual void onSelectionChangedPIz(QString);
  virtual void onValueChangedPointsRegister(int);
  virtual void onIndexChangedSendDataType(int);
  virtual void onIndexChangedVFtype(int);
  virtual void onIndexChangedVFCOFrame(int);
  virtual void onIndexChangedPICOFrame(int);
  virtual void onIndexChangedMPCOFrame(int);
  virtual void onIndexChangedVisualCOFrame(int);

protected slots:

protected:
  QScopedPointer<qSlicerLightWeightRobotIGTFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLightWeightRobotIGTFooBarWidget);
  Q_DISABLE_COPY(qSlicerLightWeightRobotIGTFooBarWidget);
};

#endif
