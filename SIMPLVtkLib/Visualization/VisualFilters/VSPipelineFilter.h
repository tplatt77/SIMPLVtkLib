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

#pragma once

#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"

#include "SIMPLib/Filtering/FilterPipeline.h"

/**
 * @class VSPipelineFilter VSPipelineFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSPipelineFilter.h
 * @brief This class handles the FilterPipeline and contains additional settings in
 * addition to VSTextFilter's for displaying the pipeline information in the filter
 * model. The FilterPipeline and pipeline name can always be retrieved even if the text
 * and tool tip are changed.
 */
class SIMPLVtkLib_EXPORT VSPipelineFilter : public VSTextFilter
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param filePath
   * @param parent
   */
  VSPipelineFilter(FilterPipeline::Pointer pipeline, VSAbstractFilter* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSPipelineFilter() = default;

  /**
   * @brief Create
   * @param json
   * @return
   */
  static VSPipelineFilter* Create(QJsonObject& json, VSAbstractFilter* parent);

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @brief Returns the stored file path
   * @return
   */
  FilterPipeline::Pointer getFilterPipeline();

  /**
   * @brief Returns the file name
   * @return
   */
  QString getPipelineName() const;

  /**
   * @brief Returns the filter's name
   * @return
   */
  QString getFilterName() const override;

  /**
   * @brief getUuid
   * @return
   */
  static QUuid GetUuid();

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filter.  Returns false otherwise.
   * @param
   * @return
   */
  static bool compatibleWithParent(VSAbstractFilter* filter);

  /**
   * @brief Imports data for all child VSSIMPLDataContainerFilter
   */
  void apply();

private:
  FilterPipeline::Pointer m_FilterPipeline;
  DataContainerArray::Pointer m_Dca;
};
