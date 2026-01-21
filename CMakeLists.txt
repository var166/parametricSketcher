cmake_minimum_required(VERSION 3.16)
project(ParametricSketcher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Eigen3 REQUIRED)
find_package(CGAL REQUIRED)

set(SOURCES
    main.cpp

    UI/MainWindow.cpp
    UI/Toolbar.cpp

    Rendering/SketchRenderer.cpp
    Rendering/Canvas.cpp
    Rendering/CanvasStates.cpp

    GeometryEngine/GeometricEntity.cpp
    GeometryEngine/Point.cpp
    GeometryEngine/Line.cpp
    GeometryEngine/Circle.cpp
    GeometryEngine/Ellipse.h
    GeometryEngine/RegularPolygon.h
    GeometryEngine/BezierCurve.h
    GeometryEngine/GeometricEntityFactory.cpp
    GeometryEngine/Sketch.cpp

    ConstraintSolver/Solver.cpp
    ConstraintSolver/Constraint.cpp
    
    Persistence/PersistenceManager.h
    Persistence/PersistenceManager.cpp
)


add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    CGAL::CGAL
    Eigen3::Eigen
)


if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUNDLE TRUE)
endif()