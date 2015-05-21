include_directories(
	../StarClassLibrary
)

add_library (
	StarRoot
	SHARED
	KFParticle.cxx
	KFParticleBase.cxx
	KFVertex.cxx
	MTrack.cxx
	MVertex.cxx
	StArchInfo.cxx
	StBiTree.cxx
	StCheckQtEnv.cxx
	StCloseFileOnTerminate.cxx
	StDraw3D.cxx
	StMemStat.cxx
	StMultiKeyMap.cxx
	StMultyKeyMap.cxx
	TAttr.cxx
	TCFit.cxx
	TDirIter.cxx
	TF1Fitter.cxx
	TH1Helper.cxx
	THack.cxx
	THelixTrack.cxx
	TMDFParameters.cxx
	TNumDeriv.cxx
	TPolinom.cxx
	TPolynomial.cxx
	TRArray.cxx
	TRDiagMatrix.cxx
	TRMatrix.cxx
	TRSymMatrix.cxx
	TRVector.cxx
	TRandomVector.cxx
	TTreeHelper.cxx
	TTreeIter.cxx
	TTreeIter.txt
	TUnixTime.cxx
	VVertex.cxx
	xTCL.cxx
)

install(TARGETS StarRoot DESTINATION ".$ENV{STAR_HOST_SYS}/lib")
