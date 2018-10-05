#!/bin/sh

# Case if OS X
case `uname` in Darwin*) glibtoolize --copy ;;
  *) libtoolize --copy ;; esac

aclocal && autoheader && automake --add-missing && autoconf
