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
