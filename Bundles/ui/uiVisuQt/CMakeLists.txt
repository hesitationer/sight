fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(glm QUIET REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
)

fwLink(
    ${Boost_REGEX_LIBRARY}
    ${Qt5Core_LIBRARIES} 
    ${Qt5Gui_LIBRARIES} 
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


