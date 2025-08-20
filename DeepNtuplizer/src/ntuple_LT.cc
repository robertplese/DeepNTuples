/*
 * ntuple_LT.cc
 *
 *  Created on: 28 Mai 2023
 *      Author: Alexandre De Moor
 */


#include "../interface/ntuple_LT.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "../interface/sorting_modules.h"


#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "TVector3.h"

#include "DataFormats/GeometrySurface/interface/Line.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalTrajectoryExtrapolatorToLine.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"

#include "../interface/TrackInfoBuilder.h"

void ntuple_LT::readSetup(const edm::EventSetup& iSetup){
  builder = iSetup.getHandle(track_builder_token_);
}

void ntuple_LT::getInput(const edm::ParameterSet& iConfig){
  min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
}

void ntuple_LT::initBranches(TTree* tree){

  addBranch(tree,"n_LT", &n_LT_,"n_LT_/I");

  addBranch(tree,"LT_pt", &LT_pt_,"LT_pt_[n_Cpfcand_]/F");
  addBranch(tree,"LT_eta", &LT_eta_,"LT_eta_[n_Cpfcand_]/F");
  addBranch(tree,"LT_phi", &LT_phi_,"LT_phi_[n_Cpfcand_]/F");
  addBranch(tree,"LT_e", &LT_e_,"LT_e_[n_Cpfcand_]/F");

  addBranch(tree,"LT_puppiw",&LT_puppiw_,"LT_puppiw_[n_Cpfcand_]/F");
  addBranch(tree,"LT_dz",&LT_dz_,"LT_dz_[n_Cpfcand_]/F");

  addBranch(tree,"LT_drminsv",&LT_drminsv_,"LT_drminsv_[n_Cpfcand_]/F");
  addBranch(tree,"LT_distminsvold",&LT_distminsvold_,"LT_distminsvold_[n_Cpfcand_]/F");
  addBranch(tree,"LT_distminsv",&LT_distminsv_,"LT_distminsv_[n_Cpfcand_]/F");
  addBranch(tree,"LT_distminsv2",&LT_distminsv2_,"LT_distminsv2_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_BtagPf_trackEtaRel",&LT_BtagPf_trackEtaRel_,"LT_BtagPf_trackEtaRel_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPtRel",&LT_BtagPf_trackPtRel_,"LT_BtagPf_trackPtRel_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPPar",&LT_BtagPf_trackPPar_,"LT_BtagPf_trackPPar_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackDeltaR",&LT_BtagPf_trackDeltaR_,"LT_BtagPf_trackDeltaR_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPParRatio",&LT_BtagPf_trackPParRatio_,"LT_BtagPf_trackPParRatio[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip3dVal",&LT_BtagPf_trackSip3dVal_,"LT_BtagPf_trackSip3dVal_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip3dSig",&LT_BtagPf_trackSip3dSig_,"LT_BtagPf_trackSip3dSig_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip2dVal",&LT_BtagPf_trackSip2dVal_,"LT_BtagPf_trackSip2dVal_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip2dSig",&LT_BtagPf_trackSip2dSig_,"LT_BtagPf_trackSip2dSig_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackDecayLen",&LT_BtagPf_trackDecayLen_,"LT_BtagPf_trackDecayLen_[n_Cpfcand_]/F");
  addBranch(tree,"LT_BtagPf_trackJetDistVal",&LT_BtagPf_trackJetDistVal_,"LT_BtagPf_trackJetDistVal_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_charge",&LT_charge_,"LT_charge_[n_Cpfcand_]/F");
  addBranch(tree,"LT_chi2",&LT_chi2_,"LT_chi2_[n_Cpfcand_]/F");
  addBranch(tree,"LT_quality",&LT_quality_,"LT_quality_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_nhitpixelBarrelLayer1",&LT_nhitpixelBarrelLayer1_,"LT_nhitpixelBarrelLayer1_[n_Cpfcand_]/F");
  addBranch(tree,"LT_nhitpixelBarrelLayer2",&LT_nhitpixelBarrelLayer2_,"LT_nhitpixelBarrelLayer2_[n_Cpfcand_]/F");
  addBranch(tree,"LT_nhitpixelBarrelLayer3",&LT_nhitpixelBarrelLayer3_,"LT_nhitpixelBarrelLayer3_[n_Cpfcand_]/F");
  addBranch(tree,"LT_nhitpixelBarrelLayer4",&LT_nhitpixelBarrelLayer4_,"LT_nhitpixelBarrelLayer4_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_nhitpixelEndcapLayer1",&LT_nhitpixelEndcapLayer1_,"LT_nhitpixelEndcapLayer1_[n_Cpfcand_]/F");
  addBranch(tree,"LT_nhitpixelEndcapLayer2",&LT_nhitpixelEndcapLayer2_,"LT_nhitpixelEndcapLayer2_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_numberOfValidHits",&LT_numberOfValidHits_,"LT_numberOfValidHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidPixelHits",&LT_numberOfValidPixelHits_,"LT_numberOfValidPixelHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidStripHits",&LT_numberOfValidStripHits_,"LT_numberOfValidStripHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidStripTIBHits",&LT_numberOfValidStripTIBHits_,"LT_numberOfValidStripTIBHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidStripTIDHits",&LT_numberOfValidStripTIDHits_,"LT_numberOfValidStripTIDHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidStripTOBHits",&LT_numberOfValidStripTOBHits_,"LT_numberOfValidStripTOBHits_[n_Cpfcand_]/F");
  addBranch(tree,"LT_numberOfValidStripTECHits",&LT_numberOfValidStripTECHits_,"LT_numberOfValidStripTECHits_[n_Cpfcand_]/F");
  
  addBranch(tree,"LT_pdgID",&LT_pdgID_,"LT_pdgID_[n_Cpfcand_]/F");
  addBranch(tree,"LT_HadFrac",&LT_HadFrac_,"LT_HadFrac_[n_Cpfcand_]/F");
  addBranch(tree,"LT_CaloFrac",&LT_CaloFrac_,"LT_CaloFrac_[n_Cpfcand_]/F");
  
}

void ntuple_LT::readEvent(const edm::Event& iEvent){

    iEvent.getByToken(ltToken_, LTs);
    n_Npfcand_=0;
    n_Cpfcand_=0;

}

//use either of these functions
bool ntuple_LT::fillBranches(const pat::Jet & jet, const size_t& jetidx, const  edm::View<pat::Jet> * coll, float EventTime){

    math::XYZVector jetDir = jet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    const reco::Vertex & pv = vertices()->at(0);

    std::vector<sorting::sortingClass<size_t> > sortedcharged;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();
    const float jet_uncorr_e=jet.correctedJet("Uncorrected").energy();

    TrackInfoBuilder trackinfo(builder);
    int n_lts = 0;

    std::vector<reco::CandidatePtr> cpfPtrs;

    for (size_t i = 0; i < LTs->size(); ++i) {
      auto cand = LTs->ptrAt(i);
      if (reco::deltaR(*cand, jet) < jetradius_) {
	const auto *PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(&(*cand));
	if(PackedCandidate){
	  if(PackedCandidate->pt() < min_candidate_pt_) continue; 
	  trackinfo.buildTrackInfo(PackedCandidate,jetDir,jetRefTrackDir,pv);
	  sortedcharged.push_back(sorting::sortingClass<size_t>
				  (i, trackinfo.getTrackSip2dSig(),
				   -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
	  cpfPtrs.push_back(cand);
	  n_lts++;
	}
      }
    }

    std::sort(sortedcharged.begin(),sortedcharged.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_Cpfcand_ = std::min(sortedcharged.size(),max_pfcand_);

    std::vector<size_t> sortedchargedindices;
    sortedchargedindices=sorting::invertSortingVector(sortedcharged);

    for (unsigned int i = 0; i <  (unsigned int)n_Cpfcand_; i++){
      const auto *PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(&(*cpfPtrs.at(i)));
      //const auto& PackedCandidate_=s.get();
      if(!PackedCandidate_) continue;
      if(PackedCandidate_->pt() < min_candidate_pt_) continue; 

        // get the dr with the closest sv
      float drminpfcandsv_ = mindrsvpfcand(PackedCandidate_);

      float pdgid_;
      if (abs(PackedCandidate_->pdgId()) == 11 and PackedCandidate_->charge() != 0){
	pdgid_ = 0.0;
      }
      else if (abs(PackedCandidate_->pdgId()) == 13 and PackedCandidate_->charge() != 0){
	pdgid_ = 1.0;
      }
      else if (abs(PackedCandidate_->pdgId()) == 22 and PackedCandidate_->charge() == 0){
	pdgid_ = 2.0;
      }
      else if (abs(PackedCandidate_->pdgId()) != 22 and PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) != 1 and abs(PackedCandidate_->pdgId()) != 2){
	pdgid_ = 3.0;
      }
      else if (abs(PackedCandidate_->pdgId()) != 11 and abs(PackedCandidate_->pdgId()) != 13 and PackedCandidate_->charge() != 0){
	pdgid_ = 4.0;
      }
      else if (PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) == 1){
	pdgid_ = 5.0;
      }
      else if (PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) == 2){
	pdgid_ = 6.0;
      }
      else{
	pdgid_ = 7.0;
      }

      /// This might include more than PF candidates, e.g. Reco muons and could
      /// be double counting. Needs to be checked.!!!!
      ///
      /// Split to charged and neutral candidates
      if(PackedCandidate_->charge()!=0 ){

	size_t fillntupleentry= sortedchargedindices.at(i);
	if(fillntupleentry>=max_pfcand_) continue;

	LT_pt_[fillntupleentry] = PackedCandidate_->pt();
	LT_eta_[fillntupleentry] = PackedCandidate_->eta();
	LT_phi_[fillntupleentry] = PackedCandidate_->phi();
	LT_e_[fillntupleentry] = PackedCandidate_->energy();

	LT_puppiw_[fillntupleentry] = PackedCandidate_->puppiWeight();
	LT_dz_[fillntupleentry] = PackedCandidate_->dz();
	LT_VTX_ass_[fillntupleentry] = PackedCandidate_->pvAssociationQuality();

	float tempdontopt=PackedCandidate_->vx();
	tempdontopt++;

	LT_pdgID_[fillntupleentry] = pdgid_;
	LT_CaloFrac_[fillntupleentry] = PackedCandidate_->caloFraction();
	LT_HadFrac_[fillntupleentry] = PackedCandidate_->hcalFraction();


	trackinfo.buildTrackInfo(PackedCandidate_,jetDir,jetRefTrackDir,pv);

	const reco::TransientTrack ttrack = trackinfo.getTTrack();
	float mindistsvold = mindistsvpfcandold(ttrack);

	LT_distminsvold_[fillntupleentry] = mindistsvold;

	float mindistsv = mindistsvpfcand(ttrack);
	float eng_mindistsv = std::log(std::fabs(mindistsv)+1.0);

	LT_distminsv_[fillntupleentry] = mindistsv;
	LT_distminsv2_[fillntupleentry] = eng_mindistsv;

	LT_BtagPf_trackEtaRel_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackEtaRel(),  0,-5,15);
	LT_BtagPf_trackPtRel_[fillntupleentry]      =catchInfsAndBound(trackinfo.getTrackPtRel(),   0,-1,4);
	LT_BtagPf_trackPPar_[fillntupleentry]       =catchInfsAndBound(trackinfo.getTrackPPar(),    0,-1e5,1e5 );
	LT_BtagPf_trackDeltaR_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackDeltaR(),  0,-5,5 );
	LT_BtagPf_trackPParRatio_[fillntupleentry]  =catchInfsAndBound(trackinfo.getTrackPParRatio(),0,-10,100);
	LT_BtagPf_trackSip3dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dVal(), 0, -1,1e5 );
	LT_BtagPf_trackSip3dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dSig(), 0, -1,4e4 );
	LT_BtagPf_trackSip2dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dVal(), 0, -1,70 );
	LT_BtagPf_trackSip2dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dSig(), 0, -1,4e4 );
	LT_BtagPf_trackDecayLen_[fillntupleentry]   =trackinfo.getTrackJetDecayLen();
	LT_BtagPf_trackJetDistVal_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistVal(),0,-20,1 );

	float cand_charge_ = PackedCandidate_->charge();
	LT_charge_[fillntupleentry] = cand_charge_;

	LT_drminsv_[fillntupleentry] = catchInfsAndBound(drminpfcandsv_,0,-0.4,0,-0.4);

	//hit pattern variables, as defined here https://github.com/cms-sw/cmssw/blob/master/DataFormats/TrackReco/interface/HitPattern.h
	//get track associated to a jet constituent
	const reco::Track *track_ptr = nullptr;
	auto pf_candidate = dynamic_cast<const reco::PFCandidate *>(PackedCandidate_);
	auto packed_candidate = dynamic_cast<const pat::PackedCandidate *>(PackedCandidate_);
	if(pf_candidate){
	  track_ptr = pf_candidate->bestTrack(); //trackRef was sometimes null
	}else if(packed_candidate && packed_candidate->hasTrackDetails()){//if PackedCandidate does not have TrackDetails this gives an Exception because unpackCovariance might be called for pseudoTrack/bestTrack
	  track_ptr = &(packed_candidate->pseudoTrack());
	}
	//get hit pattern information
	if(track_ptr){
	  const reco::HitPattern &p = track_ptr->hitPattern();
	  //Tracker per layer
	  //Pixel barrel 
	  int Cpfcan_nhitpixelBarrelLayer1 = 0;
	  int Cpfcan_nhitpixelBarrelLayer2 = 0;
	  int Cpfcan_nhitpixelBarrelLayer3 = 0;
	  int Cpfcan_nhitpixelBarrelLayer4 = 0;
	  //Pixel Endcap 
	  int Cpfcan_nhitpixelEndcapLayer1 = 0;
	  int Cpfcan_nhitpixelEndcapLayer2 = 0;
	  // loop over the hits of the track.
	  for(int nh = 0; nh < p.numberOfAllHits(reco::HitPattern::TRACK_HITS); nh++){
	    uint32_t hit = p.getHitPattern(reco::HitPattern::TRACK_HITS, nh);
	    if(p.validHitFilter(hit)){// if the hit is valid
	      //Pixel Barrel // it is in pixel barrel
	      if(p.pixelBarrelHitFilter(hit)){
                if(p.getLayer(hit)==1) Cpfcan_nhitpixelBarrelLayer1 = Cpfcan_nhitpixelBarrelLayer1+1;
                if(p.getLayer(hit)==2) Cpfcan_nhitpixelBarrelLayer2 = Cpfcan_nhitpixelBarrelLayer2+1;
                if(p.getLayer(hit)==3) Cpfcan_nhitpixelBarrelLayer3 = Cpfcan_nhitpixelBarrelLayer3+1;
                if(p.getLayer(hit)==4) Cpfcan_nhitpixelBarrelLayer4 = Cpfcan_nhitpixelBarrelLayer4+1;
	      } 
	      //Pixel Endcap
	      if(p.pixelEndcapHitFilter(hit)){
                //std::cout << "valid hit found in pixel Endcap layer " << p.getLayer(hit) << std::endl;
                if(p.getLayer(hit)==1) Cpfcan_nhitpixelEndcapLayer1 = Cpfcan_nhitpixelEndcapLayer1+1;
                if(p.getLayer(hit)==2) Cpfcan_nhitpixelEndcapLayer2 = Cpfcan_nhitpixelEndcapLayer2+1;
	      } 
	    }
	  }
	  //Pixel Barrel 
	  LT_nhitpixelBarrelLayer1_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelBarrelHits()) ? catchInfsAndBound(Cpfcan_nhitpixelBarrelLayer1,-1,0,100,0) : -1;
	  LT_nhitpixelBarrelLayer2_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelBarrelHits()) ? catchInfsAndBound(Cpfcan_nhitpixelBarrelLayer2,-1,0,100,0) : -1;
	  LT_nhitpixelBarrelLayer3_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelBarrelHits()) ? catchInfsAndBound(Cpfcan_nhitpixelBarrelLayer3,-1,0,100,0) : -1;
	  LT_nhitpixelBarrelLayer4_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelBarrelHits()) ? catchInfsAndBound(Cpfcan_nhitpixelBarrelLayer4,-1,0,100,0) : -1;
             
	  LT_nhitpixelEndcapLayer1_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelEndcapHits()) ? catchInfsAndBound(Cpfcan_nhitpixelEndcapLayer1,-1,0,100,0) : -1;
	  LT_nhitpixelEndcapLayer2_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelEndcapHits()) ? catchInfsAndBound(Cpfcan_nhitpixelEndcapLayer2,-1,0,100,0) : -1;
	  //Tracker all layers together   
	  //Valid hits
	  LT_numberOfValidHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidHits(),-1,0,100,0) : -1;
	  LT_numberOfValidPixelHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidPixelHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidPixelHits(),-1,0,100,0) : -1;
	  LT_numberOfValidStripHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidStripHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidStripHits(),-1,0,100,0) : -1;
	  LT_numberOfValidStripTIBHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidStripTIBHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidStripTIBHits(),-1,0,100,0) : -1;
	  LT_numberOfValidStripTIDHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidStripTIDHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidStripTIDHits(),-1,0,100,0) : -1;
	  LT_numberOfValidStripTOBHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidStripTOBHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidStripTOBHits(),-1,0,100,0) : -1;
	  LT_numberOfValidStripTECHits_[fillntupleentry] = (track_ptr->hitPattern().numberOfValidStripTECHits()) ? catchInfsAndBound(track_ptr->hitPattern().numberOfValidStripTECHits(),-1,0,100,0) : -1;
	}
	else{
	  //Tracker per layer
	  //Pixel barrel 
	  LT_nhitpixelBarrelLayer1_[fillntupleentry] = -1;
	  LT_nhitpixelBarrelLayer2_[fillntupleentry] = -1;
	  LT_nhitpixelBarrelLayer3_[fillntupleentry] = -1;
	  LT_nhitpixelBarrelLayer4_[fillntupleentry] = -1;
	  //Pixel Endcap 
	  LT_nhitpixelEndcapLayer1_[fillntupleentry] = -1;
	  LT_nhitpixelEndcapLayer2_[fillntupleentry] = -1;

	  LT_numberOfValidHits_[fillntupleentry] = -1; 
	  LT_numberOfValidPixelHits_[fillntupleentry] = -1; 
	  LT_numberOfValidStripHits_[fillntupleentry] = -1; 
	  LT_numberOfValidStripTIBHits_[fillntupleentry] = -1; 
	  LT_numberOfValidStripTIDHits_[fillntupleentry] = -1; 
	  LT_numberOfValidStripTOBHits_[fillntupleentry] = -1; 
	  LT_numberOfValidStripTECHits_[fillntupleentry] = -1; 
	}
      }
    }

    n_LT_ = n_lts;

    return true;
}


float ntuple_LT::mindrsvpfcand(const pat::PackedCandidate* pfcand) {

  float mindr_ = jetradius_;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if(!pfcand) continue;
    //if(!svs.at(i)) continue;
    float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
    if (tempdr_<mindr_) { mindr_ = tempdr_; }

  }
  return mindr_;
}

float ntuple_LT::mindistsvpfcandold(const reco::TransientTrack track) {

  float mindist_ = 999.999;
  float out_dist = 0.0;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if (!track.isValid()) {continue;}
    reco::Vertex::CovarianceMatrix csv; secVertices()->at(i).fillVertexCovariance(csv);
    reco::Vertex vertex(secVertices()->at(i).vertex(), csv);
    if (!vertex.isValid()) {continue;}

    GlobalVector direction(secVertices()->at(i).px(),secVertices()->at(i).py(),secVertices()->at(i).pz());


    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
    
    VertexDistance3D dist;

    if (!tsos.isValid()) {continue;}
    GlobalPoint refPoint = tsos.globalPosition();
    GlobalError refPointErr = tsos.cartesianError().position();
    GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
    GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

    std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
    if (!result.first) {continue;}

    GlobalPoint impactPoint = tsos.globalPosition();
    GlobalVector IPVec(impactPoint.x() - vertex.x(), impactPoint.y() - vertex.y(), impactPoint.z() - vertex.z());
    double prod = IPVec.dot(direction);
    double sign = (prod >= 0) ? 1. : -1.;

    if(result.second.value() < mindist_){ 
      out_dist = sign * result.second.value();
      mindist_ = result.second.value();
    } 
  }
  return out_dist;
}

GlobalPoint ntuple_LT::mingpsvpfcand(const reco::TransientTrack track) {

  float mindist_ = 999.999;
  GlobalPoint out_dist(0.0,0.0,0.0);
  GlobalPoint pca;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if (!track.isValid()) {continue;}

    reco::Vertex::CovarianceMatrix csv; secVertices()->at(i).fillVertexCovariance(csv);
    reco::Vertex vertex(secVertices()->at(i).vertex(), csv);
    if (!vertex.isValid()) {continue;}

    GlobalVector direction(secVertices()->at(i).px(),secVertices()->at(i).py(),secVertices()->at(i).pz());

    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
    
    VertexDistance3D dist;

    if (!tsos.isValid()) {continue;}
    GlobalPoint refPoint = tsos.globalPosition();
    GlobalError refPointErr = tsos.cartesianError().position();
    GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
    GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

    std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
    if (!result.first) {continue;}

    pca = tsos.globalPosition();

    if(result.second.value() < mindist_){ 
      out_dist = pca;
      mindist_ = result.second.value();
    }
  }
  return out_dist;
}

GlobalPoint ntuple_LT::gppvpfcand(const reco::TransientTrack track, const GlobalVector direction, const reco::Vertex vertex) {

  float mindist_ = 999.999;
  float dist = 0.;
  GlobalPoint out_dist(0.0,0.0,0.0);
  GlobalPoint pca;
  if ((track.isValid()) && (vertex.isValid())){
      
    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
      
    VertexDistance3D dist;

    if (tsos.isValid()) {
      GlobalPoint refPoint = tsos.globalPosition();
      GlobalError refPointErr = tsos.cartesianError().position();
      GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
      GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

      std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
      if (result.first) {
	pca = tsos.globalPosition();
      }
    }
  }
  out_dist = pca;
  return out_dist;
}

float ntuple_LT::mindistsvpfcand(const reco::TransientTrack track) {

  float mindist_ = 999.999;
  float out_dist = 0.0;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if (!track.isValid()) {continue;}
    reco::Vertex::CovarianceMatrix csv; secVertices()->at(i).fillVertexCovariance(csv);
    reco::Vertex vertex(secVertices()->at(i).vertex(), csv);
    if (!vertex.isValid()) {continue;}

    GlobalVector direction(secVertices()->at(i).px(),secVertices()->at(i).py(),secVertices()->at(i).pz());


    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
    
    VertexDistance3D dist;

    if (!tsos.isValid()) {continue;}
    GlobalPoint refPoint = tsos.globalPosition();
    GlobalError refPointErr = tsos.cartesianError().position();
    GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
    GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

    std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
    if (!result.first) {continue;}

    GlobalPoint impactPoint = tsos.globalPosition();
    GlobalVector IPVec(impactPoint.x() - vertex.x(), impactPoint.y() - vertex.y(), impactPoint.z() - vertex.z());
    double prod = IPVec.dot(direction);
    double sign = (prod >= 0) ? 1. : -1.;

    if(result.second.value() < mindist_){
      float inv_dist = 1.0 / (sign * result.second.value() + 0.001);
      out_dist = inv_dist;
      mindist_ = result.second.value();
    } 
  }
  return out_dist;
}
