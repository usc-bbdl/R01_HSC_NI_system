#
# try to find glut library and include files
#
# GLUT_INCLUDE_DIR, where to find GL/glut.h, etc.
# GLUT_LIBRARIES, the libraries to link against to use GLUT.
# GLUT_FOUND, If false, do not try to use GLUT.

# also defined, but not for general use are
# GLUT_glut_LIBRARY = the full path to the glut library.
# GLUT_Xmu_LIBRARY  = the full path to the Xmu library if available.
# GLUT_Xi_LIBRARY   = the full path to the Xi Library if available.

IF (WIN32)

  IF(CYGWIN)

    FIND_PATH( GLUT_INCLUDE_DIR GL/glut.h
      /usr/include
    )

    FIND_LIBRARY( GLUT_glut_LIBRARY glut32
      ${OPENGL_LIBRARY_DIR}
      /usr/lib
      /usr/lib/w32api
      /usr/local/lib
      /usr/X11R6/lib
    )

  ELSE(CYGWIN)

    FIND_PATH(GLUT_INCLUDE_DIR glut.h
      "C:/T5_BbdlSanger/UPC_R01_grant/source/include"
    )

    FIND_LIBRARY(GLUT_glut_LIBRARY 
		NAMES glut32
		PATHS "C:/T5_BbdlSanger/UPC_R01_grant/source/glut"		
    )

  # GLUT on Windows is usually installed into the default locations
  # used by MSVC. GLUT_INCLUDE_DIR isn't needed, and GLUT_glut_LIBRARY
  # may be explicitly set.
  #SET(GLUT_glut_LIBRARY "glut32" CACHE STRING "GLUT library for win32" FORCE)

  ENDIF(CYGWIN)

ELSE (WIN32)

  IF (APPLE)
# These values for Apple could probably do with improvement.
    FIND_PATH( GLUT_INCLUDE_DIR GLUT.framework
      ${OPENGL_INCLUDE_DIR}
    )
    SET(GLUT_glut_LIBRARY "-framework Glut" CACHE STRING "GLUT library for OSX")
    SET(GLUT_cocoa_LIBRARY "-framework Cocoa" CACHE STRING "Cocoa framework for OSX")
  ELSE (APPLE)

    FIND_PATH( GLUT_INCLUDE_DIR GL/glut.h
      /usr/include
      /usr/include/GL
      /usr/local/include
      /usr/openwin/share/include
      /usr/openwin/include
      /usr/X11R6/include
      /usr/include/X11
      /opt/graphics/OpenGL/include
      /opt/graphics/OpenGL/contrib/libglut
    )

    FIND_LIBRARY( GLUT_glut_LIBRARY glut
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GLUT_Xi_LIBRARY Xi
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

    FIND_LIBRARY( GLUT_Xmu_LIBRARY Xmu
      /usr/lib
      /usr/local/lib
      /usr/openwin/lib
      /usr/X11R6/lib
    )

  ENDIF (APPLE)

ENDIF (WIN32)

SET( GLUT_FOUND "NO" )
IF(GLUT_INCLUDE_DIR)
  IF(GLUT_glut_LIBRARY)
    # Is -lXi and -lXmu required on all platforms that have it?
    # If not, we need some way to figure out what platform we are on.
    SET( GLUT_LIBRARIES
      ${GLUT_glut_LIBRARY}
      ${GLUT_Xmu_LIBRARY}
      ${GLUT_Xi_LIBRARY} 
      ${GLUT_cocoa_LIBRARY}
    )
    SET( GLUT_FOUND "YES" )

#The following deprecated settings are for backwards compatibility with CMake1.4
    SET (GLUT_LIBRARY ${GLUT_LIBRARIES})
    SET (GLUT_INCLUDE_PATH ${GLUT_INCLUDE_DIR})

  ENDIF(GLUT_glut_LIBRARY)
ENDIF(GLUT_INCLUDE_DIR)

MARK_AS_ADVANCED(
  GLUT_INCLUDE_DIR
  GLUT_glut_LIBRARY
  GLUT_Xmu_LIBRARY
  GLUT_Xi_LIBRARY
)