 
# Project needs a name of course
project(rtm)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(OpenSSL REQUIRED)

# add_subdirectory(rtm)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${GLIB2_INCLUDE_DIR}
   ${OPENSSL_INCLUDE_DIR}
   )
 
# We add our source code here
set(rtm_SRCS
rtmapi.cpp
# rtmparser.cpp
rtm.cpp)


SET(rtm_UI
  widj.ui
)

KDE4_ADD_UI_FILES(rtm_SRCS ${rtm_UI} )

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_rtm ${rtm_SRCS})
target_link_libraries(plasma_applet_rtm 
                      ${OPENSSL_LIBRARIES} curl crypto ${GLIB2_LIBRARIES}
                      ${PLASMA_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_rtm
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-rtm.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
