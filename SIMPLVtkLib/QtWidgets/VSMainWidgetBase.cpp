/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSMainWidgetBase.h"

#include <QtConcurrent>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QThread>
#include <QtCore/QUuid>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/LoadHDF5FileDialog.h"

#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSClipFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSCropFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSDataSetFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSMaskFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSParentFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSIMPLDataContainerFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSliceFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSThresholdFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSPipelineFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidgetBase::VSMainWidgetBase(QWidget* parent)
: QWidget(parent)
, m_Controller(new VSController())
{
  connectSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectSlots()
{
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*, bool)), this, SLOT(filterAdded(VSAbstractFilter*, bool)));
  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));
  connect(m_Controller, SIGNAL(blockRender(bool)), this, SLOT(setBlockRender(bool)));
  connect(m_Controller, SIGNAL(filterSelected(VSAbstractFilter*)), this, SLOT(setCurrentFilter(VSAbstractFilter*)));

  connect(this, SIGNAL(proxyFromFilePathGenerated(DataContainerArrayProxy, const QString&)), this, SLOT(launchSIMPLSelectionDialog(DataContainerArrayProxy, const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectViewWidget(VSAbstractViewWidget* viewWidget)
{
  connect(viewWidget, SIGNAL(viewWidgetCreated(VSAbstractViewWidget*)), this, SLOT(connectViewWidget(VSAbstractViewWidget*)));

  connect(viewWidget, SIGNAL(markActive(VSAbstractViewWidget*)), this, SLOT(setActiveView(VSAbstractViewWidget*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSMainWidgetBase::getController()
{
  return m_Controller;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget* VSMainWidgetBase::getActiveViewWidget()
{
  return m_ActiveViewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractViewWidget*> VSMainWidgetBase::getAllViewWidgets()
{
  return this->findChildren<VSAbstractViewWidget*>().toVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterView* VSMainWidgetBase::getFilterView()
{
  return m_FilterView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterView(VSFilterView* view)
{
  if(m_FilterView)
  {
    disconnect(m_FilterView, SIGNAL(filterClicked(VSAbstractFilter*)));
    disconnect(m_FilterView, SIGNAL(deleteFilterRequested(VSAbstractFilter*)));
    disconnect(m_FilterView, SIGNAL(reloadFilterRequested(VSAbstractDataFilter*)));
    disconnect(m_FilterView, SIGNAL(reloadFileFilterRequested(VSFileNameFilter*)));
    disconnect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(setViewWidget(VSAbstractViewWidget*)));
    disconnect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)), view, SLOT(setActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
  }

  m_FilterView = view;
  connect(view, SIGNAL(deleteFilterRequested(VSAbstractFilter*)), this, SLOT(deleteFilter(VSAbstractFilter*)));
  connect(view, SIGNAL(reloadFilterRequested(VSAbstractDataFilter*)), this, SLOT(reloadDataFilter(VSAbstractDataFilter*)));
  connect(view, SIGNAL(reloadFileFilterRequested(VSFileNameFilter*)), this, SLOT(reloadFileFilter(VSFileNameFilter*)));
  connect(view, SIGNAL(filterClicked(VSAbstractFilter*)), this, SLOT(setCurrentFilter(VSAbstractFilter*)));
  connect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), view, SLOT(setViewWidget(VSAbstractViewWidget*)));
  connect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)), view, SLOT(setActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));

  view->setViewWidget(m_ActiveViewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterSettingsWidget* VSMainWidgetBase::getFilterSettingsWidget() const
{
  return m_FilterSettingsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisibilitySettingsWidget* VSMainWidgetBase::getVisibilitySettingsWidget() const
{
  return m_VisibilitySettingsWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSColorMappingWidget* VSMainWidgetBase::getColorMappingWidget() const
{
  return m_ColorMappingWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAdvancedVisibilitySettingsWidget* VSMainWidgetBase::getAdvancedVisibilitySettingsWidget() const
{
  return m_AdvancedVisibilityWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransformWidget* VSMainWidgetBase::getTransformWidget() const
{
  return m_TransformWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterSettingsWidget(VSFilterSettingsWidget* widget)
{
  if(m_FilterSettingsWidget)
  {
    disconnect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)), m_FilterSettingsWidget, SLOT(setFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
    disconnect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), m_FilterSettingsWidget, SLOT(setViewWidget(VSAbstractViewWidget*)));
    disconnect(m_FilterSettingsWidget, SIGNAL(filterDeleted(VSAbstractFilter*)), this, SLOT(deleteFilter(VSAbstractFilter*)));
  }

  m_FilterSettingsWidget = widget;

  if(m_FilterSettingsWidget)
  {
    connect(this, SIGNAL(changedActiveFilter(VSAbstractFilter*, VSAbstractFilterWidget*)), m_FilterSettingsWidget, SLOT(setFilter(VSAbstractFilter*, VSAbstractFilterWidget*)));
    connect(this, SIGNAL(changedActiveView(VSAbstractViewWidget*)), m_FilterSettingsWidget, SLOT(setViewWidget(VSAbstractViewWidget*)));
    connect(m_FilterSettingsWidget, SIGNAL(filterDeleted(VSAbstractFilter*)), this, SLOT(deleteFilter(VSAbstractFilter*)));

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_FilterSettingsWidget->setFilter(m_CurrentFilter, filterWidget);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setVisibilitySettingsWidget(VSVisibilitySettingsWidget* widget)
{
  if(m_VisibilitySettingsWidget)
  {
    disconnect(this, &VSMainWidgetBase::changedActiveFilter, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setFilter);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setViewWidget);
  }

  m_VisibilitySettingsWidget = widget;

  if(m_VisibilitySettingsWidget)
  {
    connect(this, &VSMainWidgetBase::changedActiveFilter, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setFilter);
    connect(this, &VSMainWidgetBase::changedActiveView, m_VisibilitySettingsWidget, &VSVisibilitySettingsWidget::setViewWidget);

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_VisibilitySettingsWidget->setFilter(m_CurrentFilter, filterWidget);
    m_VisibilitySettingsWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setColorMappingWidget(VSColorMappingWidget* widget)
{
  if(m_ColorMappingWidget)
  {
    disconnect(this, &VSMainWidgetBase::changedActiveFilter, m_ColorMappingWidget, &VSColorMappingWidget::setFilter);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_ColorMappingWidget, &VSColorMappingWidget::setViewWidget);
  }

  m_ColorMappingWidget = widget;

  if(m_ColorMappingWidget)
  {
    connect(this, &VSMainWidgetBase::changedActiveFilter, m_ColorMappingWidget, &VSColorMappingWidget::setFilter);
    connect(this, &VSMainWidgetBase::changedActiveView, m_ColorMappingWidget, &VSColorMappingWidget::setViewWidget);

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_ColorMappingWidget->setFilter(m_CurrentFilter, filterWidget);
    m_ColorMappingWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setAdvancedVisibilityWidget(VSAdvancedVisibilitySettingsWidget* widget)
{
  if(m_AdvancedVisibilityWidget)
  {
    disconnect(this, &VSMainWidgetBase::changedActiveFilter, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setFilter);
    disconnect(this, &VSMainWidgetBase::changedActiveView, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setViewWidget);
  }

  m_AdvancedVisibilityWidget = widget;

  if(m_ColorMappingWidget)
  {
    connect(this, &VSMainWidgetBase::changedActiveFilter, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setFilter);
    connect(this, &VSMainWidgetBase::changedActiveView, m_AdvancedVisibilityWidget, &VSAdvancedVisibilitySettingsWidget::setViewWidget);

    VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
    m_AdvancedVisibilityWidget->setFilter(m_CurrentFilter, filterWidget);
    m_AdvancedVisibilityWidget->setViewWidget(getActiveViewWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setTransformWidget(VSTransformWidget* widget)
{
  if(m_TransformWidget)
  {
    disconnect(this, &VSMainWidgetBase::changedActiveFilter, m_TransformWidget, &VSTransformWidget::setFilter);
  }

  m_TransformWidget = widget;

  if(m_TransformWidget)
  {
    connect(this, &VSMainWidgetBase::changedActiveFilter, m_TransformWidget, &VSTransformWidget::setFilter);

    if(m_CurrentFilter)
    {
      m_TransformWidget->setTransform(m_CurrentFilter->getTransform());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setInfoWidget(VSInfoWidget* infoWidget)
{
  if(infoWidget)
  {
    setFilterSettingsWidget(infoWidget->getFilterSettingsWidget());
    setVisibilitySettingsWidget(infoWidget->getVisibilitySettingsWidget());
    setColorMappingWidget(infoWidget->getColorMappingWidget());
    setTransformWidget(infoWidget->getTransformWidget());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSMainWidgetBase::getCurrentFilter()
{
  return m_CurrentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterAdded(VSAbstractFilter* filter, bool currentFilter)
{
  QVTKInteractor* interactor = nullptr;
  VSAbstractViewWidget* activeViewWidget = getActiveViewWidget();
  if(activeViewWidget != nullptr)
  {
    VSVisualizationWidget* vizWidget = activeViewWidget->getVisualizationWidget();
    if(vizWidget != nullptr)
    {
      interactor = vizWidget->GetInteractor();
    }
  }

  VSAbstractFilterWidget* fw = nullptr;
  if(dynamic_cast<VSClipFilter*>(filter) != nullptr)
  {
    VSClipFilter* vsFilter = dynamic_cast<VSClipFilter*>(filter);
    fw = new VSClipFilterWidget(vsFilter, interactor, this);
  }
  else if(dynamic_cast<VSCropFilter*>(filter) != nullptr)
  {
    VSCropFilter* vsFilter = dynamic_cast<VSCropFilter*>(filter);
    fw = new VSCropFilterWidget(vsFilter, interactor, this);
  }
  else if(dynamic_cast<VSMaskFilter*>(filter) != nullptr)
  {
    VSMaskFilter* vsFilter = dynamic_cast<VSMaskFilter*>(filter);
    fw = new VSMaskFilterWidget(vsFilter, interactor, this);
  }
  else if(dynamic_cast<VSSIMPLDataContainerFilter*>(filter) != nullptr)
  {
    VSSIMPLDataContainerFilter* vsFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
    fw = new VSSIMPLDataContainerFilterWidget(vsFilter, this);
  }
  else if(dynamic_cast<VSDataSetFilter*>(filter) != nullptr)
  {
    VSDataSetFilter* vsFilter = dynamic_cast<VSDataSetFilter*>(filter);
    fw = new VSDataSetFilterWidget(vsFilter, this);
  }
  else if(dynamic_cast<VSSliceFilter*>(filter) != nullptr)
  {
    VSSliceFilter* vsFilter = dynamic_cast<VSSliceFilter*>(filter);
    fw = new VSSliceFilterWidget(vsFilter, interactor, this);
  }
  else if(dynamic_cast<VSThresholdFilter*>(filter) != nullptr)
  {
    VSThresholdFilter* vsFilter = dynamic_cast<VSThresholdFilter*>(filter);
    fw = new VSThresholdFilterWidget(vsFilter, interactor, this);
  }
  else if(dynamic_cast<VSFileNameFilter*>(filter) != nullptr || dynamic_cast<VSPipelineFilter*>(filter) != nullptr)
  {
    VSTextFilter* vsFilter = dynamic_cast<VSTextFilter*>(filter);
    fw = new VSParentFilterWidget(vsFilter, this);
  }

  if(fw != nullptr)
  {
    m_FilterToFilterWidgetMap.insert(filter, fw);
  }

  if(currentFilter == true)
  {
    setCurrentFilter(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::filterRemoved(VSAbstractFilter* filter)
{
  VSAbstractFilterWidget* fw = m_FilterToFilterWidgetMap.value(filter);
  delete fw;

  m_FilterToFilterWidgetMap.remove(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::importFiles(QStringList filePaths)
{
  for(int i = 0; i < filePaths.size(); i++)
  {
    QString filePath = filePaths[i];

    QMimeDatabase db;

    QMimeType mimeType = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
    QString mimeName = mimeType.name();

    QFileInfo fi(filePath);
    QString ext = fi.completeSuffix().toLower();
    if(ext == "dream3d")
    {
      openDREAM3DFile(filePath);
    }
    else if(mimeType.inherits("image/png") || mimeType.inherits("image/tiff") || mimeType.inherits("image/jpeg") || mimeType.inherits("image/bmp"))
    {
      m_Controller->importData(filePath);
    }
    else if(ext == "vtk" || ext == "vti" || ext == "vtp" || ext == "vtr" || ext == "vts" || ext == "vtu")
    {
      m_Controller->importData(filePath);
    }
    else if(ext == "stl")
    {
      m_Controller->importData(filePath);
    }
    else
    {
      QMessageBox::critical(this, "Invalid File Type",
                            tr("IMF Viewer failed to open the file because the file extension, '.%1', is not supported by the "
                               "application.")
                                .arg(ext),
                            QMessageBox::StandardButton::Ok);
      continue;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::importFilterPipeline(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_Controller->importPipelineOutput(pipeline, dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::openDREAM3DFile(const QString& filePath)
{
  QFileInfo fi(filePath);

  SIMPLH5DataReader reader;
  connect(&reader, SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SLOT(generateError(const QString&, const QString&, const int&)));

  bool success = reader.openFile(filePath);
  if(success)
  {
    int err = 0;
    SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(&req, err);
    if(proxy.dataContainers.isEmpty())
    {
      return;
    }

    QStringList dcNames = proxy.dataContainers.keys();
    for(int i = 0; i < dcNames.size(); i++)
    {
      QString dcName = dcNames[i];
      DataContainerProxy dcProxy = proxy.dataContainers[dcName];

      // We want only data containers with geometries displayed
      if(dcProxy.dcType == static_cast<unsigned int>(DataContainer::Type::Unknown))
      {
        proxy.dataContainers.remove(dcName);
      }
      else
      {
        QStringList amNames = dcProxy.attributeMatricies.keys();
        for(int j = 0; j < amNames.size(); j++)
        {
          QString amName = amNames[j];
          AttributeMatrixProxy amProxy = dcProxy.attributeMatricies[amName];

          // We want only cell attribute matrices displayed
          if(amProxy.amType != AttributeMatrix::Type::Cell)
          {
            dcProxy.attributeMatricies.remove(amName);
            proxy.dataContainers[dcName] = dcProxy;
          }
        }
      }
    }

    if(proxy.dataContainers.size() <= 0)
    {
      QMessageBox::critical(this, "Invalid Data",
                            tr("IMF Viewer failed to open file '%1' because the file does not "
                               "contain any data containers with a supported geometry.")
                                .arg(fi.fileName()),
                            QMessageBox::StandardButton::Ok);
      return;
    }

    emit proxyFromFilePathGenerated(proxy, filePath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::launchSIMPLSelectionDialog(DataContainerArrayProxy proxy, const QString& filePath)
{
  QSharedPointer<LoadHDF5FileDialog> dialog = QSharedPointer<LoadHDF5FileDialog>(new LoadHDF5FileDialog());
  dialog->setProxy(proxy);
  int ret = dialog->exec();

  if(ret == QDialog::Accepted)
  {
    SIMPLH5DataReader reader;

    bool success = reader.openFile(filePath);
    if(success)
    {
      connect(&reader, SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SLOT(generateError(const QString&, const QString&, const int&)));

      DataContainerArrayProxy dcaProxy = dialog->getDataStructureProxy();
      DataContainerArray::Pointer dca = reader.readSIMPLDataUsingProxy(dcaProxy, false);
      if(dca.get() == nullptr)
      {
        return;
      }

      m_Controller->importDataContainerArray(filePath, dca);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::generateError(const QString& title, const QString& msg, const int& code)
{
  QMessageBox::critical(this, title, msg, QMessageBox::StandardButton::Ok);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setActiveView(VSAbstractViewWidget* viewWidget)
{
  // Do nothing if the active view is not changed
  if(viewWidget == m_ActiveViewWidget)
  {
    return;
  }

  // Disconnect the old active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(false);

    disconnect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    //disconnect(m_ActiveViewWidget, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
    disconnect(m_ActiveViewWidget, SIGNAL(applyCurrentFilter()), this, SLOT(applyCurrentFilter()));
    disconnect(m_ActiveViewWidget, SIGNAL(resetCurrentFilter()), this, SLOT(resetCurrentFilter()));
  }

  m_ActiveViewWidget = viewWidget;

  // Connect the new active view widget
  if(m_ActiveViewWidget)
  {
    m_ActiveViewWidget->setActive(true);

    connect(m_ActiveViewWidget, SIGNAL(viewWidgetClosed()), this, SLOT(activeViewClosed()));
    //connect(m_ActiveViewWidget, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), this, SLOT(setFilterVisibility(VSFilterViewSettings*, bool)));
    connect(m_ActiveViewWidget, SIGNAL(applyCurrentFilter()), this, SLOT(applyCurrentFilter()));
    connect(m_ActiveViewWidget, SIGNAL(resetCurrentFilter()), this, SLOT(resetCurrentFilter()));

    // Update filter check states to match the current view widget
    //getController()->getFilterModel()->updateModelForView(viewWidget->getAllFilterViewSettings());

    VSAbstractFilterWidget* fw;
    foreach(fw, m_FilterToFilterWidgetMap.values())
    {
      fw->setInteractor(getActiveViewWidget()->getVisualizationWidget()->GetInteractor());
    }
  }

  emit changedActiveView(viewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSMainWidgetBase::setFilterVisibility(VSFilterViewSettings* viewSettings, bool visible)
//{
//  if(false == (viewSettings && viewSettings->getFilter()))
//  {
//    return;
//  }
//  if(nullptr == m_ActiveViewWidget)
//  {
//    return;
//  }
//
//  viewSettings->getFilter()->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::activeViewClosed()
{
  setActiveView(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setCurrentFilter(VSAbstractFilter* filter)
{
  VSAbstractFilterWidget* filterWidget = m_FilterToFilterWidgetMap.value(m_CurrentFilter);
  if(filterWidget)
  {
    filterWidget->setRenderingEnabled(false);
  }

  m_CurrentFilter = filter;
  filterWidget = m_FilterToFilterWidgetMap.value(filter);

  if(filterWidget)
  {
    filterWidget->setRenderingEnabled(true);
  }

  emit changedActiveFilter(m_CurrentFilter, filterWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::applyCurrentFilter()
{
  if(m_FilterSettingsWidget)
  {
    m_FilterSettingsWidget->applyFilter();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::resetCurrentFilter()
{
  if(m_FilterSettingsWidget)
  {
    m_FilterSettingsWidget->resetFilter();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::deleteFilter(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(VSAbstractViewWidget* widget : viewWidgets)
  {
    VSFilterViewSettings* viewSettings = widget->getFilterViewSettings(filter);
    bool visible = viewSettings->isVisible();
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);

    VSFilterViewSettings* parentSettings = widget->getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      bool parentVisible = parentSettings->isVisible();
      parentSettings->setVisible(visible || parentVisible);
    }
  }

  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  for(VSAbstractFilter* child : childFilters)
  {
    deleteFilter(child);
  }

  if(m_CurrentFilter == filter)
  {
    setCurrentFilter(filter->getParentFilter());
  }

  m_Controller->getFilterModel()->removeFilter(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadDataFilter(VSAbstractDataFilter* filter)
{
  std::vector<VSAbstractDataFilter*> filters;
  filters.push_back(filter);

  QtConcurrent::run(this, &VSMainWidgetBase::reloadFilters, filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadFileFilter(VSFileNameFilter* filter)
{
  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  std::vector<VSAbstractDataFilter*> filters;
  for(VSAbstractFilter* childFilter : childFilters)
  {
    VSAbstractDataFilter* dataFilter = dynamic_cast<VSAbstractDataFilter*>(childFilter);
    if(dataFilter)
    {
      filters.push_back(dataFilter);
    }
  }

  QtConcurrent::run(this, &VSMainWidgetBase::reloadFilters, filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::reloadFilters(std::vector<VSAbstractDataFilter*> filters)
{
  if(filters.size() == 1)
  {
    // This is a single filter, so do a simple reload
    VSAbstractDataFilter* filter = filters[0];
    filter->reloadData();
  }
  else if(filters.size() > 1 && dynamic_cast<VSSIMPLDataContainerFilter*>(filters[0]) != nullptr)
  {
    // This is from a file containing multiple SIMPL Data Containers, so we will use this block of code to optimize the file reading process
    QSharedPointer<SIMPLH5DataReader> reader = QSharedPointer<SIMPLH5DataReader>(new SIMPLH5DataReader());
    connect(reader.data(), SIGNAL(errorGenerated(const QString&, const QString&, const int&)), this, SLOT(generateError(const QString&, const QString&, const int&)));

    VSSIMPLDataContainerFilter* simplFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filters[0]);
    VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(simplFilter->getParentFilter());
    if(fileNameFilter == nullptr)
    {
      QString ss = QObject::tr("Data Container filters could not be reloaded because they do not have a file filter parent.");
      emit generateError("Data Reload Error", ss, -3002);
    }

    bool success = reader->openFile(fileNameFilter->getFilePath());
    if(success)
    {
      int err = 0;
      DataContainerArrayProxy dcaProxy = reader->readDataContainerArrayStructure(nullptr, err);

      for(size_t i = 0; i < filters.size(); i++)
      {
        VSSIMPLDataContainerFilter* validFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filters[i]);

        DataContainerProxy dcProxy = dcaProxy.dataContainers.value(validFilter->getFilterName());

        AttributeMatrixProxy::AMTypeFlags amFlags(AttributeMatrixProxy::AMTypeFlag::Cell_AMType);
        DataArrayProxy::PrimitiveTypeFlags pFlags(DataArrayProxy::PrimitiveTypeFlag::Any_PType);
        DataArrayProxy::CompDimsVector compDimsVector;

        dcProxy.setFlags(Qt::Checked, amFlags, pFlags, compDimsVector);
        dcaProxy.dataContainers[dcProxy.name] = dcProxy;
      }

      DataContainerArray::Pointer dca = reader->readSIMPLDataUsingProxy(dcaProxy, false);
      m_Controller->reloadDataContainerArray(fileNameFilter, dca);
    }
  }
  else
  {
    // This should not happen, so throw an error and bail!
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createClipFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSClipFilter::compatibleWithParent(parent))
  {
    VSClipFilter* filter = new VSClipFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createCropFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSCropFilter::compatibleWithParent(parent))
  {
    VSCropFilter* filter = new VSCropFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createSliceFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSSliceFilter::compatibleWithParent(parent))
  {
    VSSliceFilter* filter = new VSSliceFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createMaskFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSMaskFilter::compatibleWithParent(parent))
  {
    VSMaskFilter* filter = new VSMaskFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createThresholdFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSThresholdFilter::compatibleWithParent(parent))
  {
    VSThresholdFilter* filter = new VSThresholdFilter(parent);
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::createTextFilter(VSAbstractFilter* parent)
{
  if(nullptr == parent)
  {
    parent = m_CurrentFilter;
  }

  if(parent && VSTextFilter::compatibleWithParent(parent))
  {
    VSTextFilter* filter = new VSTextFilter(parent, "Text", "Tool Tip");
    finishAddingFilter(filter, parent);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::finishAddingFilter(VSAbstractFilter* filter, VSAbstractFilter* parent)
{
  if(nullptr == filter)
  {
    return;
  }

  m_Controller->getFilterModel()->addFilter(filter);

  // Set parent filter to invisible for the active view
  if(getActiveViewWidget() && parent)
  {
    VSFilterViewSettings* parentSettings = getActiveViewWidget()->getFilterViewSettings(parent);
    if(parentSettings)
    {
      parentSettings->setVisible(false);
      renderAllViews();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::renderActiveView()
{
  if(getActiveViewWidget())
  {
    getActiveViewWidget()->renderView();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::renderAllViews()
{
  for(VSAbstractViewWidget* viewWidget : getAllViewWidgets())
  {
    viewWidget->renderView();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setBlockRender(bool block)
{
  for(VSAbstractViewWidget* viewWidget : getAllViewWidgets())
  {
    viewWidget->setBlockRender(block);
  }
}
