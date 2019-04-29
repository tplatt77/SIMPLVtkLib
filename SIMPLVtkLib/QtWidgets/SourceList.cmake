
set(${PROJECT_NAME}_QtWidgets_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractImporter.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAdvancedVisibilitySettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorButton.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorMappingWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSDatasetImporter.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSDatasetInfoWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterFactory.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterSettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterView.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterViewDelegate.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInfoWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSLoadHDF5DataWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget2.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMontageImporter.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueItemDelegate.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueItem.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueListView.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueModel.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSTransformWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisibilitySettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.h
)

set(${PROJECT_NAME}_QtWidgets_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractImporter.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAdvancedVisibilitySettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorButton.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorMappingWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSDatasetImporter.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSDatasetInfoWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterFactory.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterSettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterView.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterViewDelegate.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSLoadHDF5DataWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInfoWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget2.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidgetBase.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMontageImporter.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueItem.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueModel.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueItemDelegate.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueListView.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSQueueWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSTransformWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisibilitySettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.cpp
)

set(${PROJECT_NAME}_QtWidgets_UIS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VisibilitySettingsContainer.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VisualizationFilterWidgets.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSAdvancedVisibilitySettingsWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSColorMappingWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSDatasetInfoWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSFilterSettingsWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSLoadHDF5DataWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSInfoWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSMainWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSMainWidget2.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSQueueWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSTransformWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSViewWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSVisibilitySettingsWidget.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/QtWidgets" "${${PROJECT_NAME}_QtWidgets_HDRS}" "${${PROJECT_NAME}_QtWidgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/QtWidgets/UI_Files" "${${PROJECT_NAME}_QtWidgets_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS ${${PROJECT_NAME}_QtWidgets_UIS} )
foreach(h ${${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files)