# Copies and install static files
# caiwan/ir

function(copy_static_files TARGET INSTALL_DEST FILES)
	foreach(_file IN ITEMS ${FILES})
	
		if (IS_ABSOLUTE "${_file}")
			file(RELATIVE_PATH _file_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_file}")
		else()
			set(_file_rel "${_file}")
		endif()
		
		get_filename_component(_file_path "${_file_rel}" PATH)
		get_filename_component(_file_name "${_file_rel}" NAME)
		
		# Skip internal files
		if ("${_source}" MATCHES "CMake*\\.(txt)\\s?")
			continue()
		elseif ("${_source}" MATCHES ".*\\.(git*)\\s?")
			continue()
		endif()
		
		set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/${_file_path}")
		 
		file(MAKE_DIRECTORY ${dst_path})
		
		# -- win32 only
		if (WIN32)
			string(REPLACE "/" "\\" _file "${_file}")
			string(REPLACE "/" "\\" _dest "${dst_path}/${_file_name}")
		endif(WIN32)
		
		string(REPLACE "/" "//" _dest "${_dest}")
		
		add_custom_command(
            PRE_BUILD
			TARGET ${TARGET}
			COMMAND copy ${_file} ${_dest}
			DEPENDS ${_file}
		)
		
        # message("${TARGET} ${_file} ${_dest}")
        
		install(FILES "${_file_rel}" DESTINATION "${INSTALL_DEST}/${_file_path}")

	endforeach(_file)
endfunction(copy_static_files)
