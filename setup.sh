export PDStudy_WD=`pwd`
export PDStudy_LIB_PATH=$PDStudy_WD/lib/
mkdir -p $PDStudy_LIB_PATH
mkdir -p $PDStudy_WD/tar

export PDStudyV="Run1_v1"
mkdir -p $PDStudy_WD/data/$PDStudyV
export DATA_DIR=$PDStudy_WD/data/$PDStudyV

#### USER INFO ####
export PDStudyLogEmail='sungbin.oh@cern.ch'
export PDStudyLogWeb=''
export PDStudyLogWebDir=''

#### use cvmfs for root ####
export CMS_PATH=/cvmfs/cms.cern.ch
source $CMS_PATH/cmsset_default.sh
export SCRAM_ARCH=slc7_amd64_gcc700
export cmsswrel='cmssw-patch/CMSSW_10_4_0_patch1'
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/src
echo "@@@@ SCRAM_ARCH = "$SCRAM_ARCH
echo "@@@@ cmsswrel = "$cmsswrel
echo "@@@@ scram..."
eval `scramv1 runtime -sh`
cd -
source /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/external/$SCRAM_ARCH/bin/thisroot.sh

if [[ $HOSTNAME == *"tamsa1"* ]]; then

  echo "@@@@ Working on tamsa1"
  export PDStudyRunlogDir="/data6/Users/$USER/PDStudyRunlog/"
  export PDStudyOutputDir="/data6/Users/$USER/PDStudyOutput/"
  
elif [[ $HOSTNAME == *"tamsa2"* ]]; then

  echo "@@@@ Working on tamsa2"
  export PDStudyRunlogDir="/data6/Users/$USER/PDStudyRunlog/"
  export PDStudyOutputDir="/data6/Users/$USER/PDStudyOutput/"
  echo $PDStudyRunlogDir
fi

alias pdout="cd $PDStudyOutputDir/$PDStudyV/"

export MYBIN=$PDStudy_WD/bin/
export PYTHONDIR=$PDStudy_WD/python/
export PATH=${MYBIN}:${PYTHONDIR}:${PATH}

export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PDStudy_WD/DataFormats/include/:$PDStudy_WD/AnalyzerTools/include/:$PDStudy_WD/Analyzers/include/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PDStudy_LIB_PATH

source $PDStudy_WD/bin/BashColorSets.sh

## submodules ##
#source bin/CheckSubmodules.sh

## Todo list ##
python python/PrintToDoLists.py
source $PDStudy_WD/tmp/ToDoLists.sh
rm $PDStudy_WD/tmp/ToDoLists.sh

## Log Dir ##
echo "* Your Log Directory Usage"
#du -sh $PDStudyRunlogDir
echo "-----------------------------------------------------------------"
CurrentGitBranch=`git branch | grep \* | cut -d ' ' -f2`
printf "> Current PDStudyAnalyzer branch : "${BRed}$CurrentGitBranch${Color_Off}"\n"
