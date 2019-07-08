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

#include "ImportZeissZenMontageDialog.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString ImportZeissZenMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissZenMontageDialog::ImportZeissZenMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::ImportZeissZenMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissZenMontageDialog::~ImportZeissZenMontageDialog() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportZeissZenMontageDialog::Pointer ImportZeissZenMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportZeissZenMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  qRegisterMetaType<ZeissZenListInfo_t>();

  connectSignalsSlots();

  SVStyle* style = SVStyle::Instance();
  m_Ui->errLabel->setStyleSheet(tr("QLabel { color: %1; }").arg(style->getWidget_Error_color().name()));

  m_Ui->colorWeightingR->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingG->setValidator(new QDoubleValidator);
  m_Ui->colorWeightingB->setValidator(new QDoubleValidator);
  m_Ui->originX->setValidator(new QDoubleValidator);
  m_Ui->originY->setValidator(new QDoubleValidator);
  m_Ui->originZ->setValidator(new QDoubleValidator);

  m_Ui->montageStartX->setValidator(new QDoubleValidator);
  m_Ui->montageStartY->setValidator(new QDoubleValidator);
  m_Ui->montageEndX->setValidator(new QDoubleValidator);
  m_Ui->montageEndY->setValidator(new QDoubleValidator);

  m_Ui->montageNameLE->setText("Untitled Montage");

  setDisplayType(AbstractImportMontageDialog::DisplayType::Outline);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->dataDisplayTypeCB, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) { setDisplayType(static_cast<AbstractImportMontageDialog::DisplayType>(index)); });

  connect(m_Ui->zeissListWidget, &ZeissZenListWidget::inputDirectoryChanged, this, &ImportZeissZenMontageDialog::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissZenListWidget::numberOfRowsChanged, this, &ImportZeissZenMontageDialog::zeissListWidgetChanged);
  connect(m_Ui->zeissListWidget, &ZeissZenListWidget::numberOfColumnsChanged, this, &ImportZeissZenMontageDialog::zeissListWidgetChanged);

  connect(m_Ui->colorWeightingR, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colorWeightingG, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->colorWeightingB, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { checkComplete(); });

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

  connect(m_Ui->montageStartX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->montageStartY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->montageEndX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->montageEndY, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->convertGrayscaleCB, &QCheckBox::stateChanged, this, &ImportZeissZenMontageDialog::convertGrayscale_stateChanged);
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &ImportZeissZenMontageDialog::changeOrigin_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::zeissListWidgetChanged()
{
  ZeissZenListInfo_t zeissListInfo = m_Ui->zeissListWidget->getZeissZenListInfo();
  setZeissZenListInfo(zeissListInfo);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::convertGrayscale_stateChanged(int state)
{
  m_Ui->colorWeightingR->setEnabled(state == Qt::Checked);
  m_Ui->colorWeightingG->setEnabled(state == Qt::Checked);
  m_Ui->colorWeightingB->setEnabled(state == Qt::Checked);
  if(state == Qt::Unchecked)
  {
    m_Ui->colorWeightingR->setText("0.2125");
    m_Ui->colorWeightingG->setText("0.7154");
    m_Ui->colorWeightingB->setText("0.0721");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state == Qt::Checked);
  m_Ui->originY->setEnabled(state == Qt::Checked);
  m_Ui->originZ->setEnabled(state == Qt::Checked);
  if(state == Qt::Unchecked)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportZeissZenMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->zeissListWidget->isEnabled())
  {
    if(!m_Ui->zeissListWidget->isComplete())
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

  if(m_Ui->montageStartX->isEnabled())
  {
    if(m_Ui->montageStartX->text().isEmpty())
    {
      m_Ui->errLabel->setText("Montage Start X is empty");
      result = false;
    }
  }

  if(m_Ui->montageStartY->isEnabled())
  {
    if(m_Ui->montageStartY->text().isEmpty())
    {
      m_Ui->errLabel->setText("Montage Start Y is empty");
      result = false;
    }
  }

  if(m_Ui->montageEndX->isEnabled())
  {
    if(m_Ui->montageEndX->text().isEmpty())
    {
      m_Ui->errLabel->setText("Montage End X is empty");
      result = false;
    }
  }

  if(m_Ui->montageEndY->isEnabled())
  {
    if(m_Ui->montageEndY->text().isEmpty())
    {
      m_Ui->errLabel->setText("Montage End Y is empty");
      result = false;
    }
  }

  // Check that size of montage based on start and end is valid
  int montageStartX = m_Ui->montageStartX->text().toInt();
  int montageStartY = m_Ui->montageStartY->text().toInt();
  int montageEndX = m_Ui->montageEndX->text().toInt();
  int montageEndY = m_Ui->montageEndY->text().toInt();
  int numCols = montageEndX - montageStartX + 1;
  int numRows = montageEndY - montageStartY + 1;

  int numberOfMontageTiles = numCols * numRows;
  int numberOfSelectedTiles = m_Ui->zeissListWidget->getCurrentNumberOfTiles();
  if(numberOfSelectedTiles < numberOfMontageTiles)
  {
    m_Ui->errLabel->setText(tr("The number of tiles in the tile list (%1) is less than the number of tiles declared in the montage (%2).\nPlease update"
                               " the tile list as well as the 'Montage Start' and 'Montage End' fields.")
                                .arg(numberOfSelectedTiles)
                                .arg(numberOfMontageTiles));
    result = false;
  }


  QPushButton* okBtn = m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
  if(okBtn == nullptr)
  {
    throw InvalidOKButtonException();
  }

  okBtn->setEnabled(result);
  m_Ui->errLabel->setVisible(!result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportZeissZenMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissZenMontageDialog::getOverrideOrigin()
{
  return m_Ui->changeOriginCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type ImportZeissZenMontageDialog::getOrigin()
{
  float originX = m_Ui->originX->text().toFloat();
  float originY = m_Ui->originY->text().toFloat();
  float originZ = m_Ui->originZ->text().toFloat();
  FloatVec3Type origin = {originX, originY, originZ};
  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportZeissZenMontageDialog::getConvertToGrayscale()
{
  return m_Ui->convertGrayscaleCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type ImportZeissZenMontageDialog::getColorWeighting()
{
  float r = m_Ui->colorWeightingR->text().toFloat();
  float g = m_Ui->colorWeightingG->text().toFloat();
  float b = m_Ui->colorWeightingB->text().toFloat();
  FloatVec3Type colorWeighting = {r, g, b};
  return colorWeighting;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Type ImportZeissZenMontageDialog::getMontageStart()
{
  int montageStartX = m_Ui->montageStartX->text().toInt();
  int montageStartY = m_Ui->montageStartY->text().toInt();
  IntVec3Type montageStart = {montageStartX, montageStartY, 1};
  return montageStart;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Type ImportZeissZenMontageDialog::getMontageEnd()
{
  int montageEndX = m_Ui->montageEndX->text().toInt();
  int montageEndY = m_Ui->montageEndY->text().toInt();
  IntVec3Type montageEnd = {montageEndX, montageEndY, 1};
  return montageEnd;
}