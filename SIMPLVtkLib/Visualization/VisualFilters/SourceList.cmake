set(VSVisualFilterValues
		VSAbstractFilterValues
		VSClipValues
		VSCropValues
		VSDataSetValues
		VSFileNameValues
		VSMaskValues
		VSPipelineValues
		VSSIMPLDataContainerValues
		VSSliceValues
		VSTextValues
		VSThresholdValues
	)

set(VSVisualFilters
  VSAbstractDataFilter
  VSAbstractFilter
  VSClipFilter
  VSCropFilter
  VSDataSetFilter
  VSFileNameFilter
  VSMaskFilter
  VSPipelineFilter
  VSRootFilter
  VSSIMPLDataContainerFilter
  VSSliceFilter
  VSTextFilter
  VSThresholdFilter
  VSTransform
)

set(VSVisualFilter_UIS
	VSClipFilterWidget
  VSCropFilterWidget
  VSDataSetFilterWidget
  VSMaskFilterWidget
  VSSIMPLDataContainerFilterWidget
  VSSliceFilterWidget
  VSThresholdFilterWidget
)

# --------------------------------------------------------------------
# Loop through Visualization Filters
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
endforeach(VisFilter)

# --------------------------------------------------------------------
# Loop through Visualization Filter Values
# --------------------------------------------------------------------
foreach(FilterValues ${VSVisualFilterValues})
  set(VS_VisualFilters_SRCS 
    ${VS_VisualFilters_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${FilterValues}.cpp
  )

  set(VS_VisualFilters_HDRS 
    ${VS_VisualFilters_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/${FilterValues}.h
  )
endforeach(FilterValues)

# --------------------------------------------------------------------
# Loop through Filter Widget UIS
# --------------------------------------------------------------------
foreach(VisFilterWidget ${VSVisualFilter_UIS})
	set(VS_VisualFilterWidgets_UIS 
    ${VS_VisualFilterWidgets_UIS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/UI_Files/${VisFilterWidget}.ui
  )
endforeach(VisFilterWidget)


cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/VisualFilters" "${VS_VisualFilters_HDRS}" "${VS_VisualFilters_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/VisualFilters/UI_Files" "${VS_VisualFilterWidgets_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS ${VS_VisualFilterWidgets_UIS} )
foreach(h ${${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters/UI_Files)
