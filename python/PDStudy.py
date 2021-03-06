#!/usr/bin/env python

import os,sys,time
import argparse
import datetime
from CheckJobStatus import *
from TimeTools import *
import random
import subprocess

## Arguments

parser = argparse.ArgumentParser(description='PDStudy Command')
parser.add_argument('-a', dest='Analyzer', default="")
parser.add_argument('-i', dest='InputSample', default="")
parser.add_argument('-l', dest='InputSampleList', default="")
parser.add_argument('-n', dest='NJobs', default=1, type=int)
parser.add_argument('-o', dest='Outputdir', default="")
parser.add_argument('-q', dest='Queue', default="fastq")
parser.add_argument('-sim', dest='Simulator', default="GEANT4")
parser.add_argument('--no_exec', action='store_true')
parser.add_argument('--userflags', dest='Userflags', default="")
parser.add_argument('--nmax', dest='NMax', default=0, type=int)
parser.add_argument('--reduction', dest='Reduction', default=1, type=float)
parser.add_argument('--memory', dest='Memory', default=0, type=float)
parser.add_argument('--batchname',dest='BatchName', default="")
args = parser.parse_args()

## make userflags as a list
Userflags = []
if args.Userflags != "":
  Userflags = (args.Userflags).split(',')

## Add Abosolute path for outputdir
if args.Outputdir!='':
  if args.Outputdir[0]!='/':
    args.Outputdir = os.getcwd()+'/'+args.Outputdir

## TimeStamp

# 1) dir/file name style
JobStartTime = datetime.datetime.now()
timestamp =  JobStartTime.strftime('%Y_%m_%d_%H%M%S')
# 2) log style
JobStartTime = datetime.datetime.now()
string_JobStartTime =  JobStartTime.strftime('%Y-%m-%d %H:%M:%S')
string_ThisTime = ""

## Environment Variables

USER = os.environ['USER']
exec('from UserInfo_'+USER+' import *')
PDStudyLogEmail = UserInfo['PDStudyLogEmail']
PDStudyLogWebDir = UserInfo['PDStudyLogWebDir']
LogEvery = UserInfo['LogEvery']

SCRAM_ARCH = os.environ['SCRAM_ARCH']
cmsswrel = os.environ['cmsswrel']
PDStudy_WD = os.environ['PDStudy_WD']
PDStudyV = os.environ['PDStudyV']
SAMPLE_DATA_DIR = PDStudy_WD+'/data/'+PDStudyV+'/'+args.Simulator+'/'
PDStudyRunlogDir = os.environ['PDStudyRunlogDir']
PDStudyOutputDir = os.environ['PDStudyOutputDir']
PDStudy_LIB_PATH = os.environ['PDStudy_LIB_PATH']
UID = str(os.getuid())
HOSTNAME = os.environ['HOSTNAME']
SampleHOSTNAME = HOSTNAME

## Check joblog email

if PDStudyLogEmail=='':
  print '[PDStudy.py] Put your email address in setup.sh'
  exit()
SendLogToWeb = True
if PDStudyLogWebDir=='':
  SendLogToWeb = False

## Check hostname

IsKISTI = ("sdfarm.kr" in HOSTNAME)
IsUI10 = ("ui10.sdfarm.kr" in HOSTNAME)
IsUI20 = ("ui20.sdfarm.kr" in HOSTNAME)
IsTAMSA1 = ("tamsa1" in HOSTNAME)
IsTAMSA2 = ("tamsa2" in HOSTNAME)
IsTAMSA = IsTAMSA1 or IsTAMSA2
IsKNU = ("knu" in HOSTNAME)
if IsKISTI:
  HOSTNAME = "KISTI"
  SampleHOSTNAME = "KISTI"
if IsTAMSA:
  if IsTAMSA1:
    HOSTNAME = "TAMSA1"
  elif IsTAMSA2:
    HOSTNAME = "TAMSA2"
  SampleHOSTNAME = "SNU"
if IsKNU:
  HOSTNAME = "KNU"
  SampleHOSTNAME = "KNU"

## Are you skimming trees?

IsSkimTree = "SkimTree" in args.Analyzer
if IsSkimTree:
  if not IsTAMSA:
    print "Skimming only possible in SNU"
    exit()

## Machine-dependent variables

if IsKNU:
  args.Queue = "cms"

## Make Sample List

InputSamples = []
StringForHash = ""

## When using txt file for input (i.e., -l option)

if args.InputSampleList is not "":
  lines = open(args.InputSampleList)
  for line in lines:
    if "#" in line:
      continue
    line = line.strip('\n')
    InputSamples.append(line)
    StringForHash += line
else:
  InputSamples.append(args.InputSample)
  StringForHash += args.InputSample
FileRangesForEachSample = []

## add flags to hash
for flag in Userflags:
  StringForHash += flag

## Get Random Number for webdir

random.seed(hash(StringForHash+timestamp))
RandomNumber = int(random.random()*1000000)
str_RandomNumber = str(RandomNumber)
webdirname = timestamp+"_"+str_RandomNumber
webdirpathbase = PDStudyRunlogDir+'/www/PDStudyAnalyzerJobLogs/'+webdirname
while os.path.isdir(webdirpathbase):
  webdirpathbase += '_'

## Define MasterJobDir

MasterJobDir = PDStudyRunlogDir+'/'+timestamp+'__'+str_RandomNumber+"__"+args.Analyzer+'__'+'Simulator'+args.Simulator
for flag in Userflags:
  MasterJobDir += '__'+flag
MasterJobDir += '__'+HOSTNAME+'/'

## Copy libray

os.system('mkdir -p '+MasterJobDir+'/lib/')
os.system('cp '+PDStudy_LIB_PATH+'/* '+MasterJobDir+'/lib')

## Loop over samples

# true or false for each sample
SampleFinishedForEachSample = []
PostJobFinishedForEachSample = []
BaseDirForEachSample = []
XsecForEachSample = []
for InputSample in InputSamples:

  NJobs = args.NJobs

  SampleFinishedForEachSample.append(False)
  PostJobFinishedForEachSample.append(False)

  ## Global Varialbes

  IsDATA = False
  DataPeriod = ""
  if ":" in InputSample:
    IsDATA = True
    tmp = InputSample
    InputSample = tmp.split(":")[0]
    DataPeriod = tmp.split(":")[1]

  ## Prepare output

  base_rundir = MasterJobDir+InputSample
  if IsDATA:
    base_rundir = base_rundir+'_period'+DataPeriod
  base_rundir = base_rundir+"/"

  os.system('mkdir -p '+base_rundir)
  os.system('mkdir -p '+base_rundir+'/output/')

  ## Create webdir
  ## cf) base_rundir = $PDStudyRunlogDir/2019_02_26_222038__GetEffLumi__Year2016__KISTI/WW_pythia/

  this_webdir = webdirpathbase+'/'+base_rundir.replace(PDStudyRunlogDir,'').replace(HOSTNAME+'/',HOSTNAME+'__')
  os.system('mkdir -p '+this_webdir)

  ## If KNU, copy grid cert

  if IsKNU:
    os.system('cp /tmp/x509up_u'+UID+' '+base_rundir)

  ## Get Sample Path

  lines_files = []

  tmpfilepath = SAMPLE_DATA_DIR+'/For'+SampleHOSTNAME+'/'+InputSample+'.txt'
  lines_files = open(tmpfilepath).readlines()
  os.system('cp '+tmpfilepath+' '+base_rundir+'/input_filelist.txt')

  NTotalFiles = len(lines_files)

  if NJobs>NTotalFiles or NJobs==0:
    NJobs = NTotalFiles

  SubmitOutput = open(base_rundir+'/SubmitOutput.log','w')

  SubmitOutput.write("<PDStudy> NTotalFiles = "+str(NTotalFiles)+'\n')
  SubmitOutput.write("<PDStudy> NJobs = "+str(NJobs)+'\n')
  nfilepjob = int(NTotalFiles/NJobs)
  SubmitOutput.write("<PDStudy> --> # of files per job = "+str(nfilepjob)+'\n')
  nfilepjob_remainder = NTotalFiles-(NJobs)*(nfilepjob)
  if nfilepjob_remainder>=(NJobs):
    SubmitOutput.write('nfilepjob_remainder = '+str(nfilepjob_remainder)+'\n')
    SubmitOutput.write('while, NJobs = '+str(NJobs)+'\n')
    SubmitOutput.write('--> exit'+'\n')
    sys.exit()

  FileRanges = []
  temp_end_largerjob = 0
  nfile_checksum = 0

  ## First nfilepjob_remainder jobs will have (nfilepjob+1) files per job

  for it_job in range(0,nfilepjob_remainder):
    FileRanges.append(range(it_job*(nfilepjob+1),(it_job+1)*(nfilepjob+1)))
    temp_end_largerjob = (it_job+1)*(nfilepjob+1)
    nfile_checksum += len(range(it_job*(nfilepjob+1),(it_job+1)*(nfilepjob+1)))

  ## Remaining NJobs-nfilepjob_remainder jobs will have (nfilepjob) files per job

  for it_job in range(0,NJobs-nfilepjob_remainder):
    FileRanges.append(range(temp_end_largerjob+(it_job*nfilepjob),temp_end_largerjob+((it_job+1)*nfilepjob) ))
    nfile_checksum += len(range(temp_end_largerjob+(it_job*nfilepjob),temp_end_largerjob+((it_job+1)*nfilepjob) ))
  SubmitOutput.write('nfile_checksum = '+str(nfile_checksum)+'\n')
  SubmitOutput.write('NTotalFiles = '+str(NTotalFiles)+'\n')
  SubmitOutput.close()
  FileRangesForEachSample.append(FileRanges)

  ## Get xsec and SumW

  this_dasname = ""
  this_xsec = -1
  this_sumw = -1
  if not IsDATA:
    lines_SamplePath = open(SAMPLE_DATA_DIR+'/CommonSampleInfo/'+InputSample+'.txt').readlines()
    for line in lines_SamplePath:
      if line[0]=="#":
        continue
      words = line.split()
      if InputSample==words[0]:
        #this_dasname = words[1]
        #this_xsec = words[2]
        #this_sumw = words[4]
        break

      #XsecForEachSample.append(this_xsec)

  ## Write run script

  if IsKISTI or IsTAMSA:

    commandsfilename = args.Analyzer+'_'+args.Simulator+'_'+InputSample
    if IsDATA:
      commandsfilename += '_'+DataPeriod
    for flag in Userflags:
      commandsfilename += '__'+flag
    run_commands = open(base_rundir+'/'+commandsfilename+'.sh','w')
    print>>run_commands,'''#!/bin/bash
SECTION=`printf $1`
WORKDIR=`pwd`

SumNoAuth=999
Trial=0

#### make sure use C locale
export LC_ALL=C

#### Don't make root history
export ROOT_HIST=0

#### use cvmfs for root ####
export CMS_PATH=/cvmfs/cms.cern.ch
source $CMS_PATH/cmsset_default.sh
export SCRAM_ARCH={2}
export cmsswrel={3}
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/src
echo "@@@@ SCRAM_ARCH = "$SCRAM_ARCH
echo "@@@@ cmsswrel = "$cmsswrel
echo "@@@@ scram..."
eval `scramv1 runtime -sh`
cd -
source /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/$cmsswrel/external/$SCRAM_ARCH/bin/thisroot.sh

### modifying LD_LIBRARY_PATH to use libraries in base_rundir
export LD_LIBRARY_PATH=$(echo $LD_LIBRARY_PATH|sed 's@'$PDStudy_WD'/lib@{0}/lib@')

while [ "$SumNoAuth" -ne 0 ]; do

  if [ "$Trial" -gt 9999 ]; then
    break
  fi

  echo "#### running ####"
  echo "root -l -b -q {1}/run_${{SECTION}}.C"
  root -l -b -q {1}/run_${{SECTION}}.C 2> err.log || echo "EXIT_FAILURE" >> err.log
  NoAuthError_Open=`grep "Error in <TNetXNGFile::Open>" err.log -R | wc -l`
  NoAuthError_Close=`grep "Error in <TNetXNGFile::Close>" err.log -R | wc -l`

  SumNoAuth=$(($NoAuthError_Open + $NoAuthError_Close))

  if [ "$SumNoAuth" -ne 0 ]; then
    echo "SumNoAuth="$SumNoAuth
    echo "AUTH error occured.. running again in 30 seconds.."
    Trial=$((Trial+=1))
    sleep 30
  fi

done

cat err.log >&2
'''.format(MasterJobDir, base_rundir, SCRAM_ARCH, cmsswrel)
    run_commands.close()

    submit_command = open(base_rundir+'/submit.jds','w')
    if IsUI10:
      print>>submit_command,'''executable = {1}.sh
universe   = vanilla
arguments  = $(Process)
requirements = OpSysMajorVer == 6
log = condor.log
getenv     = True
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
output = job_$(Process).log
error = job_$(Process).err
transfer_output_remaps = "hists.root = output/hists_$(Process).root"
queue {0}
'''.format(str(NJobs), commandsfilename)
      submit_command.close()
    elif IsUI20:
      print>>submit_command,'''executable = {1}.sh
universe   = vanilla
requirements = ( HasSingularity == true )
arguments  = $(Process)
log = condor.log
getenv     = True
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
output = job_$(Process).log
error = job_$(Process).err
accounting_group=group_cms
+SingularityImage = "/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el6:latest"
+SingularityBind = "/cvmfs, /cms, /share"
transfer_output_remaps = "hists.root = output/hists_$(Process).root"
queue {0}
'''.format(str(NJobs), commandsfilename)
      submit_command.close()
    elif IsTAMSA:
      concurrency_limits=''
      if args.NMax:
        concurrency_limits='concurrency_limits = n'+str(args.NMax)+'.'+os.getenv("USER")
      request_memory=''
      if args.Memory:
        request_memory='request_memory = '+str(args.Memory)
      print>>submit_command,'''executable = {1}.sh
universe   = vanilla
arguments  = $(Process)
log = condor.log
getenv     = True
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
output = job_$(Process).log
error = job_$(Process).err
transfer_output_remaps = "hists.root = output/hists_$(Process).root"
{2}
{3}
queue {0}
'''.format(str(NJobs), commandsfilename,concurrency_limits,request_memory)
      submit_command.close()

  CheckTotalNFile=0
  for it_job in range(0,len(FileRanges)):

    #print "["+str(it_job)+"th]",
    #print FileRanges[it_job],
    #print " --> "+str(len(FileRanges[it_job]))

    CheckTotalNFile = CheckTotalNFile+len(FileRanges[it_job])

    thisjob_dir = base_rundir+'/job_'+str(it_job)+'/'

    runfunctionname = "run"
    libdir = (MasterJobDir+'/lib').replace('///','/').replace('//','/')+'/'
    runCfileFullPath = ""
    if IsKISTI or IsTAMSA:
      runfunctionname = "run_"+str(it_job)
      runCfileFullPath = base_rundir+'/run_'+str(it_job)+'.C'
    else:
      os.system('mkdir -p '+thisjob_dir)
      runCfileFullPath = thisjob_dir+'run.C'

    IncludeLine = ''

    out = open(runCfileFullPath, 'w')
    print>>out,'''{3}

void {2}(){{

  {0} m;


'''.format(args.Analyzer, libdir, runfunctionname, IncludeLine)

    out.write('  m.LogEvery = '+str(LogEvery)+';\n')
    out.write('  m.MCSample = "'+InputSample+'";\n')
    out.write('  m.Simulator = "'+str(args.Simulator)+'";\n')

    if len(Userflags)>0:
      out.write('  m.Userflags = {\n')
      for flag in Userflags:
        out.write('    "'+flag+'",\n')
      out.write('  };\n')

    out.write('  m.SetTreeName();\n')
    for it_file in FileRanges[it_job]:
      thisfilename = lines_files[it_file].strip('\n')
      out.write('  m.AddFile("'+thisfilename+'");\n')

    if IsTAMSA:
      out.write('  m.SetOutfilePath("hists.root");\n')
    else:
      out.write('  m.SetOutfilePath("'+thisjob_dir+'/hists.root");\n')

    if args.Reduction>1:
      out.write('  m.MaxEvent=m.fChain->GetEntries()/'+str(args.Reduction)+';\n')

    print>>out,'''  m.Init();
  m.initializeAnalyzer();
  m.initializeAnalyzerTools();
  m.SwitchToTempDir();
  m.Loop();

  m.WriteHist();

}'''

    out.close()

    if IsKNU:
      run_commands = open(thisjob_dir+'commands.sh','w')
      print>>run_commands,'''cd {0}
cp ../x509up_u{1} /tmp/
echo "[PDStudy.py] Okay, let's run the analysis"
root -l -b -q run.C 1>stdout.log 2>stderr.log
'''.format(thisjob_dir,UID)
      run_commands.close()

      jobname = 'job_'+str(it_job)+'_'+args.Analyzer
      cmd = 'qsub -V -q '+args.Queue+' -N '+jobname+' commands.sh'

      if not args.no_exec:
        cwd = os.getcwd()
        os.chdir(thisjob_dir)
        os.system(cmd+' > submitlog.log')
        os.chdir(cwd)
      sublog = open(thisjob_dir+'/submitlog.log','a')
      sublog.write('\nSubmission command was : '+cmd+'\n')
      sublog.close()

  if IsKISTI or IsTAMSA:

    cwd = os.getcwd()
    os.chdir(base_rundir)
    if not args.no_exec:
      condorOptions = ''
      if args.BatchName!="":
        condorOptions = ' -batch-name '+args.BatchName
      os.system('condor_submit submit.jds '+condorOptions) ## -- Submit to Condor
    os.chdir(cwd)

  else:

    if args.no_exec:
      continue

    ## Write Kill Command

    KillCommand = open(base_rundir+'/Script_JobKill.sh','w')
    for it_job in range(0,len(FileRanges)):
      thisjob_dir = base_rundir+'/job_'+str(it_job)+'/'
      jobid = GetJobID(thisjob_dir, args.Analyzer, it_job, HOSTNAME)
      KillCommand.write('qdel '+jobid+' ## job_'+str(it_job)+' ##\n')
    KillCommand.close()

if args.no_exec:
  exit()

## Set Output directory
## if args.Outputdir is not set, go to default setting

FinalOutputPath = args.Outputdir
if args.Outputdir=="":
  FinalOutputPath = PDStudyOutputDir+'/'+PDStudyV+'/'+args.Analyzer+'/'+args.Simulator+'/'
  for flag in Userflags:
    FinalOutputPath += flag+"__"

os.system('mkdir -p '+FinalOutputPath)

print '##################################################'
print 'Submission Finished'
print '- JobID = '+str_RandomNumber
print '- Analyzer = '+args.Analyzer
print '- InputSamples =',
print InputSamples
print '- NJobs = '+str(NJobs)
print '- Simulator = '+args.Simulator
print '- UserFlags =',
print Userflags
if IsKNU:
  print '- Queue = '+args.Queue
print '- output will be send to : '+FinalOutputPath
print '##################################################'

##########################
## Submittion all done. ##
## Now monitor job      ##
##########################

## Loop over samples again

AllSampleFinished = False
GotError = False
ErrorLog = ""

try:
  while not AllSampleFinished:

    if GotError:
      break

    AllSampleFinished = True

    for it_sample in range(0,len(InputSamples)):

      InputSample = InputSamples[it_sample]
      SampleFinished = SampleFinishedForEachSample[it_sample]
      PostJobFinished = PostJobFinishedForEachSample[it_sample]

      if PostJobFinished:
        continue
      else:
        AllSampleFinished = False

      ## Global Varialbes

      IsDATA = False
      DataPeriod = ""
      if ":" in InputSample:
        IsDATA = True
        tmp = InputSample
        InputSample = tmp.split(":")[0]
        DataPeriod = tmp.split(":")[1]

      ## Prepare output
      ## This should be copied from above

      base_rundir = MasterJobDir+InputSample
      if IsDATA:
        base_rundir = base_rundir+'_period'+DataPeriod
      base_rundir = base_rundir+"/"

      ## base_rundir = $PDStudyRunlogDir/2019_02_26_222038__GetEffLumi__Year2016__KISTI/WW_pythia/

      this_webdir = webdirpathbase+'/'+base_rundir.replace(PDStudyRunlogDir,'').replace(HOSTNAME+'/',HOSTNAME+'__')

      if not SampleFinished:

        ## This sample was not finished in the previous monitoring
        ## Monitor again this time

        ThisSampleFinished = True

        ## Write Job status until it's done

        statuslog = open(base_rundir+'/JobStatus.log','w')
        statuslog.write('Job submitted at '+string_JobStartTime+'\n')
        statuslog.write('JobNumber\t| Status\n')

        ToStatuslog = []
        n_eventran = 0
        finished = []
        EventDone = 0
        EventTotal = 0

        TotalEventRunTime = 0
        MaxTimeLeft = 0
        MaxEventRunTime = 0

        FileRanges = FileRangesForEachSample[it_sample]

        for it_job in range(0,len(FileRanges)):

          thisjob_dir = base_rundir+'/'
          if IsKISTI or IsTAMSA:
            thisjob_dir = base_rundir

          this_status = ""
          this_status = CheckJobStatus(thisjob_dir, args.Analyzer, it_job, HOSTNAME)

          if "ERROR" in this_status:
            GotError = True
            statuslog.write("#### ERROR OCCURED ####\n")
            statuslog.write(this_status+'\n')
            ErrorLog = this_status
            break

          if "FINISHED" not in this_status:
            ThisSampleFinished = False
            
          outlog = ""
          if "FINISHED" in this_status:
            finished.append("Finished")

            EventInfo = this_status.split()[1].split(':')

            this_EventDone = int(EventInfo[2])
            this_EventTotal = int(EventInfo[2])

            EventDone += this_EventDone
            EventTotal += this_EventTotal

            #### start
            line_EventRunTime = this_status.split()[2]+' '+this_status.split()[3]
            this_jobstarttime = GetDatetimeFromMyFormat(line_EventRunTime)
            #### end
            line_EventEndTime = this_status.split()[4]+' '+this_status.split()[5]
            this_jobendtime   = GetDatetimeFromMyFormat(line_EventEndTime)

            this_diff = this_jobendtime-this_jobstarttime
            this_EventRunTime = 86400*this_diff.days+this_diff.seconds

            this_TimePerEvent = float(this_EventRunTime)/float(this_EventDone)
            this_TimeLeft = (this_EventTotal-this_EventDone)*this_TimePerEvent

            TotalEventRunTime += this_EventRunTime
            MaxTimeLeft = max(MaxTimeLeft,this_TimeLeft)
            MaxEventRunTime = max(MaxEventRunTime,this_EventRunTime)

          elif "RUNNING" in this_status:
            outlog = str(it_job)+'\t| '+this_status.split()[1]+' %'

            EventInfo = this_status.split()[2].split(':')

            this_EventDone = int(EventInfo[1])
            this_EventTotal = int(EventInfo[2])

            EventDone += this_EventDone
            EventTotal += this_EventTotal

            line_EventRunTime = this_status.split()[3]+' '+this_status.split()[4]
            this_jobstarttime = GetDatetimeFromMyFormat(line_EventRunTime)
            this_diff = datetime.datetime.now()-this_jobstarttime
            this_EventRunTime = 86400*this_diff.days+this_diff.seconds

            if this_EventDone==0:
              this_EventDone = 1

            this_TimePerEvent = float(this_EventRunTime)/float(this_EventDone)
            this_TimeLeft = (this_EventTotal-this_EventDone)*this_TimePerEvent

            TotalEventRunTime += this_EventRunTime
            MaxTimeLeft = max(MaxTimeLeft,this_TimeLeft)
            MaxEventRunTime = max(MaxEventRunTime,this_EventRunTime)

            round_this_TimeLeft = round(this_TimeLeft,1)
            round_this_EventRunTime = round(this_EventRunTime,1)

            outlog += ' ('+str(round_this_EventRunTime)+' s ran, and '+str(round_this_TimeLeft)+' s left)'
            ToStatuslog.append(outlog)
            n_eventran += 1

          else:
            outlog = str(it_job)+'\t| '+this_status
            ToStatuslog.append(outlog)

          ##---- END it_job loop

        if GotError:

          ## When error occured, change both Finished/PostJob Flag to True

          SampleFinishedForEachSample[it_sample] = True
          PostJobFinishedForEachSample[it_sample] = True
          break

        for l in ToStatuslog:
          statuslog.write(l+'\n')
        statuslog.write('\n==============================================================\n')
        statuslog.write('HOSTNAME = '+HOSTNAME+'\n')
        statuslog.write('queue = '+args.Queue+'\n')
        statuslog.write(str(len(FileRanges))+' jobs submitted\n')
        statuslog.write(str(n_eventran)+' jobs are running\n')
        statuslog.write(str(len(finished))+' jobs are finished\n')

        ThisTime = datetime.datetime.now()
        string_ThisTime =  ThisTime.strftime('%Y-%m-%d %H:%M:%S')

        statuslog.write('EventDone = '+str(EventDone)+'\n')
        statuslog.write('EventTotal = '+str(EventTotal)+'\n')
        statuslog.write('EventLeft = '+str(EventTotal-EventDone)+'\n')
        statuslog.write('TotalEventRunTime = '+str(TotalEventRunTime)+'\n')
        statuslog.write('MaxTimeLeft = '+str(MaxTimeLeft)+'\n')
        statuslog.write('MaxEventRunTime = '+str(MaxEventRunTime)+'\n')

        t_per_event = 1
        if EventDone is not 0:
          t_per_event = float(TotalEventRunTime)/float(EventDone)
        statuslog.write('t_per_event = '+str(t_per_event)+'\n')

        EstTime = ThisTime+datetime.timedelta(0, MaxTimeLeft)

        statuslog.write('Estimated Finishing Time : '+EstTime.strftime('%Y-%m-%d %H:%M:%S')+'\n')
        statuslog.write('Last checked at '+string_ThisTime+'\n')
        statuslog.close()

        ## copy statuslog to webdir

        os.system('cp '+base_rundir+'/JobStatus.log '+this_webdir)

        ## This time, it is found to be finished
        ## Change the flag

        if ThisSampleFinished:
          SampleFinishedForEachSample[it_sample] = True

        ##---- END if finished

      else:

        ## Job was finished in the previous monitoring
        ## Check if PostJob is also finished

        if not PostJobFinished:

          ## PostJob was not done in the previous monitoring
          ## Copy output, and change the PostJob flag


          ## if Skim, no need to hadd. move on!

          if IsSkimTree:
            PostJobFinishedForEachSample[it_sample] = True
            continue

          outputname = args.Analyzer+'_'+InputSample

          if not GotError:
            cwd = os.getcwd()
            os.chdir(base_rundir)

            #### if number of job is 1, we can just move the file, not hadd
            nFiles = len( FileRangesForEachSample[it_sample] )
            if nFiles==1:
              if IsKISTI or IsTAMSA:
                os.system('echo "nFiles = 1, so skipping hadd and just move the file" >> JobStatus.log')
                os.system('ls -1 output/*.root >> JobStatus.log')
                os.system('mv output/hists_0.root '+outputname+'.root')
              else:
                os.system('echo "nFiles = 1, so skipping hadd and just move the file" >> JobStatus.log')
                os.system('ls -1 job_0/*.root >> JobStatus.log')
                os.system('mv job_0/hists.root '+outputname+'.root')

            else:
              if IsKISTI or IsTAMSA:
                os.system('hadd -f '+outputname+'.root output/*.root >> JobStatus.log')
                os.system('rm output/*.root')
              else:
                os.system('hadd -f '+outputname+'.root job_*/*.root >> JobStatus.log')
                os.system('rm job_*/*.root')

            ## Final Outputpath

            os.system('mv '+outputname+'.root '+FinalOutputPath)
            os.chdir(cwd)

          PostJobFinishedForEachSample[it_sample] = True

    if SendLogToWeb:

      os.system('cp -r '+webdirpathbase+'/* '+PDStudyLogWebDir)

    time.sleep(20)

except KeyboardInterrupt:
  print('interrupted!')

## Send Email now

from SendEmail import *
JobFinishEmail = '''#### Job Info ####
HOST = {3}
JobID = {5}
Analyzer = {0}
Simulator = {6}
# of Jobs = {4}
InputSample = {1}
Output sent to : {2}
'''.format(args.Analyzer,InputSamples,FinalOutputPath,HOSTNAME,NJobs,str_RandomNumber,args.Simulator)
JobFinishEmail += '''##################
Job started at {0}
Job finished at {1}
'''.format(string_JobStartTime,string_ThisTime)

if IsKNU:
  JobFinishEmail += 'Queue = '+args.Queue+'\n'

EmailTitle = '['+HOSTNAME+']'+' Summary of JobID '+str_RandomNumber
if GotError:
  JobFinishEmail = "#### ERROR OCCURED ####\n"+JobFinishEmail
  JobFinishEmail = ErrorLog+"\n------------------------------------------------\n"+JobFinishEmail
  EmailTitle = '[ERROR] Summary of JobID '+str_RandomNumber

SendEmail(USER,PDStudyLogEmail,EmailTitle,JobFinishEmail)
