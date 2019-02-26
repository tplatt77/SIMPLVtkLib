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

#include "EnterRobometDataPage.h"

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
QString EnterRobometDataPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterRobometDataPage::EnterRobometDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterRobometDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterRobometDataPage::~EnterRobometDataPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::setupGui()
{
	connectSignalsSlots();

	m_Ui->numOfRowsSB->setMinimum(1);
	m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->numOfColsSB->setMinimum(1);
	m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

  // Store the advancedGridLayout inside the QtSDisclosableWidget
  QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
  advancedGB->setTitle("Advanced");
  QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
  advancedGridLayout->setParent(nullptr);
  advancedGB->setContentLayout(advancedGridLayout);

  m_Ui->gridLayout->addWidget(advancedGB, 8, 0, 1, 4);
}

QPair<int,int> gridPosition(QWidget* widget)
{
  auto gp = qMakePair(-1,-1);
  if (!widget->parentWidget())
  {
    return gp;
  }

  auto layout = dynamic_cast<QGridLayout*>(widget->parentWidget()->layout());
  if (!layout)
  {
    return gp;
  }

  int index = layout->indexOf(widget);
  if (index < 0)
  {
    return gp;
  }

  int rs,cs;
  layout->getItemPosition(index, &gp.first, &gp.second, &rs, &cs);
  return gp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::connectSignalsSlots()
{
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &EnterRobometDataPage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &EnterRobometDataPage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &EnterRobometDataPage::dataFile_textChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, &EnterRobometDataPage::montageName_textChanged);

  connect(m_Ui->imageFilePrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageFileSuffixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageFileExtensionLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->sliceNumberSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

  connect(m_Ui->changeTileOverlapCB, &QCheckBox::stateChanged, this, &EnterRobometDataPage::changeTileOverlap_stateChanged);
  connect(m_Ui->manualDCAElementNamesCB, &QCheckBox::stateChanged, this, &EnterRobometDataPage::manualDCAElementNames_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::changeTileOverlap_stateChanged(int state)
{
  m_Ui->tileOverlapSB->setEnabled(state);
  if (state == false)
  {
    m_Ui->tileOverlapSB->setValue(15);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::manualDCAElementNames_stateChanged(int state)
{
  bool isChecked = m_Ui->manualDCAElementNamesCB->isChecked();
  m_Ui->imageDataContainerPrefixLE->setEnabled(isChecked);
  m_Ui->cellAttrMatrixNameLE->setEnabled(isChecked);
  m_Ui->imageArrayNameLE->setEnabled(isChecked);

  montageName_textChanged(m_Ui->montageNameLE->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::montageName_textChanged(const QString &text)
{
  if (!m_Ui->manualDCAElementNamesCB->isChecked())
  {
    m_Ui->imageDataContainerPrefixLE->setText(tr("%1_").arg(text));
    m_Ui->cellAttrMatrixNameLE->setText(tr("Cell Attribute Matrix"));
    m_Ui->imageArrayNameLE->setText(tr("Image Data"));
  }

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterRobometDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->dataFileLE->isEnabled())
  {
    if (m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  QFileInfo fi(m_Ui->dataFileLE->text());
  if (fi.completeSuffix() != "csv")
  {
    result = false;
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

  if (m_Ui->sliceNumberSB->isEnabled())
  {
    if (m_Ui->sliceNumberSB->value() < m_Ui->sliceNumberSB->minimum() || m_Ui->sliceNumberSB->value() > m_Ui->sliceNumberSB->maximum())
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

  if (m_Ui->imageDataContainerPrefixLE->isEnabled())
  {
    if (m_Ui->imageDataContainerPrefixLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->cellAttrMatrixNameLE->isEnabled())
  {
    if (m_Ui->cellAttrMatrixNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->imageArrayNameLE->isEnabled())
  {
    if (m_Ui->imageArrayNameLE->text().isEmpty())
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

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::registerFields()
{
  registerField(ImportMontage::Robomet::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(ImportMontage::Robomet::FieldNames::DataFilePath, m_Ui->dataFileLE);
  registerField(ImportMontage::Robomet::FieldNames::NumberOfRows, m_Ui->numOfRowsSB);
  registerField(ImportMontage::Robomet::FieldNames::NumberOfColumns, m_Ui->numOfColsSB);
  registerField(ImportMontage::Robomet::FieldNames::SliceNumber, m_Ui->sliceNumberSB);
  registerField(ImportMontage::Robomet::FieldNames::ImageFilePrefix, m_Ui->imageFilePrefixLE);
  registerField(ImportMontage::Robomet::FieldNames::ImageFileSuffix, m_Ui->imageFileSuffixLE);
  registerField(ImportMontage::Robomet::FieldNames::ImageFileExtension, m_Ui->imageFileExtensionLE);
  registerField(ImportMontage::Robomet::FieldNames::DataContainerPrefix, m_Ui->imageDataContainerPrefixLE);
  registerField(ImportMontage::Robomet::FieldNames::CellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(ImportMontage::Robomet::FieldNames::ImageArrayName, m_Ui->imageArrayNameLE);
  registerField(ImportMontage::Robomet::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
  registerField(ImportMontage::Robomet::FieldNames::ChangeTileOverlap, m_Ui->changeTileOverlapCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::selectBtn_clicked()
{
  QString filter = tr("Robomet Configuration File (*.csv);;All Files (*.*)");
  QString title = "Select a Robomet configuration file";

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
void EnterRobometDataPage::dataFile_textChanged(const QString& text)
{
  Q_UNUSED(text)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
  {
  }

  if (!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);
  }

  emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnterRobometDataPage::getInputDirectory()
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
int EnterRobometDataPage::nextId() const
{
  return ImportMontageWizard::WizardPages::DataDisplayOptions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterRobometDataPage::validatePage()
{
  return true;
}
