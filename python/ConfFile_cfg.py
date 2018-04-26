import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )
process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '94X_mc2017_realistic_v12'

#process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
#    fileNames = cms.untracked.vstring(
#        'file:myfile.root'
#    )
#)


########## Remove lines below if you do not want to override the global tag ###################
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
##########
from CondCore.DBCommon.CondDBSetup_cfi import *
process.ecalConditions = cms.ESSource("PoolDBESSource", CondDBSetup,
      #connect = cms.string('frontier://FrontierProd/CMS_COND_ECAL'),
      #connect = cms.string('sqlite_file:../../../CondTools/Ecal/python/DB.db'),
      connect = cms.string('sqlite_file:../../../CondTools/Ecal/python/simPulseShapePhaseI.db'),
      toGet = cms.VPSet(         # overide Global Tag use EcalTBWeights_EBEE_offline
                  cms.PSet(
                      record = cms.string('EcalSimPulseShapeRcd') ,
                      tag = cms.string('EcalSimPulseShape_default_mc') ##### Put here the appropiate weight record
                  ),
                #  cms.PSet(
                #      record = cms.string('EcalWeightXtalGroupsRcd') ,
                #      tag = cms.string('EcalWeightXtalGroups_EBEE_offline')
                #  )
              )
)
process.es_prefer_EcalTBWeights = cms.ESPrefer("PoolDBESSource","ecalConditions")
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


process.p = cms.Path(process.demo)
