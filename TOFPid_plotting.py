import uproot
import matplotlib.pyplot as plt
import awkward as ak  # for flattening jagged arrays

# Open the file and tree
file = uproot.open("test.root")
tree = file["deepntuplizer/tree"]

# Read jagged arrays
pt_array = tree["Cpfcan_pt"].array()
probK_array = tree["Cpfcan_tofPID_probK"].array()

# Flatten and convert to NumPy
pt_flat = ak.flatten(pt_array).to_numpy()
probK_flat = ak.flatten(probK_array).to_numpy()

# 2D histogram
plt.figure(figsize=(8, 6))
plt.hist2d(pt_flat, probK_flat, bins=[100, 100],
           range=[[0, 10], [0, 1]], cmap='viridis')
plt.colorbar(label="Entries")
plt.xlabel("pfCandidate $p_T$ [GeV]")
plt.ylabel("Kaon Probability (probK)")
plt.title("pfCandidate $p_T$ vs Kaon Probability")
plt.tight_layout()
plt.savefig("pt_vs_probK.png", dpi=300)
plt.close()

