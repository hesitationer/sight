fwLoadProperties()

target_compile_definitions(${FWPROJECT_NAME} PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Core Gui Qml Quick REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

fwForwardLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

if(USE_CONAN)
    if(FW_BUILD_EXTERNAL)
        set(FW_QT5_LOCATION "${Sight_LIBRARY_DIR}/../..")
    else()
        set(FW_QT5_LOCATION "${CONAN_QT_ROOT}")
    endif()
else()
    set(FW_QT5_LOCATION ${EXTERNAL_LIBRARIES})
endif()

target_compile_definitions(${FWPROJECT_NAME} PRIVATE QML_IMPORT_PATH="${FW_QT5_LOCATION}/qml")
