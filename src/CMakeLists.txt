CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (LSIViewer)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})

FIND_PACKAGE(GDAL REQUIRED)
IF (NOT GDAL_FOUND)
  MESSAGE (SEND_ERROR "GDAL dependency was not found!")
ENDIF (NOT GDAL_FOUND)

FIND_PACKAGE( Wt REQUIRED )
IF (NOT Wt_FOUND)
  MESSAGE (SEND_ERROR "Wt was not found!")
ENDIF (NOT Wt_FOUND)





ADD_EXECUTABLE(lsiviewer.wt 
wgis.cpp 
attribute_table.cpp
ColorPickDialog.cpp
LabelDialog.cpp
help.cpp
drawshape.cpp)


INCLUDE_DIRECTORIES( ${Wt_INCLUDE_DIR} ${GDAL_INCLUDE_DIR}  )

ADD_DEPENDENCIES(lsiviewer.wt wt ${CONNECTOR})
TARGET_LINK_LIBRARIES(lsiviewer.wt 
${Wt_LIBRARY} 
${Wt_HTTP_LIBRARY} 
${Wt_EXT_LIBRARY} 
${GDAL_LIBRARY} 
boost_filesystem boost_signals pthread boost_system pugixml pq)




