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

#include "AdvancedPipelineExecutionPage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"

#include "ExecutePipeline/ExecutePipelineConstants.h"
#include "ExecutePipeline/ExecutePipelineWizard.h"

#include "ExecutePipelineWizard.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdvancedPipelineExecutionPage::AdvancedPipelineExecutionPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::AdvancedPipelineExecutionPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AdvancedPipelineExecutionPage::~AdvancedPipelineExecutionPage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::initializePage()
{
  // Clear each combobox
  m_Ui->startingFilterCB->clear();
  m_Ui->datasetCB->clear();

  // Get the list of all filters in the pipeline
  QString filePath = field(ExecutePipeline::FieldNames::PipelineFile)
	.toString();

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
	for(int i = 0; i < pipelineFromJson->getFilterContainer().size(); i++)
	{
	  AbstractFilter::Pointer filter = pipelineFromJson->getFilterContainer().at(i);
	  m_Ui->startingFilterCB->addItem(filter->getNameOfClass(), i);
	}
  }

  // Get the list of all loaded datasets
  VSMainWidgetBase* baseWidget = dynamic_cast<VSMainWidgetBase*>(wizard()->parentWidget());
  if(baseWidget != nullptr)
  {
	VSAbstractFilter::FilterListType datasets = baseWidget->getController()->getBaseFilters();
	int i = 0;
	for(VSAbstractFilter* dataset : datasets)
	{
	  m_Ui->datasetCB->addItem(dataset->getFilterName(), i++);
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::connectSignalsSlots()
{
  connect(m_Ui->startingFilterCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
	emit completeChanged();
  });

  connect(m_Ui->datasetCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
	emit completeChanged();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AdvancedPipelineExecutionPage::isComplete() const
{
  bool result = true;

  if(m_Ui->startingFilterCB->isEnabled())
  {
	if(m_Ui->startingFilterCB->currentIndex() < 0 || m_Ui->startingFilterCB->currentIndex() > m_Ui->startingFilterCB->maxCount() - 1)
	{
	  result = false;
	}
  }

  if(m_Ui->datasetCB->isEnabled())
  {
	if(m_Ui->datasetCB->currentIndex() < 0 || m_Ui->datasetCB->currentIndex() > m_Ui->datasetCB->maxCount() - 1)
	{
	  result = false;
	}
  }
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::registerFields()
{
  registerField(ExecutePipeline::FieldNames::StartFilter, m_Ui->startingFilterCB);
  registerField(ExecutePipeline::FieldNames::SelectedDataset, m_Ui->datasetCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AdvancedPipelineExecutionPage::nextId() const
{
	return -1;
}