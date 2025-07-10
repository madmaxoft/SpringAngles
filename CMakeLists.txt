cmake_minimum_required(VERSION 3.19)
project(SpringAngles LANGUAGES CXX)

# Force the project to use C++20:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# moc files are generated in the binary dir, tell CMake to look for includes there:
set (CMAKE_INCLUDE_CURRENT_DIR ON)





qt_add_executable(SpringAngles
	WIN32 MACOSX_BUNDLE

	CadGraphicsView.cpp
	CadGraphicsView.hpp
	Document.cpp
	Document.hpp
	Geometry.hpp
	Main.cpp
	MainWindow.cpp
	MainWindow.hpp
	MainWindow.ui
	PointCoordsDlg.cpp
	PointCoordsDlg.hpp
	PointCoordsDlg.ui
	SpringNet.cpp
	SpringNet.hpp
	SpringParamsDlg.cpp
	SpringParamsDlg.hpp
	SpringParamsDlg.ui
)





target_link_libraries(
	SpringAngles
	PRIVATE
		Qt::Core
		Qt::Widgets
)

include(GNUInstallDirs)

install(
	TARGETS SpringAngles
	BUNDLE  DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
	TARGET SpringAngles
	OUTPUT_SCRIPT deploy_script
	NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
