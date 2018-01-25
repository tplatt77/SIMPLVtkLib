/* ============================================================================
* Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#ifndef _vsmainwidgetbase_h_
#define _vsmainwidgetbase_h_

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/QtWidgets/VSFilterView.h"
#include "SIMPLVtkLib/QtWidgets/VSInfoWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"

/**
* @class VSMainWidgetBase VSMainWidgetBase.h SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h
* @brief This class is the superclass for VSMainWidget and contains the base methods 
* for accessing and viewing information regarding visual filters.
*/
class VSMainWidgetBase : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSMainWidgetBase(QWidget* parent = nullptr);

  /**
  * @brief Returns the VSController for the widget
  */
  VSController* getController();

  /**
  * @brief Returns the VSFilterView used by the main widget. Returns nullptr if none is used
  * @return
  */
  VSFilterView* getFilterView();

  /**
  * @brief Sets the VSFilterView to use, apply the controller's filter model 
  * and connect to any signals it might have.
  * @param view
  */
  void setFilterView(VSFilterView* view);

  /**
  * @brief Returns the VSInfoWidget used by the main widget. Returns nullptr if none is used
  * @return
  */
  VSInfoWidget* getInfoWidget();

  /**
  * @brief Sets the VSInfoWidget to use for displaying information about the active filter
  * @param infoWidget
  */
  void setInfoWidget(VSInfoWidget* infoWidget);

signals:
  void updateFilterInfo(VSAbstractFilter* filter, VSViewController* viewController);

protected:
  /**
  * @brief Connect signals and slots for the VSController
  */
  virtual void connectSlots();

  /**
  * @brief Returns the current visual filter
  * @return
  */
  VSAbstractFilter* getCurrentFilter();

protected slots:
  /**
  * @brief Notifies change in the active VSViewController
  * @param controller
  */
  virtual void activeViewChanged(VSViewController* controller);

  /**
  * @brief Changes the active visual filter
  * @param filter
  */
  virtual void changeCurrentFilter(VSAbstractFilter* filter);

private:
  VSController* m_Controller;
  VSAbstractFilter* m_CurrentFilter = nullptr;
  VSFilterView* m_FilterView = nullptr;
  VSInfoWidget* m_InfoWidget = nullptr;
};

#endif
