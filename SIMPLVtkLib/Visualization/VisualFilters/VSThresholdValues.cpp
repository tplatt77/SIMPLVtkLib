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

#include "VSThresholdValues.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "ui_VSThresholdFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdValues::VSThresholdValues(VSThresholdFilter* filter)
: VSAbstractFilterValues(filter)
, m_Range(new double[2])
{
  QStringList scalarNames = filter->getScalarNames();
  if (scalarNames.size() > 0)
  {
    setArrayName(scalarNames[0]);
  }
  else
  {
    setRange(0.0, 1.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdValues::VSThresholdValues(const VSThresholdValues& values)
: VSAbstractFilterValues(values.getFilter())
, m_Range(new double[2])
, m_FreshFilter(values.m_FreshFilter)
{
  QStringList scalarNames = values.getFilter()->getScalarNames();
  setRange(values.getRange()[0], values.getRange()[1]);
  setMinValue(values.getMinValue());
  setMaxValue(values.getMaxValue());
  setLastArrayName(values.getLastArrayName());
  setLastMaxValue(values.getLastMaxValue());
  setLastMinValue(values.getLastMinValue());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdValues::~VSThresholdValues()
{
  delete[] m_Range;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    FilterType* filterType = dynamic_cast<FilterType*>(filter);
    if(filterType)
    {
      filterType->applyValues(this);
    }
  }

  m_FreshFilter = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::resetValues()
{
  setArrayName(getLastArrayName());
  setMinValue(getLastMinValue());
  setMaxValue(getLastMaxValue());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSThresholdValues::hasChanges() const
{
  if(m_FreshFilter)
  {
    return true;
  }

  if(getSelection().size() > 1)
  {
    return true;
  }

  if(getArrayName() != getLastArrayName())
  {
    return true;
  }

  if(getMinValue() != getLastMinValue() || getMaxValue() != getLastMaxValue())
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSThresholdValues::createFilterWidget()
{
  Ui::VSThresholdFilterWidget* ui = new Ui::VSThresholdFilterWidget;
  QWidget* filterWidget = new QWidget();
  ui->setupUi(filterWidget);

  ui->scalarsComboBox->addItems(getFilter()->getScalarNames());
  ui->scalarsComboBox->setCurrentText(m_ThresholdArrayName);

  const int numTicks = 1000;
  ui->minSlider->setRange(0, numTicks);
  ui->maxSlider->setRange(0, numTicks);
  ui->minSlider->setValue(getMinPercent() * numTicks);
  ui->maxSlider->setValue(getMaxPercent() * numTicks);
  ui->minSpinBox->setMinimum(m_Range[0]);
  ui->maxSpinBox->setMinimum(m_Range[0]);
  ui->minSpinBox->setMaximum(m_Range[1]);
  ui->maxSpinBox->setMaximum(m_Range[1]);
  ui->minSpinBox->setValue(getMinValue());
  ui->maxSpinBox->setValue(getMaxValue());

  connect(this, &VSThresholdValues::arrayNameChanged, [=](QString name) {
    ui->scalarsComboBox->setCurrentText(name);
  });
  connect(this, &VSThresholdValues::rangeChanged, [=](double range[2]) { 
    ui->minSpinBox->setMinimum(range[0]);
    ui->maxSpinBox->setMinimum(range[0]);
    ui->minSpinBox->setMaximum(range[1]);
    ui->maxSpinBox->setMaximum(range[1]);
  });
  connect(this, &VSThresholdValues::minValueChanged, [=](double value) {
    ui->minSlider->blockSignals(true);
    ui->minSlider->setValue(getMinPercent() * numTicks);
    ui->minSpinBox->setValue(value);
    ui->minSlider->blockSignals(false);
  });
  connect(this, &VSThresholdValues::maxValueChanged, [=](double value) {
    ui->maxSlider->blockSignals(true);
    double percent = getMaxPercent();
    int sliderValue = getMaxPercent() * numTicks;
    ui->maxSlider->setValue(getMaxPercent() * numTicks);
    ui->maxSpinBox->setValue(value);
    ui->maxSlider->blockSignals(false);
  });
  connect(getFilter(), &VSAbstractFilter::arrayNamesChanged, this, [=] {
    QStringList scalarNames = getFilter()->getScalarNames();
    ui->scalarsComboBox->blockSignals(true);
    ui->scalarsComboBox->clear();
    ui->scalarsComboBox->addItems(scalarNames);
    ui->scalarsComboBox->blockSignals(false);
    if(scalarNames.contains(m_ThresholdArrayName))
    {
      ui->scalarsComboBox->setCurrentText(m_ThresholdArrayName);
    }
    else
    {
      if(scalarNames.size() > 0)
      {
        setArrayName(scalarNames.at(0));
      }
      else
      {
        setArrayName(QString::Null());
      }
    }
  });

  connect(ui->scalarsComboBox, &QComboBox::currentTextChanged, this, &VSThresholdValues::setArrayName);
  connect(ui->minSlider, &QSlider::valueChanged, [=](int tick) {
    double percent = static_cast<double>(tick) / numTicks;
    setMinPercent(percent);
  });
  connect(ui->maxSlider, &QSlider::valueChanged, [=](int tick) {
    double percent = static_cast<double>(tick) / numTicks;
    setMaxPercent(percent);
  });
  connect(ui->minSpinBox, &QDoubleSpinBox::editingFinished, [=] {
    setMinValue(ui->minSpinBox->value());
  });
  connect(ui->maxSpinBox, &QDoubleSpinBox::editingFinished, [=] {
    setMaxValue(ui->maxSpinBox->value());
  });

  return filterWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getMinPercent() const
{
  double span = m_Range[1] - m_Range[0];
  double minPercent = (m_MinValue - m_Range[0]) / span;
  return minPercent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getMaxPercent() const
{
  double span = m_Range[1] - m_Range[0];
  double maxPercent = (m_MaxValue - m_Range[0]) / span;
  return maxPercent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setMinPercent(double percent)
{
  double span = m_Range[1] - m_Range[0];
  double value = percent * span + m_Range[0];
  setMinValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setMaxPercent(double percent)
{
  double span = m_Range[1] - m_Range[0];
  double value = percent * span + m_Range[0];
  setMaxValue(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSThresholdValues::getArrayName() const
{
  return m_ThresholdArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSThresholdValues::getRange() const
{
  return m_Range;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getMinValue() const
{
  return m_MinValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getMaxValue() const
{
  return m_MaxValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setArrayName(QString name)
{
  if(name.isEmpty())
  {
    m_ThresholdArrayName = "";
    setRange(0.0, 1.0);

    emit arrayNameChanged("");
    emit alertChangesWaiting();
  }
  else if(getFilter()->getScalarNames().contains(name))
  {
    m_ThresholdArrayName = name;
    double min = getFilter()->getArrayMinValue(name);
    double max = getFilter()->getArrayMaxValue(name);
    setRange(min, max);
    
    emit arrayNameChanged(name);
    emit alertChangesWaiting();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setRange(double min, double max)
{
  double minPercent = getMinPercent();
  double maxPercent = getMaxPercent();

  if(m_MinValue < min)
  {
    m_MinValue = min;
    emit minValueChanged(min);
  }

  if(m_MaxValue > max)
  {
    m_MaxValue = max;
    emit maxValueChanged(max);
  }

  m_Range[0] = min;
  m_Range[1] = max;
  emit rangeChanged(m_Range);

  setMinPercent(minPercent);
  setMaxPercent(maxPercent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setMinValue(double value)
{
  if(value < m_Range[0])
  {
    value = m_Range[0];
  }

  if(value > m_MaxValue)
  {
    m_MaxValue = value;
    emit maxValueChanged(value);
  }
  m_MinValue = value;
  emit minValueChanged(value);
  emit alertChangesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setMaxValue(double value)
{
  if(value > m_Range[1])
  {
    value = m_Range[1];
  }

  if(value < m_MinValue)
  {
    m_MinValue = value;
    emit minValueChanged(value);
  }
  m_MaxValue = value;
  emit maxValueChanged(value);
  emit alertChangesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSThresholdValues::getLastArrayName() const
{
  return m_LastArrayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getLastMinValue() const
{
  return m_LastMinValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double VSThresholdValues::getLastMaxValue() const
{
  return m_LastMaxValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setLastArrayName(QString lastArrayName)
{
  m_LastArrayName = lastArrayName;
  emit lastArrayNameChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setLastMinValue(double lastMinValue)
{
  m_LastMinValue = lastMinValue;
  emit lastMinValueChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::setLastMaxValue(double lastMaxValue)
{
  m_LastMaxValue = lastMaxValue;
  emit lastMaxValueChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::readJson(QJsonObject& json)
{
  m_LastArrayName = json["Last Array Name"].toString();
  m_LastMinValue = json["Last Minimum Value"].toDouble();
  m_LastMaxValue = json["Last Maximum Value"].toDouble();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdValues::writeJson(QJsonObject& json)
{
  json["Last Array Name"] = m_LastArrayName;
  json["Last Minimum Value"] = m_LastMinValue;
  json["Last Maximum Value"] = m_LastMaxValue;
}
