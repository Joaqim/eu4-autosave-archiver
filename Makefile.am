AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -fpic -Wall -Wextra -Werror -x c++ -isystem /usr/local/include -isystem /usr/include

# Windows
#AM_LDFLAGS = -lopengl32

if DEBUG
AM_CXXFLAGS +=-g3 -O0 -ggdb
else
AM_CXXFLAGS += -O2
endif


#third_party/repos/imgui
#third_party/repos/imgui/examples/opengl3_example third_party/repos/imgui


SUBDIRS = src doc examples man scripts
#SUBDIRS += third_party/repos/imgui

