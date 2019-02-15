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

#include "EnterDataFilePage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "ImportMontage/ImportMontageConstants.h"

#include "ImportMontageWizard.h"

// Initialize private static member variable
QString EnterDataFilePage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDataFilePage::EnterDataFilePage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterDataFilePage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDataFilePage::~EnterDataFilePage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::setupGui()
{
	connectSignalsSlots();

	m_Ui->numOfRowsSB->setMinimum(1);
	m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->numOfColsSB->setMinimum(1);
	m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->tileOverlapSB->setMinimum(0);
	m_Ui->tileOverlapSB->setMaximum(100);

	// Disable both group boxes by default and only enable when needed
	m_Ui->configFileMetadata->setVisible(false);
	m_Ui->configFileMetadata->setDisabled(true);
	m_Ui->roboMetMetadata->setVisible(false);
	m_Ui->roboMetMetadata->setDisabled(true);
	m_Ui->dream3dMetadata->setVisible(false);
	m_Ui->dream3dMetadata->setDisabled(true);
	m_Ui->zeissMetadata->setVisible(false);
	m_Ui->zeissMetadata->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::connectSignalsSlots()
{
	connect(m_Ui->selectButton, &QPushButton::clicked, this, &EnterDataFilePage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &EnterDataFilePage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &EnterDataFilePage::dataFile_textChanged);

  connect(m_Ui->sliceNumberLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageFilePrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageFileSuffixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageFileExtensionLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->zeissDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissCellAttrMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissImageDataArrayLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissMetadataAttrxMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterDataFilePage::isComplete() const
{
  bool result = true;

  if (m_Ui->dataFileLE->isEnabled())
  {
    if (m_Ui->dataFileLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if (m_Ui->numOfRowsSB->isEnabled())
  {
	  if (m_Ui->numOfRowsSB->value() < m_Ui->numOfRowsSB->minimum() || m_Ui->numOfRowsSB->value() > m_Ui->numOfRowsSB->maximum())
	  {
		  result = false;
	  }
  }

  if (m_Ui->numOfColsSB->isEnabled())
  {
	  if (m_Ui->numOfColsSB->value() < m_Ui->numOfColsSB->minimum() || m_Ui->numOfColsSB->value() > m_Ui->numOfColsSB->maximum())
	  {
		  result = false;
	  }
  }

  if (m_Ui->tileOverlapSB->isEnabled())
  {
	  if (m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() || m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
	  {
		  result = false;
	  }
  }

  if (m_Ui->sliceNumberLE->isEnabled())
  {
	  if (m_Ui->sliceNumberLE->text().isEmpty())
	  {
		  result = false;
	  }
  }
   
  if (m_Ui->imageFilePrefixLE->isEnabled())
  {
	  if (m_Ui->imageFilePrefixLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if (m_Ui->imageFileExtensionLE->isEnabled())
  {
	  if (m_Ui->imageFileExtensionLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if(m_Ui->zeissDataContainerPrefixLE->isEnabled())
  {
	  if(m_Ui->zeissDataContainerPrefixLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if(m_Ui->zeissCellAttrMatrixLE->isEnabled())
  {
	  if(m_Ui->zeissCellAttrMatrixLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if(m_Ui->zeissImageDataArrayLE->isEnabled())
  {
	  if(m_Ui->zeissImageDataArrayLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  if(m_Ui->zeissMetadataAttrxMatrixLE->isEnabled())
  {
	  if(m_Ui->zeissMetadataAttrxMatrixLE->text().isEmpty())
	  {
		  result = false;
	  }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::registerFields()
{
  registerField(ImportMontage::FieldNames::DataFilePath, m_Ui->dataFileLE);

  // Fiji / RoboMet
  registerField(ImportMontage::FieldNames::NumberOfRows, m_Ui->numOfRowsSB);
  registerField(ImportMontage::FieldNames::NumberOfColumns, m_Ui->numOfColsSB);

  // RoboMet
  registerField(ImportMontage::FieldNames::SliceNumber, m_Ui->sliceNumberLE);
  registerField(ImportMontage::FieldNames::ImageFilePrefix, m_Ui->imageFilePrefixLE);
  registerField(ImportMontage::FieldNames::ImageFileSuffix, m_Ui->imageFileSuffixLE);
  registerField(ImportMontage::FieldNames::ImageFileExtension, m_Ui->imageFileExtensionLE);

  // DREAM3D
  registerField(ImportMontage::FieldNames::ImageDataContainerPrefix, m_Ui->imageDataContainerPrefixLE);
  registerField(ImportMontage::FieldNames::CellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(ImportMontage::FieldNames::ImageArrayName, m_Ui->imageArrayNameLE);
  registerField(ImportMontage::FieldNames::DREAM3DTileOverlap, m_Ui->tileOverlapSB);

  // Zeiss
  registerField(ImportMontage::FieldNames::ZeissDataContainerPrefix, m_Ui->zeissDataContainerPrefixLE);
  registerField(ImportMontage::FieldNames::ZeissCellAttributeMatrixName, m_Ui->zeissCellAttrMatrixLE);
  registerField(ImportMontage::FieldNames::ZeissImageDataArrayName, m_Ui->zeissImageDataArrayLE);
  registerField(ImportMontage::FieldNames::ZeissMetadataAttrMatrixName, m_Ui->zeissMetadataAttrxMatrixLE);
  registerField(ImportMontage::FieldNames::ZeissImportAllMetadata, m_Ui->importMetadataCB);
  registerField(ImportMontage::FieldNames::ZeissConvertToGrayscale, m_Ui->convertGrayscaleCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::selectBtn_clicked()
{
  bool usingConfigFile = field(ImportMontage::FieldNames::UsingConfigFile).toBool();

  QString filter;
  QString title;
  if (usingConfigFile)
  {
    filter = tr("Fiji Configuration File (*.csv *.txt);;Robomet Configuration File (*.csv);; \
		Zeiss Configuration File(*.xml);;All Files (*.*)");
    title = "Select a configuration file";
  }
  else
  {
    filter = tr("DREAM3D File (*.dream3d);;All Files (*.*)");
    title = "Select a DREAM3D file";
  }

  QString outputFile = QFileDialog::getOpenFileName(this, title,
		getInputDirectory(), filter);

	if (!outputFile.isNull())
	{
    m_Ui->dataFileLE->blockSignals(true);
    m_Ui->dataFileLE->setText(QDir::toNativeSeparators(outputFile));
    dataFile_textChanged(m_Ui->dataFileLE->text());
		setOpenDialogLastFilePath(outputFile);
    m_Ui->dataFileLE->blockSignals(false);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::dataFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString inputPath = validator->convertToAbsolutePath(text);

	// Disable group boxes by default and only enable when needed
	m_Ui->configFileMetadata->setVisible(false);
	m_Ui->configFileMetadata->setDisabled(true);
	m_Ui->roboMetMetadata->setVisible(false);
	m_Ui->roboMetMetadata->setDisabled(true);
	m_Ui->dream3dMetadata->setVisible(false);
	m_Ui->dream3dMetadata->setDisabled(true);
	m_Ui->zeissMetadata->setVisible(false);
	m_Ui->zeissMetadata->setDisabled(true);

  if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
	{
	}

  if (!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);

    if (ext == "dream3d")
    {
      setField("InputType", ImportMontageWizard::InputType::DREAM3D);
      setFinalPage(false);
	  m_Ui->configFileMetadata->setVisible(true);
	  m_Ui->configFileMetadata->setDisabled(false);
	  m_Ui->dream3dMetadata->setVisible(true);
	  m_Ui->dream3dMetadata->setDisabled(false);
    }
    else if (ext == "txt")
    {
      setField("InputType", ImportMontageWizard::InputType::Fiji);
      setFinalPage(false);
	  m_Ui->configFileMetadata->setVisible(true);
	  m_Ui->configFileMetadata->setDisabled(false);
    }
    else if (ext == "csv")
    {
      setField("InputType", ImportMontageWizard::InputType::Robomet);
      setFinalPage(false);
	  m_Ui->configFileMetadata->setVisible(true);
	  m_Ui->configFileMetadata->setDisabled(false);
	  m_Ui->roboMetMetadata->setVisible(true);
	  m_Ui->roboMetMetadata->setDisabled(false);
    }
	else if(ext == "xml")
	{
		setField("InputType", ImportMontageWizard::InputType::Zeiss);
		setFinalPage(false);
		m_Ui->configFileMetadata->setVisible(true);
		m_Ui->configFileMetadata->setDisabled(false);
		m_Ui->zeissMetadata->setVisible(true);
		m_Ui->zeissMetadata->setDisabled(false);
	}
    else
    {
      setField("InputType", ImportMontageWizard::InputType::Unknown);
      setFinalPage(false);
    }
  }

	emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDataFilePage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnterDataFilePage::getInputDirectory()
{
  if (m_Ui->dataFileLE->text().isEmpty())
	{
		return QDir::homePath();
	}
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterDataFilePage::nextId() const
{
  ImportMontageWizard::InputType inputType = field(ImportMontage::FieldNames::InputType).value<ImportMontageWizard::InputType>();
  if(inputType == ImportMontageWizard::InputType::DREAM3D)
  {
    return ImportMontageWizard::WizardPages::LoadHDF5Data;
  }
  else if(inputType == ImportMontageWizard::InputType::Fiji || inputType == ImportMontageWizard::InputType::Robomet ||
	  inputType == ImportMontageWizard::InputType::Zeiss)
  {
    return ImportMontageWizard::WizardPages::DataDisplayOptions;
  }
  else
  {
    return -1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterDataFilePage::validatePage()
{
  return true;
}
