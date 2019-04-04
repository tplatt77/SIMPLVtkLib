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

#include "DisplayTypeSelectionPage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"

#include "PerformMontage/PerformMontageConstants.h"
#include "PerformMontage/PerformMontageWizard.h"

#include "PerformMontageWizard.h"

// Initialize private static member variable
QString DisplayTypeSelectionPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DisplayTypeSelectionPage::DisplayTypeSelectionPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::DisplayTypeSelectionPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DisplayTypeSelectionPage::~DisplayTypeSelectionPage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::setupGui()
{
  qRegisterMetaType<DatasetListInfo_t>();
  connectSignalsSlots();
  m_Ui->spacingX->setValidator(new QDoubleValidator);
  m_Ui->spacingY->setValidator(new QDoubleValidator);
  m_Ui->spacingZ->setValidator(new QDoubleValidator);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::initializePage()
{
  // Clear each combobox
  m_Ui->datasetsListWidget->clear();

  // Get the list of all loaded datasets
  VSMainWidgetBase* baseWidget = dynamic_cast<VSMainWidgetBase*>(wizard()->parentWidget());
  if(baseWidget != nullptr)
  {
	VSAbstractFilter::FilterListType datasets = baseWidget->getController()->getBaseFilters();
	VSAbstractFilter::FilterListType selectedDatasets = baseWidget->getActiveViewWidget()->getSelectedFilters();
	int i = 0;
	for(VSAbstractFilter* dataset : datasets)
	{
	  m_Ui->datasetsListWidget->addDataset(dataset);
	  bool isSelected = false;
	  for(VSAbstractFilter* selectedDataset : selectedDatasets)
	  {
		for(VSAbstractFilter* childFilter : dataset->getChildren())
		{
		  if(childFilter == selectedDataset || dataset == selectedDataset)
		  {
			m_Ui->datasetsListWidget->setSelected(i, true);
			isSelected = true;
			break;
		  }
		}
		if(isSelected)
		{
		  break;
		}
	  }
	  i++;
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::connectSignalsSlots()
{
  connect(m_Ui->datasetsListWidget, static_cast<void (DatasetListWidget::*)()>(&DatasetListWidget::itemSelectionChanged), [=]() {
	emit completeChanged();
  });

  connect(m_Ui->datasetsListWidget, &DatasetListWidget::itemSelectionChanged,
	this, &DisplayTypeSelectionPage::datasetListWidgetChanged);

  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, [=] {
	bool isChecked = m_Ui->changeSpacingCB->isChecked();
	m_Ui->spacingX->setEnabled(isChecked);
	m_Ui->spacingY->setEnabled(isChecked);
	m_Ui->spacingZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &DisplayTypeSelectionPage::changeSpacing_stateChanged);
  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->selectButton, &QPushButton::clicked, this, &DisplayTypeSelectionPage::selectBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &DisplayTypeSelectionPage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &DisplayTypeSelectionPage::dataFile_textChanged);

  connect(m_Ui->saveToFileCB, &QCheckBox::toggled, [=](bool checked)
  {
	m_Ui->dataFileLE->setEnabled(checked);
	m_Ui->selectButton->setEnabled(checked);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::selectBtn_clicked()
{
  QString filter = tr("Image File (*.png *.tiff *.jpeg *.bmp);;All Files (*.*)");
  QString title = "Save as Image file";

  QString outputFile = QFileDialog::getSaveFileName(this, title,
	getInputDirectory(), filter);

  if(!outputFile.isNull())
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
void DisplayTypeSelectionPage::dataFile_textChanged(const QString& text)
{
  Q_UNUSED(text)

	SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString outputPath = validator->convertToAbsolutePath(text);

  emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DisplayTypeSelectionPage::getInputDirectory()
{
  if(m_Ui->dataFileLE->text().isEmpty())
  {
	return QDir::homePath();
  }
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::datasetListWidgetChanged()
{
  DatasetListInfo_t datasetListInfo = m_Ui->datasetsListWidget->getDatasetListInfo();
  setDatasetListInfo(datasetListInfo);

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::changeSpacing_stateChanged(int state)
{
  m_Ui->spacingX->setEnabled(state);
  m_Ui->spacingY->setEnabled(state);
  m_Ui->spacingZ->setEnabled(state);
  if(state == false)
  {
	m_Ui->spacingX->setText("1");
	m_Ui->spacingY->setText("1");
	m_Ui->spacingZ->setText("1");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DisplayTypeSelectionPage::isComplete() const
{
  bool result = true;
  if(m_Ui->datasetsListWidget->isEnabled())
  {
	if(m_Ui->datasetsListWidget->selectedItems().isEmpty())
	{
	  result = false;
	}
  }

  if(m_Ui->spacingX->isEnabled())
  {
	if(m_Ui->spacingX->text().isEmpty())
	{
	  result = false;
	}
  }

  if(m_Ui->spacingY->isEnabled())
  {
	if(m_Ui->spacingY->text().isEmpty())
	{
	  result = false;
	}
  }

  if(m_Ui->spacingZ->isEnabled())
  {
	if(m_Ui->spacingZ->text().isEmpty())
	{
	  result = false;
	}
  }

  if(m_Ui->dataFileLE->isEnabled())
  {
	if(m_Ui->dataFileLE->text().isEmpty())
	{
	  result = false;
	}
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::registerFields()
{
  registerField(PerformMontage::FieldNames::SaveToFile, m_Ui->saveToFileCB);
  registerField(PerformMontage::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(PerformMontage::FieldNames::SelectedDataset,
	this, "DatasetListInfo");
  registerField(PerformMontage::FieldNames::ChangeSpacing, m_Ui->changeSpacingCB);
  registerField(PerformMontage::FieldNames::SpacingX, m_Ui->spacingX);
  registerField(PerformMontage::FieldNames::SpacingY, m_Ui->spacingY);
  registerField(PerformMontage::FieldNames::SpacingZ, m_Ui->spacingZ); 

  registerField(PerformMontage::FieldNames::StitchingOnly, m_Ui->stitchingOnlyCB);
  registerField(PerformMontage::FieldNames::OutputFilePath, m_Ui->dataFileLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DisplayTypeSelectionPage::nextId() const
{
  return -1;
}