export PDStudy_WD=`pwd`
export PDStudy_LIB_PATH=$PDStudy_WD/lib/
mkdir -p $PDStudy_LIB_PATH
mkdir -p $PDStudy_WD/tar

export PDStudyV="Run1_v1"
mkdir -p $PDStudy_WD/data/$PDStudyV
export DATA_DIR=$PDStudy_WD/data/$PDStudyV

#### USER INFO ####
export PDStudyLogEmail='sungbino@fnal.gov'
export PDStudyLogWeb=''
export PDStudyLogWebDir=''


if [[ $HOSTNAME == *"dunegpvm"*"fnal.gov" ]]; then

  echo "@@@@ Working on dunegpvm"
  export PDStudyRunlogDir="/dune/app/users/$USER/Grid/PDStudyRunlog/"
  export PDStudyOutputDir="/dune/app/users/$USER/Grid/PDStudyOutput/"

  source /cvmfs/fermilab.opensciencegrid.org/packages/common/setup-env.sh ## -- For Alma 9
  source /cvmfs/larsoft.opensciencegrid.org/spack-packages/setup-env.sh
  spack load root@6.28.12

  export ROOT_INCLUDE_PATH=/cvmfs/larsoft.opensciencegrid.org/spack-packages/opt/spack/linux-almalinux9-x86_64_v2/gcc-12.2.0/root-6.28.12-sfwfmqorvxttrxgfrfhoq5kplou2pddd/include/:$ROOT_INCLUDE_PATH
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PDStudy_LIB_PATH:/cvmfs/larsoft.opensciencegrid.org/spack-packages/opt/spack/linux-almalinux9-x86_64_v2/gcc-12.2.0/root-6.28.12-sfwfmqorvxttrxgfrfhoq5kplou2pddd/lib/
else
  export ROOT_INCLUDE_PATH=/opt/homebrew/Cellar/root/6.32.04/include/:$ROOT_INCLUDE_PATH
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PDStudy_LIB_PATH:/opt/homebrew/Cellar/root/6.32.04/lib/
fi

alias pdout="cd $PDStudyOutputDir/$PDStudyV/"

export MYBIN=$PDStudy_WD/bin/
export PYTHONDIR=$PDStudy_WD/python/
export PATH=${MYBIN}:${PYTHONDIR}:${PATH}

#export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PDStudy_WD/DataFormats/include/:$PDStudy_WD/AnalyzerTools/include/:$PDStudy_WD/Analyzers/include/
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PDStudy_LIB_PATH

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
