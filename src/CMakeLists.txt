
# ============================================================================
# Add the libraries in the current directory to the include path
#============================================================================

SET(BASE_NAME trackmon)


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src )

# ====== Reader Libraries ======

# ## ====== Network TCP(?) Listener Library ======

## ====== PCAP Reader Library ======
SET(PCAP_READER_LIB_NAME "${BASE_NAME}-pcap-readers")
SET(PCAP_READER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/readers/pcap/file-reader.cpp
    ${CMAKE_SOURCE_DIR}/src/readers/pcap/file-reader.hpp
)
ADD_LIBRARY(${PCAP_READER_LIB_NAME} STATIC ${PCAP_READER_SOURCES})
TARGET_LINK_LIBRARIES(${PCAP_READER_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PCAP_LIBRARIES} )
LIST( APPEND READER_LIBS ${PCAP_READER_LIB_NAME} )

# ====== Core Library ======
SET(CORE_LIB_NAME "${BASE_NAME}-core")
SET(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/buffers.hpp
    ${CMAKE_SOURCE_DIR}/src/core/report.cpp
    ${CMAKE_SOURCE_DIR}/src/core/report.hpp
    ${CMAKE_SOURCE_DIR}/src/core/track-cache.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track-cache.hpp
    ${CMAKE_SOURCE_DIR}/src/core/track.cpp
    ${CMAKE_SOURCE_DIR}/src/core/track.hpp
)
ADD_LIBRARY(${CORE_LIB_NAME} STATIC ${CORE_SOURCES})
TARGET_LINK_LIBRARIES(${CORE_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PROJ_LIBRARIES}
)
SET( CORE_LIBS ${CORE_LIB_NAME} )

# ====== ====== ====== ====== Parser Libraries ====== ====== ====== ======

## ====== AIS Parser Library ======
SET(AIS_PARSE_LIB_NAME "${BASE_NAME}-ais-parser")
SET(AIS_PARSE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parsers/ais/parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parsers/ais/parser.hpp
)
ADD_LIBRARY(${AIS_PARSE_LIB_NAME} STATIC ${AIS_PARSE_SOURCES})
TARGET_LINK_LIBRARIES(${AIS_PARSE_LIB_NAME} PRIVATE
                            ${AIS_LIBRARY}
                            ${SYSTEM_LIBS} )
LIST( APPEND PARSER_LIBS ${AIS_PARSE_LIB_NAME} )


## ====== MOOS Parser Library ======
SET(MOOS_PARSER_LIB_NAME "${BASE_NAME}-moos-parser")
SET(MOOS_PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parsers/moos/message-parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parsers/moos/message-parser.hpp
    ${CMAKE_SOURCE_DIR}/src/parsers/moos/packet-parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parsers/moos/packet-parser.hpp
)
ADD_LIBRARY(${MOOS_PARSER_LIB_NAME} STATIC ${MOOS_PARSER_SOURCES})
TARGET_LINK_LIBRARIES(${MOOS_PARSER_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PCAP_LIBRARIES} )
LIST( APPEND PARSER_LIBS ${MOOS_PARSER_LIB_NAME} )


## ====== NMEA-0183 Parser Library ======
SET(NMEA_0183_PARSER_LIB_NAME "${BASE_NAME}-nmea-0183-parser")
SET(NMEA_0183_PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/parsers/nmea0183/packet-parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parsers/nmea0183/packet-parser.hpp
    # ${CMAKE_SOURCE_DIR}/src/parsers/nmea0183/text-log-reader.cpp
    # ${CMAKE_SOURCE_DIR}/src/parsers/nmea0183/text-log-reader.hpp
)
ADD_LIBRARY(${NMEA_0183_PARSER_LIB_NAME} STATIC ${NMEA_0183_PARSER_SOURCES})
TARGET_LINK_LIBRARIES(${NMEA_0183_PARSER_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${PCAP_LIBRARIES} )
LIST( APPEND PARSER_LIBS ${NMEA_0183_PARSER_LIB_NAME} )

# ====== ====== ====== ====== User Interface Libraries ====== ====== ====== ======

# ====== Console Output Library ======
SET(CONSOLE_UI_LIB_NAME "${BASE_NAME}-console-ui")
SET(CONSOLE_UI_SOURCES
    ui/console/renderer.cpp
    ui/console/renderer.hpp
    ui/display-column.cpp
)
ADD_LIBRARY(${CONSOLE_UI_LIB_NAME} STATIC ${CONSOLE_UI_SOURCES})
TARGET_LINK_LIBRARIES(${CONSOLE_UI_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${CORE_LIB_NAME}
                            )
SET( CONSOLE_UI_LIBS ${CONSOLE_UI_LIB_NAME} )

# ====== Curses TUI Library ======
SET(CURSES_UI_LIB_NAME "${BASE_NAME}-curses-ui")
SET(CURSES_UI_SOURCES
    ui/curses/input-handler.cpp
    ui/curses/input-handler.hpp
    ui/curses/renderer.cpp
    ui/curses/renderer.hpp
    ui/display-column.cpp
)
ADD_LIBRARY(${CURSES_UI_LIB_NAME} STATIC ${CURSES_UI_SOURCES})
TARGET_LINK_LIBRARIES(${CURSES_UI_LIB_NAME} PRIVATE
                            ${SYSTEM_LIBS}
                            ${CORE_LIB_NAME}
                            ${CURSES_LIBRARIES}
                            )
SET( CURSES_UI_LIBS ${CURSES_UI_LIB_NAME} )

# ====== ====== ====== ====== Executables ====== ====== ====== ======

# ====== Partial Executable -- ingest only -- used for development ======
SET(INGEST_EXE_NAME ingest)
SET(INGEST_EXE_SOURCES
    ingest.cpp
    #track-monitor.cpp
)
ADD_EXECUTABLE(${INGEST_EXE_NAME} ${INGEST_EXE_SOURCES})
# TARGET_COMPILE_OPTIONS(${INGEST_EXE_NAME} PRIVATE ${COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${INGEST_EXE_NAME} PRIVATE
    ${READER_LIBS}
    ${CORE_LIBS}
    ${PARSER_LIBS}
    ${CONSOLE_UI_LIBS}
    ${SYSTEM_LIBS}
    )

# ====== Full Executable -- With TUI ======
SET(MONITOR_EXE_NAME trackmon)
SET(MONITOR_EXE_SOURCES
    track-monitor.cpp
)

ADD_EXECUTABLE(${MONITOR_EXE_NAME} ${MONITOR_EXE_SOURCES})
# TARGET_COMPILE_OPTIONS(${MONITOR_EXE_NAME} PRIVATE ${COMPILE_OPTIONS})
TARGET_LINK_LIBRARIES(${MONITOR_EXE_NAME} PRIVATE
    ${CURSES_LIBRARIES}
    ${READER_LIBS}
    ${CORE_LIBS}
    ${PARSER_LIBS}
    ${CURSES_UI_LIB_NAME}
    ${SYSTEM_LIBS}
    )
