import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = '94X_mc2017_realistic_v12'



########## Remove lines below if you do not want to override the global tag ###################
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
##########
from CondCore.CondDB.CondDB_cfi import *
process.ecalConditions = cms.ESSource("PoolDBESSource",
       connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS'),
     # connect = cms.string('sqlite_file:../../../CondTools/Ecal/python/simPulseShapePhaseI.db'),
      #connect = cms.string('sqlite_file:../../../CondTools/Ecal/python/simPulseShapePhaseII.db'),
      toGet = cms.VPSet(         
                  cms.PSet(
                      record = cms.string('EcalSimPulseShapeRcd') ,
                      tag = cms.string('EcalSimPulseShape_default_mc')
                      #tag = cms.string('simPulseShapePhaseI')
                  ),
              )
)
#process.es_prefer_ecalConditions = cms.ESPrefer("PoolDBESSource","ecalConditions")
#############################################################################################



process.source = cms.Source("EmptySource",
                             numberEventsInRun = cms.untracked.uint32(1), # do not change!
                             firstRun = cms.untracked.uint32(1)
                           )

process.TFileService = cms.Service("TFileService",
        fileName = cms.string("ecalshapes.root")
        # closeFileFast = cms.untracked.bool(True)
)


process.demo = cms.EDAnalyzer('ECALShapeTest')
process.demo.useDB = cms.bool(True)

process.p = cms.Path(process.demo)
