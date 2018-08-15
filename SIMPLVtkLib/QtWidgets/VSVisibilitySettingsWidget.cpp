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
QString SolidColorStr = "Solid Color";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisibilitySettingsWidget::VSVisibilitySettingsWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSVisibilitySettingsWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilter(nullptr, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setupGui()
{
  connect(m_Ui->representationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setRepresentationIndex(int)));
  connect(m_Ui->activeArrayCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateActiveArrayName(const QString&)));
  connect(m_Ui->activeComponentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateActiveComponentIndex(int)));

  connect(m_Ui->colorBtn, &VSColorButton::changedColor, this, &VSVisibilitySettingsWidget::colorButtonChanged);

  m_Ui->colorBtn->hide();
  m_Ui->colorLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget)
{
  m_Filter = filter;

  bool filterExists = (nullptr != filter);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filter));
  }
  else
  {
    connectFilterViewSettings(nullptr);
  }

  // Check if VSFilterSettings exist and are valid
  VSFilterViewSettings::ActorType actorType = VSFilterViewSettings::ActorType::Invalid;
  if(m_ViewSettings && m_ViewSettings->isValid())
  {
    listenSolidColor(m_ViewSettings, m_ViewSettings->getSolidColor());
    actorType = m_ViewSettings->getActorType();
  }

  switch(actorType)
  {
  case VSFilterViewSettings::ActorType::DataSet:
    m_Ui->visibilityContainer->setVisible(true);
    break;
  case VSFilterViewSettings::ActorType::Image2D:
    m_Ui->visibilityContainer->setVisible(false);
    break;
  case VSFilterViewSettings::ActorType::Invalid:
  default:
    m_Ui->visibilityContainer->setVisible(false);
    break;
  }

  updateFilterInfo();
  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::connectFilterViewSettings(VSFilterViewSettings* settings)
{
  if(m_ViewSettings)
  {
    disconnect(m_ViewSettings, &VSFilterViewSettings::representationChanged, this, &VSVisibilitySettingsWidget::listenRepresentationType);
    disconnect(m_ViewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSVisibilitySettingsWidget::listenArrayName);
    disconnect(m_ViewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSVisibilitySettingsWidget::listenComponentIndex);
    disconnect(m_ViewSettings, &VSFilterViewSettings::solidColorChanged, this, &VSVisibilitySettingsWidget::listenSolidColor);
    disconnect(m_ViewSettings, &VSFilterViewSettings::dataLoaded, this, &VSVisibilitySettingsWidget::updateFilterInfo);
  }

  m_ViewSettings = settings;

  if(m_ViewSettings)
  {
    connect(settings, &VSFilterViewSettings::representationChanged, this, &VSVisibilitySettingsWidget::listenRepresentationType);
    connect(settings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSVisibilitySettingsWidget::listenArrayName);
    connect(settings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSVisibilitySettingsWidget::listenComponentIndex);
    connect(settings, &VSFilterViewSettings::solidColorChanged, this, &VSVisibilitySettingsWidget::listenSolidColor);
    connect(settings, &VSFilterViewSettings::dataLoaded, this, &VSVisibilitySettingsWidget::updateFilterInfo);
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
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filter));
  }
  else
  {
    connectFilterViewSettings(nullptr);
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

  if(m_Filter)
  {
    m_Ui->activeArrayCombo->addItem(m_ViewSettings->getSolidColorIcon(), ::SolidColorStr);

    QStringList arrayNames = m_Filter->getArrayNames();
    QIcon arrayIcon = m_Filter->isPointData() ? m_ViewSettings->getPointDataIcon() : m_ViewSettings->getCellDataIcon();
    for(QString arrayName : arrayNames)
    {
      m_Ui->activeArrayCombo->addItem(arrayIcon, arrayName);
    }

    if(m_ViewSettings)
    {
      QString activeArrayName = m_ViewSettings->getActiveArrayName();
      int activeCompIndex = m_ViewSettings->getActiveComponentIndex();
      setComboArrayName(activeArrayName);
      m_Ui->activeComponentCombo->setCurrentIndex(activeCompIndex + 1);
    }
    else
    {
      m_Ui->activeArrayCombo->setCurrentIndex(-1);
    }
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
  if(nullptr == m_ViewSettings)
  {
    m_Ui->activeArrayCombo->setCurrentIndex(-1);
    m_Ui->activeComponentCombo->setCurrentIndex(-1);

    this->setEnabled(false);
    return;
  }

  // Apply the current filter view settings to the widget
  bool validSettings = m_ViewSettings && m_ViewSettings->isValid();
  this->setEnabled(validSettings);

  // Representation
  m_Ui->representationCombo->setCurrentIndex(m_ViewSettings->getRepresentationi());

  QString activeArrayName = m_ViewSettings->getActiveArrayName();
  int activeComponentIndex = m_ViewSettings->getActiveComponentIndex() + 1;

  // Array
  setComboArrayName(activeArrayName);
  updateActiveArrayName(activeArrayName);

  // Components
  int numComponents = m_ViewSettings->getNumberOfComponents(activeArrayName);
  if(numComponents > 1)
  {
    m_Ui->activeComponentCombo->setCurrentIndex(activeComponentIndex);
  }

  if(m_ViewSettings->isValid())
  {
    double* solidColor = m_ViewSettings->getSolidColor();
    QColor newColor = QColor::fromRgbF(solidColor[0], solidColor[1], solidColor[2]);
    m_Ui->colorBtn->setColor(newColor, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setRepresentationIndex(int index)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  VSFilterViewSettings::Representation rep = static_cast<VSFilterViewSettings::Representation>(index);
  m_ViewSettings->setRepresentation(rep);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::updateActiveArrayName(QString name)
{
  if(::SolidColorStr == name && m_Ui->activeArrayCombo->currentIndex() == 0)
  {
    name = QString::null;
  }

  bool isColor = name.isNull();
  m_Ui->colorBtn->setVisible(isColor);
  m_Ui->colorLabel->setVisible(isColor);
  m_Ui->componentLabel->setVisible(!isColor);
  m_Ui->activeComponentCombo->setVisible(!isColor);

  int componentIndex = 0;
  if(m_ViewSettings && m_ViewSettings->getActiveArrayName() == name)
  {
    componentIndex = m_ViewSettings->getActiveComponentIndex();
  }

  // Set the active component combo box values
  m_Ui->activeComponentCombo->clear();

  QStringList componentList = m_Filter->getComponentList(name);
  bool multiComponents = componentList.size() > 1;
  m_Ui->activeComponentCombo->setEnabled(multiComponents);

  if(multiComponents)
  {
    m_Ui->activeComponentCombo->setEnabled(true);
    m_Ui->activeComponentCombo->addItems(componentList);
    m_Ui->activeComponentCombo->setCurrentIndex(componentIndex);
  }
  else
  {
    m_Ui->activeComponentCombo->setEnabled(false);
  }

  if(m_ViewSettings)
  {
    m_ViewSettings->setActiveArrayName(name);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::updateActiveComponentIndex(int index)
{
  if(m_ViewSettings)
  {
    if(m_Ui->activeComponentCombo->maxCount() > 1)
    {
      index--;
    }

    m_ViewSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::colorButtonChanged(QColor color)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  double colorArray[3];
  colorArray[0] = color.redF();
  colorArray[1] = color.greenF();
  colorArray[2] = color.blueF();

  m_ViewSettings->setSolidColor(colorArray);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::setComboArrayName(QString arrayName)
{
  if(arrayName.isNull())
  {
    m_Ui->activeArrayCombo->setCurrentText(::SolidColorStr);
  }
  else
  {
    m_Ui->activeArrayCombo->setCurrentText(arrayName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenRepresentationType(VSFilterViewSettings* settings, VSFilterViewSettings::Representation rep)
{
  int index = static_cast<int>(rep);
  m_Ui->representationCombo->blockSignals(true);
  m_Ui->representationCombo->setCurrentIndex(index);
  m_Ui->representationCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenArrayName(VSFilterViewSettings* settings, QString arrayName)
{
  m_Ui->activeArrayCombo->blockSignals(true);
  if(arrayName.isNull())
  {
    m_Ui->activeArrayCombo->setCurrentText(::SolidColorStr);
  }
  else
  {
    m_Ui->activeArrayCombo->setCurrentText(arrayName);
  }
  m_Ui->activeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenComponentIndex(VSFilterViewSettings* settings, int index)
{
  m_Ui->activeComponentCombo->blockSignals(true);
  m_Ui->activeComponentCombo->setCurrentIndex(index + 1);
  m_Ui->activeComponentCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisibilitySettingsWidget::listenSolidColor(VSFilterViewSettings* settings, double* color)
{
  if(nullptr == color)
  {
    return;
  }

  QColor newColor = QColor::fromRgbF(color[0], color[1], color[2]);
  m_Ui->colorBtn->setColor(newColor, false);
}
