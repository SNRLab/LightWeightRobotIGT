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

#ifndef __qSlicerLightWeightRobotIGTFooBarWidget_h
#define __qSlicerLightWeightRobotIGTFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerLightWeightRobotIGTModuleWidgetsExport.h"
#include "qSlicerWidget.h"
#include "qSlicerModulesMenu.h"


class qSlicerLightWeightRobotIGTFooBarWidgetPrivate;
class vtkMRMLNode;
// MRML includes

//Structs for the Options for the different States
//VirtualFixtures Options
// - position of the plane/cone X,Y,Z
// - normal vector of the plane/cone nX, nY, nZ
// - Virtual Fixture type VFType =plane/cone;
struct VirtualFixturesOptions{
	std::string nX;
	std::string nY;
	std::string nZ;
	std::string VFType;
	std::string phi;
	double Offset;

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
};
//PathImp Options
// - position of end point X,Y,Z
// - Coordinate System used COFType=Imagespace/robbase;
struct PathImpOptions{
	std::string X;
	std::string Y;
	std::string Z;
};


//Visualization Options
// - Coordinate System used COFType=Imagespace/robbase/Jointspace;
struct VisualizationOptions{
	std::string COFType;

};


/// \ingroup Slicer_QtModules_LightWeightRobotIGT
class Q_SLICER_MODULE_LIGHTWEIGHTROBOTIGT_WIDGETS_EXPORT qSlicerLightWeightRobotIGTFooBarWidget
  : public qSlicerWidget
{
  Q_OBJECT
public:
  //qSlicerModulesMenu* ModulesMenu;
  typedef qSlicerWidget Superclass;
  qSlicerLightWeightRobotIGTFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerLightWeightRobotIGTFooBarWidget();
  VisualizationOptions VisualOptions;
  VirtualFixturesOptions VFOptions;
  bool StartPointActive;
  bool EndPointActive;
  bool HomePointActive;
  PathImpOptions PIOptions;
  MoveToPoseOptions MPOptions;
  QScopedPointer<qSlicerLightWeightRobotIGTFooBarWidgetPrivate> d_ptr;	

public slots:
  virtual void setMRMLScene(vtkMRMLScene *newScene);
  virtual void setSessionManagerNode(vtkMRMLNode *node);
  virtual void CreateFiducial();
  virtual void onClickStartCyclic();
  virtual void OnClickLoadRobot();
  virtual void onClickGravComp();
  virtual void onClickLeadtoStart();
  virtual void onClickIDLE();
  virtual void VisualButton();
  virtual void onClickStartVisual();
  virtual void onClickStopVisual();
  virtual void onClickShutdown();
  virtual void onClickMoveToEntrancePoint();
  virtual void onClickMoveToTargetPoint();
  virtual void OnClickShowTCPForce();
  virtual void onSelectionChangedVFphi(QString);
  virtual void onIndexChangedVFtype(int);
  virtual void onClickSetEndPoint();
  virtual void onClickSetStartPoint();
  virtual void onClickBackToStart();
  virtual void onClickFiducialRegistration();
  virtual void onCheckStatusChangedVFPreview(bool checked);
  virtual void onCheckStatusChangedAFC(bool checked);

protected slots:

protected:


private:
  Q_DECLARE_PRIVATE(qSlicerLightWeightRobotIGTFooBarWidget);
  Q_DISABLE_COPY(qSlicerLightWeightRobotIGTFooBarWidget);
};

#endif
