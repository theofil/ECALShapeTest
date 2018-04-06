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

   int nBins   = 1000;
   float tMin  = 0;
   float tMax  = 1000;
   EBShape_  = fTFileService_->make<TH1F>("EBShape","EB Shape;time [ns];amplitude", nBins, tMin, tMax);
   EEShape_  = fTFileService_->make<TH1F>("EEShape","EE Shape;time [ns];amplitude", nBins, tMin, tMax);
   APDShape_ = fTFileService_->make<TH1F>("APDShape","APD Shape;time [ns];amplitude", nBins, tMin, tMax);

   EBShapeDeriv_  = fTFileService_->make<TH1F>("EBShapeDeriv","EB ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);
   EEShapeDeriv_  = fTFileService_->make<TH1F>("EEShapeDeriv","EE ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);
   APDShapeDeriv_ = fTFileService_->make<TH1F>("APDShapeDeriv","APD ShapeDeriv;time [ns];amplitude", nBins, tMin, tMax);
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

   EBShape EcalEBShape(iSetup);
   double risingTimeEB = EcalEBShape.timeToRise();
   double tzeroEB = risingTimeEB  - 5*ADC_clock;  // 5 samples before the peak

   EEShape EcalEEShape(iSetup);
   double risingTimeEE = EcalEEShape.timeToRise();
   double tzeroEE = risingTimeEE  - 5*ADC_clock;  

   //APDShape EcalAPDShape(74.5, 40.5, iSetup);
   APDShape EcalAPDShape(iSetup);
   double risingTimeAPD = EcalAPDShape.timeToRise();
   double tzeroAPD = risingTimeAPD - 5*ADC_clock;


   for(int iSample=0; iSample<1000; iSample++)
   {
        EBShape_ ->SetBinContent(iSample, EcalEBShape (tzeroEB  + float(iSample)) );
        EEShape_ ->SetBinContent(iSample, EcalEEShape (tzeroEE  + float(iSample)) );
        APDShape_->SetBinContent(iSample, EcalAPDShape(tzeroAPD + float(iSample)) );

        EBShapeDeriv_ ->SetBinContent(iSample, EcalEBShape.derivative(tzeroEB  + float(iSample)) );
        EEShapeDeriv_ ->SetBinContent(iSample, EcalEEShape.derivative(tzeroEE  + float(iSample)) );
        APDShapeDeriv_->SetBinContent(iSample, EcalAPDShape.derivative(tzeroAPD + float(iSample)) );
   }



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
