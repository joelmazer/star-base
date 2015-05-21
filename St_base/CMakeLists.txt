include_directories(
	../StChain
	../Star2Root
)

root_generate_dictionary(
	St_base_Cint
	StObject.h
	StFileIter.h
	StFileI.h
	StMem.h
	St_Module.h
	StArray.h
	St_table_header_Table.h
	StMessMgr.h
	StVirtualEventFilter.h
	StTree.h
	Stypes.h
	LINKDEF St_baseLinkDef.h
	OPTIONS -D__ROOT__
)

add_library (
	St_base
	SHARED
	StVirtualEventFilter.cxx
	St_Module.cxx
	St_table_header_Table.cxx
	StArray.cxx
	StAutoBrowse.cxx
	StFileI.cxx
	StFileIter.cxx
	StMem.cxx
	StMessMgr.cxx
	StObject.cxx
	StStreamFile.cxx
	StStreamFileFactory.cxx
	StTree.cxx
	St_base_Cint.cxx
)

install(TARGETS St_base DESTINATION ".$ENV{STAR_HOST_SYS}/lib")
