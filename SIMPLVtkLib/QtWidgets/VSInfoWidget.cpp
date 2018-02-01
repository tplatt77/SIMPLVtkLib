/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSInfoWidget.h"

#include "ui_VSInfoWidget.h"

class VSInfoWidget::VSInternals : public Ui::VSInfoWidget
{
public:
  VSInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget::VSInfoWidget(QWidget* parent)
  : QWidget(parent)
  , m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);

  setFilter(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setFilter(VSAbstractFilter* filter)
{
  m_Filter = filter;

  bool filterExists = (nullptr != filter);
  m_Internals->applyBtn->setEnabled(filterExists);
  m_Internals->resetBtn->setEnabled(filterExists);
  m_Internals->deleteBtn->setEnabled(filterExists);

  if(filterExists && m_ViewController)
  {
    m_ViewSettings = m_ViewController->getViewSettings(m_Filter);
  }
  else
  {
    m_ViewSettings = nullptr;
  }

  updateFilterInfo();
  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setViewController(VSViewController* viewController)
{
  m_ViewController = viewController;

  if(m_ViewController)
  {
    m_ViewSettings = m_ViewController->getViewSettings(m_Filter);
  }
  else
  {
    m_ViewSettings = nullptr;
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateFilterInfo()
{
  // TODO: display information both about the filter
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateViewSettingInfo()
{
  if(nullptr == m_ViewSettings)
  {
    // Clear
    m_Internals->activeArrayCombo->clear();
    m_Internals->activeComponentCombo->clear();

    m_Internals->showScalarBarCheckBox->setChecked(Qt::Unchecked);
    m_Internals->mapScalarsCheckBox->setChecked(Qt::Unchecked);

    m_Internals->filterViewWidget->setEnabled(false);
    return;
  }

  m_Internals->filterViewWidget->setEnabled(true);

  // TODO: Add and set the array / component combo box values
  //m_Internals->activeArrayCombo->insertItems(m_Filter->getArrayNames());
  //m_Internals->activeComponentCombo->insertItems(m_Filter->getComponents(m_Internals->activeArrayCombo->currentText()));

  //m_Internals->activeArrayCombo->setCurrentText(m_ViewSettings->getActiveArray());
  m_Internals->activeComponentCombo->setCurrentIndex(m_ViewSettings->getActiveComponentIndex());

  m_Internals->showScalarBarCheckBox->setChecked(m_ViewSettings->isScalarBarVisible() ? Qt::Checked : Qt::Unchecked);
  m_Internals->mapScalarsCheckBox->setChecked(m_ViewSettings->getMapColors() ? Qt::Checked : Qt::Unchecked);
}
