#	Copyright (c) 2015, TecSec, Inc.
#
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions are met:
#	
#		* Redistributions of source code must retain the above copyright
#		  notice, this list of conditions and the following disclaimer.
#		* Redistributions in binary form must reproduce the above copyright
#		  notice, this list of conditions and the following disclaimer in the
#		  documentation and/or other materials provided with the distribution.
#		* Neither the name of TecSec nor the names of the contributors may be
#		  used to endorse or promote products derived from this software 
#		  without specific prior written permission.
#		 
#	ALTERNATIVELY, provided that this notice is retained in full, this product
#	may be distributed under the terms of the GNU General Public License (GPL),
#	in which case the provisions of the GPL apply INSTEAD OF those given above.
#		 
#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#	DISCLAIMED.  IN NO EVENT SHALL TECSEC BE LIABLE FOR ANY 
#	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#	LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckSymbolExists)

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
    HINTS
        $ENV{GTEST_ROOT}/include
        ${GTEST_ROOT}/include
)
mark_as_advanced(GTEST_INCLUDE_DIR)
# if (NOT GTEST_LIBRARIES)
    find_library(GTEST_SHARED_LIBRARY_RELEASE NAMES gtest HINTS $ENV{GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX})
    find_library(GTEST_SHARED_LIBRARY_RELWITHDEBINFO NAMES gtest HINTS $ENV{GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX})
    find_library(GTEST_SHARED_LIBRARY_DEBUG NAMES gtestd HINTS $ENV{GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/lib${TS_LIB_DIR_SUFFIX})
	IF(WIN32)
		SET(_tmp ${CMAKE_FIND_LIBRARY_SUFFIXES})
		SET(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})
		find_library(GTEST_SHARED_SO_RELWITHDEBINFO NAMES gtest HINTS $ENV{GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX})
		find_library(GTEST_SHARED_SO_RELEASE NAMES gtest HINTS $ENV{GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX})
		find_library(GTEST_SHARED_SO_DEBUG NAMES gtestd HINTS $ENV{GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX} ${GTEST_ROOT}/bin${TS_LIB_DIR_SUFFIX})
		SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_tmp})
	endif(WIN32)
# endif ()

# handle the QUIETLY and REQUIRED arguments and set BZip2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTEST
                                  REQUIRED_VARS GTEST_SHARED_LIBRARY_RELEASE GTEST_SHARED_LIBRARY_DEBUG GTEST_INCLUDE_DIR
								  )

if(GTEST_FOUND)
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
    set(GTEST_LIBRARIES ${GTEST_LIBRARY})

    if(NOT TARGET GTEST)
		if(WIN32)
		  add_library(GTEST SHARED IMPORTED)
		  set_property(TARGET GTEST PROPERTY IMPORTED_LOCATION_DEBUG "${GTEST_SHARED_SO_DEBUG}")
		  set_property(TARGET GTEST PROPERTY IMPORTED_LOCATION_RELEASE "${GTEST_SHARED_SO_RELEASE}")
		  set_property(TARGET GTEST PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${GTEST_SHARED_SO_RELWITHDEBINFO}")
		  set_property(TARGET GTEST PROPERTY IMPORTED_IMPLIB_DEBUG "${GTEST_SHARED_LIBRARY_DEBUG}")
		  set_property(TARGET GTEST PROPERTY IMPORTED_IMPLIB_RELEASE "${GTEST_SHARED_LIBRARY_RELEASE}")
		  set_property(TARGET GTEST PROPERTY IMPORTED_IMPLIB_RELWITHDEBINFO "${GTEST_SHARED_LIBRARY_RELWITHDEBINFO}")
		  set_property(TARGET GTEST PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
		else(WIN32)
		  add_library(GTEST SHARED IMPORTED)
		  set_target_properties(GTEST PROPERTIES
			IMPORTED_LOCATION_DEBUG "${GTEST_SHARED_LIBRARY_DEBUG}"
			IMPORTED_LOCATION_RELEASE "${GTEST_SHARED_LIBRARY_RELEASE}"
			IMPORTED_LOCATION_RELWITHDEBINFO "${GTEST_SHARED_LIBRARY_RELWITHDEBINFO}"
			INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
		endif(WIN32)
    endif()
endif()

mark_as_advanced(GTEST_INCLUDE_DIR)
