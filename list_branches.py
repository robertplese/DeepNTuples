import uproot

file = uproot.open("TTbar1_TOFPID_ntuple.root")
tree = file["deepntuplizer/tree"]

print(tree.keys())  # list all branch names

