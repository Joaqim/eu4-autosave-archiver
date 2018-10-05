# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([main], [1.0], [joaqimpla@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

m4_include([m4/m4_ax_cxx_compile_stdcxx.m4])
#m4_include([m4/m4_ax_cxx_compile_stdcxx_11.m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AX_CXX_COMPILE_STDCXX(17, [noext], [mandatory])

if test x$ac_success = xno; then
	if test x$CXX = xg++; then
		GCC_VERSION_MAJOR=$(g++ -dumpversion | cut -d'.' -f1)
		GCC_VERSION_MINOR=$(g++ -dumpversion | cut -d'.' -f2)

		if test "$GCC_VERSION_MAJOR" -ge 4; then
			if test "$GCC_VERSION_MINOR" -ge 6; then
				ac_success=yes
				CXXFLAGS="-std=c++0x"
				HAVE_CXX11=1
			fi
		fi
	fi
fi

# Conditionals
AC_ARG_ENABLE(debug,
	      AS_HELP_STRING([--enable-debug], [enable debugging, default: yes]),
	      [case "${enableval}" in
	       yes) debug=true ;;
	       no)  debug=false ;;
	       *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
       esac],
       [debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
#AC_DEFINE_UNQUOTED([DEBUG], ["$debug"])

AC_ARG_ENABLE(gl3,
	      AS_HELP_STRING([--enable-gl3], [enable opengl3, default: no]),
	      [case "${enableval}" in
	       yes) gl3=true ;;
	       no)  gl3=false ;;
	       *)   AC_MSG_ERROR([bad value ${enableval} for --enable-gl3]) ;;
       esac],
       [gl3=false])

# Checks for libraries.
LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL(libtool)



AC_ARG_WITH([glfw], AS_HELP_STRING([--with-glfw], [Build with the GLFW3]))

AS_IF([test "x$with_glfw" = "xyes"], [
          PKG_CHECK_MODULES([GLFW], [glfw])
	  AC_CHECK_LIB(glfw, glfwInit, [], AC_MSG_ERROR([Could not find libglfw. Try $ ./configure LDFLAGS='-Lyour-glfw-lib-path']))
	  AC_CHECK_HEADERS([GLFW/glfw3.h], [], AC_MSG_ERROR([Could not find GLFW/glfw3.h. Try $ ./configure CXXFLAGS='-Iyour-GLFW-include-path]))
	  ])


# Find Unix -lgl
#AC_CHECK_LIB(GL, glPushMatrix, [], AC_MSG_ERROR([Could not find libGL. Try $ ./configure LDFLAGS='-Lyour-GL-lib-path']))

# Find Windows -lopengl32
#AC_CHECK_LIB(opengl32, glPushMatrix, [], AC_MSG_ERROR([Could not find libopengl32. Try $ ./configure LDFLAGS='-Lyour-opengl32-lib-path']))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h string.h sys/time.h termios.h unistd.h])
#AC_CHECK_HEADERS([argtable2.h], [], AC_MSG_ERROR([Could not find argtable2.h. Try $ ./configure CXXFLAGS='-Iyour-argtable2-include-path]))


# Finds Dependencies

#PKG_CHECK_MODULES([DEPS], [glib-2.0 >= 2.24.1] [opencv] [lept] [tesseract] )

PKG_CHECK_MODULES([DEPS], [opencv] [lept] [tesseract] [libssh] [glib-2.0 >= 2.24.1])


# Find the X11 include and library directories
AC_PATH_X
AC_PATH_XTRA

saved_cxxflags="$CXXFLAGS"
saved_ldflags="$LDFLAGS"

if test "x$x_includes" = "x"; then
	  x_includes="/usr/include"
fi

if test "x$no_x" = "xyes"; then
	AC_MSG_ERROR([
  *** X libraries or include files not found. Check 'config.log' for 
  *** more details.])
fi

# Checks for libraries.
# Check for the X11 library

AC_CHECK_LIB(X11, XOpenDisplay, x_libs="-lX11 $X_EXTRA_LIBS", 
AC_MSG_ERROR([*** libX11 not found. Check 'config.log' for more details.]),
$X_EXTRA_LIBS)

AC_ARG_ENABLE([disable_shm], AS_HELP_STRING([--disable-shm], [Build without the XShm Extension]))

if test "x$enable_shm" != "xno"; then
   # Check for the Xext library (needed for XShm extention)
   AC_CHECK_LIB(Xext, XShmAttach,
                      x_libs="-lXext $x_libs", 
                      # On AIX, it is in XextSam instead, but we still need -lXext
                      AC_CHECK_LIB(XextSam, XShmAttach, 
                                            x_libs="-lXextSam -lXext $x_libs", 
                                            no_xext_lib=yes, $x_libs),
                      $x_libs)
fi

x_cxxflags="$X_CFLAGS"
x_ldflags="$X_LDFLAGS $X_LIBS"

CXXFLAGS="$saved_cxxflags $x_cflags"
LDFLAGS="$saved_ldflags $x_ldflags"

AC_SUBST(x_cflags)
AC_SUBST(x_includes)
AC_SUBST(x_ldflags)
AC_SUBST(x_libs)


#CPPFLAGS += x_cxxflags x_includes x_ldflags x_libs
#-lglfw3 -pthread -ldl -lGLU -lGL -lrt -lXrandr -lXxf86vm -lXi -lXinerama -lX11
#PKG_CHECK_MODULES([DEPS], [opencv-3.4.1-2 >= 3])
#PKG_CHECK_MODULES([DEPS], [glew-2.1.0])
#PKG_CHECK_MODULES([DEPS], [glut-3.0.0])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Output Makefile files.
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile examples/Makefile man/Makefile scripts/Makefile ])
# third_party/repos/imgui/Makefile \
AC_OUTPUT
