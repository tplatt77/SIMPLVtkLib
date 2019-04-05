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

#include "VSPipelineFilter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

QString fetchPipelineName(FilterPipeline::Pointer pipeline)
{
  if(pipeline->getName().isEmpty())
  {
    return "Untitled Pipeline";
  }

  return pipeline->getName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPipelineFilter::VSPipelineFilter(FilterPipeline::Pointer pipeline, VSAbstractFilter* parent)
: VSTextFilter(parent, fetchPipelineName(pipeline), fetchPipelineName(pipeline))
, m_FilterPipeline(pipeline)
{
  setCheckable(false);
  // setEditable(false);

  m_PipelineValues = new VSPipelineValues(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPipelineFilter* VSPipelineFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  QJsonObject pipelineObj = json["Pipeline"].toObject();
  FilterPipeline::Pointer pipeline = FilterPipeline::FromJson(pipelineObj);

  // Check error conditions
  pipeline->preflightPipeline();
  if(pipeline->getErrorCode() < 0)
  {
    // The file doesn't exist, so have the user give the new location of the file
    QMessageBox::warning(nullptr, "Filter Creation Warning", tr("The pipeline '%1' is used by a filter but contains errors.").arg(fetchPipelineName(pipeline)));
    return nullptr;
  }

  VSPipelineFilter* newFilter = new VSPipelineFilter(pipeline, parent);
  newFilter->setToolTip(json["Tooltip"].toString());

  QFont font = newFilter->font();
  font.setItalic(json["Italic"].toBool());
  font.setBold(json["Bold"].toBool());
  font.setUnderline(json["Underline"].toBool());
  newFilter->setFont(font);

  newFilter->setInitialized(true);
  newFilter->readTransformJson(json);

  return newFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPipelineFilter::writeJson(QJsonObject& json)
{
  VSTextFilter::writeJson(json);

  json["Pipeline"] = m_FilterPipeline->toJson();
  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSPipelineFilter::GetUuid()
{
  return QUuid("{b02e564e-6ef9-58cb-a4a2-9d067f92bc7c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer VSPipelineFilter::getFilterPipeline()
{
  return m_FilterPipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSPipelineFilter::getPipelineName() const
{
  return fetchPipelineName(m_FilterPipeline);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSPipelineFilter::getFilterName() const
{
  return getPipelineName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSPipelineFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPipelineFilter::apply()
{
  // Finish importing any VSSIMPLDataContainerFilters
  VSAbstractFilter::FilterListType children = getChildren();
  for(VSAbstractFilter* child : children)
  {
    VSSIMPLDataContainerFilter* childCast = dynamic_cast<VSSIMPLDataContainerFilter*>(child);
    if(childCast)
    {
      childCast->apply();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSPipelineFilter::getFilterType() const
{
  return FilterType::Pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterValues* VSPipelineFilter::getValues()
{
  return m_PipelineValues;
}
