// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <boost/core/demangle.hpp>

#include "../interface/ntuple_content.h"
#include "../interface/ntuple_SV.h"
#include "../interface/ntuple_JetInfo.h"
#include "../interface/ntuple_pfCands.h"
#include "../interface/ntuple_bTagVars.h"
#include "../interface/ntuple_FatJetInfo.h"
#include "../interface/ntuple_pairwise.h"
#include "../interface/ntuple_LT.h"
//#include "../interface/ntuple_DeepVertex.h"
//ROOT includes
#include "TTree.h"
#include <TFile.h>
#include <TROOT.h>
#include "TBranch.h"
#include <string>
#include <vector>
#include "TSystem.h"
#include <TRandom.h>

//CMSSW includes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
// for ivf
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TLorentzVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "DataFormats/BTauReco/interface/PixelClusterTagInfo.h"

#include <algorithm>
#include <iterator>
#include <map>

//trash?

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"


#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "../interface/TrackInfoBuilder.h"

#if defined( __GXX_EXPERIMENTAL_CXX0X__)
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#endif

struct MagneticField;
const reco::TrackBaseRef toTrackRef(const reco::PFCandidate * pfcand);

class DeepNtuplizer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit DeepNtuplizer(const edm::ParameterSet&);
  ~DeepNtuplizer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  Measurement1D vertexDxy(const reco::VertexCompositePtrCandidate &svcand, const reco::Vertex &pv) const;
  Measurement1D vertexD3d(const reco::VertexCompositePtrCandidate &sv, const reco::Vertex &pv) const ;
  float vertexDdotP(const reco::VertexCompositePtrCandidate &sv, const reco::Vertex &pv) const ;


  // ----------member data ---------------------------
  edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
  edm::EDGetTokenT<reco::VertexCompositePtrCandidateCollection> svToken_;
  edm::EDGetTokenT<edm::View<pat::Jet> >      jetToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puToken_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT< edm::View<reco::BaseTagInfo> > pixHitsToken_;
  std::string t_qgtagger;
  
  
 //////////////////////////////////////////////////////////////////
 edm::EDGetTokenT<edm::ValueMap<float>> tofPIDProbPToken_;
edm::EDGetTokenT<edm::ValueMap<float>> tofPIDProbKToken_;
edm::EDGetTokenT<edm::ValueMap<float>> tofPIDProbPiToken_;

// Token for tracks
edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
 
 /////////////////////////////////////////////////////////////////
  

  edm::Service<TFileService> fs;
  TTree *tree_;

//$$
  float event_time_ = 0;
  float jet_time_   = 0;

  const reco::Vertex  *pv;

  bool PV4D = false; // if event time is taken from PV4D
//$$

  size_t njetstotal_;
  size_t njetswithgenjet_;
  size_t njetsselected_;
  size_t njetsselected_nogen_;

  ntuple_content * addModule(ntuple_content *m, std::string name = ""){
    modules_.push_back(m);
    module_names_.push_back(name);
    return m;
  }
  std::vector<ntuple_content* > modules_;
  std::vector<std::string> module_names_;

  bool applySelection_;
  };

DeepNtuplizer::DeepNtuplizer(const edm::ParameterSet& iConfig):
  vtxToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
  svToken_(consumes<reco::VertexCompositePtrCandidateCollection>(iConfig.getParameter<edm::InputTag>("secVertices"))),
  jetToken_(consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jets"))),
  puToken_(consumes<std::vector<PileupSummaryInfo >>(iConfig.getParameter<edm::InputTag>("pupInfo"))),
  rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rhoInfo"))),
  pixHitsToken_(consumes< edm::View<reco::BaseTagInfo> > (iConfig.getParameter<edm::InputTag>("pixelhit"))),
  t_qgtagger(iConfig.getParameter<std::string>("qgtagger"))

{	
  //////////////////////////////////////
    tofPIDProbPToken_  = consumes<edm::ValueMap<float>>(edm::InputTag("tofPID", "probP",  "RECO"));
    tofPIDProbKToken_  = consumes<edm::ValueMap<float>>(edm::InputTag("tofPID", "probK",  "RECO"));
    tofPIDProbPiToken_ = consumes<edm::ValueMap<float>>(edm::InputTag("tofPID", "probPi", "RECO"));

    tracksToken_ = consumes<reco::TrackCollection>(edm::InputTag("generalTracks"));
//    tracksToken_ = consumes<reco::TrackCollection>(edm::InputTag("trackExtenderWithMTD"));

  ///////////////////////////////////

 

  /*
   *  Initialise the modules here
   *  Everything else does not need to be changed if
   *  modules don't interact.
   */

  // read configuration parameters
  const double jetR = iConfig.getParameter<double>("jetR");
  const bool  runFatJets_ = iConfig.getParameter<bool>("runFatJet");
  // AS const bool  runDeepVertex_ = iConfig.getParameter<bool>("runDeepVertex");

  //not implemented yet
  const bool useHerwigCompatibleMatching=iConfig.getParameter<bool>("useHerwigCompatible");
  const bool isHerwig=iConfig.getParameter<bool>("isHerwig");

  ntuple_content::useoffsets = iConfig.getParameter<bool>("useOffsets");

  applySelection_=iConfig.getParameter<bool>("applySelection");

  ntuple_SV* svmodule=new ntuple_SV("", jetR);
  svmodule->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));
  addModule(svmodule, "SVNtuple");

  ntuple_JetInfo* jetinfo=new ntuple_JetInfo();
  jetinfo->setQglToken(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "qgLikelihood")));
  jetinfo->setPtDToken(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "ptD")));
  jetinfo->setAxis2Token(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "axis2")));
  jetinfo->setMultToken(consumes<edm::ValueMap<int>>(edm::InputTag(t_qgtagger, "mult")));

  jetinfo->setUseHerwigCompatibleMatching(useHerwigCompatibleMatching);
  jetinfo->setIsHerwig(isHerwig);
  jetinfo->setgenVtxPositionToken(consumes<ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float>, ROOT::Math::DefaultCoordinateSystemTag>>(iConfig.getParameter<edm::InputTag>("genvtx_pos")));
  jetinfo->setgenVtxTimeToken(consumes<float>(iConfig.getParameter<edm::InputTag>("genvtx_t")));
  jetinfo->setGenJetMatchReclusterToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchRecluster" )));
  jetinfo->setGenJetMatchWithNuToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchWithNu" )));
  jetinfo->setGenJetMatchAllowDuplicatesToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchAllowDuplicates" ))); 
  jetinfo->setGenParticlesToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("pruned")));
  jetinfo->setMuonsToken(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")));
  jetinfo->setElectronsToken(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons")));
  jetinfo->setPUInfoToken(consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("slimmedAddPileupInfo")));
  
  addModule(jetinfo, "jetinfo");

  ntuple_pfCands * pfcands = new ntuple_pfCands();
 

  pfcands->setJetRadius(jetR);
  pfcands->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));

  addModule(pfcands, "pfcands");

  ntuple_LT * LT = new ntuple_LT();
  LT->setJetRadius(jetR);
  LT->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));
  LT->setLTToken(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("losttracks")));

  addModule(LT, "LT");

  ntuple_pairwise * pairwise = new ntuple_pairwise();
  pairwise->setJetRadius(jetR);
  pairwise->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));

  addModule(pairwise, "pairwise");

  addModule(new ntuple_bTagVars(), "bTagVars");

  if(runFatJets_){
    auto *fatjetinfo = new ntuple_FatJetInfo(jetR);
    fatjetinfo->setGenParticleToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("pruned")));
    fatjetinfo->setFatJetToken(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("fatjets")));
    addModule(fatjetinfo, "fatJets");
  }

  /*
   *
   * Modules initialized
   *
   * parse the input parameters (if any)
   */
   
  for(auto& m: modules_)
    m->getInput(iConfig);

}


DeepNtuplizer::~DeepNtuplizer()
{
  return;
  for(auto& m:modules_)
    delete m;
}


// ------------ method called for each event  ------------
void
DeepNtuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  //global info
  
  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtxToken_, vertices);
  if (vertices->empty()) return; // skip the event if no PV found
//$$
  pv = &(*vertices->begin());
  float PVtime      = (pv)[0].t();
  float PVtimeError = (pv)[0].tError();
//$$
  float event_time       = 0;
  float event_timeNtk = 0;

  edm::View<pat::Jet>::const_iterator jetIter;

  edm::Handle<std::vector<reco::VertexCompositePtrCandidate> > secvertices;
  iEvent.getByToken(svToken_, secvertices);

  edm::Handle<std::vector<PileupSummaryInfo> > pupInfo;
  iEvent.getByToken(puToken_, pupInfo);

  edm::Handle<double> rhoInfo;
  iEvent.getByToken(rhoToken_,rhoInfo);

  edm::Handle<edm::View<pat::Jet> > jets;
  iEvent.getByToken(jetToken_, jets);
  
  
  ///////////////////////////////////////////////77
  edm::Handle<edm::ValueMap<float>> tofProtonHandle;
  iEvent.getByToken(tofPIDProbPToken_, tofProtonHandle);

  edm::Handle<edm::ValueMap<float>> tofKaonHandle;
  iEvent.getByToken(tofPIDProbKToken_, tofKaonHandle);

  edm::Handle<edm::ValueMap<float>> tofPionHandle;
  iEvent.getByToken(tofPIDProbPiToken_, tofPionHandle);

  edm::Handle<reco::TrackCollection> tracksHandle;
  iEvent.getByToken(tracksToken_, tracksHandle);

  auto* pfcands = static_cast<ntuple_pfCands*>(modules_[2]);

  //if (tofProtonHandle.isValid()) {   // check if the Handle actually points to data
    pfcands->setHandles(tofProtonHandle, tofKaonHandle, tofPionHandle, tracksHandle);  

  edm::Handle< edm::View<reco::BaseTagInfo> > pixHits;
  iEvent.getByToken(pixHitsToken_, pixHits);
  for(auto& m:modules_){
    m->setPrimaryVertices(vertices.product());
    m->setSecVertices(secvertices.product());
    m->setPuInfo(pupInfo.product());
    m->setRhoInfo(rhoInfo.product());
    m->readSetup(iSetup);
    m->readEvent(iEvent);
  }
  
  std::vector<size_t> indices(jets->size());
  for(size_t i=0;i<jets->size();i++)
    indices.at(i)=i;

  if(applySelection_)
    std::random_shuffle (indices.begin(),indices.end());

  if ( !PV4D ) { 
    for (size_t j=0; j<indices.size(); j++) {
      size_t jetidx=indices.at(j);
      jetIter = jets->begin()+jetidx;
      const pat::Jet& jet = *jetIter;
      for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++) {
        const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
      if ( !PackedCandidate ) continue;
      if ( PackedCandidate->charge() == 0 ) continue;
     	auto track = PackedCandidate->bestTrack();
      if ( !track ) continue;
        float track_dxy = track->dxy(pv->position());
        float track_dz  = track->dz(pv->position());
	float cand_time = PackedCandidate->time();
	float cand_timeError = PackedCandidate->timeError();
        if ( cand_timeError > 0. && abs(cand_time) < 1 
	     && abs(track_dxy) < 0.05 && abs(track_dz) < 0.10 ) {
          event_timeNtk    += 1;
          event_time	   += cand_time;
        }
      }
    }
    if ( event_timeNtk > 0 ) event_time /= event_timeNtk;
      else                     event_time = -1;
  }
  else {
    if ( PVtimeError > 0. ) {
      event_timeNtk = 1;
      event_time = PVtime;
    }
    else {
      event_timeNtk = 0;
      event_time = -1;
    }
  }
    event_time_ = event_time;
  /*for (size_t i_j = 0; i_j < jets->size(); ++i_j) {
    pat::Jet jet = jets->at(i_j);
    edm::RefToBase<pat::Jet> jetRef = jets->refAt(i_j);
    std::vector<std::string> labels = jet.tagInfoLabels();
    for(unsigned int k = 0; k < labels.size(); k++) {
      //std::cout << labels[k] << " "  << std::endl;
    }
    if(jet.hasTagInfo("pixelCluster")){
      std::cout << jetRef.get() << " | " << jetRef << " : ";
      const reco::PixelClusterTagInfo *test = static_cast<const reco::PixelClusterTagInfo*>( jet.tagInfo("pixelCluster") );
      reco::PixelClusterData pcd =test->data();
      for(size_t i = 0; i < pcd.r004.size(); i++){
	std::cout << "\tL1 004: " << (int)pcd.r004[i] << std::endl;
      }
    }
    }*/

  // loop over the jets
  //for (edm::View<pat::Jet>::const_iterator jetIter = jets->begin(); jetIter != jets->end(); ++jetIter) {
  for(size_t j=0;j<indices.size();j++){
    njetstotal_++;
    size_t jetidx=indices.at(j);
    jetIter = jets->begin()+jetidx;
    const pat::Jet& jet = *jetIter;

    if(jet.genJet())
      njetswithgenjet_++;

    bool writejet=true;
    size_t idx = 0;
    for(auto& m:modules_){
       //std::cout << module_names_[idx] << std::endl;
      //if(! m->fillBranches(jet, jetidx, jets.product())){
      if(! m->fillBranches(jet, jetidx, jets.product(),event_time)){
        writejet=false;
	if(applySelection_) break;
      }
      idx++;
    }

    //$$
      float jet_time	   = 0;
      float jet_timeNtk    = 0;
      float jet_timeError  = 0;

      int  nSV = -2;
      const reco::CandSecondaryVertexTagInfo *candSVTagInfo = jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder");
      if ( candSVTagInfo != nullptr ) nSV = candSVTagInfo->nVertices();
      if ( nSV > 0 && candSVTagInfo->vertexTracks().size() == 0 ) nSV = -1;

//   std::cout << "        jet " << j << " pt eta phi "
// 	 << jet.pt() << " " << jet.eta() << " " << jet.phi() << "   nSV " << nSV << std::endl;

	if (jet.pt()>0){



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
          jet_time += cand_time;
        }

	}  // end loop on tracks in jets

      if ( jet_timeNtk > 0 ) {
        jet_time = jet_time/jet_timeNtk;
        jet_timeError = sqrt(jet_timeError) / jet_timeNtk;
      }
      else{ 
        jet_time = -1;
        jet_timeError=-1;
       }
      jet_time_ = jet_time;
//$$
    //std::cout << "Jet done" << std::endl;
    if( (writejet&&applySelection_) || !applySelection_ ){
      tree_->Fill();
      njetsselected_++;
      if(!jet.genJet())
	njetsselected_nogen_++;

    }


	} //   <-----------

  } // end of looping over the jets
}


// ------------ method called once each job just before starting event loop  ------------
void
DeepNtuplizer::beginJob()
{
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  tree_=(fs->make<TTree>("tree" ,"tree" ));

  for(auto& m:modules_)
    m->initBranches(tree_);

  njetstotal_=0;
  njetswithgenjet_=0;
  njetsselected_=0;
  njetsselected_nogen_=0;
}

// ------------ method called once each job just after ending the event loop  ------------
void
DeepNtuplizer::endJob()
{

  std::cout << "total number of processed jets: " << njetstotal_<<std::endl;
  std::cout << "total number of jets with gen:  " << njetswithgenjet_<<std::endl;
  std::cout << "total number of selected jets:  "<< njetsselected_<<std::endl;
  std::cout << "fraction of selected jets:      "<< (float)njetsselected_/(float)njetstotal_<<std::endl;
  std::cout << "fraction of selected jets with gen: "<< (float)njetsselected_/(float)njetswithgenjet_<<std::endl;
  std::cout << "fraction of selected jetsout with gen: "<< (float)njetsselected_nogen_/(float)njetsselected_<<std::endl;

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DeepNtuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DeepNtuplizer);
