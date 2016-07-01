cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(star-base)


# Add to path in order to pick up the FindXXX.cmake files included in this project
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/star-cmake")

# This project depends on ROOT
find_package(ROOT)
find_package(Log4Cxx)
find_package(MySQL)


if(NOT ROOT_FOUND)
	message(FATAL_ERROR "Fatal error: ROOT package not found")
endif()

# Perform some setup standard to STAR experiment environment
include(StarCommon)

add_definitions(-D__ROOT__)

include_directories(
	${ROOT_INCLUDE_DIR}
	${LOG4CXX_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${STAR_STDB_DIR}
)

# StChain library
root_generate_dictionary(
	StChain_Cint
	StChain/StChain.h
	StChain/StIOInterFace.h
	StChain/StEvtHddr.h
	StChain/StRTSBaseMaker.h
	StChain/StRtsTable.h
	StChain/StMaker.h
	StChain/StChainOpt.h
	StChain/StMkDeb.h
	LINKDEF StChain/StChainLinkDef.h
)

file(GLOB StChain_SOURCES "StChain/*.cxx") 
add_library(StChain SHARED ${StChain_SOURCES} StChain_Cint.cxx)


# StIOMaker library
root_generate_linkdef_and_dictionary(StIOMaker HEADERS StIOMaker/StIOMaker.h)

add_library(StIOMaker SHARED StIOMaker/StIOMaker.cxx StIOMaker_dict.cxx)


# St_base library
root_generate_dictionary(
	St_base_Cint
	St_base/StObject.h
	St_base/StFileIter.h
	St_base/StFileI.h
	St_base/StMem.h
	St_base/St_Module.h
	St_base/StArray.h
	St_base/St_table_header_Table.h
	St_base/StMessMgr.h
	St_base/StVirtualEventFilter.h
	St_base/StTree.h
	St_base/Stypes.h
	LINKDEF St_base/St_baseLinkDef.h
	OPTIONS -D__ROOT__
)

file(GLOB St_base_SOURCES "St_base/*.cxx") 
add_library(St_base SHARED ${St_base_SOURCES} St_base_Cint.cxx)

# StarClassLibrary library
root_generate_dictionary(
	StarClassLibrary_Cint
	StarClassLibrary/StPhysicalHelix.hh
	StarClassLibrary/StLorentzVector.hh
	StarClassLibrary/StMatrix.hh
	StarClassLibrary/StThreeVector.hh
	StarClassLibrary/StHelix.hh
	StarClassLibrary/StHelixHelper.h
	StarClassLibrary/BetheBloch.h
	StarClassLibrary/StThreeVectorF.hh
	StarClassLibrary/StThreeVectorD.hh
	StarClassLibrary/StLorentzVectorF.hh
	StarClassLibrary/StLorentzVectorD.hh
	StarClassLibrary/StMatrixF.hh
	StarClassLibrary/StMatrixD.hh
	StarClassLibrary/StPhysicalHelixD.hh
	StarClassLibrary/StParticleTable.hh
	StarClassLibrary/StParticleDefinition.hh
	LINKDEF StarClassLibrary/StarClassLibraryLinkDef.hh
	OPTIONS -D__ROOT__
)

file(GLOB StarClassLibrary_SOURCES "StarClassLibrary/*.cc" "StarClassLibrary/*.cxx") 
add_library( StarClassLibrary SHARED ${StarClassLibrary_SOURCES} StarClassLibrary_Cint.cxx)

# StarRoot library
root_generate_dictionary(
	StarRoot_Cint
	StarRoot/TPolinom.h
	StarRoot/TRandomVector.h
	StarRoot/StMemStat.h
	StarRoot/TNumDeriv.h
	StarRoot/TTreeHelper.h
	StarRoot/StDraw3D.h
	StarRoot/TF1Fitter.h
	StarRoot/THelixTrack.h
	StarRoot/TAttr.h
	StarRoot/TUnixTime.h
	StarRoot/TRDiagMatrix.h
	StarRoot/StCloseFileOnTerminate.h
	StarRoot/TTreeIter.h
	StarRoot/KFVertex.h
	StarRoot/TRVector.h
	StarRoot/TRArray.h
	StarRoot/KFParticleBase.h
	StarRoot/TMDFParameters.h
	StarRoot/TRSymMatrix.h
	StarRoot/TRMatrix.h
	StarRoot/StArchInfo.h
	StarRoot/TCFit.h
	StarRoot/VVertex.h
	StarRoot/TH1Helper.h
	StarRoot/TDirIter.h
	StarRoot/KFParticle.h
	StarRoot/THack.h
	StarRoot/StCheckQtEnv.h
	StarRoot/StMultiKeyMap.h
	StarRoot/TPolynomial.h
	StarRoot/MTrack.h
	StarRoot/MVertex.h
	StarRoot/TIdTruUtil.h
	LINKDEF StarRoot/StarRootLinkDef.h
)

file(GLOB StarRoot_SOURCES "StarRoot/*.cxx") 
add_library (StarRoot SHARED ${StarRoot_SOURCES} StarRoot_Cint.cxx)


# StarMagField library
root_generate_dictionary(StarMagField_Cint StarMagField/StarMagField.h LINKDEF StarMagField/StarMagFieldLinkDef.h OPTIONS -D__ROOT__)
file(GLOB StarMagField_SOURCES "StarMagField/*.cxx")
add_library (StarMagField SHARED ${StarMagField_SOURCES} StarMagField_Cint.cxx)


# StStarLogger library
root_generate_dictionary(StStarLogger_Cint StStarLogger/StLoggerManager.h LINKDEF StStarLogger/StStarLoggerLinkDef.h OPTIONS -D__ROOT__)
file(GLOB StStarLogger_SOURCES "StStarLogger/*.cxx" "StStarLogger/logging/*.cxx" "StStarLogger/logging/*.cpp")
add_library (StStarLogger SHARED ${StStarLogger_SOURCES} StStarLogger_Cint)
set_target_properties(StStarLogger PROPERTIES COMPILE_FLAGS "-D_UCMLOGGER_")

# StUtilities library
root_generate_dictionary(StUtilities_dict StUtilities/StMessage.h LINKDEF StUtilities/StUtilitiesLinkDef.h OPTIONS "-D__ROOT__")

add_library(
	StUtilities
	SHARED
	StUtilities/StMessage.cxx
	StUtilities/StMessageCounter.cxx
	StUtilities/StMessageManager.cxx
	StUtilities/StMessTypeList.cxx
	StUtilities_dict.cxx
)


# A collective target to build all libraries in this project. Can be used to
# build all specified targets from a parent project
add_custom_target(star-base DEPENDS
	StarClassLibrary StarRoot St_base StChain StIOMaker StarMagField StStarLogger StUtilities
)


# Installation section
install(TARGETS
	StarClassLibrary StarRoot St_base StChain StIOMaker StarMagField StStarLogger StUtilities
	DESTINATION "${STAR_ADDITIONAL_INSTALL_PREFIX}/lib" OPTIONAL
)
