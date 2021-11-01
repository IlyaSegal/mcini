#include <iostream>

#include "TFile.h"
#include "TString.h"
#include "TTree.h"

#include "../include/EventInitialState.h"
#include "../include/UEvent.h"
#include "../include/URun.h"

void convertMCGlauber(TString inputFileName = "Au_Au_12AGeV.root", TString inputNtupleName = "nt_Au_Au", TString outputFileName = "glauber.root",
    Int_t aProj = 197, Int_t zProj = 79, Double_t pProj = 12., 
    Int_t aTarg = 197, Int_t zTarg = 79, Double_t pTarg = 0.,
    Double_t bMin = 0., Double_t bMax = 20., Int_t bWeight = 0,
    Double_t phiMin = 0., Double_t phiMax = 0.,
    Double_t sigma = 0., Int_t nEvents = 0)
{
    TFile *fIn = new TFile(inputFileName, "read");
    TNtuple *fNtuple = (TNtuple*) fIn->Get(inputNtupleName);
    if (!fNtuple)
    {
        std::cerr << "TNtuple was not found in the input file." << std::endl;
        return;
    }
    
    
    Float_t Npart=0;   
    Float_t Ncoll=0;   
    Float_t B=0;           

    fNtuple->SetBranchAddress("Npart",&Npart);
    fNtuple->SetBranchAddress("Ncoll",&Ncoll);
    fNtuple->SetBranchAddress("B",&B);

    TFile *fOut = new TFile(outputFileName, "recreate");

    TTree *iniTree = new TTree("events", "MC-Glauber");
    URun header("MC-Glauber","initial state only", aProj, zProj, pProj, aTarg, zTarg, pTarg, bMin, bMax, bWeight, phiMin, phiMax, sigma, nEvents);
    UEvent *event = new UEvent;
    EventInitialState *iniState = new EventInitialState;
    iniTree->Branch("iniState", "EventInitialState", iniState);
    iniTree->Branch("event", "UEvent", event);

    Long64_t nentries = fNtuple->GetEntriesFast();
    Long64_t eventCounter = 0;
    Int_t child[2] = {0,0};

    for (Long64_t iev=0; iev<nentries; iev++)
    {
        fNtuple->GetEntry(iev);

        event->Clear();
        iniState->Clear();

        if (iev % 100 == 0) std::cout << "Event [" << iev << "/" << nentries << "]" << std::endl;

        // Fill event
        event->SetEventNr(iev);
        event->SetB(B);
        iniState->setNPart(Npart);
        iniState->setNColl(Ncoll);

        iniTree->Fill();
        eventCounter++;
    }
    header.SetNEvents(eventCounter);
    
    fOut->cd();
    header.Write();
    iniTree->Write();
    fOut->Close();
}
