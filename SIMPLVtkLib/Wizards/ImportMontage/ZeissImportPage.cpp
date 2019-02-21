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

#include "ZeissImportPage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

#include "ImportMontage/ImportMontageConstants.h"

#include "ImportMontageWizard.h"

// Initialize private static member variable
QString ZeissImportPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ZeissImportPage::ZeissImportPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::ZeissImportPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ZeissImportPage::~ZeissImportPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::setupGui()
{
  connectSignalsSlots();

  QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
  advancedGB->setTitle("Advanced");
  QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
  advancedGridLayout->setParent(nullptr);
  advancedGB->setContentLayout(advancedGridLayout);

  m_Ui->gridLayout->addWidget(advancedGB, 10, 1, 1, 5);

  m_Ui->zeissMontageNameLE->setText("UntitledMontage");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::connectSignalsSlots()
{
	connect(m_Ui->selectButton, &QPushButton::clicked, this, &ZeissImportPage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->zeissDataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &ZeissImportPage::dataFile_textChanged);
  connect(m_Ui->zeissDataFileLE, &QLineEdit::textChanged, this, &ZeissImportPage::dataFile_textChanged);

  connect(m_Ui->zeissMontageNameLE, &QLineEdit::textChanged, this, &ZeissImportPage::montageName_textChanged);

  connect(m_Ui->zeissDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissCellAttrMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissImageDataArrayLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissMetadataAttrxMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  // Connect the checkboxes for grayscale, origin, and spacing to respective line edit group boxes
  connect(m_Ui->convertGrayscaleCB, &QCheckBox::stateChanged, [=] {
	  bool isChecked = m_Ui->convertGrayscaleCB->isChecked();
    m_Ui->colorWeightingR->setEnabled(isChecked);
    m_Ui->colorWeightingG->setEnabled(isChecked);
    m_Ui->colorWeightingB->setEnabled(isChecked);
  });
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, [=] {
	  bool isChecked = m_Ui->changeOriginCB->isChecked();
    m_Ui->originX->setEnabled(isChecked);
    m_Ui->originY->setEnabled(isChecked);
    m_Ui->originZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, [=] {
	  bool isChecked = m_Ui->changeSpacingCB->isChecked();
    m_Ui->spacingX->setEnabled(isChecked);
    m_Ui->spacingY->setEnabled(isChecked);
    m_Ui->spacingZ->setEnabled(isChecked);
  });

  connect(m_Ui->manualDCAElementNamesCB, &QCheckBox::stateChanged, this, &ZeissImportPage::manualDCAElementNames_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ZeissImportPage::isComplete() const
{
  bool result = true;

  if (m_Ui->zeissMontageNameLE->text().isEmpty())
  {
    result = false;
  }

  if (m_Ui->zeissDataFileLE->isEnabled())
  {
    if (m_Ui->zeissDataFileLE->text().isEmpty())
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
void ZeissImportPage::registerFields()
{
  registerField(ImportMontage::FieldNames::ZeissDataFilePath, m_Ui->zeissDataFileLE);

  registerField(ImportMontage::FieldNames::ZeissDataContainerPrefix, m_Ui->zeissDataContainerPrefixLE);
  registerField(ImportMontage::FieldNames::ZeissCellAttributeMatrixName, m_Ui->zeissCellAttrMatrixLE);
  registerField(ImportMontage::FieldNames::ZeissImageDataArrayName, m_Ui->zeissImageDataArrayLE);
  registerField(ImportMontage::FieldNames::ZeissMetadataAttrMatrixName, m_Ui->zeissMetadataAttrxMatrixLE);
  registerField(ImportMontage::FieldNames::ZeissConvertToGrayscale, m_Ui->convertGrayscaleCB);
  registerField(ImportMontage::FieldNames::ZeissChangeOrigin, m_Ui->changeOriginCB);
  registerField(ImportMontage::FieldNames::ZeissChangeSpacing, m_Ui->changeSpacingCB);

  registerField(ImportMontage::FieldNames::ZeissColorWeightingR, m_Ui->colorWeightingR);
  registerField(ImportMontage::FieldNames::ZeissColorWeightingG, m_Ui->colorWeightingG);
  registerField(ImportMontage::FieldNames::ZeissColorWeightingB, m_Ui->colorWeightingB);
  registerField(ImportMontage::FieldNames::ZeissOriginX, m_Ui->originX);
  registerField(ImportMontage::FieldNames::ZeissOriginY, m_Ui->originY);
  registerField(ImportMontage::FieldNames::ZeissOriginZ, m_Ui->originZ);
  registerField(ImportMontage::FieldNames::ZeissSpacingX, m_Ui->spacingX);
  registerField(ImportMontage::FieldNames::ZeissSpacingY, m_Ui->spacingY);
  registerField(ImportMontage::FieldNames::ZeissSpacingZ, m_Ui->spacingZ);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::selectBtn_clicked()
{
  bool usingConfigFile = field(ImportMontage::FieldNames::UsingConfigFile).toBool();

  QString filter = tr("Zeiss Configuration File(*.xml);;All Files (*.*)");
  QString title = "Select a configuration file";

  QString outputFile = QFileDialog::getOpenFileName(this, title,
		getInputDirectory(), filter);

	if (!outputFile.isNull())
	{
    m_Ui->zeissDataFileLE->blockSignals(true);
    m_Ui->zeissDataFileLE->setText(QDir::toNativeSeparators(outputFile));
    dataFile_textChanged(m_Ui->zeissDataFileLE->text());
		setOpenDialogLastFilePath(outputFile);
    m_Ui->zeissDataFileLE->blockSignals(false);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::manualDCAElementNames_stateChanged(int state)
{
  bool isChecked = m_Ui->manualDCAElementNamesCB->isChecked();
  m_Ui->zeissDataContainerPrefixLE->setEnabled(isChecked);
  m_Ui->zeissCellAttrMatrixLE->setEnabled(isChecked);
  m_Ui->zeissImageDataArrayLE->setEnabled(isChecked);
  m_Ui->zeissMetadataAttrxMatrixLE->setEnabled(isChecked);

  montageName_textChanged(m_Ui->zeissMontageNameLE->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::montageName_textChanged(const QString &text)
{
  if (!m_Ui->manualDCAElementNamesCB->isChecked())
  {
    m_Ui->zeissDataContainerPrefixLE->setText(tr("%1_").arg(text));
    m_Ui->zeissCellAttrMatrixLE->setText(tr("Cell Attribute Matrix"));
    m_Ui->zeissImageDataArrayLE->setText(tr("Image Data"));
    m_Ui->zeissMetadataAttrxMatrixLE->setText(tr("Metadata Attribute Matrix"));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ZeissImportPage::dataFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString inputPath = validator->convertToAbsolutePath(text);

//	// Disable optional group boxes by default and only enable when needed
//	m_Ui->colorWeighting->setDisabled(true);
//	m_Ui->newOrigin->setDisabled(true);
//	m_Ui->newSpacing->setDisabled(true);

  if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->zeissDataFileLE))
	{
	}

  if (!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);

    if (ext == "xml")
	{
		setField("InputType", ImportMontageWizard::InputType::Zeiss);
		setFinalPage(false);
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
void ZeissImportPage::setInputDirectory(QString val)
{
  m_Ui->zeissDataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ZeissImportPage::getInputDirectory()
{
  if (m_Ui->zeissDataFileLE->text().isEmpty())
	{
		return QDir::homePath();
	}
  return m_Ui->zeissDataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ZeissImportPage::nextId() const
{
  ImportMontageWizard::InputType inputType = field(ImportMontage::FieldNames::InputType).value<ImportMontageWizard::InputType>();
  if(inputType == ImportMontageWizard::InputType::Zeiss)
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
bool ZeissImportPage::validatePage()
{
  return true;
}
