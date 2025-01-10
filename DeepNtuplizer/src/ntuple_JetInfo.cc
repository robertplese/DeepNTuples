/*
 * ntuple_JetInfo.cc
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */

#include "../interface/ntuple_JetInfo.h"
#include "../interface/helpers.h"
#include <vector>
#include <algorithm>
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
using namespace std;

void ntuple_JetInfo::getInput(const edm::ParameterSet& iConfig){

    gluonReduction_=(iConfig.getParameter<double>("gluonReduction"));
    jetPtMin_=(iConfig.getParameter<double>("jetPtMin"));
    jetPtMax_=(iConfig.getParameter<double>("jetPtMax"));
    jetAbsEtaMin_=(iConfig.getParameter<double>("jetAbsEtaMin"));
    jetAbsEtaMax_=(iConfig.getParameter<double>("jetAbsEtaMax"));
    
    vector<string> disc_names = iConfig.getParameter<vector<string> >("bDiscriminators");
    for(auto& name : disc_names) {
        discriminators_[name] = 0.;
    }
}
void ntuple_JetInfo::initBranches(TTree* tree){

    //more general event info, here applied per jet
    addBranch(tree,"npv"    ,&npv_    ,"npv/I"    );
    addBranch(tree,"rho", &rho_, "rho/F");
    addBranch(tree,"ntrueInt",&ntrueInt_,"ntrueInt/F");
    addBranch(tree,"event_no"    ,&event_no_    ,"event_no/I"    );
    addBranch(tree,"jet_no"    ,&jet_no_    ,"jet_no/I"    );

    // truth label
    addBranch(tree,"npv_0_z" ,&npv_0_z_ ,"npv_0_z/f" );
    addBranch(tree, "PUrho", &PU_rho_, "PU_rho/f");
    addBranch(tree,"gen_pt"    ,&gen_pt_    ,"gen_pt_/F"    );
    addBranch(tree,"Delta_gen_pt"    ,&Delta_gen_pt_,"Delta_gen_pt_/F"    );
    addBranch(tree,"isB",&isB_, "isB_/I");
    addBranch(tree,"isGBB",&isGBB_, "isGBB_/I");
    addBranch(tree,"isBB",&isBB_, "isBB_/I");
    addBranch(tree,"isLeptonicB",&isLeptonicB_, "isLeptonicB_/I");
    addBranch(tree,"isLeptonicB_C",&isLeptonicB_C_, "isLeptonicB_C_/I");
    addBranch(tree,"isC",&isC_, "isC_/I");
    addBranch(tree,"isGCC",&isGCC_, "isGCC_/I");
    addBranch(tree,"isCC",&isCC_, "isCC_/I");
    addBranch(tree,"isUD",&isUD_, "isUD_/I");
    addBranch(tree,"isS",&isS_, "isS_/I");
    addBranch(tree,"isG",&isG_, "isG_/I");
    addBranch(tree,"isMU",&isMU_, "isMU_/I");
    addBranch(tree,"isELE",&isELE_, "isELE_/I");
    addBranch(tree,"isTau",&isTau_, "isTau_/I");
    addBranch(tree,"isPU",&isPU_, "isPU_/I");
    addBranch(tree,"isUndefined",&isUndefined_, "isUndefined_/I");
    addBranch(tree,"genDecay",&genDecay_, "genDecay_/F"); //dxy corresponds to the distance the Bhadron traveled
    
    addBranch(tree,"jet_hflav", &jet_hflav_);
    addBranch(tree,"jet_pflav", &jet_pflav_);
    addBranch(tree,"jet_phflav", &jet_phflav_);

    // jet variables
    addBranch(tree,"jet_pt", &jet_pt_);
    addBranch(tree,"jet_corr_pt", &jet_corr_pt_);
    addBranch(tree,"jet_eta", &jet_eta_);
    addBranch(tree,"jet_phi", &jet_phi_);
    addBranch(tree,"jet_mass", &jet_mass_);
    addBranch(tree,"jet_energy", &jet_energy_);

    //jet id
    addBranch(tree,"jet_looseId", &jet_looseId_);
    addBranch(tree,"jet_jetId", &jet_jetId_);
    addBranch(tree,"jet_puId", &jet_puId_); 

    // quark gluon
    addBranch(tree,"jet_qgl",   &jet_qgl_);  // qg tagger from jmar
    addBranch(tree,"QG_ptD",   &QG_ptD_);   // momentum fraction per jet constituent
    addBranch(tree,"QG_axis2", &QG_axis2_); // jet shape i.e. gluon are wider than quarks
    addBranch(tree,"QG_mult",  &QG_mult_);  // multiplicity i.e. total num of PFcands reconstructed

    addBranch(tree,"gen_pt_Recluster"    ,&gen_pt_Recluster_    ,"gen_pt_Recluster_/F"    );
    addBranch(tree,"gen_pt_WithNu"    ,&gen_pt_WithNu_    ,"gen_pt_WithNu_/F"    );
    addBranch(tree,"Delta_gen_pt_Recluster"    ,&Delta_gen_pt_Recluster_    ,"Delta_gen_pt_Recluster_/F"    );
    addBranch(tree,"Delta_gen_pt_WithNu"    ,&Delta_gen_pt_WithNu_    ,"Delta_gen_pt_WithNu_/F"    );

    addBranch(tree, "Event_time", &event_time_, "Event_time/f");
    addBranch(tree, "event_timeNtk", &event_timeNtk, "event_timeNtk/f");
    addBranch(tree, "Jet_time", &jet_time_, "Jet_time/f");
    addBranch(tree, "Jet_timeError", &jet_timeError_, "Jet_timeError/f");
    addBranch(tree, "Jet_timeNtk", &jet_timeNtk_, "Jet_timeNtk/f");
    addBranch(tree, "Jet_timesig", &jet_timesig_, "Jet_timesig/f");
    addBranch(tree, "genvtx_z", &genvtx_z_, "genvtx_z/f");
    addBranch(tree, "genvtx_time", &genvtx_time_, "genvtx_time_/f");
    addBranch(tree,"pv_num", &pv_num_,"pv_num_/I");
    addBranch(tree,"pu_z", &pu_z_,"pu_z_[50]/F");
    addBranch(tree,"pu_time", &pu_time_,"pu_time_[50]/F");
    addBranch(tree,"pu_pthats", &pu_pthats_,"pu_pthats_[50]/F");
    if(1) // discriminators might need to be filled differently. FIXME
        for(auto& entry : discriminators_) {
            string better_name(entry.first);
            std::replace(better_name.begin(), better_name.end(), ':', '_');
            addBranch(tree,better_name.c_str(), &entry.second, (better_name+"/F").c_str());
        }
}

void ntuple_JetInfo::readEvent(const edm::Event& iEvent){
    iEvent.getByToken(qglToken_, qglHandle);
    iEvent.getByToken(ptDToken_, ptDHandle);
    iEvent.getByToken(axis2Token_, axis2Handle);
    iEvent.getByToken(multToken_, multHandle);

    iEvent.getByToken(genJetMatchReclusterToken_, genJetMatchRecluster);
    iEvent.getByToken(genJetMatchWithNuToken_, genJetMatchWithNu);
    iEvent.getByToken(genJetMatchAllowDuplicatesToken_, genJetMatchAllowDuplicates);
    iEvent.getByToken(genParticlesToken_, genParticlesHandle);


    iEvent.getByToken(muonsToken_, muonsHandle);
    iEvent.getByToken(electronsToken_, electronsHandle);
    iEvent.getByToken(puInfoToken_, PUInfo);
    
    const auto& genvtx_zpos = iEvent.get(genVtxPositionToken_).z();
    const auto& genvtx_time = iEvent.get(genVtxTimeToken_);
    genvtx_t = genvtx_time;
    genvtx_z = genvtx_zpos;
    event_no_=iEvent.id().event();

    //presumably this whole part can be removed!

    neutrinosLepB.clear();
    neutrinosLepB_C.clear();
    gToBB.clear();
    gToCC.clear();
    alltaus_.clear();
    Bhadron_.clear();
    Bhadron_daughter_.clear();

    gen_particle_pt.clear();
    gen_particle_eta.clear();
    gen_particle_phi.clear();
    gen_particle_mass.clear();
    gen_particle_id.clear();
    gen_particle_status.clear();
    gen_particle_daughters_id.clear();
    gen_particle_daughters_igen.clear();
    gen_particle_daughters_status.clear();
    gen_particle_daughters_pt.clear();
    gen_particle_daughters_eta.clear();
    gen_particle_daughters_phi.clear();
    gen_particle_daughters_mass.clear();
    gen_particle_daughters_charge.clear();
    genLepFromResonance4V_.clear();
    genMuonsFromResonance4V_.clear();
    genElectronsFromResonance4V_.clear();
    tau_gen_visible_.clear();
    tau_gen_.clear();
    
 for (const reco::Candidate &genC : *genParticlesHandle)
   {
     const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
     
     if((abs(gen.pdgId())>500&&abs(gen.pdgId())<600)||(abs(gen.pdgId())>5000&&abs(gen.pdgId())<6000)) {

       Bhadron_.push_back(gen);
       if(gen.numberOfDaughters()>0){
     
	 if( (abs(gen.daughter(0)->pdgId())>500&&abs(gen.daughter(0)->pdgId())<600)||(abs(gen.daughter(0)->pdgId())>5000&&abs(gen.daughter(0)->pdgId())<6000))
	   {
	     if(gen.daughter(0)->numberOfDaughters()>0)
	       {
		
		 const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*(gen.daughter(0)->daughter(0)));
		 
		 if(daughter_.vx()!=gen.vx())
		   { 
		     Bhadron_daughter_.push_back(daughter_);
		   }
                 else Bhadron_daughter_.push_back(gen);
	       }
	     else  Bhadron_daughter_.push_back(gen);
	     
	   }
	 else{
	   const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*gen.daughter(0));
	   Bhadron_daughter_.push_back(daughter_);
	 }

       }// if daughter is there
       else {
	 Bhadron_daughter_.push_back(gen);
       }
     }
   }

 for (const reco::Candidate &genC : *genParticlesHandle) {
        const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
        if(abs(gen.pdgId())==12||abs(gen.pdgId())==14||abs(gen.pdgId())==16) {
            const reco::GenParticle* mother =  static_cast< const reco::GenParticle*> (gen.mother());
            if(mother!=NULL) {
                if((abs(mother->pdgId())>500&&abs(mother->pdgId())<600)||(abs(mother->pdgId())>5000&&abs(mother->pdgId())<6000)) {
                    neutrinosLepB.emplace_back(gen);
                }
                if((abs(mother->pdgId())>400&&abs(mother->pdgId())<500)||(abs(mother->pdgId())>4000&&abs(mother->pdgId())<5000)) {
                    neutrinosLepB_C.emplace_back(gen);
                }
            }
            else {
                std::cout << "No mother" << std::endl;
            }
        }

        int id(std::abs(gen.pdgId())); 
        int status(gen.status());

        if (id == 21 && status >= 21 && status <= 59) { //// Pythia8 hard scatter, ISR, or FSR
            if ( gen.numberOfDaughters() == 2 ) {
                const reco::Candidate* d0 = gen.daughter(0);
                const reco::Candidate* d1 = gen.daughter(1);
                if ( std::abs(d0->pdgId()) == 5 && std::abs(d1->pdgId()) == 5
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToBB.push_back(gen) ;
                if ( std::abs(d0->pdgId()) == 4 && std::abs(d1->pdgId()) == 4
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToCC.push_back(gen) ;
            }
        }

        if(id == 15 && false){
            alltaus_.push_back(gen);
        }

 }
 // GEN particle information
 if(genParticlesHandle.isValid()){
   unsigned int igen = 0;
   for (auto gens_iter = genParticlesHandle->begin(); gens_iter != genParticlesHandle->end(); ++gens_iter) { 
     if((abs(gens_iter->pdgId()) == 25 or abs(gens_iter->pdgId()) == 24 or abs(gens_iter->pdgId()) == 23) and
	gens_iter->isLastCopy() and gens_iter->statusFlags().fromHardProcess()){ 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }

     // Final states Leptons (e,mu) and Neutrinos --> exclude taus. They need to be prompt or from Tau decay      
     if (abs(gens_iter->pdgId()) > 10 and abs(gens_iter->pdgId()) < 17 and abs(gens_iter->pdgId()) != 15  and 
	 (gens_iter->isPromptFinalState() or gens_iter->isDirectPromptTauDecayProductFinalState())) { 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       // No need to save daughters here
       igen++;
     }

     // Final state quarks or gluons from the hard process before the shower --> partons in which H/Z/W/top decay into
     if (((abs(gens_iter->pdgId()) >= 1 and abs(gens_iter->pdgId()) <= 5) or abs(gens_iter->pdgId()) == 21) and 
	 gens_iter->statusFlags().fromHardProcess() and gens_iter->statusFlags().isFirstCopy()){
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       igen++;
       // no need to save daughters
     }

     // Special case of taus: last-copy, from hard process and, prompt and decayed
     if(abs(gens_iter->pdgId()) == 15 and gens_iter->isLastCopy() and
	gens_iter->statusFlags().fromHardProcess() and gens_iter->isPromptDecayed()){ 
            
       // hadronic taus
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       // only store the final decay particles
       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 if(not dynamic_cast<const reco::GenParticle*>(gens_iter->daughter(idau))->statusFlags().isPromptTauDecayProduct()) continue;
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());    
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }  
   }
 }

}

//use either of these functions

bool ntuple_JetInfo::fillBranches(const pat::Jet & jet, const size_t& jetidx, const edm::View<pat::Jet> * coll, float EventTime){
    if(!coll)
        throw std::runtime_error("ntuple_JetInfo::fillBranches: no jet collection");

    /// thresholds for matching
    static float dRCone        = 0.2;
    static float dRMatchingPF  = 0.1;
    static float ptGenLeptonMin = 8;
    static float ptGenTauVisibleMin = 15;
    genvtx_z_ = genvtx_z;
    genvtx_time_ = genvtx_t;
    // Gen leptons from resonance decay 
    std::vector<TLorentzVector> genLepFromResonance4V;
    std::vector<TLorentzVector> genMuonsFromResonance4V;
    std::vector<TLorentzVector> genElectronsFromResonance4V;
    std::vector<int> genMuonsFromResonanceCharge;
    std::vector<int> genElectronsFromResonanceCharge;
    std::vector<TLorentzVector> tau_gen_visible;
    std::vector<TLorentzVector> tau_gen;
    std::vector<int> tau_gen_charge;
    std::vector<unsigned int> tau_gen_nch;
    std::vector<unsigned int> tau_gen_np0;
    std::vector<unsigned int> tau_gen_nnh;
    
    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      // select resonances like Higgs, W, Z, taus
      if(abs(gen_particle_id.at(igen)) == 25 or
	 abs(gen_particle_id.at(igen)) == 23 or
	 abs(gen_particle_id.at(igen)) == 24 or
	 abs(gen_particle_id.at(igen)) == 15){
	for(size_t idau = 0; idau < gen_particle_daughters_id.size(); idau++){
	  // select electrons or muons from the resonance / tau decay
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     (abs(gen_particle_daughters_id.at(idau)) == 11 or
	      abs(gen_particle_daughters_id.at(idau)) == 13)){
	    TLorentzVector gen4V;
	    gen4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    if(std::find(genLepFromResonance4V.begin(),genLepFromResonance4V.end(),gen4V) == genLepFromResonance4V.end())
	      genLepFromResonance4V.push_back(gen4V);
	    if(abs(gen_particle_daughters_id.at(idau)) == 13 and 
	       std::find(genMuonsFromResonance4V.begin(),genMuonsFromResonance4V.end(),gen4V) == genMuonsFromResonance4V.end()){
	      genMuonsFromResonance4V.push_back(gen4V);
	      genMuonsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	    if(abs(gen_particle_daughters_id.at(idau)) == 11 and 
	       std::find(genElectronsFromResonance4V.begin(),genElectronsFromResonance4V.end(),gen4V) == genElectronsFromResonance4V.end()){
	      genElectronsFromResonance4V.push_back(gen4V);
	      genElectronsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	  }
	}
      }
    }

    // Gen hadronic taus
    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      if(abs(gen_particle_id.at(igen)) == 15){ // hadronic or leptonic tau
	TLorentzVector tau_gen_tmp;
	unsigned int tau_gen_nch_tmp = 0;
	unsigned int tau_gen_np0_tmp = 0;
	unsigned int tau_gen_nnh_tmp = 0;
	for(size_t idau = 0; idau < gen_particle_daughters_pt.size(); idau++){
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     abs(gen_particle_daughters_id.at(idau)) != 11 and // no mu
	     abs(gen_particle_daughters_id.at(idau)) != 13 and // no el
	     abs(gen_particle_daughters_id.at(idau)) != 12 and // no neutrinos
	     abs(gen_particle_daughters_id.at(idau)) != 14 and
	     abs(gen_particle_daughters_id.at(idau)) != 16){
	    TLorentzVector tmp4V; 
	    tmp4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    tau_gen_tmp += tmp4V;
	    if (gen_particle_daughters_charge.at(idau) != 0 and gen_particle_daughters_status.at(idau) == 1) tau_gen_nch_tmp ++; // charged particles
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) == 111) tau_gen_np0_tmp++;
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) != 111) tau_gen_nnh_tmp++;
	  }
	}
	if(tau_gen_tmp.Pt() > 0){ // good hadronic tau
	  tau_gen_visible.push_back(tau_gen_tmp);
	  tau_gen_tmp.SetPtEtaPhiM(gen_particle_pt.at(igen),gen_particle_eta.at(igen),gen_particle_phi.at(igen),gen_particle_mass.at(igen));
	  tau_gen_charge.push_back((gen_particle_id.at(igen) > 0) ? -1 : 1);
	  tau_gen.push_back(tau_gen_tmp);
	  tau_gen_nch.push_back(tau_gen_nch_tmp);
	  tau_gen_np0.push_back(tau_gen_np0_tmp);
	  tau_gen_nnh.push_back(tau_gen_nnh_tmp);
	}
      }
    }

    // matching with gen-leptons (muons/electrons/hadronic taus)
    float minDR = 1000;
    int nlep_in_cone  = 0;
    int pos_matched_genmu = -1;
    int pos_matched_genele = -1;
    int pos_matched_tauh = -1;
    //int gentau_decaymode = -1;   
    TLorentzVector genLepton4V;
    TLorentzVector genLeptonVis4V;

    TLorentzVector jet4V;
    jet4V.SetPtEtaPhiM(jet.pt(),jet.eta(),jet.phi(),jet.mass());

    for(size_t igen = 0; igen < genMuonsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genMuonsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genMuonsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu = igen;
	minDR = dR;
	genLepton4V = genMuonsFromResonance4V.at(igen);
	genLeptonVis4V = genMuonsFromResonance4V.at(igen);
      }
    }
    
    for(size_t igen = 0; igen < genElectronsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genElectronsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genElectronsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = igen;
	minDR = dR;
	genLepton4V = genElectronsFromResonance4V.at(igen);
	genLeptonVis4V = genElectronsFromResonance4V.at(igen);
      }
    }
    
    for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){      
      float dR = tau_gen_visible.at(itau).DeltaR(jet4V); 
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and tau_gen_visible.at(itau).Pt() >= ptGenTauVisibleMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = -1;
	pos_matched_tauh = itau;
	minDR = dR;
	//gentau_decaymode = 5*(tau_gen_nch.at(itau)-1)+tau_gen_np0.at(itau);
	genLepton4V = tau_gen.at(itau);
	genLeptonVis4V = tau_gen_visible.at(itau);
      }
    }

    /// cuts ///
    bool returnval=true;
    
    // some cuts to contrin training region
    if ( jet.pt() < jetPtMin_ ||  jet.pt() > jetPtMax_ ) returnval=false;                  // apply jet pT cut
    if ( fabs(jet.eta()) < jetAbsEtaMin_ || fabs(jet.eta()) > jetAbsEtaMax_ ) returnval=false; // apply jet eta cut


    // often we have way to many gluons that we do not need. This randomply reduces the gluons
    if (gluonReduction_>0 && jet.partonFlavour()==21)
        if(TRandom_.Uniform()>gluonReduction_) returnval=false;

    //branch fills
    for(auto& entry : discriminators_) {
        entry.second = catchInfs(jet.bDiscriminator(entry.first),-0.1);
    }

    npv_ = vertices()->size();
    npv_0_z_ = vertices()->at(0).z();
    
    float PUrho = 0;
    std::vector<PileupSummaryInfo>::const_iterator ipu;
    
    for(ipu = PUInfo->begin(); ipu != PUInfo->end(); ++ipu) {
	    if ( ipu->getBunchCrossing() != 0 ) continue; // storing detailed PU info only for BX=0
    //  pu_z_ = ipu->getPU_zpositions();
    //  pu_time_ = ipu->getPU_times();
      const auto& zpositions =ipu->getPU_zpositions();
      const auto& times = ipu->getPU_times();
      const auto& pthats = ipu->getPU_pT_hats();
       std::vector<std::tuple<float, float, float>> pu_z_time_pthat_tuples;

        pv_num_ = 0;
        for (unsigned int i = 0; i < zpositions.size(); ++i) {
            pu_z_time_pthat_tuples.emplace_back(zpositions[i], times[i], pthats[i]);
        }
        // Sort the tuples based on pu_pthats
      std::sort(pu_z_time_pthat_tuples.begin(), pu_z_time_pthat_tuples.end(),
          [](const std::tuple<float, float, float>& a, const std::tuple<float, float, float>& b) {
              return std::get<2>(a) > std::get<2>(b);
          });
	     for (const auto& tuple : pu_z_time_pthat_tuples) {
        if (pv_num_ < 50) {
            pu_z_[pv_num_] = std::get<0>(tuple);
            pu_time_[pv_num_] = std::get<1>(tuple);
            pu_pthats_[pv_num_] = std::get<2>(tuple);
            pv_num_++;
        }
    }

    }	
    
	PUrho /= 20.;

	 // std::cout << "====================" << std::endl; 
	 // std::cout << "PUrho: " << PUrho << std::endl;
	 // std::cout << "====================" << std::endl; 

	PU_rho_ = PUrho;
    for (auto const& v : *pupInfo()) {
        int bx = v.getBunchCrossing();
        if (bx == 0) {
            ntrueInt_ = v.getTrueNumInteractions();
        }
    }
    rho_ = rhoInfo()[0];


    jet_no_=jetidx;

    const auto jetRef = reco::CandidatePtr(coll->ptrs().at( jetidx));

    jet_qgl_ = (*qglHandle)[jetRef];
    QG_ptD_ = (*ptDHandle)[jetRef];
    QG_axis2_ = (*axis2Handle)[jetRef];
    QG_mult_ = (*multHandle)[jetRef];

    isB_=0; isGBB_=0; isBB_=0; isC_=0; isGCC_=0; isCC_=0; isUD_=0;isTau_=0;isMU_=0;isELE_=0;
    isS_=0; isG_=0, isPU_=0, isLeptonicB_=0, isLeptonicB_C_=0, isUndefined_=0;
    auto muIds = deep_ntuples::jet_muonsIds(jet,*muonsHandle);
    auto elecIds = deep_ntuples::jet_electronsIds(jet,*electronsHandle);

    muons_number_ = muIds.size();
    electrons_number_ = elecIds.size();

    float etasign = 1.;
    if (jet.eta()<0) etasign = -1.;

    for(std::size_t i=0; i<max_num_lept; i++) {
        if (i < muIds.size()) {
            const auto & muon = (*muonsHandle).at(muIds.at(i));
            muons_isLooseMuon_[i] = muon.isLooseMuon();
            muons_isTightMuon_[i] = muon.isTightMuon(vertices()->at(0));
            muons_isSoftMuon_[i] = muon.isSoftMuon(vertices()->at(0));
            muons_isHighPtMuon_[i] = muon.isHighPtMuon(vertices()->at(0));
            muons_pt_[i] = muon.pt();
            muons_relEta_[i] = etasign*(muon.eta()-jet.eta());
            muons_relPhi_[i] = reco::deltaPhi(muon.phi(),jet.phi());
            muons_energy_[i] = muon.energy()/jet.energy();
        }
        if (i < elecIds.size()) {
            const auto & electron = (*electronsHandle).at(elecIds.at(i));
            electrons_pt_[i] = electron.pt();
            electrons_relEta_[i] = etasign*(electron.eta()-jet.eta());
            electrons_relPhi_[i] = reco::deltaPhi(electron.phi(),jet.phi());
            electrons_energy_[i] = electron.energy()/jet.energy();
        }
    }

    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    if(true){
      switch(deep_ntuples::jet_flavour(jet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh)) {
        case deep_ntuples::JetFlavor::MU:  isMU_=1; break;
        case deep_ntuples::JetFlavor::ELE:  isELE_=1; break;
        case deep_ntuples::JetFlavor::TAU:  isTau_=1; break;
        case deep_ntuples::JetFlavor::B:  isB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isLeptonicB_C_=1; break;
        case deep_ntuples::JetFlavor::GBB: isGBB_=1; break;
        case deep_ntuples::JetFlavor::BB: isBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isGCC_=1; break;
        case deep_ntuples::JetFlavor::CC: isCC_=1; break;
        case deep_ntuples::JetFlavor::G:  isG_=1; break;
        case deep_ntuples::JetFlavor::PU:  isPU_=1; break;
        case deep_ntuples::JetFlavor::UD: isUD_=1; break;
        case deep_ntuples::JetFlavor::S:  isS_=1; break;
        default : isUndefined_=1; break;
        }
    }

    //truth labeling with fallback to physics definition for light/gluon/undefined of standard flavor definition
    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    isPhysB_=0; isPhysBB_=0; isPhysGBB_=0; isPhysC_=0; isPhysCC_=0;
    isPhysGCC_=0; isPhysUD_=0; isPhysS_=0; isPhysG_=0, isPhysLeptonicB_=0, isPhysLeptonicB_C_=0, isPhysUndefined_=0;
    isPhysTau_=0, isPhysPU_=0;
    if(true){
      switch(deep_ntuples::jet_flavour(jet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh, true)) {
        case deep_ntuples::JetFlavor::UD: isPhysUD_=1; break;
        case deep_ntuples::JetFlavor::S:  isPhysS_=1; break;
        case deep_ntuples::JetFlavor::B:  isPhysB_=1; break;
        case deep_ntuples::JetFlavor::BB: isPhysBB_=1; break;
        case deep_ntuples::JetFlavor::GBB: isPhysGBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isPhysC_=1; break;
        case deep_ntuples::JetFlavor::CC: isPhysCC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isPhysGCC_=1; break;
        case deep_ntuples::JetFlavor::TAU: isPhysTau_=1;break;
        case deep_ntuples::JetFlavor::G:  isPhysG_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isPhysLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isPhysLeptonicB_C_=1; break;
	case deep_ntuples::JetFlavor::PU: isPhysPU_=1; break;
        default : isPhysUndefined_=1; break;
        }
    }

    if(isUndefined_) returnval=false;
    pat::JetCollection h;

    jet_pt_ = jet.correctedJet("Uncorrected").pt();
    jet_eta_ = jet.eta();
    jet_phi_ = jet.phi();
    jet_corr_pt_ = jet.pt();
    jet_mass_ = jet.mass();
    jet_energy_ = jet.energy();

    float jet_time        = 0;
    float jet_timeError = 0;
    float jet_timeNtk    = 0;
    float jet_timesig    = 0;
    for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++) {
      const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
      if ( !PackedCandidate ) continue;
      if ( PackedCandidate->charge() == 0 ) continue;
        auto track = PackedCandidate->bestTrack();
      if ( !track ) continue;
        float cand_time = PackedCandidate->time();
        float cand_timeError = PackedCandidate->timeError();
        if ( cand_timeError > 0. && abs(cand_time) < 1 ) {
          jet_timeNtk += 1;
          jet_timeError+=cand_timeError*cand_timeError;
          jet_time += cand_time ;
        }
    }
    if ( jet_timeNtk > 0 ) {
        jet_time = jet_time / jet_timeNtk;
        jet_timeError = sqrt(jet_timeError) / jet_timeNtk;
    }
    else {
    jet_time = -1;
    jet_timeError= -1;
}
    if ( jet_timeError > 0 ) {
        jet_timesig = jet_time / jet_timeError;
    }
    else jet_timesig = -1000;

    jet_time_=jet_time;
    jet_timeNtk_=jet_timeNtk;
    jet_timeError_=jet_timeError;
    jet_timesig_=jet_timesig;

    genDecay_ = -1.;

    try {
        reco::GenParticleRefVector Bhadrons_in_jet = jet.jetFlavourInfo().getbHadrons();

        if (Bhadrons_in_jet.size() > 0){ 

            for (unsigned int idx=0; idx<Bhadron_.size(); ++idx){

                reco::GenParticle bhad = Bhadron_[idx];

                bool bhad_is_in_jet = false;

                for (reco::GenParticleRefVector::const_iterator bhad_in_jet = Bhadrons_in_jet.begin(); bhad_in_jet!=Bhadrons_in_jet.end(); ++bhad_in_jet) {

                    //check if bhad is identical to bhad_in_jet
                    if ( (*bhad_in_jet)->pt() == bhad.pt() && (*bhad_in_jet)->eta() == bhad.eta()
                            && (*bhad_in_jet)->phi() == bhad.phi() && (*bhad_in_jet)->pdgId() == bhad.pdgId())              
                        bhad_is_in_jet = true;
                }
                if (bhad_is_in_jet){

                    if (Bhadron_daughter_[idx].vx()!=bhad.vx()){

                        float vx = Bhadron_daughter_[idx].vx() - bhad.vx();
                        float vy = Bhadron_daughter_[idx].vy() - bhad.vy();

                        float dxy = sqrt(vx*vx+vy*vy);
                        if (dxy > genDecay_)
                            genDecay_= dxy;
                    }
                    else if (genDecay_ < 0) 
                        genDecay_ = -0.1;
                }
            }
        }
    }
    catch (const cms::Exception &e){
        genDecay_ = -1.;
    }

    try{
        float NHF  = jet.neutralHadronEnergyFraction();
        float NEMF = jet.neutralEmEnergyFraction();
        float CHF  = jet.chargedHadronEnergyFraction();
        float MUF  = jet.muonEnergyFraction();
        float CEMF = jet.chargedEmEnergyFraction();
        float NumConst = jet.chargedMultiplicity()+jet.neutralMultiplicity();
        float NumNeutralParticles =jet.neutralMultiplicity();
        float CHM      = jet.chargedMultiplicity();

        jet_looseId_ = ((NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7) ||
                (NHF<0.98 && NEMF>0.01 && NumNeutralParticles>2 && abs(jet_eta_)>2.7 && abs(jet_eta_)<=3.0 ) ||
                (NEMF<0.90 && NumNeutralParticles>10 && abs(jet_eta_)>3.0 );
        //I do it only for eta that I use
        bool tightJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        bool tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        jet_jetId_=tightJetID*2+4*tightLepVetoJetID;
        
    }catch(const cms::Exception &e){
        jet_looseId_ = -1;
        jet_jetId_=-1;
    }
    
    jet_puId_= 0;
    jet_hflav_=abs(jet.hadronFlavour());
    jet_pflav_=abs(jet.partonFlavour());
    jet_phflav_=0;
    if(jet.genParton()) jet_phflav_=abs(jet.genParton()->pdgId());

    gen_pt_ =  0;
    Delta_gen_pt_ =  0;
    gen_pt_Recluster_=0;
    gen_pt_WithNu_=0;
    Delta_gen_pt_Recluster_=0;
    Delta_gen_pt_WithNu_=0;
    if(!jet.genJet()){
        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
	reco::GenJetRef genjetDuplication = (*genJetMatchAllowDuplicates)[patJetRef];
        if (genjetDuplication.isNonnull() && genjetDuplication.isAvailable()) {
	  returnval=false;
        }

    }

    if(jet.genJet()){
        gen_pt_ =  jet.genJet()->pt();
        Delta_gen_pt_ =  jet.genJet()->pt()- jet_pt_;

	float deltaR_match = reco::deltaR(jet, *jet.genJet());
	if( deltaR_match > 0.2){
	  returnval = false;
	}

        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
        reco::GenJetRef genjetRecluster = (*genJetMatchRecluster)[patJetRef];

        gen_pt_Recluster_ = 0.;
        if (genjetRecluster.isNonnull() && genjetRecluster.isAvailable()) {
            gen_pt_Recluster_ = genjetRecluster->pt();
        }
        reco::GenJetRef genjetWithNu = (*genJetMatchWithNu)[patJetRef];

        gen_pt_WithNu_ = 0.;
        if (genjetWithNu.isNonnull() && genjetWithNu.isAvailable()) {
            gen_pt_WithNu_ = genjetWithNu->pt();
        }

        Delta_gen_pt_Recluster_=gen_pt_Recluster_-jet.pt();
        Delta_gen_pt_WithNu_=gen_pt_WithNu_-jet.pt();
    }

    auto qgtuple=yuta::calcVariables(&jet);
    //(multiplicity, charged_multiplicity, neutral_multiplicity, ptD, axis1, axis2, pt_dr_log);

    y_multiplicity_=std::get<0>(qgtuple);
    y_charged_multiplicity_=std::get<1>(qgtuple);
    y_neutral_multiplicity_=std::get<2>(qgtuple);
    y_ptD_    =  std::get<3>(qgtuple);
    y_axis1_  =  std::get<4>(qgtuple);
    y_axis2_  =  std::get<5>(qgtuple);
    y_pt_dr_log_=std::get<6>(qgtuple);

    return returnval;
}
