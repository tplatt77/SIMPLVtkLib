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

#include "ImportDataWizard.h"

#include "SIMPLVtkLib/Wizards/GenericMontage/GenericCollectionTypePage.h"
#include "SIMPLVtkLib/Wizards/GenericMontage/GenericMetadataPage.h"
#include "SIMPLVtkLib/Wizards/ImportData/FileTypeSelectionPage.h"
#include "SIMPLVtkLib/Wizards/ImportData/DREAM3DFilePage.h"
#include "SIMPLVtkLib/Wizards/ImportData/FijiConfigFilePage.h"
#include "SIMPLVtkLib/Wizards/ImportData/RobometConfigFilePage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDataWizard::ImportDataWizard(QWidget* parent)
: AbstractMontageWizard(parent)
{
  m_montageSettings = new GenericMontageSettings();
  
  setWindowTitle("Import Data Wizard");

  setPage(WizardPages::FileTypeSelection, new FileTypeSelectionPage);

  setPage(WizardPages::GenericCollectionType, new GenericCollectionTypePage);

  setPage(WizardPages::GenericMetadata, new GenericMetadataPage);

  setPage(WizardPages::DREAM3DFile, new DREAM3DFilePage);

  setPage(WizardPages::FijiConfigFile, new FijiConfigFilePage);

  setPage(WizardPages::RobometConfigFile, new RobometConfigFilePage);

  setStartId(WizardPages::FileTypeSelection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings* ImportDataWizard::getMontageSettings()
{
	return m_montageSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDataWizard::accept()
{
	// For DREAM3D File, perform ordinary open procedure
	if (currentId() == WizardPages::DREAM3DFile)
	{
		QString dream3dFilePath = field("DREAM3DFile").toString();
		if (dream3dFilePath.isEmpty())
		{
			return;
		}
		m_montageSettings->setOutputFileName(dream3dFilePath);
		fileType = FileType::DREAM3D;
	}
	else if (currentId() == WizardPages::FijiConfigFile)
	{
		QString fijiConfigFilePath = field("FijiConfigFile").toString();
		if (fijiConfigFilePath.isEmpty())
		{
			return;
		}
		m_montageSettings->setLayoutFile(fijiConfigFilePath);
		fileType = FileType::FIJI;

	}
	else if (currentId() == WizardPages::RobometConfigFile)
	{
		QString robometConfigFilePath = field("RobometConfigFile").toString();
		if (robometConfigFilePath.isEmpty())
		{
			return;
		}
		m_montageSettings->setLayoutFile(robometConfigFilePath);
		fileType = FileType::Robomet;
	}
	else if (currentId() == WizardPages::GenericMetadata)
	{
		fileType = FileType::GenericMontage;
		// Get type and order of collection method
		GenericMontageSettings::MontageType montageType = static_cast<GenericMontageSettings::MontageType>(field("montageType").toInt());
		if (montageType < 0) {
			montageType = GenericMontageSettings::MontageType::GridRowByRow;
		}
		m_montageSettings->setMontageType(montageType);
		GenericMontageSettings::MontageOrder montageOrder = static_cast<GenericMontageSettings::MontageOrder>(field("montageOrder").toInt());
		if (montageOrder < 0) {
			montageOrder = GenericMontageSettings::MontageOrder::RightAndDown;
		}
		m_montageSettings->setMontageOrder(montageOrder);

		// Get the metadata
		uint64_t numOfRows = field("numOfRows").toInt();
		m_montageSettings->setGridSizeY(numOfRows);
		uint64_t numOfCols = field("numOfCols").toInt();
		m_montageSettings->setGridSizeX(numOfCols);
		double tileOverlap = field("tileOverlap").toDouble();
		m_montageSettings->setTileOverlap(tileOverlap);
		uint64_t firstFileIndex = field("firstFileIndex").toInt();
		m_montageSettings->setFirstFileIndex(firstFileIndex);
		QString tilesDirectory = field("tilesDirectory").toString();
		m_montageSettings->setImagesDirectory(tilesDirectory);
		QString outputFileName = field("outputFileName").toString();
		m_montageSettings->setOutputFileName(outputFileName);
		int fusionMethod = field("fusionMethod").toInt();
		if (fusionMethod < 0)
		{
			fusionMethod = 0;
		}
		m_montageSettings->setFusionMethod(static_cast<GenericMontageSettings::FusionMethod>(fusionMethod));

	}
	QDialog::accept();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDataWizard::FileType ImportDataWizard::getFileType() const
{
	return fileType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDataWizard::~ImportDataWizard() = default;
