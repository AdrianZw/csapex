cmake_minimum_required(VERSION 2.8.11)

project(csapex_remote)

find_package(catkin REQUIRED COMPONENTS csapex_core)

catkin_package(
   INCLUDE_DIRS
      include
      ${CMAKE_CURRENT_BINARY_DIR}
   LIBRARIES
      csapex_remote
   CATKIN_DEPENDS
      csapex_core
)

include (GenerateExportHeader)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${catkin_INCLUDE_DIRS})

#
# BUILD
#


file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS
    include/csapex/core/*.h*
    include/csapex/io/*.h*
)

add_library(${PROJECT_NAME} SHARED
    src/command/dispatcher_proxy.cpp

    src/core/settings/settings_proxy.cpp

    src/factory/node_factory_proxy.cpp

    src/io/broadcast_message.cpp
    src/io/channel.cpp
    src/io/connector_server.cpp
    src/io/feedback.cpp
    src/io/graph_server.cpp
    src/io/node_server.cpp
    src/io/note.cpp
    src/io/protocol/add_parameter.cpp
    src/io/protocol/command_broadcasts.cpp
    src/io/protocol/command_requests.cpp
    src/io/protocol/connector_notes.cpp
    src/io/protocol/connector_requests.cpp
    src/io/protocol/core_notes.cpp
    src/io/protocol/core_requests.cpp
    src/io/protocol/graph_broadcasts.cpp
    src/io/protocol/graph_facade_notes.cpp
    src/io/protocol/graph_facade_requests.cpp
    src/io/protocol/graph_notes.cpp
    src/io/protocol/graph_requests.cpp
    src/io/protocol/node_broadcasts.cpp
    src/io/protocol/node_notes.cpp
    src/io/protocol/node_requests.cpp
    src/io/protocol/notification_message.cpp
    src/io/protocol/parameter_changed.cpp
    src/io/protocol/profiler_note.cpp
    src/io/protocol/profiler_requests.cpp
    src/io/protocol/request_nodes.cpp
    src/io/protocol/request_parameter.cpp
    src/io/protocol/tick_message.cpp
    src/io/proxy.cpp
    src/io/raw_message.cpp
    src/io/request.cpp
    src/io/response.cpp
    src/io/session_client.cpp
    src/io/session.cpp
    src/io/tcp_server.cpp

    src/model/connector_proxy.cpp
    src/model/graph_facade_proxy.cpp
    src/model/graph/graph_proxy.cpp
    src/model/node_facade_proxy.cpp

    src/profiling/profiler_proxy.cpp

    src/serialization/broadcast_message_serializer.cpp
    src/serialization/feedback_serializer.cpp
    src/serialization/note_serializer.cpp
    src/serialization/raw_message_serializer.cpp
    src/serialization/request_serializer.cpp

    ${${PROJECT_NAME}_HEADERS}
)

generate_export_header( ${PROJECT_NAME}
             BASE_NAME ${PROJECT_NAME}
             EXPORT_MACRO_NAME CSAPEX_REMOTE_EXPORT
             EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
             STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

target_link_libraries(${PROJECT_NAME}
    ${catkin_LIBRARIES}
)

#
# INSTALL
#

install(DIRECTORY include/csapex/
            #LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
            DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

