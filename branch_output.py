import ROOT

def print_branch_names():
    # Open the ROOT file
    file = ROOT.TFile("/eos/cms/store/relval/CMSSW_15_0_0/RelValTTbar_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecoOnly_Run4D110_PU-v1/2580000/069414cc-81de-4063-9760-9ec3ef83bc11.root")
   # file=ROOT.TFile("TTbar2_TOFPID_MiniAOD.root")
    if not file or file.IsZombie():
        print("Error opening file!")
        return

    # Get the TTree
    tree = file.Get("Events")
    if not tree:
        print("Tree not found!")
        return

    # Open output file
    with open("branch_names.txt", "w") as out_file:
        # Loop over all branches in the tree
        for branch in tree.GetListOfBranches():
            # Write just the branch name
            out_file.write(branch.GetName() + "\n")

    print("Branch names saved to branch_names.txt")

print_branch_names()

