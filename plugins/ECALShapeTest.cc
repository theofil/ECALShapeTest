// -*- C++ -*-
//
// Package:    Tools/ECALShapeTest
// Class:      ECALShapeTest
// 
/**\class ECALShapeTest ECALShapeTest.cc Tools/ECALShapeTest/plugins/ECALShapeTest.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Konstantinos Theofilatos (ETHZ) [theofil]
//         Created:  Thu, 01 Mar 2018 15:35:16 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <iostream>

#include "SimCalorimetry/EcalSimAlgos/interface/EBShape.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EEShape.h"
#include "SimCalorimetry/EcalSimAlgos/interface/APDShape.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class ECALShapeTest : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit ECALShapeTest(const edm::ParameterSet&);
      ~ECALShapeTest();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::Service<TFileService> fTFileService_;
      TH1F *EBShape_;
      TH1F *EEShape_;
      TH1F *APDShape_;

      TH1F *EBShapeDeriv_;
      TH1F *EEShapeDeriv_;
      TH1F *APDShapeDeriv_;

      TTree *shapes_;
      double timeToRise_EB_;
      double timeToRise_EE_;
      double timeToRise_APD_;

      double timeOfThr_EB_;
      double timeOfThr_EE_;
      double timeOfThr_APD_;

      double timeOfMax_EB_;
      double timeOfMax_EE_;
      double timeOfMax_APD_;

      double threshold_EB_;
      double threshold_EE_;
      double threshold_APD_;
    
      double shape_EB_[500000]; // save first 500 ns without any time shift with 0.05 ns 
      double shape_EE_[500000];
      double shape_APD_[500000];
      double time_[500000];
       
      bool useDB_;
     
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ECALShapeTest::ECALShapeTest(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed
   usesResource("TFileService");

   useDB_ = iConfig.getParameter<bool>("useDB");

   int nBins   = 1000;
   float tMin  = 0;
   float tMax  = 1000;
   EBShape_  = fTFileService_->make<TH1F>("EBShape","EB Shape;time [ns];amplitude", nBins, tMin, tMax);
   EEShape_  = fTFileService_->make<TH1F>("EEShape","EE Shape;time [ns];amplitude", nBins, tMin, tMax);
   APDShape_ = fTFileService_->make<TH1F>("APDShape","APD Shape;time [ns];amplitude", nBins, tMin, tMax);

   EBShapeDeriv_  = fTFileService_->make<TH1F>("EBShapeDeriv","EB ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);
   EEShapeDeriv_  = fTFileService_->make<TH1F>("EEShapeDeriv","EE ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);
   APDShapeDeriv_ = fTFileService_->make<TH1F>("APDShapeDeriv","APD ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);

   shapes_  = fTFileService_->make<TTree>("shapes","shapes");

   shapes_  ->Branch("timeToRise_EB",  &timeToRise_EB_,  "timeToRise_EB/D");
   shapes_  ->Branch("timeToRise_EE",  &timeToRise_EE_,  "timeToRise_EE/D");
   shapes_  ->Branch("timeToRise_APD", &timeToRise_APD_, "timeToRise_APD/D");

   shapes_  ->Branch("timeOfThr_EB",  &timeOfThr_EB_,  "timeOfThr_EB/D");
   shapes_  ->Branch("timeOfThr_EE",  &timeOfThr_EE_,  "timeOfThr_EE/D");
   shapes_  ->Branch("timeOfThr_APD", &timeOfThr_APD_, "timeOfThr_APD/D");

   shapes_  ->Branch("timeOfMax_EB",  &timeOfMax_EB_,  "timeOfMax_EB/D");
   shapes_  ->Branch("timeOfMax_EE",  &timeOfMax_EE_,  "timeOfMax_EE/D");
   shapes_  ->Branch("timeOfMax_APD", &timeOfMax_APD_, "timeOfMax_APD/D");

   shapes_  ->Branch("threshold_EB",  &threshold_EB_,  "threshold_EB/D");
   shapes_  ->Branch("threshold_EE",  &threshold_EE_, " threshold_EE/D");
   shapes_  ->Branch("threshold_APD", &threshold_APD_, "threshold_APD/D");


   shapes_  ->Branch("shape_EB",  shape_EB_,  "shape_EB[500000]/D");
   shapes_  ->Branch("shape_EE",  shape_EE_,  "shape_EE[500000]/D");
   shapes_  ->Branch("shape_APD", shape_APD_, "shape_APD[500000]/D");
   shapes_  ->Branch("time",      time_,      "time[500000]/D");
}


ECALShapeTest::~ECALShapeTest()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ECALShapeTest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   // ------------------------

   double ADC_clock = 25.0; // 25 ns
   if(useDB_) cout <<"going to use DB" << endl;  
   if(!useDB_) cout <<"NOT going to use DB" << endl;  

   EBShape EcalEBShape(useDB_);
   if(useDB_)EcalEBShape.setEventSetup(iSetup);

   double risingTimeEB = EcalEBShape.timeToRise();
   double tzeroEB = risingTimeEB  - 5*ADC_clock;  // 5 samples before the peak
   EcalEBShape.m_shape_print("EBShape.txt");
   cout << endl;

   cout << "creating EE shape" << endl; 
   EEShape EcalEEShape(useDB_);

   if(useDB_)EcalEEShape.setEventSetup(iSetup);
   double risingTimeEE = EcalEEShape.timeToRise();
   double tzeroEE = risingTimeEE  - 5*ADC_clock;  
   EcalEEShape.m_shape_print("EEShape.txt");
   cout << endl;

   cout << "creating APD shape" << endl; 
   APDShape EcalAPDShape(useDB_);
   if(useDB_)EcalAPDShape.setEventSetup(iSetup);

   double risingTimeAPD = EcalAPDShape.timeToRise();
   double tzeroAPD = risingTimeAPD - 5*ADC_clock;
   EcalAPDShape.m_shape_print("APDShape.txt");
   cout << endl;

   for(int iSample=0; iSample<1000; iSample++)
   {
        EBShape_ ->SetBinContent(iSample, EcalEBShape (tzeroEB  + float(iSample)) );
        EEShape_ ->SetBinContent(iSample, EcalEEShape (tzeroEE  + float(iSample)) );
        APDShape_->SetBinContent(iSample, EcalAPDShape(tzeroAPD + float(iSample)) );

        EBShapeDeriv_ ->SetBinContent(iSample, EcalEBShape.derivative(tzeroEB  + float(iSample)) );
        EEShapeDeriv_ ->SetBinContent(iSample, EcalEEShape.derivative(tzeroEE  + float(iSample)) );
        APDShapeDeriv_->SetBinContent(iSample, EcalAPDShape.derivative(tzeroAPD + float(iSample)) );
   }

   timeToRise_EB_ = EcalEBShape.timeToRise();
   timeToRise_EE_ = EcalEEShape.timeToRise();
   timeToRise_APD_ = EcalAPDShape.timeToRise();

   timeOfThr_EB_ = EcalEBShape.timeOfThr();
   timeOfThr_EE_ = EcalEEShape.timeOfThr();
   timeOfThr_APD_ = EcalAPDShape.timeOfThr();

   timeOfMax_EB_ = EcalEBShape.timeOfMax();
   timeOfMax_EE_ = EcalEEShape.timeOfMax();
   timeOfMax_APD_ = EcalAPDShape.timeOfMax();

   threshold_EB_ = EcalEBShape.threshold();
   threshold_EE_ = EcalEEShape.threshold();
   threshold_APD_ = EcalAPDShape.threshold();

   for(int iTime=0; iTime<500000; iTime++)
   {
        double absTime = iTime*0.001;
	time_[iTime] = absTime;
        
        shape_EB_[iTime]  = EcalEBShape(absTime);
        shape_EE_[iTime]  = EcalEEShape(absTime);
        shape_APD_[iTime] = EcalAPDShape(absTime);
   }

   shapes_->Fill();


#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
ECALShapeTest::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ECALShapeTest::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ECALShapeTest::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ECALShapeTest);
