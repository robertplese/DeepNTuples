# DeepNTuples
NTuple framework for DeepFlavour


Installation (CMSSW 14_2_0_pre1) & setup for rereco
============

```
cmsrel CMSSW_15_0_0
cd CMSSW_15_0_0/src/
cmsenv
git cms-init
git cms-merge-topic 46019
git cms-merge-topic 46054
git clone https://github.com/robertplese/DeepNTuples.git
cd DeepNTuples
git checkout 15_0_0_TOF
# Add JetToolBox
git submodule init
git submodule update

scram b -j8
```

Producing required MiniAOD files
========
```
cmsDriver.py miniAOD-produce \
  --filein file:/eos/cms/store/relval/CMSSW_15_0_0/RelValTTbar_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecoOnly_Run4D110_PU-v1/2580000/069414cc-81de-4063-9760-9ec3ef83bc11.root \
  --fileout file:TTbar_TOFPID_MiniAOD.root \
  --mc \
  --eventcontent MINIAODSIM \
  --datatier MINIAODSIM \
  --conditions 141X_mcRun4_realistic_v3 \
  --step PAT \
  --geometry ExtendedRun4D110 \
  --era Phase2C17I13M9 \
  --runUnscheduled \
  --no_exec \
  -n -1

```
- open produced .py file (in this case miniAOD-produce_PAT.py) and add these two lines in "Path and EndPath definitions" section:

```
process.MINIAODSIMoutput.outputCommands.append('keep *_*tofPID*_*_*');
process.MINIAODSIMoutput.outputCommands.append('keep recoTracks_generalTracks_*_*');
```
- produce MiniAOD file with

```
cmsRun miniAOD-produce_PAT.py
```


Producing ntuples
========

- run the DeepNtuplizer with produced file as an input

```
cmsRun DeepNtuplizer/production/DeepNtuplizer.py outputFile=TOFPID_TTbar phase2=True inputFiles=file:TTbar_TOFPID_MiniAOD.root maxEvents=-1
```

Further settings
============

It is important to create your grid proxy in a location that is accessible by other nodes (there is no security issue, your full credentials are still needed for access). For this purpose, redirect the grid proxy location by adding the following to your login script:

```
export X509_USER_PROXY=${HOME}/.gridproxy.pem
```




Production
==========

Before doing a batch submission you can test the ntuplizer locally in the production directory with:
```
cmsRun DeepNtuplizer.py inputFiles=/path/to/file.root
```
The jobs can be submitted using the following syntax
```
jobSub.py --file <sample file> DeepNtuplizer.py <batch directory> --outpath /path/to/output/directory/
```
For an example of sample files, please refer to the .cfg files already in the production directory. You first specify the number of jobs to be submitted, then the input dataset name, which should then be followed by the name of the output. Other arguments such as gluonReduction can then be specified if needed. Each argument need to be separted by at least two whitespaces.
 
The large job output (root files) will NOT be stored in the batch directory. The storage directory is specified by the --outpath argument. The batch directory will contain a symlink to this directory. If the outpath is not specified the ntuples are stored in the deepjet directory, where you need write permission.

The status of the jobs can be checked with
```
cd <batch directory>
check.py <sample subdirectories to be checked>
```

The check.py script provides additional options to resubmit failed jobs or to create sample lists in case a satisfying fraction of jobs ended successfully. 
In this case do:
```
check.py <sample subdirectories to be checked> --action filelist
```
This will create file lists that can be further processed by the DeepJet framework
For resubmitting failed jobs, do:
```
check.py <sample subdirectories to be checked> --action resubmit
```

When the file lists are created, the part used for training of the ttbar and QCD samples (or in principle any other process) can be merged using the executable:
```
mergeSamples.py <no of jets per file> <output dir> <file lists 1> <file lists 2> <file lists 3> ...
```
Here the output directory cannot already exist. For example:
```
mergeSamples.py 400000 /path/to/dir/merged ntuple_*/train_val_samples.txt
```
This will take a significant amount of time - likely more than the ntuple production itself. It is therefore recommended to run the command within 'screen'. In the 106X branch you can also submit via batch by doing --batch. This will create a batch directory in the folder the command is called from.

```
mergeSamples.py 400000 /path/to/dir/merged ntuple_*/train_val_samples.txt --batch
```
