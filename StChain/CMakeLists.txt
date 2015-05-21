include_directories(
	../Star2Root
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
)

install(TARGETS StChain DESTINATION ".$ENV{STAR_HOST_SYS}/lib")
