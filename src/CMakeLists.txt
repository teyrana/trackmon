
# ============================================================================
# Add the libraries in the current directory to the include path
#============================================================================

SET(BASE_NAME trackmon)


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src )

# ====== Connectors Library ======
SET(CONNECTOR_LIB_NAME "${BASE_NAME}-connectors")
SET(CONNECTOR_SOURCES
    ${CMAKE_SOURCE_DIR}/src/connectors/file/pcap-file-connector.cpp
    ${CMAKE_SOURCE_DIR}/src/connectors/file/text-file-connector.cpp
)
ADD_LIBRARY(${CONNECTOR_LIB_NAME} STATIC ${CONNECTOR_SOURCES})
TARGET_LINK_LIBRARIES(${CONNECTOR_LIB_NAME} PRIVATE 
                            ${SYSTEM_LIBS}
                            ${PCAP_LIBRARIES} )


# ====== Core Library ======
SET(CORE_LIB_NAME "${BASE_NAME}-core")
SET(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/report.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track-cache.cpp
)
ADD_LIBRARY(${CORE_LIB_NAME} STATIC ${CORE_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PROJ_LIBRARIES}
)

# ====== Parsers Library ======
SET(PARSE_LIB_NAME "${BASE_NAME}-parsers")
SET(PARSE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parsers/ais/ais-parser.cpp
)
ADD_LIBRARY(${PARSE_LIB_NAME} STATIC ${PARSE_SOURCES})
TARGET_LINK_LIBRARIES(${PARSE_LIB_NAME} PRIVATE
                            ${AIS_LIBRARY}
                            ${SYSTEM_LIBS}
)

# ====== UI Library ======
SET(UI_LIB_NAME "${BASE_NAME}-ui")
SET(UI_SOURCES
    ui/curses-input-handler.cpp
    ui/curses-renderer.cpp
    ui/display-column.cpp
)
ADD_LIBRARY(${UI_LIB_NAME} STATIC ${UI_SOURCES})
TARGET_LINK_LIBRARIES(${UI_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${CORE_LIB_NAME}
                            ${CURSES_LIBRARIES}
)

# ====== Full Executable -- With UI ======
SET(UI_EXE_NAME trackmon)
SET(UI_EXE_SOURCES
    track-monitor.cpp
)

ADD_EXECUTABLE(${UI_EXE_NAME} ${UI_EXE_SOURCES})
TARGET_COMPILE_OPTIONS(${UI_EXE_NAME} PRIVATE ${UI_COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${UI_EXE_NAME} PRIVATE
    ${CURSES_LIBRARIES}
    # ${MOOS_LIBRARIES}
    # ${MOOS_IVP_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${CONNECTOR_LIB_NAME}
    ${CORE_LIB_NAME}
    ${PARSE_LIB_NAME}
    ${UI_LIB_NAME}
    ${SYSTEM_LIBS}
    )

# ====== Partial Executable -- ingest only -- used for development ======
SET(INGEST_EXE_NAME ingest)
SET(INGEST_EXE_SOURCES
    ingest.cpp
    #track-monitor.cpp
)
ADD_EXECUTABLE(${INGEST_EXE_NAME} ${INGEST_EXE_SOURCES})
TARGET_COMPILE_OPTIONS(${INGEST_EXE_NAME} PRIVATE ${COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${INGEST_EXE_NAME} PRIVATE
    # ${CURSES_LIBRARIES}
    # ${MOOS_LIBRARIES}
    # ${MOOS_IVP_LIBRARIES}
    # ${PROJ_LIBRARIES}
    ${CONNECTOR_LIB_NAME}
    ${CORE_LIB_NAME}
    ${PARSE_LIB_NAME}
    ${UI_LIB_NAME}
    ${SYSTEM_LIBS}
    )

