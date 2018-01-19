
set(${PROJECT_NAME}_Visualization_Controllers_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSController.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSViewController.cpp
)

set(${PROJECT_NAME}_Visualization_Controllers_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSController.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSViewController.cpp
  
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Controllers" "${${PROJECT_NAME}_Visualization_Controllers_HDRS}" "${${PROJECT_NAME}_Visualization_Controllers_SRCS}" "0")