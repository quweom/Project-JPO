cmake_minimum_required(VERSION 3.16)
project(ProjectJPO LANGUAGES CXX)

# Konwersja ścieżki na format CMake (obsługuje spacje i backslashe)
file(TO_CMAKE_PATH "D:/projects/JPO/ProjectJPO" PROJECT_ROOT)

# Wymagane moduły Qt
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Location
    Positioning
    # Charts  # Odkomentuj jeśli używasz wykresów
)


set(CMAKE_PREFIX_PATH "C:/Qt/6.6.0/mingw_64/lib/cmake")
#set(Qt6Location_DIR "C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6Location")

# Ustawienia kompilacji
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)       # Wymagane dla klas Qt (Q_OBJECT)
set(CMAKE_AUTORCC ON)       # Obsługa zasobów (.qrc)
set(CMAKE_AUTOUIC ON)       # Obsługa plików .ui


# Pliki źródłowe (.cpp)
set(SOURCES
    "${PROJECT_ROOT}/main.cpp"
    "${PROJECT_ROOT}/mainwindow.cpp"
    "${PROJECT_ROOT}/Station.cpp"
    "${PROJECT_ROOT}/Sensor.cpp"
    "${PROJECT_ROOT}/Measurement.cpp"
    "${PROJECT_ROOT}/ApiHandler.cpp"
    "${PROJECT_ROOT}/AirQualityIndex.cpp"
)

# Pliki nagłówkowe (.h)
set(HEADERS
    "${PROJECT_ROOT}/mainwindow.h"
    "${PROJECT_ROOT}/Station.h"
    "${PROJECT_ROOT}/Sensor.h"
    "${PROJECT_ROOT}/Measurement.h"
    "${PROJECT_ROOT}/ApiHandler.h"
    "${PROJECT_ROOT}/AirQualityIndex.h"
)

# Pliki interfejsu (opcjonalne)
set(FORMS
    "${PROJECT_ROOT}/mainwindow.ui"  # Jeśli używasz Qt Designer
)


# Tworzenie pliku wykonywalnego
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# Linkowanie bibliotek Qt
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Location
    Qt6::Positioning
    # Qt5::Charts  # Odkomentuj jeśli używasz wykresów
)

# Ścieżki do nagłówków
target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_ROOT}/include"
)

# Dodatkowe ustawienia
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
)
