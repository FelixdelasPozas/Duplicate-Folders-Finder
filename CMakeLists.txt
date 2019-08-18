project (Duplicates)

cmake_minimum_required (VERSION 2.8.6)

# Version Number
set (DUPLICATES_VERSION_MAJOR 1)
set (DUPLICATES_VERSION_MINOR 1)
set (DUPLICATES_VERSION_PATCH 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

include_directories (${CMAKE_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Widgets)
include_directories ( ${Qt5Widgets_INCLUDE_DIRS}
                      ${Qt5Core_INCLUDE_DIRS})
					  
find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(CORE_EXTERNAL_LIBS ${CORE_EXTERNAL_LIBS} ${QT_QTTEST_LIBRARY})
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(CMAKE_CXX_FLAGS " -Wall -Wno-deprecated -std=c++11 -mno-ms-bitfields")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

configure_file("${PROJECT_SOURCE_DIR}/duplicates.rc.in" "${PROJECT_BINARY_DIR}/duplicates.rc")

set (CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> -i <SOURCE>")
ENABLE_LANGUAGE(RC)

# Add Qt Resource files
qt5_add_resources(RESOURCES
	resources/resources.qrc
	)

# Create Interface files
qt5_wrap_ui (UI_FILES
	Duplicates.ui
	AboutDialog.ui
	)
	
set (SOURCES 
	${RESOURCES}
	${MOC_FILES}
	${UI_FILES}
	${PROJECT_BINARY_DIR}/duplicates.rc
	main.cpp
	Duplicates.cpp
	AboutDialog.cpp
	)
  
set (LIBRARIES 
    Qt5::Core
 	Qt5::Widgets
	)
  
add_executable(Duplicates ${SOURCES})
target_link_libraries (Duplicates ${LIBRARIES})
	

	