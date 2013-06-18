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

/// \ingroup Slicer_QtModules_LightWeightRobotIGT
class Q_SLICER_MODULE_LIGHTWEIGHTROBOTIGT_WIDGETS_EXPORT qSlicerLightWeightRobotIGTFooBarWidget
  : public qSlicerWidget
{
  Q_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerLightWeightRobotIGTFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerLightWeightRobotIGTFooBarWidget();

public slots:
  virtual void setMRMLScene(vtkMRMLScene *newScene);
  virtual void setSessionManagerNode(vtkMRMLNode *node);
  virtual void onClickRegistration();

protected slots:

protected:
  QScopedPointer<qSlicerLightWeightRobotIGTFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLightWeightRobotIGTFooBarWidget);
  Q_DISABLE_COPY(qSlicerLightWeightRobotIGTFooBarWidget);
};

#endif
