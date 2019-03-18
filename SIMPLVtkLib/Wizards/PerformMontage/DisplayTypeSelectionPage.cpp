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

#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"

#include "PerformMontage/PerformMontageConstants.h"
#include "PerformMontage/PerformMontageWizard.h"

#include "PerformMontageWizard.h"

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
void DisplayTypeSelectionPage::initializePage()
{
  // Clear each combobox
  m_Ui->datasetCB->clear();

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
void DisplayTypeSelectionPage::connectSignalsSlots()
{
  connect(m_Ui->datasetCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
	emit completeChanged();
  });

  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, [=] {
	bool isChecked = m_Ui->changeOriginCB->isChecked();
	m_Ui->originX->setEnabled(isChecked);
	m_Ui->originY->setEnabled(isChecked);
	m_Ui->originZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &DisplayTypeSelectionPage::changeOrigin_stateChanged);
  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DisplayTypeSelectionPage::changeOrigin_stateChanged(int state)
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
void DisplayTypeSelectionPage::registerFields()
{
  registerField(PerformMontage::FieldNames::SelectedDataset, m_Ui->datasetCB);
  registerField(PerformMontage::FieldNames::ChangeOrigin, m_Ui->changeOriginCB);
  registerField(PerformMontage::FieldNames::ChangeSpacing, m_Ui->changeSpacingCB);
  registerField(PerformMontage::FieldNames::SpacingX, m_Ui->spacingX);
  registerField(PerformMontage::FieldNames::SpacingY, m_Ui->spacingY);
  registerField(PerformMontage::FieldNames::SpacingZ, m_Ui->spacingZ);
  registerField(PerformMontage::FieldNames::OriginX, m_Ui->originX);
  registerField(PerformMontage::FieldNames::OriginY, m_Ui->originY);
  registerField(PerformMontage::FieldNames::OriginZ, m_Ui->originZ);

  registerField(PerformMontage::FieldNames::DisplayMontage, m_Ui->displayMontageRB);
  registerField(PerformMontage::FieldNames::DisplayOutlineOnly, m_Ui->outlineOnlyRB);
  registerField(PerformMontage::FieldNames::StitchingOnly, m_Ui->stitchingOnlyCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DisplayTypeSelectionPage::nextId() const
{
	return -1;
}