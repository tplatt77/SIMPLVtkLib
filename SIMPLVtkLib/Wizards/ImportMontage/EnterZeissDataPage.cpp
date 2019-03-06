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

#include "EnterZeissDataPage.h"

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
QString EnterZeissDataPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterZeissDataPage::EnterZeissDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterZeissDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterZeissDataPage::~EnterZeissDataPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::setupGui()
{
  connectSignalsSlots();

  m_Ui->colorWeightingR->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingG->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingB->setValidator(new QDoubleValidator);
  m_Ui->originX->setValidator(new QDoubleValidator);
  m_Ui->originY->setValidator(new QDoubleValidator);
  m_Ui->originZ->setValidator(new QDoubleValidator);
  m_Ui->spacingX->setValidator(new QDoubleValidator);
  m_Ui->spacingY->setValidator(new QDoubleValidator);
  m_Ui->spacingZ->setValidator(new QDoubleValidator);

  QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
  advancedGB->setTitle("Advanced");
  QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
  advancedGridLayout->setParent(nullptr);
  advancedGB->setContentLayout(advancedGridLayout);

  m_Ui->gridLayout->addWidget(advancedGB, 11, 1, 1, 5);

  m_Ui->montageNameLE->setText("UntitledMontage");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::connectSignalsSlots()
{

  connect(m_Ui->zeissListWidget, &ZeissListWidget::inputDirectoryChanged, this, &EnterZeissDataPage::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissListWidget::numberOfRowsChanged, this, &EnterZeissDataPage::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissListWidget::numberOfColumnsChanged, this, &EnterZeissDataPage::zeissListWidgetChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, &EnterZeissDataPage::montageName_textChanged);

  connect(m_Ui->zeissDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissCellAttrMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissImageDataArrayLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->zeissMetadataAttrxMatrixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

  connect(m_Ui->colorWeightingR, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->colorWeightingG, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->colorWeightingB, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

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

  connect(m_Ui->manualDCAElementNamesCB, &QCheckBox::stateChanged, this, &EnterZeissDataPage::manualDCAElementNames_stateChanged);
  connect(m_Ui->changeTileOverlapCB, &QCheckBox::stateChanged, this, &EnterZeissDataPage::changeTileOverlap_stateChanged);
  connect(m_Ui->convertGrayscaleCB, &QCheckBox::stateChanged, this, &EnterZeissDataPage::convertGrayscale_stateChanged);
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &EnterZeissDataPage::changeOrigin_stateChanged);
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &EnterZeissDataPage::changeSpacing_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::zeissListWidgetChanged()
{
	ZeissListInfo_t zeissListInfo = m_Ui->zeissListWidget->getZeissListInfo();
	setZeissListInfo(zeissListInfo);

	emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::changeTileOverlap_stateChanged(int state)
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
void EnterZeissDataPage::convertGrayscale_stateChanged(int state)
{
  m_Ui->colorWeightingR->setEnabled(state);
  m_Ui->colorWeightingG->setEnabled(state);
  m_Ui->colorWeightingB->setEnabled(state);
  if (state == false)
  {
    m_Ui->colorWeightingR->setText("0.2125");
    m_Ui->colorWeightingG->setText("0.7154");
    m_Ui->colorWeightingB->setText("0.0721");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state);
  m_Ui->originY->setEnabled(state);
  m_Ui->originZ->setEnabled(state);
  if (state == false)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::changeSpacing_stateChanged(int state)
{
  m_Ui->spacingX->setEnabled(state);
  m_Ui->spacingY->setEnabled(state);
  m_Ui->spacingZ->setEnabled(state);
  if (state == false)
  {
    m_Ui->spacingX->setText("1");
    m_Ui->spacingY->setText("1");
    m_Ui->spacingZ->setText("1");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterZeissDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
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

  if (m_Ui->tileOverlapSB->isEnabled())
  {
    if (m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() || m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingR->isEnabled())
  {
    if(m_Ui->colorWeightingR->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingG->isEnabled())
  {
    if(m_Ui->colorWeightingG->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->colorWeightingB->isEnabled())
  {
    if(m_Ui->colorWeightingB->text().isEmpty())
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
void EnterZeissDataPage::registerFields()
{
  registerField(ImportMontage::Zeiss::FieldNames::MontageName, m_Ui->montageNameLE);

  registerField(ImportMontage::Zeiss::FieldNames::DataContainerPrefix, m_Ui->zeissDataContainerPrefixLE);
  registerField(ImportMontage::Zeiss::FieldNames::CellAttributeMatrixName, m_Ui->zeissCellAttrMatrixLE);
  registerField(ImportMontage::Zeiss::FieldNames::ImageDataArrayName, m_Ui->zeissImageDataArrayLE);
  registerField(ImportMontage::Zeiss::FieldNames::MetadataAttrMatrixName, m_Ui->zeissMetadataAttrxMatrixLE);
  registerField(ImportMontage::Zeiss::FieldNames::ConvertToGrayscale, m_Ui->convertGrayscaleCB);
  registerField(ImportMontage::Zeiss::FieldNames::ChangeOrigin, m_Ui->changeOriginCB);
  registerField(ImportMontage::Zeiss::FieldNames::ChangeSpacing, m_Ui->changeSpacingCB);
  registerField(ImportMontage::Zeiss::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
  registerField(ImportMontage::Zeiss::FieldNames::ChangeTileOverlap, m_Ui->changeTileOverlapCB);

  registerField(ImportMontage::Zeiss::FieldNames::ColorWeightingR, m_Ui->colorWeightingR);
  registerField(ImportMontage::Zeiss::FieldNames::ColorWeightingG, m_Ui->colorWeightingG);
  registerField(ImportMontage::Zeiss::FieldNames::ColorWeightingB, m_Ui->colorWeightingB);
  registerField(ImportMontage::Zeiss::FieldNames::OriginX, m_Ui->originX);
  registerField(ImportMontage::Zeiss::FieldNames::OriginY, m_Ui->originY);
  registerField(ImportMontage::Zeiss::FieldNames::OriginZ, m_Ui->originZ);
  registerField(ImportMontage::Zeiss::FieldNames::SpacingX, m_Ui->spacingX);
  registerField(ImportMontage::Zeiss::FieldNames::SpacingY, m_Ui->spacingY);
  registerField(ImportMontage::Zeiss::FieldNames::SpacingZ, m_Ui->spacingZ);
  registerField(ImportMontage::Zeiss::FieldNames::ZeissListInfo, this, "ZeissListInfo");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::manualDCAElementNames_stateChanged(int state)
{
  bool isChecked = m_Ui->manualDCAElementNamesCB->isChecked();
  m_Ui->zeissDataContainerPrefixLE->setEnabled(isChecked);
  m_Ui->zeissCellAttrMatrixLE->setEnabled(isChecked);
  m_Ui->zeissImageDataArrayLE->setEnabled(isChecked);
  m_Ui->zeissMetadataAttrxMatrixLE->setEnabled(isChecked);

  montageName_textChanged(m_Ui->montageNameLE->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::montageName_textChanged(const QString &text)
{
  if (!m_Ui->manualDCAElementNamesCB->isChecked())
  {
    m_Ui->zeissDataContainerPrefixLE->setText(tr("%1_").arg(text));
    m_Ui->zeissCellAttrMatrixLE->setText(tr("Cell Attribute Matrix"));
    m_Ui->zeissImageDataArrayLE->setText(tr("Image Data"));
    m_Ui->zeissMetadataAttrxMatrixLE->setText(tr("Metadata Attribute Matrix"));
  }

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterZeissDataPage::nextId() const
{
  return ImportMontageWizard::WizardPages::DataDisplayOptions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterZeissDataPage::validatePage()
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterZeissDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}
