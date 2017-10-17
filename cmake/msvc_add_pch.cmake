# Enables pch generation for the project on MSVC
# caiwan/IR

function(msvc_add_pch PrecompiledHeader PrecompiledSource SourcesVar)
  if(MSVC)
    get_filename_component(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    set(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    set(Sources ${SourcesVar})
	
    set_source_files_properties(${PrecompiledSource} PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\"" OBJECT_OUTPUTS "${PrecompiledBinary}")
    set_source_files_properties(${Sources} PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\"" OBJECT_DEPENDS "${PrecompiledBinary}")  
	
  endif(MSVC)
endfunction(msvc_add_pch)
