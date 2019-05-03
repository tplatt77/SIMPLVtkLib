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

#include "VSConcurrentImport.h"

#include <QtConcurrent>
#include <QtCore/QFutureWatcher>

#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSConcurrentImport::VSConcurrentImport(VSController* controller)
: QObject(controller)
, m_Controller(controller)
, m_ImportDataContainerOrderLock(1)
, m_UnappliedDataFilterLock(1)
, m_AppliedDataFilterLock(1)
, m_FilterLock(1)
, m_WrappedDcLock(1)
, m_ThreadCountLock(1)
, m_AppliedFilterCountLock(1)
{
  if(controller && controller->getFilterModel())
  {
    connect(this, SIGNAL(importedFilter(VSAbstractFilter*, bool)), controller->getFilterModel(), SLOT(addFilter(VSAbstractFilter*, bool)));
  }

  connect(this, SIGNAL(finishedPartialWrapping()), this, SLOT(partialWrappingThreadFinished()));
  connect(this, SIGNAL(finishedApplying()), this, SLOT(applyDataFiltersThreadFinished()));

  int threadsUsed = 2;
  m_ThreadCount = QThreadPool::globalInstance()->maxThreadCount();
  if(m_ThreadCount > threadsUsed)
  {
    m_ThreadCount -= threadsUsed;
  }
  else
  {
    m_ThreadCount = 1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  VSPipelineFilter* pipelineFilter = new VSPipelineFilter(pipeline);
  addDataContainerArray(pipelineFilter, dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(VSPipelineFilter* pipelineFilter, DataContainerArray::Pointer dca)
{
  addDataContainerArray(std::make_pair(pipelineFilter, dca));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(QString filePath, DataContainerArray::Pointer dca)
{
  VSFileNameFilter* fileFilter = new VSFileNameFilter(filePath);
  addDataContainerArray(fileFilter, dca);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(VSFileNameFilter* fileFilter, DataContainerArray::Pointer dca)
{
  addDataContainerArray(std::make_pair(fileFilter, dca));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::addDataContainerArray(DcaGenericPair fileDcPair)
{
  m_WrappedList.push_back(fileDcPair);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::run()
{
  while(m_WrappedList.size() > 0)
  {
    DcaGenericPair filePair = m_WrappedList.front();
    importDataContainerArray(filePair);

    m_WrappedList.pop_front();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::importDataContainerArray(DcaGenericPair genericPair)
{
  m_DataParentFilter = genericPair.first;
  DataContainerArray::Pointer dca = genericPair.second;

  VSFilterModel* filterModel = m_Controller->getFilterModel();

  auto currentFilters = filterModel->getAllFilters();
  bool parentFilterFound = (std::find(currentFilters.begin(), currentFilters.end(), m_DataParentFilter) != currentFilters.end());
  if (!parentFilterFound)
  {
    filterModel->addFilter(m_DataParentFilter);
  }

  m_ImportDataContainerQueue = dca->getDataContainers();

  for (int i = 0; i < m_ImportDataContainerQueue.size(); i++)
  {
    DataContainer::Pointer dc = m_ImportDataContainerQueue[i];
    m_DataContainerIndexMap.insert(dc, i);
  }

  m_WrappedDataContainers.resize(m_ImportDataContainerQueue.size());
  m_UnappliedDataFilters.resize(m_ImportDataContainerQueue.size());
  m_AppliedDataFilters.resize(m_ImportDataContainerQueue.size());

  emit blockRender(true);

  // Wait for the filter lock
  m_FilterLock.acquire();

  m_ThreadCountLock.acquire();
  m_ThreadsRemaining = m_ThreadCount;
  m_ThreadCountLock.release();
  for(int i = 0; i < m_ThreadCount; i++)
  {
    QtConcurrent::run(this, &VSConcurrentImport::wrapDataContainer);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::partialWrappingThreadFinished()
{
  // Threads remaining lock
  m_ThreadCountLock.acquire();
  m_ThreadsRemaining--;
  if(m_ThreadsRemaining <= 0)
  {
    m_ThreadCountLock.release();

    for(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc : m_WrappedDataContainers)
    {
      VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedDc, m_DataParentFilter);
		DataContainer::Pointer dataContainer = wrappedDc->m_DataContainer;
		ImageGeom::Pointer imageGeom = dataContainer->getGeometryAs<ImageGeom>();
    FloatVec3Type originTuple = imageGeom->getOrigin();
		double origin[3];
    origin[0] = originTuple[0];
    origin[1] = originTuple[1];
    origin[2] = originTuple[2];

		filter->getTransform()->setLocalPosition(origin);
		filter->getTransform()->setOriginPosition(origin);

    // Attempting to run applyDataFilters requires the QSemaphore to lock when modifying this vector
    m_UnappliedDataFilterLock.acquire();
    int index = m_DataContainerIndexMap.value(filter->getWrappedDataContainer()->m_DataContainer);
    m_UnappliedDataFilters[index] = filter;
    m_UnappliedDataFilterLock.release();
    }

    m_AppliedFilterCountLock.acquire();
    m_AppliedFilterCount = 0;
    m_AppliedFilterCountLock.release();
    m_WrappedDataContainers.clear();
    emit blockRender(false);
    m_FilterLock.release();

    m_UnappliedDataFilterLock.acquire();
    emit applyingDataFilters(m_UnappliedDataFilters.size());
    m_UnappliedDataFilterLock.release();

    // Apply the filters only after all DataContainers for all files have been wrapped
    if(m_WrappedList.size() == 0)
    {
      // Apply filters on multiple threads
      for(int i = 0; i < m_ThreadCount; i++)
      {
        QFuture<void> future = QtConcurrent::run(this, &VSConcurrentImport::applyDataFilters);
      }
    }
  }
  m_ThreadCountLock.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::wrapDataContainer()
{
  m_ImportDataContainerOrderLock.acquire();
  while(m_ImportDataContainerQueue.size() > 0)
  {
    DataContainer::Pointer dc = m_ImportDataContainerQueue.front();
    m_ImportDataContainerQueue.erase(m_ImportDataContainerQueue.begin());
    m_ImportDataContainerOrderLock.release();

    SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc = SIMPLVtkBridge::WrapGeometryPtr(dc);
    if(wrappedDc)
    {
      m_WrappedDcLock.acquire();
      int index = m_DataContainerIndexMap.value(dc);
      m_WrappedDataContainers[index] = wrappedDc;
      m_WrappedDcLock.release();
    }

    // Lock the semaphore before the while loop checks
    m_ImportDataContainerOrderLock.acquire();
  }
  m_ImportDataContainerOrderLock.release();

  emit finishedPartialWrapping();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::applyDataFilters()
{
  m_UnappliedDataFilterLock.acquire();
  while(m_UnappliedDataFilters.size() > 0)
  {
    VSSIMPLDataContainerFilter* filter = m_UnappliedDataFilters.front();
    m_UnappliedDataFilters.erase(m_UnappliedDataFilters.begin());
    m_UnappliedDataFilterLock.release();

    filter->finishWrapping();

    m_AppliedFilterCountLock.acquire();
    emit dataFilterApplied(++m_AppliedFilterCount);
    m_AppliedFilterCountLock.release();

    m_AppliedDataFilterLock.acquire();
    int index = m_DataContainerIndexMap.value(filter->getWrappedDataContainer()->m_DataContainer);
    m_AppliedDataFilters[index] = filter;
    m_AppliedDataFilterLock.release();

    // Lock semaphore before the while statement is checked again
    m_UnappliedDataFilterLock.acquire();
  }
  m_UnappliedDataFilterLock.release();

  emit finishedApplying();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSConcurrentImport::applyDataFiltersThreadFinished()
{
  // Threads remaining lock
  m_ThreadCountLock.acquire();
  m_ThreadsRemaining--;
  if(m_ThreadsRemaining <= 0)
  {
    m_ThreadCountLock.release();

    for (VSSIMPLDataContainerFilter* filter : m_AppliedDataFilters)
    {
      auto currentFilters = m_Controller->getFilterModel()->getAllFilters();
      bool filterFound = (std::find(currentFilters.begin(), currentFilters.end(), m_DataParentFilter) != currentFilters.end());
      if (!filterFound)
      {
        m_Controller->getFilterModel()->addFilter(filter, false);
      }
    }

    m_AppliedDataFilters.clear();
  }
  m_ThreadCountLock.release();
}
