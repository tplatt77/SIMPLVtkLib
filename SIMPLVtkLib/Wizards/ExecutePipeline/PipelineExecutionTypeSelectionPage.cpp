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

#include "PipelineExecutionTypeSelectionPage.h"
#include "ExecutePipelineWizard.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "ExecutePipeline/ExecutePipelineConstants.h"

// Initialize private static member variable
QString PipelineExecutionTypeSelectionPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineExecutionTypeSelectionPage::PipelineExecutionTypeSelectionPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::PipelineExecutionTypeSelectionPage)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineExecutionTypeSelectionPage::~PipelineExecutionTypeSelectionPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::setupGui()
{
  registerFields();
  
  connectSignalsSlots();

  // Set the default radio button selection
  m_Ui->pipelineFromDiskRB->setChecked(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::connectSignalsSlots()
{
  connect(m_Ui->selectButton, &QPushButton::clicked, this,
	&PipelineExecutionTypeSelectionPage::selectBtn_clicked);
  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->pipelineFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), 
	this, &PipelineExecutionTypeSelectionPage::pipelineFile_textChanged);
  connect(m_Ui->pipelineFileLE, &QLineEdit::textChanged, this,
	&PipelineExecutionTypeSelectionPage::pipelineFile_textChanged);

  connect(m_Ui->pipelineFromDiskRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
	  setField(ExecutePipeline::FieldNames::ExecutionType, ExecutePipelineWizard::ExecutionType::FromFilesystem);
    }
  });

  connect(m_Ui->pipelineOnLoadedDataRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ExecutePipeline::FieldNames::ExecutionType, ExecutePipelineWizard::ExecutionType::OnLoadedData);
	  setFinalPage(false);
    }
	else
	{
	  setFinalPage(true);
	}
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::pipelineFile_textChanged(const QString& text)
{
  Q_UNUSED(text)

	SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->pipelineFileLE))
  {
  }

  if(!inputPath.isEmpty())
  {
	QFileInfo fi(inputPath);
	QString ext = fi.completeSuffix();
	QMimeDatabase db;
	QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);

	setFinalPage(false);
  }

  QString filePath = m_Ui->pipelineFileLE->text();

  emit completeChanged();
  emit pipelineFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineExecutionTypeSelectionPage::isComplete() const
{
  bool result = true;

  if(m_Ui->pipelineFileLE->isEnabled())
  {
	if(m_Ui->pipelineFileLE->text().isEmpty())
	{
	  result = false;
	}
  }

  QFileInfo fi(m_Ui->pipelineFileLE->text());
  if(fi.completeSuffix() != "json")
  {
	result = false;
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::registerFields()
{
  registerField(ExecutePipeline::FieldNames::PipelineFile, m_Ui->pipelineFileLE);
  registerField(ExecutePipeline::FieldNames::ExecutionType, this, "ExecutionType");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::selectBtn_clicked()
{
  QString filter = tr("Pipeline JSON File (*.json);;All Files (*.*)");
  QString title = "Select a pipeline JSON file";

  QString outputFile = QFileDialog::getOpenFileName(this, title,
	getInputDirectory(), filter);

  if(!outputFile.isNull())
  {
	m_Ui->pipelineFileLE->blockSignals(true);
	m_Ui->pipelineFileLE->setText(QDir::toNativeSeparators(outputFile));
	pipelineFile_textChanged(m_Ui->pipelineFileLE->text());
	setOpenDialogLastFilePath(outputFile);
	m_Ui->pipelineFileLE->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineExecutionTypeSelectionPage::setInputDirectory(QString val)
{
  m_Ui->pipelineFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineExecutionTypeSelectionPage::getInputDirectory()
{
  if(m_Ui->pipelineFileLE->text().isEmpty())
  {
	return QDir::homePath();
  }
  return m_Ui->pipelineFileLE->text();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineExecutionTypeSelectionPage::nextId() const
{
  if (m_Ui->pipelineFromDiskRB->isChecked())
  {
	emit m_Ui->pipelineFromDiskRB->toggled(true);
	return -1;
  }
  if (m_Ui->pipelineOnLoadedDataRB->isChecked())
  {
	return ExecutePipelineWizard::WizardPages::AdvancedPipelineExecution;
  }
  else
  {
	return -1;
  }
}

