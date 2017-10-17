# Quck and dirty thingy
# Adds blend files to assimpconv if tool has already built


function(add_blend_assimpconv TARGET INSTALL_DEST FILES)
	
    # quick find assimpconv, if compiled or installed
    if (WIN32)
        # set(ASSIMPCONV_DIR "${CMAKE_INSTALL_PREFIX}/tools/AssimpConverter.exe")
        # set(ASSIMPCONV_DIR "${CMAKE_BINARY_DIR}/src/assimpConv/Release/AssimpConverter.exe")
        # set(ASSIMPCONV_DIR "${CMAKE_BINARY_DIR}/src/assimpConv/Debug/AssimpConverter.exe")
        
        # message("Assimpconv: ${ASSIMPCONV_DIR}")
    
    endif(WIN32)
    
    foreach(_file IN ITEMS ${FILES})
        if (IS_ABSOLUTE "${_file}")
			file(RELATIVE_PATH _file_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_file}")
		else()
			set(_file_rel "${_file}")
		endif()
		
		get_filename_component(_file_path "${_file_rel}" REALPATH)
		get_filename_component(_file_name "${_file_rel}" NAME)
        
        message("${_file_path} -> ${_file_name}")
        
    endforeach(_file)

ENDFUNCTION(add_blend_assimpconv)

