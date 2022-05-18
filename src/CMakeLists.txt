
# ============================================================================
# Add the libraries in the current directory to the include path
#============================================================================

SET(BASE_NAME trackmon)

# ====== Core Library ======
SET(CORE_LIB_NAME "${BASE_NAME}-core")
SET(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/report.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track-cache.cpp
)
ADD_LIBRARY(${CORE_LIB_NAME} STATIC ${CORE_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${CORE_LIB_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/src/core)
TARGET_LINK_LIBRARIES(${CORE_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PROJ_LIBRARIES}
)

# ====== UI Library ======
SET(UI_LIB_NAME "${BASE_NAME}-ui")
SET(UI_SOURCES
    ui/curses-input-handler.cpp
    ui/curses-renderer.cpp
    ui/display-column.cpp
)
ADD_LIBRARY(${UI_LIB_NAME} STATIC ${UI_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${UI_LIB_NAME} INTERFACE ui)
TARGET_LINK_LIBRARIES(${UI_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${CORE_LIB_NAME}
                            ${CURSES_LIBRARIES}
)

# ====== Parent Executable ======
SET(EXE_NAME trackmon)
SET(EXE_SOURCES
    main.cpp
    track-monitor.cpp
    track-monitor-info.cpp
)
ADD_EXECUTABLE(${EXE_NAME} ${EXE_SOURCES})
TARGET_COMPILE_OPTIONS(${EXE_NAME} PRIVATE ${COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${EXE_NAME} PRIVATE
    ${SYSTEM_LIBS}
    ${CURSES_LIBRARIES}
    # ${MOOS_LIBRARIES}
    # ${MOOS_IVP_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${CORE_LIB_NAME}
    ${UI_LIB_NAME}
    # genutil
    # mbutil
    # apputil
    )

