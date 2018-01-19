/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VtkVisualizationWidget.h"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QVector>

#include <vtkAutoInit.h>
#include <vtkInteractionStyleModule.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <vtkAxesActor.h>
#include <vtkBMPWriter.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkJPEGWriter.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPNGWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "VtkSIMPL/QtWidgets/VtkMappingData.h"
#include "VtkSIMPL/QtWidgets/VtkToggleWidget.h"
#include "VtkSIMPL/VtkFilters/VtkAbstractFilter.h"
#include "VtkSIMPL/VtkFilters/VtkClipFilter.h"
#include "VtkSIMPL/VtkFilters/VtkDataSetFilter.h"
#include "VtkSIMPL/VtkFilters/VtkMaskFilter.h"
#include "VtkSIMPL/VtkFilters/VtkSliceFilter.h"


VtkVisualizationWidget* VtkVisualizationWidget::self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkVisualizationWidget::VtkVisualizationWidget(QWidget* parent)
: QWidget(parent)
, m_EditFiltersWidget(nullptr)
, m_PopUpWidget(new PopUpWidget())
, m_VtkRenderController(nullptr)
, m_DataContainerArray(nullptr)
, m_Renderer(nullptr)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkVisualizationWidget::~VtkVisualizationWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkVisualizationWidget* VtkVisualizationWidget::Instance()
{
  if(nullptr == self)
  {
    self = new VtkVisualizationWidget(nullptr);
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::addVisualizationFilter(VtkAbstractFilter* filter)
{
  filter->setInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());
  m_VtkRenderController->addFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::setupGui()
{
  initializeRendererAndAxes();

  m_EditFiltersWidget = new VtkEditFiltersWidget(nullptr);
  m_EditFiltersWidget->setRenderWindowInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());
  m_EditFiltersWidget->hide();

  m_PopUpWidget->setWidget(m_EditFiltersWidget);
  m_PopUpWidget->hide();

  connect(editFiltersBtn, SIGNAL(clicked()), this, SLOT(displayEditFiltersWidget()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::setDataContainerArray(DataContainerArray::Pointer dca)
{
  cleanupVisualizationWindow();

  if(nullptr == dca)
  {
    return;
  }

  m_DataContainerArray = dca;

  if(m_VtkRenderController)
  {
    clearRenderWindow();
    m_VtkRenderController->clearFilters();
  }

  m_VtkRenderController = SIMPLVtkBridge::WrapDataContainerArrayAsVtkRenderController(m_DataContainerArray);
  m_VtkRenderController->setVtkRenderControllerRenderer(m_Renderer);
  m_EditFiltersWidget->setRenderController(m_VtkRenderController);

  setupDataComboBoxes();

  update();
  resetCamera();
  show();
  this->setVisible(true);
  qvtkWidget->GetRenderWindow()->Render();

  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// void VtkVisualizationWidget::setDataArrayPath(DataArrayPath path);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::filterExecuted(AbstractFilter* filter)
{
#if 0
  m_ActiveVisualizationFilter = dynamic_cast<VtkVisualizationFilter*>(filter);
  if(m_ActiveVisualizationFilter)
  {
    setWindowTitle("[" + QString::number(filter->getPipelineIndex() + 1) + "] " + filter->getHumanLabel());
    m_LocalDataContainerArray = m_ActiveVisualizationFilter->getDataContainerArray();

		VtkAbstractFilter::setInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());

		m_VtkRenderController = SIMPLVtkBridge::WrapDataContainerArrayAsVtkRenderController(m_LocalDataContainerArray);
    m_VtkRenderController->setVtkRenderControllerRenderer(m_Renderer);
    
		contextItem = nullptr;

		setupFilterTreeView();
		setupDataComboBoxes();
		createActions();

    render();
    show();
    this->setVisible(true);

    if(m_mappingData != nullptr)
    {
      toggleRenderProperties->setWidget(m_mappingData);
    }

		scrollAreaWidgetContents->adjustSize();

		connectSlots();
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::connectSlots()
{
  // Add VTK Filter
  connect(clipBtn, SIGNAL(clicked()), 
    this, SLOT(addClipFilter()));
  connect(sliceBtn, SIGNAL(clicked()), 
    this, SLOT(addSliceFilter()));
  connect(maskBtn, SIGNAL(clicked()), 
    this, SLOT(addMaskFilter()));

  connect(viewScalarComboBox, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(changeDataView(int)));
  connect(viewScalarComponentComboBox, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(changeDataViewComponent(int)));

  connect(m_EditFiltersWidget, SIGNAL(renderVtk()),
    this, SLOT(renderVtk()), Qt::UniqueConnection);
  connect(m_EditFiltersWidget, SIGNAL(activeFilterChanged(VtkAbstractFilter*)),
    this, SLOT(setActiveFilter(VtkAbstractFilter*)), Qt::UniqueConnection);

  // Mapping data pass-through
  connect(m_EditFiltersWidget, SIGNAL(mapFilterScalars(int)),
    this, SLOT(mapFilterScalars(int)));
  connect(m_EditFiltersWidget, SIGNAL(showLookupTable(int)),
    this, SLOT(showLookupTable(int)));
  connect(m_EditFiltersWidget, SIGNAL(checkScalarMapping()),
    this, SLOT(checkScalarMapping()));

  connect(saveAsBtn, SIGNAL(clicked()), this, SLOT(saveAs()));

  connect(camXPlusBtn, SIGNAL(clicked()), this, SLOT(camXPlus()));
  connect(camXMinusBtn, SIGNAL(clicked()), this, SLOT(camXMinus()));

  connect(camYPlusBtn, SIGNAL(clicked()), this, SLOT(camYPlus()));
  connect(camYMinusBtn, SIGNAL(clicked()), this, SLOT(camYMinus()));

  connect(camZPlusBtn, SIGNAL(clicked()), this, SLOT(camZPlus()));
  connect(camZMinusBtn, SIGNAL(clicked()), this, SLOT(camZMinus()));

  connect(showAxisBtn, SIGNAL(clicked()), this, SLOT(checkAxisWidget()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::disconnectSlots()
{
  // Add VTK Filter
  disconnect(clipBtn, SIGNAL(clicked()), 
    this, SLOT(addClipFilter()));
  disconnect(sliceBtn, SIGNAL(clicked()), 
    this, SLOT(addSliceFilter()));
  disconnect(maskBtn, SIGNAL(clicked()), 
    this, SLOT(addMaskFilter()));

  disconnect(m_EditFiltersWidget, SIGNAL(renderVtk()),
    this, SLOT(renderVtk()));
  disconnect(m_EditFiltersWidget, SIGNAL(activeFilterChanged(VtkAbstractFilter*)),
    this, SLOT(setActiveFilter(VtkAbstractFilter*)));

  disconnect(viewScalarComboBox, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(changeDataView(int)));
  disconnect(viewScalarComponentComboBox, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(changeDataViewComponent(int)));

  // Mapping data pass-through
  disconnect(m_EditFiltersWidget, SIGNAL(mapFilterScalars(int)),
    this, SLOT(mapFilterScalars(int)));
  disconnect(m_EditFiltersWidget, SIGNAL(showLookupTable(int)),
    this, SLOT(showLookupTable(int)));
  disconnect(m_EditFiltersWidget, SIGNAL(checkScalarMapping()),
    this, SLOT(checkScalarMapping()));

  disconnect(saveAsBtn, SIGNAL(clicked()), this, SLOT(saveAs()));

  disconnect(camXPlusBtn, SIGNAL(clicked()), this, SLOT(camXPlus()));
  disconnect(camXMinusBtn, SIGNAL(clicked()), this, SLOT(camXMinus()));

  disconnect(camYPlusBtn, SIGNAL(clicked()), this, SLOT(camYPlus()));
  disconnect(camYMinusBtn, SIGNAL(clicked()), this, SLOT(camYMinus()));

  disconnect(camZPlusBtn, SIGNAL(clicked()), this, SLOT(camZPlus()));
  disconnect(camZMinusBtn, SIGNAL(clicked()), this, SLOT(camZMinus()));

  disconnect(showAxisBtn, SIGNAL(clicked()), this, SLOT(checkAxisWidget()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::initializeRendererAndAxes()
{
  m_Renderer = vtkSmartPointer<vtkRenderer>::New();

  double* bgColor = new double[3]{0.3, 0.3, 0.35};
  m_Renderer->SetBackground(bgColor);

  // QVTKOpenGLWidget does not create a render window automatically
  vtkGenericOpenGLRenderWindow* ren = vtkGenericOpenGLRenderWindow::New();
  qvtkWidget->SetRenderWindow(ren);
  ren->Finalize();
  ren->SetMapped(1);
  ren->SetSize(width(), height());
  ren->SetPosition(x(), y());

  QVTKInteractor* iren = QVTKInteractor::New();
  iren->SetUseTDx(false);
  ren->SetInteractor(iren);
  iren->Initialize();

  vtkInteractorStyle* style = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(style);

  iren->Delete();
  style->Delete();

  qvtkWidget->GetRenderWindow()->AddRenderer(m_Renderer);

  VTK_NEW(vtkAxesActor, axes);

  m_OrientationWidget = vtkOrientationMarkerWidget::New();
  m_OrientationWidget->SetOutlineColor(0.93, 0.57, 0.13);
  m_OrientationWidget->SetOrientationMarker(axes);
  m_OrientationWidget->SetInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());
  m_OrientationWidget->SetEnabled(1);
  m_OrientationWidget->InteractiveOff();

  showAxisBtn->setChecked(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::cleanupVisualizationWindow()
{
  disconnectSlots();
  clearRenderWindow();

  m_EditFiltersWidget->cleanupVisualization();

  if(nullptr != m_VtkRenderController)
  {
    m_VtkRenderController->clearFilters();
  }

  m_VtkRenderController = VtkRenderController::NullPointer();
  m_DataContainerArray = DataContainerArray::NullPointer();
  m_ActiveVisualizationFilter = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::clearRenderWindow()
{
  m_Renderer->RemoveAllViewProps();
  m_Renderer->Render();
  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
  m_Renderer->ResetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::setupDataComboBoxes()
{
  if(!m_VtkRenderController)
  {
    return;
  }

  VtkRenderController::VtkDataStructVector_t vtkDataStructs = m_VtkRenderController->getVtkDataStructs();

  if(vtkDataStructs.length() > 0)
  {
    viewScalarComboBox->setCurrentIndex(0);
    changeDataSet(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::setActiveFilter(VtkAbstractFilter* filter)
{
  if(nullptr == m_VtkRenderController || nullptr == filter)
  {
    return;
  }

  // update UI
  clipBtn->setEnabled(VtkAbstractFilter::compatibleInput(filter->getOutputType(), VtkClipFilter::getRequiredInputType()));
  sliceBtn->setEnabled(VtkAbstractFilter::compatibleInput(filter->getOutputType(), VtkSliceFilter::getRequiredInputType()));

  viewScalarComboBox->setCurrentIndex(filter->getViewScalarId());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::changeDataSet(int setId)
{
  std::shared_ptr<VtkRenderController::VtkDataSetStruct_t> dataSet = m_VtkRenderController->getActiveDataSet();
  if(nullptr == dataSet)
  {
    return;
  }

  QList<QString> dataViewNameList;

  for(int i = 0; i < dataSet->ViewOptions.length(); i++)
  {
    dataViewNameList.push_back(dataSet->ViewOptions.at(i)->Name);
  }

  viewScalarComboBox->clear();
  viewScalarComboBox->addItems(dataViewNameList);

  if(dataSet->ViewOptions.length() > 0)
  {
    m_VtkRenderController->setDataViewActive(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::changeDataView(int viewId)
{
  if(nullptr == m_VtkRenderController || viewId < 0)
  {
    return;
  }

  VtkAbstractFilter* activeFilter = m_VtkRenderController->getActiveFilter();

  if(nullptr == activeFilter)
  {
    return;
  }

  m_VtkRenderController->setActiveFilterScalars(viewId);
  setViewComponentComboBox(activeFilter->getBaseDataArray(viewId));

  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::changeDataViewComponent(int componentId)
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  VtkAbstractFilter* activeFilter = m_VtkRenderController->getActiveFilter();

  if(nullptr == activeFilter)
  {
    return;
  }

  m_VtkRenderController->setActiveFilterComponent(componentId);

  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
  qvtkWidget->GetRenderWindow()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::setViewComponentComboBox(vtkDataArray* dataArray)
{
  if(nullptr == dataArray)
  {
    return;
  }

  int numComponents = dataArray->GetNumberOfComponents();

  viewScalarComponentComboBox->clear();
  viewScalarComponentComboBox->setEnabled(numComponents > 1);

  if(numComponents > 1)
  {
    QList<QString> componentNameList;

    for(int i = 0; i < numComponents; i++)
    {
      if(dataArray->GetComponentName(i) != nullptr)
      {
        componentNameList.append(dataArray->GetComponentName(i));
      }
      else
      {
        componentNameList.append(QString::number(i + 1));
      }
    }

    componentNameList.append("Magnitude");

    viewScalarComponentComboBox->addItems(componentNameList);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::mapFilterScalars(int state)
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  VtkAbstractFilter* activeFilter = m_VtkRenderController->getActiveFilter();

  if(nullptr == activeFilter)
  {
    return;
  }

  bool shouldMap = state == Qt::Checked;

  activeFilter->setMapScalars(shouldMap);

  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::showLookupTable(int state)
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  VtkAbstractFilter* activeFilter = m_VtkRenderController->getActiveFilter();

  if(nullptr == activeFilter)
  {
    return;
  }

  bool showTable = state == Qt::Checked;

  activeFilter->setLookupTableActive(showTable);

  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::resetCamera()
{
  if(!m_VtkRenderController)
  {
    return;
  }

  m_Renderer->ResetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::renderVtk()
{
  if(!m_VtkRenderController)
  {
    return;
  }

  qvtkWidget->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::displayEditFiltersWidget()
{
  QPoint targetPoint = editFiltersBtn->mapToGlobal(QPoint(0, editFiltersBtn->geometry().height()));

  m_PopUpWidget->setWidget(m_EditFiltersWidget);
  m_PopUpWidget->resize(m_EditFiltersWidget->sizeHint());
  m_PopUpWidget->move(targetPoint);
  m_PopUpWidget->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::addClipFilter()
{
  m_EditFiltersWidget->addClipFilter();
  displayEditFiltersWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::addSliceFilter()
{
  m_EditFiltersWidget->addSliceFilter();
  displayEditFiltersWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::addMaskFilter()
{
  m_EditFiltersWidget->addMaskFilter();
  displayEditFiltersWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::saveAs()
{
  QString filter = "VTK Data Object (*.vtk);;PNG Image Data (*.png);;JPEG Image Data (*jpg);;Bitmap Image Data(*bmp)";

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), filter);

  if(fileName.isEmpty())
  {
    return;
  }

  if(fileName.endsWith(".vtk"))
  {
    m_VtkRenderController->getActiveFilter()->saveFile(fileName);
  }
  else if(fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".bmp"))
  {
    saveScreenshot(fileName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::saveAsVTK()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), tr("VTK Data Object (*.vtk)"));

  if(fileName.isEmpty())
  {
    return;
  }

  m_VtkRenderController->getActiveFilter()->saveFile(fileName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::saveScreenshot(QString fileName)
{
  VTK_NEW(vtkWindowToImageFilter, screenshotFilter);
  screenshotFilter->SetInput(m_Renderer->GetRenderWindow());
  screenshotFilter->SetMagnification(1);
  screenshotFilter->SetInputBufferTypeToRGBA();
  screenshotFilter->ReadFrontBufferOff();
  screenshotFilter->Update();

  VTK_PTR(vtkImageWriter) imageWriter;

  if(fileName.endsWith(".png"))
  {
    imageWriter = VTK_PTR(vtkPNGWriter)::New();
  }
  else if(fileName.endsWith(".jpg"))
  {
    imageWriter = VTK_PTR(vtkJPEGWriter)::New();
  }
  else if(fileName.endsWith(".bmp"))
  {
    imageWriter = VTK_PTR(vtkBMPWriter)::New();
  }
  else
  {
    return;
  }

  imageWriter->SetFileName(fileName.toStdString().c_str());
  imageWriter->SetInputConnection(screenshotFilter->GetOutputPort());
  imageWriter->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camXPlus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraXPlus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camYPlus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraYPlus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camZPlus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraZPlus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camXMinus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraXMinus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camYMinus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraYMinus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::camZMinus()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->setCameraZMinus();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::checkScalarMapping()
{
  if(nullptr == m_VtkRenderController)
  {
    return;
  }

  m_VtkRenderController->checkScalarBarWidgets();
  m_Renderer->GetRenderWindow()->GetInteractor()->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkVisualizationWidget::checkAxisWidget()
{
  m_OrientationWidget->SetEnabled(showAxisBtn->isChecked());
  renderVtk();
}
