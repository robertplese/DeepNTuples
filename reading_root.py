import ROOT

def dump_branch_to_file():
    file = ROOT.TFile.Open("TTbar_TOFPID_tracks_MiniAOD.root")
    if not file or file.IsZombie():
        print("Error opening file!")
        return

    tree = file.Get("Events")  # Replace with the actual TTree name
    if not tree:
        print("TTree not found!")
        return

    with open("output.txt", "w") as outfile:
        for entry in tree:
            value = getattr(entry, "floatedmValueMap_tofPID_probP_RECO")  # branch name
            outfile.write(f"{value}\n")

dump_branch_to_file()

