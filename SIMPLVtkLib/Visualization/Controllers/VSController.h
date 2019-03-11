/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#pragma once

#include <QtCore/QItemSelectionModel>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/QtWidgets/VSAbstractImporter.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSConcurrentImport.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class ImportMontageWizard;
class ExecutePipelineWizard;
class MontageWorker;
class PipelineWorker;
class VSDataSetFilter;

/**
 * @class VSController VSController.h SIMPLVtkLib/Visualization/Controllers/VSController.h
 * @brief This class handles the top-level controls for SIMPLVtkLib from importing data
 * to updating VisualFilters for rendering data across multiple views.  The VisualFilter
 * tree is stored within the class so that it can be readily accessed or modified from
 * inside or outside the library.
 */
class SIMPLVtkLib_EXPORT VSController : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSController(QObject* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSController();

  /**
   * @brief Saves the session to the file at sessionFilePath
   * @param sessionFilePath
   * @param instance
   * @return
   */
  bool saveSession(const QString& sessionFilePath);

  /**
   * @brief Loads the session stored in the file at sessionFilePath
   * @param sessionFilePath
   * @param instance
   * @return
   */
  bool loadSession(const QString& sessionFilePath);

  /**
   * @brief importMontage
   * @param montageWizard
   * @return
   */
  void importMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief Import data from a DataContainerArray and add any relevant DataContainers
   * as top-level VisualFilters
   * @param dca
   */
  void importDataContainerArray(QString filePath, DataContainerArray::Pointer dca);

  /**
   * @brief Import a pipeline to execute
   * @param executePipelineWizard
   */
  void importPipeline(ExecutePipelineWizard* executePipelineWizard);

  /**
   * @brief Import data from a FilterPipeline and add any relevant DataContainers as top-level VisualFilters
   * @param pipeline
   * @param dca
   */
  void importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca);

  /**
   * @brief Import data from a FilterPipeline and add any relevant DataContainers as top-level VisualFilters
   * @param pipelines
   */
  void importPipelineOutput(std::vector<FilterPipeline::Pointer> pipelines);

  /**
   * @brief Import data from a FilterPipeline and add any relevant DataContainers as top-level VisualFilters
   * @param pipeline
   * @param dca
   */
  void reloadPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca);

  /**
   * @brief Import data from a DataContainerArray and add any relevant DataContainers
   * as top-level VisualFilters
   * @param fileFilter
   * @param dca
   */
  void reloadDataContainerArray(VSFileNameFilter* fileFilter, DataContainerArray::Pointer dca);

  /**
   * @brief Import data from a DataContainerArray and add any relevant DataContainers
   * as top-level VisualFilters
   * @param dca
   */
  void importDataContainerArray(DataContainerArray::Pointer dca);

  /**
   * @brief Import data from a DataContainer and add a top-level VisualFilter if there
   * is relevant data for visualization
   * @param dc
   */
  void importDataContainer(DataContainer::Pointer dc);

  /**
   * @brief Import data from a file
   * @param filePath
   */
  void importData(const QString& filePath);

  /**
   * @brief Returns the first top level text filter with the given value;
   * @param text
   * @return
   */
  VSFileNameFilter* getBaseFileNameFilter(QString text);

  /**
   * @brief Returns a vector of top-level data filters
   * @return
   */
  VSAbstractFilter::FilterListType getBaseFilters();

  /**
   * @brief Returns a vector of all visual filters
   * @return
   */
  VSAbstractFilter::FilterListType getAllFilters();

  /**
   * @brief Return the filter model used
   * @return
   */
  VSFilterModel* getFilterModel();

  /**
   * @brief Alert the VSMainWidgetBase, if available, that the given filter should be selected.
   * @param filter
   */
  //void selectFilter(VSAbstractFilter* filter);

protected slots:
  /**
   * @brief handleDatasetResults
   * @param textFilter
   * @param filter
   */
  void handleDatasetResults(VSFileNameFilter* textFilter, VSDataSetFilter* filter);

  /**
   * @brief handleMontageResults
   * @param pipeline
   * @param err
   */
  void handleMontageResults(FilterPipeline::Pointer pipeline, int err);

  /**
   * @brief workerFinished
   */
  void montageWorkerFinished();

  /**
   * @brief montageThreadFinished
   */
  void montageThreadFinished();

  /**
   * @brief workerFinished
   */
  void pipelineWorkerFinished();

  /**
   * @brief montageThreadFinished
   */
  void pipelineThreadFinished();

signals:
  void filterAdded(VSAbstractFilter*, bool currentFilter);
  void filterRemoved(VSAbstractFilter*);
  void filterCheckStateChanged(VSAbstractFilter* filter);
  void importerAddedToQueue(const QString &name, VSAbstractImporter::Pointer importer);
  void blockRender(bool block);
  void dataImported();
  //void filterSelected(VSAbstractFilter* filter);
  void applyingDataFilters(int count);
  void dataFilterApplied(int num);
  void notifyErrorMessage(const QString &msg, int code);
  void notifyStatusMessage(const QString &msg);

private:
  VSFilterModel* m_FilterModel;
  VSConcurrentImport* m_ImportObject;

  QThread* m_ImportDataWorkerThread = nullptr;
  QThread* m_PipelineWorkerThread = nullptr;
  MontageWorker* m_ImportDataWorker = nullptr;
  PipelineWorker* m_PipelineWorker = nullptr;

  std::vector<FilterPipeline::Pointer> m_Pipelines;
  DataContainerArray::Pointer m_dataContainerArray;
  bool m_DisplayMontage = false;
  bool m_DisplayOutline = false;

  /**
   * @brief getImportDataWorker
   * @return
   */
  MontageWorker* getImportDataWorker();

  /**
   * @brief saveFilter
   * @param filter
   * @param obj
   */
  void saveFilter(VSAbstractFilter* filter, QJsonObject& obj);

  /**
   * @brief loadFilter
   * @param filterObj
   */
  void loadFilter(QJsonObject& obj, VSAbstractFilter* parentFilter = nullptr);

  /**
   * @brief importGenericMontage
   * @param montageWizard
   */
  void importGenericMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief importDREAM3DMontage
   * @param montageWizard
   */
  void importDREAM3DMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief importFijiMontage
   * @param montageWizard
   */
  void importFijiMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief importRobometMontage
   * @param montageWizard
   */
  void importRobometMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief importZeissMontage
   * @param montageWizard
   */
  void importZeissMontage(ImportMontageWizard* montageWizard);

  /**
   * @brief runPipeline
   * @param pipeline
   */
  void addMontagePipelineToQueue(FilterPipeline::Pointer pipeline);

  /**
   * @brief executePipeline
   * @param pipeline
   */
  void executePipeline(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer);
};
