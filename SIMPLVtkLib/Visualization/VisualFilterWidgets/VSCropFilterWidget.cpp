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

#include "VSCropFilterWidget.h"

#include <QtCore/QString>

#include <vtkDataSet.h>
#include <vtkImageData.h>

#include "ui_VSCropFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class VSCropFilterWidget::vsInternals : public Ui::VSCropFilterWidget
{
public:
  vsInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropFilterWidget::VSCropFilterWidget(VSCropFilter* filter, vtkRenderWindowInteractor* interactor, QWidget* widget)
: VSAbstractFilterWidget(widget)
, m_Internals(new vsInternals())
, m_CropFilter(filter)
{
  m_Internals->setupUi(this);

  setupGui();

  if (m_CropFilter->isInitialized() == true)
  {
    m_CropFilter->setInitialized(false);
    reset();
    apply();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::setupGui()
{
  if(false == (m_CropFilter && m_CropFilter->getParentFilter() && m_CropFilter->getParentFilter()->getOutput()))
  {
    return;
  }

  VTK_PTR(vtkDataSet) dataSet = m_CropFilter->getParentFilter()->getOutput();
  VTK_PTR(vtkImageData) imageData = dynamic_cast<vtkImageData*>(dataSet.Get());

  int imageExtent[6];
  if(imageData)
  {
    imageData->GetExtent(imageExtent);
  }
  else
  {
    for(int i = 0; i < 6; i++)
    {
      imageExtent[i] = 0;
    }
  }

  int* sampleRate = new int[3]{ 1, 1, 1 };

  m_Internals->xMinSpinBox->setMinimum(imageExtent[0]);
  m_Internals->xMaxSpinBox->setMinimum(imageExtent[0]);

  m_Internals->xMinSpinBox->setMaximum(imageExtent[1]);
  m_Internals->xMaxSpinBox->setMaximum(imageExtent[1]);

  m_Internals->yMinSpinBox->setMinimum(imageExtent[2]);
  m_Internals->yMaxSpinBox->setMinimum(imageExtent[2]);

  m_Internals->yMinSpinBox->setMaximum(imageExtent[3]);
  m_Internals->yMaxSpinBox->setMaximum(imageExtent[3]);

  m_Internals->zMinSpinBox->setMinimum(imageExtent[4]);
  m_Internals->zMaxSpinBox->setMinimum(imageExtent[4]);

  m_Internals->zMinSpinBox->setMaximum(imageExtent[5]);
  m_Internals->zMaxSpinBox->setMaximum(imageExtent[5]);

  setVOI(imageExtent);
  setSampleRate(sampleRate);

  connect(m_Internals->xMinSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiXMin()));
  connect(m_Internals->xMaxSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiXMax()));
  connect(m_Internals->yMinSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiYMin()));
  connect(m_Internals->yMaxSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiYMax()));
  connect(m_Internals->zMinSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiZMin()));
  connect(m_Internals->zMaxSpinBox, SIGNAL(editingFinished()), this, SLOT(updatedVoiZMax()));

  // Enable the Apply / Reset buttons
  connect(m_Internals->xMinSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->xMaxSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->yMinSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->yMaxSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->zMinSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->zMaxSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  
  connect(m_Internals->sampleISpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->sampleJSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
  connect(m_Internals->sampleKSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] { changesWaiting(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::apply()
{
  VSAbstractFilterWidget::apply();

  int* voi = getVOI();
  int* sampleRate = getSampleRate();

  m_CropFilter->apply(voi, sampleRate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::reset()
{
  int* voi = m_CropFilter->getVOI();
  int* sampleRate = m_CropFilter->getSampleRate();

  setVOI(voi);
  setSampleRate(sampleRate);

  cancelChanges();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropFilterWidget::getVOI()
{
  int* voi = new int[6];

  voi[0] = m_Internals->xMinSpinBox->value();
  voi[1] = m_Internals->xMaxSpinBox->value();
  voi[2] = m_Internals->yMinSpinBox->value();
  voi[3] = m_Internals->yMaxSpinBox->value();
  voi[4] = m_Internals->zMinSpinBox->value();
  voi[5] = m_Internals->zMaxSpinBox->value();

  return voi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSCropFilterWidget::getSampleRate()
{
  int* sampleRate = new int[3];

  sampleRate[0] = m_Internals->sampleISpinBox->value();
  sampleRate[1] = m_Internals->sampleJSpinBox->value();
  sampleRate[2] = m_Internals->sampleKSpinBox->value();

  return sampleRate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::setVOI(int* voi)
{
  m_Internals->xMinSpinBox->setValue(voi[0]);
  m_Internals->xMaxSpinBox->setValue(voi[1]);

  m_Internals->yMinSpinBox->setValue(voi[2]);
  m_Internals->yMaxSpinBox->setValue(voi[3]);

  m_Internals->zMinSpinBox->setValue(voi[4]);
  m_Internals->zMaxSpinBox->setValue(voi[5]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::setSampleRate(int* sampleRate)
{
  m_Internals->sampleISpinBox->setValue(sampleRate[0]);
  m_Internals->sampleJSpinBox->setValue(sampleRate[1]);
  m_Internals->sampleKSpinBox->setValue(sampleRate[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiXMin()
{
  int value = m_Internals->xMinSpinBox->value();
  if(m_Internals->xMaxSpinBox->value() < value)
  {
    m_Internals->xMaxSpinBox->setValue(value);
  }
  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiXMax()
{
  int value = m_Internals->xMaxSpinBox->value();
  if(m_Internals->xMinSpinBox->value() > value)
  {
    m_Internals->xMinSpinBox->setValue(value);
  }
  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiYMin()
{
  int value = m_Internals->yMinSpinBox->value();
  if(m_Internals->yMaxSpinBox->value() < value)
  {
    m_Internals->yMaxSpinBox->setValue(value);
  }
  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiYMax()
{
  int value = m_Internals->yMaxSpinBox->value();
  if(m_Internals->yMinSpinBox->value() > value)
  {
    m_Internals->yMinSpinBox->setValue(value);
  }
  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiZMin()
{
  int value = m_Internals->zMinSpinBox->value();
  if(m_Internals->zMaxSpinBox->value() < value)
  {
    m_Internals->zMaxSpinBox->setValue(value);
  }
  changesWaiting();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilterWidget::updatedVoiZMax()
{
  int value = m_Internals->zMaxSpinBox->value();
  if(m_Internals->zMinSpinBox->value() > value)
  {
    m_Internals->zMinSpinBox->setValue(value);
  }
  changesWaiting();
}
