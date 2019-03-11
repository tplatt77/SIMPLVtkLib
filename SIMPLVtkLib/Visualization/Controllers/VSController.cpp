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
#include "SIMPLVtkLib/QtWidgets/VSMontageImporter.h"
#include "SIMPLVtkLib/QtWidgets/VSDatasetImporter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

#include "SIMPLVtkLib/Wizards/ExecutePipeline/ExecutePipelineConstants.h"
#include "SIMPLVtkLib/Wizards/ExecutePipeline/ExecutePipelineWizard.h"
#include "SIMPLVtkLib/Wizards/ExecutePipeline/PipelineWorker.h"

#include "SIMPLVtkLib/Wizards/ImportMontage/FijiListWidget.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/ImportMontageConstants.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/ImportMontageWizard.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/MontageWorker.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/RobometListWidget.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/TileConfigFileGenerator.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/ZeissListWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
: QObject(parent)
, m_FilterModel(new VSFilterModel())
{
  m_ImportObject = new VSConcurrentImport(this);

  qRegisterMetaType<VSAbstractImporter::Pointer>();

  m_ImportDataWorker = new MontageWorker();
//  connect(m_ImportDataWorker, SIGNAL(finished()), this, SLOT(montageWorkerFinished()));

  m_WorkerThread = new QThread;
  m_ImportDataWorker->moveToThread(m_WorkerThread);
  connect(m_WorkerThread, SIGNAL(started()), m_ImportDataWorker, SLOT(process()));
//  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT(montageThreadFinished()));
  connect(m_ImportDataWorker, SIGNAL(finished()), m_WorkerThread, SLOT(quit()));

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
void VSController::montageWorkerFinished()
{
//  delete m_ImportDataWorker;
//  m_ImportDataWorker = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::montageThreadFinished()
{
  //  delete m_WorkerThread;
  //  m_WorkerThread = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::pipelineWorkerFinished()
{
  delete m_PipelineWorker;
  m_PipelineWorker = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::pipelineThreadFinished()
{
  delete m_PipelineWorkerThread;
  m_PipelineWorkerThread = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importMontage(ImportMontageWizard* montageWizard)
{
  m_DisplayMontage = montageWizard->field(ImportMontage::FieldNames::DisplayMontage).toBool();
  m_DisplayOutline = montageWizard->field(ImportMontage::FieldNames::DisplayOutlineOnly).toBool();

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
    QString msg = tr("IMF Viewer failed to detect the proper data file type from the given input file.  "
                     "Supported file types are DREAM3D, VTK, STL and Image files, as well as Fiji and Robomet configuration files.");
    emit notifyErrorMessage(msg, -2002);
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
  bool changeSpacing = montageWizard->field(ImportMontage::Generic::FieldNames::ChangeSpacing).toBool();
  montageWizard->setField(ImportMontage::Fiji::FieldNames::ChangeSpacing, changeSpacing);
  if(changeSpacing)
  {
	float spacingX = montageWizard->field(ImportMontage::Generic::FieldNames::SpacingX).toFloat();
	float spacingY = montageWizard->field(ImportMontage::Generic::FieldNames::SpacingY).toFloat();
	float spacingZ = montageWizard->field(ImportMontage::Generic::FieldNames::SpacingZ).toFloat();
	montageWizard->setField(ImportMontage::Fiji::FieldNames::SpacingX, spacingX);
	montageWizard->setField(ImportMontage::Fiji::FieldNames::SpacingY, spacingY);
	montageWizard->setField(ImportMontage::Fiji::FieldNames::SpacingZ, spacingZ);
  }
  bool changeOrigin = montageWizard->field(ImportMontage::Generic::FieldNames::ChangeOrigin).toBool();
  montageWizard->setField(ImportMontage::Fiji::FieldNames::ChangeOrigin, changeOrigin);
  if(changeOrigin)
  {
	float originX = montageWizard->field(ImportMontage::Generic::FieldNames::OriginX).toFloat();
	float originY = montageWizard->field(ImportMontage::Generic::FieldNames::OriginY).toFloat();
	float originZ = montageWizard->field(ImportMontage::Generic::FieldNames::OriginZ).toFloat();
	montageWizard->setField(ImportMontage::Fiji::FieldNames::OriginX, originX);
	montageWizard->setField(ImportMontage::Fiji::FieldNames::OriginY, originY);
	montageWizard->setField(ImportMontage::Fiji::FieldNames::OriginZ, originZ);
  }

  importFijiMontage(montageWizard);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDREAM3DMontage(ImportMontageWizard* montageWizard)
{
  VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();
  connect(filterFactory.get(), &VSFilterFactory::notifyErrorMessage, this, &VSController::notifyErrorMessage);

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  QString dataFilePath = montageWizard->field(ImportMontage::DREAM3D::FieldNames::DataFilePath).toString();

  SIMPLH5DataReader reader;
  bool success = reader.openFile(dataFilePath);
  if(success)
  {
    connect(&reader, &SIMPLH5DataReader::errorGenerated, [=](const QString& title, const QString& msg, const int& code) { emit notifyErrorMessage(msg, code); });

    DataContainerArrayProxy dream3dProxy = montageWizard->field(ImportMontage::DREAM3D::FieldNames::Proxy).value<DataContainerArrayProxy>();

    m_dataContainerArray = reader.readSIMPLDataUsingProxy(dream3dProxy, false);
    if(m_dataContainerArray.get() == nullptr)
    {
      reader.closeFile();
      return;
    }

    reader.closeFile();

    AbstractFilter::Pointer dataContainerReader = filterFactory->createDataContainerReaderFilter(dataFilePath, dream3dProxy);
    if(!dataContainerReader)
    {
      // Error!
    }

    pipeline->pushBack(dataContainerReader);

    if(m_DisplayMontage || m_DisplayOutline)
    {
      QStringList dcNames = m_dataContainerArray->getDataContainerNames();

      // Change spacing and/or origin (if selected)
      bool changeSpacing = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ChangeSpacing).toBool();
      bool changeOrigin = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ChangeOrigin).toBool();
      if(changeSpacing || changeOrigin)
      {
        float spacingX = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingX).toFloat();
        float spacingY = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingY).toFloat();
        float spacingZ = montageWizard->field(ImportMontage::DREAM3D::FieldNames::SpacingZ).toFloat();
        FloatVec3_t newSpacing = {spacingX, spacingY, spacingZ};
        float originX = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginX).toFloat();
        float originY = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginY).toFloat();
        float originZ = montageWizard->field(ImportMontage::DREAM3D::FieldNames::OriginZ).toFloat();
        FloatVec3_t newOrigin = {originX, originY, originZ};
        QVariant var;

        // For each data container, add a new filter
        for(QString dcName : dcNames)
        {
          AbstractFilter::Pointer setOriginResolutionFilter = filterFactory->createSetOriginResolutionFilter(dcName, changeSpacing, changeOrigin, newSpacing, newOrigin);

          if(!setOriginResolutionFilter)
          {
            // Error!
          }

          pipeline->pushBack(setOriginResolutionFilter);
        }
      }

      int rowCount = montageWizard->field(ImportMontage::DREAM3D::FieldNames::NumberOfRows).toInt();
      int colCount = montageWizard->field(ImportMontage::DREAM3D::FieldNames::NumberOfColumns).toInt();

      IntVec3_t montageSize = {colCount, rowCount, 1};

      QString amName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::CellAttributeMatrixName).toString();
      QString daName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::ImageArrayName).toString();
      double tileOverlap = montageWizard->field(ImportMontage::DREAM3D::FieldNames::TileOverlap).toDouble();

      AbstractFilter::Pointer itkRegistrationFilter = filterFactory->createPCMTileRegistrationFilter(montageSize, dcNames, amName, daName);
      pipeline->pushBack(itkRegistrationFilter);

      DataArrayPath montagePath("MontageDC", "MontageAM", "MontageData");
      AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dcNames, amName, daName, montagePath, tileOverlap);
      pipeline->pushBack(itkStitchingFilter);
    }

    addMontagePipelineToQueue(pipeline);
  }
  else
  {
    emit notifyErrorMessage(tr("Unable to open file '%1'. Aborting.").arg(dataFilePath), -2001);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importFijiMontage(ImportMontageWizard* montageWizard)
{
  VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();

  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  FijiListInfo_t fijiListInfo = montageWizard->field(ImportMontage::Fiji::FieldNames::FijiListInfo).value<FijiListInfo_t>();

  QString fijiConfigFilePath = fijiListInfo.FijiFilePath;
  QString dcPrefix = montageWizard->field(ImportMontage::Fiji::FieldNames::DataContainerPrefix).toString();
  QString amName = montageWizard->field(ImportMontage::Fiji::FieldNames::CellAttributeMatrixName).toString();
  QString daName = montageWizard->field(ImportMontage::Fiji::FieldNames::ImageArrayName).toString();

  AbstractFilter::Pointer importFijiMontageFilter = filterFactory->createImportFijiMontageFilter(fijiConfigFilePath, dcPrefix, amName, daName);
  if (!importFijiMontageFilter)
  {
    // Error!
  }

  pipeline->pushBack(importFijiMontageFilter);

  if(m_DisplayMontage || m_DisplayOutline)
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
      float spacingX = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingX).toFloat();
      float spacingY = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingY).toFloat();
      float spacingZ = montageWizard->field(ImportMontage::Fiji::FieldNames::SpacingZ).toFloat();
      FloatVec3_t newSpacing = {spacingX, spacingY, spacingZ};
      float originX = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginX).toFloat();
      float originY = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginY).toFloat();
      float originZ = montageWizard->field(ImportMontage::Fiji::FieldNames::OriginZ).toFloat();
      FloatVec3_t newOrigin = {originX, originY, originZ};
      QVariant var;

      // For each data container, add a new filter
      for(QString dcName : dcNames)
      {
        AbstractFilter::Pointer setOriginResolutionFilter = filterFactory->createSetOriginResolutionFilter(dcName, changeSpacing, changeOrigin, newSpacing, newOrigin);
        if(!setOriginResolutionFilter)
        {
          // Error!
        }

        pipeline->pushBack(setOriginResolutionFilter);
      }
    }

    int rowCount = importFijiMontageFilter->property("RowCount").toInt();
    int colCount = importFijiMontageFilter->property("ColumnCount").toInt();

    IntVec3_t montageSize = {colCount, rowCount, 1};
    double tileOverlap = 0.0;

    bool changeOverlap = montageWizard->field(ImportMontage::Fiji::FieldNames::ChangeTileOverlap).toBool();
    if(changeOverlap)
    {
      tileOverlap = montageWizard->field(ImportMontage::Fiji::FieldNames::TileOverlap).toDouble();
    }

    AbstractFilter::Pointer itkRegistrationFilter = filterFactory->createPCMTileRegistrationFilter(montageSize, dcNames, amName, daName);
    pipeline->pushBack(itkRegistrationFilter);

    DataArrayPath montagePath("MontageDC", "MontageAM", "MontageData");
    AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dcNames, amName, daName, montagePath, tileOverlap);
    pipeline->pushBack(itkStitchingFilter);
  }

  // Run the pipeline
  addMontagePipelineToQueue(pipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importRobometMontage(ImportMontageWizard* montageWizard)
{
  VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();

  RobometListInfo_t rbmListInfo = montageWizard->field(ImportMontage::Robomet::FieldNames::RobometListInfo).value<RobometListInfo_t>();
  int sliceMin = rbmListInfo.SliceMin;
  int sliceMax = rbmListInfo.SliceMax;

  for(int slice = sliceMin; slice <= sliceMax; slice++)
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();
    QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
    QString pipelineName = montageName;
    pipelineName.append(tr("_%1").arg(slice));
    pipeline->setName(pipelineName);

    QString robometFilePath = rbmListInfo.RobometFilePath;
    QString dcPrefix = montageWizard->field(ImportMontage::Robomet::FieldNames::DataContainerPrefix).toString();
    QString amName = montageWizard->field(ImportMontage::Robomet::FieldNames::CellAttributeMatrixName).toString();
    QString daName = montageWizard->field(ImportMontage::Robomet::FieldNames::ImageArrayName).toString();
    QString imagePrefix = rbmListInfo.ImagePrefix;
    QString imageFileExtension = rbmListInfo.ImageExtension;

    AbstractFilter::Pointer importRoboMetMontageFilter = filterFactory->createImportRobometMontageFilter(robometFilePath, dcPrefix, amName, daName, slice, imagePrefix, imageFileExtension);
    if (!importRoboMetMontageFilter)
    {
      // Error!
    }

    pipeline->pushBack(importRoboMetMontageFilter);

    if(m_DisplayMontage || m_DisplayOutline)
    {
      importRoboMetMontageFilter->preflight();
      DataContainerArray::Pointer dca = importRoboMetMontageFilter->getDataContainerArray();
      QStringList dcNames = dca->getDataContainerNames();

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
      AbstractFilter::Pointer itkStitchingFilter = filterFactory->createTileStitchingFilter(montageSize, dcNames, amName, daName, montagePath, tileOverlap);
      pipeline->pushBack(itkStitchingFilter);
    }

	addMontagePipelineToQueue(pipeline);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importZeissMontage(ImportMontageWizard* montageWizard)
{
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
  AbstractFilter::Pointer importZeissMontage;
  QString montageName = montageWizard->field(ImportMontage::DREAM3D::FieldNames::MontageName).toString();
  pipeline->setName(montageName);

  VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();

  ZeissListInfo_t zeissListInfo = montageWizard->field(ImportMontage::Zeiss::FieldNames::ZeissListInfo).value<ZeissListInfo_t>();

  QString configFilePath = zeissListInfo.ZeissFilePath;
  QString dataContainerPrefix = montageWizard->field(ImportMontage::Zeiss::FieldNames::DataContainerPrefix).toString();
  QString cellAttrMatrixName = montageWizard->field(ImportMontage::Zeiss::FieldNames::CellAttributeMatrixName).toString();
  QString attributeArrayName = montageWizard->field(ImportMontage::Zeiss::FieldNames::ImageDataArrayName).toString();
  QString metadataAttrMatrixName = montageWizard->field(ImportMontage::Zeiss::FieldNames::MetadataAttrMatrixName).toString();
  bool importAllMetadata = true;
  bool convertToGrayscale = montageWizard->field(ImportMontage::Zeiss::FieldNames::ConvertToGrayscale).toBool();
  bool changeSpacing = montageWizard->field(ImportMontage::Zeiss::FieldNames::ChangeSpacing).toBool();
  bool changeOrigin = montageWizard->field(ImportMontage::Zeiss::FieldNames::ChangeOrigin).toBool();
  float colorWeights[3];
  float newSpacing[3];
  float newOrigin[3];

  if(changeSpacing || changeOrigin)
  {
	newSpacing[0] = montageWizard->field(ImportMontage::Zeiss::FieldNames::SpacingX).toFloat();
	newSpacing[1] = montageWizard->field(ImportMontage::Zeiss::FieldNames::SpacingY).toFloat();
	newSpacing[2] = montageWizard->field(ImportMontage::Zeiss::FieldNames::SpacingZ).toFloat();
	newOrigin[0] = montageWizard->field(ImportMontage::Zeiss::FieldNames::OriginX).toFloat();
	newOrigin[1] = montageWizard->field(ImportMontage::Zeiss::FieldNames::OriginY).toFloat();
	newOrigin[2] = montageWizard->field(ImportMontage::Zeiss::FieldNames::OriginZ).toFloat();
  }
  if(convertToGrayscale)
  {
	colorWeights[0] = montageWizard->field(ImportMontage::Zeiss::FieldNames::ColorWeightingR).toFloat();
	colorWeights[1] = montageWizard->field(ImportMontage::Zeiss::FieldNames::ColorWeightingG).toFloat();
	colorWeights[2] = montageWizard->field(ImportMontage::Zeiss::FieldNames::ColorWeightingB).toFloat();
  }
  importZeissMontage = filterFactory->createImportZeissMontageFilter(configFilePath, dataContainerPrefix, cellAttrMatrixName, attributeArrayName, metadataAttrMatrixName,
	  importAllMetadata, convertToGrayscale, changeOrigin, changeSpacing, colorWeights, newOrigin, newSpacing);

  if (!importZeissMontage)
  {
    // Error!
  }

  pipeline->pushBack(importZeissMontage);

  if(m_DisplayMontage || m_DisplayOutline)
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

  addMontagePipelineToQueue(pipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::addMontagePipelineToQueue(FilterPipeline::Pointer pipeline)
{
  VSMontageImporter::Pointer importer = VSMontageImporter::New(pipeline);
  connect(importer.get(), &VSMontageImporter::notifyStatusMessage, this, &VSController::notifyStatusMessage);
  connect(importer.get(), &VSMontageImporter::notifyErrorMessage, this, &VSController::notifyErrorMessage);
  connect(importer.get(), &VSMontageImporter::resultReady, this, &VSController::handleMontageResults);

  m_ImportDataWorker->addDataImporter(importer);
  emit importerAddedToQueue(pipeline->getName(), importer);

  m_ImportDataWorkerThread->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::executePipeline(FilterPipeline::Pointer pipeline,
  DataContainerArray::Pointer dca)
{
  pipeline->addMessageReceiver(this);
  m_PipelineWorker = new PipelineWorker();
  connect(m_PipelineWorker, SIGNAL(finished()), this, SLOT(pipelineWorkerFinished()));
  connect(m_PipelineWorker, &PipelineWorker::resultReady, this, &VSController::handleMontageResults);

  m_PipelineWorker->addPipeline(pipeline, dca);
  
  m_PipelineWorkerThread = new QThread;
  m_PipelineWorker->moveToThread(m_PipelineWorkerThread);
  connect(m_PipelineWorkerThread, SIGNAL(started()), m_PipelineWorker, SLOT(process()));
  connect(m_PipelineWorkerThread, SIGNAL(finished()), this, SLOT(pipelineThreadFinished()));
  connect(m_PipelineWorker, SIGNAL(finished()), m_PipelineWorkerThread, SLOT(quit()));
  
  m_PipelineWorkerThread->start();
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
void VSController::handleDatasetResults(VSFileNameFilter* textFilter, VSDataSetFilter* filter)
{
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
void VSController::importPipeline(ExecutePipelineWizard* executePipelineWizard)
{
	QString filePath = executePipelineWizard->field(ExecutePipeline::FieldNames::PipelineFile)
	  .toString();
	ExecutePipelineWizard::ExecutionType executionType = executePipelineWizard
	  ->field(ExecutePipeline::FieldNames::ExecutionType)
	  .value<ExecutePipelineWizard::ExecutionType>();
	m_DisplayMontage = executePipelineWizard->field(ImportMontage::FieldNames::DisplayMontage).toBool();
	m_DisplayOutline = executePipelineWizard->field(ImportMontage::FieldNames::DisplayOutlineOnly).toBool();

	if(filePath.isEmpty())
	{
		return;
	}

	QFileInfo fi(filePath);

	QString jsonContent;
	QFile jsonFile;
	jsonFile.setFileName(fi.absoluteFilePath());
	jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
	jsonContent = jsonFile.readAll();
	jsonFile.close();
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonContent.toUtf8());
	QJsonObject jsonObj = jsonDoc.object();
	FilterPipeline::Pointer pipelineFromJson = FilterPipeline::FromJson(jsonObj);

	if(pipelineFromJson != FilterPipeline::NullPointer())
	{
	  if(executionType == ExecutePipelineWizard::ExecutionType::FromFilesystem)
	  {
		addMontagePipelineToQueue(pipelineFromJson);
	  }
	  else if(executionType == ExecutePipelineWizard::ExecutionType::OnLoadedData)
	  {
		int startFilter = executePipelineWizard->field(ExecutePipeline::FieldNames::StartFilter).toInt();
		int selectedDataset = executePipelineWizard->field(ExecutePipeline::FieldNames::SelectedDataset).toInt();
		
		// Construct Data Container Array with selected Dataset
		DataContainerArray::Pointer dca = DataContainerArray::New();
		VSAbstractFilter::FilterListType datasets = this->getBaseFilters();
		int i = 0;
		for(VSAbstractFilter* dataset : datasets)
		{
		  if(i == selectedDataset)
		  {
			// Add contents to data container array
			VSAbstractFilter::FilterListType children = dataset->getChildren();
			for(VSAbstractFilter* childFilter : children)
			{
			  bool isSIMPL = dynamic_cast<VSSIMPLDataContainerFilter*>(childFilter);
			  if(isSIMPL)
			  {
				VSSIMPLDataContainerFilter* dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(childFilter);
				if(dcFilter != nullptr)
				{
				  DataContainer::Pointer dataContainer = dcFilter->getWrappedDataContainer()->m_DataContainer;
				  dca->addDataContainer(dataContainer);
				}
			  }
			}
			break;
		  }
		  i++;
		}
		// Change origin and/or spacing if specified
		FilterPipeline::Pointer pipeline = FilterPipeline::New();
		VSFilterFactory::Pointer filterFactory = VSFilterFactory::New();
		QStringList dcNames = dca->getDataContainerNames();

		bool changeSpacing = executePipelineWizard->field(ExecutePipeline::FieldNames::ChangeSpacing).toBool();
		bool changeOrigin = executePipelineWizard->field(ExecutePipeline::FieldNames::ChangeOrigin).toBool();
		if(changeSpacing || changeOrigin)
		{
		  float spacingX = executePipelineWizard->field(ExecutePipeline::FieldNames::SpacingX).toFloat();
		  float spacingY = executePipelineWizard->field(ExecutePipeline::FieldNames::SpacingY).toFloat();
		  float spacingZ = executePipelineWizard->field(ExecutePipeline::FieldNames::SpacingZ).toFloat();
		  FloatVec3_t newSpacing = { spacingX, spacingY, spacingZ };
		  float originX = executePipelineWizard->field(ExecutePipeline::FieldNames::OriginX).toFloat();
		  float originY = executePipelineWizard->field(ExecutePipeline::FieldNames::OriginY).toFloat();
		  float originZ = executePipelineWizard->field(ExecutePipeline::FieldNames::OriginZ).toFloat();
		  FloatVec3_t newOrigin = { originX, originY, originZ };
		  QVariant var;

		  // For each data container, add a new filter
		  for(QString dcName : dcNames)
		  {
			AbstractFilter::Pointer setOriginResolutionFilter = filterFactory->createSetOriginResolutionFilter(dcName, changeSpacing, changeOrigin, newSpacing, newOrigin);

			if(!setOriginResolutionFilter)
			{
			  // Error!
			}

			pipeline->pushBack(setOriginResolutionFilter);
		  }
		}

		// Reconstruct pipeline starting with new start filter
		for(int i = startFilter; i < pipelineFromJson->getFilterContainer().size(); i++)
		{
		  AbstractFilter::Pointer filter = pipelineFromJson->getFilterContainer().at(i);
		  filter->setDataContainerArray(dca);
		  pipeline->pushBack(filter);
		}

		executePipeline(pipeline, dca);
	  }
	}
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
  VSDatasetImporter::Pointer importer = VSDatasetImporter::New(filePath);
  connect(importer.get(), &VSDatasetImporter::resultReady, this, &VSController::handleDatasetResults);
  m_ImportDataWorker->addDataImporter(importer);

  QFileInfo fi(filePath);
  emit importerAddedToQueue(fi.fileName(), importer);

  m_WorkerThread->start();
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
