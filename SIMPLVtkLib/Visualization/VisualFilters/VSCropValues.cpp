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

#include "VSCropValues.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "ui_VSCropFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropValues::VSCropValues(VSCropFilter* filter)
: VSAbstractFilterValues(filter)
, m_Voi(new int[6])
, m_SampleRate(new int[3])
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropValues::~VSCropValues()
{
  delete[] m_Voi;
  delete[] m_SampleRate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::applyValues()
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::resetValues()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSCropValues::hasChanges() const
{
  VSCropFilter* filter = dynamic_cast<VSCropFilter*>(getFilter());

  int* lastVoi = filter->getVOI();
  int* lastSampleRate = filter->getSampleRate();

  for(int i = 0; i < 3; i++)
  {
    if(m_Voi[i * 2] != lastVoi[i * 2] || m_Voi[i * 2 + 1] != lastVoi[i * 2 + 1])
    {
      return true;
    }

    if(m_SampleRate[i] != lastSampleRate[i])
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSCropValues::createFilterWidget()
{
  Ui::VSCropFilterWidget ui;
  QWidget* widget = new QWidget();

  auto updateVOI = [=] {
    int voi[6];
    voi[0] = ui.xMinSpinBox->value();
    voi[1] = ui.xMaxSpinBox->value();
    voi[2] = ui.yMinSpinBox->value();
    voi[3] = ui.yMaxSpinBox->value();
    voi[4] = ui.zMinSpinBox->value();
    voi[5] = ui.zMaxSpinBox->value();
    setVOI(voi);
  };

  auto updateSampleRate = [=] {
    int sampleRate[3];
    sampleRate[0] = ui.sampleISpinBox->value();
    sampleRate[1] = ui.sampleJSpinBox->value();
    sampleRate[2] = ui.sampleKSpinBox->value();
    setSampleRate(sampleRate);
  };

  connect(ui.xMinSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.xMaxSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.yMinSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.yMaxSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.zMinSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.zMaxSpinBox, &QSpinBox::editingFinished, updateVOI);
  connect(ui.sampleISpinBox, &QSpinBox::editingFinished, updateSampleRate);
  connect(ui.sampleJSpinBox, &QSpinBox::editingFinished, updateSampleRate);
  connect(ui.sampleKSpinBox, &QSpinBox::editingFinished, updateSampleRate);

  connect(this, &VSCropValues::volumeOfInterestChanged, [=](int voi[6]) {
    ui.xMinSpinBox->blockSignals(true);
    ui.xMaxSpinBox->blockSignals(true);
    ui.yMinSpinBox->blockSignals(true);
    ui.yMaxSpinBox->blockSignals(true);
    ui.zMinSpinBox->blockSignals(true);
    ui.zMaxSpinBox->blockSignals(true);
    ui.xMinSpinBox->setValue(voi[0]);
    ui.xMaxSpinBox->setValue(voi[1]);
    ui.yMinSpinBox->setValue(voi[2]);
    ui.yMaxSpinBox->setValue(voi[3]);
    ui.zMinSpinBox->setValue(voi[4]);
    ui.zMaxSpinBox->setValue(voi[5]);
    ui.xMinSpinBox->blockSignals(false);
    ui.xMaxSpinBox->blockSignals(false);
    ui.yMinSpinBox->blockSignals(false);
    ui.yMaxSpinBox->blockSignals(false);
    ui.zMinSpinBox->blockSignals(false);
    ui.zMaxSpinBox->blockSignals(false);
  });
  connect(this, &VSCropValues::sampleRateChanged, [=](int sampleRate[3]) {
    ui.sampleISpinBox->blockSignals(true);
    ui.sampleJSpinBox->blockSignals(true);
    ui.sampleKSpinBox->blockSignals(true);
    ui.sampleISpinBox->setValue(sampleRate[0]);
    ui.sampleJSpinBox->setValue(sampleRate[1]);
    ui.sampleKSpinBox->setValue(sampleRate[2]);
    ui.sampleISpinBox->blockSignals(false);
    ui.sampleJSpinBox->blockSignals(false);
    ui.sampleKSpinBox->blockSignals(false);
  });

  return widget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropValues::getVOI() const
{
  return m_Voi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropValues::getSampleRate() const
{
  return m_SampleRate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::setVOI(int volumeOfInterest[6])
{
  for(int i = 0; i < 6; i++)
  {
    m_Voi[i] = volumeOfInterest[i];
  }

  emit volumeOfInterestChanged(volumeOfInterest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropValues::setSampleRate(int sampleRate[3])
{
  for(int i = 0; i < 3; i++)
  {
    m_SampleRate[i] = sampleRate[i];
  }

  emit sampleRateChanged(sampleRate);
}
