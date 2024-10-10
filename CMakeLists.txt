cmake_minimum_required(VERSION 3.3)

project(maliit-framework VERSION 0.99.1)

# Build options

option(enable-docs "Build documentation" ON)
option(enable-tests "Build tests" ON)

option(enable-glib "Build GLib support" ON)

option(enable-xcb "Compile with xcb support" ON)
option(enable-wayland "Compile with support for wayland" ON)
option(enable-qt6-inputcontext "Compile with Qt6 input context" ON)

option(enable-hwkeyboard "Enable support for the hardware keyboard" ON)
option(enable-dbus-activation "Enable dbus activation support for maliit-server" OFF)

# Install paths
include(GNUInstallDirs)

if(NOT DEFINED QT6_PLUGINS_INSTALL_DIR)
    set(QT6_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt6/plugins" CACHE PATH
            "Installation directory for Qt 6 plugins [LIB_INSTALL_DIR/qt6/plugins]")
endif()

if(NOT DEFINED QT6_MKSPECS_INSTALL_DIR)
    set(QT6_MKSPECS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt6/mkspecs" CACHE PATH
            "Installation directory for Qt 6 mkspecs files [LIB_INSTALL_DIR/qt6/mkspecs]")
endif()

if(NOT DEFINED GTK3_PLUGINS_INSTALL_DIR)
    set(GTK3_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/gtk-3.0/3.0.0" CACHE PATH
            "Installation directory for Gtk+ 3 plugins [LIB_INSTALL_DIR/gtk-3.0/3.0.0]")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")

find_package(PkgConfig REQUIRED)

find_package(Qt6Core)
find_package(Qt6DBus)
find_package(Qt6Gui REQUIRED PRIVATE)
find_package(Qt6Quick)
find_package(Qt6Widgets)

if(enable-wayland)
    find_package(WaylandProtocols REQUIRED PRIVATE)
    find_package(QtWaylandScanner REQUIRED)
    find_package(Wayland REQUIRED)
    find_package(Qt6WaylandClient REQUIRED PRIVATE)

    find_package(Gtk3)
endif()

include_directories(src common)

add_library(maliit-common STATIC
            common/maliit/namespace.h
            common/maliit/namespaceinternal.h
            common/maliit/settingdata.cpp
            common/maliit/settingdata.h)
target_link_libraries(maliit-common Qt6::Core)
target_include_directories(maliit-common PUBLIC common)

set(CONNECTION_SOURCES
    connection/connectionfactory.cpp
    connection/connectionfactory.h
    connection/dbuscustomarguments.cpp
    connection/dbuscustomarguments.h
    connection/dbusinputcontextconnection.cpp
    connection/dbusinputcontextconnection.h
    connection/dbusserverconnection.cpp
    connection/dbusserverconnection.h
    connection/inputcontextdbusaddress.cpp
    connection/inputcontextdbusaddress.h
    connection/mimserverconnection.cpp
    connection/mimserverconnection.h
    connection/minputcontextconnection.cpp
    connection/minputcontextconnection.h
    connection/serverdbusaddress.cpp
    connection/serverdbusaddress.h)

if(enable-wayland)
    list(APPEND CONNECTION_SOURCES
         connection/waylandinputmethodconnection.cpp
         connection/waylandinputmethodconnection.h)

    ecm_add_qtwayland_client_protocol(CONNECTION_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
    ecm_add_qtwayland_client_protocol(CONNECTION_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)

    add_definitions(-DHAVE_WAYLAND)
endif()

set_source_files_properties(dbus_interfaces/minputmethodcontext1interface.xml dbus_interfaces/minputmethodserver1interface.xml
                            PROPERTIES INCLUDE maliit/settingdata.h)

qt6_add_dbus_adaptor(CONNECTION_SOURCES dbus_interfaces/minputmethodcontext1interface.xml dbusserverconnection.h DBusServerConnection)
qt6_add_dbus_adaptor(CONNECTION_SOURCES dbus_interfaces/minputmethodserver1interface.xml dbusinputcontextconnection.h DBusInputContextConnection)

qt6_add_dbus_interface(CONNECTION_SOURCES dbus_interfaces/minputmethodcontext1interface.xml minputmethodcontext1interface_interface)
qt6_add_dbus_interface(CONNECTION_SOURCES dbus_interfaces/minputmethodserver1interface.xml minputmethodserver1interface_interface)

add_library(maliit-connection STATIC ${CONNECTION_SOURCES})
target_link_libraries(maliit-connection Qt6::Core Qt6::DBus Qt6::Gui maliit-common)
if(enable-wayland)
    target_link_libraries(maliit-connection Wayland::Client)
endif()
target_include_directories(maliit-connection PUBLIC connection)

include_directories(${Qt6Gui_PRIVATE_INCLUDE_DIRS})

add_definitions(-DMALIIT_FRAMEWORK_USE_INTERNAL_API
                -DMALIIT_PLUGINS_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/maliit/plugins"
                -DMALIIT_EXTENSIONS_DIR="${CMAKE_INSTALL_FULL_DATADIR}/maliit-framework/extensions"
                -DMALIIT_CONFIG_ROOT="/maliit/"
                -DMALIIT_PLUGINS_DIR="${CMAKE_INSTALL_FULL_LIBDIR}/maliit/plugins"
                -DMALIIT_DEFAULT_HW_PLUGIN="libmaliit-keyboard-plugin.so"
                -DMALIIT_ENABLE_MULTITOUCH=true
                -DMALIIT_DEFAULT_PLUGIN="libmaliit-keyboard-plugin.so"
                -DMALIIT_DEFAULT_SUBVIEW="")

if(enable-qt6-inputcontext)
    set(INPUT_CONTEXT_SOURCES
            input-context/main.cpp
            input-context/minputcontext.cpp
            input-context/minputcontext.h)

    add_library(maliitplatforminputcontextplugin MODULE ${INPUT_CONTEXT_SOURCES})
    target_link_libraries(maliitplatforminputcontextplugin maliit-connection Qt6::Quick)
endif()

if(enable-qt6-inputcontext)
    install(TARGETS maliitplatforminputcontextplugin LIBRARY DESTINATION ${QT6_PLUGINS_INSTALL_DIR}/platforminputcontexts)
endif()
