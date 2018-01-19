
set(VSVisualFilters
  VSClipFilter
  VSDataSetFilter
  VSMaskFilter
  VSSliceFilter
  VSCropFilter
  VSThresholdFilter
)

set(VSVisualFilters_NoUi
  VSAbstractFilter
)

# --------------------------------------------------------------------
# Loop through Visualization Filters with UI Files
# --------------------------------------------------------------------
foreach(VisFilter ${VSVisualFilters})
  set(VS_VisualFilters_SRCS 
    ${VS_VisualFilters_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${VisFilter}.cpp
  )

  set(VS_VisualFilters_HDRS 
    ${VS_VisualFilters_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${VisFilter}.h
  )

  set(VS_VisualFilters_UIS 
    ${VS_VisualFilters_UIS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/UI_Files/${VisFilter}.ui
  )

  # set(VS_VisualFilterWidgets_HDRS
  #   ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/FilterWidgets/${VisFilter}.h
  # )

  # set(VS_VisualFilterWidgets_SRCS
  #   ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/FilterWidgets/${VisFilter}.cpp
  # )
endforeach(VisFilter)

# --------------------------------------------------------------------
# Loop through Visualization Filters without UI Files
# --------------------------------------------------------------------
foreach(VisFilter ${VSVisualFilters_NoUi})
  set(VS_VisualFilters_SRCS 
    ${VS_VisualFilters_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${VisFilter}.cpp
  )

  set(VS_VisualFilters_HDRS 
    ${VS_VisualFilters_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${VisFilter}.h
  )
endforeach(VisFilter)


cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/VisualFilters" "${VS_VisualFilters_HDRS}" "${VS_VisualFilters_SRCS}" "0")
# cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/VisualFilters/FilterWidgets" "${VS_VisualFilterWidgets_HDRS}" "${VS_VisualFilterWidgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/VisualFilters/UI_Files" "${VS_VisualFilters_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_VisualFilters_Generated_UI_HDRS ${VS_VisualFilters_UIS} )

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_VisualFilters_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/UI_Files)
