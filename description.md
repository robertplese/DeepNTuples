Relval RECO files produced in CMSSW_15_0_0 for PhaseII upgrade includes information about hadronic flavour expressed as the probability that a particle is a proton, kaon, or pion. These probabilities are calculated within the MTD software code TOFPIDProducer.cc [1].

The `CMSSW_15_0_0_TOF` branch of DeepNtuplizer implements this information and is based on previous the `phase2_142X_hgcaltiming` branch. The main changes are listed here.

1. - Changes in the code are mostly found in three files: `/plugins/DeepNtuplizer.cc`, `/src/ntuple_pfCands.cc` (and corresponding .h) and `/interface/TrackInfoBuilder.h`. 
   - Changes are usually framed by two lines consisting of the symbol `/`.<br><br>

2. - In `DeepNtuplizer.cc` all changes relate to obtaining TOFPID information (`probP`, `probK`, `probPi`) and forwarding it to the module ntuple_pfCands.<br><br>

3. - In `ntuple_pfcands.cc` corresponding changes were made handle the new information.<br><br>

4. - Biggest change is regarding `TrackInfoBuilder.h`. 
   - The TrackInfoBuilder class was previously part of several modules (`ntuple_pfCands.cc`, `ntuple_SV.cc`, `ntuple_LT.cc` etc.), but it has been moved to a separate header file `/interface/TrackInfoBuilder.h`.
   - Since only the pfCands module uses TOFPID information, specific function `setHandles` was added which assignes values to `probP`, `probK` and `probPi` if they exist (otherwise value is `-1`).<br><br>

The following snippet of `TrackInfoBuilder.h` requires special attention:
```
    if (handlesAreSet_ && packedCandidate->charge() != 0 && packedCandidate->bestTrack()) {
   	        const reco::Track* bestTr = packedCandidate->bestTrack(); 
    	    reco::TrackRef matchedTrackRef;
			
	    // Loop over tracks_ (generalTracks from RECO level) to find the one that matches the bestTrack (assigned to PackedCandidate from MiniAOD level)
    		for (size_t i = 0; i < tracks_->size(); ++i) {
        		if ((std::abs((*tracks_)[i].pt()-bestTr->pt())<0.01) &&
                (reco::deltaR((*tracks_)[i].eta(),(*tracks_)[i].phi(),bestTr->eta(),bestTr->phi()))<0.01) {
                    	matchedTrackRef = reco::TrackRef(tracks_, i);  
                        break;
                    }
    		}

    		if (matchedTrackRef.isNonnull() && matchedTrackRef.isAvailable()) {
        		if (tofPIDProton_) trackProtonProb_ = (*tofPIDProton_)[matchedTrackRef];
             ...
```
Inside the relval RECO sample `probP/K/Pi` branches are mapped to `generalTracks` branch, while `DeepNtuplizer` creates branches based on information found in `PackedCandidate`. To map `probP/K/Pi` to `PackedCandidate` in the snippet above, we compare `generalTracks` with `PackedCandidate` tracks. Unfortunately, no better solution than this crude one was found.

The problem with such algorithm is fixed magnitude of mismatch we allow (in this case `0.01` was chosen based on the output numbers for tracks). The problem arises if "track precision" varies depending on purely physical context (such as detector region or particle properties). In that case fixed value of allowed error is far from an optimal solution.
