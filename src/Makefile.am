
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)# $(AM_CXXFLAGS)
bin_PROGRAMS = plan
main_SOURCES = main.cpp KNearestOcr.cpp Config.cpp

main_LDADD = $(DEPS_LIBS) libFileWatcher.la "@x_libs@"
main_DEPENDENCIES =  libFileWatcher.la


main_CPPFLAGS = $(DEPS_CFLAGS) \
 -I../include \
 -I../third_party/simplefilewatcher/includes

# SimpleFileWatcher
noinst_LTLIBRARIES = libFileWatcher.la
noinst_HEADERS = \
	../third_party/simplefilewatcher/includes/FileWatcher/FileWatcher.h                \
	../third_party/simplefilewatcher/includes/FileWatcher/FileWatcherImpl.h 			   	 \
	../third_party/simplefilewatcher/includes/FileWatcher/FileWatcherLinux.h           \
	../third_party/simplefilewatcher/includes/FileWatcher/FileWatcherOSX.h             \
	../third_party/simplefilewatcher/includes/FileWatcher/FileWatcherWin32.h

libFileWatcher_la_SOURCES = \
	../third_party/simplefilewatcher/source/FileWatcher.cpp \
	../third_party/simplefilewatcher/source/FileWatcherLinux.cpp \
	../third_party/simplefilewatcher/source/FileWatcherOSX.cpp \
	../third_party/simplefilewatcher/source/FileWatcherWin32.cpp

 #-I../third_party/repos/imgui \
 #-I../third_party/repos/imgui/examples/opengl3_example \
 #-I../third_party/repos/imgui/examples/opengl2_example

libFileWatcher_la_LDFLAGS = -static
libFileWatcher_la_CPPFLAGS = $(AM_CXXFLAGS) -I../third_party/simplefilewatcher/includes

#plan_LDADD += $(DEPS_LIBS) libImgui.la

#lib_LTLIBRARIES = libPlan.la

#libPlan_la_LDFLAGS = -version-info 0:0:0
#libPlan_la_SOURCES = plan.cpp plan.hpp

# ImGui Library
#noinst_LTLIBRARIES = libImgui.la
#noinst_HEADERS = ../third_party/repos/imgui/imgui.h
#libImgui_la_LDFLAGS = -static
#libImgui_la_SOURCES = ../third_party/repos/imgui/imgui.cpp \
 #../third_party/repos/imgui/imgui_draw.cpp \
 #../include/imguifilesystem.cpp

#if GL3
#libImgui_la_SOURCES += ../third_party/repos/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
#noinst_HEADERS += ../third_party/repos/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h
#else
#libImgui_la_SOURCES += ../third_party/repos/imgui/examples/opengl2_example/imgui_impl_glfw_gl2.cpp
#noinst_HEADERS += ../third_party/repos/imgui/examples/opengl2_example/imgui_impl_glfw_gl2.h
#endif

#libImgui_la_CPPFLAGS = $(GL_CFLAGS) $(AM_CXXFLAGS)

#libImgui_la_CPPFLAGS += -I../third_party/repos/imgui -I../third_party/repos/imgui/examples/libs/glfw/ -I../third_party/repos/ #-std=c++17

