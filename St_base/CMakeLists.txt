include_directories(
	../StChain
	../Star2Root
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
)

install(TARGETS St_base DESTINATION ".$ENV{STAR_HOST_SYS}/lib")
