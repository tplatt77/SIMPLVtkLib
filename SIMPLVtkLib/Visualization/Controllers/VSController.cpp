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

#include "VSController.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/QtWidgets/VSFilterFactory.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

#include "SIMPLVtkLib/Wizards/ImportMontage/FijiListWidget.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/ImportMontageConstants.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/ImportMontageWizard.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/MontageWorker.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/RobometListWidget.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/TileConfigFileGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
: QObject(parent)
, m_FilterModel(new VSFilterModel())
{
  m_ImportObject = new VSConcurrentImport(this);

  m_WorkerThread = new QThread;
  m_MontageWorker = new MontageWorker();
  m_MontageWorker->moveToThread(m_WorkerThread);

  connect(m_WorkerThread, SIGNAL(started()), m_MontageWorker, SLOT(process()));
//  connect(m_MontageWorker, SIGNAL(finished()), m_WorkerThread, SLOT(quit()));
//  connect(m_MontageWorker, SIGNAL(finished()), m_MontageWorker, SLOT(deleteLater()));
//  connect(m_WorkerThread, SIGNAL(finished()), m_WorkerThread, SLOT(deleteLater()));
  connect(m_MontageWorker, &MontageWorker::resultReady, this, &VSController::handleMontageResults);

  connect(m_FilterModel, &VSFilterModel::filterAdded, this, &VSController::filterAdded);
  connect(m_FilterModel, &VSFilterModel::filterRemoved, this, &VSController::filterRemoved);

  // VSConcurrentImport works on another thread, so use the old-style connections to forward signals on the current thread
  connect(m_ImportObject, SIGNAL(blockRender(bool)), this, SIGNAL(blockRender(bool)));
  connect(m_ImportObject, SIGNAL(applyingDataFilters(int)), this, SIGNAL(applyingDataFilters(int)));
  connect(m_ImportObject, SIGNAL(dataFilterApplied(int)), this, SIGNAL(dataFilterApplied(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::~VSController()
{
  delete m_FilterModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::importMontage(ImportMontageWizard* montageWizard)
{
  m_DisplayMontage = montageWizard->field(ImportMontage::FieldNames::DisplayMontage).toBool();
  ImportMontageWizard::InputType inputType = montageWizard->field(ImportMontage::FieldNames::InputType).value<ImportMontageWizard::InputType>();

  // Based on the type of file imported, perform next action
  if(inputType == ImportMontageWizard::InputType::Generic)
  {
    importGenericMontage(montageWizard);
  }
  else if(inputType == ImportMontageWizard::InputType::DREAM3D)
  {
    importDREAM3DMontage(montageWizard);
  }
  else if(inputType == ImportMontageWizard::InputType::Fiji)
  {
    importFijiMontage(montageWizard);
  }
  else if(inputType == ImportMontageWizard::InputType::Robomet)
  {
    importRobometMontage(montageWizard);
  }
  else if(inputType == ImportMontageWizard::InputType::Zeiss)
  {
    importZeissMontage(montageWizard);
  }
  else
  {
    QMessageBox::critical(this, "Invalid File Type",
                          tr("IMF Viewer failed to detect the proper data file type from the given input file.  "
                             "Supported file types are DREAM3D, VTK, STL and Image files, as well as Fiji and Robomet configuration files."),
                          QMessageBox::StandardButton::Ok);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importGenericMontage(ImportMontageWizard* montageWizard)
{
  QString tileConfigFile = "TileConfiguration.txt";
  int numOfRows = montageWizard->field(ImportMontage::Generic::FieldNames::NumberOfRows).toInt();
  int numOfCols = montageWizard->field(ImportMontage::Generic::FieldNames::NumberOfColumns).toInt();

  // Get input file names
  FileListInfo_t inputFileInfo = montageWizard->field(ImportMontage::Generic::FieldNames::FileListInfo).value<FileListInfo_t>();

  // Generate tile configuration file.
  TileConfigFileGenerator tileConfigFileGenerator(inputFileInfo, montageWizard->field(ImportMontage::Generic::FieldNames::MontageType).value<MontageSettings::MontageType>(),
                                                  montageWizard->field(ImportMontage::Generic::FieldNames::MontageOrder).value<MontageSettings::MontageOrder>(), numOfCols, numOfRows,
                                                  montageWizard->field(ImportMontage::Generic::FieldNames::TileOverlap).toDouble(), tileConfigFile);
  tileConfigFileGenerator.buildTileConfigFile();

  QString fijiFilePath(inputFileInfo.InputPath);
  fijiFilePath.append(QDir::separator());
  fijiFilePath.append(tileConfigFile);

  // Change wizard data for Fiji use case
  QString montageName = montageWizard->field(ImportMontage::Generic::FieldNames::MontageName).toString();
  montageWizard->setField(ImportMontage::Fiji::FieldNames::MontageName, montageName);
  FijiListInfo_t fijiListInfo;
  fijiListInfo.FijiFilePath = fijiFilePath;
  QVariant var;
  var.setValue(fijiListInfo);
  montageWizard->setField(ImportMontage::Fiji::FieldNames::FijiListInfo, var);

  importFijiMontage(montageWizard);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDREAM3DMontage(ImportMontageWizard* montageWizard)
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  QString dataFilePath = montageWizard->field(ImportMontage::DREAM3D::FieldNames::DataFilePath).toString();

  SIMPLH5DataReader reader;
  bool success = reader.openFile(dataFilePath);
  if(success)
  {
    connect(&reader, &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, const int& code) { QMessageBox::critical(this, title, msg, QMessageBox::StandardButton::Ok); });

    DataContainerArrayProxy dream3dProxy = montageWizard->field(ImportMontage::DREAM3D::FieldNames::Proxy).value<DataContainerArrayProxy>();

    m_dataContainerArray = reader.readSIMPLDataUsingProxy(dream3dProxy, false);
    if(m_dataContainerArray.get() == nullptr)
    {
      reader.closeFile();
      return;
    }

    reader.closeFile();

    // Instantiate DREAM3D File Reader filter
    QString filterName = "DataContainerReader";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);
    AbstractFilter::Pointer dataContainerReader;

    if(factory.get() != nullptr)
    {
      dataContainerReader = factory->create();
      if(dataContainerReader.get() != nullptr)
      {
        dataContainerReader->setDataContainerArray(m_dataContainerArray);

        QVariant var;

        // Set input file
        var.setValue(dataFilePath);
        if(!setFilterProperty(dataContainerReader, "InputFile", var))
        {
          return;
        }

        // Set data container array proxy
        var.setValue(dream3dProxy);
        if(!setFilterProperty(dataContainerReader, "InputFileDataContainerArrayProxy", var))
        {
          return;
        }

        m_pipelines[0]->pushBack(dataContainerReader);
      }
      else
      {
        statusBar()->showMessage(tr("Could not create filter from %1 factory. Aborting.").arg(filterName));
        return;
      }
    }
    else
    {
      statusBar()->showMessage(tr("Could not create %1 factory. Aborting.").arg(filterName));
      return;
    }

    if(m_displayMontage || m_displayOutline)
    {
      QStringList dcNames = m_dataContainerArray->getDataContainerNames();

      // Change spacing and/or origin (if selected)
      bool changeSpacing = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ChangeSpacing).toBool();
      bool changeOrigin = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ChangeOrigin).toBool();
      if(changeSpacing || changeOrigin)
      {
        factory = fm->getFactoryFromClassName("SetOriginResolutionImageGeom");
        AbstractFilter::Pointer setOriginResolutionFilter;
        float spacingX = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingX).toFloat();
        float spacingY = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingY).toFloat();
        float spacingZ = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingZ).toFloat();
        FloatVec3_t newSpacing = {spacingX, spacingY, spacingZ};
        float originX = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginX).toFloat();
        float originY = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginY).toFloat();
        float originZ = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginZ).toFloat();
        FloatVec3_t newOrigin = {originX, originY, originZ};
        QVariant var;

        if(factory.get() != nullptr)
        {

          // For each data container, add a new filter
          for(QString dcName : dcNames)
          {
            setOriginResolutionFilter = factory->create();
            if(setOriginResolutionFilter.get() != nullptr)
            {
              // Set the data container name
              var.setValue(dcName);
              if(!setFilterProperty(setOriginResolutionFilter, "DataContainerName", var))
              {
                return;
              }

              // Set the change spacing boolean flag (change resolution)
              var.setValue(changeSpacing);
              if(!setFilterProperty(setOriginResolutionFilter, "ChangeResolution", var))
              {
                return;
              }

              // Set the spacing
              var.setValue(newSpacing);
              if(!setFilterProperty(setOriginResolutionFilter, "Resolution", var))
              {
                return;
              }

              // Set the change origin boolean flag (change resolution)
              var.setValue(changeOrigin);
              if(!setFilterProperty(setOriginResolutionFilter, "ChangeOrigin", var))
              {
                return;
              }

              // Set the origin
              var.setValue(newOrigin);
              if(!setFilterProperty(setOriginResolutionFilter, "Origin", var))
              {
                return;
              }

              pipeline->pushBack(setOriginResolutionFilter);
            }
          }
        }
      }

      int rowCount = montageWizard->field(ImportMontage::DREAM3D::FieldNames::NumberOfRows).toInt();
      int colCount = montageWizard->field(ImportMontage::DREAM3D::FieldNames::NumberOfColumns).toInt();

      performMontaging(montageWizard, dcNames, ImportMontageWizard::InputType::DREAM3D, rowCount, colCount, pipeline);
    }

    m_pipelines.push_back(pipeline);

    pipeline->addMessageReceiver(this);
    m_MontageWorker->addMontagePipeline(pipeline);

    if (m_WorkerThread->isFinished())
    {
      m_WorkerThread->start();
    }
  }
  else
  {
    statusBar()->showMessage(tr("Unable to open file '%1'. Aborting.").arg(dataFilePath));
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importFijiMontage(ImportMontageWizard* montageWizard)
{
  VSFilterFactory* filterFactory = VSFilterFactory::Instance();

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  FijiListInfo_t fijiListInfo = montageWizard->field(ImportMontage::Fiji::FieldNames::FijiListInfo).value<FijiListInfo_t>();

  QString fijiConfigFilePath = fijiListInfo.FijiFilePath;
  QString dcPrefix = montageWizard->field(ImportMontage::Fiji::FieldNames::DataContainerPrefix).toString();
  QString amName = montageWizard->field(ImportMontage::Fiji::FieldNames::CellAttributeMatrixName).toString();
  QString daName = montageWizard->field(ImportMontage::Fiji::FieldNames::ImageArrayName);

  AbstractFilter::Pointer importFijiMontageFilter = filterFactory->createImportFijiMontageFilter(fijiConfigFilePath, dcPrefix, amName, daName);

  if(m_displayMontage || m_displayOutline)
  {
    // Set Image Data Containers
    importFijiMontageFilter->preflight();
    DataContainerArray::Pointer dca = importFijiMontageFilter->getDataContainerArray();
    QStringList dcNames = dca->getDataContainerNames();

    // Change spacing and/or origin (if selected)
    bool changeSpacing = montageWizard->field(ImportMontage::Fiji::FieldNames::ChangeSpacing).toBool();
    bool changeOrigin = montageWizard->field(ImportMontage::Fiji::FieldNames::ChangeOrigin).toBool();
    if(changeSpacing || changeOrigin)
    {
      factory = fm->getFactoryFromClassName("SetOriginResolutionImageGeom");
      AbstractFilter::Pointer setOriginResolutionFilter;
      float spacingX = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingX).toFloat();
      float spacingY = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingY).toFloat();
      float spacingZ = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingZ).toFloat();
      FloatVec3_t newSpacing = {spacingX, spacingY, spacingZ};
      float originX = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginX).toFloat();
      float originY = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginY).toFloat();
      float originZ = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginZ).toFloat();
      FloatVec3_t newOrigin = {originX, originY, originZ};
      QVariant var;

      if(factory.get() != nullptr)
      {

        // For each data container, add a new filter
        for(QString dcName : dcNames)
        {
          setOriginResolutionFilter = factory->create();
          if(setOriginResolutionFilter.get() != nullptr)
          {
            // Set the data container name
            var.setValue(dcName);
            if(!setFilterProperty(setOriginResolutionFilter, "DataContainerName", var))
            {
              return;
            }

            // Set the change spacing boolean flag (change resolution)
            var.setValue(changeSpacing);
            if(!setFilterProperty(setOriginResolutionFilter, "ChangeResolution", var))
            {
              return;
            }

            // Set the spacing
            var.setValue(newSpacing);
            if(!setFilterProperty(setOriginResolutionFilter, "Resolution", var))
            {
              return;
            }

            // Set the change origin boolean flag (change resolution)
            var.setValue(changeOrigin);
            if(!setFilterProperty(setOriginResolutionFilter, "ChangeOrigin", var))
            {
              return;
            }

            // Set the origin
            var.setValue(newOrigin);
            if(!setFilterProperty(setOriginResolutionFilter, "Origin", var))
            {
              return;
            }

            pipeline->pushBack(setOriginResolutionFilter);
          }
        }
      }
    }

    int rowCount = importFijiMontageFilter->property("RowCount").toInt();
    int colCount = importFijiMontageFilter->property("ColumnCount").toInt();

    performMontaging(montageWizard, dcNames, ImportMontageWizard::InputType::Fiji, rowCount, colCount, pipeline);
  }

  m_pipelines.push_back(pipeline);

  // Run the pipeline
  pipeline->addMessageReceiver(this);
  m_MontageWorker->addMontagePipeline(pipeline);

  if (m_WorkerThread->isFinished())
  {
    m_WorkerThread->start();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importRobometMontage(ImportMontageWizard* montageWizard)
{
  VSFilterFactory* filterFactory = VSFilterFactory::Instance();

  RobometListInfo_t rbmListInfo = montageWizard->field(ImportMontage::Robomet::FieldNames::RobometListInfo).value<RobometListInfo_t>();
  int sliceMin = rbmListInfo.SliceMin;
  int sliceMax = rbmListInfo.SliceMax;

  for(int slice = sliceMin; slice <= sliceMax; slice++)
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
    QString pipelineName = montageName;
    if(sliceMax - sliceMin + 1 > 1)
    {
      pipelineName.append(tr("_%1").arg(slice));
    }
    pipeline->setName(pipelineName);

    QString robometFilePath = rbmListInfo.RobometFilePath;
    QString dcPrefix = montageWizard->field(ImportMontage::Robomet::FieldNames::DataContainerPrefix).toString();
    QString amName = montageWizard->field(ImportMontage::Robomet::FieldNames::CellAttributeMatrixName).toString();
    QString daName = montageWizard->field(ImportMontage::Robomet::FieldNames::ImageArrayName);
    QString imagePrefix = rbmListInfo.ImagePrefix;
    QString imageFilePrefix = rbmListInfo.ImagePrefix;

    AbstractFilter::Pointer importRoboMetMontageFilter = filterFactory->createImportRobometMontageFilter(robometFilePath, dcPrefix, amName, daName, slice, imagePrefix, imageFilePrefix);

    if(m_displayMontage || m_displayOutline)
    {
      importRoboMetMontageFilter->preflight();
      DataContainerArray::Pointer dca = importRoboMetMontageFilter->getDataContainerArray();
      QStringList dcNames = dca->getDataContainerNames();

      ImportMontageWizard::InputType inputType = montageWizard->field(ImportMontage::FieldNames::InputType).value<ImportMontageWizard::InputType>();

      int rowCount = rbmListInfo.NumberOfRows;
      int colCount = rbmListInfo.NumberOfColumns;
      IntVec3_t montageSize = {colCount, rowCount, 1};
      double tileOverlap = 0.0;

      bool changeOverlap = montageWizard->field(ImportMontage::Robomet::FieldNames::ChangeTileOverlap).toBool();
      if(changeOverlap)
      {
        tileOverlap = montageWizard->field(ImportMontage::Robomet::FieldNames::TileOverlap).toDouble();
      }

      AbstractFilter::Pointer itkRegistrationFilter = filterFactory->createPCMTileRegistrationFilter(montageSize, dcNames, amName, daName);
      pipeline->pushBack(itkRegistrationFilter);

      DataArrayPath montagePath("MontageDC", "MontageAM", "MontageData");
      AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dcNames, amName, daName, montagePath);
      pipeline->pushBack(itkStitchingFilter);

      pipeline->addMessageReceiver(this);
      m_MontageWorker->addMontagePipeline(pipeline);

      if (m_WorkerThread->isFinished())
      {
        m_WorkerThread->start();
      }
    }

    pipeline->addMessageReceiver(this);
    m_MontageWorker->addMontagePipeline(pipeline);

    if (m_WorkerThread->isFinished())
    {
      m_WorkerThread->start();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importZeissMontage(ImportMontageWizard* montageWizard)
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  VSFilterFactory* filterFactory = VSFilterFactory::Instance();

  QString configFilePath = montageWizard->field(ImportMontage::Zeiss::FieldNames::DataFilePath).toString();
  QString dataContainerPrefix = montageWizard->field(ImportMontage::Zeiss::FieldNames::DataContainerPrefix).toString();
  QString cellAttrMatrixName = montageWizard->field(ImportMontage::Zeiss::FieldNames::CellAttributeMatrixName).toString();
  QString attributeArrayName = montageWizard->field(ImportMontage::Zeiss::FieldNames::ImageDataArrayName).toString();
  QString metadataAttrMatrixName = montageWizard->field(ImportMontage::Zeiss::FieldNames::MetadataAttrMatrixName).toString();
  bool importAllMetadata = true;
  bool convertToGrayscale = false;
  bool changeOrigin = false;
  bool changeSpacing = false;

  AbstractFilter::Pointer importZeissMontage = filterFactory->createImportZeissMontageFilter(configFilePath, dataContainerPrefix, cellAttrMatrixName, attributeArrayName, metadataAttrMatrixName,
                                                                                             importAllMetadata, convertToGrayscale, changeOrigin, changeSpacing);

  if(m_displayMontage || m_displayOutline)
  {
    importZeissMontage->preflight();
    DataContainerArray::Pointer dca = importZeissMontage->getDataContainerArray();
    QStringList dcNames = dca->getDataContainerNames();
    int rowCount = importZeissMontage->property("RowCount").toInt();
    int colCount = importZeissMontage->property("ColumnCount").toInt();
    IntVec3_t montageSize = {colCount, rowCount, 1};
    double tileOverlap = 0.0;

    bool changeOverlap = montageWizard->field(ImportMontage::Zeiss::FieldNames::ChangeTileOverlap).toBool();
    if(changeOverlap)
    {
      tileOverlap = montageWizard->field(ImportMontage::Zeiss::FieldNames::TileOverlap).toDouble();
    }

    AbstractFilter::Pointer itkRegistrationFilter = filterFactory->createPCMTileRegistrationFilter(montageSize, dcNames, cellAttrMatrixName, attributeArrayName);
    pipeline->pushBack(itkRegistrationFilter);

    DataArrayPath montagePath("MontageDC", "MontageAM", "MontageData");
    AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dcNames, cellAttrMatrixName, attributeArrayName, montagePath, tileOverlap);
    pipeline->pushBack(itkStitchingFilter);
  }

  pipeline->addMessageReceiver(this);
  m_MontageWorker->addMontagePipeline(pipeline);

  if (m_WorkerThread->isFinished())
  {
    m_WorkerThread->start();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::performMontaging(ImportMontageWizard* montageWizard, QStringList dataContainerNames, ImportMontageWizard::InputType inputType, int rowCount, int colCount,
                                    FilterPipeline::Pointer pipeline)
{
  IntVec3_t montageSize = {colCount, rowCount, 1};

  QString cellAttrMatrixName;
  QString commonDataArrayName;
  double tileOverlap = 0.0;
  switch(inputType)
  {
  case ImportMontageWizard::InputType::DREAM3D:
  {
    cellAttrMatrixName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::CellAttributeMatrixName).toString();
    commonDataArrayName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ImageArrayName).toString();
    tileOverlap = montageWizard->field(ImportMontage::DREAM3D::FieldNames::TileOverlap).toDouble();
    break;
  }
  case ImportMontageWizard::InputType::Fiji:
  {
    cellAttrMatrixName = montageWizard->field(ImportMontage::Fiji::FieldNames::CellAttributeMatrixName).toString();
    commonDataArrayName = montageWizard->field(ImportMontage::Fiji::FieldNames::ImageArrayName).toString();
    bool changeOverlap = montageWizard->field(ImportMontage::Fiji::FieldNames::ChangeTileOverlap).toBool();
    if(changeOverlap)
    {
      tileOverlap = montageWizard->field(ImportMontage::Fiji::FieldNames::TileOverlap).toDouble();
    }
    break;
  }
  case ImportMontageWizard::InputType::Generic:
  {
    break;
  }
  case ImportMontageWizard::InputType::Robomet:
  {
    break;
  }
  case ImportMontageWizard::InputType::Zeiss:
  {
    break;
  }
  }

  VSFilterFactory* filterFactory = VSFilterFactory::Instance();

  AbstractFilter::Pointer itkRegistrationFilter = filterFactory->createPCMTileRegistrationFilter(montageSize, dataContainerNames, cellAttrMatrixName, commonDataArrayName);
  pipeline->pushBack(itkRegistrationFilter);

  DataArrayPath montagePath("MontageDC", "MontageAM", "MontageData");
  AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dataContainerNames, cellAttrMatrixName, commonDataArrayName, montagePath);
  pipeline->pushBack(itkStitchingFilter);

  pipeline->addMessageReceiver(this);
  m_MontageWorker->addMontagePipeline(pipeline);

  if (m_WorkerThread->isFinished())
  {
    m_WorkerThread->start();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::handleMontageResults(FilterPipeline::Pointer pipeline, int err)
{
  if(err >= 0)
  {
    DataContainerArray::Pointer dca = pipeline->getDataContainerArray();
    QStringList pipelineNameTokens = pipeline->getName().split("_", QString::SplitBehavior::SkipEmptyParts);
    int slice = 0;
    if(pipelineNameTokens.size() > 1)
    {
      slice = pipelineNameTokens[1].toInt();
    }

    // If Display Montage was selected, remove non-stitched image data containers
    if(m_DisplayMontage)
    {
      for(DataContainer::Pointer dc : dca->getDataContainers())
      {
        if(dc->getName() == "MontageDC")
        {
          ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();
          if(imageGeom)
          {
            float origin[3];
            imageGeom->getOrigin(origin);
            origin[2] += slice;
            imageGeom->setOrigin(origin);
          }
        }
        else
        {
          dca->removeDataContainer(dc->getName());
        }
      }
    }

    importPipelineOutput(pipeline, dca);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(QString filePath, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(filePath, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadDataContainerArray(VSFileNameFilter* fileFilter, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Reload);
  m_ImportObject->addDataContainerArray(fileFilter, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(pipeline, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importPipelineOutput(std::vector<FilterPipeline::Pointer> pipelines)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  for(FilterPipeline::Pointer pipeline : pipelines)
  {
    DataContainerArray::Pointer dca = pipeline->getDataContainerArray();
    m_ImportObject->addDataContainerArray(pipeline, dca);
  }
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  VSPipelineFilter* parentFilter = dynamic_cast<VSPipelineFilter*>(getFilterModel()->getPipelineFilter(pipeline));
  if(parentFilter)
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::SemiReload);
    m_ImportObject->addDataContainerArray(parentFilter, dca);
  }
  else
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Geometry);
    m_ImportObject->addDataContainerArray(pipeline, dca);
  }

  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray("No File", dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainer(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // Add VSSIMPLDataContainerFilter if the DataContainer contains relevant data for rendering
  if(wrappedData)
  {
    VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedData);
    m_FilterModel->addFilter(filter);
  }

  emit dataImported();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(const QString& filePath)
{
  VSFileNameFilter* textFilter = new VSFileNameFilter(filePath);
  VSDataSetFilter* filter = new VSDataSetFilter(filePath, textFilter);
  // Check if any data was imported
  if(filter->getOutput())
  {
    m_FilterModel->addFilter(textFilter, false);
    m_FilterModel->addFilter(filter);

    emit dataImported();
  }
  else
  {
    textFilter->deleteLater();
  }
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilter(VSAbstractFilter* filter)
{
  // Do not change the selection when nullptr is passed in
  if(nullptr == filter || m_FilterModel->getRootFilter() == filter)
  {
    return;
  }

  QModelIndex index = m_FilterModel->getIndexFromFilter(filter);
  //m_SelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveSession(const QString& sessionFilePath)
{
  QFile outputFile(sessionFilePath);
  if(outputFile.open(QIODevice::WriteOnly))
  {
    QJsonObject rootObj;

    VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
    for(VSAbstractFilter* filter : baseFilters)
    {
      saveFilter(filter, rootObj);
    }

    QJsonDocument doc(rootObj);

    outputFile.write(doc.toJson());
    outputFile.close();

    return true;
  }
  else
  {
    // "Failed to open output file" error
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::saveFilter(VSAbstractFilter* filter, QJsonObject& obj)
{
  QJsonObject rootFilterObj;

  filter->writeJson(rootFilterObj);

  // Write the children
  QJsonObject childrenObj;

  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  for(VSAbstractFilter* childFilter : childFilters)
  {
    saveFilter(childFilter, childrenObj);
  }

  rootFilterObj["Child Filters"] = childrenObj;

  obj[filter->getFilterName()] = rootFilterObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::loadSession(const QString& sessionFilePath)
{
  QFile inputFile(sessionFilePath);
  if(inputFile.open(QIODevice::ReadOnly) == false)
  {
    return false;
  }

  QByteArray byteArray = inputFile.readAll();
  QJsonParseError parseError;

  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    return false;
  }

  QJsonObject rootObj = doc.object();
  for(QJsonObject::iterator iter = rootObj.begin(); iter != rootObj.end(); iter++)
  {
    QJsonObject filterObj = iter.value().toObject();
    loadFilter(filterObj);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::loadFilter(QJsonObject& obj, VSAbstractFilter* parentFilter)
{
  QUuid uuid(obj["Uuid"].toString());

  VSAbstractFilter* newFilter = nullptr;
  if(uuid == VSClipFilter::GetUuid())
  {
    newFilter = VSClipFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSCropFilter::GetUuid())
  {
    newFilter = VSCropFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSDataSetFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSDataSetFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSFileNameFilter::GetUuid())
  {
    newFilter = VSFileNameFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSMaskFilter::GetUuid())
  {
    newFilter = VSMaskFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSSIMPLDataContainerFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSSIMPLDataContainerFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSSliceFilter::GetUuid())
  {
    newFilter = VSSliceFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSTextFilter::GetUuid())
  {
    newFilter = VSTextFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSThresholdFilter::GetUuid())
  {
    newFilter = VSThresholdFilter::Create(obj, parentFilter);
  }

  if(newFilter != nullptr)
  {
    QJsonObject childrenObj = obj["Child Filters"].toObject();

    m_FilterModel->addFilter(newFilter, (childrenObj.size() == 0));
    newFilter->setChecked(obj["CheckState"].toInt() == Qt::Checked);
    emit filterCheckStateChanged(newFilter);

    for(QJsonObject::iterator iter = childrenObj.begin(); iter != childrenObj.end(); iter++)
    {
      QJsonObject childObj = iter.value().toObject();
      loadFilter(childObj, newFilter);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFileNameFilter* VSController::getBaseFileNameFilter(QString text)
{
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
  for(VSAbstractFilter* baseFilter : baseFilters)
  {
    VSFileNameFilter* filter = dynamic_cast<VSFileNameFilter*>(baseFilter);
    if(filter)
    {
      if(filter->getFilterName().compare(text) == 0)
      {
        return filter;
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getBaseFilters()
{
  return m_FilterModel->getBaseFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getAllFilters()
{
  return m_FilterModel->getAllFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel* VSController::getFilterModel()
{
  return m_FilterModel;
}
