all: DataFormats AnalyzerTools Analyzers Archive

DataFormats::
	(cd DataFormats; make)
	(mvexist.sh DataFormats/src/DataFormats_Dict_rdict.pcm lib/)
	(mvexist.sh DataFormats/libDataFormats.rootmap lib/)

AnalyzerTools::
	(cd AnalyzerTools; make)
	(mvexist.sh AnalyzerTools/src/AnalyzerTools_Dict_rdict.pcm lib/)
	(mvexist.sh AnalyzerTools/libAnalyzerTools.rootmap lib/)

Analyzers::
	(cd Analyzers; make)
	(mvexist.sh Analyzers/src/Analyzers_Dict_rdict.pcm lib/)
	(mvexist.sh Analyzers/libAnalyzers.rootmap lib/)

Archive::
	(tar -zcf lib/DataFormats.tar.gz DataFormats)
	(tar -zcf lib/AnalyzerTools.tar.gz AnalyzerTools)
	(tar -zcf lib/Analyzers.tar.gz Analyzers)

clean::
	(cd DataFormats; make clean)
	(cd AnalyzerTools; make clean)
	(cd Analyzers; make clean)

distclean::
	(cd DataFormats; make distclean)
	(cd AnalyzerTools; make distclean)
	(cd Analyzers; make distclean)

LibTarFile = tar/lib.tar.gz
$(LibTarFile): $(wildcard ./lib/*)
	tar -czf $@ ./lib/*

DataTarFile = tar/data.tar.gz
DataFiles = $(shell find data/$(PDStudyV)/ -type f -name '*')
$(DataTarFile): $(DataFiles)
	tar --exclude=data/$(PDStudyV)/Sample -czf $@ ./data/$(PDStudyV)/

CondorTar: $(LibTarFile) $(DataTarFile)
