#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_TRACKINFOBUILDER_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_TRACKINFOBUILDER_H_

#include "ntuple_content.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
//#include "DataFormats/TrackReco/interface/TrackFwd.H"

class TrackInfoBuilder {
public:
    TrackInfoBuilder(edm::ESHandle<TransientTrackBuilder>& build) :
        builder(build),
        trackMomentum_(0),
        trackEta_(0),
        trackEtaRel_(0),
        trackPtRel_(0),
        trackPPar_(0),
        trackDeltaR_(0),
        trackPtRatio_(0),
        trackPParRatio_(0),
        trackSip2dVal_(0),
        trackSip2dSig_(0),
        trackSip3dVal_(0),
        trackSip3dSig_(0),
        trackJetDecayLen_(0),
        trackJetDistVal_(0),
        trackJetDistSig_(0),
        ttrack_()
    {
    }

    void buildTrackInfo(const pat::PackedCandidate* packedCandidate, const math::XYZVector& jetDir, GlobalVector refjetdirection, const reco::Vertex& pv) {
        TVector3 jetDir3(jetDir.x(), jetDir.y(), jetDir.z());
        if (!packedCandidate->hasTrackDetails()) {
            TVector3 trackMom3(
                packedCandidate->momentum().x(),
                packedCandidate->momentum().y(),
                packedCandidate->momentum().z()
            );
            trackMomentum_ = packedCandidate->p();
            trackEta_ = packedCandidate->eta();
            trackEtaRel_ = reco::btau::etaRel(jetDir, packedCandidate->momentum());
            trackPtRel_ = trackMom3.Perp(jetDir3);
            trackPPar_ = jetDir.Dot(packedCandidate->momentum());
            trackDeltaR_ = reco::deltaR(packedCandidate->momentum(), jetDir);
            trackPtRatio_ = trackMom3.Perp(jetDir3) / packedCandidate->p();
            trackPParRatio_ = jetDir.Dot(packedCandidate->momentum()) / packedCandidate->p();
            trackSip2dVal_ = 0.;
            trackSip2dSig_ = 0.;
            trackSip3dVal_ = 0.;
            trackSip3dSig_ = 0.;
            trackJetDecayLen_ = 0.;
            trackJetDistVal_ = 0.;
            trackJetDistSig_ = 0.;
            trackProtonProb_ = -1.;
            trackKaonProb_ = -1.;
            trackPionProb_ = -1.;
            return;
        }

        const reco::Track& pseudoTrack = packedCandidate->pseudoTrack();

        reco::TransientTrack transientTrack = builder->build(pseudoTrack);
        Measurement1D meas_ip2d = IPTools::signedTransverseImpactParameter(transientTrack, refjetdirection, pv).second;
        Measurement1D meas_ip3d = IPTools::signedImpactParameter3D(transientTrack, refjetdirection, pv).second;
        Measurement1D jetdist = IPTools::jetTrackDistance(transientTrack, refjetdirection, pv).second;
        Measurement1D decayl = IPTools::signedDecayLength3D(transientTrack, refjetdirection, pv).second;
        math::XYZVector trackMom = pseudoTrack.momentum();
        double trackMag = std::sqrt(trackMom.Mag2());
        TVector3 trackMom3(trackMom.x(), trackMom.y(), trackMom.z());

        trackMomentum_ = trackMag;
        trackEta_ = trackMom.Eta();
        trackEtaRel_ = reco::btau::etaRel(jetDir, trackMom);
        trackPtRel_ = trackMom3.Perp(jetDir3);
        trackPPar_ = jetDir.Dot(trackMom);
        trackDeltaR_ = reco::deltaR(trackMom, jetDir);
        trackPtRatio_ = trackMom3.Perp(jetDir3) / trackMag;
        trackPParRatio_ = jetDir.Dot(trackMom) / trackMag;

        trackSip2dVal_ = meas_ip2d.value();
        trackSip2dSig_ = meas_ip2d.significance();
        trackSip3dVal_ = meas_ip3d.value();
        trackSip3dSig_ = meas_ip3d.significance();

        trackJetDecayLen_ = decayl.value();
        trackJetDistVal_ = jetdist.value();
        trackJetDistSig_ = jetdist.significance();

        ttrack_ = transientTrack;

        trackProtonProb_ = -1.f;  // default if not found
        trackKaonProb_ = -1.f;
        trackPionProb_ = -1.f;
        
        if (handlesAreSet_ && packedCandidate->charge() != 0 && packedCandidate->bestTrack()) {
   	 const reco::Track* bestTr = packedCandidate->bestTrack(); 
    	 reco::TrackRef matchedTrackRef;


	    // Loop over tracks_ (generalTracks from RECO level) to find the one that matches the bestTrack (assigned to PackedCandidate from MiniAOD level)
    		for (size_t i = 0; i < tracks_->size(); ++i) {
        		if ((std::abs((*tracks_)[i].pt()-bestTr->pt())<0.01) && 
            	 	(std::abs((*tracks_)[i].eta()-bestTr->eta())<0.01) && 
            	 	(std::abs((*tracks_)[i].phi()-bestTr->phi())<0.01)) {
            			matchedTrackRef = reco::TrackRef(tracks_, i);  // build TrackRef from collection and index
            			break;
        		}
    		}

    		if (matchedTrackRef.isNonnull() && matchedTrackRef.isAvailable()) {
        		if (tofPIDProton_) trackProtonProb_ = (*tofPIDProton_)[matchedTrackRef];
			if (tofPIDKaon_)   trackKaonProb_   = (*tofPIDKaon_)[matchedTrackRef];
        		if (tofPIDPion_)   trackPionProb_   = (*tofPIDPion_)[matchedTrackRef];
    		} else {
        		trackProtonProb_ = -1;
        		trackKaonProb_   = -1;
        		trackPionProb_   = -1;   
    		}
	}
        
    }

   void setHandles(const edm::Handle<edm::ValueMap<float>>& tofPIDProtonHandle,
                    const edm::Handle<edm::ValueMap<float>>& tofPIDKaonHandle,
                    const edm::Handle<edm::ValueMap<float>>& tofPIDPionHandle,
                    const edm::Handle<reco::TrackCollection>& tracksHandle) {
        tofPIDProton_ = tofPIDProtonHandle;
        tofPIDKaon_   = tofPIDKaonHandle;
        tofPIDPion_   = tofPIDPionHandle;
        tracks_       = tracksHandle;
        handlesAreSet_ = true;
    }

    const float& getTrackDeltaR() const { return trackDeltaR_; }
    const float& getTrackEta() const { return trackEta_; }
    const float& getTrackEtaRel() const { return trackEtaRel_; }
    const float& getTrackJetDecayLen() const { return trackJetDecayLen_; }
    const float& getTrackJetDistSig() const { return trackJetDistSig_; }
    const float& getTrackJetDistVal() const { return trackJetDistVal_; }
    const float& getTrackMomentum() const { return trackMomentum_; }
    const float& getTrackPPar() const { return trackPPar_; }
    const float& getTrackPParRatio() const { return trackPParRatio_; }
    const float& getTrackPtRatio() const { return trackPtRatio_; }
    const float& getTrackPtRel() const { return trackPtRel_; }
    const float& getTrackSip2dSig() const { return trackSip2dSig_; }
    const float& getTrackSip2dVal() const { return trackSip2dVal_; }
    const float& getTrackSip3dSig() const { return trackSip3dSig_; }
    const float& getTrackSip3dVal() const { return trackSip3dVal_; }
    const reco::TransientTrack getTTrack() const { return ttrack_; }

    const float& getTrackProtonProb() const { return trackProtonProb_; }
    const float& getTrackKaonProb() const { return trackKaonProb_; }
    const float& getTrackPionProb() const { return trackPionProb_; }


private:
    edm::ESHandle<TransientTrackBuilder>& builder;

    float trackMomentum_;
    float trackEta_;
    float trackEtaRel_;
    float trackPtRel_;
    float trackPPar_;
    float trackDeltaR_;
    float trackPtRatio_;
    float trackPParRatio_;
    float trackSip2dVal_;
    float trackSip2dSig_;
    float trackSip3dVal_;
    float trackSip3dSig_;

    float trackJetDecayLen_;
    float trackJetDistVal_;
    float trackJetDistSig_;
    reco::TransientTrack ttrack_;

    float trackProtonProb_ = -1.f;
    float trackKaonProb_ = -1.f;
    float trackPionProb_ = -1.f;

    edm::Handle<edm::ValueMap<float>> tofPIDProton_;
    edm::Handle<edm::ValueMap<float>> tofPIDKaon_;
    edm::Handle<edm::ValueMap<float>> tofPIDPion_;
    edm::Handle<reco::TrackCollection> tracks_;
    bool handlesAreSet_ = false;


};

#endif /* DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_TRACKINFOBUILDER_H_ */

