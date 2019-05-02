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

#include "PerformMontageDialog.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString PerformMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PerformMontageDialog::PerformMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::PerformMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PerformMontageDialog::~PerformMontageDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PerformMontageDialog::Pointer PerformMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new PerformMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::setupGui()
{
  qRegisterMetaType<DatasetListInfo_t>();
  initializeDatasetList();
  connectSignalsSlots();

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::initializeDatasetList()
{
  // Clear each combobox
  m_Ui->datasetsListWidget->clear();

  // Get the list of all loaded datasets
  VSMainWidgetBase* baseWidget = dynamic_cast<VSMainWidgetBase*>(this->parentWidget());
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
            datasetListWidgetChanged();
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
void PerformMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->datasetsListWidget, &DatasetListWidget::itemSelectionChanged, this, &PerformMontageDialog::datasetListWidgetChanged);

  connect(m_Ui->saveToFileCB, &QCheckBox::toggled, [=](bool checked) {
    m_Ui->dataFileLE->setEnabled(checked);
    m_Ui->selectButton->setEnabled(checked);
  });

  connect(m_Ui->selectButton, &QPushButton::clicked, this, &PerformMontageDialog::selectBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &PerformMontageDialog::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &PerformMontageDialog::dataFile_textChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::disconnectSignalsSlots()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::datasetListWidgetChanged()
{
  DatasetListInfo_t datasetListInfo = m_Ui->datasetsListWidget->getDatasetListInfo();
  setDatasetListInfo(datasetListInfo);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::selectBtn_clicked()
{
  QString filter = tr("Image File (*.png *.tiff *.jpeg *.bmp);;All Files (*.*)");
  QString title = "Save as Image file";

  QString outputFile = QFileDialog::getSaveFileName(this, title, getInputDirectory(), filter);

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
void PerformMontageDialog::dataFile_textChanged(const QString& text)
{
  Q_UNUSED(text)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString outputPath = validator->convertToAbsolutePath(text);

  checkComplete();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PerformMontageDialog::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PerformMontageDialog::getInputDirectory()
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
void PerformMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->datasetsListWidget->isEnabled())
  {
    if(m_Ui->datasetsListWidget->selectedItems().isEmpty())
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

  QPushButton* okBtn = m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
  if(!okBtn)
  {
    throw InvalidOKButtonException();
  }

  okBtn->setEnabled(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PerformMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PerformMontageDialog::getOutputPath()
{
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PerformMontageDialog::getSaveToFile()
{
  return m_Ui->saveToFileCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PerformMontageDialog::getStitchingOnly()
{
  return m_Ui->stitchingOnlyCB->isChecked();
}

