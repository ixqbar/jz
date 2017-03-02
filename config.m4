dnl $Id$
dnl config.m4 for extension jz

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(jz, for jz support,
dnl Make sure that the comment is aligned:
dnl [  --with-jz             Include jz support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(jz, whether to enable jz support,
Make sure that the comment is aligned:
[  --enable-jz           Enable jz support])

PHP_ARG_ENABLE(jieba, enable cjieba support,
[  --enable-jieba       Do you have cjieba?], no, no)

if test "$PHP_JZ" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-jz -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/mcrypt.h"  # you most likely want to change this
  if test -r $PHP_JZ/$SEARCH_FOR; then # path given as parameter
     JZ_DIR=$PHP_JZ
  else # search default path list
     AC_MSG_CHECKING([for jz files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         JZ_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
  fi
  
  if test -z "$JZ_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the jz distribution])
  fi
  
  if test "$PHP_JIEBA" = "yes"; then
     AC_DEFINE(JZ_USE_JIEBA, 1, [use jieba])
     PHP_ADD_INCLUDE("./cjieba/include")
     PHP_ADD_LIBRARY_WITH_PATH(jieba, "./cjieba/lib", JZ_SHARED_LIBADD)
  fi

  PHP_ADD_INCLUDE($JZ_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH("mcrypt", $JZ_DIR/$PHP_LIBDIR, JZ_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH("z", $JZ_DIR/$PHP_LIBDIR, JZ_SHARED_LIBADD)
  
  PHP_SUBST(JZ_SHARED_LIBADD)

  PHP_NEW_EXTENSION(jz, jz.c jz_data.c jz_buffer.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
