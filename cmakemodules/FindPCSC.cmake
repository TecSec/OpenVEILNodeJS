
include(FindPkgConfig)

function (get_soname SONAME OBJFILE)
  find_program(CMAKE_OBJDUMP names objdump DOC "The objdump program")
  execute_process(
	COMMAND objdump -p "${OBJFILE}"
   	COMMAND sed -n -es/^[[:space:]]*SONAME[[:space:]]*//p
	RESULT_VARIABLE STATUS
	OUTPUT_VARIABLE SONAME_OUT
	ERROR_QUIET
  )
  STRING(REPLACE "\n" "" SONAME_OUT "${SONAME_OUT}")
#  get_filename_component(_tmp ${OBJFILE} DIRECTORY)
  if (STATUS EQUAL 0)
    set(${SONAME} "${SONAME_OUT}" PARENT_SCOPE)
  else()
    set(${SONAME} "" PARENT_SCOPE)
  endif()
endfunction()


if (PKG_CONFIG_FOUND)
	pkg_check_modules(PCSC libpcsclite)
else()
	set(PCSC_INCLUDE_DIRS ${PCSC_INCLUDE_DIRS} /usr/local/include)
	set(PCSC_LIBRARY_DIRS ${PCSC_LIBRARY_DIRS} /usr/local/lib)
endif()

find_path(PCSC_INCLUDE_DIR pcsclite.h PATHS ${PCSC_INCLUDE_DIRS} PATH_SUFFIXES PCSC)

find_library(PCSC_LIBRARY pcsclite PATHS ${PCSC_LIBRARY_DIRS})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCSC DEFAULT_MSG PCSC_INCLUDE_DIR PCSC_LIBRARY)

mark_as_advanced(PCSC_INCLUDE_DIR PCSC_LIBRARY)

if(PCSC_FOUND)
    if(NOT TARGET pcsc)
	if(UNIX)
		get_soname(PCSC_SO_NAME ${PCSC_LIBRARY})
		find_library(PCSC_LIBRARY_SO ${PCSC_SO_NAME} PATHS ${PCSC_LIBRARY_DIRS})
		add_library(pcsc SHARED IMPORTED)
message(STATUS "PCSC_LIBRARY = ${PCSC_LIBRARY}")
message(STATUS "PCSC SO name = ${PCSC_SO_NAME}")
		set_target_properties(pcsc PROPERTIES
			IMPORTED_LOCATION "${PCSC_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${PCSC_INCLUDE_DIR}"
			IMPORTED_SONAME "${PCSC_SO_NAME}"
			DLOPEN_SONAME "${PCSC_SO_NAME}"
		)
	endif(UNIX)
    endif()
endif(PCSC_FOUND)
