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

#include "VSVisibilitySettingsWidget.h"

#include "SIMPLVtkLib/QtWidgets/VSColorButton.h"

namespace
{
const QString SolidColorStr = "Solid Color";
const QString MultiOption = "---";
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisibilitySettingsWidget::VSVisibilitySettingsWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSVisibilitySettingsWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setupGui()
{
  connect(m_Ui->representationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(representationComboChanged(int)));
  connect(m_Ui->activeArrayCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(arrayNameComboChanged(const QString&)));
  connect(m_Ui->activeComponentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(arrayComponentComboChanged(int)));
  connect(m_Ui->subsampleSB, SIGNAL(valueChanged(int)), this, SLOT(subsampleValueChanged(int)));

  connect(m_Ui->colorBtn, &VSColorButton::changedColor, this, &VSVisibilitySettingsWidget::colorButtonChanged);

  m_Ui->colorBtn->hide();
  m_Ui->colorLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  m_Filters = filters;

  bool filterExists = (filters.size() > 0);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  // Check if VSFilterSettings exist and are valid
  bool isDataSetType = false;
  if(VSFilterViewSettings::HasValidSettings(m_ViewSettings))
  {
    listenSolidColor();
    VSFilterViewSettings::ActorType actorType = VSFilterViewSettings::GetActorType(m_ViewSettings);
    isDataSetType = (VSFilterViewSettings::ActorType::DataSet == actorType);
  }

  if(isDataSetType)
  {
    m_Ui->visibilityContainer->setVisible(true);
  }
  else
  {
    m_Ui->visibilityContainer->setVisible(false);
  }

  updateFilterInfo();
  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::connectFilterViewSettings(VSFilterViewSettings::Collection settings)
{
  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    disconnect(setting, &VSFilterViewSettings::representationChanged, this, &VSVisibilitySettingsWidget::listenRepresentationType);
    disconnect(setting, &VSFilterViewSettings::activeArrayNameChanged, this, &VSVisibilitySettingsWidget::listenArrayName);
    disconnect(setting, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSVisibilitySettingsWidget::listenComponentIndex);
    disconnect(setting, &VSFilterViewSettings::solidColorChanged, this, &VSVisibilitySettingsWidget::listenSolidColor);
    disconnect(setting, &VSFilterViewSettings::dataLoaded, this, &VSVisibilitySettingsWidget::updateFilterInfo);
  }

  m_ViewSettings = settings;

  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    connect(setting, &VSFilterViewSettings::representationChanged, this, &VSVisibilitySettingsWidget::listenRepresentationType);
    connect(setting, &VSFilterViewSettings::activeArrayNameChanged, this, &VSVisibilitySettingsWidget::listenArrayName);
    connect(setting, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSVisibilitySettingsWidget::listenComponentIndex);
    connect(setting, &VSFilterViewSettings::solidColorChanged, this, &VSVisibilitySettingsWidget::listenSolidColor);
    connect(setting, &VSFilterViewSettings::dataLoaded, this, &VSVisibilitySettingsWidget::updateFilterInfo);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;

  if(m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::updateFilterInfo()
{
  // Add and set the array / component combo box values
  m_Ui->activeArrayCombo->blockSignals(true);
  m_Ui->activeComponentCombo->blockSignals(true);
  m_Ui->activeArrayCombo->clear();

  if(m_Filters.size() > 0)
  {
    m_Ui->activeArrayCombo->addItem(VSFilterViewSettings::GetSolidColorIcon(), ::SolidColorStr);

    QStringList arrayNames = VSFilterViewSettings::GetArrayNames(m_ViewSettings);
    QIcon arrayIcon = VSAbstractFilter::HasPointData(m_Filters) ? VSFilterViewSettings::GetPointDataIcon() : VSFilterViewSettings::GetCellDataIcon();
    for(QString arrayName : arrayNames)
    {
      m_Ui->activeArrayCombo->addItem(arrayIcon, arrayName);
    }

    if(m_ViewSettings.size() > 0)
    {
      QString activeArrayName = VSFilterViewSettings::GetActiveArrayName(m_ViewSettings);
      int activeCompIndex = VSFilterViewSettings::GetActiveComponentIndex(m_ViewSettings);

      setComboArrayName(activeArrayName);
      if(activeCompIndex == -2)
      {
        addMultiValueOption(m_Ui->activeComponentCombo);
      }
      else
      {
        removeMultiValueOption(m_Ui->activeComponentCombo);
        m_Ui->activeComponentCombo->setCurrentIndex(activeCompIndex + 1);
      }
    }
    else
    {
      m_Ui->activeArrayCombo->setCurrentIndex(-1);
      removeMultiValueOption(m_Ui->activeComponentCombo);
    }
  }
  else
  {
    removeMultiValueOption(m_Ui->activeComponentCombo);
  }

  m_Ui->activeArrayCombo->blockSignals(false);
  m_Ui->activeComponentCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(m_ViewSettings.size() == 0)
  {
    removeMultiValueOption(m_Ui->activeArrayCombo);
    removeMultiValueOption(m_Ui->activeComponentCombo);
    removeMultiValueOption(m_Ui->representationCombo);

    m_Ui->activeArrayCombo->setCurrentIndex(-1);
    m_Ui->activeComponentCombo->setCurrentIndex(-1);

    this->setEnabled(false);
    return;
  }

  // Apply the current filter view settings to the widget
  bool validSettings = VSFilterViewSettings::HasValidSettings(m_ViewSettings);
  this->setEnabled(validSettings);

  // Representation
  m_Ui->representationCombo->blockSignals(true);
  int index = VSFilterViewSettings::GetRepresentationi(m_ViewSettings);
  if(static_cast<int>(VSFilterViewSettings::Representation::MultiValues) == index)
  {
    addMultiValueOption(m_Ui->representationCombo);
  }
  else
  {
    removeMultiValueOption(m_Ui->representationCombo);
    m_Ui->representationCombo->setCurrentIndex(index);
  }
  m_Ui->representationCombo->blockSignals(false);

  QString activeArrayName = VSFilterViewSettings::GetActiveArrayName(m_ViewSettings);
  int activeComponentIndex = VSFilterViewSettings::GetActiveComponentIndex(m_ViewSettings) + 1;

  m_Ui->activeArrayCombo->blockSignals(true);
  m_Ui->activeComponentCombo->blockSignals(true);

  // Array
  setComboArrayName(activeArrayName);
  updateComponentComboBox(activeArrayName);

  // Components
  int numComponents = VSFilterViewSettings::GetNumberOfComponents(m_ViewSettings, activeArrayName);
  if(numComponents > 1)
  {
    // Check for multiple values
    if(-1 == activeComponentIndex)
    {
      addMultiValueOption(m_Ui->activeComponentCombo);
    }
    else
    {
      removeMultiValueOption(m_Ui->activeComponentCombo);
      m_Ui->activeComponentCombo->setCurrentIndex(activeComponentIndex);
    }
  }

  m_Ui->activeArrayCombo->blockSignals(false);
  m_Ui->activeComponentCombo->blockSignals(false);

  if(VSFilterViewSettings::HasValidSettings(m_ViewSettings))
  {
    QColor solidColor = m_ViewSettings.front()->getSolidColor();
    m_Ui->colorBtn->setColor(solidColor, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::representationComboChanged(int index)
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  if(hasMultiValueOption(m_Ui->representationCombo))
  {
    if(index == 0)
    {
      return;
    }

    index--;
  }

  VSFilterViewSettings::Representation rep = static_cast<VSFilterViewSettings::Representation>(index);
  VSFilterViewSettings::SetRepresentation(m_ViewSettings, rep);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::arrayNameComboChanged(const QString& text)
{
  // Use empty string for solid color array
  // Reserve QString::Null() for multiple arrays

  QString name = text;
  if(hasMultiValueOption(m_Ui->activeArrayCombo))
  {
    if(m_Ui->activeArrayCombo->currentIndex() == 0)
    {
      name = QString::Null();
    }
    else if(::SolidColorStr == text && m_Ui->activeArrayCombo->currentIndex() == 1)
    {
      name = QString("");
      removeMultiValueOption(m_Ui->activeArrayCombo);
    }
  }
  else if(::SolidColorStr == text && m_Ui->activeArrayCombo->currentIndex() == 0)
  {
    name = QString("");
  }

  bool isColor = name.isEmpty() && !name.isNull();
  m_Ui->colorBtn->setVisible(isColor);
  m_Ui->colorLabel->setVisible(isColor);
  m_Ui->componentLabel->setVisible(!isColor);
  m_Ui->activeComponentCombo->setVisible(!isColor);

  updateComponentComboBox(name);

  VSFilterViewSettings::SetActiveArrayName(m_ViewSettings, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::arrayComponentComboChanged(int index)
{
  if(m_ViewSettings.size() > 0)
  {
    if(hasMultiValueOption(m_Ui->activeComponentCombo))
    {
      index--;

      // Do not change components for the multi-value option
      if(m_Ui->activeComponentCombo->currentIndex() == 0)
      {
        return;
      }
    }

    if(m_Ui->activeComponentCombo->maxCount() > 1)
    {
      index--;
    }

    VSFilterViewSettings::SetActiveComponentIndex(m_ViewSettings, index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::colorButtonChanged(QColor color)
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  VSFilterViewSettings::SetSolidColor(m_ViewSettings, color);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setComboArrayName(QString arrayName)
{
  m_Ui->activeArrayCombo->blockSignals(true);
  if(arrayName.isNull())
  {
    addMultiValueOption(m_Ui->activeArrayCombo);
  }
  else
  {
    removeMultiValueOption(m_Ui->activeArrayCombo);
    if(arrayName.isEmpty())
    {
      m_Ui->activeArrayCombo->setCurrentText(::SolidColorStr);
    }
    else
    {
      m_Ui->activeArrayCombo->setCurrentText(arrayName);
    }
  }
  m_Ui->activeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::updateComponentComboBox(QString arrayName)
{
  m_Ui->activeComponentCombo->clear();

  int componentIndex = 0;
  if(VSFilterViewSettings::GetActiveArrayName(m_ViewSettings) == arrayName)
  {
    componentIndex = VSFilterViewSettings::GetActiveComponentIndex(m_ViewSettings);
  }

  // Only add items to the component QComboBox if the arrays are compatible
  QStringList componentList = VSFilterViewSettings::GetComponentNames(m_ViewSettings, arrayName);
  bool multiComponents = componentList.size() > 1;
  m_Ui->activeComponentCombo->setEnabled(multiComponents);

  if(multiComponents)
  {
    m_Ui->activeComponentCombo->setEnabled(true);
    m_Ui->activeComponentCombo->blockSignals(true);
    m_Ui->activeComponentCombo->addItems(componentList);

    // Check for multiple components selected
    if(-2 == componentIndex)
    {
      addMultiValueOption(m_Ui->activeComponentCombo);
    }
    else
    {
      removeMultiValueOption(m_Ui->activeComponentCombo);
      m_Ui->activeComponentCombo->setCurrentIndex(componentIndex);
    }
    m_Ui->activeComponentCombo->blockSignals(false);
  }
  else
  {
    m_Ui->activeComponentCombo->setEnabled(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenRepresentationType(VSFilterViewSettings::Representation rep)
{
  int index = VSFilterViewSettings::GetRepresentationi(m_ViewSettings);

  m_Ui->representationCombo->blockSignals(true);
  if(static_cast<int>(VSFilterViewSettings::Representation::MultiValues) == index)
  {
    addMultiValueOption(m_Ui->representationCombo);
  }
  else
  {
    removeMultiValueOption(m_Ui->representationCombo);
    m_Ui->representationCombo->setCurrentIndex(index);
  }
  m_Ui->representationCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenArrayName(QString arrayName)
{
  arrayName = VSFilterViewSettings::GetActiveArrayName(m_ViewSettings);

  m_Ui->activeArrayCombo->blockSignals(true);
  if(arrayName.isNull())
  {
    addMultiValueOption(m_Ui->activeArrayCombo);
  }
  else
  {
    removeMultiValueOption(m_Ui->activeArrayCombo);
    if(arrayName.isEmpty())
    {
      m_Ui->activeArrayCombo->setCurrentText(::SolidColorStr);
    }
    else
    {
      m_Ui->activeArrayCombo->setCurrentText(arrayName);
    }
  }
  m_Ui->activeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenComponentIndex(int index)
{
  index = VSFilterViewSettings::GetActiveComponentIndex(m_ViewSettings);

  m_Ui->activeComponentCombo->blockSignals(true);
  if(-2 == index)
  {
    addMultiValueOption(m_Ui->activeComponentCombo);
  }
  else
  {
    removeMultiValueOption(m_Ui->activeComponentCombo);
    m_Ui->activeComponentCombo->setCurrentIndex(index + 1);
  }
  m_Ui->activeComponentCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenSolidColor()
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  QColor color = VSFilterViewSettings::GetSolidColor(m_ViewSettings);
  if(false == color.isValid())
  {
    return;
  }

  m_Ui->colorBtn->setColor(color, false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::subsampleValueChanged(int value)
{
	VSFilterViewSettings::SetSubsampling(m_ViewSettings, value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSVisibilitySettingsWidget::hasMultiValueOption(QComboBox* comboBox) const
{
  return comboBox->findText(::MultiOption) != -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::addMultiValueOption(QComboBox* comboBox)
{
  if(hasMultiValueOption(comboBox))
  {
    // Select the Multi-Value option
    comboBox->setCurrentIndex(comboBox->findText(::MultiOption));
    return;
  }

  comboBox->insertItem(0, ::MultiOption);
  comboBox->setCurrentIndex(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::removeMultiValueOption(QComboBox* comboBox)
{
  if(!hasMultiValueOption(comboBox))
  {
    return;
  }

  int index = comboBox->findText(::MultiOption);
  comboBox->removeItem(index);
}
