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

#include "ImportRobometMontageDialog.h"

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
QString ImportRobometMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::ImportRobometMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::ImportRobometMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::~ImportRobometMontageDialog()
{
  disconnectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportRobometMontageDialog::Pointer ImportRobometMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportRobometMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  qRegisterMetaType<RobometListInfo_t>();

  connectSignalsSlots();

  setDisplayType(AbstractImportMontageDialog::DisplayType::Outline);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->dataDisplayTypeCB, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) { setDisplayType(static_cast<AbstractImportMontageDialog::DisplayType>(index)); });

  connect(m_Ui->robometListWidget, &RobometListWidget::inputDirectoryChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::filePrefixChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileSuffixChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileExtensionChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMinChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMaxChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfRowsChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfColumnsChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::slicePaddingChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::rowColPaddingChanged, this, &ImportRobometMontageDialog::robometListWidgetChanged);

  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &ImportRobometMontageDialog::changeOrigin_stateChanged);
  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { checkComplete(); });

  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &ImportRobometMontageDialog::changeSpacing_stateChanged);
  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { checkComplete(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { checkComplete(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::disconnectSignalsSlots()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::robometListWidgetChanged()
{
  RobometListInfo_t robometListInfo = m_Ui->robometListWidget->getRobometListInfo();
  setRobometListInfo(robometListInfo);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state);
  m_Ui->originY->setEnabled(state);
  m_Ui->originZ->setEnabled(state);
  m_Ui->pixelCoordsCB->setEnabled(state);
  if(state == false)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
    m_Ui->pixelCoordsCB->setChecked(false);
  }

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::changeSpacing_stateChanged(int state)
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

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportRobometMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->robometListWidget->isEnabled())
  {
    if(!m_Ui->robometListWidget->isComplete())
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
QString ImportRobometMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportRobometMontageDialog::getOverrideSpacing()
{
  return m_Ui->changeSpacingCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type ImportRobometMontageDialog::getSpacing()
{
  float spacingX = m_Ui->spacingX->text().toFloat();
  float spacingY = m_Ui->spacingY->text().toFloat();
  float spacingZ = m_Ui->spacingZ->text().toFloat();
  FloatVec3Type spacing = {spacingX, spacingY, spacingZ};
  return spacing;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportRobometMontageDialog::getOverrideOrigin()
{
  return m_Ui->changeOriginCB->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Type ImportRobometMontageDialog::getOrigin()
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
bool ImportRobometMontageDialog::usePixelCoordinates()
{
  return m_Ui->pixelCoordsCB->isChecked();
}
