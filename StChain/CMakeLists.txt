include_directories(
	../Star2Root
)

root_generate_dictionary(
	StChain_Cint
	StChain.h
	StIOInterFace.h
	StEvtHddr.h
	StRTSBaseMaker.h
	StRtsTable.h
	StMaker.h
	StChainOpt.h
	StMkDeb.h
	LINKDEF StChainLinkDef.h
)

add_library (
	StChain
	SHARED
	StChain.cxx
	StMaker.cxx
	StEvtHddr.cxx
	StIOInterFace.cxx
	StMkDeb.cxx
	StRTSBaseMaker.cxx
	StRtsTable.cxx
	StChain_Cint.cxx
)

install(TARGETS StChain DESTINATION ".$ENV{STAR_HOST_SYS}/lib")
