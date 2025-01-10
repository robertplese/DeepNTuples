# DeepNTuples
NTuple framework for DeepFlavour


Installation (CMSSW 14_2_0_pre1) & setup for rereco
============

```
cmsrel CMSSW_14_2_0_pre1
cd CMSSW_14_2_0_pre1/src/
cmsenv
git cms-init
git cms-merge-topic 46019
git cms-merge-topic 46054
git cherry-pick b4e8c86523036f8211f096b9145c99a9368c452c 63e73168549cd1e30d404d2104db6c625f5a13e4 # Adding #45508 and #46060
git clone https://github.com/robertplese/DeepNTuples.git
cd DeepNTuples
git checkout phase2_142X_hgcaltiming
# Add JetToolBox
git submodule init
git submodule update

scram b -j8
```

ReReco to include HGCAL timing
========
HLT instructions from [HLT](https://cmshltupgrade.docs.cern.ch/RunningInstructions/#to-run-on-the-spring24-samples-use-cmssw_14_2_0_pre1-or-later)
- Rerun L1 

```
cmsDriver.py Phase2 -s L1,L1TrackTrigger \
--conditions auto:phase2_realistic_T33 \
--geometry Extended2026D110 \
--era Phase2C17I13M9 \
--eventcontent FEVTDEBUGHLT \
--datatier GEN-SIM-DIGI-RAW-MINIAOD \
--customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring,L1Trigger/Configuration/customisePhase2FEVTDEBUGHLT.customisePhase2FEVTDEBUGHLT,L1Trigger/Configuration/customisePhase2TTOn110.customisePhase2TTOn110 \
--filein /eos/cms/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_AllTP_140X_mcRun4_realistic_v4-v1/130000/51b389e2-ddb2-4c14-9d4f-403546254c83.root \
--fileout file:output_Phase2_L1T.root \
--python_filename rerunL1_cfg.py \
--inputCommands="keep *, drop l1tPFJets_*_*_*, drop l1tTrackerMuons_l1tTkMuonsGmt*_*_HLT" \
--outputCommands="drop l1tTrackerMuons_l1tTkMuonsGmt*_*_HLT" \
--mc \
-n -1 --nThreads 1
```

- Rerun HLT
  in the last line -n 400 is chosen due to the size of the file (eos accepts only files of 50 GB)
```
cmsDriver.py Phase2 -s L1P2GT,HLT:75e33 --processName=HLTX \
--conditions auto:phase2_realistic_T33 \
--geometry Extended2026D110 \
--era Phase2C17I13M9 \
--eventcontent FEVTDEBUGHLT \
--customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000 \
--filein file:output_Phase2_L1T.root \
--inputCommands='keep *, drop *_hlt*_*_HLT, drop triggerTriggerFilterObjectWithRefs_l1t*_*_HLT' \
--mc \
-n 400 --nThreads 1

```
- Run Rereco, using [step3](https://cmsweb.cern.ch/couchdb/reqmgr_config_cache/c6c8107a92728c9d3c7d4e36f2560c01/configFile) process, but modified the following

```
# Rerun reco step with ticl_v5
from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9
from Configuration.ProcessModifiers.ticl_v5_cff import ticl_v5

process = cms.Process('RECOX',Phase2C17I13M9,ticl_v5)

# Replace input files
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
                            fileNames = cms.untracked.vstring('file:Phase2_L1P2GT_HLT.root'),
    inputCommands = cms.untracked.vstring('keep *'),
    secondaryFileNames = cms.untracked.vstring()
)

```

- Then run the DeepNtuplizer

```
cmsRun DeepNTuples/DeepNtuplizer/production/DeepNtuplizer.py outputFile=test_ttbar_timing_phase2_npf phase2=True inputFiles=file:PPD-Phase2Spring24DIGIRECOMiniAOD-00021.root maxEvents=-1
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
