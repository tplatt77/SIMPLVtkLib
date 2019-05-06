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
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeDatabase>

#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

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
  connectSignalsSlots();

  m_Ui->originX->setValidator(new QDoubleValidator);
  m_Ui->originY->setValidator(new QDoubleValidator);
  m_Ui->originZ->setValidator(new QDoubleValidator);
  m_Ui->spacingX->setValidator(new QDoubleValidator);
  m_Ui->spacingY->setValidator(new QDoubleValidator);
  m_Ui->spacingZ->setValidator(new QDoubleValidator);

  // Store the advancedGridLayout inside the QtSDisclosableWidget
  QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
  advancedGB->setTitle("Advanced");
  QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
  advancedGridLayout->setParent(nullptr);
  advancedGB->setContentLayout(advancedGridLayout);

  m_Ui->gridLayout->addWidget(advancedGB, 5, 0, 1, 4);

  // Set the default radio button selection
  m_Ui->displayMontageRB->setChecked(true);
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
  QString filePath = field(ExecutePipeline::FieldNames::PipelineFile).toString();

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
  connect(m_Ui->startingFilterCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) { emit completeChanged(); });

  connect(m_Ui->datasetCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) { emit completeChanged(); });

  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeOriginCB->isChecked();
    m_Ui->originX->setEnabled(isChecked);
    m_Ui->originY->setEnabled(isChecked);
    m_Ui->originZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &AdvancedPipelineExecutionPage::changeOrigin_stateChanged);
  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeSpacingCB->isChecked();
    m_Ui->spacingX->setEnabled(isChecked);
    m_Ui->spacingY->setEnabled(isChecked);
    m_Ui->spacingZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &AdvancedPipelineExecutionPage::changeSpacing_stateChanged);
  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state);
  m_Ui->originY->setEnabled(state);
  m_Ui->originZ->setEnabled(state);
  if(state == false)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::changeSpacing_stateChanged(int state)
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

  if(m_Ui->originX->isEnabled())
  {
    if(m_Ui->originX->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originY->isEnabled())
  {
    if(m_Ui->originY->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originZ->isEnabled())
  {
    if(m_Ui->originZ->text().isEmpty())
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
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AdvancedPipelineExecutionPage::registerFields()
{
  registerField(ExecutePipeline::FieldNames::StartFilter, m_Ui->startingFilterCB);
  registerField(ExecutePipeline::FieldNames::SelectedDataset, m_Ui->datasetCB);
  registerField(ExecutePipeline::FieldNames::ChangeOrigin, m_Ui->changeOriginCB);
  registerField(ExecutePipeline::FieldNames::ChangeSpacing, m_Ui->changeSpacingCB);
  registerField(ExecutePipeline::FieldNames::SpacingX, m_Ui->spacingX);
  registerField(ExecutePipeline::FieldNames::SpacingY, m_Ui->spacingY);
  registerField(ExecutePipeline::FieldNames::SpacingZ, m_Ui->spacingZ);
  registerField(ExecutePipeline::FieldNames::OriginX, m_Ui->originX);
  registerField(ExecutePipeline::FieldNames::OriginY, m_Ui->originY);
  registerField(ExecutePipeline::FieldNames::OriginZ, m_Ui->originZ);

  registerField(ExecutePipeline::FieldNames::DisplayMontage, m_Ui->displayMontageRB);
  registerField(ExecutePipeline::FieldNames::DisplaySideBySide, m_Ui->sideBySideRB);
  registerField(ExecutePipeline::FieldNames::DisplayOutlineOnly, m_Ui->outlineOnlyRB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AdvancedPipelineExecutionPage::nextId() const
{
  return -1;
}