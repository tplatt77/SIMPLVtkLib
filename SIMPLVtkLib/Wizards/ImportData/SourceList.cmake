set(${PROJECT_NAME}_ImportData_HDRS
  ${WIZARDS_SOURCE_DIR}/ImportData/DataDisplayOptionsPage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/FileTypeSelectionPage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/GenericCollectionTypePage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/GenericMetadataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/ImportDataWizard.h
  ${WIZARDS_SOURCE_DIR}/ImportData/LoadHDF5DataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/MontageSettings.h
  ${WIZARDS_SOURCE_DIR}/ImportData/EnterDataFilePage.h
  ${WIZARDS_SOURCE_DIR}/ImportData/TileListWidget.h
)

set(${PROJECT_NAME}_ImportData_SRCS
  ${WIZARDS_SOURCE_DIR}/ImportData/DataDisplayOptionsPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/FileTypeSelectionPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/GenericCollectionTypePage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/GenericMetadataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/ImportDataWizard.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/LoadHDF5DataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/MontageSettings.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/EnterDataFilePage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportData/TileListWidget.cpp
)

set(${PROJECT_NAME}_ImportData_UIS
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/DataDisplayOptionsPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/FileTypeSelectionPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/GenericCollectionTypePage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/GenericMetadataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/LoadHDF5DataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/EnterDataFilePage.ui
  ${WIZARDS_SOURCE_DIR}/ImportData/UI_Files/TileListWidget.ui
  
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_ImportData_HDRS}" "${${PROJECT_NAME}_ImportData_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${WIZARDS_SOURCE_DIR}/ImportData/UI_Files" "${${PROJECT_NAME}_ImportData_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_ImportData_Generated_UI_HDRS ${${PROJECT_NAME}_ImportData_UIS} )
foreach(h ${${PROJECT_NAME}_ImportData_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_ImportData_Generated_UI_HDRS}" "" "0")

include_directories(${WIZARDS_SOURCE_DIR}/ImportData/UI_Files)