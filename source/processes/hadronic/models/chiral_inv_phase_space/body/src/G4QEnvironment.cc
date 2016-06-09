//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//       1         2         3         4         5         6         7         8         9
//34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
//
//
// $Id: G4QEnvironment.cc,v 1.123 2007/05/03 07:35:27 mkossov Exp $
// GEANT4 tag $Name: geant4-08-03 $
//
//      ---------------- G4QEnvironment ----------------
//             by Mikhail Kossov, August 2000.
//  class for Multy Quasmon Environment used by the CHIPS Model
// ------------------------------------------------------------
 
//#define chdebug
//#define debug
//#define sdebug
//#define ppdebug
//#define cdebug
//#define cldebug
//#define edebug
//#define fdebug
//#define pdebug
//#define rdebug
//#define ffdebug
//#define pcdebug
//#define mudebug

#include "G4QEnvironment.hh" 
#include <cmath>
#include <cstdlib>
using namespace std;

G4QEnvironment::G4QEnvironment(const G4QHadronVector& projHadrons, const G4int targPDG) :
  theEnvironment(90000000)                   // User is responsible for projHadrons(Vector)
{
  //static const G4double mNeut= G4QPDGCode(2112).GetMass();
  //static const G4QContent neutQC(2,1,0,0,0,0);
  static const G4QPDGCode pimQPDG(-211);
  theWorld= G4QCHIPSWorld::Get();            // Get a pointer to the CHIPS World
  G4bool fake=false;                         // At present only fake pi-
  theTargetPDG=targPDG;                      // Remenber it for error message
  G4int nHadrons=projHadrons.size();         // A#of hadrons in the input Vector
#ifdef debug
  G4cout<<">>>>>>G4QE::Const: Called targPDG="<<targPDG<<", nInpHadr="<<nHadrons<<G4endl;
#endif
  if(nHadrons<1 || targPDG==90000000)        // No projectile Hadrons or no target Nucleus
  {
    G4cerr<<"---Warning---G4QEnv::Const:a#ofINPHadr="<<nHadrons<<",tPDG="<<targPDG<<G4endl;
    //throw G4QException("***G4QEnvironment: There is no one projectile or vacuum target");
    if(nHadrons)                             // The projectiles are copied to the output
	   {
      for(G4int ih=0; ih<nHadrons; ih++)
      {
        G4QHadron* curQH    = new G4QHadron(projHadrons[ih]);
#ifdef pdebug
        G4cout<<"*G4QE::Const:iH#"<<ih<<","<<curQH->GetQC()<<curQH->Get4Momentum()<<G4endl;
#endif
        theQHadrons.push_back(curQH);        // (delete equivalent)
        curQH    = new G4QHadron(projHadrons[ih]); // ... to remember
        theProjectiles.push_back(curQH);      // Remenber it for the error message
      }
	   }
    else if(targPDG!=90000000)               // No projHadrons,fill targetNucleus to output
    {
      G4QHadron* curQH    = new G4QHadron(targPDG);
#ifdef pdebug
      G4cout<<"**G4QE::Const:No iHad,eH="<<curQH->GetQC()<<curQH->Get4Momentum()<<G4endl;
#endif
      theQHadrons.push_back(curQH);          // (delete equivalent)
    }
    if (nHadrons<0) G4cerr<<"**G4QE::Const:NH="<<nHadrons<<" < 0 !"<<G4endl;
    return;
  }
  G4QPDGCode targQPDG(targPDG);
#ifdef pdebug
  G4cout<<"G4QE::C:targQPDG="<<targQPDG<<G4endl;
#endif
  G4int    targA=targQPDG.GetBaryNum();
  G4double targM=targQPDG.GetMass();
  totCharge=targQPDG.GetCharge();
  totBaryoN=targA;
  tot4Mom=G4LorentzVector(0.,0.,0.,targM);
  // === Print out of the input information at Creation time & tot 4-mom Calculation ======
#ifdef pdebug
  G4cout<<"G4QE::C:PDG="<<targPDG<<",C="<<totCharge<<",M="<<targM<<",n="<<nHadrons<<G4endl;
#endif
  for(G4int ipr=0; ipr<nHadrons; ipr++)// LOOP is used for the tot4Mom calc. & for printing
  {
    G4QHadron* prHadr = projHadrons[ipr];
    G4QHadron* curQH  = new G4QHadron(prHadr);// Remenber it for _
    theProjectiles.push_back(curQH);          // the error message
    G4LorentzVector h4Mom = prHadr->Get4Momentum();
    tot4Mom      += h4Mom;
    totCharge    += prHadr->GetCharge();
    totBaryoN    += prHadr->GetBaryonNumber();
#ifdef pdebug
    G4int           hPDG  = prHadr->GetPDGCode();
    G4int           hNFrag= prHadr->GetNFragments();
    G4QContent      hQC   = prHadr->GetQC();
    G4cout<<"G4QE::C:#"<<ipr<<",PDG="<<hPDG<<hQC<<",4M="<<h4Mom<<",hNFr="<<hNFrag<<G4endl;
#endif
  }
#ifdef pdebug
  G4cout<<"G4QEnv::Const:tC="<<totCharge<<",tB="<<totBaryoN<<",tot4M="<<tot4Mom<<G4endl;
#endif
#ifdef pdebug
  G4cout<<"G4QEnv::Const: ----> tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
  G4int nP=theWorld->GetQPEntries();         // A#of init'ed particles in CHIPS World
  G4int nCl=nP-90;                           // A#of init'ed clusters in CHIPS World
#ifdef pdebug
  G4cout<<"G4QEnv:Const:Before NCI:n="<<nP<<",F="<<projHadrons[0]->GetNFragments()<<",tC="
        <<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
  InitClustersVector(nCl,targA);             // Init Clusters as Particles (to interact)
#ifdef pdebug
  G4cout<<"G4QEnv::Const:NucClust,n="<<nCl<<",F="<<projHadrons[0]->GetNFragments()<<",tC="
        <<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
  if(targPDG>80000000)                        // ==> Nuclear target (including NUCPDG)
  {
    theEnvironment.InitByPDG(targPDG);        // Create nuclear environment
#ifdef pdebug
    G4cout<<"G4QEnv::Const:nH="<<nHadrons<<",PDG="<<projHadrons[0]->GetPDGCode()<<",tC="
          <<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
    if(nHadrons==1)
	   {
      G4QHadron* opHad=projHadrons[0];
      G4int opPDG=opHad->GetPDGCode();
#ifdef pdebug
      G4cout<<"G4QEnviron::Constructor: *** Only one input hadron*** PDG="<<opPDG<<G4endl;
#endif
      if(opPDG==22)                           // *** Check photon's NuclearSplitThreshold
	     {
        G4double exMass=tot4Mom.m();
#ifdef pdebug
        G4cout<<"G4QEnvironment::Const: exM="<<exMass-targM<<" > mPi0 ?"<<G4endl;
#endif      
        if(exMass<targM+135.977) // Nucleus is below the pion production threshold
	       {
          G4QNucleus exEnviron(tot4Mom,targPDG);
          // @@ One can put here the pbpt= (M.K.) @@ What about d,t,alpha splitting?
          if(targM>999.&&!exEnviron.SplitBaryon())//Nucleus is below SplitFragmentThreshold
		        {
#ifdef pdebug
            G4cout<<"G4QEnv::Const:Photon's added to Output, Env="<<theEnvironment<<G4endl;
#endif      
            G4QHadron* photon = new G4QHadron(opHad); // Fill projPhoton to Output
#ifdef pdebug
            G4cout<<"**G4QE::Const:Phot="<<photon->GetQC()<<photon->Get4Momentum()<<G4endl;
#endif
            theQHadrons.push_back(photon);      // (delete equivalent)
            return;
		        }
          else if(targM<=999.)                  // Target is a nucleon
	         {
            G4LorentzVector prot4m(0.,0.,0.,targM); // Prototype of secondary proton 4mom
            G4LorentzVector gam4m(0.,0.,0.,0.);     // Prototype for secondary gamma 4mom
            if(!G4QHadron(tot4Mom).DecayIn2(prot4m,gam4m))
            {
#ifdef pdebug
              G4cout<<"*War*G4QEnv::Const:(P)Photon->Output, Env="<<theEnvironment<<G4endl;
#endif      
              G4QHadron* photon = new G4QHadron(opHad); // Fill projPhoton to Output
#ifdef pdebug
              G4cout<<"**G4QE::Const:Ph="<<photon->GetQC()<<photon->Get4Momentum()<<G4endl;
#endif
              theQHadrons.push_back(photon);    // (delete equivalent)
              return;
            }
            G4QHadron* proton = new G4QHadron(targPDG,prot4m); // Fill tgProton to Output
            theQHadrons.push_back(proton);      // (delete equivalent)
            G4QHadron* photon = new G4QHadron(22,gam4m);       // Fill prPhoton to Output
            theQHadrons.push_back(photon);      // (delete equivalent)
            theEnvironment.InitByPDG(90000000); // Create nuclear environment
#ifdef pdebug
            G4cout<<"G4QEnv::Const:Fill proton and photon from gam+p"<<nuPDG<<nu4m<<G4endl;
#endif      
            return;
          }
        }
      }
      else if(opPDG==13 || opPDG==15)
	     {
        G4int         nuPDG=14;
        if(opPDG==15) nuPDG=16;
		      G4LorentzVector mu4m=opHad->Get4Momentum();
        //G4double qpen=-180.*log(G4UniformRand()); // Energy of target-quark-parton(T=180)
        G4double qpen=465.*sqrt(sqrt(G4UniformRand())); // UniformDistr for 3-q nucleon
        G4double qpct=2*G4UniformRand()-1.;         // Cos(thet) of target-quark-parton
        G4double qpst=sqrt(1.-qpct*qpct);           // Sin(theta) of target-quark-parton
        G4double qppt=qpen*qpst;                    // PT of target-quark-parton
        G4double qphi=twopi*G4UniformRand();        // Phi of target-quark-parton
        G4LorentzVector qi4m(qppt*sin(qphi),qppt*cos(qphi),qpen*qpct,qpen); // quark-parton
        G4LorentzVector qt4m=mu4m+qi4m;             // Total 4mom (iniQP+lepton)
        G4LorentzVector nu4m(0.,0.,0.,0.);          // Prototype of secondary neutrino 4mom
        G4LorentzVector qf4m(0.,0.,0.,0.);          // Prototype for secondary quark-parton
        G4QContent targQC=targQPDG.GetQuarkContent(); // QC of the target nucleus (local!)
        targQC+=G4QContent(1,0,0,0,1,0);      // Make iso-shift with fake pi- is added
        G4LorentzVector fn4m=G4LorentzVector(0.,0.,0.,0.); // Prototype of the residual 4M
        G4QNucleus fnN(targQC,fn4m);          // Define the final state nucleus
        G4double   fnm=fnN.GetMZNS();         // GS Mass of the final state nucleus
        //G4QContent resiQC=targQC-neutQC; // QC of resid nucleus (-neutron)
        //G4QNucleus rsN(resiQC,fn4m);          // Define the final state nucleus
        //G4double   rsm=rsN.GetMZNS()+mNeut;   // GS Mass of residual nucleus w/o neutron
        G4double   tm=0.;                     // Prototype of RealMass of the final nucleus
        G4LorentzVector tg4m=G4LorentzVector(0.,0.,0.,targM); // 4mom of all target nucleus
        G4LorentzVector fd4m=tg4m-qi4m;       // 4mom of the residual coloured nuclear sys.
#ifdef pdebug
        //G4cout<<">>>G4QEnv::Const:rM="<<rsm<<",fM="<<fnm<<",tM="<<targM<<G4endl;
        G4cout<<"G4QEnvironment::Const:mu4M="<<mu4m<<",t4M="<<qt4m<<",tgQP="<<qi4m<<G4endl;
#endif      
        while (tm<=fnm)
        {
          if(!G4QHadron(qt4m).DecayIn2(nu4m,qf4m))
          {
            G4cerr<<"***G4QE::Constr:Muon error (1) 4M="<<mu4m<<". Fill as it is."<<G4endl;
            G4QHadron* lepton = new G4QHadron(opHad); // Fill projMuon to Output
            theQHadrons.push_back(lepton);        // (delete equivalent)
            return;
          }
#ifdef mudebug
          G4cout<<"G4QEnv::Const:i="<<qi4m<<",t="<<qt4m<<"->n="<<nu4m<<"+q="<<qf4m<<G4endl;
#endif
          fn4m=fd4m+qf4m;
          tm=fn4m.m();                 // Real mass of the final nucleus
#ifdef mudebug
          G4cout<<"--G4QEnv::Const:M="<<tm<<",GSM=="<<fnm<<G4endl;
#endif
		      }
		      fnN.Set4Momentum(fn4m);
        // (mu,q->nu,q) reaction succeded and Neutrino can be pushed to Output
        G4QHadron* neutrino = 0;              // NeutrinoPrototype to be filled to Output
#ifdef mudebug
        G4cout<<"G4QEnv::Const:fM="<<tm<<fn4m<<",GSM="<<fnm<<G4endl;
#endif      
        if(tm<fnm)                            // Final Nucleus is below the GS threshold
		      {
          qf4m=G4LorentzVector(0.,0.,0.,fnm); // Final nucleus 4M for the final decay
          qt4m=tg4m+mu4m;
          if(!G4QHadron(qt4m).DecayIn2(nu4m,qf4m)) // Decay in Nucleus+nu_mu
          {
            G4cerr<<"***G4QE::Constr:Muon error (2) 4M="<<mu4m<<". Fill as it is."<<G4endl;
            G4QHadron* muon = new G4QHadron(opHad); // Fill projMuon to Output
            theQHadrons.push_back(muon);        // (delete equivalent)
            return;
          }
          G4QHadron* fnuc = new G4QHadron(targQC,qf4m); // Fill Final Nucleus to Output
          //theQHadrons.push_back(fnuc);      // (delete equivalent)
          EvaporateResidual(fnuc);            // Try to evaporate residual (del. equiv.)
          neutrino = new G4QHadron(nuPDG,nu4m);// Fill Neutrino to Output
          theEnvironment.InitByPDG(90000000); // Create nuclear environment
#ifdef pdebug
          G4cout<<"G4QEnv::Const:Fill neutrino (1) "<<nuPDG<<nu4m<<G4endl;
#endif      
          theQHadrons.push_back(neutrino);    // (delete equivalent)
          return;
        }
        neutrino = new G4QHadron(nuPDG,nu4m); // Fill Neutrino to Output
#ifdef pdebug
        G4cout<<"G4QEnv::Const:Fill neutrino (2) "<<nuPDG<<nu4m<<G4endl;
#endif      
        theQHadrons.push_back(neutrino);      // (delete equivalent)
        if(tm<fnm+135.98)                     // FinalNucleus is below thePionThreshold(HE)
	       {
          if(!fnN.SplitBaryon()) // Final Nucleus is below the splittingFragmentThreshold
		        {
#ifdef mudebug
            G4cout<<"G4QEnv::Const: impossible to split nucleon after mu->nu"<<G4endl;
#endif      
            G4LorentzVector ga4m(0.,0.,0.,0.);
            qf4m=G4LorentzVector(0.,0.,0.,fnm);// Final nucleus 4M for the final decay
            if(!G4QHadron(fn4m).DecayIn2(ga4m,qf4m)) // Decay in Nucleus+photon
            {
              G4cerr<<"***G4QE::Constr:LepCapError(3),M="<<fn4m.m()<<"<"<<fnm<<G4endl;
              G4QHadron* resid = new G4QHadron(targQC,qt4m); // Fill ResidNucleus to Output
              theQHadrons.push_back(resid);     // (delete equivalent)
              theEnvironment.InitByPDG(90000000);// Create nuclear environment
              return;
            }
            G4QHadron* photon = new G4QHadron(22,ga4m); // Fill projPhoton to Output
#ifdef pdebug
            G4cout<<"G4QEnv::Const:Fill photon "<<ga4m<<G4endl;
#endif      
            theQHadrons.push_back(photon);     // (delete equivalent)
            G4QHadron* fnuc = new G4QHadron(targQC,qf4m); // Fill Final Nucleus to Output
#ifdef pdebug
            G4cout<<"G4QEnv::Const:Fill target "<<targQC<<qf4m<<" in any form"<<G4endl;
#endif      
            EvaporateResidual(fnuc);            // Try to evaporate residual (del. equiv.)
            theEnvironment.InitByPDG(90000000);// Create nuclear environment
            return;
										}
	       }
        // At this poin it is possible to convert mu- to pi-
        fn4m=qf4m-qi4m;
        opHad->SetQPDG(pimQPDG);              //Convert (mu-)u->d to (virt pi-)u->d capture
        fake=false;                           // normal pi- for q-muon scattering
        //fake=true;                            // fake pi- for q-muon scattering *****
        //if(G4UniformRand()>.5) fake=false;    // normal pi- for q-muon scattering *****
        opHad->Set4Momentum(fn4m);
      }
    }
    for(G4int ih=0; ih<nHadrons; ih++)        // ==> The main LOOP over projQHadrons
    {
      G4QHadron* curHadr=projHadrons[ih];     // Pointer to current projectile Hadron
      G4int hNFrag = curHadr->GetNFragments();// #0 means intermediate (skip)
      G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4-momenyum of the current projectile
#ifdef pdebug
      G4cout<<"G4QE:C:"<<ih<<",F="<<hNFrag<<",0="<<projHadrons[0]->GetNFragments()<<G4endl;
#endif
      if(!hNFrag&&ch4M.e()>0.)                // => "Final hadron" case
	     {
        G4int envPDG=theEnvironment.GetPDG();
        if(envPDG==90000000||(theEnvironment.Get4Momentum().m2())<1.) // ==>"Vacuum"
        {
          G4int hPDG  = curHadr->GetPDGCode();// A PDG Code of the projQHadron
          //if(!hPDG||hPDG==10)        // Check for the validity of the QHadron (@@ 10 OK?)
          if(!hPDG)          // Check for the validity of the QHadron
          {
            //G4cerr<<"--Warning--G4QEnvironment::Constructor: wrong PDG("<<ih<<")="<<hPDG
            //    <<", HQC="<<curHadr->GetQC()<<", HM="<<curHadr->GetMass()<<G4endl;
            //throw G4QException("***G4QEnvironment::Constructor: theInputHadron is Chip");
          }
          else
          {
            G4int hQ = curHadr->GetQCode();  // One more check for valid of the QHadron
            if(hQ<0)
	           {
              //G4cerr<<"--Warning--G4QEnv::Constructor:Q<0, PDG=("<<ih<<")"<<hPDG<<G4endl;
              //throw G4QException("***G4QEnvironment::Constructor:theInputHadron is bad");
	           }
            else
            {
              G4QHadron* newHadr = new G4QHadron(curHadr);
#ifdef pdebug
              G4cout<<"*G4QE::Const:H="<<newHadr->GetQC()<<newHadr->Get4Momentum()<<G4endl;
#endif
              theQHadrons.push_back(newHadr); // Fill existing hadron (delete equivalent)
#ifdef pdebug
              G4cout<<"G4QEnviron::Constructor: Fill h="<<hPDG<<ch4M<<G4endl;
              for(unsigned ipo=0; ipo<theQHadrons.size(); ipo++) // LOOP just for printing
              {
                G4int           hPDG  = theQHadrons[ipo]->GetPDGCode();
                G4LorentzVector h4Mom = theQHadrons[ipo]->Get4Momentum();
                G4int           hNFrag= theQHadrons[ipo]->GetNFragments();
                G4QContent      hQC   = theQHadrons[ipo]->GetQC();
                G4cout<<"h#"<<ipo<<": "<<hPDG<<hQC<<",4M="<<h4Mom<<",nFr="<<hNFrag<<G4endl;
              }
#endif
            } // End of Q-Code check
          } // End of proper PDG for i-th Hadron
        }
        else                                  // Nuclear Environment still exists
		      {
          G4QContent      hQC   = curHadr->GetQC();
#ifdef pdebug
          G4cout<<"G4QE::Const:CreateQuasm, 4M="<<ch4M<<",QC="<<hQC<<",E="<<envPDG<<",tC="
                <<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
          CreateQuasmon(hQC, ch4M,fake);
		      } // End of Existing Nuclear Environment case
	     } // End of final hadron case
    } // End of the LOOP over input hadrons
  } // End of nuclear target case (including neutron=90000001 & proton=90001000)
  else                                        // => "Unique hadron" case
  {
    // the nuclEnviron is already init'ed as vacuum + get the first hadron for interaction
    G4QHadron* curHadr=projHadrons[0];        // Pointer to the firstProjecHadron (checked)
    G4int hPDG  = curHadr->GetPDGCode();      // A PDG Code of the projQHadron
    if(!hPDG||hPDG==10)                       // Check for the validity of the QHadron
    {
      G4cerr<<"---Warning---G4QEnvironment::Constructor:Vacuum,1st Hadron wrong PDG="<<hPDG
            <<", HQC="<<curHadr->GetQC()<<", HM="<<curHadr->GetMass()<<G4endl;
      //throw G4QException("***G4QEnvironment::Constructor: Fiest input Hadron is wrong");
    }
    else
    {
      G4int hQ = curHadr->GetQCode();         // One more check for valid of the QHadron
      if(hQ<0)
	     {
        G4cerr<<"---Warning---G4QEnviron::Constructor:Vacuum,Q<0, 1st HPDG="<<hPDG<<G4endl;
        //throw G4QException("***G4QEnvironment::Constructor:theFirstInputHadron's wrong");
	     }
      else                                // Now we can get 4Mom &  QC of incedent particle
      {
        G4LorentzVector h4Mom = curHadr->Get4Momentum();
        G4QContent      hQC   = curHadr->GetQC();
        if(!targPDG||targPDG==10) G4cout<<"G4QEnv::CreateQ; (1) PDG="<<targPDG<<G4endl;
        G4QPDGCode      tQPDG(targPDG);
        G4int           tQ    = tQPDG.GetQCode();
        if(tQ<0||targPDG==10)
		      {
          G4cerr<<"---Warning---G4QEnv::Constructor:TrgQC<0, Chipo?,PDG="<<targPDG<<G4endl;
          //throw G4QException("***G4QEnvironment::Constructor: Target is wrong");
		      }
        else                                 // Now we can create a unique Quasmon
		      {
          h4Mom+=G4LorentzVector(0.,0.,0.,tQPDG.GetMass()); //Projectile + TargetHadron
          hQC+=tQPDG.GetQuarkContent();
#ifdef pdebug
          G4cout<<"G4QEnv::Const:VacHadrTarg="<<h4Mom<<hQC<<",E="<<theEnvironment<<G4endl;
#endif
          G4Quasmon* curQuasmon = new G4Quasmon(hQC, h4Mom);
          theQuasmons.push_back(curQuasmon); // Insert Quasmon or hadron/gamma (del. eq.)
		      }
      } // End of Q-Code check
    } // End of proper PDG for i-th Hadron
    if(nHadrons>1) for(G4int ih=0; ih<nHadrons; ih++) // fill other Hadrons to Output
	   {
      G4QHadron* newHadr = new G4QHadron(curHadr);
#ifdef pdebug
      G4cout<<"*G4QE::Const:#"<<ih<<","<<curHadr->GetQC()<<curHadr->Get4Momentum()<<G4endl;
#endif
      theQHadrons.push_back(newHadr);        // Fill existing hadron (delete equivalent)
	   }
  } // End of Unique Hadron target treatment
#ifdef chdebug
  G4int finCharge=theEnvironment.GetCharge();
  G4int finBaryoN=theEnvironment.GetA();
  G4int nHad=theQHadrons.size();
  if(nHad) for(G4int ih=0; ih<nHad; ih++)
  {
    finCharge+=theQHadrons[ih]->GetCharge();
    finBaryoN+=theQHadrons[ih]->GetBaryonNumber();
  }
  G4int nQuas=theQuasmons.size();
  if(nQuas) for(G4int iq=0; iq<nQuas; iq++)
  {
    finCharge+=theQuasmons[iq]->GetCharge();
    finBaryoN+=theQuasmons[iq]->GetBaryonNumber();
  }
  if(finCharge!=totCharge || finBaryoN!=totBaryoN)
  {
    G4cerr<<"***G4QEnv::C:tC="<<totCharge<<",C="<<finCharge<<",tB="<<totBaryoN
          <<",B="<<finBaryoN<<",E="<<theEnvironment<<G4endl;
    if(nHad) for(G4int h=0; h<nHad; h++)
    {
      G4QHadron* cH = theQHadrons[h];
      G4cerr<<"::G4QE::C:h#"<<h<<",QC="<<cH->GetQC()<<",PDG="<<cH->GetPDGCode()<<G4endl;
    }
    if(nQuas) for(G4int q=0; q<nQuas; q++)
    {
      G4Quasmon* cQ = theQuasmons[q];
      G4cerr<<"::G4QE::C:q#"<<q<<",C="<<cQ->GetCharge()<<",QuarkCon="<<cQ->GetQC()<<G4endl;
    }
  }
#endif
} // End of the G4QEnvironment constructor

G4QEnvironment::G4QEnvironment(const G4QEnvironment &right)
{
  // theQHadrons (Vector)
  G4int nQH             = right.theQHadrons.size();
  if(nQH) for(G4int ih=0; ih<nQH; ih++)
  {
    G4QHadron* curQH    = new G4QHadron(right.theQHadrons[ih]);
#ifdef pdebug
    G4cout<<"G4QE::CopyByVal:cH#"<<ih<<","<<curQH->GetQC()<<curQH->Get4Momentum()<<G4endl;
#endif
    theQHadrons.push_back(curQH);            // (delete equivalent)
  }
  // theProjectiles (Vector)
  G4int nP              = right.theProjectiles.size();
  if(nP) for(G4int ip=0; ip<nP; ip++)
  {
    G4QHadron* curP     = new G4QHadron(right.theProjectiles[ip]);
    theProjectiles.push_back(curP);           // (delete equivalent)
  }
  theTargetPDG          = right.theTargetPDG;
  theWorld              = right.theWorld;
  nBarClust             = right.nBarClust;
  f2all                 = right.f2all;
  tot4Mom               = right.tot4Mom;
  totCharge             = right.totCharge;
  totBaryoN             = right.totBaryoN;

  // theQuasmons (Vector)
  G4int nQ              = right.theQuasmons.size();
  if(nQ) for(G4int iq=0; iq<nQ; iq++)
  {
    G4Quasmon* curQ     = new G4Quasmon(right.theQuasmons[iq]);
    theQuasmons.push_back(curQ);             // (delete equivalent)
  }

  // theQCandidates (Vector)
  G4int nQC             = right.theQCandidates.size();
  if(nQC) for(G4int ic=0; ic<nQC; ic++)
  {
    G4QCandidate* curQC = new G4QCandidate(right.theQCandidates[ic]);
    theQCandidates.push_back(curQC);         // (delete equivalent)
  }

  theEnvironment        = right.theEnvironment;
}

G4QEnvironment::G4QEnvironment(G4QEnvironment* right)
{
  // theQHadrons (Vector)
  G4int nQH             = right->theQHadrons.size();
  if(nQH) for(G4int ih=0; ih<nQH; ih++)
  {
    G4QHadron* curQH    = new G4QHadron(right->theQHadrons[ih]);
#ifdef pdebug
    G4cout<<"G4QE::CopyByPtr:cH#"<<ih<<","<<curQH->GetQC()<<curQH->Get4Momentum()<<G4endl;
#endif
    theQHadrons.push_back(curQH);            // (delete equivalent)
  }

  // theProjectiles (Vector)
  G4int nP              = right->theProjectiles.size();
  if(nP) for(G4int ip=0; ip<nP; ip++)
  {
    G4QHadron* curP     = new G4QHadron(right->theProjectiles[ip]);
    theProjectiles.push_back(curP);           // (delete equivalent)
  }
  theTargetPDG          = right->theTargetPDG;
  theWorld              = right->theWorld;
  nBarClust             = right->nBarClust;
  f2all                 = right->f2all;
  tot4Mom               = right->tot4Mom;
  totCharge             = right->totCharge;
  totBaryoN             = right->totBaryoN;

  // theQuasmons (Vector)
  G4int nQ              = right->theQuasmons.size();
  if(nQ) for(G4int iq=0; iq<nQ; iq++)
  {
    G4Quasmon* curQ     = new G4Quasmon(right->theQuasmons[iq]);
    theQuasmons.push_back(curQ);             // (delete equivalent)
  }

  // theQCandidates (Vector)
  G4int nQC             = right->theQCandidates.size();
  if(nQC) for(G4int ic=0; ic<nQC; ic++)
  {
    G4QCandidate* curQC = new G4QCandidate(right->theQCandidates[ic]);
    theQCandidates.push_back(curQC);         // (delete equivalent)
  }

  theEnvironment        = right->theEnvironment;
}

G4QEnvironment::~G4QEnvironment()
{
#ifdef debug
  G4cout<<"~G4QEnvironment: before theQCandidates nC="<<theQCandidates.size()<<G4endl;
#endif
  for_each(theQCandidates.begin(), theQCandidates.end(), DeleteQCandidate());
#ifdef debug
  G4cout<<"~G4QEnvironment: before theQuasmons nQ="<<theQuasmons.size()<<G4endl;
#endif
  for_each(theQuasmons.begin(), theQuasmons.end(), DeleteQuasmon());
#ifdef debug
  G4cout<<"~G4QEnvironment: before theQHadrons nH="<<theQHadrons.size()<<G4endl;
#endif
  for_each(theQHadrons.begin(), theQHadrons.end(), DeleteQHadron());
#ifdef debug
  G4cout<<"~G4QEnvironment: before theProjectiles nP="<<theProjectiles.size()<<G4endl;
#endif
  for_each(theProjectiles.begin(), theProjectiles.end(), DeleteQHadron());
#ifdef debug
  G4cout<<"~G4QEnvironment: === DONE ==="<<G4endl;
#endif
}

G4double G4QEnvironment::SolidAngle=0.8;     // Part of Solid Angle to capture (@@A-dep.)
G4bool   G4QEnvironment::EnergyFlux=false;   // Flag for Energy Flux use (not MultyQuasmon)
G4bool   G4QEnvironment::WeakDecays=false;   // Flag for WeakDecays(notUsed hadwaredClosed)
G4bool   G4QEnvironment::ElMaDecays=true;    // Flag for Electromagnetic decays of hadrons
G4double G4QEnvironment::PiPrThresh=141.4;   // Pion Production Threshold for gammas
G4double G4QEnvironment::M2ShiftVir=20000.;  // Shift for M2=-Q2=m_pi^2 of the virtualGamma
G4double G4QEnvironment::DiNuclMass=1880.;   // DoubleNucleon Mass for VirtualNormalization

// Open decay of particles with possible electromagnetic channels of decay (gammas)
void G4QEnvironment::OpenElectromagneticDecays(){ElMaDecays=true;}

// Close decay of particles with possible electromagnetic channels of decay (gammas)
void G4QEnvironment::CloseElectromagneticDecays(){ElMaDecays=false;}

// Fill the private static parameters
void G4QEnvironment::SetParameters(G4double solAn, G4bool efFlag, G4double piThresh,
                                   G4double mpisq, G4double dinum)
{//  ======================================================================================
  EnergyFlux=efFlag;       // Flag for Energy Flux use instead of Multy Quasmon
  SolidAngle=solAn;        // Part of Solid Angle to capture secondaries (@@A-dep)
  PiPrThresh=piThresh;     // Pion Production Threshold for gammas
  M2ShiftVir=mpisq;        // Shift for M2=-Q2=m_pi^2 of the virtual gamma
  DiNuclMass=dinum;        // Double Nucleon Mass for virtual normalization
}

const G4QEnvironment& G4QEnvironment::operator=(const G4QEnvironment &right)
{// ========================================================================
  if(this != &right)                          // Beware of self assignment
  {
    // theQHadrons (Vector)
    G4int iQH             = theQHadrons.size();
    if(iQH) for(G4int ii=0; ii<iQH; ii++) delete theQHadrons[ii];
    theQHadrons.clear();
    G4int nQH             = right.theQHadrons.size();
    if(nQH) for(G4int ih=0; ih<nQH; ih++)
    {
      G4QHadron* curQH    = new G4QHadron(right.theQHadrons[ih]);
#ifdef pdebug
      G4cout<<"G4QE::Operator=:c#"<<ih<<","<<curQH->GetQC()<<curQH->Get4Momentum()<<G4endl;
#endif
      theQHadrons.push_back(curQH);            // (delete equivalent)
    }

    theWorld              = right.theWorld;
    nBarClust             = right.nBarClust;
    f2all                 = right.f2all;

    // theQuasmons (Vector)
    G4int iQ              = theQuasmons.size();
    if(iQ) for(G4int jq=0; jq<iQ; jq++) delete theQuasmons[jq];
    theQuasmons.clear();
    G4int nQ              = right.theQuasmons.size();
    if(nQ) for(G4int iq=0; iq<nQ; iq++)
    {
      G4Quasmon* curQ     = new G4Quasmon(right.theQuasmons[iq]);
      theQuasmons.push_back(curQ);             // (delete equivalent)
    }

    // theQCandidates (Vector)
    G4int iQC             = theQCandidates.size();
    if(iQC) for(G4int jc=0; jc<iQC; jc++) delete theQCandidates[jc];
    theQCandidates.clear();
    G4int nQC             = right.theQCandidates.size();
    if(nQC) for(G4int ic=0; ic<nQC; ic++)
    {
      G4QCandidate* curQC = new G4QCandidate(right.theQCandidates[ic]);
      theQCandidates.push_back(curQC);        // (delete equivalent)
    }

    theEnvironment        = right.theEnvironment;
  }
  return *this;
}

// Member function for Quasmon Creation & Environment nucleus modification
void G4QEnvironment::CreateQuasmon(const G4QContent& projQC, const G4LorentzVector& pro4M,
                                   G4bool fake)
{//=======================================================================================
  static const G4double third=1./3.;
  //static const G4double mNeut= G4QPDGCode(2112).GetMass();
  //static const G4double mProt= G4QPDGCode(2212).GetMass();
  //static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mPi2 = mPi*mPi;
  //static const G4double mMu  = G4QPDGCode(13).GetMass();
  //static const G4double mMu2 = mMu*mMu;
  //static const G4QContent gamQC(0,0,0,0,0,0);
  //static const G4QContent pimQC(1,0,0,0,1,0);
  //static const G4QContent pipQC(0,1,0,1,0,0);
  static const G4QContent neutQC(2,1,0,0,0,0);
  static const G4QContent protQC(1,2,0,0,0,0);
  static const G4QContent deutQC(3,3,0,0,0,0);
  static const G4QContent lambQC(1,1,1,0,0,0);
  static const G4QNucleus vacuum(90000000);
  G4QContent valQ(0,0,0,0,0,0);             // Prototype of the Quasmon's Quark Content
  G4LorentzVector q4Mom(0.,0.,0.,0.);       // Prototype of the Quasmon's 4-momentum
  nBarClust = 1;                            // By default only quasi-free nucleons
  G4LorentzVector proj4M=pro4M;             // Fake equivalence to avoid & const
  G4double  projE=proj4M.e();               // energy of the projectile
  G4int projPDG=projQC.GetSPDGCode();     // Minimum hadron for the projectile QC
  if(projE<0.)
  {
    G4cout<<"*Warning*G4QEnvironment::CreateQuasmon:Epr="<<projE<<"<0,QC="<<projQC<<G4endl;
    projE=0.;
    proj4M=G4LorentzVector(0.,0.,0.,0.);
  }
  G4double  projM2=proj4M.m2();             // projectile's squared mass (print & v.gamma)
  G4int     targPDG=theEnvironment.GetPDG();// PDG Code of the target nucleus
  if(targPDG>80000000&&targPDG!=90000000&&(theEnvironment.Get4Momentum().m2())>1.)//Nucleus
  {
    G4double  tgMass=theEnvironment.GetMass();// mass of the target (QEnvironment) nucleus
#ifdef pdebug
    G4cout<<"G4QEnvironment::CreateQ:Interact "<<projQC<<proj4M<<"(m2="<<projM2<<") + A="
          <<targPDG<<",M="<<tgMass<<",tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
    G4int envZ=theEnvironment.GetZ();       // A#of protons in the target nucleus
    G4int envN=theEnvironment.GetN();       // A#of neutrons in the target nucleus
    G4int envS=theEnvironment.GetS();       // A#of lambdas in the target nucleus
    G4int envBN=envZ+envN+envS;             // A baryon number of the target nucleus
    G4int nP  =theWorld->GetQPEntries();    // A#of initialized particles in CHIPS World
    G4int nCl =nP-90;                       // A#of initialized clusters in CHIPS World
    if(nCl<0) G4cerr<<"---Warning---G4QEnv::CreaQ:nP="<<nP<<" for Targ="<<targPDG<<G4endl;
    if     (nCl<3) nBarClust=1;             // Fix the maximum Baryon Number for clusters
    else if(nCl<9) nBarClust=2;
    else
	   {
      G4int v=nCl-9;
      G4int d=v/15;
      G4int r=v%15;
      if(r<7) nBarClust=3+d+d;
      else    nBarClust=4+d+d;
    }
#ifdef pdebug
	   G4cout<<"G4QE::CrQ:TNuc:Z="<<envZ<<",N="<<envN<<",nC="<<nBarClust<<",tC="
          <<totCharge<<", tB="<<totBaryoN<<G4endl;
#endif
    G4bool pbpt=projE<PiPrThresh+(M2ShiftVir+projM2)/DiNuclMass;// PhotonBelowPionThreshold
    G4bool din=false;
    G4bool piF=false;
    G4bool gaF=false;
    if((projM2-mPi2<.00001||projE-mPi<0.1)&&projPDG==-211&&!fake) piF=true;//PiAtRestCase
    //if(pbpt&&projPDG==22) din=true; // InCaseOf GammaBelowPiThresh needs DiNucl (?)
    if(pbpt&&projPDG==22) gaF=true; // InCaseOf GammaBelowPiThresh needs DiNucl (?)
    theEnvironment.SetMaxClust(nBarClust);
    nBarClust=theEnvironment.UpdateClusters(din); // Cluster Probabilities upto maxClust
#ifdef pdebug
	   G4cout<<"G4QEnv::CreateQ: Nucleus("<<targPDG<<") is created ("<<nBarClust<<" clast's)";
    for(G4int ic=0;ic<nBarClust;ic++)
      G4cout<<" #"<<ic<<"("<<theEnvironment.GetProbability(ic)<<")";
    G4cout<<G4endl;
#endif
    theEnvironment.PrepareCandidates(theQCandidates,piF,gaF,proj4M);//Calc.Clust's probab's
#ifdef pdebug
	   G4cout<<"G4QE::CrQ:ClusterProbabCalculation tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;
#endif
    G4bool efFlag=false;                    // EnergyFlowFlag=FALSE (@@=DEFOLT=@@ make par)
    // ********** Change if necessary to compare Energy Flux & Multy Quasmon **************
    G4int efCounter=0;                      // Counter of Energy Flux particles
    G4QContent EnFlQC(0,0,0,0,0,0);         // Quark Content of Energy Flux
    G4LorentzVector ef4Mom(0.,0.,0.,0.);    // Summed 4-momentum of Energy Flux
    G4double proj3M=proj4M.rho();
    //   ---   Pbar     ---    Nbar  -------  LAMBDAbar  ----  SIGMA-bar  ---  SIGMA0bar
    if((projPDG==-2212||projPDG==-2112||projPDG==-3122||projPDG==-3112||projPDG==-3212||
        projPDG==-3222) && envBN>1 && proj3M<10.) // OnlyForAtRestReactions(@@to Interface)
	   //  ^^^^^  SIGMA+bar
	   {
      // @@ Annihilation on one baryon is implemented (no annihilation on clusters! @@?) @@
#ifdef pdebug
      G4cout<<"G4QE::CreQ:Annihilation on a perif. nucleon, Z="<<envZ<<",N="<<envN<<G4endl;
#endif
      G4double   zpn=envZ+envN;             // a#of nucleons in the nucleus
      G4double   rnd=zpn*G4UniformRand();   // Random number to find a nucleon
      //G4double   envD=.1*envZ*envN/zpn;   // a#of possible quasifree deuterons (@@Param.)
      //G4double   rnd=(zpn+envD)*G4UniformRand(); // Random number to find a baryon
      //G4double   rnd=(zpn+envS)*G4UniformRand(); // Random number to find a baryon
      G4int      targNPDG = 90000000;       // Nucl-Prototype of PDG of Periferal Target
      G4QContent targQC(0,0,0,0,0,0);       // Quark Content of Periferal Target
      if     (rnd<envN)                     // Neutron is a Periferal Target
      {
        targNPDG = 90000001;
        targQC   = neutQC;
	     }
      else
						//     if(rnd<=zpn)                      // Proton is a Periferal Target
      {
        targNPDG = 90001000;
        targQC   = protQC;
	     }
      //else                                  // Deuteron as a Periferal Target
      //{
      //  targNPDG = 90001001;
      //  targQC   = deutQC;
	     //}
      //else                                  // Lambda is a Periferal Target (?)
      //{
      //  targNPDG = 91000000;
      //  targQC   = lambQC;
	     //}
      theEnvironment.Reduce(targNPDG);      // Subtract periferal baryon from Nucleus
#ifdef pdebug
      G4cout<<"G4QEnvironment::CQ:"<<targNPDG<<" is selected Env="<<theEnvironment<<G4endl;
#endif
      G4double resMass=theEnvironment.GetGSMass(); // Nuclear mass after baryon subtraction
      G4double barMass=tgMass-resMass;      // Mass of the bound baryon for annihilation
      tgMass=resMass;                       // New mass of theEnvironment
      q4Mom=G4LorentzVector(0,0,0,barMass)+proj4M;// 4-mom of the intermediate B-Bbar Quasm
      valQ=targQC+projQC;                   // Quark Content of intermediate B-Bbar Quasmon
      // Remember the Quasmon parameters, defined by user for recovery after annihilation
      G4Quasmon fakeQ;                      // fake Quasmon to get and restore parameters
      G4double QTemper=fakeQ.GetTemper();   // Temperature defined by user for Quasmons
      G4double QSOverU=fakeQ.GetSOverU();   // S/U defined by user for Quasmons
      G4double QEtaSup=fakeQ.GetEtaSup();   // Eta Suppresion defined by user in Quasmons
      G4Quasmon::SetParameters(180.,QSOverU,.3); // Parameters for N-barN Annihilation
      G4Quasmon::CloseElectromagneticDecays(); // Parameters for N-barN Annihilation
      G4Quasmon* pan = new G4Quasmon(valQ,q4Mom);// N-Nbar Quasm creation (del.at 9th line)
      G4QNucleus vE(90000000);                 // Annihilation in vacuum (in NuclMatter?)
#ifdef pdebug
      G4cout<<"G4QE::CreQ: before Fragment vE="<<vE<<",QQC="<<valQ<<",Q4M="<<q4Mom<<G4endl;
#endif
      G4QHadronVector* output=pan->Fragment(vE,1);//Output of inVacAnnihilation*DESTROY*<-+
#ifdef pdebug
      G4cout<<"G4QE::CrQ:NucleonAntinucleonAnnihilation's done,N="<<output->size()<<G4endl;
#endif
      G4Quasmon::OpenElectromagneticDecays();  // Parameter for multihadronFragmentatation^
#ifdef pdebug
	     G4cout<<"G4QE::CrQ:>>AnnihilationIsDone,C="<<totCharge<<",B="<<totBaryoN<<G4endl;// ^
#endif
      delete pan;                              // The N-NbarQuasmon is deleted A.S.A.P.   ^
      G4QHadronVector input;                   // Input for MultyQuasmon **DESTROY**<---+ ^
      //G4int trgPDG = theEnvironment.GetPDG();// New PDG Code for the Residual Nucleus ^ ^
      G4LorentzVector trg4M(0.,0.,0.,resMass); // New 4-momentum for the ResidualNucleus^ ^
      G4int tNH = output->size();              // For the selection LOOP                ^ ^
      G4ThreeVector dir = RndmDir();           // For the selection in LOOP (@@ at rest)^ ^
#ifdef pdebug
	     G4cout<<"G4QE::CrQ:AnihN="<<tNH<<", tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;// ^ ^
#endif
      G4double ra=std::pow(G4double(totBaryoN),third);  //                              ^ ^
      for (G4int ind=0; ind<tNH; ind++)        // Loop over annihilation  QHadrons      ^ ^
      {
        //G4QHadron* curHadr = output->operator[](ind); // Pointer to theCurrentHadron  ^ ^
        G4QHadron*   curHadr = (*output)[ind];          // Pointer to theCurrentHadron  ^ ^
        G4int           shDFL= curHadr->GetNFragments();// A#of decFragments for proj.  ^ ^
        G4LorentzVector sh4m = curHadr->Get4Momentum(); // 4Mom for the projectile      ^ ^
        G4ThreeVector   shDIR= sh4m.vect().unit();      // unitVector in projMomDirect  ^ ^
        G4int           shPDG= curHadr->GetPDGCode();   // PDG Code of the projectile   ^ ^
        G4int           shCHG= curHadr->GetCharge();    // Charge of the projectile     ^ ^
        G4double        shMOM= sh4m.rho();              // Momentum of the projectile   ^ ^
#ifdef pdebug
		      G4cout<<"G4QE::CrQ:"<<ind<<","<<shDFL<<",PDG="<<curHadr->GetPDGCode() //        ^ ^
              <<",4M="<<curHadr->Get4Momentum()<<G4endl;//                              ^ ^
#endif
        G4double solAnCut=SolidAngle+1000*shCHG/shMOM/ra;// ChargeDepSolAngle           ^ ^
        //G4double solAnCut=SolidAngle+20*shCHG*sqrt(1.*envZ)/shMOM;//ChargeDepSolAngle ^ ^
#ifdef pdebug
		      G4cout<<"G4QE::CrQ: PDG="<<curHadr->GetPDGCode()<<", p="<<shMOM<<", r="<<ra //  ^ ^
              <<",d="<<1500*shCHG/shMOM/ra<<G4endl;     //                              ^ ^
#endif
        if(!shDFL)                                      // Final(notDecayed) hadrons    ^ ^
		      {
#ifdef pdebug
		        G4cout<<"G4QE::CQ:>H="<<shPDG<<":"<<dir.dot(shDIR)<<">"<<solAnCut<<G4endl; // ^ ^
#endif
		        //if((dir.dot(shDIR)>solAnCut||shMOM<120.) && abs(shPDG)>99) // Absorb mesons ^ ^
		        if(dir.dot(shDIR)>solAnCut && abs(shPDG)>99) // Absorb mesons                 ^ ^
		        {
#ifdef pdebug
												G4cout<<"G4QE::CQ:>H="<<shPDG<<":"<<dir.dot(shDIR)<<">"<<solAnCut<<", P="// ^ ^
																		<<shMOM<<" < 120"<<G4endl;                           //               ^ ^
#endif
            if(efFlag)                           // => Case of "Energy Flux approach"   ^ ^
		          {
              G4QContent shQC = curHadr->GetQC();// QuarkContent of the Current Hadron  ^ ^
              ef4Mom+=sh4m;
              EnFlQC+=shQC;
              efCounter++;
#ifdef pdebug
              G4int hPDG=curHadr->GetPDGCode();    // Only for gebug printing           ^ ^
              G4LorentzVector h4M = curHadr->Get4Momentum();  // Only for gebug printing^ ^
			           G4cout<<"G4QE::CrQ:#"<<efCounter<<", PDG="<<hPDG<<", h4M="<<h4M<<G4endl;//^ ^
#endif
		          }
		          else                                   //=>"MultyQuasFragmentation"(!efFlag)^ ^
		          {
              G4QHadron* mqHadron = new G4QHadron(curHadr);
              input.push_back(mqHadron);           // Fill hadron-copy (del equiv)  <...^ ^
#ifdef pdebug
              G4int hPDG=curHadr->GetPDGCode();    // Only for debug printing           ^ ^
              G4LorentzVector h4M = curHadr->Get4Momentum(); // Only for gebug printing ^ ^
		            G4cout<<"G4QE::CrQ:Absorb#"<<ind<<", PDG="<<hPDG<<", h4M="<<h4M<<G4endl;//^ ^
#endif
		          }                                      //                                   ^ ^
		        }                                        //                                   ^ ^
		        else                                     // DirectFilling of the output vector^ ^
		        {                                        //                                   ^ ^
#ifdef pdebug
            G4int hPDG=curHadr->GetPDGCode();      // Only for gebug printing           ^ ^
            G4LorentzVector h4M = curHadr->Get4Momentum(); // Only for gebug printing   ^ ^
		          G4cout<<"G4QE::CrQ: Fill OUT #"<<ind<<",PDG="<<hPDG<<",h4M="<<h4M<<G4endl;//^ ^
#endif
            // Just fill a hadron to the output stack (One should make EM decays elsewhere)
            G4QHadron* curHadron = new G4QHadron(curHadr); //                           ^ ^
            theQHadrons.push_back(curHadron);      // TheQHadrs are filled as new Hadrs ^ ^
          }
		      } // End of the IF over projectiles                                             ^ ^
	     } // End of LOOP over "output" of annihilation                                    ^ ^
      for_each(output->begin(), output->end(), DeleteQHadron());     //DESTRUCT output>-^-^
      output->clear();                             //                                   ^ ^
      delete output;                               // ==================================^=^
      if(!efFlag)                                  // =>NotEnergyFlux=MultyQuasmon Case ^
	     {
        G4int noh = theQHadrons.size();            // a#oh hadrons in Output UpToNow    ^
#ifdef pdebug
        G4cout<<"G4QEnv::CreateQ:*******#ofH="<<noh<<" is found *******"<<G4endl; //    ^
#endif
        if(noh) for(G4int kh=0; kh<noh; kh++)      // One can escape it but...          ^
								{                                          //                                   ^
#ifdef pdebug
          G4cout<<"G4QE::CreateQ:H#"<<kh<<", QC="<<theQHadrons[kh]->GetQC() //          ^
                <<", 4M="<<theQHadrons[kh]->Get4Momentum()<<G4endl;         //          ^
#endif
          G4QHadronVector* tmpQHadVec=G4Quasmon().DecayQHadron(theQHadrons[kh]);//d.e<^ ^
          G4int tmpS=tmpQHadVec->size();           //                                 ^ ^
          intQHadrons.resize(tmpS+intQHadrons.size()); // Resize theQHadrons length   ^ ^
          copy(tmpQHadVec->begin(), tmpQHadVec->end(), intQHadrons.end()-tmpS); //    ^ ^
          tmpQHadVec->clear();                     //                                 ^ ^
          delete tmpQHadVec;           // who calls DecayQHadron must clear & delete  ^ ^
		      }                                          //                                 ^ ^
        theQHadrons.clear(); // deletedWhenDecayed // Now theQHadrons is EmptyVector->^ ^
#ifdef pdebug
        G4int nInH=intQHadrons.size();             // Resulting #of hadrons after decay ^
	       G4cout<<"G4QE::CrQ:nH="<<nInH<<",C="<<totCharge<<",B="<<totBaryoN<<G4endl;//    ^
#endif
        if(!(input.size()))                        // *RETURN* Without Quasmon creation-^
		      {                                          //                                   ^
#ifdef pdebug
	         G4cout<<"*G4QEnv::CrQ:AnnihStack tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;//^
#endif
		        return;                                  // Do not clear and delete objects --^
        }                                          //                                   ^
#ifdef pdebug
	       G4cout<<"G4QE::CrQ:fakeQ, restPars tC="<<totCharge<<",tB="<<totBaryoN<<G4endl;//^
#endif
        G4Quasmon::SetParameters(QTemper,QSOverU,QEtaSup);//RecoverQParam's after anihil^
        G4Quasmon::OpenElectromagneticDecays(); // Parameter for multihadron fragmentat.^
        // From this point the new temporary environment is created (multiQuasmon)      ^
        G4QEnvironment* muq = new G4QEnvironment(input,theEnvironment.GetPDG());//<--+  ^
#ifdef pdebug
	       G4cout<<"G4QE::CrQ:befCl&Dest tC="<<totCharge<<", tB="<<totBaryoN<<G4endl; //^  ^
#endif
        for_each(input.begin(), input.end(), DeleteQHadron());     //DESTROING inp >-^--^
        input.clear();                             // =Clearing=>==========>=========^==+
        theEnvironment = muq->GetEnvironment();    // RestoreResidEnv after interact.^  
        G4QHadronVector* outH = muq->GetQHadrons();// Copy of QHadrons *DESTROY* <---^-<--+
        G4QuasmonVector* outQ = muq->GetQuasmons();// Copy of Quasmons *DESTROY* <---^--+ ^
        delete muq;                                //=====>===========>==============^  ^ ^
        noh = outH->size();                        // a#of Not Interacting(Q) Hadrons   ^ ^
#ifdef pdebug
        G4cout<<"G4QEnv::CreateQ:*** #ofNotInterQH="<<noh<<" is found ***"<<G4endl; //  ^ ^
#endif
        if(noh) for(G4int nh=0; nh<noh; nh++)      // One can escape it but...          ^ ^
								{                                          //                                   ^ ^
#ifdef pdebug
          G4cout<<"G4QE::CreateQ: NotIntQH#"<<nh<<", QC="<<(*outH)[nh]->GetQC()  //     ^ ^
                <<", 4M="<<(*outH)[nh]->Get4Momentum()<<G4endl;                  //     ^ ^
#endif
          G4QHadronVector* tmpQHadVec=G4Quasmon().DecayQHadron((*outH)[nh]);//del.eq<-+ ^ ^
          G4int tmpS=tmpQHadVec->size();           //                                 ^ ^ ^
          intQHadrons.resize(tmpS+intQHadrons.size()); // Resize theQHadrons length   ^ ^ ^
          copy(tmpQHadVec->begin(), tmpQHadVec->end(), intQHadrons.end()-tmpS); //    ^ ^ ^
          tmpQHadVec->clear();                     //                                 ^ ^ ^
          delete tmpQHadVec;           // who calls DecayQHadron must clear & delete->+ ^ ^
		      }                                          //                                   ^ ^
        outH->clear();                             //                                   ^ ^
        delete outH;                               // >---->---->---->---->---->---->---^-+
	       G4int nMQ = outQ->size();                  // A#ofQuasmons in MultyQuasmonOutput^
#ifdef pdebug
        G4LorentzVector eLorV=theEnvironment.Get4Momentum(); //                         ^
	       G4cout<<"G4QE::CrQ:nMQ="<<nMQ<<",tC="<<totCharge<<", tB="<<totBaryoN<<G4endl;// ^
	       G4cout<<"G4QE::CrQ:Env4M="<<eLorV<<G4endl; //                                   ^
        G4LorentzVector contr4M=eLorV; //                                               ^
#endif
        if(nMQ) for(G4int mh=0; mh<nMQ; mh++)      // Can escape CreationDistruct but...^
								{                                          //                                   ^
          G4Quasmon* curQ = new G4Quasmon((*outQ)[mh]);// Copy to destroy TMP(?)        ^
#ifdef pdebug
          G4LorentzVector qLorV=curQ->Get4Momentum(); //                                ^
	         G4cout<<"G4QE::CrQ:Q#"<<mh<<",4M="<<qLorV<<curQ->GetQC()<<G4endl; //          ^
          contr4M+=qLorV; //                                                            ^
#endif
          theQuasmons.push_back(curQ);             // Fill QuasmonCopies in theQuasmons ^
	       }                                          //                                   ^
        for_each(outQ->begin(), outQ->end(), DeleteQuasmon()); // >---------------------^
        outQ->clear();                             //                                   ^
        delete outQ;                               // >=================================+
#ifdef pdebug
        G4int nsHadr  = theQHadrons.size();      // Update the value of OUTPUT entries
	       G4cout<<"G4QEnvironment::CreateQ: before return nH="<<nsHadr<<G4endl;
        if(nsHadr) for(G4int jso=0; jso<nsHadr; jso++)// LOOP over output hadrons 
        {
          G4int hsNF  = theQHadrons[jso]->GetNFragments(); // A#of secondary fragments
          if(!hsNF)                                        // Add only final hadrons
										{
            G4LorentzVector hLorV=theQHadrons[jso]->Get4Momentum();
            G4int           hPDGC=theQHadrons[jso]->GetPDGCode();
          		G4cout<<"G4QE::CrQ: H#"<<jso<<",4M="<<hLorV<<hPDGC<<G4endl;
            contr4M+=hLorV;
          }
          else
            G4cout<<"G4Q::CrQ:"<<jso<<"NF=0,4M="<<theQHadrons[jso]->Get4Momentum()<<G4endl;
        }
	       G4cout<<"G4QEnvironment::CreateQ: before return tot4M="<<contr4M<<G4endl;
#endif
        return;                                    // *** RETURN *** 
      }
      else                                         // ==> Energy Flux case
	     {
        if (!efCounter) return;                    // ***RETURN*** Without Quasmon creation
	     }
	   }                                              // End of Hyperon annihilation case
    else EnFlQC=projQC;                            // For notAntiBar, don't use EnergyFlux
    G4double EnFlP=ef4Mom.rho();                   // Mom. of EnergyFlow forClusterCreation
    PrepareInteractionProbabilities(EnFlQC,EnFlP); // InteractionProbabilities for clusters
#ifdef pdebug
	   G4cout<<"G4QEnvironment::CreateQ: Interaction Probabilities are calculated"<<G4endl;
#endif
    G4int nCandid = theQCandidates.size();
    G4double maxP = theQCandidates[nCandid-1]->GetIntegProbability();
    G4int i=0;
    G4QContent    curQC;                           // Quark Content of the selected cluster
    if(nCandid<=0)
	   {
	     G4cerr<<"---Warning---G4QEnv::CreaQ:nC="<<nCandid<<",maxP="<<maxP<<",E="
            <<theEnvironment<<G4endl;
      //throw G4QException("G4QEnvironment::CreateQ: Can not select a cluster");
	   }
    if(nCandid==1||maxP==0.)
	   {
#ifdef pdebug
	     G4cout<<"***G4QEnv::CrQ:MaxP=0||nCand=1: Use all Env., Env="<<theEnvironment<<G4endl;
#endif
      curQC=theEnvironment.GetQCZNS();
      theEnvironment=vacuum;
    }
    else
    {
      G4double totP = maxP * G4UniformRand();
#ifdef pdebug
	     G4cout<<"G4QEnvironment::CrQ:nC="<<nCandid<<", maxP="<<maxP<<", totP="<<totP<<G4endl;
#endif
      while(theQCandidates[i]->GetIntegProbability()<totP) i++;
      G4QCandidate* curCand = theQCandidates[i];// Pointer to selected cluster to interact
      curQC   = curCand->GetQC();               // Get QuarkContent of the selected cluster
      G4QNucleus targClust(curQC.GetP(),curQC.GetN(),curQC.GetL());//Define Clust as a QNuc
      G4double clMass=targClust.GetGSMass();    // Mass of residual nuclear environment
#ifdef cldebug
	     G4cout<<"G4QEnv::CrQ:Cl#"<<i<<"(of "<<nCandid<<"),QC="<<curQC<<",M="<<clMass<<G4endl;
#endif
      G4LorentzVector pq4M=proj4M+G4LorentzVector(0.,0.,0.,clMass); 
      if(pq4M.m()>=clMass)
	     {
#ifdef pdebug
	       G4cout<<"G4QEnv::CQ:#"<<i<<"("<<targClust<<curQC<<") Env="<<theEnvironment<<G4endl;
#endif
        theEnvironment.Reduce(targClust.GetPDG());// Subtract selected cluster from Nucleus
	     }
      else
	     {
        G4double teMass=theEnvironment.GetGSMass(); //Mass of theResidualNuclearEnvironment
        G4LorentzVector te4M=proj4M+G4LorentzVector(0.,0.,0.,teMass);
        if(te4M.m()>=teMass)
	       {
#ifdef pdebug
	         G4cout<<"***G4QEnv::CrQ: Deep virtual, use all Env,Env="<<theEnvironment<<G4endl;
#endif
          curQC=theEnvironment.GetQCZNS();
          theEnvironment=vacuum;
        }
        else
		      {
          G4QHadron* projH = new G4QHadron(projQC,proj4M);
          theQHadrons.push_back(projH);
	         G4cerr<<"---Warning---G4QE::CrQ:Fill Proj asItIs QC/4m="<<projQC<<proj4M<<G4endl;
          return;
        }
	     }
	   }
    G4double envMass=theEnvironment.GetGSMass();   // Mass of residual nuclear environment
    if(projPDG==22&&projE<PiPrThresh+(M2ShiftVir+projM2)/DiNuclMass) // Gam+quark Interact
    //if(2>3)                                      //@@ TMP:PhotoAbsorbtion by q is closed
	   {
      q4Mom=G4LorentzVector(0.,0.,0.,tgMass-envMass);// PhotoInteracts with BoundedCluster
      valQ=curQC;
#ifdef pdebug
      G4cout<<"G4QE::CrQ:Q="<<q4Mom<<valQ<<"+vg="<<proj4M<<",Env="<<theEnvironment<<G4endl;
#endif
      G4Quasmon* curQuasmon = new G4Quasmon(valQ, q4Mom, proj4M);//Interaction gam+q inside
      theQuasmons.push_back(curQuasmon);  // Insert Quasmon without incid. gamma (del.eq.)
	   }
    else if((projM2-mPi2<.00001&&projE-mPi<0.1)&&projPDG==-211&&!fake)
    //if(2>3)                                //@@ ***TMP*** PionAbsorbAtRest by q is closed
	   {
      q4Mom=proj4M+G4LorentzVector(0.,0.,0.,tgMass-envMass);// PION + BoundCluster
      valQ=EnFlQC+curQC;
#ifdef pdebug
      if(projE<mPi)G4cout<<"*VirtualPiM*G4QE::CrQ:Ener(pi-)="<<projE<<"<mPi="<<mPi<<G4endl;
      G4cout<<"G4QEnv::CrQ:Q="<<q4Mom<<valQ<<"+pi="<<proj4M<<",E="<<theEnvironment<<G4endl;
#endif
      G4Quasmon* curQuasmon = new G4Quasmon(valQ, q4Mom, -proj4M);//Interact gam+q inside
      theQuasmons.push_back(curQuasmon);  // Insert Quasmon without incid. gamma (del.eq.)
	   }
    else
	   {
      q4Mom=proj4M+G4LorentzVector(0.,0.,0.,tgMass-envMass);//Projectile + BoundCluster
      valQ=EnFlQC+curQC;
#ifdef pdebug
      G4cout<<"G4QEnv::CreQAll: Q="<<q4Mom<<valQ<<", QEnv="<<theEnvironment<<G4endl;
#endif
      G4Quasmon* curQuasmon = new G4Quasmon(valQ, q4Mom);
      theQuasmons.push_back(curQuasmon); // Insert Quasmon (even hadron/gamma) (del.eq.)
	   }
  }
  else
  {
    G4cerr<<"---Warning---G4QEnvironment::CreateQuasmon:Strange targPDG="<<targPDG<<G4endl;
    //throw G4QException("***G4QEnvironment::CreateQuasmon: Impossible target");
  }
}

// Calculate a probability to interact with clusters for the givven PDG of the projectile
void G4QEnvironment::PrepareInteractionProbabilities(const G4QContent& projQC, G4double AP)
//   ===========================================================================Proj.3Mom.=
{
  G4double sum    = 0.;                            // Sum of probabilities of interaction
  G4double probab = 0.;                            // Interaction probability
  G4double denseB = 0.;                            // A#of*prob baryons in dense part
  G4double allB   = 0.;                            // A#of*prob baryons in the nucleus
  G4int pPDG = projQC.GetSPDGCode();               // PDG code of the projectile particle
  if(2>3) {allB=AP; allB=pPDG;}                    // A trick to use not used AP(3M) & pPDG
  for (unsigned index=0; index<theQCandidates.size(); index++)
  {
    G4QCandidate* curCand=theQCandidates[index];   // Intermediate pointer
    G4int cPDG = curCand->GetPDGCode();            // PDG Code of the Candidate
    G4int cST  = curCand->GetStrangeness();        // Strangeness of the candidate
    G4int cBN  = curCand->GetBaryonNumber();       // Baryon Number of the candidate
    G4int cCH  = curCand->GetCharge();             // Charge of the candidate
#ifdef sdebug
	   G4cout<<"G4QE::PIP:=====> #"<<index<<", cPDG="<<cPDG<<",S="<<cST<<G4endl;
#endif
    if(cPDG>80000000&&cPDG!=90000000&&!cST&&cCH>0&&cBN>0&&cCH<=cBN) // ===> Nuclear cluster
	   {
      G4int zc = cCH;                              // "Z" of the cluster
      G4int nc = cBN-cCH;                          // "N" of the cluster
      G4double nOfCl=curCand->GetPreProbability(); // A number of clusters of the type
      G4double dOfCl=curCand->GetDenseProbability();// A number of clusters in dense region
#ifdef sdebug
						G4cout<<"G4QE::PIP:Z="<<zc<<",N="<<nc<<",nC="<<nOfCl<<",dC="<<dOfCl<<G4endl;
#endif
      if(cPDG==91000000||cPDG==90001000||cPDG==90000001)
	     {
        allB+=nOfCl;
        denseB+=dOfCl;
	     }
      G4QContent pQC=curCand->GetQC();             // Quark Content of the candidate
      ////////////G4int pC   = projQC.GetCharge();   // Charge of the projectile
      G4QContent qQC=pQC+projQC;                   // Total Quark content of the Compound
      G4QPDGCode qQPDG(qQC);
      G4int qC   = qQPDG.GetQCode();
      G4double d = abs(zc-nc);
      G4double fact=1./pow(2.,d);
      if (qC<-1) probab=0.;     
      //else if(pPDG==-211&&AP<152.&&cBN<2) probab=0.; // PionCaptureByCluster
      else if(pPDG==22&&AP<152.)
      {
        if(cBN<2)probab=nOfCl*cBN*fact; //Gamma Under Pi Threshold (QuarkCapture)
        else probab=0.;
      }
      ////////////////////////else if((pPDG==-211&&AP<10.)&&cBN<2) probab=0;//PiCapAtRst(D)
      //else if((pPDG==-211||pPDG==-13)&&AP<27.)probab=dOfCl*cBN*fact;//Pi/Mu-CaptureAtRest
      //else if(pPDG==-211&&AP<10.)            probab=nOfCl*fact;// special PiCaptureAtRest
      //else if(pPDG==-211&&AP<10.)            probab=nOfCl*cBN*(cBN-1)*fact;
      //else                                   probab=nOfCl*fact;
      else                                   probab=nOfCl*cBN*fact;
      //else                                      probab=dOfCl*cBN*fact;
      //if(cBN>1) probab=0.;                       // Suppress clusters
      //if(cBN>2) probab=0.;                       // Suppress heavy clusters
#ifdef sdebug
      G4int pPDG      = projQC.GetSPDGCode();      // PDG code of the projectile particle
      G4int rPDG = qQC.GetSPDGCode();
      G4double baryn = qQC.GetBaryonNumber();
      G4double charge= qQC.GetCharge();
      G4double dq= abs(baryn-charge-charge);
	     G4cout<<"G4QE::PIP:P="<<probab<<",ac="<<cBN<<",dq="<<dq<<",f="<<fact<<",qC="
            <<qC<<",rPDG="<<rPDG<<",pPDG="<<pPDG<<",nCP="<<nOfCl<<",dCP="<<dOfCl<<G4endl;
#endif
	   }
    else probab=0.;
    sum+=probab;
    curCand->SetIntegProbability(sum);
  }
  if(allB>0.)f2all=(allB-denseB)/allB;
  else       f2all=0.;
} // End of PrepareInteractionProbabilities

//Initialize a Clusters Vector for the Nucleus of the QEnvironment
void G4QEnvironment::InitClustersVector(G4int maxClust, G4int maxA)
//   ==============================================================
{
#ifdef pdebug
  G4cout<<"G4QEnvironment::InitClustersVector called with nC="<<maxClust<<G4endl;
#endif
  if(maxClust>=0) for (G4int i=0; i<maxClust; i++) 
  {
    G4int clustQCode = i+90; // Q-code of the cluster in the CHIPS World "IsoNuclei"
#ifdef sdebug
	   G4cout<<"G4QEnvironment::InitClustersVector: Before Init Q ="<<clustQCode<<G4endl;
#endif
    G4QPDGCode clustQPDG(true,clustQCode);
    //clustQPDG.InitByQCode(clustQCode);
    G4int clusterPDG=clustQPDG.GetPDGCode();
    G4int clustB=clustQPDG.GetBaryNum();
#ifdef sdebug
	   G4cout<<"G4QEnvironment::InitClustersVector: Before insert ="<<clusterPDG<<G4endl;
#endif
	   //theQCandidates.push_back(new G4QCandidate(clusterPDG)); // (delete equivalent)
	   if(clustB<=maxA) theQCandidates.push_back(new G4QCandidate(clusterPDG)); // (del.eq.)
#ifdef sdebug
    G4cout<<"G4QEnvironment::InitClustersVector: Cluster # "<<i<<" with code = "
          <<clusterPDG<<", QC="<<clustQPDG.GetQuarkContent()<<G4endl;
#endif
  }
} // End of InitClastersVector

// Fragmentation of the QEnvironment with MultyQuasmon (the main internal member function)
G4QHadronVector  G4QEnvironment::HadronizeQEnvironment()
//               ================***********************
{
  static const G4int  NUCPDG = 90000000;
  static const G4QNucleus vacuum(NUCPDG);
  static const G4QContent PiQC(0,1,0,1,0,0);
  static const G4QContent K0QC(1,0,0,0,0,1);
  static const G4QContent KpQC(0,1,0,0,0,1);
  static const G4QContent SiPQC(0,2,1,0,0,0);
  static const G4QContent SiMQC(2,0,1,0,0,0);
  //static const G4QContent alQC(6,6,0,0,0,0);
  static const G4QPDGCode nQPDG(2112);
  static const G4QPDGCode pQPDG(2212);
  static const G4QPDGCode lQPDG(3122);
  static const G4QPDGCode s0QPDG(3122);
  static const G4double mPi0 = G4QPDGCode(111).GetMass();
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mK   = G4QPDGCode(321).GetMass();
  static const G4double mK0  = G4QPDGCode(311).GetMass();
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mSigZ= G4QPDGCode(3212).GetMass();
  static const G4double mSigM= G4QPDGCode(3112).GetMass();
  static const G4double mSigP= G4QPDGCode(3222).GetMass();
  //static const G4double mAlph = G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double eps=.003;
  G4int nQuasmons = theQuasmons.size();
#ifdef chdebug
  G4int finCharge=theEnvironment.GetCharge();
  G4int finBaryoN=theEnvironment.GetA();
  G4int nHad=theQHadrons.size();
  if(nHad) for(G4int ih=0; ih<nHad; ih++)
  {
    finCharge+=theQHadrons[ih]->GetCharge();
    finBaryoN+=theQHadrons[ih]->GetBaryonNumber();
  }
  //G4int nQuas=theQuasmons.size();
  if(nQuasmons)for(G4int iq=0; iq<nQuasmons; iq++)
  {
    finCharge+=theQuasmons[iq]->GetCharge();
    finBaryoN+=theQuasmons[iq]->GetBaryonNumber();
  }
  if(finCharge!=totCharge || finBaryoN!=totBaryoN)
  {
    G4cerr<<"**G4QE::HQ:T(1) tC="<<totCharge<<",C="<<finCharge<<",tB="<<totBaryoN
          <<",B="<<finBaryoN<<",E="<<theEnvironment<<G4endl;
    if(nHad) for(G4int h=0; h<nHad; h++)
    {
      G4QHadron* cH = theQHadrons[h];
      G4cerr<<":G4QE::HQ:h#"<<h<<",QC="<<cH->GetQC()<<",PDG="<<cH->GetPDGCode()<<G4endl;
    }
    if(nQuasmons) for(G4int q=0; q<nQuasmons; q++)
    {
      G4Quasmon* cQ = theQuasmons[q];
      G4cerr<<":G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QuarkCon="<<cQ->GetQC()<<G4endl;
    }
  }
#endif
#ifdef pdebug
  G4cout<<"G4QE::HQE:*HADRONIZE Q-ENVIRONMENT="<<theEnvironment<<",nQ="<<nQuasmons<<G4endl;
#endif
  if(nQuasmons<1)                            // "No Quasmons" case -> Fill QEnviron
  {
    G4int nPDG = theEnvironment.GetPDG();    // PDG code of the residual Nucl.Environ.
#ifdef pdebug
	   G4cout<<"G4QE::HQE:***NO QUASMONS***Env="<<nPDG<<theEnvironment.Get4Momentum()<<G4endl;
#endif
    if(nPDG==90000000) return theQHadrons;
    if(nPDG>80000000)
	   {
      G4QHadron* rNucleus = new G4QHadron(theEnvironment); // Create HadronEnvironment
      theQHadrons.push_back(rNucleus);       // Fill GS - no further decay (del. equiv.)
#ifdef fdebug
	     G4cout<<"G4QEnv::HadrQE: >>>> Fill Environment="<<theEnvironment<<G4endl;
#endif
	   }
    return theQHadrons;
  }
  if(theEnvironment.GetPDG()==NUCPDG)        // ==> "Environment is Vacuum" case
  {
#ifdef rdebug
    G4cout<<"G4QEnv::HadrQE: ***Vacuum*** #ofQ="<<nQuasmons<<G4endl;
    G4int totInC=0;
    G4LorentzVector totIn4M(0.,0.,0.,0.);
	   for (G4int is=0; is<nQuasmons; is++)     // Sum4mom's of Quasmons for the comparison
	   {
	     G4Quasmon*      pQ = theQuasmons[is];
      G4LorentzVector Q4M= pQ->Get4Momentum();
      totIn4M           += Q4M;
      totInC            += pQ->GetQC().GetCharge();
	   } // End of TotInitial4Momentum summation LOOP over Quasmons
    G4int nsHadr  = theQHadrons.size();      // Update the value of OUTPUT entries
    if(nsHadr) for(G4int jso=0; jso<nsHadr; jso++)// LOOP over output hadrons 
    {
      G4int hsNF  = theQHadrons[jso]->GetNFragments(); // A#of secondary fragments
      if(!hsNF)                                        // Add only final hadrons
      {
        G4LorentzVector hs4Mom = theQHadrons[jso]->Get4Momentum();
        totIn4M          += hs4Mom;
        totInC           += theQHadrons[jso]->GetCharge();
      }
    }
#endif
    G4QNucleus vE(90000000);
    G4int     nlq = 0;                       // Prototype of a#of Living Quasmons
	   if(nQuasmons) for(G4int lq=0; lq<nQuasmons; lq++)if(theQuasmons[lq]->GetStatus())nlq++;
	   if(nQuasmons) for(G4int iq=0; iq<nQuasmons; iq++)
	   {
#ifdef chdebug
      G4int f1Charge=theEnvironment.GetCharge();
      G4int f1BaryoN=theEnvironment.GetA();
      G4int nHad=theQHadrons.size();
      if(nHad) for(G4int ih=0; ih<nHad; ih++)
      {
        f1Charge+=theQHadrons[ih]->GetCharge();
        f1BaryoN+=theQHadrons[ih]->GetBaryonNumber();
      }
      G4int nQuas=theQuasmons.size();
      if(nQuas)for(G4int iqs=0; iqs<nQuas; iqs++)
      {
        f1Charge+=theQuasmons[iqs]->GetCharge();
        f1BaryoN+=theQuasmons[iqs]->GetBaryonNumber();
      }
      if(f1Charge!=totCharge || f1BaryoN!=totBaryoN)
      {
        G4cerr<<"**G4QE::HQ:q#"<<iq<<",tC="<<totCharge<<",C="<<f1Charge<<",tB="<<totBaryoN
              <<",B="<<f1BaryoN<<",E="<<theEnvironment<<G4endl;
        if(nHad) for(G4int h=0; h<nHad; h++)
        {
          G4QHadron* cH = theQHadrons[h];
          G4cerr<<"G4QE::HQ:#"<<h<<",QC="<<cH->GetQC()<<",P="<<cH->GetPDGCode()<<G4endl;
        }
        if(nQuas) for(G4int q=0; q<nQuas; q++)
        {
          G4Quasmon* cQ = theQuasmons[q];
          G4cerr<<"G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QCont="<<cQ->GetQC()<<G4endl;
        }
      }
#endif
      G4int ist=theQuasmons[iq]->GetStatus();// Status of the Quasmon before fragmentation
      if(ist)
	     {
        G4QHadronVector* output=theQuasmons[iq]->Fragment(vE,1);//!!!DESTROY!!! <---------+
        G4int ast=theQuasmons[iq]->GetStatus();  // Quasmon's Status after fragmentation  ^
        if(!ast) nlq--;                          // Reduce nlq is Quasmon decayed         ^
        G4int nHadrons = output->size();         // A#of output Hadrons in the Quasmon    ^
#ifdef pdebug
        G4cout<<"G4QEnv::HadrQE: ***Vacuum*** Q#"<<iq<<", nHadr="<<nHadrons<<G4endl; //   ^
#endif
        if(nHadrons>0)                           // Copy QHadrons-Quasmon to Output       ^
	       {
    	     for (G4int ih=0; ih<nHadrons; ih++)    // LOOP over QHadrons of the Quasmon     ^
          {
            //G4QHadron* curH=new G4QHadron(output->operator[](ih));// (Del 7 lines below)^
            G4QHadron* curH = new G4QHadron((*output)[ih]); // (Deleted 7 lines below)    ^
#ifdef pdebug
            G4cout<<"G4QEnv::HadrQE:Vacuum, H#"<<ih<<", QPDG="<<curH->GetQPDG() //        ^
                  <<",4M="<<curH->Get4Momentum()<<G4endl; //                              ^
#endif
            theQHadrons.push_back(curH);         // Fill hadron-copy (delete equivalent)  ^
          }
	       }                                        //                                       ^
        else                                     // => "Quasmon can't decay" case         ^
								{                                        //                                       ^
          G4QContent totQC=theQuasmons[iq]->GetQC();//                                    ^
          G4int     tQBN=totQC.GetBaryonNumber();// Baryon Number of not decayed Quasmon  ^
          G4QNucleus     tqN(totQC);             // Define the quasmon as a nucleus       ^
          G4double   gsM=tqN.GetMZNS();          // GS Mass                               ^
          G4LorentzVector tot4M=theQuasmons[iq]->Get4Momentum();
          G4double totQM=tot4M.m();              // Real Mass of Quasmon                  ^
          if(tQBN>0&&totQM>gsM)                  // => "Try Quasmon evaporation" case     ^
										{                                      //                                       ^
            G4QHadron* nuclQ = new G4QHadron(totQC,tot4M); //                             ^
#ifdef fdebug
            G4cout<<"G4QEnv::HadrQE:Vac,tQC"<<totQC<<",t4M="<<tot4M<<G4endl; //           ^
#endif
            EvaporateResidual(nuclQ);            // Evaporate ResNuc (del.equiv)          ^
            theQuasmons[iq]->KillQuasmon();      // Kill evaporated Quasmon               ^
            nlq--;                               //                                       ^
		        }
          else if(iq+1<nQuasmons&&nlq>1)         // => "Try to merge with next" case      ^
		        {
            G4int s=theQuasmons[iq+1]->GetStatus();//Status of the next Quasmon           ^
            theQuasmons[iq+1]->IncreaseBy(theQuasmons[iq]);// Merge with the next Quasmon ^
            theQuasmons[iq]->KillQuasmon();      // Kill the week Quasmon                 ^
            if(s) nlq--;                         // Reduce a number of "living Quasmons"  ^
		        }
          else if(iq+1==nQuasmons&&iq&&nlq>1)    // => "Quasmon stack is exhosted" case   ^
		        {
            G4int s=theQuasmons[0]->GetStatus(); // Status of the first Quasmon           ^
            theQuasmons[0]->IncreaseBy(theQuasmons[iq]);// Merge with the first Quasmon   ^
            theQuasmons[iq]->KillQuasmon();      // Kill the week Quasmon                 ^
            if(s) nlq--;                         // Reduce a number of "living Quasmons"  ^
		        }
		        else                                   // "Have a chance to recover" case       ^
		        {                                      //                                       ^
#ifdef pdebug
		          G4cout<<"***G4QE::HQE:"<<iq<<",n="<<nHadrons<<",Tot="<<totQC<<totQM<<G4endl;//^
		          for (G4int kq=0; kq<nQuasmons; kq++) // LOOP over Quasmons for DEBUG PRINTING ^
														G4cout<<kq<<",St/QC="<<theQuasmons[kq]->GetStatus()<<theQuasmons[kq] //     ^
                    ->GetQC()<<",M="<<theQuasmons[kq]->Get4Momentum().m()<<G4endl; //     ^
#endif
            G4int nOfOUT = theQHadrons.size();   // Total #of QHadrons at this point      ^
            G4double  dM = totQM-gsM;            // Excitation of the Quasmon             ^
            G4bool corrf = true;                 // False when corrected & needs to quit  ^
            while(nOfOUT && corrf)               // LOOP over all existing QHadrons       ^
			         {                                    //                                       ^
              G4QHadron*     theLast = theQHadrons[nOfOUT-1];     //  Remember            ^
              G4LorentzVector last4M = theLast->Get4Momentum();   //  all                 ^
              G4QContent      lastQC = theLast->GetQC();          //  content             ^
              G4int           lastS  = lastQC.GetStrangeness();   //  of    // Only       ^
              G4int           totS   = totQC.GetStrangeness();    //  the   // for        ^
              G4int           nFr    = theLast->GetNFragments();  //  Last  // if()       ^
              G4int           gam    = theLast->GetPDGCode();     //        //            ^
			           if(gam!=22&&!nFr&&lastS<0&&lastS+totS<0&&nOfOUT>1)//=> Skip K,gam & decayed ^
              {                                  //                                       ^
                G4QHadron* thePrev = theQHadrons[nOfOUT-2];// Kill Prev & make Last->Prev ^
                theQHadrons.pop_back();          // theLastQHadron is excluded from OUTPUT^
                theQHadrons.pop_back();          // thePrevQHadron is excluded from OUTPUT^
                theQHadrons.push_back(thePrev);  // thePrev becomes theLast as an object  ^
                delete     theLast;              // the Last QHadron is destructed        ^
                theLast = thePrev;               // Update parameters (thePrev*->theLast*)^
                last4M = theLast->Get4Momentum();// 4Mom of the previouse Quasmon         ^
                lastQC = theLast->GetQC();       // Quark Content of the previouse Quasmon^
			           }                                  //                                       ^
              else                               // Just Clear and destroy theLast        ^
			           {                                  //                                       ^
                theQHadrons.pop_back();          // theLastQHadron is excluded from OUTPUT^
                delete         theLast;          // theLastQHadron is deleated as instance^
			           }                                  //                                       ^
              totQC+=lastQC;                     // Update (increase) the total QC        ^
              tot4M+=last4M;                     // Update (increase) the total 4-momentum^
              totQM=tot4M.m();                   // Calculate new real total mass         ^
              G4QNucleus nN(totQC);              // Define the Quasmon as a nucleus       ^
              gsM=nN.GetMZNS();                  // Calculate the new GS Mass             ^
              dM = totQM-gsM;                    // Escitation energy for the Quasmon     ^
              if(dM>0)                           // "Mass of Q is big enough" case        ^
			           {                                  //                                       ^
                theQuasmons[iq]->InitQuasmon(totQC,tot4M);// Update the week Quasmon      ^
                G4QHadronVector* curout=theQuasmons[iq]->Fragment(vE,1);//!DESTROY! <---+ ^
                G4int ast=theQuasmons[iq]->GetStatus();  // Status of the Quasmon       ^ ^
                if(!ast) nlq--;                  // Reduce nlq if Quasmon decayed       ^ ^
                G4int nHadrons=curout->size();   // A#of outputQHadrons in theDecayedQ  ^ ^
#ifdef pdebug
                G4cout<<"G4QEnv::HadrQE:VacuumRecoverQ#"<<iq<<",n="<<nHadrons<<G4endl;//^ ^
#endif
                if(nHadrons>0)                   // => "QHadrons from Quasmon to Output"^ ^
				            {                                //                                     ^ ^
    	             for (G4int ih=0; ih<nHadrons; ih++) // LOOP over Hadrons of theQuasmon^ ^
				              {                              //                                     ^ ^
                    //G4QHadron* curH = new G4QHadron(curout->operator[](ih)); //       ^ ^
                    G4QHadron* curH = new G4QHadron((*curout)[ih]); //                  ^ ^
#ifdef pdebug
                    G4cout<<"G4QEnv::HadrQE:Recovered, H#"<<ih<<", QPDG=" //            ^ ^
                          <<curH->GetQPDG()<<",4M="<<curH->Get4Momentum()<<G4endl;  //  ^ ^
#endif
                    totQC-=curH->GetQC();        // totQC recalculation                 ^ ^
					               tot4M-=curH->Get4Momentum(); // tot4M recalculation                 ^ ^
                    theQHadrons.push_back(curH); // Fill hadron-copy (delete equivalent)^ ^
                    //delete curout->operator[](ih);//>-Necessary to delete instances*>-^ ^
                    delete (*curout)[ih];        // >-*Necessary to delete instances*>--^ ^
                  } // End of LOOP over Hadrons of the Quasmon                          ^ ^
                  curout->clear();               //                                     ^ ^
                  delete curout;                 //>*Necessary to delete VectPointers*>=^ ^
                  corrf = false;                 // Corrected: go out of the while loop ^ ^
                  //break;                       // @@ ??                               ^ ^
	               } // End of check for existing output Hadrons in the Quasmon            ^ ^
                else                             //                                     ^ ^
				            {                                //                                     ^ ^
                  for_each(curout->begin(), curout->end(), DeleteQHadron()); // >-------^ ^
                  curout->clear();               //                                     ^ ^
                  delete curout;                 //>*Necessary to delete VectPointers>--^ ^
				            }                                //                                       ^
			           }                                  //                                       ^
              nOfOUT  = theQHadrons.size();      // Update the value of OUTPUT entries    ^
#ifdef rdebug
              G4int tC=totInC;                   // Vacuum: No ResidualEnvironCharge      ^
              G4LorentzVector t4M=totIn4M;       // Vacuum: No ResidualEnvironment 4-Mom  ^
	             for (G4int js=0; js<nQuasmons; js++) // Subtract 4mom's of Quasmons from dif^
			           {                                  //                                       ^
	               G4Quasmon*      pQ = theQuasmons[js]; //                                  ^
                if(pQ->GetStatus())              // Subtract only if Quasmon is alive     ^
				            {                                //                                       ^
                  G4LorentzVector Q4M= pQ->Get4Momentum(); //                             ^
                  t4M               -= Q4M;                //                             ^
                  tC                -= pQ->GetQC().GetCharge(); //                        ^
                }                                //                                       ^
                else G4cout<<"G4QE::HQ:SUM-4-Mom s("<<js<<")="<<pQ->GetStatus()<<G4endl;//^
	             } // End of Quasmons4Momentum subtractions                                  ^
              if(nOfOUT) for(G4int jpo=0; jpo<nOfOUT; jpo++)// LOOP over output hadrons   ^
			           {                                  //                                       ^
                G4int hsNF  = theQHadrons[jpo]->GetNFragments();//A#of secondary fragments^
                if(!hsNF)                                   // Subtract only final hadrons^
                {
                  G4LorentzVector hs4Mom = theQHadrons[jpo]->Get4Momentum(); //           ^
                  t4M                   -= hs4Mom;                           //           ^
                  tC                    -= theQHadrons[jpo]->GetCharge();    //           ^
                }                                //                                       ^
              }                                  //                                       ^
              G4cout<<"G4QE::HQ:|||Vacuum|||4-MomCHECK|||d4M="<<t4M<<",dC="<<tC<<G4endl;//^
#endif
		          }                                    // End of the WHILE LOOP                 ^
		          //if(!nOfOUT&&nQuasmons==1)          // TRY TO EVAPORATE THE TOTAL SYSTEM     ^
		          if((!nOfOUT&&nQuasmons==1)||theEnvironment.GetPDGCode()==NUCPDG)//EvaporTotal ^
			         {                                    //                                       ^
              G4int totS=totQC.GetStrangeness(); //  Total Strangeness                    ^
              //G4int totBN=totQC.GetBaryonNumber();// Total Baryon Number                ^
              G4int totPDG=totQC.GetZNSPDGCode();// Convert QC to PDGCOde for the nucleus ^
              if(totS) totPDG-=totS*999999;      // @@ ??                                 ^
#ifdef fdebug
		            G4cout<<"G4QE::HQE: totPDG="<<totPDG<<",totM="<<totQM<<G4endl; //           ^
#endif
              G4QHadron* evH = new G4QHadron(totQC,tot4M); // Create a Hadron-ResidualNucl^
              CleanUp();                         //                                       ^
              EvaporateResidual(evH);           // Evaporate ResNuc (del.equiv)           ^
              for_each(output->begin(), output->end(), DeleteQHadron());// >--------------^
              output->clear();                   //                                       ^
              delete output;                     // >=====================================^
              return theQHadrons;                //                                       ^
            }                                    //                                       ^
		          else if(!nOfOUT)                     // Still remain not used Quasmons        ^
			         {                                    //                                       ^
		            G4cerr<<"***G4QE::HQ:T="<<tot4M<<totQC<<",M="<<totQM<<"< gsM=" //           ^
                    <<gsM<<",d="<<dM<<",Env="<<theEnvironment<<G4endl; //                 ^
              throw G4QException("G4QEnvironment::HadronizeQEnv: Can't decay Quasmon");// ^
	           }                                    //                                       ^
		        } // End of PANIC treatment                                                     ^
		      } // End of trouble handling with Quasmon decay in Vacuum                         ^
        for_each(output->begin(), output->end(), DeleteQHadron());  // >------------------^
        output->clear();                         //                                       ^
        delete output;                           // >=====================================^
	     } // End of check for the already decayed Quasmon
	   } // End of the LOOP over Quasmons
  }
  else                                           // ==> "Nuclear environment" case
  {
#ifdef rdebug
    G4cout<<"G4QEnv::HadrQE:FRAGMENTATION IN NUCLEAR ENVIRONMENT nQ="<<nQuasmons<<G4endl;
    G4int totInC=theEnvironment.GetZ();
    G4LorentzVector totIn4M=theEnvironment.Get4Momentum();
	   for (G4int is=0; is<nQuasmons; is++) // Sum 4mom's of Quasmons for comparison
	   {
	     G4Quasmon*      pQ = theQuasmons[is];
      G4LorentzVector Q4M= pQ->Get4Momentum();
      totIn4M           += Q4M;
      totInC            += pQ->GetQC().GetCharge();
	   } // End of TotInitial4Momentum summation LOOP over Quasmons
    G4int nsHadr  = theQHadrons.size();        // Update the value of OUTPUT entries
    if(nsHadr) for(G4int jso=0; jso<nsHadr; jso++)// LOOP over output hadrons 
    {
      G4int hsNF  = theQHadrons[jso]->GetNFragments(); // A#of secondary fragments
      if(!hsNF)                                        // Add only final hadrons
      {
        G4LorentzVector hs4Mom = theQHadrons[jso]->Get4Momentum();
        totIn4M          += hs4Mom;
        totInC           += theQHadrons[jso]->GetCharge();
      }
    }
#endif
    //G4int   c3Max = 27;
    //G4int   c3Max = 9;                    // Max # for the no hadrons steps
    //G4int   c3Max = 3;
    G4int   c3Max = 1;
    //G4int   premC = 27;
    //G4int   premC = 3;
    G4int   premC = 1;
    G4int   envA=theEnvironment.GetA();
    //if(envA>1&&envA<13) premC = 24/envA;
    //if(envA>1&&envA<19) premC = 36/envA;
    //if(envA>1&&envA<25) premC = 48/envA;
    if(envA>1&&envA<31) premC = 60/envA;
    G4int  sumstat= 2;                     // Sum of statuses of all Quasmons
    G4bool force  = false;                 // Prototype of the Force Major Flag
    G4int cbR     =0;                      // Counter of the "Stoped by Coulomb Barrier"
    //
    G4int cbRM    =1;                      // MaxCounter of the "StopedByCoulombBarrier"
    //G4int cbRM    =3;                      // MaxCounter of the "StopedByCoulombBarrier"
    //G4int cbRM    =9;                      // MaxCounter of the "Stoped byCoulombBarrier"
    G4int totC    = 0;                     // Counter to break the "infinit" loop
    //G4int totCM   = 227;                   // Limit for the "infinit" loop counter
    G4int totCM   = envA;                   // Limit for the "infinit" loop counter
    //G4int totCM   = 27;                    // Limit for this counter
    G4int nCnMax = 1;                      // MaxCounterOfHadrFolts for shortCutSolutions
    //G4int nCnMax = 3;                      // MaxCounterOfHadrFolts for shortCutSolutions
    //G4int nCnMax = 9;                      // MaxCounterOfHadrFolts for shortCutSolutions
    while (sumstat||totC<totCM)            // ===***=== The MAIN "FOREVER" LOOP ===***===
	   {
#ifdef chdebug
      G4int f2Charge=0;
      G4int f2BaryoN=0;
      if(theEnvironment.GetMass()>0.)
      {
        f2Charge=theEnvironment.GetCharge();
        f2BaryoN=theEnvironment.GetA();
      }
      G4int nHad=theQHadrons.size();
      if(nHad) for(G4int ih=0; ih<nHad; ih++)
      {
        f2Charge+=theQHadrons[ih]->GetCharge();
        f2BaryoN+=theQHadrons[ih]->GetBaryonNumber();
      }
      G4int nQuas=theQuasmons.size();
      if(nQuas)for(G4int iqs=0; iqs<nQuas; iqs++)
      {
        f2Charge+=theQuasmons[iqs]->GetCharge();
        f2BaryoN+=theQuasmons[iqs]->GetBaryonNumber();
      }
      if(f2Charge!=totCharge || f2BaryoN!=totBaryoN)
      {
        G4cerr<<"**G4QE::HQ:(NucEnv)i#"<<totC<<",tC="<<totCharge<<",C="<<f2Charge<<",tB="
              <<totBaryoN<<",B="<<f2BaryoN<<",E="<<theEnvironment<<G4endl;
        if(nHad) for(G4int h=0; h<nHad; h++)
        {
          G4QHadron* cH = theQHadrons[h];
          G4cerr<<"G4QE::HQ:#"<<h<<",QC="<<cH->GetQC()<<",P="<<cH->GetPDGCode()<<G4endl;
        }
        if(nQuas) for(G4int q=0; q<nQuas; q++)
        {
          G4Quasmon* cQ = theQuasmons[q];
          G4cerr<<"G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QCont="<<cQ->GetQC()<<G4endl;
        }
      }
#endif
      totC++;
      if(nQuasmons==1&&sumstat==3) cbR++;
      else cbR=0;
      G4QContent envQC=theEnvironment.GetQCZNS();// QuarkCont of current NuclearEnvironment
      G4QContent totQC=envQC;                    // Total QuarkContent in the system
      G4double   envM =theEnvironment.GetMass(); // mass of NuclEnvironment (@@GetMZNS())
      G4double   sumM =envM;                     // Sum of all residual masses in theSystem
      G4LorentzVector env4M=theEnvironment.Get4Momentum();      
      G4LorentzVector tot4M=env4M;               // 4-momentum of the Total System
      sumstat         =0;
      G4int     nCount=0;                        // Counter of notsuccessful fragmentations
      G4int     fCount=0;                        // Counter of successful(notFinal) fragm's
	     G4int     eCount=0;                        // Counter of not yet decayed Quasmons
	     for (G4int iq=0; iq<nQuasmons; iq++)       // Sum up Quasmons for making a decision
	     {
	       G4Quasmon*      pQ = theQuasmons[iq];
        G4QContent      QQC= pQ->GetQC();
        totQC             += QQC;
        G4LorentzVector Q4M= pQ->Get4Momentum();
        tot4M             += Q4M;
        G4double        QM = Q4M.m();
        sumM              += QM;
        G4int           Qst= pQ->GetStatus();
        sumstat           += Qst;
#ifdef pdebug
	       G4cout<<"G4QEnv::HadrQE:#"<<iq<<", Qst="<<Qst<<", Q="<<Q4M<<Q4M.m()<<QQC<<", Env="
              <<theEnvironment<<G4endl;
#endif
        if(Qst==1||Qst==3||Qst==4)
        {
          fCount++;                              // Incr. counterSuccessfulFragmentations
          nCount=0;                             // SetCounterNotSuccessfulFragmentations
        }
        if(Qst>0)                  eCount++;     // Incr. counter of existing Quasmons 
	     } // End of summation LOOP over Quasmons
      G4int      totS  =totQC.GetStrangeness();  // Total Strangeness of the Total System
      G4int      totBN =totQC.GetBaryonNumber(); // Total Baryon Number of the Total System
      G4int      totPDG=0;                       // Total PDG Code for the Current compound
      G4double   totM  =0.;                      // min(GroundSt)Mass of theResidualSystem
      if(totBN<2)  
	     {
        totPDG=totQC.GetSPDGCode();              // Min totPDGCode for theCurrentCompound
        if(totPDG) totM=G4QPDGCode(totPDG).GetMass(); // min Mass of the Residual System
        else throw G4QException("G4QEnv::HadrQEnv: Impossible PDG for B=1");
      }
      else
	     {
        G4QNucleus totN(totQC,tot4M);            // Excited nucleus for theResidualSystem
        totM=totN.GetMZNS();                     // min(GroundSt)Mass of theResidualSystem
        totPDG=totN.GetPDG();                    // Total PDG Code for the Current compound
	     }
#ifdef fdebug
	     G4cout<<"G4QEnv::HadrQE:totC="<<totC<<"<totCM="<<totCM<<",ss="<<sumstat<<G4endl;
#endif
      if(totC>=totCM||cbR>cbRM)
      {
        CleanUp();
        G4QHadron* evH = new G4QHadron(totQC,tot4M);// Create a Hadron for ResidualNucl
        EvaporateResidual(evH);               // Try to evaporate residual (del. equiv.)
        return theQHadrons;
	     }
      // === Now we should be prepared for evaporation ===
      G4int      totChg=totQC.GetCharge();    // Total Electric Charge of the Total System
#ifdef pdebug
      if(totPDG==90999999||totPDG==90999000||totPDG==90000999||totPDG==89999001)
		    G4cout<<"***G4QEnv::HadrQEnv: Meson (1) PDG="<<totPDG<<", M="<<tot4M.m()<<G4endl;
      G4int           nOH=theQHadrons.size(); // A#of output hadrons
      G4LorentzVector s4M=tot4M;              // Total 4-momentum (@@ only for checking)
      if(nOH) for(G4int ih=0; ih<nOH; ih++) s4M+=theQHadrons[ih]->Get4Momentum();     
	     G4cout<<"G4QEnv::HadrQE:tBN="<<totBN<<",s="<<sumstat<<",fC="<<fCount<<",eC="<<eCount
            <<",En="<<theEnvironment<<",nH="<<nOH<<",tLV="<<s4M<<totQC<<nCount<<G4endl;
#endif
      if(totBN<2)                             // => "Baryons or Mesons" case (@@antiBaryon)
      {
        totPDG =totQC.GetSPDGCode();
        if(totPDG && totPDG!=10 && totPDG!=1114 && totPDG!=2224)
                                                         totM=G4QPDGCode(totPDG).GetMass();
        else if(totPDG==1114) totM=mNeut+mPi;
        else if(totPDG==2224) totM=mProt+mPi;
        else if(totPDG==10)
		      {
          G4QChipolino totChip(totQC);        // define the residual as a Chipolino
          totM  =totChip.GetQPDG1().GetMass()+totChip.GetQPDG2().GetMass();
		      }
        else
		      {
          G4cerr<<"***G4QEnv::HadrQE:totPDG="<<totPDG<<", totQC="<<totQC<<G4endl;
          throw G4QException("G4QEnvironment::HadronizeQEnvir: ImpossibleHadron in CHIPS");
		      }
	     }
      G4double totMass = tot4M.m();           // Total effective Mass
      G4bool   Premium = eCount&&premC&&envM; // Premium condition
      G4int    count3  =0;
      if(sumstat&&(fCount||Premium)&&!force&&count3<c3Max)//=>"Still try to decay Quasmons"
	     {
        if(!fCount) premC--;                  // Reduce premium efforts counter
	       if(nQuasmons) for (G4int jq=0; jq<nQuasmons; jq++)//FragmentationLOOP over Quasmons
	       {
	         G4Quasmon* pQ     = theQuasmons[jq];// Pointer to the CurrentQuasmon <--<--<--+
          G4int      status = pQ->GetStatus();// Old status of the Quasmon              ^
#ifdef pdebug
	         G4cout<<"G4QE::HQE:Status of Q#"<<jq<<" (before Fragment)="<<status<<G4endl;//^
#endif
          if(status)                          // Skip dead Quasmons                     ^
          {                                   //                                        ^
 	          G4QHadronVector* output=pQ->Fragment(theEnvironment,eCount);//*!DEST* <--<--^-+
            envM =theEnvironment.GetMass();   // new mass of Nuclear Environment        ^ ^
            status = pQ->GetStatus();         // NewStatus after FragmentationAttempt   ^ ^
            if(!status) eCount--;             // Dec. ExistingQuasmonsCounter for Q=0   ^ ^
            G4int nHadrons = output->size();  //                                        ^ ^
#ifdef rdebug
	           G4cout<<"G4QE::HQE:**AfterFragmAttempt**#"<<jq<<",stat="<<status<<", Q4M="//^ ^
                  <<pQ->Get4Momentum()<<", Env="<<theEnvironment<<",nH="<<nHadrons //   ^ ^
                  <<",c3="<<count3<<" < "<<c3Max<<",eC="<<eCount<<G4endl;          //   ^ ^
            G4int tC=totInC-theEnvironment.GetZ(); // Subtract theResidualEnvironCharge ^ ^
            G4LorentzVector t4M=totIn4M;      // Compare with the total                 ^ ^
			         G4LorentzVector theEnv4m=theEnvironment.Get4Momentum(); // Environment 4Mom ^ ^
			         t4M-=theEnv4m;                    // Subtract the Environment 4-momentum    ^ ^
            G4cout<<"G4QEnv::HadrQE:SUM-4Mom e4M="<<theEnv4m<<theEnvironment<<G4endl;// ^ ^
	           for (G4int js=0; js<nQuasmons; js++)// Subtract 4mom's of Quasmons (compare)^ ^
            {                                 //                                        ^ ^
	             G4Quasmon*      prQ = theQuasmons[js];//                                  ^ ^
              if(prQ->GetStatus())            // Subtract only if Quasmon is alive      ^ ^
              {                               //                                        ^ ^
                G4LorentzVector Q4M= prQ->Get4Momentum(); //                            ^ ^
                G4QContent qQC= prQ->GetQC(); //                                        ^ ^
                G4cout<<"G4QE::HQE:SUM-4Mom q("<<js<<")4M="<<Q4M<<",QC="<<qQC<<G4endl;//^ ^
                t4M          -= Q4M;          //                                        ^ ^
                tC           -= prQ->GetQC().GetCharge(); //                            ^ ^
			           }                               //                                        ^ ^
              else G4cout<<"G4QE::HQE:SUM-4M,st("<<js<<")="<<prQ->GetStatus()<<G4endl;//^ ^
	           } // End of Quasmons4Momentum subtractions                                  ^ ^
            G4int nsbHadr=theQHadrons.size(); // Update the value of OUTPUT entries     ^ ^
            if(nsbHadr) for(G4int jpo=0; jpo<nsbHadr; jpo++)// LOOP over output hadrons ^ ^
			         {                                 //                                        ^ ^
              G4int hsNF  = theQHadrons[jpo]->GetNFragments();// A#of out fragments     ^ ^
              if(!hsNF)                       // Subtract only final hadrons            ^ ^
              {                               //                                        ^ ^
                G4LorentzVector hs4Mom = theQHadrons[jpo]->Get4Momentum(); //           ^ ^
                G4int hPDG = theQHadrons[jpo]->GetPDGCode(); //                         ^ ^
                G4cout<<"G4QE::HQE:SUM-4-Mom eh("<<jpo<<")4M="<<hs4Mom<<hPDG<<G4endl;// ^ ^
                t4M          -= hs4Mom;       //                                        ^ ^
                tC           -= theQHadrons[jpo]->GetCharge(); //                       ^ ^
	  		         }                               // End of the "FinalHadron" IF            ^ ^
            }                                 // End of the LOOP over output hadrons    ^ ^
            if(nHadrons) for(G4int kpo=0; kpo<nHadrons; kpo++)//LOOP over out QHadrons  ^ ^
            {                                 //                                        ^ ^
	  		         //G4QHadron* insH =output->operator[](kpo);// Pointer to theOutputQHadron ^ ^
	  		         G4QHadron* insH = (*output)[kpo];// Pointer to the Output QHadron         ^ ^
              G4int qhsNF  = insH->GetNFragments(); // A#of secondary fragments         ^ ^
              if(!qhsNF)                      // Subtract only final hadrons            ^ ^
			           {                               //                                        ^ ^
                G4LorentzVector qhs4Mom = insH->Get4Momentum();// 4M of theOutputQHadron^ ^
                G4int hPDG = insH->GetPDGCode(); //  PDG Code of the Q-output Hadron    ^ ^
                G4cout<<"G4QE::HQE:SUM-4-Mom qh("<<kpo<<")4M="<<qhs4Mom<<hPDG<<G4endl;//^ ^
                t4M          -= qhs4Mom;      //                                        ^ ^
                tC           -= insH->GetCharge(); //                                   ^ ^
			           }                               //                                        ^ ^
            }                                 // End of the LOOP over output QHadrons   ^ ^
            G4cout<<"G4QEnv::HadrQE:|||||4-MomCHECK||||d4M="<<t4M<<",dC="<<tC<<G4endl;//^ ^
#endif
            if(!status||status==1||nHadrons)  //OutHadronVector was filled in G4Q::Frag ^ ^
            {                                 //                                        ^ ^
              nCount=0;                       // Reset the NotSuccessfulFragmCounter    ^ ^
              if(nHadrons>0)                  // Transfer QHadrons from Quasm to Output ^ ^
              {                               //                                        ^ ^
    	           for (G4int ih=0; ih<nHadrons; ih++) // LOOP over Q-output QHadrons      ^ ^
                {                             //                                        ^ ^
				              //G4QHadron* inpH = output->operator[](ih); //                        ^ ^
				              G4QHadron* inpH = (*output)[ih]; //                                   ^ ^
                  G4int hC=inpH->GetCharge(); // Charge of the Hadron                   ^ ^
                  G4int hF=inpH->GetNFragments();// Number of fragments                 ^ ^
                  G4double hCB=0.;            // Coulomb Barrier                        ^ ^
                  G4double hKE=0.;            // Kinetic Energy of the Hadron           ^ ^
                  G4LorentzVector hLV=inpH->Get4Momentum(); //                          ^ ^
#ifdef pdebug
                  G4cout<<"G4QEnv::HadrQE:H#"<<ih<<", hC="<<hC<<",hF="<<hF<<",4M=" //   ^ ^
                        <<hLV<<inpH->GetPDGCode()<<G4endl;  //                          ^ ^
#endif
                  G4bool can=hC&&!hF;         // Charged and not yet decayed hadron     ^ ^
                  if(can)                     //                                        ^ ^
                  {                           //                                        ^ ^
                    G4int hB=inpH->GetBaryonNumber(); //                                ^ ^
                    hCB=theEnvironment.CoulombBarrier(hC,hB); //                        ^ ^
                    hKE=hLV.e()-hLV.m();      //                                        ^ ^
				              }                           //                                        ^ ^
                  if(can&&hKE<hCB)            // => "Suck Hadron in Quasm or Env" case  ^ ^
				              {                           //                                        ^ ^
                    if(status)                // => "Suck in the existing Quasmon" case ^ ^
                    {                         //                                        ^ ^
                      G4QContent tQC=inpH->GetQC()+pQ->GetQC();  //                     ^ ^
                      G4LorentzVector tLV=hLV+pQ->Get4Momentum();//                     ^ ^
                      pQ->InitQuasmon(tQC,tLV); // Reinitialize the current Quasmon     ^ ^
#ifdef pdebug
	                     G4cout<<"G4QE::HQE:Medium, H#"<<ih<<", QPDG="<<inpH->GetQPDG() // ^ ^
                            <<",4M="<<inpH->Get4Momentum()<<" is suckedInQ"<<G4endl; // ^ ^
#endif
				                }                         //                                        ^ ^
                    else                      // => "Suck in the existing Quasmon" case ^ ^
                    {                         //                                        ^ ^
                      G4QContent tQC=inpH->GetQC()+theEnvironment.GetQCZNS();//         ^ ^
                      G4LorentzVector tLV=hLV+theEnvironment.Get4Momentum(); //         ^ ^
                      theEnvironment=G4QNucleus(tQC,tLV); // Reinit currentEnvironment  ^ ^
#ifdef pdebug
	                     G4cout<<"G4QE::HQE:Med,H#"<<ih<<",PDG="<<inpH->GetQPDG()<<",4M="//^ ^
                            <<inpH->Get4Momentum()<<" is suckedInEnvironment"<<G4endl;//^ ^
#endif
				                }                         // End of the STATUS IF                   ^ ^
				              }                           // End of the E/Q suck Hdron IF           ^ ^
                  else if(!hF)                // => "Hadron can go out" case            ^ ^
                  {                           //                                        ^ ^
                    G4QHadron* curH = new G4QHadron(inpH); //                           ^ ^
#ifdef pdebug
                    G4LorentzVector ph4M=curH->Get4Momentum(); // 4-mom of the hadron   ^ ^
                    G4double phX=ph4M.x();    // p_x of the hadron                      ^ ^
                    G4double phY=ph4M.y();    // p_y of the hadron                      ^ ^
                    G4double phZ=ph4M.z();    // p_x of the hadron                      ^ ^
                    G4double phCost=phZ/sqrt(phX*phX+phY*phY+phZ*phZ);// Hadr cos(theta)^ ^
	                   G4cout<<"G4QEnv::HadrQE:Medium, H#"<<ih<<",QPDG="<<curH->GetQPDG()//^ ^
                          <<", 4M="<<ph4M<<", ct="<<phCost<<G4endl; //                  ^ ^
#endif
                    theQHadrons.push_back(curH); // Fill hadron-copy (delete equivalent)^ ^
                    totQC-=curH->GetQC();     //                                        ^ ^
                    tot4M-=curH->Get4Momentum(); //                                     ^ ^
				              }                           //                                        ^ ^
				            }                             // ==> End of the LOOP over outQHadrons   ^ ^
                pQ->ClearOutput();            // Hadrons are filled, Clear Frag-out <-<-^ ^
                count3=0;                     // Reset counter of empty hadronizations    ^
	             }                               //                                          ^
              else count3++;                  // Increment counter of empty hadronizations^
			         }                                 //                                          ^
            else if(status<0||status==2)      // => "PANIC or NOTHING was done" case      ^
            {                                 //                                          ^
#ifdef pdebug
	             G4cout<<"G4QE::HQE:***PANIC***,status="<<status<<",nC="<<nCount<<G4endl; // ^
#endif
														nCount++;                       //                                          ^
              if(eCount==1 && status<0 && CheckGroundState(pQ,true))// Correct & Finish   ^
              {                               //                                          ^
                for_each(output->begin(), output->end(), DeleteQHadron()); // ->->->------^
                output->clear();              //                                          ^
                delete output;                // >========================================^
                pQ->KillQuasmon();            // If BackFusion succeeded, kill the Quasmon^
                delete pQ;
                eCount--;                     // Reduce the number of the living Quasmons ^
                return theQHadrons;           //                                          ^
              }                               //                                          ^
              else if(status<0&&nHadrons)     // This is just a confusion in the status...^
              {                               //                                          ^
		              G4cerr<<"***G4QEnv::HadrQE: nH="<<nHadrons<<"< status="<<status<<G4endl;//^
                for_each(output->begin(), output->end(), DeleteQHadron()); // -->-->-->---^
                output->clear();              //                                          ^
                delete output;                // >========================================^
                throw G4QException("G4QEnvironment::HadronizeQEnvironment:DoNothingEr");//^
			           }                               //                                          ^
              else if(status==2)              // Treat PANIC for stat=2 (NothingWasDone)  ^
              {                               //                                          ^
                G4QContent qQC=pQ->GetQC();   // QuarkContent of the Quasmon              ^
                G4int      pqC=qQC.GetCharge(); // Charge (nP) of the Current Quasmon     ^
                G4int      pqS=qQC.GetStrangeness(); // Strangeness (nL) of theCurrQuasmon^
                G4int      pqB=qQC.GetBaryonNumber(); // BaryNumber of the CurrentQuasmon ^
                G4LorentzVector cq4M=pQ->Get4Momentum(); // 4Mom of the Current Quasmon   ^
                G4double cqMass=cq4M.m();     // Real Mass of the current Quasmon         ^
                G4double fqMass=G4QPDGCode(22).GetNuclMass(pqC,pqB-pqC-pqS,pqS);//CQ FreeM^
#ifdef edebug
    		          G4cout<<"G4QEnv::HQE:M="<<cqMass<<">fM="<<fqMass<<",S="<<pqS<<",C="<<pqC//^
                      <<",ePDG="<<theEnvironment.GetPDG()<<",qQC="<<qQC<<",eC="<<eCount //^
                      <<G4endl;                                                     //    ^
#endif
                if(pqB>0&&pqS<0&&cqMass>fqMass)// "AntiStrangeNucleus-Chipolino" case     ^
				            {                             //                                          ^
                  G4QHadron* nuclQ = new G4QHadron(qQC,cq4M);// Hadron for AntiStrangeNuc.^
                  DecayAntiStrange(nuclQ);    // Decay the AntyStrangeNucl (Del.Equiv.)   ^
                  pQ->KillQuasmon();          // If BackFusion succeeded, kill theQuasmon ^
#ifdef edebug
     		           G4cout<<"G4QEnv::HQE:Status after kill (#"<<jq<<")="<<pQ->GetStatus()// ^
                        <<", nH="<<theQHadrons.size()<<G4endl;                         // ^
#endif
                  tot4M-=cq4M;                // Update TotalResidNucleus for hadronizPro.^
                  totQC-=qQC;                 // Update total QC for the HadronizationPro.^
                  eCount--;                   // Reduce the number of the living Quasmons ^
                }                             //                                          ^
                else if(theEnvironment.GetPDG()!=NUCPDG) // ==> "NuclearEnvironment" case ^
				            {                             //                                          ^
                  if(eCount>1)                //                                          ^
																		{                           //                                          ^
#ifdef fdebug
		                  G4cout<<"G4QE::HQE:TOTEVAP tPDG="<<totPDG<<",t4M="<<tot4M<<G4endl; // ^
#endif
                    G4QHadron* evH = new G4QHadron(totQC,tot4M); // Create Hadron-ResidNuc^
                    CleanUp();                //                                          ^
                    EvaporateResidual(evH);   // Evaporate ResNuc (delete equivalemt)     ^
                    for_each(output->begin(), output->end(), DeleteQHadron());// >--------^
                    output->clear();          //                                          ^
                    delete output;            // >==========>===========>=================^
                    return theQHadrons;       //                                          ^
                  }                           //                                          ^
                  G4LorentzVector t4M=cq4M+theEnvironment.Get4Momentum(); // Q+E tot4Mom  ^
                  G4double      tM=t4M.m();   // Real total (Quasmon+Environment) mass    ^
                  G4QContent envQC=theEnvironment.GetQCZNS(); // QuarkCont of NucEnviron  ^
                  G4QContent curQC=envQC+qQC; // Total Quark Content                      ^
                  G4QNucleus curE(curQC);     // Pseudo nucleus for the Total System      ^
                  G4double   curM=curE.GetGSMass();// min mass of the Total System        ^
#ifdef edebug
    		            G4cout<<"G4QEnv::HQE:Q#"<<jq<<",tM="<<tM<<">gsM="<<curM<<curE<<G4endl;//^
#endif
                  if(tM<curM)                 //                                          ^
                  {
                    G4int qPDG=qQC.GetZNSPDGCode();// PDG Code of the Quasmon             ^
                    G4double qMass=G4QPDGCode(qPDG).GetMass(); // GroundStM of theQuasmon ^
#ifdef edebug
    		              G4cout<<"G4QE::HQE:nQ="<<nQuasmons<<",eC="<<eCount<<",qPDG="<<qPDG // ^
                          <<",qM="<<qMass<<",eM="<<envM<<",tM="<<tM<<",Q+E="<<qMass+envM//^
                          <<G4endl;           //                                          ^
#endif
                    if(eCount==1&&qPDG&&qMass&&tM>qMass+envM)//==> Q+E decay for one Quasm^
					               //if(nQuasmons==1 && qPDG && qMass && tM>qMass+envM) // ==> Q+E decay ^
                    {                         //                                          ^
                      G4int envPDG = theEnvironment.GetPDG(); // PDGCode of the NuclQEnv. ^
#ifdef edebug
		                    G4cout<<"G4QEnv::HadrQEnv: Q+E decay, nQ=1, qPDG=="<<qPDG<<G4endl;//^
#endif
                      // => "Quasmon-Chipolino or Environment-Dibaryon" case              ^
                      if(qPDG==10 || qPDG==92000000 || qPDG==90002000 || qPDG==90000002)//^
																						{                          //                                       ^
                        G4QPDGCode h1QPDG=nQPDG; // QPDG of the first hadron              ^
                        G4double   h1M   =mNeut; // Mass of the first hadron              ^
                        G4QPDGCode h2QPDG=h1QPDG;// QPDG of the second hadron             ^
                        G4double   h2M   =mNeut; // Mass of the second hadron             ^
                        if(qPDG==10)             // CHIPOLINO decay case                  ^
						                  {                     //                                          ^
                          G4QChipolino QChip(qQC);// define the Quasmon as a Chipolino    ^
                          h1QPDG=QChip.GetQPDG1();// QPDG of the first hadron             ^
                          h1M   =h1QPDG.GetMass();// Mass of the first hadron             ^
                          h2QPDG=QChip.GetQPDG2();// QPDG of the second hadron            ^
                          h2M   =h2QPDG.GetMass();// Mass of the second hadron            ^
			                     }                     //                                          ^
			                     else if(qPDG==90002000) // DiProton decay case                    ^
                        {                     //                                          ^
                          h1QPDG=pQPDG;       // QPDG of the first hadron                 ^
                          h1M   =mProt;       // Mass of the first hadron                 ^
                          h2QPDG=h1QPDG;      // QPDG of the second hadron                ^
                          h2M   =mProt;       // Mass of the second hadron                ^
			                     }                     //                                          ^
			                     else if(qPDG==92000000) // Two lambdas case                       ^
                        {                     //                                          ^
                          h1QPDG=lQPDG;       // QPDG of the first hadron                 ^
                          h1M   =mLamb;       // Mass of the first hadron                 ^
                          h2QPDG=h1QPDG;      // QPDG of the second hadron                ^
                          h2M   =mLamb;       // Mass of the second hadron                ^
                          G4double ddMass=totMass-envM; // Free CM energy                 ^
                          if(ddMass>mSigZ+mSigZ) // Sigma0+Sigma0 is possible             ^
						                    {                   // @@ Only two particles PS is used         ^
                            G4double dd2=ddMass*ddMass; // Squared free energy            ^
                            G4double sma=mLamb+mLamb; // Lambda+Lambda sum                ^
                            G4double pr1=0.;          // Prototype to avoid sqrt(-)       ^
                            if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Lamb+Lamb PS   ^
                            sma=mLamb+mSigZ;          // Lambda+Sigma0 sum                ^
                            G4double smi=mSigZ-mLamb; // Sigma0-Lambda difference         ^
                            G4double pr2=pr1;         // Prototype of +L+S0 PS            ^
																												if(ddMass>sma&&ddMass>smi) //                                 ^
																													 pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi)); //                  ^
                            sma=mSigZ+mSigZ;          // Sigma0+Sigma0 sum                ^
                            G4double pr3=pr2;         // Prototype of +Sigma0+Sigma0 PS   ^
                            if(ddMass>sma) pr3+=sqrt((dd2-sma*sma)*dd2); //               ^
                            G4double hhRND=pr3*G4UniformRand(); // Randomize PS           ^
                            if(hhRND>pr2)     // --> "ENnv+Sigma0+Sigma0" case            ^
                            {                 //                                          ^
                              h1QPDG=s0QPDG;  // QPDG of the first hadron                 ^
                              h1M   =mSigZ;   // Mass of the first hadron                 ^
                              h2QPDG=h1QPDG;  // QPDG of the second hadron                ^
                              h2M   =mSigZ;   // Mass of the second hadron                ^
                            }                 //                                          ^
                            else if(hhRND>pr1)// --> "ENnv+Sigma0+Lambda" case            ^
                            {                 //                                          ^
                              h1QPDG=s0QPDG;  // QPDG of the first hadron                 ^
                              h1M   =mSigZ;   // Mass of the first hadron                 ^
                            }                 //                                          ^
                          }                   //                                          ^
                          else if(ddMass>mSigZ+mLamb) // Lambda+Sigma0 is possible        ^
						                    {                   // @@ Only two particles PS is used         ^
                            G4double dd2=ddMass*ddMass; // Squared free energy            ^
                            G4double sma=mLamb+mLamb; // Lambda+Lambda sum                ^
                            G4double pr1=0.;          // Prototype to avoid sqrt(-)       ^
                            if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Lamb+Lamb PS   ^
                            sma=mLamb+mSigZ;          // Lambda+Sigma0 sum                ^
                            G4double smi=mSigZ-mLamb; // Sigma0-Lambda difference         ^
                            G4double pr2=pr1;         //+L+S0 PS                          ^
                            if(ddMass>sma && ddMass>smi) //                               ^
																														pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi)); //                  ^
                            if(pr2*G4UniformRand()>pr1) // --> "ENnv+Sigma0+Lambda" case  ^
                            {                 //                                          ^
                              h1QPDG=s0QPDG;  // QPDG of the first hadron                 ^
                              h1M   =mSigZ;   // Mass of the first hadron                 ^
                            }                 //                                          ^
                          }                   //                                          ^
			                     }                     //                                          ^
                        if(h1M+h2M+envM<totMass) // => "Three parts decay" case           ^
                        {                     //                                          ^
                          G4LorentzVector h14M(0.,0.,0.,h1M);           //                ^
                          G4LorentzVector h24M(0.,0.,0.,h2M);           //                ^
                          G4LorentzVector e4M(0.,0.,0.,envM);           //                ^
                          if(!G4QHadron(tot4M).DecayIn3(h14M,h24M,e4M)) //                ^
                          {                   //                                          ^
				                        G4cerr<<"***G4QE::HQE:(0)tM="<<tot4M.m()<<"->h1="<<h1QPDG //  ^
                                  <<"("<<h1M<<")+h2="<<h1QPDG<<"("<<h2M<<")+envM="<<envM//^
                                  <<"=="<<h1M+h2M+envM<<G4endl; //                        ^
				                        throw G4QException("**G4QE::HadrQEnv:QChip+E DecIn3 error");//^
			                       }                   //                                          ^
                          G4QHadron* h1H = new G4QHadron(h1QPDG.GetPDGCode(),h14M); //    ^
                          theQHadrons.push_back(h1H);        // (delete equivalent)       ^
#ifdef pdebug
						                    G4cout<<"G4QE::HQE:(1) H1="<<h1QPDG<<h14M<<G4endl;        //    ^
#endif
                          G4QHadron* h2H = new G4QHadron(h2QPDG.GetPDGCode(),h24M); //    ^
                          theQHadrons.push_back(h2H);        // (delete equivalent)       ^
#ifdef pdebug
                          G4cout<<"G4QE::HQE:(1) H2="<<h2QPDG<<h24M<<G4endl;        //    ^
#endif
                          G4QHadron* qeH = new G4QHadron(envPDG,e4M);               //    ^
                          theQHadrons.push_back(qeH);        // (delete equivalent)       ^
#ifdef pdebug
                          G4cout<<"G4QE::HQE:(1) QEnv="<<envPDG<<e4M<<G4endl;       //    ^
#endif
			                     }                     //                                          ^
			                     else                  // Try to recover                           ^
                        {                     //                                          ^
                          //if(eCount==1&&CheckGroundState(pQ,true)) // @@ BackFusion     ^
                          if(eCount==1&&CheckGroundState(pQ))// BackFusion attempt        ^
						                    {                   //                                          ^
                            pQ->KillQuasmon();// ??                                       ^
                            eCount--;         // Reduce a#of theLivingQuasmons            ^
                            for_each(output->begin(),output->end(),DeleteQHadron()); //   ^
                            output->clear();  // -->-->-->-->-->-->-->-->-->-->-->-->-->--^
                            delete output;    // >========================================^
                            return theQHadrons; //                                        ^
                          }                   //                                          ^
                          for_each(output->begin(),output->end(),DeleteQHadron()); //-->--^
                          output->clear();    // -->-->-->-->-->-->-->-->-->-->-->-->-->--^
                          delete output;      // >========================================^
#ifdef fdebug
                          G4cerr<<"--Warning--G4QE::HQE:tM="<<tot4M.m()<<"< h1="<<h1QPDG//^
                                <<"(M="<<h1M<<")+h2="<<h1QPDG<<"(M="<<h2M<<")+EM="<<envM//^
                                <<"="<<h1M+h2M+envM<<G4endl; //                           ^
			                       //throw G4QException("G4QEnv::HQE:(0)Chi+Env mass > totMass");//^
#endif
                          CleanUp();          //                                          ^
                          G4QHadron* evH = new G4QHadron(totQC,tot4M);// ResidualNuclHadr ^
                          EvaporateResidual(evH);  // Evaporate residual (del. equiv.)    ^
                          return theQHadrons; //                                          ^
			                     }                     //                                          ^
		                    }                       //                                          ^
                      else                    // => "Two particles decay" case            ^
                      {                       //                                          ^
                        G4LorentzVector fq4M(0.,0.,0.,qMass); //                          ^
                        G4LorentzVector qe4M(0.,0.,0.,envM);  //                          ^
                        if(!G4QHadron(tot4M).RelDecayIn2(fq4M,qe4M,cq4M,1.,1.))//Q ch.dir.^
                        {                     //                                          ^
                          G4cerr<<"***G4QEnv::HadQE:(0)tM="<<tot4M.m()<<"-> qPDG="<<qPDG//^
                                <<"(M="<<qMass<<") + envM="<<envM<<")"<<G4endl;         //^
                          for_each(output->begin(),output->end(),DeleteQHadron()); //->->-^
                          output->clear();    // -->-->-->-->-->-->-->-->-->-->-->-->-->--^
                          delete output;      // >========================================^
				                      throw G4QException("***G4QEnv::HadrQEnv: Q+Env DecIn2 error");//^
			                     }                     //                                          ^
                        G4QHadron* qH = new G4QHadron(qPDG,fq4M);// the out going Quasmon ^
                        theQHadrons.push_back(qH); // (delete equivalent)                 ^
#ifdef pdebug
						                  G4cout<<"G4QE::HQE:QuasmH="<<qPDG<<fq4M<<G4endl;         //       ^
#endif
                        G4QHadron* qeH = new G4QHadron(envPDG,qe4M);//theRecoilEnvironment^
#ifdef pdebug
						                  G4cout<<"G4QE::HQE:EnvironH="<<envPDG<<qe4M<<G4endl;     //       ^
#endif
                        if(envPDG==92000000||envPDG==90002000||envPDG==90000002) //       ^
						                                           DecayDibaryon(qeH); // ^
                        else theQHadrons.push_back(qeH);// (del.equiv.) *** this too ***  ^
		                    }                       //                                          ^
                      for_each(output->begin(),output->end(),DeleteQHadron());//-->-->-->-^
                      output->clear();        //                                          ^
                      delete output;          // >========================================^
                      CleanUp();              // Clean up Environ and Quasmon             ^
                      return theQHadrons;     // Finish the hadronization process         ^
                    }                         //                                          ^
                    else status=-1;           // Q+E && totM below MassShell - PANIC      ^
                  }                           //                                          ^
                  else if(eCount>1&&(nCount>nCnMax||theEnvironment.GetA()<2))// 2Quasmons ^
																		{                                  //                                   ^
                    theEnvironment.InitByPDG(NUCPDG);// KillEnvironment(@@ Q's? CleanUp)) ^
#ifdef fdebug
					               G4cerr<<"G4QEnv::HQE:Evaporate Env+Quasm Env="<<curE<<G4endl;//       ^
#endif
                    G4QHadron* nucQE = new G4QHadron(curQC,t4M);// Hadron for Quasm+Envir.^
                    EvaporateResidual(nucQE); // Evaporate residual Quasm+Env(del.equiv.) ^
                    pQ->KillQuasmon();        // If BackFusion succeeded, kill theQuasmon ^
#ifdef edebug
     		             G4cout<<"G4QEnv::HQE:StatusAfterKill (#"<<jq<<")="<<pQ->GetStatus()// ^
                          <<", nH="<<theQHadrons.size()<<G4endl;                       // ^
#endif
                    tot4M-=t4M;               // Update TotalResidNucleus for hadronizPro.^
                    totQC-=curQC;             // Update total QC for the HadronizationPro.^
                    eCount--;                 // Reduce the number of the living Quasmons ^
                  }                           //                                          ^
                  if(eCount==1 && tM>=curM)   //==>for one Quasmon evaporate ResTotN      ^
                  {                           //                                          ^
                    theEnvironment.InitByPDG(NUCPDG); // Cancele the Environment          ^
                    G4int ttPDG=totQC.GetSPDGCode(); // Total PDG Code (10 - Chipolino)   ^
#ifdef pcdebug
                    G4cout<<"G4QE::HQE:BefEv 4M="<<tot4M<<",QC="<<totQC<<ttPDG<<G4endl;// ^
#endif
                    for_each(output->begin(),output->end(),DeleteQHadron()); //-->-->-->--^
                    output->clear();          //                                          ^
                    delete output;            // >========================================^
                    CleanUp();                // Clean up the Environ and Quasmons        ^
                    G4int ttBN=totQC.GetBaryonNumber(); //                                ^
                    if(ttPDG==10&&ttBN<2)     // Chipolino case                           ^
					               {                         //                                          ^
                      G4QChipolino QCh(totQC);// define the TotalResidual as a Chipolino  ^
                      G4QPDGCode   h1QPDG=QCh.GetQPDG1();  // QPDG of the first hadron    ^
                      G4double     h1M   =h1QPDG.GetMass();// Mass of the first hadron    ^
                      G4QPDGCode   h2QPDG=QCh.GetQPDG2();  // QPDG of the second hadron   ^
                      G4double     h2M   =h2QPDG.GetMass();// Mass of the second hadron   ^
                      G4double ttM=tot4M.m(); // Mass of the Chipolino                    ^
                      if(h1M+h2M<ttM)         // Two particles decay of Chipolino is pos. ^
                      {                       //                                          ^
                        G4LorentzVector h14M(0.,0.,0.,h1M);       //                      ^
                        G4LorentzVector h24M(0.,0.,0.,h2M);       //                      ^
                        if(!G4QHadron(tot4M).DecayIn2(h14M,h24M)) //                      ^
                        {                     //                                          ^
				                      G4cerr<<"***G4QE::HQE:tM="<<ttM<<"->h1="<<h1QPDG<<"("<<h1M //   ^
                                <<")+h2="<<h1QPDG<<"("<<h2M<<"="<<h1M+h2M<<G4endl;   //   ^
				                      throw G4QException("**G4QE::HadrQEnv:QChip (1) DecIn2 error");//^
			                     }                     //                                          ^
                        G4QHadron* h1H = new G4QHadron(h1QPDG.GetPDGCode(),h14M);    //   ^
                        theQHadrons.push_back(h1H);               // (delete equivalent)  ^
#ifdef pdebug
						                  G4cout<<"G4QE::HQE: QCip-> H1="<<h1QPDG<<h14M<<G4endl;       //   ^
#endif
                        G4QHadron* h2H = new G4QHadron(h2QPDG.GetPDGCode(),h24M);    //   ^
                        theQHadrons.push_back(h2H);               // (delete equivalent)  ^
#ifdef pdebug
                        G4cout<<"G4QE::HQE: QChip->H2="<<h2QPDG<<h24M<<G4endl;       //   ^
#endif
                      }                       //                                          ^
                      else                    //                                          ^
                      {                       //                                          ^
				                    G4cerr<<"***G4QE::HQE:tM="<<ttM<<totQC<<"->h1="<<h1QPDG<<"(" //   ^
                              <<h1M<<")+h2="<<h1QPDG<<"("<<h2M<<"="<<h1M+h2M<<G4endl;//   ^
				                    throw G4QException("***G4QE::HadrQEnv:QChip (2) DecIn2 error");// ^
			                   }                       //                                          ^
                    }                         //                                          ^
                    else                      //                                          ^
					               {                         //                                          ^
#ifdef edebug
                      if(ttPDG<80000000&&ttBN<1) //                                       ^
                        G4cerr<<"---Warning---G4QE::HQE: NotNuc, tPDG="<<ttPDG<<G4endl;// ^
#endif
                      G4QHadron* evH = new G4QHadron(totQC,tot4M);// Hadron for ResidNucl ^
                      EvaporateResidual(evH); // Evaporate residual (del.equiv.)          ^
                    }                         //                                          ^
                    return theQHadrons;       //                                          ^
                  }                           //                                          ^
                  else if(eCount==1 && CheckGroundState(pQ,true)) // Correct and Finish   ^
                  {                           //                                          ^
                    for_each(output->begin(), output->end(), DeleteQHadron()); // -->-->--^
                    output->clear();          //                                          ^
                    delete output;            // >========================================^
                    pQ->KillQuasmon();        // If BackFusion succeeded, kill Quasm      ^
                    delete pQ;
                    eCount--;                 // Reduce a#of the living Quasmons          ^
                    return theQHadrons;       //                                          ^
                  }                           //                                          ^
                }                             //                                          ^
                else                          // "Vacuum" case                            ^
                {                             //                                          ^
                  G4QPDGCode QPDGQ=pQ->GetQPDG(); // QPDG Code for the Quasmon            ^
                  G4int PDGQ=QPDGQ.GetPDGCode();  // PDG Code of the QUASMON              ^
#ifdef edebug
				              G4cout<<"G4QEnv::HadrQEnv: vacuum PDGQ="<<PDGQ<<G4endl; //              ^
#endif
                  if(!PDGQ) status=-1;        // Unknown Quasmon in Vaquum - PANIC        ^
                  // @@ There still can be a case for 2pSigma+ or 2nSigma- (PDGCode?)     ^
                  else if(PDGQ==3112||PDGQ==3222||PDGQ==90999001||PDGQ==91000999)// S+/S- ^
                  {                           //                                          ^
#ifdef edebug
				                G4cout<<"G4QEnv::HadrQEnv:Sigma Mass="<<cqMass<<G4endl;      //       ^
#endif
                    G4double hyM=mNeut;       // Prototype of the hyperon mass            ^
                    G4int    hyPDG=2112;      // Prototype of the hyperon PDG Code        ^
                    G4double pigM=mPi;        // Prototype of the gamma/pion mass         ^
                    G4int    pigPDG=-211;     // Prototype of the gamma/pion PDG Code     ^
                    if(PDGQ==3112||PDGQ==90999001) // --> "Sigma-" case                   ^
					               { //                                                                  ^
                      if(cqMass>mPi+mLamb)    // "Lambda + Pi- is possible" case          ^
					                 { //                                                                ^
                        hyM   = mLamb;        // Lambda mass                              ^
                        hyPDG = 3122;         // Lambda PDG Code                          ^
					                 } //                                                                ^
                      else if(cqMass>mSigM)   // "Sigma- gamma decay" case                ^
					                 { //                                                                ^
                        hyM=mSigM;            // Sigma- mass                              ^
                        hyPDG=3112;           // Sigma- PDG Code                          ^
                        pigM=0.;              // Gamma mass                               ^
                        pigPDG=22;            // Gamma PDG Code                           ^
					                 } //                                                                ^
                    } //                                                                  ^
                    else if(PDGQ==3222||PDGQ==91000999) // --> "Sigma+" case              ^
					               { //                                                                  ^
                      pigPDG= 211;            // Pi+ PDG Code                             ^
                      if(cqMass>mPi+mLamb)    // --- "Lambda + Pi+ is possible" case      ^
					                 { //                                                                ^
                        hyM   = mLamb;        // Lambda mass                              ^
                        hyPDG = 3122;         // Lambda PDG Code                          ^
                        pigM  = mPi;          // Pi+ mass                                 ^
                        pigPDG= 211;          // Pi+ PDG Code                             ^
                      } //                                                                ^
                      else if(cqMass>mSigP)   // "Sigma- gamma decay" case                ^
					                 { //                                                                ^
                        hyM=mSigP;            // Sigma+ mass                              ^
                        hyPDG=3222;           // Sigma+ PDG Code                          ^
                        pigM=0.;              // Gamma mass                               ^
                        pigPDG=22;            // Gamma PDG Code                           ^
					                 } //                                                                ^
                      else if(cqMass>mPi0+mProt&&G4UniformRand()>.5) // "P + Pi0" case    ^
					                 { //                                                                ^
                        hyM   = mProt;        // Proton mass                              ^
                        hyPDG = 2212;         // Proton PDG Code                          ^
                        pigM  = mPi0;         // Pi0 mass                                 ^
                        pigPDG= 111;          // Pi0 PDG Code                             ^
					                 } //                                                                ^
                      else if(cqMass<mPi+mNeut)// "P+gamma" case as "N+Pi+" is impossible ^
					                 { //                                                                ^
                        hyM   = mProt;        // Proton mass                              ^
                        hyPDG = 2212;         // Proton PDG Code                          ^
                        pigM=0.;              // Gamma mass                               ^
                        pigPDG=22;            // Gamma PDG Code                           ^
					                 } //                                                                ^
                      // othing should be done for "N + P+" case                          ^
                    } //                                                                  ^
                    G4LorentzVector b4Mom(0.,0.,0.,hyM); // Hyperon mass                  ^
                    G4LorentzVector m4Mom(0.,0.,0.,pigM);// pion/gamma mass               ^
                    if(!G4QHadron(cq4M).DecayIn2(b4Mom, m4Mom)) // "DecayIn2 failed" case ^
					               { //                                                                  ^
                      G4cerr<<"---Warning---G4QE::HQE:H="<<hyPDG<<"(m="<<hyM<<")+G/Pi=" //^
							                     <<pigPDG<<"(m="<<pigM<<")="<<hyM+pigM<<">"<<cqMass<<G4endl; //^
                      G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC             ^
                      CleanUp();              //                                          ^
		                    if(!CheckGroundState(quasH,true)) // Last posibility to correct     ^
					                 { //                                                                ^
                        G4QHadron* hadr = new G4QHadron(totQC,tot4M); //                  ^
                        theQHadrons.push_back(hadr);    // Cor or fill as It Is           ^
#ifdef pdebug
                        G4cerr<<"-Warn-G4QE::HQE:Sig,QC="<<totQC<<",4M="<<tot4M<<G4endl;//^
#endif
    	                   //throw G4QException("G4QEnvironment::HadronizeQEnv:Sig error");//^
                      } //                                                                ^
                      delete quasH;          // Delete the temporary fake Quasmon         ^
                      return theQHadrons;    //                                           ^
                    } //                                                                  ^
#ifdef pdebug
	                   G4cout<<"G4QEnv::HadronizeQEnv: Sigma="<<PDGQ<<cq4M<<" -> Hyperon="// ^
                          <<hyPDG<<b4Mom<<" + Gamma/Pi="<<pigPDG<<m4Mom<<G4endl; //       ^
#endif
                    G4QHadron* curBar = new G4QHadron(hyPDG,b4Mom); //                    ^
                    theQHadrons.push_back(curBar); // Fill the Hyperon (delete equivalent)^
                    G4QHadron* curMes = new G4QHadron(pigPDG,m4Mom);  //                  ^
                    theQHadrons.push_back(curMes); // Fill the gam/pi (delete equivalent) ^
                    pQ->KillQuasmon();        // Make done the current Quasmon            ^
                    tot4M-=cq4M;              // Update theTotalResidNucl of HadrPr.      ^
                    totQC-=qQC;               // Update total residual QC of HadrPr.      ^
                    eCount--;                 // Reduce a#of the living Quasmons          ^
				              }                           //                                          ^
                  else if(PDGQ==90999002||PDGQ==91001999) // pS+/nS-                      ^
                  {                           //                                          ^
#ifdef edebug
				                G4cout<<"G4QEnv::HadrQEnv: Nucleon+Sigma Mass="<<cqMass<<G4endl; //   ^
#endif
                    G4bool dinFlag = false;   // Di-nucleon flag                          ^
                    G4double hyM=mSigM;       // Prototype of the hyperon mass (n+Sigma-) ^
                    G4int    hyPDG=3112;      // Prototype of the hyperon PDG Code        ^
                    G4double pigM=mNeut;      // Prototype of the nucleon mass            ^
                    G4int    pigPDG=2112;     // Prototype of the nucleon PDG Code        ^
                    if     (PDGQ==90999002)   // --> "n+Sigma-" case                      ^
					               {                         //                                          ^
                      if(cqMass<mNeut+mSigM)  // ----> "DiNeutron+Pi-" case               ^
					                 { //                                                                ^
                        dinFlag = true;       // For the final decay                      ^
                        hyM=mNeut+mNeut;      // Di-neutron                               ^
                        hyPDG=2112;           // Neutron PDG Code                         ^
                        pigM=mPi;             // Pi- mass                                 ^
                        pigPDG=-211;          // Pi- PDG Code                             ^
					                 } //                                                                ^
                    } //                                                                  ^
                    else if(PDGQ==91001999)   // --> "p+Sigma+" case                      ^
					               { //                                                                  ^
                      hyM=mSigP;              // Sigma+                                   ^
                      hyPDG=3222;             // PDG Code of Sigma+                       ^
                      pigM=mProt;             // Proton mass                              ^
                      pigPDG=2212;            // PDG Code of proton                       ^
                      if(cqMass<mProt+mSigP)  // ----> "Proton+Proton" case               ^
					                 { //                                                                ^
                        hyM=mProt;            // Proton mass                              ^
                        hyPDG=2212;           // Proton PDG Code                          ^
                        pigM=mProt;           // Proton mass                              ^
                        pigPDG=2212;          // Proton PDG Code                          ^
					                 } //                                                                ^
                    } //                                                                  ^
                    G4LorentzVector b4Mom(0.,0.,0.,hyM); // Hyperon (di-nucleon) mass     ^
                    G4LorentzVector m4Mom(0.,0.,0.,pigM);// Nucleon (pion) mass           ^
                    if(!G4QHadron(cq4M).DecayIn2(b4Mom, m4Mom)) // "DecayIn2 failed" case ^
					               { //                                                                  ^
                      G4cerr<<"--Warning--G4QE::HQE:S/D="<<hyPDG<<"(m="<<hyM<<")+N/Pi=" //^
							                     <<pigPDG<<"(m="<<pigM<<")="<<hyM+pigM<<">"<<cqMass<<G4endl; //^
                      G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC             ^
                      CleanUp();              //                                          ^
		                    if(!CheckGroundState(quasH,true)) // Last posibility to correct     ^
					                 { //                                                                ^
                        G4QHadron* hadr = new G4QHadron(totQC,tot4M); //                  ^
                        theQHadrons.push_back(hadr);    // Cor or fill as It Is           ^
#ifdef pdebug
                        G4cerr<<"-Warn-G4QE::HQE:Sig,QC="<<totQC<<",4M="<<tot4M<<G4endl;//^
#endif
    	                   //throw G4QException("G4QEnvironment::HadronizeQEnv:Sig error");//^
                      } //                                                                ^
                      delete quasH;          // Delete the temporary fake Quasmon         ^
                      return theQHadrons;    //                                           ^
                    } //                                                                  ^
#ifdef pdebug
	                   G4cout<<"G4QEnv::HadronizeQEnv: NSigma="<<PDGQ<<cq4M<<"-> Sigma/dN="//^
                          <<hyPDG<<b4Mom<<" + N/Pi="<<pigPDG<<m4Mom<<G4endl; //           ^
#endif
                    if(dinFlag) b4Mom/=2.;    // Split the 4-mom for the dinucleon        ^
                    G4QHadron* curBar = new G4QHadron(hyPDG,b4Mom); //                    ^
                    theQHadrons.push_back(curBar); // Fill the Hyperon (delete equivalent)^
                    if(dinFlag)               //                                          ^
					               {                         //                                          ^
                      G4QHadron* secBar = new G4QHadron(hyPDG,b4Mom);// Cre. 2-nd nucleon ^
                      theQHadrons.push_back(secBar);// Fill 2-nd nucleon (delete equiv.)  ^
                    }                         //                                          ^
                    G4QHadron* curMes = new G4QHadron(pigPDG,m4Mom);  //                  ^
                    theQHadrons.push_back(curMes); // Fill the gam/pi (delete equivalent) ^
                    pQ->KillQuasmon();        // Make done the current Quasmon            ^
                    tot4M-=cq4M;              // Update theTotalResidNucl of HadrPr.      ^
                    totQC-=qQC;               // Update total residual QC of HadrPr.      ^
                    eCount--;                 // Reduce a#of the living Quasmons          ^
				              }                           //                                          ^
                  else if(PDGQ==90999003||PDGQ==91002999) // ppS+/nnS-                    ^
                  {                           //                                          ^
#ifdef edebug
				                G4cout<<"G4QEnv::HadrQEnv: DiNucleon+Sigma Mass="<<cqMass<<G4endl; // ^
#endif
                    G4bool dinFlag = false;   // Di-nucleon flag                          ^
                    G4double hyM=mSigM;       // Prototype of the hyperon mass (n+Sigma-) ^
                    G4int    hyPDG=3112;      // Prototype of the hyperon PDG Code        ^
                    G4double pigM=mNeut+mNeut;// Prototype of the di-nucleon mass         ^
                    G4int    pigPDG=2112;     // Prototype of the nucleon PDG Code        ^
                    if     (PDGQ==90999003)   // --> "n+Sigma-" case                      ^
					               {                         //                                          ^
                      if(cqMass<pigM+mSigM)   // ----> "DiNeutron+Pi-" case               ^
					                 { //                                                                ^
                        dinFlag = true;       // For the final decay                      ^
                        pigM=mNeut+mNeut+mNeut;// Tri-neutron                             ^
                        pigPDG=2112;          // Neutron PDG Code                         ^
                        hyM=mPi;              // Pi- mass                                 ^
                        hyPDG=-211;           // Pi- PDG Code                             ^
					                 } //                                                                ^
                    } //                                                                  ^
                    else if(PDGQ==91002999)   // --> "p+Sigma+" case                      ^
					               { //                                                                  ^
                      hyM=mSigP;              // Sigma+                                   ^
                      hyPDG=3222;             // PDG Code of Sigma+                       ^
                      pigM=mProt+mProt;       // Di-Proton mass                           ^
                      pigPDG=2212;            // PDG Code of proton                       ^
                      if(cqMass<pigM+mSigP)   // ----> "DiProton+Pi+" case                ^
					                 { //                                                                ^
                        dinFlag = true;       // For the final decay                      ^
                        pigM=mProt+mProt+mProt;// Tri-proton                              ^
                        pigPDG=2212;          // Neutron PDG Code                         ^
                        hyM=mPi;             // Pi+ mass                                  ^
                        hyPDG=211;           // Pi+ PDG Code                              ^
					                 } //                                                                ^
                    } //                                                                  ^
                    G4LorentzVector b4Mom(0.,0.,0.,hyM); // Hyperon (di-nucleon) mass     ^
                    G4LorentzVector m4Mom(0.,0.,0.,pigM);// Nucleon (pion) mass           ^
                    if(!G4QHadron(cq4M).DecayIn2(b4Mom, m4Mom)) // "DecayIn2 failed" case ^
					               { //                                                                  ^
                      G4cerr<<"--Warning--G4QE::HQE:S/Pi="<<hyPDG<<"(m="<<hyM<<")+D/T=" //^
							                     <<pigPDG<<"(m="<<pigM<<")="<<hyM+pigM<<">"<<cqMass<<G4endl; //^
                      G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC             ^
                      CleanUp();              //                                          ^
		                    if(!CheckGroundState(quasH,true)) // Last posibility to correct     ^
					                 { //                                                                ^
                        G4QHadron* hadr = new G4QHadron(totQC,tot4M); //                  ^
                        theQHadrons.push_back(hadr);    // Cor or fill as It Is           ^
#ifdef pdebug
                        G4cerr<<"-Warn-G4QE::HQE:Sig,QC="<<totQC<<",4M="<<tot4M<<G4endl;//^
#endif
    	                   //throw G4QException("G4QEnvironment::HadronizeQEnv:Sig error");//^
                      } //                                                                ^
                      delete quasH;          // Delete the temporary fake Quasmon         ^
                      return theQHadrons;    //                                           ^
                    } //                                                                  ^
#ifdef pdebug
	                   G4cout<<"G4QEnv::HadronizeQEnv:2NSigma="<<PDGQ<<cq4M<<"-> Sigma/Pi="//^
                          <<hyPDG<<b4Mom<<" + 2N/3N="<<pigPDG<<m4Mom<<dinFlag<<G4endl; // ^
#endif
                    G4QHadron* curBar = new G4QHadron(hyPDG,b4Mom); //                    ^
                    theQHadrons.push_back(curBar); // Fill the Hyperon (delete equivalent)^
                    if(dinFlag) m4Mom/=3.;   // Split the 4-mom for the dinucleon in 3    ^
                    else m4Mom/=2.;          // Split the 4-mom for the dinucleon in 2    ^
                    G4QHadron* curMes = new G4QHadron(pigPDG,m4Mom);  //                  ^
                    theQHadrons.push_back(curMes); // Fill the gam/pi (delete equivalent) ^
                    G4QHadron* secBar = new G4QHadron(pigPDG,m4Mom);  //                  ^
                    theQHadrons.push_back(secBar); // Fill the gam/pi (delete equivalent) ^
                    if(dinFlag)               //                                          ^
					               {                         //                                          ^
                      G4QHadron* triBar = new G4QHadron(pigPDG,m4Mom);// Cre. 3-d nucleon ^
                      theQHadrons.push_back(triBar);// Fill 3-d nucleon (delete equival.) ^
                    }                         //                                          ^
                    pQ->KillQuasmon();        // Make done the current Quasmon            ^
                    tot4M-=cq4M;              // Update theTotalResidNucl of HadrPr.      ^
                    totQC-=qQC;               // Update total residual QC of HadrPr.      ^
                    eCount--;                 // Reduce a#of the living Quasmons          ^
				              }                           //                                          ^
                  else if (PDGQ!=10)          // @@ Chipolino can wait @@                 ^
                  {                           //                                          ^
                    G4double qM =cq4M.m();    // Real mass of the Quasmon                 ^
                    G4double gsM=QPDGQ.GetMass(); // GSmass of the Quasmon                ^
#ifdef edebug
    		              G4cout<<"G4QEnv::HadrQEnv:#"<<jq<<",qM="<<qM<<">gsM="<<gsM<<G4endl;// ^
#endif
					               if(fabs(qM-gsM)<0.0001)    // "Fill & Kill" Case                       ^
                    {                         //                                          ^
                      G4QHadron* resQ = new G4QHadron(PDGQ,cq4M); // GSM hadron for CurQ  ^
#ifdef pdebug
					                 G4cout<<"G4QEnv::HadrQEnv:ResQ="<<PDGQ<<cq4M<<G4endl;         //    ^
#endif
                      theQHadrons.push_back(resQ); // @@ Check Dibarions @@ (del.equiv.)  ^
                      pQ->KillQuasmon();      // Make done the current Quasmon            ^
                      tot4M-=cq4M;            // Update theTotalResidNucl of HadrPr.      ^
                      totQC-=qQC;             // Update total residual QC of HadrPr.      ^
                      eCount--;               // Reduce a#of the living Quasmons          ^
					               }                         //                                          ^
					               else if(eCount==1 && qM<gsM && CheckGroundState(pQ,true))// Cor.& Fin.^
                    {                         //                                          ^
#ifdef edebug
					                 G4cout<<"G4QEnv::HadrQEnv:**>>** CGS Correction **>>**"<<G4endl; // ^
#endif
                      for_each(output->begin(), output->end(), DeleteQHadron()); //-->-->-^
                      output->clear();        //                                          ^
                      delete output;          // >========================================^
                      pQ->KillQuasmon();      // If BackFusion -> kill theQuasmon         ^
                      eCount--;               // Reduce a#of the living Quasmons          ^
                      return theQHadrons;     //                                          ^
                    }                         //                                          ^
				                else if(qM<gsM&&(pQ->GetQC().GetSPDGCode()==1114     //               ^
									                            || pQ->GetQC().GetSPDGCode()==2224) //               ^
                          &&qM>theWorld->GetQParticle(QPDGQ)->MinMassOfFragm())//Del&Kill ^
                    {                         //                                          ^
#ifdef edebug
					                 G4cout<<"G4QEnv::HadrQEnv:**||** Copy&Decay **||**"<<G4endl; //     ^
#endif
                      G4QHadronVector* decHV=pQ->DecayQuasmon();//Dec.Quasm & fill decHV=*^
                      CopyAndDeleteHadronVector(decHV);// Copy output to QHadrV of G4Env  ^
                      tot4M-=pQ->Get4Momentum(); // tot4M recalculation                   ^
                      totQC-=pQ->GetQC();     // totQC recalculation                      ^
                      pQ->KillQuasmon();      // Make done the current Quasmon            ^
                      eCount--;               // Reduce a#of the living Quasmons          ^
					               }                         //                                          ^
				              }                           //                                          ^
				            }                             // End of the Medium/Vacuum IF              ^
			           }                               // End of the status ELSE IF                ^
              else if(status==3) count3++;    //                                          ^
              if(status<0)                    // Panic: Quasmon is below theMassShell     ^
              {                               //                                          ^
                //if(eCount==1 && DecayInEnvQ(pQ)) //                                     ^
                //{                             //                                        ^
                //  for_each(output->begin(), output->end(), DeleteQHadron());//-->-->-->-^
                //  output->clear();            //                                        ^
                //  delete output;              // >======================================^
                //  eCount--;                   // Reduce a#of the living Quasmons        ^
                //  pQ->KillQuasmon();          //                                        ^
                //  return theQHadrons;         //                                        ^
                //}                             //                                        ^
                G4int    ppm=jq;                // Initialized by PANIC Quasmon pointer   ^
                G4int    nRQ=0;                 // Prot. of a#of additionalRealQuasmons   ^
#ifdef edebug
    		          G4cout<<"G4QEnv::HadrQEnv: ***PANIC*** for jq="<<jq<<G4endl; //           ^
#endif
                G4ThreeVector vp= pQ->Get4Momentum().vect(); // PANICQuasmon momentum     ^
                G4double dpm=1.e+30;            // Big number (dot product of momenta)    ^
	               if(nQuasmons>1) for(G4int ir=0; ir<nQuasmons; ir++)// Search for partner  ^
                {                               //                                        ^
                  if(ir!=jq)                    // Skip the current (PANIC) Quasmon itself^
                  {                             //                                        ^
	                   G4Quasmon* rQ = theQuasmons[ir]; //                                   ^
                    G4int Qst = rQ->GetStatus();// Status of a Quasmon                    ^
#ifdef edebug
					               G4cout<<"G4QEnv::HadrQEnv: ir="<<ir<<",Qstatus="<<Qst<<G4endl; //     ^
#endif
                    if(Qst>0)                   // Skip the dead Quasmon                  ^
				                {
					                 nRQ++;                    // Increment real-Quasmon-counter         ^
                      G4double dp=vp.dot(rQ->Get4Momentum().vect()); //                   ^
                      if(dp<dpm)                // Search for the "moving in thesameDir"  ^
                      {                         //                                        ^
                        ppm=ir;                 // Remember the index of MinProj Quasmon  ^
                        dpm=dp;                 // Remember the value of theMinProjection ^
					                 }                         //                                        ^
				                }                           //                                        ^
				              } // End of the Quasmon LOOP 
                } // End of the partner-search-for-the-PANIC-Quasmon LOOP                 ^
                if(nRQ)                         // Merge with theBestPartnerQuasmonCandid ^
                {                               //                                        ^
	                 G4Quasmon*      rQ = theQuasmons[ppm];   //                             ^
                  G4QContent      rQC= rQ->GetQC();        //                             ^
                  G4LorentzVector r4M= rQ->Get4Momentum(); //                             ^
                  rQC               += pQ->GetQC();        //                             ^
                  r4M               += pQ->Get4Momentum(); //                             ^
                  rQ->InitQuasmon(rQC, r4M);    // Make new Quasmon                       ^
#ifdef edebug
		                G4cout<<"G4QE::HQE:"<<pQ->GetQC()<<"+"<<rQ->GetQC()<<"="<<rQC<<G4endl;//^
#endif
                  pQ->KillQuasmon();            // Delete old Quasmon                     ^
                  eCount--;                     // Decrement counter of living Quasmons   ^
			             }                               //                                        ^
                else // No candidate to resolve PANIC was found                           ^
                {                               //                                        ^
#ifdef edebug
		                G4cout<<"G4QEnv::HQE: No Q-cand. nRQ="<<nRQ<<",eC="<<eCount<<G4endl; // ^
#endif
                  //if(eCount==1 && CheckGroundState(pQ,true)) //  BackFusion attempt     ^
                  if(CheckGroundState(pQ,true)) //  The only Q: BackFusion attempt        ^
                  {
                    for_each(output->begin(), output->end(), DeleteQHadron()); //-->-->---^
                    output->clear();            //                                        ^
                    delete output;              // >======================================^
                    pQ->KillQuasmon();          //                                        ^
                    eCount--;                   // Reduce a#of the living Quasmons        ^
					               return theQHadrons;         //                                        ^
                  }                             //                                        ^
#ifdef fdebug
		                G4cout<<"G4QEnv::HadrQEnv:NO PANICsolution,t="<<tot4M<<totQC<<G4endl;// ^
#endif
                  totQC=theEnvironment.GetQC(); //                                        ^
                  tot4M=theEnvironment.Get4Momentum(); //                                 ^
	                 if(nQuasmons) for(G4int jr=0; jr<nQuasmons; jr++) // Search for partner ^
                  {                             //                                        ^
	                   G4Quasmon* rQ = theQuasmons[jr]; // Pointer to the Quasmon            ^
                    G4int Qst = rQ->GetStatus();// Status of a Quasmon                    ^
                    if(jr==jq)                  //                                        ^
					               {                           //                                        ^
                      totQC+=rQ->GetQC();       // QuarkContent of the Quasmon            ^
                      tot4M+=rQ->Get4Momentum();// QuarkContent of the Quasmon            ^
                    }                           //                                        ^
                    else if(Qst)                // Skip dead Quasmons                     ^
                    {                           //                                        ^
                      totQC+=rQ->GetQC();       // QuarkContent of the Quasmon            ^
                      tot4M+=rQ->Get4Momentum();// QuarkContent of the Quasmon            ^
					               }                           //                                        ^
                  } // End of the "No candidate to resolve PANIC" ELSE                    ^
                  pQ->KillQuasmon();            // Kill the only Quasmon                  ^
                  eCount--;                     // Reduce a#of the living Quasmons        ^
                  CleanUp();                    // Clean up THIS Quasmon and Environment  ^
                  G4QHadron* evH = new G4QHadron(totQC,tot4M); // Create ResidNuclHadron  ^
                  EvaporateResidual(evH);       // Try to evaporate residual (del.equiv.) ^
                  for_each(output->begin(), output->end(), DeleteQHadron()); //-->-->-->--^
                  output->clear();              //                                        ^
                  delete output;                // >======================================^
                  force=true;                   // Make the force decision                ^
                  break;                        // Out of the fragmentation loop >->+     ^
			             }                               //                                  |     ^
			           }                                 //                                  |     ^
			         }                                   //                                  |     ^
            for_each(output->begin(), output->end(), DeleteQHadron()); // ->-->-->--|-----^
            output->clear();                    //                                  |     ^
            delete output;                      // >================================|=====^
		        } // End of skip of the dead Quasmons                                     |
#ifdef pdebug
	         G4cout<<"G4QE::HQE:QStat("<<jq<<"="<<status<<pQ->Get4Momentum()<<G4endl;//|
#endif
	       } // End of fragmentation LOOP over Quasmons (jq) <--------<----------<-----+
      }
      else if(totMass>totM+.001)                // ==> "Try Evaporate or decay" case
	     {
      // @@@@ For 1 livingQuasmon & totMass>minRQ+mREnv -> decay in RE+RQ (in RQ direction)
#ifdef edebug
		      G4cout<<"G4QEnv::HadrQE: M="<<totMass<<",PDG="<<totPDG<<",B="<<totBN<<",GSM="<<totM
              <<",dM="<<totMass-totM<<",totQC="<<totQC<<G4endl;
#endif
        if(totBN<2)                             // ==> "Baryon/Meson residual Quasmon" case
		      {
          if(totPDG==90999999||totPDG==90999000||totPDG==90000999||totPDG==89999001)//"M"ca
		        {
		          G4cerr<<"---Warning---G4QE::HQE:Meson(2) PDG="<<totPDG<<",M="<<totMass<<G4endl;
		        }
          else if(totPDG==1114||totPDG==2224)   //==> "DELTA- or DELTA++" case (?antiDELTA)
		        {
            G4double   mBar=mProt;
			         G4int      bPDG=2212;
            G4double   mMes=mPi;
			         G4int      mPDG=211;
            if(totPDG==1114)                    // "DELTA-" case
			         {
              mBar=mNeut;
              bPDG=2112;
              mPDG=-211;
		          }
            if(totMass<mBar+mMes)
			         {
		            G4cerr<<"--Warning--G4QE::HQE:tM="<<totMass<<"<GSM+mPi0="<<totM+mPi0<<G4endl;
              G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
              CleanUp();
		            if(!CheckGroundState(quasH,true))
              {
                G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                G4cerr<<"***G4QE::HQE:FillAsIs(-4),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
                //throw G4QException("G4QEnvironment::HadronizeQEnvironment:(1)DecayQEnv");
              }
              delete quasH;  
              return theQHadrons;
			         }
            else
			         {
              //G4QHadron* delta = new G4QHadron(totQC,tot4M);
              //delta->SetNFragments(2);           // Put a#of Fragments=2
              //theQHadrons.push_back(delta);      // Fill the residual DELTA (del.Eq.)
              // Instead
              //delete delta;
              //
              G4LorentzVector b4Mom(0.,0.,0.,mBar);
              G4LorentzVector m4Mom(0.,0.,0.,mMes);
              if(!G4QHadron(tot4M).DecayIn2(b4Mom, m4Mom))
              {
                G4cerr<<"---Warning---G4QEnv::HadronizeQE:B="<<bPDG<<"(m="<<mBar<<") + M="
					                 <<mPDG<<"(m="<<mMes<<")="<<mBar+mMes<<" > mDel="<<totMass<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QE::HQE:FillAsIs(-3),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
    	             //throw G4QException("G4QEnvironment::HadronizeQEnv:Del->Bar+Mes error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef pdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: DELTA="<<totPDG<<tot4M<<" -> Bar="
                    <<bPDG<<b4Mom<<" + Mes="<<mPDG<<m4Mom<<G4endl;
#endif
              G4QHadron* curBar = new G4QHadron(bPDG,b4Mom);
              theQHadrons.push_back(curBar);     // Fill the baryon (delete equivalent)
#ifdef edebug
			           G4cout<<"G4QEnv::HadrQEnv:BaryonH="<<bPDG<<b4Mom<<G4endl;
#endif
              G4QHadron* curMes = new G4QHadron(mPDG,m4Mom);
              theQHadrons.push_back(curMes);     // Fill the meson (delete equivalent)
#ifdef edebug
			           G4cout<<"G4QEnv::HadrQEnv:MesonH="<<mPDG<<m4Mom<<G4endl;
#endif
              return theQHadrons;
			         }
		        }
          else if(totPDG==10)                    // ==> "Chipolino" case
		        {
            G4QChipolino resChip(totQC);         // define Residual as Chipolino
            G4QPDGCode h1QPDG=resChip.GetQPDG1();// QPDG of the first hadron
            G4int      h1PDG=h1QPDG.GetPDGCode();// PDG code of the first hadron
            G4double   h1M  =h1QPDG.GetMass();   // Mass of the first hadron
            G4QPDGCode h2QPDG=resChip.GetQPDG2();// QPDG of the second hadron
            G4int      h2PDG=h2QPDG.GetPDGCode();// PDG code of the second hadron
            G4double   h2M  =h2QPDG.GetMass();   // Mass of the second hadron
            G4LorentzVector h14Mom(0.,0.,0.,h1M);
            G4LorentzVector h24Mom(0.,0.,0.,h2M);
            if(!G4QHadron(tot4M).DecayIn2(h14Mom, h24Mom))
            {
              G4cerr<<"---Warning---G4QEnv::HadronizeQE:h1="<<h1PDG<<"(m="<<h1M<<") + h2="
				                <<h2PDG<<"(m="<<h2M<<")="<<h1M+h2M<<" > mChipo="<<totMass<<G4endl;
              G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
              CleanUp();
		            if(!CheckGroundState(quasH,true))
              {
                G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                G4cerr<<"***G4QE::HQE:FillAsIs(-2),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
    	           //throw G4QException("G4QEnvironment::HadrQEnv: Chipo->1+2 decay failed");
              }
              delete quasH;  
              return theQHadrons;
            }
#ifdef pdebug
	           G4cout<<"G4QEnv::HadronizeQEnv: Chipo="<<tot4M<<" -> h1="
                  <<h1PDG<<h14Mom<<" + Mes="<<h2PDG<<h24Mom<<G4endl;
#endif
            G4QHadron* curH1 = new G4QHadron(h1PDG,h14Mom);
            theQHadrons.push_back(curH1);        // Fill the curH1 (delete equivalent)
#ifdef edebug
			         G4cout<<"G4QEnv::HadrQEnv:HadronH="<<h1PDG<<h14Mom<<G4endl;
#endif
            G4QHadron* curH2 = new G4QHadron(h2PDG,h24Mom);
            theQHadrons.push_back(curH2);        // Fill the curH2 (delete equivalent)
#ifdef edebug
			         G4cout<<"G4QEnv::HadrQEnv:MesAsHadrPartnerH="<<h2PDG<<h24Mom<<G4endl;
#endif
            return theQHadrons;
		        }
          else if(totBN<2&&totPDG&&totMass<totM+mPi0+.001)// ==> "Meson/Baryon+gamma" case
		        {
            G4LorentzVector h4Mom(0.,0.,0.,totM);
            G4LorentzVector g4Mom(0.,0.,0.,0.);
            if(!G4QHadron(tot4M).DecayIn2(h4Mom, g4Mom))
            {
              G4cerr<<"---Warning---G4QEnv::HadronizeQEnv: h="<<totPDG<<"(m="<<totM
                    <<") + gamma > mTot="<<totMass<<G4endl;
              G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
              CleanUp();
		            if(!CheckGroundState(quasH,true))
              {
                G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                G4cerr<<"***G4QE::HQE:FillAsIs(-1),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
       	        //throw G4QException("G4QEnvironment::HadronizeQEnv:Gamma Decay failed");
              }
              delete quasH;  
              return theQHadrons;
            }
#ifdef pdebug
	           G4cout<<"G4QE::HQE:"<<tot4M<<"->h="<<totPDG<<h4Mom<<" + gamma="<<g4Mom<<G4endl;
#endif
            G4QHadron* curG = new G4QHadron(22,g4Mom);
            theQHadrons.push_back(curG);         // Fill the gamma (delete equivalent)
#ifdef edebug
			         G4cout<<"G4QEnv::HadrQEnv:PhotonH="<<g4Mom<<G4endl;
#endif
            G4QHadron* curH = new G4QHadron(totPDG,h4Mom);
#ifdef edebug
			         G4cout<<"G4QEnv::HadrQEnv:GamPartnerH="<<totPDG<<h4Mom<<G4endl;
#endif
            if(totPDG==92000000||totPDG==90002000||totPDG==90000002) DecayDibaryon(curH);
            else theQHadrons.push_back(curH);    // Fill the baryon (delete equivalent)
            return theQHadrons;
		        }
          else if(totBN<2&&totPDG)               // ==> "Meson/Baryon+pi" case
		        {
            G4int piPDG=111;
            G4double mpi=mPi0;
            G4int mbPDG=totPDG;
            G4double mbm=totM;
            if(totPDG==1114)
			         {
			           piPDG=-211;
              mpi=mPi;
              mbPDG=2112;
              mbm=mNeut;
            }
            else if(totPDG==2224)
			         {
			           piPDG=211;
              mpi=mPi;
              mbPDG=2212;
              mbm=mProt;
            }
            else if(totPDG==113)
			         {
			           piPDG=-211;
              mpi=mPi;
              mbPDG=211;
              mbm=mPi;
            }
            G4LorentzVector h4Mom(0.,0.,0.,mbm);
            G4LorentzVector g4Mom(0.,0.,0.,mpi);
            if(!G4QHadron(tot4M).DecayIn2(h4Mom, g4Mom))
            {
              G4cerr<<"---Warning---G4QEnv::HadronizeQEnv: h="<<mbPDG<<"(m="<<mbm
                    <<") + pi(m="<<mpi<<")="<<mbm+mpi<<" > mTot="<<totMass<<G4endl;
              G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
              CleanUp();
		            if(!CheckGroundState(quasH,true))
              {
                G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                G4cerr<<"***G4QE::HQE:FillAsIs(0),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
       	        //throw G4QException("G4QEnvironment::HadronizeQE: DecIn2 mB+nPi failed");
              }
              delete quasH;  
              return theQHadrons;
            }
#ifdef pdebug
	           G4cout<<"G4QE::HQE:"<<tot4M<<"->h="<<mbPDG<<h4Mom<<"+p="<<piPDG<<g4Mom<<G4endl;
#endif
            G4QHadron* curH = new G4QHadron(mbPDG,h4Mom);
            if(totPDG==92000000||totPDG==90002000||totPDG==90000002) DecayDibaryon(curH);
            else theQHadrons.push_back(curH);    // Fill the baryon (delete equivalent)
            G4QHadron* curG = new G4QHadron(piPDG,g4Mom);
#ifdef edebug
			         G4cout<<"G4QEnv::HadrQEnv:Gamma/Pi0H="<<piPDG<<g4Mom<<G4endl;
#endif
            theQHadrons.push_back(curG);         // Fill the pi0 (delete equivalent)
            return theQHadrons;
		        }
          else                                   // ==> "|B|<2 new Quasmon" case
		        {
            G4Quasmon* resid = new G4Quasmon(totQC,tot4M); // delete is 3 lines below <-+
            G4QNucleus vacuum(90000000);         //                                     ^
 	          G4QHadronVector* curout=resid->Fragment(vacuum,1);//**!!DESTROY!!** <-<-+   ^
            G4int rest = resid->GetStatus();     // New status after fragm attempt  ^   ^
            if(!rest) eCount--;                  // Dec ExistingQuasmonsCounter     ^   ^
            delete resid;                        //_________________________________^___^
            G4int nHadrons = curout->size();     // a#of Hadrons in the outHV       ^
            if(nHadrons>0)                       // Transfer QHadrons to Output     ^
	           {
    	         for (G4int ih=0; ih<nHadrons; ih++)// LOOP over output QHadrons       ^
			           {                                  //                                 ^
#ifdef pdebug
				            G4cout<<"G4QEnv::HadrQE:NewB<2, H#"<<ih //                          ^
                      <<", QPDG="<<(*curout)[ih]->GetQPDG() //                      ^
                      <<", 4M="<<(*curout)[ih]->Get4Momentum()<<G4endl; //          ^
#endif
                //theQHadrons.push_back(curout->operator[](ih));//(delete equ.) <-<-^
                theQHadrons.push_back((*curout)[ih]);          // (delete equ.) <-<-^
			           }                                  //                                 ^
	           }                                    //                                 ^
			         else                                 //                                 ^
												{                                    //                                 ^
              G4cerr<<"***G4QEnv::HadrQEnv:MQ="<<tot4M.m()<<",QC="<<totQC<<G4endl;//^
			           throw G4QException("G4QEnvironment::HadronizeQEnv: Quasmon decay?");//^
			         }                                    // *** Do not destroy instances ***^
            curout->clear();                     // The instances are filled above  ^
            delete curout;                       // >===============================^
            return theQHadrons;
		        }
		      }
        else
		      {
          G4QContent tQC    =totQC;              // Not subtracted copy for error prints
          G4int      NSi    =0;                  // a#of additional Sigma
          G4int      SiPDG  =0;                  // PDG of additional Sigma
          G4double   MSi    =0.;                 // TotalMass of additional Sigma
          G4int      NaK    =0;                  // a#of additional Kaons/anti-Kaons
          G4int      aKPDG  =0;                  // PDG of additional Kaons/anti-Kaons
          G4double   MaK    =0.;                 // TotalMass of additionalKaons/anti-Kaons
          G4int      NPi    =0;                  // a#of additional pions
          G4int      PiPDG  =0;                  // PDG of additional pions
          G4double   MPi    =0.;                 // Total Mass of additional pions
          if    (totBN>0&&totS<0&&totChg+totChg>=totBN)// => "additional K+" case
	         {
            aKPDG=321;
            NaK=-totS;
            MaK=mK*NaK;
            tQC+=totS*KpQC;
            totChg+=totS;                        // Charge reduction (totS<0!)
            totS=0;                              // Anti-strangness goes to anti-Kaons
	         }
          else if (totBN>0&&totS<0)              // => "additional aK0" case
	         {
            aKPDG=311;
            NaK=-totS;
            MaK=mK0*NaK;
            tQC+=totS*K0QC;
            totS=0;                              // Anti-strangness goes to anti-Kaons
	         }
          else if (totBN>1&&totS>0&&(totChg<0||totChg>totBN-totS))//=>"additional Sigma"
	         {
            NSi=totS;                            // Prototype of a#of Sigmas
            if(totChg<0)                         // Negative Sigmas
			         {
              SiPDG=3112;
              if(-totChg<NSi) NSi=-totChg;       // A#of Sigma- is restricted by charge
              MSi=mSigM*NSi;                     // Total mass of Sigma-'s
              tQC-=NSi*SiMQC;                    // Subtract QC of Sigma-'s from totQC
              totChg+=NSi;                       // Increase the TotalResidualCharge
            }
            else
			         {
              SiPDG=3222;                        // Positive Sigmas
              G4int exChg=totChg-totBN+totS;     // Excesive positive charge
              if(exChg<NSi) NSi=exChg;           // A#of Sigma+ is restricted by charge
              MSi=mSigP*NSi;                     // Total mass of Sigma+'s
              tQC-=NSi*SiPQC;                    // Subtract QC of Sigma-'s from totQC
              totChg-=NSi;                       // Reduce the TotalResidualCharge
            }
            totS-=NSi;                           // Reduce the TotalResidualStrangeness
            totBN-=NSi;                          // A#of excessive pions is added below
	         }
          else if (totBN>0&&totS>totBN&&totBN<totS+totChg)// => "additional K0" case
	         {// @@ Here Ksi0 check should be added totS=2>totBN=1&&totBN=1<totS=2+totChg=0
            aKPDG=-311;
            NaK=totS-totBN;
            MaK=mK0*NaK;
            tQC+=NaK*K0QC;
            totS-=NaK;                           // Reduce residualstrangeness
	         }
          else if (totBN>0&&totS>totBN&&totChg<0)// => "additional K-" case
	         {// @@ Here Ksi- check should be added totS=2>totBN=1&&totChg=-1<0
            aKPDG=-321;
            NaK=totS-totBN;
            MaK=mK0*NaK;
            tQC+=NaK*KpQC;
            totChg+=NaK;                         // Increase residual charge
            totS-=NaK;                           // Reduce residual strangeness
	         }
          // === Now residual DELTAS should be subtracted === 
          if      (totBN>0&&totChg>totBN-totS)   // => "additional PI+" case
	         {// @@ Here Sigma+ check should be added totChg=1>totBn=1-totS=1
            PiPDG=211;
            NPi=totChg-totBN+totS;
            MPi=mPi*NPi;
            tQC-=NPi*PiQC;
            totChg-=NPi;
	         }
          else if (totBN>0&&totChg<0)            // => "additional PI-" case
	         {// @@ Here Sigma- check should be added totChg<0
            PiPDG=-211;
            NPi=-totChg;
            MPi=mPi*NPi;
            tQC+=NPi*PiQC;                       // Now anti-Pions must be subtracted
            totChg+=NPi;
	         }
          else if (!totBN&&totChg>1-totS)        // => "additional PI+" case
	         {// @@ Here Sigma+ check should be added totChg=1>totBn=1-totS=1
            PiPDG=211;
            NPi=totChg+totS-1;
            MPi=mPi*NPi;
            tQC-=NPi*PiQC;
            totChg-=NPi;
	         }
          else if (!totBN&&totChg<-1-totS)       // => "additional PI-" case
	         {// @@ Here Sigma- check should be added totChg<0
            PiPDG=-211;
            NPi-=totChg+totS+1;
            MPi=mPi*NPi;
            tQC+=NPi*PiQC;                       // Now anti-Pions must be subtracted
            totChg+=NPi;
	         }
          G4double      totRM=0.;                // min (GS) Mass of the Residual System
          if(totBN<2)                            // Calculate totPDG & totRM
	         {
            totPDG=tQC.GetSPDGCode();            // MinPDGCode for the Residual compound
            if(totPDG==10&&tQC.GetBaryonNumber()>0) totPDG=tQC.GetZNSPDGCode();
            if(totPDG) totRM=G4QPDGCode(totPDG).GetMass(); // minMass of theResidualSystem
            else
            {
              G4cerr<<"***G4QEnvironment::HadronizeQEnv: totPDG=0"<<G4endl;
              throw G4QException("G4QEnv::HadrQEnv: Impossible PDG for B=1");
            }
          }
          else
	         {
            G4QNucleus totN(tQC,tot4M);          // Excited nucleus for the Residual System
            totRM=totN.GetMZNS();                // min (GS) Mass of the Residual System
            totPDG=totN.GetPDG();                // Total PDG Code for the Current compound
	         }
          if(NaK)                                // ==> "Decay in K0 or K+ + NPi" case
	         {//@@ Can (must) be moved to EvaporateResidual ??
            if(!NPi)                             // ==> "Only anti-strange K" case
		          {
              G4LorentzVector m4Mom(0.,0.,0.,MaK);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              G4double sum=MaK+totRM;
              if(fabs(totMass-sum)<eps)
			           {
                m4Mom=tot4M*(MaK/sum);
                n4Mom=tot4M*(totRM/sum);
              }
              else if(totMass<sum || !G4QHadron(tot4M).DecayIn2(m4Mom, n4Mom))
              {
#ifdef edebug
                G4cerr<<"***G4QE::HadronizeQE:M="<<aKPDG<<"(m="<<MaK<<")+N="<<totPDG<<"(m="
                      <<totRM<<")="<<sum<<" > mSN="<<totMass<<",d="<<sum-totMass<<G4endl;
#endif
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(1),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
                  //throw G4QException("G4QEnvironment::HadronizeQEnv:AntiS-Nuc error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef pdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> M="
                    <<aKPDG<<m4Mom<<" + N="<<totPDG<<n4Mom<<totQC<<G4endl;
#endif
              G4LorentzVector oneK=m4Mom;        // 4-mom of only kaon  
              if(NaK>1) oneK = m4Mom/NaK;        // 4-mom of one kaon  
              for (G4int jp=0; jp<NaK; jp++)
			           {
                G4QHadron* curK = new G4QHadron(aKPDG,oneK);
                theQHadrons.push_back(curK);     // Fill the curK (delete equivalent)
              }
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom); // @@ Use DecayDib then Evap
              EvaporateResidual(curN);           // Try to evaporate residual (del.eq.)
		          }
            else                                 // ==> "Anti-strange K's + DELTA's" case
		          {
              G4LorentzVector m4Mom(0.,0.,0.,MPi);
              G4LorentzVector k4Mom(0.,0.,0.,MaK);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              if(!G4QHadron(tot4M).DecayIn3(m4Mom, k4Mom, n4Mom))
              {
                G4cerr<<"---Warning---G4QE::HadronQE:K="<<aKPDG<<"(m="<<MaK<<")+PI="<<PiPDG
                      <<"(m="<<MPi<<")+N="<<totPDG<<"(m="<<totRM<<")>tM="<<totMass<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(2),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
      	           //throw G4QException("G4QEnvironment::HadronizeQE:2AntiS-Nucl(1) error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef fdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> nK="<<aKPDG<<k4Mom
                    <<" + nPi="<<PiPDG<<m4Mom<<" + N="<<totPDG<<n4Mom<<G4endl;
#endif
              G4LorentzVector onePi=m4Mom;       // 4-mom of only pion  
              if(NPi>1) onePi = m4Mom/NPi;       // 4-mom of one pion  
              for (G4int ip=0; ip<NPi; ip++)
			           {
                G4QHadron* curP = new G4QHadron(PiPDG,onePi);
#ifdef pdebug
			             G4cout<<"G4QEnv::HadrQEnv:SPion#"<<ip<<",H="<<PiPDG<<onePi<<G4endl;
#endif
                theQHadrons.push_back(curP);     // Fill the curM (delete equivalent)
	 		          }
              G4LorentzVector oneK=k4Mom;        // 4-mom of one kaon  
              if(NaK>1) oneK = k4Mom/NaK;        // 4-mom of one kaon  
              for (G4int jp=0; jp<NaK; jp++)
			           {
                G4QHadron* curP = new G4QHadron(aKPDG,oneK);
#ifdef pdebug
			             G4cout<<"G4QEnv::HadrQEnv:Kaon#"<<jp<<",H="<<aKPDG<<oneK<<G4endl;
#endif
                theQHadrons.push_back(curP);     // Fill the curM (delete equivalent)
	 		          }
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom);
              EvaporateResidual(curN);           // Try to evaporate residual (del.equiv.)
		          }
            return theQHadrons;
          }
          else if(NSi)                           // ==> "Decay in Sig+ or Sig- + NPi" case
	         {//@@ Can (must) be moved to EvaporateResidual ??
            if(!NPi)                             // ==> "Only Sigma's" case
		          {
              G4LorentzVector m4Mom(0.,0.,0.,MSi);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              G4double sum=MSi+totRM;
              if(fabs(totMass-sum)<eps)
			           {
                m4Mom=tot4M*(MSi/sum);
                n4Mom=tot4M*(totRM/sum);
              }
              else if(totMass<sum || !G4QHadron(tot4M).DecayIn2(m4Mom, n4Mom))
              {
#ifdef edebug
                G4cerr<<"***G4QE::HadronizeQE:M="<<aKPDG<<"(s="<<MSi<<")+N="<<totPDG<<"(m="
                      <<totRM<<")="<<sum<<" > mSN="<<totMass<<",d="<<sum-totMass<<G4endl;
#endif
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(2),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
                  //throw G4QException("G4QEnvironment::HadronizeQEnv:Sigma-Nuc error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef pdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> Sig="
                    <<SiPDG<<m4Mom<<" + N="<<totPDG<<n4Mom<<totQC<<G4endl;
#endif
              G4LorentzVector oneS=m4Mom;        // 4-mom of the only sigma  
              if(NSi>1) oneS = m4Mom/NSi;        // 4-mom of one sigma  
              for (G4int jp=0; jp<NSi; jp++)
			           {
                G4QHadron* curS = new G4QHadron(SiPDG,oneS);
                theQHadrons.push_back(curS);     // Fill the curS (delete equivalent)
              }
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom); // @@ Use DecayDib then Evap
              EvaporateResidual(curN);           // Try to evaporate residual (del.eq.)
		          }
            else                                 // ==> "Sigma's + DELTA's" case
		          {
              G4LorentzVector m4Mom(0.,0.,0.,MPi);
              G4LorentzVector k4Mom(0.,0.,0.,MSi);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              if(!G4QHadron(tot4M).DecayIn3(m4Mom, k4Mom, n4Mom))
              {
                G4cerr<<"---Warning---G4QE::HadronQE:S="<<SiPDG<<"(m="<<MSi<<")+PI="<<PiPDG
                      <<"(m="<<MPi<<")+N="<<totPDG<<"(m="<<totRM<<")>tM="<<totMass<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(3),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
       	          //throw G4QException("G4QEnvironment::HadronizeQE:2Sigma-Nucl(1) error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef fdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> nS="<<SiPDG<<k4Mom
                    <<" + nPi="<<PiPDG<<m4Mom<<" + N="<<totPDG<<n4Mom<<G4endl;
#endif
              G4LorentzVector onePi=m4Mom;       // 4-mom of the only pion  
              if(NPi>1) onePi = m4Mom/NPi;       // 4-mom of one pion  
              for (G4int ip=0; ip<NPi; ip++)
			           {
                G4QHadron* curP = new G4QHadron(PiPDG,onePi);
#ifdef pdebug
			             G4cout<<"G4QEnv::HadrQEnv:SPion#"<<ip<<",H="<<PiPDG<<onePi<<G4endl;
#endif
                theQHadrons.push_back(curP);     // Fill the curM (delete equivalent)
	 		          }
              G4LorentzVector oneS=k4Mom;        // 4-mom of the only kaon  
              if(NSi>1) oneS = k4Mom/NSi;        // 4-mom of one kaon  
              for (G4int jp=0; jp<NSi; jp++)
			           {
                G4QHadron* curP = new G4QHadron(SiPDG,oneS);
#ifdef pdebug
			             G4cout<<"G4QEnv::HadrQEnv:Sigma#"<<jp<<",H="<<SiPDG<<oneS<<G4endl;
#endif
                theQHadrons.push_back(curP);     // Fill the curM (delete equivalent)
	 		          }
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom);
              EvaporateResidual(curN);           // Try to evaporate residual (del.equiv.)
		          }
            return theQHadrons;
		        }
          else if(NPi)                           // ==> "Decay in Pi+ or Pi-" case
	         {
            if(NPi==1)                           // ==> "One isobar" case
		          {
              G4LorentzVector m4Mom(0.,0.,0.,MPi);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              if(!G4QHadron(tot4M).DecayIn2(m4Mom, n4Mom))
              {
                G4cerr<<"---Warning---G4QEnv::HadronizeQEnv:M="<<PiPDG<<"(m="<<MPi<<")+N="
				                  <<totPDG<<"(m="<<totRM<<")="<<MPi+totRM<<" > mSN="<<totMass<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(5),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
    	             //throw G4QException("G4QEnvironment::HadronizeQEnv:Iso-Nucleus error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef pdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> M="<<PiPDG<<m4Mom<<" + N="
                    <<totPDG<<n4Mom<<totQC<<G4endl;
#endif
              G4QHadron* curK = new G4QHadron(PiPDG,m4Mom);
              theQHadrons.push_back(curK);       // Fill the curK (delete equivalent)
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom);
              EvaporateResidual(curN);           // Evaporate residual (delete equivalent)
		          }
		          else                                 // ==> "Many Isobars" case
		          {
              G4int N1Pi = NPi/2;                // First pion cluster
              G4int N2Pi = NPi-N1Pi;             // Second pion cluster
              G4double mM  = MPi/NPi;            // Mass of Pi
              G4double m1M = mM*N1Pi;            // Mass of the first Pi-cluster
              G4double m2M = mM*N2Pi;            // Mass of the second Pi-cluster
              G4LorentzVector m4Mom(0.,0.,0.,m1M);
              G4LorentzVector k4Mom(0.,0.,0.,m2M);
              G4LorentzVector n4Mom(0.,0.,0.,totRM);
              if(!G4QHadron(tot4M).DecayIn3(m4Mom, k4Mom, n4Mom))
              {
                G4cerr<<"---Warning---G4QEnv::HadronizeQE:N*Pi="<<PiPDG<<"(m="<<mM<<")+N="
                      <<totPDG<<"(m="<<totRM<<") >(?)SN="<<totMass<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(5),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
       	          //throw G4QException("G4QEnvironment::HadronizeQE:ManyIsoNucleus error");
                }
                delete quasH;  
                return theQHadrons;
              }
#ifdef pdebug
	             G4cout<<"G4QEnv::HadronizeQEnv: SN="<<tot4M<<" -> N*PI="<<PiPDG
                    <<" (4M1="<<m4Mom<<" + 4M2="<<k4Mom<<") + N="<<totPDG<<n4Mom<<G4endl;
#endif
              G4LorentzVector one1=m4Mom;        // 4-mom of the 1st cluster only pion  
              if(N1Pi>1) one1=m4Mom/N1Pi;        // 4-mom of the 1st cluster one pion  
              for (G4int ip=0; ip<N1Pi; ip++)
			           {
                G4QHadron* curP = new G4QHadron(PiPDG,one1);
                theQHadrons.push_back(curP);     // Fill the curP (delete equivalent)
	 		          }
              G4LorentzVector one2=k4Mom;        // 4-mom of the 2nd cluster only pion  
              if(N2Pi>1) one2=k4Mom/N2Pi;        // 4-mom of the 2nd cluster one pion  
              for (G4int jp=0; jp<N2Pi; jp++)
			           {
                G4QHadron* curP = new G4QHadron(PiPDG,one2);
                theQHadrons.push_back(curP);     // Fill the curP (delete equivalent)
	 		          }
              G4QHadron* curN = new G4QHadron(totPDG,n4Mom);
              EvaporateResidual(curN);           // Try to evaporate residual (del.equiv.)
		          }
            return theQHadrons;
		        }
		      }
#ifdef fdebug
		      G4cout<<"G4QE::HadrQEnv: Try FinalEvaporation t4M="<<tot4M<<",tQC="<<totQC<<G4endl;
#endif
        CleanUp();
        G4QHadron* evH = new G4QHadron(totQC,tot4M); // Create a Hadron for theResidualNucl
        EvaporateResidual(evH);                  // Try to evaporate residual (del.equiv.)
        return theQHadrons;
	     }
      else                                       // ==> "Only GSEnvironment exists" case
      { 
        if(totPDG==90000000 || fabs(totMass)<0.000001)
		      {
          CleanUp();
          return theQHadrons;
		      }
        G4double dM=totMass-totM;
#ifdef pdebug
		      G4cout<<"G4QEnv::HadrQEnv:GroundState tM-GSM="<<dM<<",GSM="<<totM<<",tPDG="<<totPDG
              <<",nQ="<<nQuasmons<<G4endl;
#endif
	       G4Quasmon*       pQ = theQuasmons[0];    // Pointer to the first Quasmon          
        G4QPDGCode    QQPDG = pQ->GetQPDG();     // QPDG of the Quasmon
        G4int          QPDG = QQPDG.GetPDGCode();
        G4QNucleus    totRN(totQC,tot4M);        // Nucleus for theTotalResidualNuclearComp
        G4int          spbRN=totRN.SplitBaryon();// PossibilityToSplit baryon from Residual
        if(dM>-0.001)
		      {
#ifdef fdebug
		        G4cout<<"G4QE::HadrQE:ExcitedNucleus, dM="<<dM<<">0,tBN="<<totBN<<",nQ="<<G4endl;
#endif
          CleanUp();
          G4QHadron* evH = new G4QHadron(totQC,tot4M);// Create a Hadron for ResidualNucl
          EvaporateResidual(evH);                // Try to evaporate residual (del. equiv.)
		      }
        else if(nQuasmons==1&&QPDG!=22&&QPDG!=111)// => "Decay Quasmon or Q+Environ" case
		      {
          G4int envPDG = theEnvironment.GetPDG();// PDGCode of the NuclQEnvironment
#ifdef pdebug
		        G4cout<<"G4QEnv::HadrQEnv: nQ=1, QPDG=="<<QPDG<<G4endl;
#endif
          if(!QPDG)
		        {
			         G4cerr<<"***G4QEnv::HadrQE: Quasmon is an unknown QHadron: PDG="<<QPDG<<G4endl;
			         throw G4QException("G4QEnvironment::HadronizeQEnvironment:(2)Cann'tDecayQEnv");
		        }
          // => "Quasmon-Chipolino or Environment-Dibaryon" case
          else if(QPDG==10||QPDG==92000000||QPDG==90002000||QPDG==90000002)
		        {
            G4QContent QQC = pQ->GetQC();        // Quark Content of the Quasmon
            G4QPDGCode h1QPDG=nQPDG;             // QPDG of the first hadron
            G4double   h1M   =mNeut;             // Mass of the first hadron
            G4QPDGCode h2QPDG=h1QPDG;            // QPDG of the second hadron
            G4double   h2M   =mNeut;             // Mass of the second hadron
            if(QPDG==10)
            {
              G4QChipolino QChip(QQC);           // define the Quasmon as a Chipolino
              h1QPDG=QChip.GetQPDG1();           // QPDG of the first hadron
              h1M   =h1QPDG.GetMass();           // Mass of the first hadron
              h2QPDG=QChip.GetQPDG2();           // QPDG of the second hadron
              h2M   =h2QPDG.GetMass();           // Mass of the second hadron
			         }
			         else if(QPDG==90002000)
			         {
              h1QPDG=pQPDG;                      // QPDG of the first hadron
              h1M   =mProt;                      // Mass of the first hadron
              h2QPDG=h1QPDG;                     // QPDG of the second hadron
              h2M   =mProt;                      // Mass of the second hadron
			         }
			         else if(QPDG==92000000)
			         {
              h1QPDG=lQPDG;                      // QPDG of the first hadron
              h1M   =mLamb;                      // Mass of the first hadron
              h2QPDG=h1QPDG;                     // QPDG of the second hadron
              h2M   =mLamb;                      // Mass of the second hadron
              G4double ddMass=totMass-envM;      // Free CM energy
              if(ddMass>mSigZ+mSigZ)             // Sigma0+Sigma0 is possible
			           {                                  // @@ Only two particles PS is used
                G4double dd2=ddMass*ddMass;      // Squared free energy
                G4double sma=mLamb+mLamb;        // Lambda+Lambda sum
                G4double pr1=0.;                 // Prototype to avoid sqrt(-)
                if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Lamb+Lamb PS
                sma=mLamb+mSigZ;                 // Lambda+Sigma0 sum
                G4double smi=mSigZ-mLamb;        // Sigma0-Lambda difference
                G4double pr2=pr1;                // Prototype of +L+S0 PS
                if(ddMass>sma && ddMass>smi) pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi));
                sma=mSigZ+mSigZ;                 // Sigma0+Sigma0 sum
                G4double pr3=pr2;                // Prototype of +Sigma0+Sigma0 PS
                if(ddMass>sma) pr3+=sqrt((dd2-sma*sma)*dd2);
                G4double hhRND=pr3*G4UniformRand(); // Randomize PS
                if(hhRND>pr2)                    // --> "ENnv+Sigma0+Sigma0" case
                {                                //
                  h1QPDG=s0QPDG;                 // QPDG of the first hadron
                  h1M   =mSigZ;                  // Mass of the first hadron
                  h2QPDG=h1QPDG;                 // QPDG of the second hadron
                  h2M   =mSigZ;                  // Mass of the second hadron
                }                                //
                else if(hhRND>pr1)               // --> "ENnv+Sigma0+Lambda" case
                {                                //
                  h1QPDG=s0QPDG;                 // QPDG of the first hadron
                  h1M   =mSigZ;                  // Mass of the first hadron
                }                                //
              }                                  //
              else if(ddMass>mSigZ+mLamb)        // Lambda+Sigma0 is possible
			           {                                  // @@ Only two particles PS is used
                G4double dd2=ddMass*ddMass;      // Squared free energy
                G4double sma=mLamb+mLamb;        // Lambda+Lambda sum
                G4double pr1=0.;                 // Prototype to avoid sqrt(-)
                if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Lamb+Lamb PS
                sma=mLamb+mSigZ;                 // Lambda+Sigma0 sum
                G4double smi=mSigZ-mLamb;        // Sigma0-Lambda difference
                G4double pr2=pr1;                // Prototype of +L+S0 PS
                if(ddMass>sma && ddMass>smi) pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi));
                if(pr2*G4UniformRand()>pr1)      // --> "ENnv+Sigma0+Lambda" case
                {                                //
                  h1QPDG=s0QPDG;                 // QPDG of the first hadron
                  h1M   =mSigZ;                  // Mass of the first hadron
                }                                //
              }                                  //
			         }                                    //
            if(h1M+h2M+envM<totMass)             // => "Three parts decay" case
			         {
              G4LorentzVector h14M(0.,0.,0.,h1M);
              G4LorentzVector h24M(0.,0.,0.,h2M);
              G4LorentzVector e4M(0.,0.,0.,envM);
              if(!G4QHadron(tot4M).DecayIn3(h14M,h24M,e4M))
			           {
				            G4cerr<<"Warning->G4QE::HQE:M="<<tot4M.m()<<","<<totMass<<"->"<<h1QPDG<<"("
					                 <<h1M<<")+"<<h1QPDG<<"("<<h2M<<")+"<<envM<<"="<<h1M+h2M+envM<<G4endl;
                G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                CleanUp();
		              if(!CheckGroundState(quasH,true))
                {
                  G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                  theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                  G4cerr<<"***G4QEnv::HQE:FillAsItIs(6),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
				              //throw G4QException("G4QEnv::HadrQEnv:QChipo+Environment DecIn3 Error");
                }
                delete quasH;  
                return theQHadrons;

			           }
              G4QHadron* h1H = new G4QHadron(h1QPDG.GetPDGCode(),h14M);
              theQHadrons.push_back(h1H);        // (delete equivalent)
#ifdef pdebug
			           G4cout<<"G4QE::HQE:(2) H1="<<h1QPDG<<h14M<<G4endl;
#endif
              G4QHadron* h2H = new G4QHadron(h2QPDG.GetPDGCode(),h24M);
              theQHadrons.push_back(h2H);        // (delete equivalent)
#ifdef pdebug
              G4cout<<"G4QE::HQE:(2) H2-"<<h2QPDG<<h24M<<G4endl;
#endif
              G4QHadron* qeH = new G4QHadron(envPDG,e4M);
              theQHadrons.push_back(qeH);        // (delete equivalent)
#ifdef pdebug
              G4cout<<"G4QE::HQE:(2) QEenv="<<envPDG<<e4M<<G4endl;
#endif
			         }
#ifdef fdebug
            G4cerr<<"***G4QEnv::HadQEnv:tM="<<tot4M.m()<<totQC<<"< h1="<<h1QPDG<<"(M="<<h1M
                  <<")+h2="<<h1QPDG<<"(M="<<h2M<<")+eM="<<envM<<"="<<h1M+h2M+envM<<G4endl;
			         //throw G4QException("G4QEnv::HadrQEnv:QChipo+Env mass > than decaying mass");
#endif
            CleanUp();
            G4QHadron* evH = new G4QHadron(totQC,tot4M);// Create a Hadron for ResidualNucl
            EvaporateResidual(evH);            // Try to evaporate residual (del. equiv.)
            return theQHadrons;
		        }
        }
        else if(spbRN)// => "Join all quasmons to the residual compound and evaporate" case
		      {
#ifdef fdebug
          G4cout<<"***G4QEnv::HadQEnv: Evaporate the total residual tRN="<<totRN<<G4endl;
#endif
          CleanUp();
          G4QHadron* evH = new G4QHadron(totQC,tot4M);// Create Hadron for theResidNucleus
          EvaporateResidual(evH);               // Try to evaporate residual (del.equiv.)
          return theQHadrons;
		      }
        //else if(nQuasmons<3||theQHadrons.size()<12)//"Try to correct" case (change cond)
        else if(2>3)  // "Try to correct" case (change condition)
		      {
#ifdef pdebug
		        G4cout<<"***G4QEnv::HadrQE: M="<<totMass<<",dM="<<dM<<",nQ="<<nQuasmons<<G4endl;
#endif
          G4int          nOfOUT  = theQHadrons.size();
          while(nOfOUT)
          {
            G4QHadron*     theLast = theQHadrons[nOfOUT-1];
            G4LorentzVector last4M = theLast->Get4Momentum();
            G4QContent      lastQC = theLast->GetQC();
            G4int           lastS  = lastQC.GetStrangeness();
            G4int           totS   = totQC.GetStrangeness();
            G4int           nFr    = theLast->GetNFragments();
            G4int           gam    = theLast->GetPDGCode();
		          if(gam!=22&&!nFr&&lastS<0&&lastS+totS<0&&nOfOUT>1) // => "Skip K,gam & decayed"
		          {
              G4QHadron* thePrev = theQHadrons[nOfOUT-2];
              theQHadrons.pop_back();         // the last QHadron is excluded from OUTPUT
              theQHadrons.pop_back();         // the prev QHadron is excluded from OUTPUT
              theQHadrons.push_back(thePrev); // thePast becomes theLast as an instance
              delete    theLast;              // theLast QHadron is deleated as an instance
              theLast = thePrev;              // Update parameters(thePrev becomes theLast)
              last4M = theLast->Get4Momentum();
              lastQC = theLast->GetQC();
		          }
            else
            {
              theQHadrons.pop_back();         // the last QHadron is excluded from OUTPUT 
              delete         theLast;         // theLastQHadron is deleated as an instance
            }
            totQC+=lastQC;                    // Update (increase) the total QC
            tot4M+=last4M;                    // Update (increase) the total 4-momentum
            totMass=tot4M.m();                // Calculate new real total mass
            G4int bn=totQC.GetBaryonNumber(); // The BaryNum after addition
            totPDG=totQC.GetSPDGCode();
            if(totPDG==10&&totQC.GetBaryonNumber()>0) totPDG=totQC.GetZNSPDGCode();
            if(bn>1)
		          {
              totS  =totQC.GetStrangeness();  // Total Strangeness of this System
              if(totS>=0)                     // => "This is a normal nucleus" case
			           {
                G4QNucleus newN(totQC,tot4M);
                totPDG=newN.GetPDG();
                totM  =newN.GetMZNS();           // Calculate new minimum (GS) mass
			           }
              else if(totS==-1)                  // => "Try to decay in K+/aK0 and finish"
			           {
                G4double m1=mK;         
                G4int  PDG1=321;
                G4QNucleus  newNp(totQC-KpQC);
                G4int  PDG2=newNp.GetPDG();
                G4double m2=newNp.GetMZNS();
                G4QNucleus  newN0(totQC-K0QC);
                G4double m3=newN0.GetMZNS();
                if (m3+mK0<m2+mK)                // => "aK0+ResA is better" case
	               {
                  m1  =mK0;
                  PDG1=311;
                  m2  =m3;
                  PDG2=newN0.GetPDG();
	               }
                if(totMass>m1+m2)                // => "can decay" case
                {
                  G4LorentzVector fq4M(0.,0.,0.,m1);
                  G4LorentzVector qe4M(0.,0.,0.,m2);
                  if(!G4QHadron(tot4M).DecayIn2(fq4M,qe4M))
			               {
                    G4cerr<<"---Warning---G4QE::HadQE:tM="<<tot4M.m()<<"->aK="<<PDG1<<"(M="
                          <<m1<<")+ResA="<<PDG2<<"(M="<<m2<<")="<<m1+m2<<G4endl;
                    G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                    CleanUp();
		                  if(!CheckGroundState(quasH,true))
                    {
                      G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                      theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                      G4cerr<<"***G4QE::HQE:FillAsIs(7),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
		  		                //throw G4QException("G4QEnv::HadrQEnv: aK+ResA DecayIn2 error");
                    }
                    delete quasH;  
                    return theQHadrons;
			               }
                  G4QHadron* H1 = new G4QHadron(PDG1,fq4M);
#ifdef pdebug
                  G4cout<<"G4QE::HQE:Kaon(Env)="<<PDG1<<fq4M<<G4endl;
#endif
                  theQHadrons.push_back(H1);     // (delete equivalent)
                  G4QHadron* H2 = new G4QHadron(PDG2,qe4M);
#ifdef pdebug
                  G4cout<<"G4QE::HQE:ResidEnv="<<PDG2<<qe4M<<G4endl;
#endif
                  theQHadrons.push_back(H2);     // (delete equivalent)
                  break;
			             }
                else totM=250000.;               // => "Continue reversion" case
			           }
              else if(totS==-2)                  //=>"Try to decay in 2(K+/aK0) and finish"
		  	         {
                G4double m1=mK;         
                G4int  PDG1=321;
                G4double m2=mK0;         
                G4int  PDG2=311;
                G4QNucleus  newNp0(totQC-KpQC-K0QC);
                G4int  PDG3=newNp0.GetPDG();
                G4double m3=newNp0.GetMZNS();    // M-K^0-K^+
                G4QNucleus  newN00(totQC-K0QC-K0QC);
                G4double m4=newN00.GetMZNS();    // M-2*K^0
                G4QNucleus  newNpp(totQC-KpQC-KpQC);
                G4double m5=newNpp.GetMZNS();    // M-2*K^+
                if (m4+mK0+mK0<m3+mK+mK0 && m4+mK0+mK0<=m5+mK+mK) //=>"2K0+ResA is theBest"
	               {
                  m1  =mK0;
                  PDG1=311;
                  m3  =m4;
                  PDG3=newN00.GetPDG();
	               }
                else if(m5+mK+mK<m3+mK+mK0 && m5+mK+mK<=m4+mK0+mK0)//=>"2Kp+ResA isTheBest"
	               {
                  m2  =mK;
                  PDG1=321;
                  m3  =m5;
                  PDG3=newNpp.GetPDG();
	               }
                if(totMass>m1+m2+m3)             // => "can decay" case
                {
                  G4LorentzVector k14M(0.,0.,0.,m1);
                  G4LorentzVector k24M(0.,0.,0.,m2);
                  G4LorentzVector ra4M(0.,0.,0.,m3);
                  if(!G4QHadron(tot4M).DecayIn3(k14M,k24M,ra4M))
			               {
                    G4cerr<<"--Warning--G4QE::HQE:tM="<<tot4M.m()<<"->aK="<<PDG1<<"(M="<<m1
                          <<")+K2="<<PDG2<<"(M="<<m2<<")+A="<<PDG3<<"(M="<<m3<<")"<<G4endl;
                    G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                    CleanUp();
		                  if(!CheckGroundState(quasH,true))
                    {
                      G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                      theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                      G4cerr<<"***G4QE::HQE:FillAsIs(8),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
				                  //throw G4QException("G4QEnv::HadrQE:2K+ResidNucleus DecIn3 Error");
                    }
                    delete quasH;  
                    return theQHadrons;
			               }
                  G4QHadron* H1 = new G4QHadron(PDG1,k14M);
                  theQHadrons.push_back(H1);     // (delete equivalent)
#ifdef pdebug
                  G4cout<<"G4QE::HQE:K1(Env)="<<PDG1<<k14M<<G4endl;
#endif
                  G4QHadron* H2 = new G4QHadron(PDG2,k24M);
                  theQHadrons.push_back(H2);     // (delete equivalent)
#ifdef pdebug
                  G4cout<<"G4QE::HQE:K2(Env)="<<PDG2<<k24M<<G4endl;
#endif
                  G4QHadron* H3 = new G4QHadron(PDG3,ra4M);
                  theQHadrons.push_back(H3);     // (delete equivalent)
#ifdef pdebug
                  G4cout<<"G4QE::HQE:ResKKEnv="<<PDG3<<ra4M<<G4endl;
#endif
                  break;
			             }
                else totM=270000.;               // => "Continue reversion" case
			           }
              else totM=300000.;                 // => "Continue reversion" case
			         }
            else
			         {
              if     (totPDG==1114||totPDG==2224||totPDG==10) // Decay right now and finish
			           {
                G4double m1=mNeut;
                G4int  PDG1=2112;
                G4double m2=mPi;
                G4int  PDG2=-211;
				            if(totPDG==2224)
				            {
                  m1=mProt;
                  PDG1=2212;
                  m2=mPi;
                  PDG2=211;
				            }
                else if(totPDG==10)              // "Chipolino" case
				            {
                  G4QChipolino resChip(totQC);   // define the residual as a Chipolino
                  G4QPDGCode h1=resChip.GetQPDG1();
                  PDG1=h1.GetPDGCode();          // PDG code of the first hadron
                  m1  =h1.GetMass();             // Mass of the first hadron
                  G4QPDGCode h2=resChip.GetQPDG2();
                  PDG2=h2.GetPDGCode();          // PDG code of the second hadron
                  m2  =h2.GetMass();             // Mass of the second hadron
				            }
                if(totMass>m1+m2)
				            {
                  G4LorentzVector fq4M(0.,0.,0.,m1);
                  G4LorentzVector qe4M(0.,0.,0.,m2);
                  if(!G4QHadron(tot4M).DecayIn2(fq4M,qe4M))
			               {
                    G4cerr<<"---Warning---G4QE::HaQE:tM="<<tot4M.m()<<"-> h1="<<PDG1<<"(M="
					                     <<m1<<") + h2="<<PDG2<<"(M="<<m2<<")="<<m1+m2<<G4endl;
                    G4Quasmon* quasH = new G4Quasmon(totQC,tot4M); // totQC not tQC!
                    CleanUp();
		                  if(!CheckGroundState(quasH,true))
                    {
                      G4QHadron* hadr = new G4QHadron(totQC,tot4M); // totQC not tQC!
                      theQHadrons.push_back(hadr);   // Cor or fill as It Is
#ifdef pdebug
                      G4cerr<<"***G4QE::HQE:FillAsIs(9),QC="<<totQC<<",4M="<<tot4M<<G4endl;
#endif
				                  //throw G4QException("G4QEnv::HadrQEnv: h1+h2 DecayIn2 Error");
                    }
                    delete quasH;  
                    return theQHadrons;
			               }
                  G4QHadron* H1 = new G4QHadron(PDG1,fq4M);
                  theQHadrons.push_back(H1);     // (delete equivalent)
#ifdef pdebug
                  G4cout<<"G4QE::HQE:h1="<<PDG1<<fq4M<<G4endl;
#endif
                  G4QHadron* H2 = new G4QHadron(PDG2,qe4M);
#ifdef pdebug
                  G4cout<<"G4QE::HQE:h2="<<PDG2<<qe4M<<G4endl;
#endif
                  theQHadrons.push_back(H2);     // (delete equivalent)
                  break;
				            }
                else totM=350000.;
			           }
			           else if(totPDG) totM=G4QPDGCode(totPDG).GetMass();
              else totM=400000.;
			         }
            totBN=totQC.GetBaryonNumber();      // The BaryNum after addition
            totS=totQC.GetStrangeness();        // The Strangeness after addition
            G4double dM=totMass-totM;
#ifdef fdebug
		          G4cout<<"G4QEnv::HadrQE: Add H="<<last4M<<lastQC<<",tM="<<tot4M<<totM<<totQC
                  <<",dM="<<dM<<", tB="<<totBN<<", tS="<<totS<<G4endl;
#endif
            if(dM>-0.001&&totPDG)
		          {
              CleanUp();
              G4QHadron* evH = new G4QHadron(totPDG,tot4M);//Create Hadron for ResidNucleus
              EvaporateResidual(evH);           // Evaporate ResNuc (del.equiv)
              break;
		          }
            nOfOUT  = theQHadrons.size();       // Update the value of OUTPUT entries
		        } // End of WHILE(nOfOUT)
          nOfOUT  = theQHadrons.size();         // Update the value of OUTPUT entries
		        if(!nOfOUT)
		        {
		          G4cerr<<"---Warning---G4QEnv::HadrQE:M="<<totMass<<"<gsM="<<totM<<",dM="<<dM
                  <<", tPDG="<<totPDG<<", t4M="<<tot4M<<G4endl;
			         // throw G4QException("G4QEnvironment::HadronizeQEnv:Can't decayExhostedQEnv");
            CleanUp();
            G4QHadron* evH = new G4QHadron(totPDG,tot4M);// Create Hadron for ResidNucleus
            EvaporateResidual(evH);             // Evaporate ResidNucl (del.equiv)
	         }
		      }
        else                                    // "Last decay was fatal" case @@ buggy ?MK
		      {
#ifdef pdebug
		        G4cout<<"***G4QEnv::HadrQE: M="<<totMass<<",dM="<<dM<<",nQ="<<nQuasmons<<G4endl;
#endif
          G4Quasmon* quasH = new G4Quasmon(totQC,tot4M);
          CleanUp();
		        if(!CheckGroundState(quasH,true))
          {
            G4QHadron* hadr = new G4QHadron(totQC,tot4M);
#ifdef pdebug
            G4cout<<"G4QE::HQE:CheckGS failed H="<<totQC<<tot4M<<G4endl;
#endif
            theQHadrons.push_back(hadr); // Cor or fill asItIs
          }
          delete quasH;  
        }
        CleanUp();
	     }
	   } // End of "infinit" WHILE LOOP
  } // End of the "Nuclear Environment" case
  return theQHadrons;
} // End of the main member function HadronizeQEnvironment

// Clean up the QEnvironment to Zero
void G4QEnvironment::CleanUp()
//   =========================
{
  static const G4QNucleus vacuum(90000000);
  theEnvironment=vacuum;
  G4int nQuasmons = theQuasmons.size();
  if (nQuasmons) for (G4int iq=0; iq<nQuasmons; iq++)theQuasmons[iq]->KillQuasmon();
} // End of CleanUp

//Evaporate Residual Nucleus
void G4QEnvironment::EvaporateResidual(G4QHadron* qH, G4bool corFlag)
{//  ================================================================
  static const G4double mHel6 = G4QPDGCode(2112).GetNuclMass(2,4,0);
  static const G4double mAlph = G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double mDeut = G4QPDGCode(2112).GetNuclMass(1,1,0);
  static const G4double mNeut = G4QPDGCode(2112).GetMass();
  static const G4double mProt = G4QPDGCode(2212).GetMass();
  static const G4double mLamb = G4QPDGCode(3122).GetMass();
  static const G4double mPi   = G4QPDGCode(211).GetMass();
  static const G4double mPi0  = G4QPDGCode(111).GetMass();
  static const G4double mK    = G4QPDGCode(321).GetMass();
  static const G4double mK0   = G4QPDGCode(311).GetMass();
  static const G4QContent neutQC(2,1,0,0,0,0);
  static const G4QContent protQC(1,2,0,0,0,0);
  static const G4QContent lambQC(1,1,1,0,0,0);
  static const G4QContent deutQC(3,3,0,0,0,0);
  static const G4QContent alphQC(6,6,0,0,0,0);
  G4int thePDG = qH->GetPDGCode();         // Get PDG code of the Residual Nucleus
  G4int theBN  = qH->GetBaryonNumber();    // Baryon number of the nucleus
  if(theBN==1)
  {
#ifdef fdebug
    G4cout<<"G4QE::EvaporateRes: Baryon="<<thePDG<<qH->Get4Momentum()<<G4endl;
#endif
    DecayBaryon(qH); // (delete equivalent)
    return;
  }
  else if(!theBN) // @@ In future it is usefull to add the MesonExcitationDecay (?!)
  {
#ifdef fdebug
    G4LorentzVector mesLV=qH->Get4Momentum();
    G4cout<<"G4QE::EvaporateRes:(!)Meson(!) PDG="<<thePDG<<",4M="<<mesLV<<mesLV.m()
          <<",QC="<<qH->GetQC()<<",MPDG="<<G4QPDGCode(thePDG).GetMass()<<G4endl;
#endif
    //DecayMeson(qH);
    //return;
  }
  /// @@@@@@@ *** TEMPORARY TO AVOID HYPERMUCLEI FOR GEANT4 *** @@@@@@@
  if(thePDG>91000000) //@@MadeForGeant4@@: If there is a Lambda, substitute it by a neutron
  {
    G4int S=(thePDG-90000000)/1000000;
    thePDG-=S*999999;                  // S Neutrons instead of S Lambdas
    qH->SetQPDG(G4QPDGCode(thePDG));
  }
  /// @@@ *** ^^^ END OF TEMPORARY ^^^ *** @@@
  if(thePDG<80000000)
  {
#ifdef pdebug
    G4cout<<"G4QE::EvaporateRes: FundamentalParticle="<<thePDG<<qH->Get4Momentum()<<G4endl;
#endif
    theQHadrons.push_back(qH);// TheFundamentalParticles must be FilledAsTheyAre(del.eq)
    return;
  }
  G4QContent  theQC  = qH->GetQC();          // Quark Content of the hadron
  //G4int theBN=theQC.GetBaryonNumber();       // A
  G4int theC=theQC.GetCharge();              // P
  G4int theS=theQC.GetStrangeness();         // S
  G4int theN=theBN-theC-theS;                // N
  if(!thePDG) thePDG = theQC.GetSPDGCode();  // If there is no PDG code, get it from QC
  if(thePDG==10&&theQC.GetBaryonNumber()>0) thePDG=theQC.GetZNSPDGCode();
  G4double totGSM = G4QNucleus(thePDG).GetGSMass();// TheGroundStMass of theTotalResNucleus
  G4LorentzVector q4M = qH->Get4Momentum();  // Get 4-momentum of theTotalResidNucleus
  G4double    totMass = q4M.m();             // Get theRealMass of theTotalResidNucleus
#ifdef pdebug
  G4cout<<"G4QEnvironment::EvapResid(EvaRes):===IN==> PDG="<<thePDG<<",4Mom="<<q4M<<G4endl;
  G4cout<<"G4QEnviron::EvaRes:A="<<theBN<<", Z="<<theC<<", N="<<theN<<", S="<<theS<<G4endl;
#endif
  if     (thePDG==90000000)                  // ==> "Nothing in the INPUT Hadron" KEEP IT!
  {
    delete qH;
#ifdef debug
    G4cerr<<"---Warning---G4QEnvir::EvaRes: Residual Nucleus is vacuum,4Mom="<<q4M<<G4endl;
#endif
    return;
  }
  else if(thePDG==91000000||thePDG==90001000||thePDG==90000001)
  //else if(2>3)// One can easily close this decay as it will be done later (time of calc?)
  {
    G4double gsM=mNeut;
    if(thePDG==90001000)      gsM=mProt;
	   else if(thePDG==91000000) gsM=mLamb;
    if(fabs(totMass-gsM)<.001)
    {
#ifdef pdebug
	     G4cout<<"G4QE::EvaporateR:GSM="<<gsM<<", H="<<thePDG<<qH->Get4Momentum()<<G4endl;
#endif
      theQHadrons.push_back(qH); // (delete equivalent)
      return;
    }
    else if(totMass<gsM)
	   {
       G4cerr<<"***G4QE::EvaRes:Baryon "<<thePDG<<" is belowMassShell M="<<totMass<<G4endl;
       throw G4QException("G4QEnvironment::EvaporateResidual: Baryon is below Mass Shell");
    }
    else                                 // Decay in gamma or charged pion (@@ neutral)
    {
      G4double d=totMass-gsM;
#ifdef debug
	     G4cout<<"G4QE::EvaporResid:PDG="<<thePDG<<",M="<<totMass<<">"<<gsM<<",d="<<d<<G4endl;
#endif
      G4int decPDG=22;
      G4double decM=0.;
      if(d>142.)                           // @@ to avoid more precise calculations
						{
        if(thePDG==90001000)               // D+ -> n + pi+
								{
          gsM=mNeut;
          thePDG=90000001;
          decPDG=211;
          decM=mPi;
        }
        else if(thePDG==90000001)          // D+ -> n + pi+
								{
          gsM=mProt;
          thePDG=90001000;
          decPDG=-211;
          decM=mPi;
        }
        else
								{
          decPDG=111;
          decM=mPi0;
        }
      }
      G4LorentzVector h4Mom(0.,0.,0.,gsM); // GSMass must be updated in previous while-LOOP
      G4LorentzVector g4Mom(0.,0.,0.,decM);
      if(!G4QHadron(q4M).DecayIn2(h4Mom, g4Mom))
      {
        G4cerr<<"***G4QEnv::EvaRes:h="<<thePDG<<"(GSM="<<gsM<<")+gam>tM="<<totMass<<G4endl;
        throw G4QException("G4QEnvironment::EvaporateResidual:BaryonDecInBaryon+GamError");
      }
#ifdef debug
	     G4cout<<"G4QE::EvaRes:"<<totMass<<q4M<<"->"<<thePDG<<h4Mom<<"+g="<<g4Mom<<",n="
            <<theQHadrons.size()<<G4endl;
#endif
      G4QHadron* curH = new G4QHadron(thePDG,h4Mom);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR:Hadr="<<thePDG<<h4Mom<<G4endl;
#endif
      theQHadrons.push_back(curH);         // Fill Baryon (delete equiv.)
      G4QHadron* curG = new G4QHadron(decPDG,g4Mom);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateResidual: Gamma(pion)4M="<<g4Mom<<G4endl;
#endif
      theQHadrons.push_back(curG);         // Fill gamma/pion (delete equivalent)
      delete qH;
    }
  }
  else if((thePDG==90001999||thePDG==89999002) && totMass>1080.) // @@ to avoid threshold
  //else if(2>3)// One can easily close this decay as it will be done later (time of calc?)
  {
    G4double gsM=mNeut;
    G4int barPDG=2112;
    G4int mesPDG=-211;
    if(thePDG==90001999)
    {
      gsM=mProt;
      barPDG=2212;
      mesPDG=211;
    }
    if(fabs(totMass-gsM-mPi)<.001)
    {
#ifdef pdebug
	     G4cout<<"G4QE::EvaporateR:(D)GSM="<<gsM<<", H="<<thePDG<<qH->Get4Momentum()<<G4endl;
#endif
      G4LorentzVector h4Mom=q4M*(gsM/totMass);           // At rest in CM
      G4QHadron* curB = new G4QHadron(barPDG,h4Mom);
      theQHadrons.push_back(curB); // (delete equivalent)
      G4LorentzVector g4Mom=q4M*(mPi/totMass);
      G4QHadron* curM = new G4QHadron(mesPDG,g4Mom);
      theQHadrons.push_back(curM); // (delete equivalent)
      return;
    }
    else if(totMass<gsM+mPi)
	   {
       G4cerr<<"***G4QE::EvaRes:Delta "<<thePDG<<" is belowMassShell M="<<totMass<<G4endl;
       throw G4QException("G4QEnvironment::EvaporateResidual: Delta is below Mass Shell");
    }
    else                                 // Decay in gamma or charged pion (@@ neutral)
    {
      G4LorentzVector h4Mom(0.,0.,0.,gsM); // GSMass must be updated in previous while-LOOP
      G4LorentzVector g4Mom(0.,0.,0.,mPi);
      if(!G4QHadron(q4M).DecayIn2(h4Mom, g4Mom))
      {
        G4cerr<<"***G4QEnv::EvaRes:Dh="<<thePDG<<"N+pi="<<gsM+mPi<<">tM="<<totMass<<G4endl;
        throw G4QException("G4QEnvironment::EvaporateResidual: DeltaDecInBaryon+Pi Error");
      }
#ifdef debug
	     G4cout<<"G4QE::EvaRes:"<<totMass<<q4M<<"->"<<thePDG<<h4Mom<<"+pi="<<g4Mom<<", nH="
            <<theQHadrons.size()<<G4endl;
#endif
      G4QHadron* curH = new G4QHadron(barPDG,h4Mom);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR: Nucleon="<<thePDG<<h4Mom<<G4endl;
#endif
      theQHadrons.push_back(curH);         // Fill the nucleon (delete equiv.)
      G4QHadron* curG = new G4QHadron(mesPDG,g4Mom);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR: Pion="<<g4Mom<<G4endl;
#endif
      theQHadrons.push_back(curG);         // Fill the pion (delete equivalent)
      delete qH;
    }
  }
  else if(theBN>0&&theS<0) DecayAntiStrange(qH); // "AntyStrange Nucleus" case (del eq.)
  else if(theBN>0&&(theC<0||theC>theBN-theS)) DecayIsonucleus(qH);// "UnavoidableIsonucl"
  else if(theBN==2) DecayDibaryon(qH);           //=> "Dibaryon" case (del eq.)
  else if(thePDG==89999003||thePDG==90002999)    //=> "ISO-dibarion" (@@Look at prev.line!)
  {
    // @@ Check that it never comes here !!
    G4int  nucPDG = 2112;
    G4double nucM = mNeut;
    G4int   piPDG = -211;
    if(thePDG==90002999)
	   {
      nucPDG = 2212;
      nucM   = mProt;
      piPDG  = 211;
	   }
    if(totMass>mPi+nucM+nucM)
	   {
      G4LorentzVector n14M(0.,0.,0.,nucM);
      G4LorentzVector n24M(0.,0.,0.,nucM);
      G4LorentzVector pi4M(0.,0.,0.,mPi);
      if(!G4QHadron(q4M).DecayIn3(n14M,n24M,pi4M))
	     {
        G4cerr<<"***G4QEnv::HadQEnv: tM="<<totMass<<"-> 2N="<<nucPDG<<"(M="
		      <<nucM<<") + pi="<<piPDG<<"(M="<<mPi<<")"<<G4endl;
		      throw G4QException("G4QEnv::EvaporateResidual:ISO-dibaryon DecayIn3 error");
	     }
      delete qH;
      G4QHadron* h1H = new G4QHadron(nucPDG,n14M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR:Bar1="<<nucPDG<<n14M<<G4endl;
#endif
      theQHadrons.push_back(h1H);                // (delete equivalent)
      G4QHadron* h2H = new G4QHadron(nucPDG,n24M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR:Bar2="<<nucPDG<<n24M<<G4endl;
#endif
      theQHadrons.push_back(h2H);                // (delete equivalent)
      G4QHadron* piH = new G4QHadron(piPDG,pi4M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR:Pi="<<piPDG<<pi4M<<G4endl;
#endif
      theQHadrons.push_back(piH);                // (delete equivalent)
	   }
	   else
	   {
      delete qH;
      G4cerr<<"***G4QEnv::EvaRes: IdPDG="<<thePDG<<", q4M="<<q4M<<", M="<<totMass
            <<" < M_2N+Pi, d="<<totMass-2*nucM-mPi<<G4endl;
      throw G4QException("G4QEnvironment::EvaporateResidual:ISO-DiBar is under MassShell");
	   }
  }
  else if((thePDG==90000002||thePDG==90001001||thePDG==90002000)&&totMass>2020.) //=> IsoDB
  {
    // @@ Pi0 can be taken into account !
    G4int  n1PDG = 2212;
    G4int  n2PDG = 2112;
    G4int  piPDG = -211;
    G4double n1M = mProt;
    G4double n2M = mNeut;
    if     (thePDG==90002000) piPDG  =  211;      // pp -> np + pi-
    else if(thePDG==90000002) piPDG  = -211;      // nn -> np + pi-
    else                                          // np -> 50%(nnpi+) 50%(pppi-)
    {
      if(G4UniformRand()>.5)
						{
        n1PDG=2112;
        n1M=mNeut;
        piPDG  =  211;
      }
      else
						{
        n2PDG=2212;
        n2M=mProt;
        piPDG  = -211;
      }
    }
    if(totMass>mPi+n1M+n2M)
	   {
      G4LorentzVector n14M(0.,0.,0.,n1M);
      G4LorentzVector n24M(0.,0.,0.,n2M);
      G4LorentzVector pi4M(0.,0.,0.,mPi);
      if(!G4QHadron(q4M).DecayIn3(n14M,n24M,pi4M))
	     {
        G4cerr<<"***G4QEnv::HadQEnv:IsoDN, tM="<<totMass<<"-> N1="<<n1PDG<<"(M="<<n1M
              <<") + N2="<<n2PDG<<"(M="<<n2M<<") + pi="<<piPDG<<"(M="<<mPi<<")"<<G4endl;
		      throw G4QException("G4QEnv::EvaporateResidual:ISO-dibaryon excit. DecayIn3 error");
	     }
      delete qH;
      G4QHadron* h1H = new G4QHadron(n1PDG,n14M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateRes: Bar1="<<n1PDG<<n14M<<G4endl;
#endif
      theQHadrons.push_back(h1H);                // (delete equivalent)
      G4QHadron* h2H = new G4QHadron(n2PDG,n24M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateRes: Bar2="<<n2PDG<<n24M<<G4endl;
#endif
      theQHadrons.push_back(h2H);                // (delete equivalent)
      G4QHadron* piH = new G4QHadron(piPDG,pi4M);
#ifdef pdebug
      G4cout<<"G4QE::EvaporateR: Pi="<<piPDG<<pi4M<<G4endl;
#endif
      theQHadrons.push_back(piH);                // (delete equivalent)
	   }
	   else
	   {
      delete qH;
      G4cerr<<"***G4QEnv::EvaRes: IdPDG="<<thePDG<<", q4M="<<q4M<<", M="<<totMass
            <<" < M1+M2+Pi, d="<<totMass-n1M-n2M-mPi<<G4endl;
      throw G4QException("G4QEnvironment::EvaporateResidual:ISODiBarEx's under MassShell");
	   }
  }
  else if(fabs(totMass-totGSM)<.001)  // Fill as it is or decay Be8, He5, Li5 (@@ add more)
  {
#ifdef pdebug
    G4cout<<"G4QE::EvaR: GS "<<qH->GetQC()<<qH->Get4Momentum()<<" AsIs"<<G4endl;
#endif
    if(thePDG==90004004) DecayAlphaAlpha(qH);    // "Alpha+Alpha Decay" case (del eq.)
    else if(thePDG==90004002) DecayAlphaDiN(qH);  // Decay alpha+2p (alpha+2n is stable)
    else if((theC==theBN||theN==theBN||theS==theBN)&&theBN>1) DecayMultyBaryon(qH); 
    else if(theBN==5)    DecayAlphaBar(qH);   // Try to decay unstable A5 system (del eq.)
    else                 theQHadrons.push_back(qH); // Fill as it is (del eq.)
    //return;
  }
  else if(theBN>0&&thePDG>88000000&&thePDG<89000000)//==> 2antiK in the nucleus (!Comment!)
  {
    G4cerr<<"---Warning---G4QEnv::EvaRes:MustNotBeHere.PDG="<<thePDG<<",S="<<theS<<G4endl;
    G4int bZ=theQC.GetCharge();
    G4int bN=theBN-bZ;
    G4int k1PDG = 321;
    G4double mK1= mK;
    G4int k2PDG = 321;
    G4double mK2= mK;
    G4int  nucPDG = thePDG;
    if(bZ>=bN) nucPDG+=999000;
    else
    {
      nucPDG+=999999;
      k1PDG = 311;
      mK1= mK0;
    }
    if(bZ>bN) nucPDG+=999000;
    else
    {
      nucPDG+=999999;
      k2PDG = 311;
      mK2= mK0;
    }
    G4double nucM = G4QNucleus(nucPDG).GetGSMass();
    G4cerr<<"-Warning-G4QE::EvR:M="<<nucM<<","<<nucPDG<<",1="<<k1PDG<<",2="<<k2PDG<<G4endl;
    G4LorentzVector n4M(0.,0.,0.,nucM);
    G4LorentzVector k14M(0.,0.,0.,mK1);
    G4LorentzVector k24M(0.,0.,0.,mK2);
    if(!G4QHadron(q4M).DecayIn3(n4M,k14M,k24M))
	   {
      G4cerr<<"***G4QEnv::EvaRes: tM="<<totMass<<"-> N="<<nucPDG<<"(M="<<nucM<<") + k1="
            <<k1PDG<<"(M="<<mK1<<") + k2="<<k2PDG<<"(M="<<mK2<<")"<<G4endl;
	     throw G4QException("G4QEnv::EvaporateResidual: Nucleus+2antiK DecayIn3 error");
	   }
    delete qH;
    G4QHadron* k1H = new G4QHadron(k1PDG,k14M);
#ifdef pdebug
    G4cout<<"G4QE::EvaRes:k1="<<k1PDG<<k14M<<G4endl;
#endif
    theQHadrons.push_back(k1H);                // (delete equivalent)
    G4QHadron* k2H = new G4QHadron(k2PDG,k24M);
#ifdef pdebug
    G4cout<<"G4QE::EvaRes:k2="<<k2PDG<<k24M<<G4endl;
#endif
    theQHadrons.push_back(k2H);                // (delete equivalent)
    G4QHadron* nH = new G4QHadron(nucPDG,n4M);
#ifdef pdebug
    G4cout<<"G4QE::EvaRes:resNuc="<<nucPDG<<n4M<<G4endl;
#endif
    theQHadrons.push_back(nH);                 // (delete equivalent)
  }
  // From here the EVA code starts
  else if(thePDG>80000000&&thePDG!=90000000||thePDG==2112||thePDG==2212||thePDG==3122)
  { // @@ Improve for Sigma+, Sigma-, Ksi0 & Ksi- content in the Total Residual Nucleus
    if(thePDG<80000000)                        // Switch from QHadronCode to QNuclearCode
    {
      if     (thePDG==2112) thePDG=90000001;   // n
      else if(thePDG==2212) thePDG=90001000;   // p
      else if(thePDG==3122) thePDG=91000000;   // lambda
	   }
    G4QNucleus qNuc(q4M,thePDG);               // Make a Nucleus for theTotalResidNucleus
    G4double GSMass =qNuc.GetGSMass();         // GSMass of the Total Residual Nucleus
    G4QContent totQC=qNuc.GetQCZNS();          // QuarkCont of theTotalResidNucleus (theQC)
    G4int    bA     =qNuc.GetA();              // A#of baryons in theTotal Residual Nucleus
    G4int    bZ     =qNuc.GetZ();              // A#of protons in theTotal Residual Nucleus
    G4int    bN     =qNuc.GetN();              // A#of neutrons in theTotal ResidualNucleus
    G4int    bS     =qNuc.GetS();              // A#of lambdas in theTotal Residual Nucleus
#ifdef fdebug
    if(bZ==2&&bN==5)G4cout<<"G4QE::EvR: (2,5) GSM="<<GSMass<<" > "
						  <<G4QPDGCode(2112).GetNuclMass(2,4,0)+mNeut<<G4endl;
    if(bZ==1&&bN==3)G4cout<<"G4QE::EvR: (1,3) GSM="<<GSMass<<" > "
						  <<G4QPDGCode(2112).GetNuclMass(1,2,0)+mNeut<<G4endl;
    G4double dM=totMass-GSMass;
	   G4cout<<"G4QEnv::EvaRes:"<<qNuc<<",PDG="<<thePDG<<",M="<<totMass<<",dM="<<dM<<G4endl;
    ////////if(dM>7.) throw G4QException("G4QEnvironment::EvaporateResidual: CALLED");
#endif
    G4int   bsCond =qNuc.SplitBaryon();        // (Bary/Deut/Alph)SeparCond for TotResNucl
    G4bool  dbsCond=qNuc.Split2Baryons();      // (Two Baryons)SeparCond for TotResidNucl
#ifdef debug
	   G4cout<<"G4QEnv::EvaRes: bs="<<bsCond<<", dbs="<<dbsCond<<", A="<<bA<<G4endl;
#endif
    if(fabs(totMass-GSMass)<.003&&!bsCond&&!dbsCond) // GS or can't split 1(2)B FillAsItIs
    {
#ifdef pdebug
      G4cout<<"G4QE::EvaR: GS direct "<<qH->GetQC()<<qH->Get4Momentum()<<" AsIs"<<G4endl;
#endif
      theQHadrons.push_back(qH);
      return;
    }
    else if((bA==1||!bsCond&&!dbsCond)&&totMass>GSMass+.003)//==>Fuse&DecayTech (no gamDec)
	   //if(2>3)                                  // Close "Fuse&Decay Technology" ***@@@***
	   {
#ifdef debug
	     G4cout<<"G4QE::EvaR:SplitBar, s="<<bsCond<<",M="<<totMass<<" > GSM="<<GSMass<<G4endl;
#endif
      G4int nOfOUT = theQHadrons.size();       // Total#of QHadrons in Vector at this point
      while(nOfOUT && corFlag)                 // Try BackFusionDecays till something is in
	     {
        G4QHadron*     theLast = theQHadrons[nOfOUT-1];
        G4int          lastBN = theLast->GetBaryonNumber();
        G4int          nFragm = theLast->GetNFragments();
        //////////////////G4int          gam    = theLast->GetPDGCode();
#ifdef debug
		      G4cout<<"G4QE::EvaRes:*BackFus,BN="<<lastBN<<",nF="<<nFragm<<",n="<<nOfOUT<<G4endl;
#endif
		      while(nFragm)                            // => "Delete Decayed Hadrons" case
		      {
          G4QHadron* thePrev = theQHadrons[nOfOUT-2];
          nFragm = thePrev->GetNFragments();
#ifdef debug
          G4int          prevPDG = thePrev->GetPDGCode();
		        G4cout<<"G4QEnv::EvaRes:DelTheLast, nFr="<<nFragm<<", pPDG="<<prevPDG<<G4endl;
#endif
          theQHadrons.pop_back();          // the prev QHadron is excluded from OUTPUT
          delete theLast;//!!When kill,DON'T forget to del. theLastQHadron as an instance!!
          theLast = thePrev;               // Update theLastPntr(Prev instead of Last)
		        nOfOUT--;
		      }
        if(nOfOUT)
		      {
          if(lastBN<1&&nOfOUT>1)           // => "Skip Meson/Gams & Antibaryons" case
		        {
            G4QHadron* thePrev = theQHadrons[nOfOUT-2];//***Exchange between Last & Prev***
            theQHadrons.pop_back();        // the last QHadron is excluded from OUTPUT
            theQHadrons.pop_back();        // the prev QHadron is excluded from OUTPUT
            theQHadrons.push_back(theLast);// the Last becomes the Prev (1st part of exch.)
            theQHadrons.push_back(thePrev);// the Prev becomes the Last (2nd part of exch.)
            theLast = thePrev;             // Update theLastPointer (Prev instead of Last)
		        }
          G4LorentzVector last4M = theLast->Get4Momentum();
          G4QContent  lastQC = theLast->GetQC();
          G4double lastM  = last4M.m();    // Mass of the BackFused Fragment
          totQC+=lastQC;                   // Update (increase) the total QC
          q4M+=last4M;                     // Update (increase) the total 4-momentum
          totMass=q4M.m();                 // Calculate new real total mass
          G4int totPDG=totQC.GetSPDGCode();// The updated PDG for the TotalResidualNucleus
          if(totPDG==10&&totQC.GetBaryonNumber()>0) totPDG=totQC.GetZNSPDGCode();
          G4int totBN=totQC.GetBaryonNumber();// BaryonNumber of the Total Residual Nucleus
          G4double dM=totMass-GSMass -lastM;
#ifdef debug
		        G4cout<<"G4QE::EvaRes: tM="<<totMass<<"-LM="<<lastM<<lastQC<<"-GSM="<<GSMass<<"="
                <<dM<<G4endl;
#endif
          if(dM>-0.001)
		        {
            G4QHadron* evH = new G4QHadron(totPDG,q4M);// Create QHadron for TotalResidNucl
            if(dM<=0.)
            {
              theQHadrons.pop_back();    // lastQHadron is excluded from QHadrV asIs in TRN
              delete theLast; //When kill, DON'T forget to delete lastQHadron asAnInstance!
#ifdef pdebug
              G4cout<<"G4QE::EvaR: EVH "<<totPDG<<q4M<<" fill AsIs"<<G4endl;
#endif
              if(totBN==2)DecayDibaryon(evH); // Fill dibaryon (with decay products)
              else theQHadrons.push_back(evH);// Fill TRN to HVect asIs (delete equivalent)
		          }
            else                        // Decay TotalResidualNucleus in GSM+Last and Break
		          {
              G4LorentzVector r4Mom(0.,0.,0.,GSMass);
              if(!G4QHadron(q4M).DecayIn2(last4M,r4Mom))
              {
                theQHadrons.pop_back(); // lastQHadron is excluded from QHadrV as is in TRN
                delete theLast; //When kill,DON'T forget to delete lastQHadron asAnInstance
#ifdef pdebug
              G4cout<<"***G4QE::EvaR: EVH "<<totPDG<<q4M<<" fill AsIs"<<G4endl;
#endif
                theQHadrons.push_back(evH);// Fill TRN to Vect as it is (delete equivalent)
#ifdef debug
                G4cout<<"***G4QE::EvaR:DecayIn L"<<lastQC<<"+RN"<<totQC<<" failed"<<G4endl;
#endif
	             }
              else
              {
                delete evH;                // Delete theHadron of the TotalResidualNucleus
                theLast->Set4Momentum(last4M);// Already exists:don't create&fill,->set4Mom
                G4QHadron* nuclH = new G4QHadron(thePDG,r4Mom);
#ifdef pdebug
                G4cout<<"G4QE::EvaR: fill NH "<<totPDG<<r4Mom<<" inAnyForm"<<G4endl;
#endif
                // @@ What about others, not DB possibilities?
                if(thePDG==92000000||thePDG==90002000||thePDG==90000002)
                                                                  DecayDibaryon(nuclH);//DE
                else theQHadrons.push_back(nuclH);// Fill the Residual Nucleus (del.eq.)
              }
              break;
		          }
		        }
          thePDG=totPDG;                   // Make ResidualNucleus outOf theTotResidualNucl
		        GSMass=G4QPDGCode(thePDG).GetMass();// Update the Total Residual Nucleus mass
          theQHadrons.pop_back();          // the last QHadron is excluded from OUTPUT
          delete theLast;//!!When kill,DON'T forget to delete theLastQHadron asAnInstance!!
          nOfOUT--;                        // Update the value of OUTPUT entries
		      }
	     }
      if(!nOfOUT || !corFlag)
      {
        G4LorentzVector h4Mom(0.,0.,0.,GSMass);//GSMass must be updated inPreviousWhileLOOP
        G4LorentzVector g4Mom(0.,0.,0.,0.);
        if(!G4QHadron(q4M).DecayIn2(h4Mom, g4Mom))
        {
          G4cerr<<"***G4QE::EvaR: h="<<thePDG<<"(GSM="<<GSMass<<")+g>tM="<<totMass<<G4endl;
          throw G4QException("G4QEnvironment::EvaporateResidual: Decay in Gamma failed");
        }
#ifdef debug
	       G4cout<<"G4QE::EvaRes: "<<q4M<<"->totResN="<<thePDG<<h4Mom<<"+gam="<<g4Mom<<G4endl;
#endif
        G4QHadron* curH = new G4QHadron(thePDG,h4Mom);
#ifdef pdebug
        G4cout<<"G4QE::EvaRes:FillFragment="<<thePDG<<h4Mom<<G4endl;
#endif
        if(thePDG==92000000||thePDG==90002000||thePDG==90000002) DecayDibaryon(curH);//(DE)
        else theQHadrons.push_back(curH);  // Fill the TotalResidualNucleus (del.equiv.)
        G4QHadron* curG = new G4QHadron(22,g4Mom);
#ifdef pdebug
        G4cout<<"G4QE::EvaRes:FillGamma="<<g4Mom<<G4endl;
#endif
        theQHadrons.push_back(curG);       // Fill the gamma (delete equivalent)
	     }
      delete qH;
	   }
    else if(bA==2) DecayDibaryon(qH);      // Decay the residual dibaryon (del.equivalent)
    else if(bA>0&&bS<0) DecayAntiStrange(qH); // Decay the state with antistrangeness
    else if(totMass<GSMass+.003&&(bsCond||dbsCond))//==>" M<GSM but decay is possible" case
    {
#ifdef debug
	     G4cout<<"G4QE::EvR:2B="<<dbsCond<<",B="<<bsCond<<",M="<<totMass<<"<"<<GSMass<<G4endl;
#endif
      G4double gResM  =1000000.;           // Prototype of mass of residual for a gamma
      G4int    gResPDG=0;                  // Prototype of residualPDGCode for a gamma
      if(bN==4&&bZ==2&&!bS)                // It's He6 nucleus
	     {
        gResPDG= thePDG;                   // PDG of the Residual Nucleus
        gResM  = mHel6;                    // min mass of the Residual Nucleus
	     }
      G4double nResM  =1000000.;           // Prototype of mass of residual for a neutron
      G4int    nResPDG=0;                  // Prototype of ResidualPDGCode for a neutron
      if(bsCond==2112&&bN>0&&bA>1)         // There's aNeutr in theNucl, which can be split
	     {
        G4QContent resQC=totQC-neutQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        nResPDG=resN.GetPDG();             // PDG of the Residual Nucleus
        if     (nResPDG==90000001) nResM=mNeut;
        else if(nResPDG==90001000) nResM=mProt;
        else if(nResPDG==91000000) nResM=mLamb;
        else nResM=resN.GetMZNS();         // min mass of the Residual Nucleus
	     }
      G4double pResM  =1000000.;           // Prototype of mass of residual for a proton
      G4int    pResPDG=0;                  // Prototype of PDGCode of residual for a proton
      if(bsCond==2212&&bZ>0&&bA>1)         // There's aProton in Nucl, which can be split
	     {
        G4QContent resQC=totQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        pResPDG=resN.GetPDG();             // PDG of the Residual Nucleus
        if     (pResPDG==90000001) pResM=mNeut;
        else if(pResPDG==90001000) pResM=mProt;
        else if(pResPDG==91000000) pResM=mLamb;
        else pResM  =resN.GetMZNS();       // min mass of the Residual Nucleus
	     }
      G4double lResM  =1000000.;           // Prototype of mass of residual for a Lambda
      G4int    lResPDG=0;                  // Prototype of PDGCode of residual for a Lambda
      if(bsCond==3122&&bS>0&&bA>1)         // There's aLambd in theNucl, which can be split
	     {
        G4QContent resQC=totQC-lambQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        lResPDG=resN.GetPDG();             // PDG of the Residual Nucleus
        if     (lResPDG==90000001) lResM=mNeut;
        else if(lResPDG==90001000) lResM=mProt;
        else if(lResPDG==91000000) lResM=mLamb;
        else lResM  =resN.GetMZNS();       // min mass of the Residual Nucleus
	     }
      G4double dResM  =1000000.;           // Prototype of mass of residual for a Alpha
      G4int    dResPDG=0;                  // Prototype of PDGCode of residual for a Alpha
      if(bsCond==90001001&&bN>0&&bZ>0&&bA>2)// There's aDeuter in Nucl, which canBeRadiated
	     {
        G4QContent resQC=totQC-deutQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        dResPDG=resN.GetPDG();             // PDG of the Residual Nucleus
        if     (dResPDG==90000001) dResM=mNeut;
        else if(dResPDG==90001000) dResM=mProt;
        else if(dResPDG==91000000) dResM=mLamb;
        else dResM  =resN.GetMZNS();       // minMass of the Residual Nucleus
	     }
      G4double aResM  =1000000.;           // Prototype of mass of residual for a Alpha
      G4int    aResPDG=0;                  // Prototype of PDGCode of residual for a Alpha
      if(bsCond==90002002&&bN>1&&bZ>1&&bA>4)// There's Alpha in theNucl, which can be split
	     {
        G4QContent resQC=totQC-alphQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        aResPDG=resN.GetPDG();             // PDG of the Residual Nucleus
        if     (aResPDG==90000001) aResM=mNeut;
        else if(aResPDG==90001000) aResM=mProt;
        else if(aResPDG==91000000) aResM=mLamb;
        else aResM  =resN.GetMZNS();       // minMass of the Residual Nucleus
	     }
      G4double nnResM  =1000000.;          // Prototype of mass of residual for a dineutron
      G4int    nnResPDG=0;                 // Prototype of ResidualPDGCode for a dineutron
      if(dbsCond&&bN>1&&bA>2)              // It's nucleus and there is a dineutron
	     {
        G4QContent resQC=totQC-neutQC-neutQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        nnResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (nnResPDG==90000001) nnResM=mNeut;
        else if(nnResPDG==90001000) nnResM=mProt;
        else if(nnResPDG==91000000) nnResM=mLamb;
        else nnResM  =resN.GetMZNS();      // min mass of the Residual Nucleus
	     }
      G4double ppResM  =1000000.;          // Prototype of mass of residual for a diproton
      G4int    ppResPDG=0;                 // Prototype of ResidualPDGCode for a diproton
      if(dbsCond&&bZ>1&&bA>2)              // It's nucleus and there is a diproton
	     {
        G4QContent resQC=totQC-protQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        ppResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (ppResPDG==90000001) ppResM=mNeut;
        else if(ppResPDG==90001000) ppResM=mProt;
        else if(ppResPDG==91000000) ppResM=mLamb;
        else ppResM  =resN.GetMZNS();      // min mass of the Residual Nucleus
	     }
      G4double npResM  =1000000.;          // Prototype of ResidualMass for proton+neutron
      G4int    npResPDG=0;                 // Prototype of ResidualPDGCode for a prot+neut
      if(dbsCond&&bN>0&&bZ>0&&bA>2)        // It's nucleus and there is aProton & aNeutron
	     {
        G4QContent resQC=totQC-neutQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        npResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (npResPDG==90000001) npResM=mNeut;
        else if(npResPDG==90001000) npResM=mProt;
        else if(npResPDG==91000000) npResM=mLamb;
        else npResM  =resN.GetMZNS();      // min mass of the Residual Nucleus
	     }
      G4double lnResM  =1000000.;          // Prototype of residualMass for lambda+neutron
      G4int    lnResPDG=0;                 // Prototype of ResidualPDGCode for aLambda+Neut
      if(dbsCond&&bN>0&&bS>0&&bA>2)        // It's nucleus and there is aLambda & aNeutron
	     {
        G4QContent resQC=totQC-lambQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        lnResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (lnResPDG==90000001) lnResM=mNeut;
        else if(lnResPDG==90001000) lnResM=mProt;
        else if(lnResPDG==91000000) lnResM=mLamb;
        else lnResM  =resN.GetMZNS();      // min mass of the Residual Nucleus
	     }
      G4double lpResM  =1000000.;          // Prototype of residualMass for a proton+lambda
      G4int    lpResPDG=0;                 // Prototype of ResidualPDGCode for theProt+lamb
      if(dbsCond&&bS>0&&bZ>0&&bA>2)        // It's nucleus and there is aProton and aLambda
	     {
        G4QContent resQC=totQC-neutQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        lpResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (lpResPDG==90000001) lpResM=mNeut;
        else if(lpResPDG==90001000) lpResM=mProt;
        else if(lpResPDG==91000000) lpResM=mLamb;
        else lpResM  =resN.GetMZNS();      // minMass of the Residual Nucleus
	     }
      G4double llResM  =1000000.;          // Prototype of mass of residual for a di-lambda
      G4int    llResPDG=0;                 // Prototype of ResidPDGCode for the di-lambda
      if(dbsCond&&bS>1&&bA>2)              // It's nucleus and there is a di-lambda
	     {
        G4QContent resQC=totQC-neutQC-protQC;
        G4QNucleus resN(resQC);            // Pseudo nucleus for the Residual Nucleus
        llResPDG=resN.GetPDG();            // PDG of the Residual Nucleus
        if     (llResPDG==90000001) llResM=mNeut;
        else if(llResPDG==90001000) llResM=mProt;
        else if(llResPDG==91000000) llResM=mLamb;
        else llResM  =resN.GetMZNS();      // min mass of the Residual Nucleus
	     }
#ifdef debug
      G4cout<<"G4QEnv::EvaRes: rP="<<pResPDG<<",rN="<<nResPDG<<",rL="<<lResPDG<<",N="
            <<bN<<",Z="<<bZ<<",nL="<<bS<<",totM="<<totMass<<",n="<<totMass-nResM-mNeut
            <<",p="<<totMass-pResM-mProt<<",l="<<totMass-lResM-mLamb<<G4endl;
#endif
      if(   thePDG==90004004 || thePDG==90002004 && totMass>mHel6+.003
         || bA>4 && bsCond && bN>1 && bZ>1 && totMass>aResM+mAlph
         || bA>1 && bsCond && (   bN>0&&totMass>nResM+mNeut
                               || bZ>0&&totMass>pResM+mProt
							  || bS>0&&totMass>lResM+mLamb)
         ||bA>2&&(bN>0&&bZ>0&&(bsCond&&totMass>dResM+mDeut||dbsCond && totMass>dResM+mDeut)
				               ||dbsCond && (   bN>1&&totMass>nnResM+mNeut+mNeut
                                             || bZ>1&&totMass>ppResM+mProt+mProt
                                             || bS>1&&totMass>llResM+mLamb+mLamb
                                             || bN&&bS&&totMass>lnResM+mLamb+mNeut
                                             || bZ&&bS&&totMass>lpResM+mLamb+mProt)))
	     {
        G4int barPDG = 90002002;           // Just for the default case of Be8->alpha+alpha
        G4int resPDG = 90002002;
        G4int thdPDG = 0;
        G4double barM= mAlph;
        G4double resM= mAlph;
        G4double thdM= mNeut;              // This default value is used in the IF
        G4double tMC=totMass+.0002;
		      if(gResPDG&&tMC>mHel6+.003)        // Can make radiative decay of He6 (priority 0)
		      {
          barPDG=90002004;
          resPDG=22;
          barM  =mHel6;
          resM  =0.;
		      }
		      else if(nResPDG&&tMC>nResM+mNeut)  // Can radiate a neutron (priority 1)
		      {
          barPDG=90000001;
          resPDG=nResPDG;
          barM  =mNeut;
          resM  =nResM;
		      }
		      else if(pResPDG&&totMass+.001>pResM+mProt)   // Can radiate a proton (priority 2)
		      {
          barPDG=90001000;
          resPDG=pResPDG;
          barM  =mProt;
          resM  =pResM;
		      }
		      else if(lResPDG&&tMC>lResM+mLamb)  // Can radiate a Lambda (priority 3) @@ Sigma0
		      {
          barPDG=91000000;
          resPDG=lResPDG;
          barM  =mLamb;
          resM  =lResM;
		      }
        else if(thePDG!=90004004&&bN>1&&bZ>1&&bA>4&&tMC>aResM+mAlph)// Decay in alpha (p4)
		      {
          barPDG=90002002;
          resPDG=aResPDG;
          barM  =mAlph;
          resM  =aResM;
		      }
		      else if(dResPDG&&tMC>dResM+mDeut)  // Can radiate a Deuteron (priority 5)
		      {
          barPDG=90001001;
          resPDG=dResPDG;
          barM  =mDeut;
          resM  =dResM;
		      }
		      else if(ppResPDG&&tMC>ppResM+mProt+mProt)// Can radiate a DiProton (priority 6)
		      {
          barPDG=90001000;
          resPDG=ppResPDG;
          thdPDG=90001000;
          barM  =mProt;
          resM  =ppResM;
          thdM  =mProt;
		      }
		      else if(nnResPDG&&tMC>nnResM+mNeut+mNeut)// Can radiate a DiNeutron (priority 7)
		      {
          barPDG=90000001;
          resPDG=nnResPDG;
          thdPDG=90000001;
          barM  =mNeut;
          resM  =nnResM;
		      }
		      else if(npResPDG&&tMC>npResM+mProt+mNeut)// Can radiate a neutron+proton (prior 8)
		      {
          barPDG=90001000;
          resPDG=npResPDG;
          thdPDG=90000001;
          barM  =mProt;
          resM  =npResM;
		      }
		      else if(lnResPDG&&tMC>lnResM+mLamb+mNeut)// Can radiate a Lambda+neutron (prior 9)
		      {
          barPDG=91000000; // @@ Sigma0
          resPDG=lnResPDG;
          thdPDG=90000001;
          barM  =mLamb;
          resM  =lnResM;
		      }
		      else if(lpResPDG&&tMC>lpResM+mLamb+mProt)// Can radiate a Lambda+proton (prior 10)
		      {
          barPDG=91000000; // @@ Sigma0
          resPDG=lpResPDG;
          thdPDG=90001000;
          barM  =mLamb;
          resM  =lpResM;
          thdM  =mProt;
		      }
		      else if(llResPDG&&tMC>llResM+mLamb+mLamb)// Can radiate a DiLambda (priority 11)
		      {
          barPDG=91000000; // @@ Sigma0
          resPDG=llResPDG;
          thdPDG=91000000; // @@ Sigma0
          barM  =mLamb;
          resM  =llResM;
          thdM  =mLamb;
		      }
        else if(thePDG!=90004004&&tMC>GSMass)// If it's not Be8 decay in gamma & GSM
		      {
          barPDG=thePDG;
          resPDG=22;
          barM  =GSMass;
          resM  =0.;
		      }
        else if(thePDG!=90004004)
		      {
          G4cerr<<"***G4QEnv::EvaR:PDG="<<thePDG<<",M="<<totMass<<"< GSM="<<GSMass<<G4endl;
          throw G4QException("G4QEnvironment::EvaporateResidual:M<GSM & can't decayInPNL");
		      }
        G4LorentzVector a4Mom(0.,0.,0.,barM);
        G4LorentzVector b4Mom(0.,0.,0.,resM);
        if(!thdPDG)
        {
          if(!qH->DecayIn2(a4Mom,b4Mom))
          {
            theQHadrons.push_back(qH);          // Fill as it is (delete equivalent)
            G4cerr<<"---Warning---G4QEnv::EvaRes:rP="<<pResPDG<<",rN="<<nResPDG<<",rL="
                  <<lResPDG<<",N="<<bN<<",Z="<<bZ<<",L="<<bS<<",totM="<<totMass<<",n="
                  <<totMass-nResM-mNeut<<",p="<<totMass-pResM-mProt<<",l="
                  <<totMass-lResM-mLamb<<G4endl;
            G4cerr<<"---Warning---G4QE::EvR:DecIn2Error b="<<barPDG<<",r="<<resPDG<<G4endl;
            return;
	         }
          else
          {
            delete qH;
            G4QHadron* HadrB = new G4QHadron(barPDG,a4Mom);
#ifdef pdebug
            G4cout<<"G4QE::EvaRes:(1) Baryon="<<barPDG<<a4Mom<<G4endl;
#endif
            theQHadrons.push_back(HadrB);       // Fill the baryon (delete equivalent)
            G4QHadron* HadrR = new G4QHadron(resPDG,b4Mom);
#ifdef pdebug
            G4cout<<"G4QE::EvaRes:(1) Residual="<<resPDG<<b4Mom<<G4endl;
#endif
            // @@ Self-call !!
            if(HadrR->GetBaryonNumber()>1) EvaporateResidual(HadrR);//Continue decay(del.e)
            else theQHadrons.push_back(HadrR);  // Fill ResidNucl=Baryon to OutHadronVector
          }
        }
        else
        {
          G4LorentzVector c4Mom(0.,0.,0.,thdM);
          if(!qH->DecayIn3(a4Mom,b4Mom,c4Mom))
          {
            theQHadrons.push_back(qH);          // Fill as it is (delete equivalent)
            G4cout<<"---Warning---G4QE::EvR:rNN="<<nnResPDG<<",rNP="<<npResPDG<<",rPP="
                  <<ppResPDG<<",N="<<bN<<",Z="<<bZ<<",L="<<bS<<",tM="<<totMass<<",nn="
                  <<totMass-nnResM-mNeut-mNeut<<",np="<<totMass-npResM-mProt-mNeut<<",pp="
                  <<totMass-ppResM-mProt-mProt<<G4endl;
            G4cerr<<"---Warning---G4QE::EvR:DecIn2Error,b="<<barPDG<<",r="<<resPDG<<G4endl;
            return;
	         }
          else
          {
            delete qH;
            G4QHadron* HadrB = new G4QHadron(barPDG,a4Mom);
#ifdef pdebug
            G4cout<<"G4QE::EvaRes:(2) Baryon1="<<barPDG<<a4Mom<<G4endl;
#endif
            theQHadrons.push_back(HadrB);       // Fill the first baryon (del.equiv.)
            G4QHadron* HadrC = new G4QHadron(thdPDG,c4Mom);
#ifdef pdebug
            G4cout<<"G4QE::EvaRes:(2) Baryon2="<<thdPDG<<c4Mom<<G4endl;
#endif
            theQHadrons.push_back(HadrC);       // Fill the second baryon (del.equiv.)
            G4QHadron* HadrR = new G4QHadron(resPDG,b4Mom);
#ifdef pdebug
            G4cout<<"G4QE::EvaRes:(2) Residual="<<resPDG<<b4Mom<<G4endl;
#endif
            // @@ Self-call !!
            if(HadrR->GetBaryonNumber()>1) EvaporateResidual(HadrR);//Continue decay(DelEq)
            else theQHadrons.push_back(HadrR); // Fill ResidNucl=Baryon to OutputHadrVector
          }
        }
	     }
      else if (fabs(totMass-GSMass)<.003) // @@ Looks like a duplication of the prev. check
      {
#ifdef pdebug
        G4cout<<"*|*|*|*G4QE::EvaR: fill AsIs. Should never be here"<<G4endl;
#endif
        theQHadrons.push_back(qH);     // FillAsItIs (del.eq.)
        return;
      }
      else                             // "System is below mass shell and can't decay" case
	     {
#ifdef pdebug
        G4cout<<"***G4QEnv::EvaRes: tM="<<totMass<<"("<<thePDG<<") < GSM="<<GSMass<<", d="
              <<totMass-GSMass<<", QC="<<qH->GetQC()<<qH->Get4Momentum()<<"*AsIs*"<<G4endl;
#endif
        //@@ Why this does not work? - Wait for the close message
        //G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
        //theEnvironment=G4QNucleus(90000000,G4LorentzVector(0.,0.,0.,0.));
		//if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH);//Correct or FillAsIs
        //else delete qH;  
        //delete quasH;
        // @@ Temporary
        theQHadrons.push_back(qH); // Correct or fill as it is
        return;
      }
    }
    //else if(bA==5) DecayAlphaBar(qH);// Decay alpha-nucleon state (delete equivalent)
    //else if(bZ==4&&bN==2&&!bS) DecayAlphaDiN(qH); // Decay alpha+2protons state (del.eq.)
    //else if(bZ==4&&bN==4&&!bS) DecayAlphaAlpha(qH); // Decay alpha+alpha state (del.eq.)
    else                                        // ===> Evaporation of excited system
	   {
#ifdef pdebug
      G4cout<<"G4QE::EvaRes:***EVA***tPDG="<<thePDG<<",M="<<totMass<<">GSM="<<GSMass<<",d="
            <<totMass-GSMass<<", N="<<qNuc.Get4Momentum()<<qNuc.Get4Momentum().m()<<G4endl;
#endif
      G4LorentzVector b4M;
      G4LorentzVector r4M;
      G4bool evC=true;                  // @@ It makes only one attempt to be possible
      G4int bPDG=0;
      G4int rPDG=0;
      G4double bM = 0.;                 // Prototype of Real Mass of the EvaporatedDibaryon
      G4double rM = 0.;                 // Prototype of Real Mass of the residual nucleus
      G4int bB=0;                       // Proto of Baryon Number of the evaporated baryon
      G4int rB=0;                       // Proto of Baryon Number of the residual nucleus
      G4QHadron* bHadron = new G4QHadron;// Proto of the evaporated baryon @@where deleted?
      G4QHadron* rHadron = new G4QHadron;// Proto of the residual nucleus @@where deleted?
      G4int evcn=0;
      //G4int evcm=27;
      G4int evcm=9;                     // Max numder of attempts to evaporate
      // @@ Does not look like it can make two attempts @@ Improve, Simplify @@
      while(evC&&evcn<evcm)
      {
        evC=true;
        evcn++;
        if(!qNuc.EvaporateBaryon(bHadron,rHadron)) // Evaporation did not succeed
	       {
#ifdef pdebug
          G4cout<<"***G4QEnv::EvaRes:***EVA Failed***PDG="<<thePDG<<",M="<<totMass<<G4endl;
#endif
          delete bHadron;
          delete rHadron;
          G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
#ifdef pdebug
          G4cout<<"***G4QE::EvaRes: Residual="<<qH->GetQC()<<qH->Get4Momentum()<<G4endl;
#endif
          theEnvironment=G4QNucleus(90000000,G4LorentzVector(0.,0.,0.,0.));
		        if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH); // Cor or fillAsItIs
          else delete qH;  
          delete quasH;
          return;
	       }
        evC=false;
        b4M=bHadron->Get4Momentum();
        r4M=rHadron->Get4Momentum();
        bM   = b4M.m();                       // Real mass of the evaporated dibaryon
        rM   = r4M.m();                       // Real mass of the residual nucleus
        bB=bHadron->GetBaryonNumber();        // Baryon number of the evaporated baryon
        rB=rHadron->GetBaryonNumber();        // Baryon number of the residual nucleus
        bPDG=bHadron->GetPDGCode();
        rPDG=rHadron->GetPDGCode();
#ifdef debug
        G4int bC=bHadron->GetCharge();        // Baryon number of the evaporated baryon
        //G4int rC=rHadron->GetCharge();       // Baryon number of the residual nucleus
        G4double bCB=qNuc.CoulombBarrier(bC,bB);
        //G4double rCB=qNuc.CoulombBarrier(rC,rB);
        G4cout<<"G4QEnv::EvaRes:Attempt #"<<evcn<<" > "<<evcm<<", rPDG="<<rPDG<<", bPDG="
              <<bPDG<<", bE="<<b4M.e()-b4M.m()<<" > bCB="<<bCB<<G4endl;
#endif
        //if(b4M.e()-b4M.m()<bCB&&evcn<evcm) evC=true;
	     }  // End of while
#ifdef debug
      G4cout<<"G4QEnv::EvaRes:*** EVA IS DONE *** F="<<bPDG<<b4M<<",bB="<<bB<<", ResNuc="
            <<rPDG<<r4M<<",rB="<<rB<<G4endl;
#endif
      delete qH;
      if(bB<2)theQHadrons.push_back(bHadron); // Fill EvaporatedBaryon (del.equivalent)
      else if(bB==2) DecayDibaryon(bHadron);  // => "Dibaryon" case needs decay
      else if(bB==4) theQHadrons.push_back(bHadron); // "Alpha radiation" case (del.eq.)
      else if(bB==5) DecayAlphaBar(bHadron);  // "Alpha+Baryon Decay" case (del.equiv.)
      else if(bPDG==90004002) DecayAlphaDiN(bHadron); // Decay alph+2p(alph+2n is stable)
      else if(bPDG==90004004) DecayAlphaAlpha(bHadron);// "Alph+Alph Decay" case(del.eq.)
      else
	     {
        delete bHadron;
        G4cerr<<"***G4QE::EvaRes:bB="<<bB<<">2 - unexpected evaporated fragment"<<G4endl;
        throw G4QException("G4QEnvironment::EvaporateResidual: Wrong evaporation act");
	     }
      if(rB>2) EvaporateResidual(rHadron);    // Continue evaporation (@@ Self-call)
      else if(rB==2)                   // => "Dibaryon" case needs decay @@ DecayDibaryon
	     {
        G4double rGSM = rHadron->GetQPDG().GetMass(); // Ground State mass of the dibaryon
#ifdef debug
		      G4cout<<"G4QE::EvaRes:ResidDibM="<<rM<<",GSM="<<rGSM<<",M-GSM="<<rM-rGSM<<G4endl;
#endif
        if(rM<=rGSM-0.01)
		      {
          delete rHadron;
          G4cerr<<"***G4QEnv::EvaRes: <residual> M="<<rM<<" < GSM="<<rGSM<<G4endl;
          throw G4QException("G4QEnvironment::EvaporateResidual:Evapor below MassShell");
		      }
        else if(fabs(rM-rGSM)<0.01&&rPDG==90001001) theQHadrons.push_back(rHadron); // (DE)
        else DecayDibaryon(rHadron);         // => "Dibaryon Decay" case (del.equivalent)
	     }
      else if(rB==5) DecayAlphaBar(rHadron); // "Alpha+Baryon Decay" case (del.equival.)
      else if(rPDG==90004002) DecayAlphaDiN(rHadron);//Decay alph+2p (alph+2n is stable)
      else if(rPDG==90004004) DecayAlphaAlpha(rHadron);// "Alpha+Alpha Decay" case(delEq)
      else theQHadrons.push_back(rHadron);   // Fill ResidNucl=Baryon to OutputHadronVector
      if(2>3)                                // @@ This can be skipped @@ ! @@
      {
        G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
        theEnvironment=G4QNucleus(90000000,G4LorentzVector(0.,0.,0.,0.));
		      if(!CheckGroundState(quasH,true))
        {
#ifdef pdebug
          G4cout<<"*G4QE::EvaR:EmerFill(1) "<<qH->GetQC()<<qH->Get4Momentum()<<G4endl;
#endif
          theQHadrons.push_back(qH); // Cor or fillAsItIs
        }
        else delete qH;  
        delete quasH;
        return;
      }
	   } // End of Evaporation of excited system
#ifdef sdebug
    G4cout<<"G4QEnv::EvaRes: === End of the evaporation attempt"<<G4endl;
#endif
  }
  else                                          // => "Decay if impossible evaporate" case
  {
#ifdef debug
    G4cout<<"G4QEnv::EvaRes: InputHadron4M="<<q4M<<", PDG="<<thePDG<<G4endl;
#endif
    if(thePDG)
    {
      if(thePDG==10)                            // "Chipolino decay" case 
	     {
        G4QContent totQC = qH->GetQC();         // Quark content of the hadron
        G4QChipolino resChip(totQC);            // define the Residual as a Chipolino
        G4QPDGCode h1=resChip.GetQPDG1();
        G4double m1  =h1.GetMass();             // Mass of the first hadron
        G4QPDGCode h2=resChip.GetQPDG2();
        G4double m2  =h2.GetMass();             // Mass of the second hadron
        if(totMass+.0001>m1+m2)
        {
          delete qH;                            // Chipolino should not be in a sequence
          G4LorentzVector fq4M(0.,0.,0.,m1);
          G4LorentzVector qe4M(0.,0.,0.,m2);
          if(!G4QHadron(q4M).DecayIn2(fq4M,qe4M))
		        {
            G4cerr<<"***G4QEnv::EvaRes:tM="<<totMass<<"-> h1M="<<m1<<" + h2M="<<m2<<G4endl;
		          throw G4QException("G4QEnvironment::EvaporateResid:Chip->h1+h2 DecIn2 error");
	         }
          G4QHadron* H2 = new G4QHadron(h2.GetPDGCode(),qe4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(2) h2="<<h2.GetPDGCode()<<qe4M<<G4endl;
#endif
          theQHadrons.push_back(H2);            // (delete equivalent)
          G4QHadron* H1 = new G4QHadron(h1.GetPDGCode(),fq4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(2) h1="<<h1.GetPDGCode()<<fq4M<<G4endl;
#endif
          theQHadrons.push_back(H1);            // (delete equivalent)
		      }
        else
	       {
          delete qH;
          G4cerr<<"**G4QE::ER:M="<<totMass<<"<"<<m1<<"+"<<m2<<",d="<<m1+m2-totMass<<G4endl;
          throw G4QException("G4QEnvironment::EvaporateResidual:ChipolinoUnder MassShell");
	       }
	     }
      else                                      // "Hadron" case
	     {
        G4double totM=G4QPDGCode(thePDG).GetMass();
        if(fabs(totMass-totM)<0.001||abs(thePDG)-10*(abs(thePDG)/10)>2)
								{
#ifdef pdebug
          G4cout<<"*G4QE::EvaR:EmerFill(2) "<<qH->GetQC()<<qH->Get4Momentum()<<G4endl;
#endif
          theQHadrons.push_back(qH);
        }
        else if ((thePDG==221||thePDG==331)&&totMass>mPi+mPi) // "Decay in pipi" case
	       {
          G4LorentzVector fq4M(0.,0.,0.,mPi);
          G4LorentzVector qe4M(0.,0.,0.,mPi);
          if(!G4QHadron(q4M).DecayIn2(fq4M,qe4M))
		        {
            G4cerr<<"***G4QEnv::EvaporateResidual: tM="<<totMass<<"-> pi+ + pi-"<<G4endl;
		          throw G4QException("G4QEnv::EvaporateResidual: H->Pi+Pi DecayIn2 error");
	         }
          delete qH;
          G4QHadron* H1 = new G4QHadron(211,fq4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(3) PiPlus="<<fq4M<<G4endl;
#endif
          theQHadrons.push_back(H1);            // (delete equivalent)
          G4QHadron* H2 = new G4QHadron(-211,qe4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(3) PiMinus="<<qe4M<<G4endl;
#endif
          theQHadrons.push_back(H2);            // (delete equivalent)
	       }
        else if ((thePDG==221||thePDG==331)&&totMass>mPi0+mPi0) // "Decay in 2pi0" case
	       {
          G4LorentzVector fq4M(0.,0.,0.,mPi0);
          G4LorentzVector qe4M(0.,0.,0.,mPi0);
          if(!G4QHadron(q4M).DecayIn2(fq4M,qe4M))
		        {
            G4cerr<<"***G4QEnv::EvaporateResidual: tM="<<totMass<<"-> pi0 + pi0"<<G4endl;
		          throw G4QException("G4QEnv::EvaporateResidual: H->Pi+Pi DecayIn2 error");
	         }
          delete qH;
          G4QHadron* H1 = new G4QHadron(111,fq4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(4) Pi01="<<fq4M<<G4endl;
#endif
          theQHadrons.push_back(H1);            // (delete equivalent)
          G4QHadron* H2 = new G4QHadron(111,qe4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(4) Pi02="<<qe4M<<G4endl;
#endif
          theQHadrons.push_back(H2);            // (delete equivalent)
	       }
        else if (totMass>totM)                  // "Radiative Hadron decay" case
	       {
          G4LorentzVector fq4M(0.,0.,0.,0.);
          G4LorentzVector qe4M(0.,0.,0.,totM);
          if(!G4QHadron(q4M).DecayIn2(fq4M,qe4M))
		        {
            G4cerr<<"***G4QEnv::EvaporateRes:tM="<<totMass<<"->h1M="<<totM<<"+gam"<<G4endl;
		          throw G4QException("G4QEnv::EvaporateResidual:H*->H+gamma DecIn2 error");
	         }
          delete qH;
          G4QHadron* H2 = new G4QHadron(thePDG,qe4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(5) tot="<<thePDG<<qe4M<<G4endl;
#endif
          theQHadrons.push_back(H2);            // (delete equivalent)
          G4QHadron* H1 = new G4QHadron(22,fq4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(5) GamFortot="<<fq4M<<G4endl;
#endif
          theQHadrons.push_back(H1);            // (delete equivalent)
	       }
        else if (thePDG==111||thePDG==221||thePDG==331) // "Gamma+Gamma decay" case
	       {
          G4LorentzVector fq4M(0.,0.,0.,0.);
          G4LorentzVector qe4M(0.,0.,0.,0.);
          if(!G4QHadron(q4M).DecayIn2(fq4M,qe4M))
		        {
            G4cerr<<"***G4QEnv::EvaporateResidual:tM="<<totMass<<"->gamma + gamma"<<G4endl;
		          throw G4QException("G4QEnv::EvaporateResidual:pi/eta->g+g DecIn2 error");
	         }
          delete qH;
          G4QHadron* H2 = new G4QHadron(22,qe4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(6) gam1="<<qe4M<<G4endl;
#endif
          theQHadrons.push_back(H2);            // (delete equivalent)
          G4QHadron* H1 = new G4QHadron(22,fq4M);
#ifdef pdebug
          G4cout<<"G4QE::EvaRes:(6) gam2="<<fq4M<<G4endl;
#endif
          theQHadrons.push_back(H1);            // (delete equivalent)
	       }
        else
	       {
          G4cerr<<"***G4QEnv::EvaRes: RNuc="<<thePDG<<theQC<<", q4M="<<q4M<<", M="<<totMass
                <<" < GSM="<<totM<<", 2Pi="<<mPi+mPi<<", 2Pi0="<<mPi0+mPi0<<G4endl;
          throw G4QException("G4QEnvironment::EvaporateResidual:Hadron's under MassShell");
	       }
	     }
	   }
    else
    {
      G4cerr<<"***G4QE::EvaRes: RN="<<thePDG<<theQC<<",q4M="<<q4M<<",qM="<<totMass<<G4endl;
      throw G4QException("G4QEnv::EvaporateResidual: This is not a nucleus nor a hadron");
    }
  }
#ifdef sdebug
  G4cout<<"G4QEnv::EvaRes:===>>>>End. "<<G4endl;
#endif
  return;                            //!! This a recursive function, so some compilers...!!
} // End of EvaporateResidual

//The public Hadronisation function with the Exception treatment (del respons. of User !)
G4QHadronVector* G4QEnvironment::Fragment()
{//              ==========================
#ifdef chdebug
  G4int fCharge=theEnvironment.GetCharge();
  G4int fBaryoN=theEnvironment.GetA();
  G4int nHad=theQHadrons.size();
  if(nHad) for(G4int ih=0; ih<nHad; ih++)
  {
    fCharge+=theQHadrons[ih]->GetCharge();
    fBaryoN+=theQHadrons[ih]->GetBaryonNumber();
  }
  G4int nQuas=theQuasmons.size();
  if(nQuas)for(G4int iqs=0; iqs<nQuas; iqs++)
  {
    fCharge+=theQuasmons[iqs]->GetCharge();
    fBaryoN+=theQuasmons[iqs]->GetBaryonNumber();
  }
  if(fCharge!=totCharge || fBaryoN!=totBaryoN)
  {
    G4cerr<<"***G4QE::Frag:tC="<<totCharge<<",C="<<fCharge<<",tB="<<totBaryoN
          <<",B="<<fBaryoN<<",E="<<theEnvironment<<G4endl;
    if(nHad) for(G4int h=0; h<nHad; h++)
    {
      G4QHadron* cH = theQHadrons[h];
      G4cerr<<">G4QE::HQ:h#"<<h<<",QC="<<cH->GetQC()<<",PDG="<<cH->GetPDGCode()<<G4endl;
    }
    if(nQuas) for(G4int q=0; q<nQuas; q++)
    {
      G4Quasmon* cQ = theQuasmons[q];
      G4cerr<<">G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QCont="<<cQ->GetQC()<<G4endl;
    }
  }
#endif
  G4QHadronVector dummy;       // Prototype of the output G4QHadronVector to avoid warnings
  G4QHadronVector* theFragments = &dummy; // Prototype of the output G4QHadronVector
  G4int ExCount =0;                       // Counter of the repetitions
  G4int MaxExCnt=1;                       // A#of of repetitions + 1 (1 for no repetitions)
  G4bool RepFlag=true;                    // To come inside the while
  // For the purpose of the recalculation the Quasmons, Hadrons, Environment must be stored
  G4QuasmonVector* reQuasmons = new G4QuasmonVector; // deleted after the "while LOOP"
  G4int nQ = theQuasmons.size();
  if(nQ)
  {
    for(G4int iq=0; iq<nQ; iq++)
    {
      G4Quasmon* curQ     = new G4Quasmon(theQuasmons[iq]);
      reQuasmons->push_back(curQ);                  // deleted after the "while LOOP"
    }
  }
  G4QHadronVector* reQHadrons = new G4QHadronVector; // deleted after the "while LOOP"
  G4int nH = theQHadrons.size();
  if(nH)
  {
    for(G4int ih=0; ih<nH; ih++)
    {
      G4QHadron* curH     = new G4QHadron(theQHadrons[ih]);
      reQHadrons->push_back(curH);                 // deleted after the "while LOOP"
    }
  }
  G4QNucleus reEnvironment=theEnvironment;
  G4LorentzVector rem4M=tot4Mom;
  while (RepFlag && ExCount<MaxExCnt)
  {
    try
    {
      RepFlag=false;                      // If OK - go out of the while
      theFragments = FSInteraction();     //InterClass creation. User must delet QHadrons.
    }
    catch (G4QException& error)
    {
      G4cerr<<"***G4QEnvironment::Fragment: Exception is catched"<<G4endl;
      RepFlag=true;                       // For the Exception - repete
      ExCount++;                          // Increment the repetition counter
      G4cerr<<"***G4QEnv::Fragment:Exception #"<<ExCount<<": "<<error.GetMessage()<<G4endl;
      G4LorentzVector dif=rem4M-theEnvironment.Get4Momentum(); // CHECK difference
      G4int nHp=theQHadrons.size();
      G4int nQp = theQuasmons.size();
      G4cerr<<"***G4QEnvir::Fragment:nH="<<nHp<<",nQ="<<nQp<<",E="<<theEnvironment<<G4endl;
      for(G4int ph=0; ph<nHp; ph++)
      {
        G4QHadron* cH = theQHadrons[ph];
        dif-=cH->Get4Momentum();
		      G4cerr<<"***G4QEnvir::Fr:H"<<ph<<"="<<cH->Get4Momentum()<<cH->GetPDGCode()<<G4endl;
      }
      for(G4int pq=0; pq<nQp; pq++)
      {
        G4Quasmon* cQ = theQuasmons[pq];
        dif-=cQ->Get4Momentum();
        G4cerr<<"***G4QEnvir::Fr:Quasm#"<<pq<<"="<<cQ->Get4Momentum()<<cQ->GetQC()<<G4endl;
      }
      // *** Cleaning Up of all old output instances for the recalculation purposes ***
      for_each(theFragments->begin(), theFragments->end(), DeleteQHadron()); // old Hadrons
      theFragments->clear();
      for_each(theQHadrons.begin(), theQHadrons.end(), DeleteQHadron()); //internal Hadrons
      theQHadrons.clear();
      for_each(theQuasmons.begin(), theQuasmons.end(), DeleteQuasmon()); // old Quasmons
      theQuasmons.clear();
      G4cerr<<"***G4QEnv::Fragment: ----------- End of CleaningUp: 4Mdif="<<dif<<G4endl;
      // **************** Recover all conditions for the recalculation ********************
      theEnvironment=reEnvironment;             // Recover the nuclear environment
      tot4Mom=rem4M;                            // Recover the total 4Momentum of the React
      G4cerr<<"***G4QEnv::Fragment:*Recover*Env="<<theEnvironment<<",4M="<<tot4Mom<<G4endl;
      G4int mQ = reQuasmons->size();            // Recover the memorizedQuasmons with print
      for(G4int jq=0; jq<mQ; jq++)
      {
        //G4Quasmon* curQ = new G4Quasmon(reQuasmons->operator[](jq));
        G4Quasmon* curQ = new G4Quasmon((*reQuasmons)[jq]);
        G4cerr<<"***G4QE::Fragm:Q("<<jq<<")="<<curQ->Get4Momentum()<<curQ->GetQC()<<G4endl;
        theQuasmons.push_back(curQ);            // (delete equivalent)
      }
      G4int mH = reQHadrons->size();            // Recover the memorizedQHadrons with print
      for(G4int jh=0; jh<mH; jh++)
      {
        //G4QHadron* curH = new G4QHadron(reQHadrons->operator[](jh));
        G4QHadron* curH = new G4QHadron((*reQHadrons)[jh]);
        G4cerr<<"***G4QE::Fragm:H("<<jh<<")="<<curH->Get4Momentum()<<curH->GetQC()<<G4endl;
        theQHadrons.push_back(curH);            // (delete equivalent)
      }
    }
  }
  if(reQuasmons->size()) // If something is still in memory then clean it up
  {
    for_each(reQuasmons->begin(),reQuasmons->end(),DeleteQuasmon()); // CleanUp oldQuasmons
    reQuasmons->clear();
  }
  delete reQuasmons;     // All temporary Quasmons memory is wiped out
  if(reQHadrons->size()) // If something is still in memory then clean it up
  {
    for_each(reQHadrons->begin(),reQHadrons->end(),DeleteQHadron()); //CleanUp old QHadrons
    reQHadrons->clear();
  }
  delete reQHadrons;     // All temporary QHadrons memory is wiped out
  if(ExCount>=MaxExCnt)
  {
    G4cerr<<"*G4QEnv::Fragment:Exception.Target="<<theTargetPDG<<". Projectiles:"<<G4endl;
    G4int nProj=theProjectiles.size();
    if(nProj) for(G4int ipr=0; ipr<nProj; ipr++)
	   {
      G4QHadron* prH = theProjectiles[ipr];
      G4cerr<<"G4QE::F:#"<<ipr<<",PDG/4M="<<prH->GetPDGCode()<<prH->Get4Momentum()<<G4endl;
    }
	   throw G4QException("G4QEnvironment::Fragment:This reaction caused the CHIPSException");
    //G4Exception("G4QEnvironment::Fragment","027",FatalException,"GeneralCHIPSException");
  }
  // Put the postponed hadrons in the begining of theFragments and clean them up
  G4int tmpS=intQHadrons.size();
  if(tmpS)
  {
    tmpS=theFragments->size();
    intQHadrons.resize(tmpS+intQHadrons.size()); // Resize intQHadrons
    copy(theFragments->begin(), theFragments->end(), intQHadrons.end()-tmpS);
    tmpS=intQHadrons.size();
    theFragments->resize(tmpS); // Resize theFragments
    copy(intQHadrons.begin(), intQHadrons.end(), theFragments->begin());
    intQHadrons.clear();
  }
  return theFragments;
} // End of the Fragmentation member function

//The Final State Interaction Filter for the resulting output of ::HadronizeQEnvironment()
G4QHadronVector* G4QEnvironment::FSInteraction()
{//              ===============================
  static const G4QPDGCode gQPDG(22);
  static const G4QPDGCode pizQPDG(111);
  static const G4QPDGCode pipQPDG(211);
  static const G4QPDGCode pimQPDG(-211);
  static const G4QPDGCode nQPDG(2112);
  static const G4QPDGCode pQPDG(2212);
  static const G4QPDGCode lQPDG(3122);
  static const G4QPDGCode s0QPDG(3212);
  //static const G4QPDGCode dQPDG(90001001);
  static const G4QPDGCode tQPDG(90001002);
  static const G4QPDGCode he3QPDG(90002001);
  static const G4QPDGCode aQPDG(90002002);
  static const G4QPDGCode a6QPDG(90002004);
  static const G4QPDGCode be6QPDG(90004002);
  //static const G4QPDGCode b7QPDG(90005002);
  //static const G4QPDGCode he7QPDG(90002005);
  static const G4QPDGCode c8QPDG(90006002);
  static const G4QPDGCode a8QPDG(90002006);
  static const G4QPDGCode c10QPDG(90006004);
  static const G4QPDGCode o14QPDG(90008006);
  static const G4QPDGCode o15QPDG(90008007);
  static const G4QContent K0QC(1,0,0,0,0,1);
  static const G4QContent KpQC(0,1,0,0,0,1);
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mPi0 = G4QPDGCode(111).GetMass();
  static const G4double mK   = G4QPDGCode(321).GetMass();
  static const G4double mK0  = G4QPDGCode(311).GetMass();
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mSigZ= G4QPDGCode(3212).GetMass();
  static const G4double mSigM= G4QPDGCode(3112).GetMass();
  static const G4double mSigP= G4QPDGCode(3222).GetMass();
#ifdef pdebug
  static const G4double mDeut= G4QPDGCode(2112).GetNuclMass(1,1,0);
#endif
  static const G4double mTrit= G4QPDGCode(2112).GetNuclMass(1,2,0);
  static const G4double mHe3 = G4QPDGCode(2112).GetNuclMass(2,1,0);
  static const G4double mAlph= G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double mHe6 = G4QPDGCode(2112).GetNuclMass(2,4,0);
  static const G4double mBe6 = G4QPDGCode(2112).GetNuclMass(4,2,0);
  //static const G4double mHe7 = G4QPDGCode(2112).GetNuclMass(2,5,0);
  //static const G4double mB7  = G4QPDGCode(2112).GetNuclMass(5,2,0);
  static const G4double mHe8 = G4QPDGCode(2112).GetNuclMass(2,6,0);
  static const G4double mC8  = G4QPDGCode(2112).GetNuclMass(6,2,0);
  static const G4double mC10 = G4QPDGCode(2112).GetNuclMass(6,4,0);
  static const G4double mO14 = G4QPDGCode(2112).GetNuclMass(8,6,0);
  static const G4double mO15 = G4QPDGCode(2112).GetNuclMass(8,7,0);
  static const G4double mKmP = mK+mProt;
  static const G4double mKmN = mK+mNeut;
  static const G4double mK0mP = mK0+mProt;
  static const G4double mK0mN = mK0+mNeut;
  static const G4QNucleus vacuum(90000000);
  static const G4double eps=0.005;
  ///////////////static const G4double third=1./3.;
  ///////////////static const G4double nPDG=90000001;
  G4int envA=theEnvironment.GetBaryonNumber();
  ///////////////G4int envC=theEnvironment.GetCharge();
#ifdef rdebug
  G4int totInC=theEnvironment.GetZ();
  G4LorentzVector totIn4M=theEnvironment.Get4Momentum();
  G4cout<<"G4QEnvironment(G4QE)::FSInter(FSI): ***called*** envA="<<envA<<totIn4M<<G4endl;
  G4int nQuasmons=theQuasmons.size();
  for (G4int is=0; is<nQuasmons; is++) // Sum 4mom's of Quasmons for comparison
  {
	   G4Quasmon*      pQ = theQuasmons[is];
    G4LorentzVector Q4M= pQ->Get4Momentum();
    G4cout<<"G4QE::FSI: Quasmon ("<<is<<") is added, 4M="<<Q4M<<G4endl;
    totIn4M           += Q4M;
    totInC            += pQ->GetQC().GetCharge();
  } // End of TotInitial4Momentum summation LOOP over Quasmons
  G4int nsHadr  = theQHadrons.size();        // Update the value of OUTPUT entries
  if(nsHadr) for(G4int jso=0; jso<nsHadr; jso++)// LOOP over output hadrons 
  {
    G4int hsNF  = theQHadrons[jso]->GetNFragments(); // A#of secondary fragments
    if(!hsNF)                                        // Add only final hadrons
    {
      G4LorentzVector hs4Mom = theQHadrons[jso]->Get4Momentum();
      G4cout<<"G4QE::FSI: Hadron ("<<jso<<") is added, 4M="<<hs4Mom<<G4endl;
      totIn4M          += hs4Mom;
      totInC           += theQHadrons[jso]->GetCharge();
    }
  }
  G4cout<<"G4QE::FSI: The resulting 4Momentum="<<totIn4M<<G4endl;
#endif
#ifdef chdebug
  G4int fCharge=theEnvironment.GetCharge();
  G4int fBaryoN=theEnvironment.GetA();
  G4int nHad=theQHadrons.size();
  if(nHad) for(G4int ih=0; ih<nHad; ih++)
  {
    fCharge+=theQHadrons[ih]->GetCharge();
    fBaryoN+=theQHadrons[ih]->GetBaryonNumber();
  }
  G4int nQuas=theQuasmons.size();
  if(nQuas)for(G4int iqs=0; iqs<nQuas; iqs++)
  {
    fCharge+=theQuasmons[iqs]->GetCharge();
    fBaryoN+=theQuasmons[iqs]->GetBaryonNumber();
  }
  if(fCharge!=totCharge || fBaryoN!=totBaryoN)
  {
    G4cerr<<"***G4QE::FSI:tC="<<totCharge<<",C="<<fCharge<<",tB="<<totBaryoN
          <<",B="<<fBaryoN<<",E="<<theEnvironment<<G4endl;
    if(nHad) for(G4int h=0; h<nHad; h++)
    {
      G4QHadron* cH = theQHadrons[h];
      G4cerr<<":G4QE::HQ:h#"<<h<<",QC="<<cH->GetQC()<<",PDG="<<cH->GetPDGCode()<<G4endl;
    }
    if(nQuas) for(G4int q=0; q<nQuas; q++)
    {
      G4Quasmon* cQ = theQuasmons[q];
      G4cerr<<":G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QCont="<<cQ->GetQC()<<G4endl;
    }
  }
#endif
  G4QHadronVector* theFragments = new G4QHadronVector;//Internal creation. User must delete
  HadronizeQEnvironment();                // >>>>>>>>> Call the main fragmentation function
#ifdef rdebug
  G4int tC=totInC-theEnvironment.GetZ();  // Subtract theResidualEnvironCharge 
  G4LorentzVector t4M=totIn4M;            // Compare with the total                 
  G4cout<<"G4QEnv::FSI: Initial tot4M="<<t4M<<" to be subtracted"<<G4endl;
  G4LorentzVector theEnv4m=theEnvironment.Get4Momentum(); // Environment 4Mom 
  t4M-=theEnv4m;                          // Subtract the Environment 4-momentum    
  G4cout<<"G4QEnv::FSI: Subtract Environ="<<theEnv4m<<theEnvironment<<G4endl;
  for (G4int js=0; js<nQuasmons; js++)    // Subtract 4mom's of Quasmons (compare)
  {                                       //                                        
	   G4Quasmon*      prQ = theQuasmons[js];//                                  
    if(prQ->GetStatus())                  // Subtract only if Quasmon is alive      
    {                                     //                                        
      G4LorentzVector Q4M= prQ->Get4Momentum(); // 4-momentum of the Quasmon
      G4QContent qQC= prQ->GetQC();       //                                        
      G4cout<<"G4QE::FSI: Subtract Quasmon("<<js<<"),4M="<<Q4M<<",QC="<<qQC<<G4endl;
      t4M          -= Q4M;                // Subtract 4-momentum of the Quasmon        
      tC           -= prQ->GetQC().GetCharge(); //                            
	   }                                     //                                        
    else G4cout<<"G4QE::FSI:Dead Quasmon("<<js<<")="<<prQ->GetStatus()<<G4endl;
  } // End of Quasmons4Momentum subtractions                                  
  G4int nsbHadr=theQHadrons.size();       // Update the value of OUTPUT entries     
  if(nsbHadr) for(G4int jpo=0; jpo<nsbHadr; jpo++)// LOOP over output hadrons 
  {
    G4int hsNF  = theQHadrons[jpo]->GetNFragments();// A#of out fragments     
    if(!hsNF)                            // Subtract only final hadrons            
    {                                    //                                        
      G4LorentzVector hs4Mom = theQHadrons[jpo]->Get4Momentum(); // Output hadron
      G4int hPDG = theQHadrons[jpo]->GetPDGCode(); // PDG of the Output Hadron
      G4cout<<"G4QE::FSI: Subtract Hadron("<<jpo<<"), 4M="<<hs4Mom<<hPDG<<G4endl; 
      t4M          -= hs4Mom;           //                                        
      tC           -= theQHadrons[jpo]->GetCharge(); // Subtract charge of the OutHadron
	   }                                   // End of the "FinalHadron" IF            
  }                                     // End of the LOOP over output hadrons    
  G4cout<<"G4QEnv::FSI:|||||4-MomCHECK||||d4M="<<t4M<<",dCharge="<<tC<<G4endl;
#endif
  unsigned nHadr=theQHadrons.size();
  if(nHadr<=0)
  {
    G4cerr<<"---Warning---G4QEnvironment::FSInteraction: nHadrons="<<nHadr<<G4endl;
   	//throw G4QException("G4QEnvironment::FSInteraction: No hadrons in the output");
    return theFragments;
  }
  G4int lHadr=theQHadrons[nHadr-1]->GetBaryonNumber();
#ifdef pdebug
  G4cout<<"G4QE::FSI:after HQE,nH="<<nHadr<<",lHBN="<<lHadr<<",E="<<theEnvironment<<G4endl;
#endif
  if(lHadr>1)                          // TheLastHadron is nucleus:try to decay/evap/cor it
  {
    G4QHadron* theLast = theQHadrons[nHadr-1];
    G4QHadron* curHadr = new G4QHadron(theLast);
    G4LorentzVector lh4M=curHadr->Get4Momentum(); // Actual mass of the last fragment
    G4double lhM=lh4M.m();             // Actual mass of the last fragment
    G4int lhPDG=curHadr->GetPDGCode();            // PDG code of the last fragment
    G4double lhGSM=G4QPDGCode(lhPDG).GetMass();   // GroundStateMass of the last fragment
#ifdef pdebug
    G4cout<<"G4QE::FSI:lastHadr 4M/M="<<lh4M<<lhM<<",GSM="<<lhGSM<<",PDG="<<lhPDG<<G4endl;
#endif
    if(lhM>lhGSM+eps)                  // ==> Try to evaporate the residual nucleus
    {
      theQHadrons.pop_back();          // the last QHadron-Nucleus is excluded from OUTPUT
      delete theLast;//*!!When kill,DON'T forget to delete theLastQHadron as an instance!*
      EvaporateResidual(curHadr);      // Try to evaporate Hadr-Nucl (@@DecDib)(delete eq.)
      nHadr=theQHadrons.size();
#ifdef pdebug
      G4cout<<"G4QE::FSI:After nH="<<nHadr<<",PDG="<<curHadr->GetPDGCode()<<G4endl;
#endif
    }
    else if(lhM<lhGSM-eps)             // ==> Try to make the HadronicSteck FSI correction
    {
      theQHadrons.pop_back();          //exclude LastHadronPointer from OUTPUT
      delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
      G4Quasmon* quasH = new G4Quasmon(curHadr->GetQC(),lh4M); // Fake Quasmon ctreation
      if(!CheckGroundState(quasH,true))// Try to correct with other hadrons
      {
        G4cerr<<"---Warning---G4QEnv::FSI:Correction error LeaveAsItIs h4m="<<lh4M<<G4endl;
        theQHadrons.push_back(curHadr);// Fill theResidualNucleus asItIs(delete equivalent)
      }
      else
      {
        delete curHadr;              // The intermediate curHadr isn't necessary any more
        nHadr=theQHadrons.size();      // Update nHadr after successful correction
      }
      delete quasH;                    // Delete the temporary fake Quasmon
    }
    else delete curHadr;               // ==> Leave the nucleus as it is (close to the GSM)
  }
#ifdef pdebug
  G4LorentzVector ccs4M(0.,0.,0.,0.);  // CurrentControlSum of outgoing Hadrons
#endif
  // *** Initial Charge Control Sum Calculation ***
  G4int chContSum=0;                   // ChargeControlSum to keepTrack FSI transformations
  G4int bnContSum=0;                   // BaryoNControlSum to keepTrack FSI transformations
  if(nHadr)for(unsigned ich=0; ich<nHadr; ich++) if(!(theQHadrons[ich]->GetNFragments()))
  {
    chContSum+=theQHadrons[ich]->GetCharge();
    bnContSum+=theQHadrons[ich]->GetBaryonNumber();
  }
#ifdef chdebug
  if(chContSum!=totCharge || bnContSum!=totBaryoN)
  {
    G4cerr<<"***G4QE::Fr:(1)tC="<<totCharge<<",C="<<fCharge<<",tB="<<totBaryoN
          <<",B="<<bnContSum<<",E="<<theEnvironment<<G4endl;
    if(nHadr) for(unsigned h=0; h<nHadr; h++)
    {
      G4QHadron* cH = theQHadrons[h];
      G4cerr<<":G4QE::HQ:h#"<<h<<",QC="<<cH->GetQC()<<",PDG="<<cH->GetPDGCode()<<G4endl;
    }
    if(nQuas) for(G4int q=0; q<nQuas; q++)
    {
      G4Quasmon* cQ = theQuasmons[q];
      G4cerr<<":G4QE::HQ:q#"<<q<<",C="<<cQ->GetCharge()<<",QCont="<<cQ->GetQC()<<G4endl;
    }
  }
#endif
  // ***
  if(nHadr)for(unsigned ipo=0; ipo<theQHadrons.size(); ipo++)//FindBigestNuclFragm & DecayA
  {
    unsigned jpo=ipo;
    nHadr=theQHadrons.size();
    lHadr=theQHadrons[nHadr-1]->GetBaryonNumber();
    G4QHadron* theCurr = theQHadrons[ipo];    // Pointer to the Current Hadron
    G4int hBN  = theCurr->GetBaryonNumber();
    G4int sBN  = theCurr->GetStrangeness();
    G4int cBN  = theCurr->GetCharge();
    G4int hPDG = theCurr->GetPDGCode();
    G4LorentzVector h4Mom = theCurr->Get4Momentum();
#ifdef pdebug
    G4int hNF  = theCurr->GetNFragments();
    G4cout<<"G4QE::FSI:h#"<<ipo<<",PDG="<<hPDG<<h4Mom<<",mGS="<<G4QPDGCode(hPDG).GetMass()
          <<",F="<<hNF<<",nH="<<theQHadrons.size()<<G4endl;
#endif
    if(hBN>lHadr) // Current Hadron is the Biggest fragment -> Swap with theLast Hadron
	   {
      G4QHadron* theLast = theCurr;          // Prototype of the pointer to the Last Hadron
      G4QHadron* curHadr = new G4QHadron(theCurr);// Remember CurrentHadron for evaporation
      if(ipo+1<theQHadrons.size())          // If ipo<Last, swap theCurHadr and theLastHadr
      {
        theLast = theQHadrons[theQHadrons.size()-1];// Pointer to theLastHadron (ipo<Last)
        G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
        if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of LastHadr
        else theCurr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
        theCurr->Set4Momentum(theLast->Get4Momentum()); // ... continue substitution
      }
      theQHadrons.pop_back();           // pointer to theLastHadron is excluded from OUTPUT
      delete theLast; //*!!When kill,DON'T forget to delete theLastQHadron asAnInstance !!*
      theQHadrons.push_back(curHadr);
      nHadr=theQHadrons.size();
      h4Mom = theCurr->Get4Momentum();
      hBN  = theCurr->GetBaryonNumber();
      cBN  = theCurr->GetCharge();
      sBN  = theCurr->GetStrangeness();
      hPDG = theCurr->GetPDGCode();
	   }
    if(hPDG==89002000||hPDG==89001001||hPDG==89000002)// 2pt dec. of anti-strange (3pt dec)
    {
#ifdef pdebug
      G4cout<<"G4QE::FSI:***ANTISTRANGE*** i="<<ipo<<",PDG="<<hPDG<<",BaryN="<<hBN<<G4endl;
#endif
      G4double hM=h4Mom.m(); // 89002000
      G4double hMi=hM+eps;
      G4QPDGCode fQPDG = pQPDG;
      G4double fM = mProt;
      G4int  sPDG = 321;
      G4double sM = mK;
      G4int  tPDG = 0;
      G4double tM = 0.;
      if(hPDG==89002000)                     // Use the prototypes above
	     {
        if(hMi<mKmP)
		      {
          if(hMi>mProt+mPi+mPi0)
		        {
            sPDG=211;
            sM  =mPi;
            tPDG=111;
            tM  =mPi0;
          }
          else if(hMi>mProt+mPi) // @@ Does not conserve strangeness (Week decay)
		        {
#ifdef pdebug
            G4cerr<<"**G4QE::FSI:ANTISTRANGE*++*STRANGENESS,PDG="<<hPDG<<",M="<<hM<<G4endl;
#endif
            sPDG=211;
            sM  =mPi;
          }
          else sPDG=0;
        }
      }
	     else if(hPDG==89001001)
	     {
        fQPDG= nQPDG;
        fM   = mNeut;
        sPDG = 321;
        sM   = mK;
        if(hMi>mK0mP&&G4UniformRand()>.5)
        {
          fQPDG= pQPDG;
          fM   = mProt;
          sPDG = 311;
          sM   = mK0;
        }
		      else if(hMi<mKmN)
		      {
          if(hMi>mProt+mPi0+mPi0)
		        {
            fQPDG= pQPDG;
            fM   = mProt;
            sPDG = 111;
            sM   = mPi0;
            tPDG = 111;
            tM   = mPi0;
            if(hMi>mNeut+mPi+mPi0&&G4UniformRand()>.67)
		          {
              fQPDG= nQPDG;
              fM   = mNeut;
              tPDG = 211;
              tM   = mPi;
            }
            if(hMi>mProt+mPi+mPi&&G4UniformRand()>.5)
		          {
              sPDG = 211;
              sM   = mPi;
              tPDG =-211;
              tM   = mPi;
            }
          }
          else if(hMi>mProt+mPi0) // @@ Does not conserve strangeness (Week decay)
		        {
#ifdef pdebug
            G4cerr<<"**G4QE::FSI:*ANTISTRANGE*+*STRANGENESS*PDG="<<hPDG<<",M="<<hM<<G4endl;
#endif
            fQPDG= pQPDG;
            fM   = mProt;
            sPDG = 111;
            sM   = mPi0;
          }
          else sPDG=0;      // @@ Still can try to decay in gamma+neutron (electromagnetic)
        }
      }
	     else if(hPDG==89000002)
	     {
        fQPDG= nQPDG;
        fM   = mNeut;
        sPDG = 311;
        sM   = mK0;
        if(hMi<mK0mN)
		      {
          if(hMi>mNeut+mPi+mPi)
		        {
            sPDG = 211;
            sM   = mPi;
            tPDG =-211;
            tM   = mPi;
          }
          if(hMi>mProt+mPi+mPi0)
		        {
            fQPDG= pQPDG;
            fM   = mProt;
            sPDG = 111;
            sM   = mPi0;
            tPDG =-211;
            tM   = mPi;
          }
          else if(hMi>mProt+mPi) // @@ Does not conserve strangeness (Week decay)
		        {
#ifdef pdebug
            G4cerr<<"**G4QE::FSI:**ANTISTRANGE*0*STRANGENE**PDG="<<hPDG<<",M="<<hM<<G4endl;
#endif
            fQPDG= pQPDG;
            fM   = mProt;
            sPDG =-211;
            sM   = mPi;
          }
          else sPDG=0;      // @@ Still can try to decay in gamma+neutron (electromagnetic)
        }
	     }
      if(!sPDG)
	     {
#ifdef pdebug
        G4cerr<<"***G4QE::FSI:***ANTISTRANGE***CANN'T DECAY,PDG="<<hPDG<<",M="<<hM<<G4endl;
#endif
      }
      else if(!tPDG)           // 2 particle decay
      {
        G4bool fOK=true;
        G4LorentzVector f4M(0.,0.,0.,fM);
        G4LorentzVector s4M(0.,0.,0.,sM);
        G4double sum=fM+sM;
        if(fabs(hM-sum)<=eps)
		      {
          f4M=h4Mom*(fM/sum);
          s4M=h4Mom*(sM/sum);
		      }
        else if(hM<sum || !G4QHadron(h4Mom).DecayIn2(f4M,s4M))
	       {
          G4cerr<<"---Warning---G4QE::FSI: Still try(2),M="<<hM<<"->"<<fM<<"("<<fQPDG<<")+"
                <<sM<<"("<<sPDG<<")="<<sum<<G4endl;
          // Tipical scenario of recovery:
          //             1. Check that the Environment is vacuum (must be), 
          //if(theEnvironment==vacuum)
          if(!theEnvironment.GetA())
          {
            //           2. Extract and put in qH, substitute by the Last and make quasH,
            G4QHadron* theLast = theCurr;       // Prototype of thePointer to theLastHadron
            G4QHadron* qH = new G4QHadron(theCurr); // Copy of the Current Hadron
            if(ipo+1<theQHadrons.size())            // If ipo<Last, swap CurHadr & LastHadr
            {
              theLast = theQHadrons[theQHadrons.size()-1];//PointerTo theLastHadr(ipo<Last)
              G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
              if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of LastHadr
              else theCurr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
              theCurr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
            }                                       //ELSE: it's already theLast -> no swap
            theQHadrons.pop_back();                 //exclude LastHadronPointer from OUTPUT
            delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
            G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
            //           3. Try to use other hadrons to recover this one (under Mass Shell)
            if(!CheckGroundState(quasH,true))       // Try to correct with other hadrons
            {
              G4cerr<<"---Warning---G4QEnv::FSI:Failed (2) LeaveAsItIs 4m="<<h4Mom<<G4endl;
              theQHadrons.push_back(qH);            // Fill as it is (delete equivalent)
            }
            else
            {
              delete qH;
              //         4. Decrement jpo index (copy of ipo) to find theBiggestNuclearFrag
              jpo--;
              //         5. Recheck the probuct, which replaced the Last and check others
              nHadr=theQHadrons.size();
            }
            //           6. Delete the intermediate quasmon  
            delete quasH;
            //           7. Forbid the decay
            fOK=false;
		        }
          else
          {
            G4cerr<<"***G4QEnv::FSI: No recovery (1) Env="<<theEnvironment<<G4endl;
	           throw G4QException("G4QEnv::FSI:ANTISTRANGE DecayIn2 did not succeed");
          }
	       }
        if(fOK)
        {
          theQHadrons[ipo]->SetQPDG(fQPDG);
          theQHadrons[ipo]->Set4Momentum(f4M);
          G4QHadron* sH = new G4QHadron(sPDG,s4M);
          theQHadrons.push_back(sH);               // (delete equivalent)
        }
	     }
      else
      {
        G4bool fOK=true;
        G4LorentzVector f4M(0.,0.,0.,fM);
        G4LorentzVector s4M(0.,0.,0.,sM);
        G4LorentzVector t4M(0.,0.,0.,tM);
        G4double sum=fM+sM+tM;
        if(fabs(hM-sum)<=eps)
		      {
          f4M=h4Mom*(fM/sum);
          s4M=h4Mom*(sM/sum);
          t4M=h4Mom*(tM/sum);
		      }
        else if(hM<sum || !G4QHadron(h4Mom).DecayIn3(f4M,s4M,t4M))
	       {
          G4cerr<<"---Warning---G4QE::FSI: Still try(3), M"<<hM<<"->"<<fM<<"("<<fQPDG<<")+"
                <<sM<<"("<<sPDG<<")+"<<tM<<"("<<tPDG<<")="<<sum<<G4endl;
          //if(theEnvironment==vacuum)
          if(!theEnvironment.GetA())
          {
            G4QHadron* theLast = theCurr;    // Prototype of the pointer to the Last Hadron
            G4QHadron* qH = new G4QHadron(theCurr); // Copy of the Current Hadron
            if(ipo+1<theQHadrons.size())       // If ipo<Last, swap CurHadr and theLastHadr
            {
              theLast = theQHadrons[theQHadrons.size()-1];//Pointer to LastHadron(ipo<Last)
              G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
              if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of LastHadr
              else theCurr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
              theCurr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
            }
            theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
            delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
            G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
            if(!CheckGroundState(quasH,true))         // Try to correct by other hadrons
            {
              G4cerr<<"---Warning---G4QEnv::FSI:Failed (3) LeaveAsItIs,4M="<<h4Mom<<G4endl;
              theQHadrons.push_back(qH);              // Fill as it is (delete equivalent)
            }
            else
            {
              delete qH;
              jpo--;
              nHadr=theQHadrons.size();
            }
            delete quasH;
            fOK=false;
		        }
          else
          {
            G4cerr<<"***G4QEnv::FSI: No recovery (2) Env="<<theEnvironment<<G4endl;
	           throw G4QException("G4QEnv::FSI:ANTISTRANGE DecayIn3 did not succeed");
          }
	       }
        if(fOK)
        {
          theQHadrons[ipo]->SetQPDG(fQPDG);
          theQHadrons[ipo]->Set4Momentum(f4M);
          G4QHadron* sH = new G4QHadron(sPDG,s4M);
          theQHadrons.push_back(sH);               // (delete equivalent)
          G4QHadron* tH = new G4QHadron(tPDG,t4M);
          theQHadrons.push_back(tH);               // (delete equivalent)
        }
	     }
      nHadr=theQHadrons.size();
	   }
    else if(hPDG==89999003||hPDG==90002999||hPDG==90000003||hPDG==90003000||
            hPDG==90999002||hPDG==91001999) // "3-particles decays of dibaryons and 3N"
    {
#ifdef pdebug
      G4cout<<"G4QE::FSI:***nD-/pD++/nnn/ppp***i="<<ipo<<",PDG="<<hPDG<<",A="<<hBN<<G4endl;
#endif
      G4double hM=h4Mom.m();
      G4QPDGCode nuQPDG=nQPDG; // n,n,pi-
      G4double nucM = mNeut;
      G4int  barPDG = 2112;
      G4double barM = mNeut;
      G4int   tPDG  = -211;
      G4double tM   = mPi;
      if(hPDG==90002999)       // p,p,pi+
	     {
        nuQPDG = pQPDG;        // Substitute p for the first n
        nucM   = mProt;
        barPDG = 2212;         // Substitute p for the second n
        barM   = mProt;
        tPDG   = 211;          // Substitute pi+ for the first pi-
	     }
      else if(hPDG==90003000)  // 3p
	     {
        nuQPDG = pQPDG;        // Substitute p for the first n
        nucM   = mProt;
        barPDG = 2212;         // Substitute p for the second n
        barM   = mProt;
        tPDG   = 2212;         // Substitute p for pi-
        tM     = mProt;
	     }
      else if(hPDG==90999002)  // n,Lambda,pi-/n,Sigma0,pi-/n,Sigma-,gamma(@@)
	     {
        if(hM>mSigZ+mNeut+mPi)
		      {
          G4double ddMass=hM-mPi;          // Free CM energy
          G4double dd2=ddMass*ddMass;      // Squared free energy
          G4double sma=mLamb+mNeut;        // Neutron+Lambda sum
          G4double pr1=0.;                 // Prototype to avoid sqrt(-)
          if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Neut+Lamb PS
          sma=mNeut+mSigZ;                 // Neutron+Sigma0 sum
          G4double smi=mSigZ-mNeut;        // Sigma0-Neutron difference
          G4double pr2=pr1;                // Prototype of +N+S0 PS
          if(ddMass>sma && ddMass>smi) pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi));
          if(pr2*G4UniformRand()>pr1)      // --> "ENnv+Sigma0+Lambda" case
          {
            barPDG = 3212;     // Substitute Sigma0 for the second n
            barM   = mSigZ;
          }
          else
          {
            barPDG = 3122;     // Substitute Lambda for the second n
            barM   = mLamb;
		        }
        }
        else if(hM>mLamb+mNeut+mPi)
        {
          barPDG = 3122;       // Substitute Lambda for the second n
          barM   = mLamb;
        }
        else if(hM>mSigM+mNeut)// @@ Decay in 2
        {
          barPDG = 3112;       // Substitute Sigma- for the second n
          barM   = mSigM;
          tPDG   = 22;
          tM     = 0;
        }
	     }
      else if(hPDG==91001999)  // p,Lambda,pi+/p,Sigma0,pi+/p,Sigma+,gamma(@@)
	     {
        nuQPDG = pQPDG;        // Substitute p for the first n
        nucM   = mProt;
        tPDG   = 211;          // Substitute pi+ for the first pi-
        if(hM>mSigZ+mProt+mPi)
		      {
          G4double ddMass=hM-mPi;          // Free CM energy
          G4double dd2=ddMass*ddMass;      // Squared free energy
          G4double sma=mLamb+mProt;        // Lambda+Proton sum
          G4double pr1=0.;                 // Prototype to avoid sqrt(-)
          if(ddMass>sma) pr1=sqrt((dd2-sma*sma)*dd2); // Lamb+Prot PS
          sma=mProt+mSigZ;                 // Proton+Sigma0 sum
          G4double smi=mSigZ-mProt;        // Sigma0-Proton difference
          G4double pr2=pr1;                // Prototype of +P+S0 PS
          if(ddMass>sma && ddMass>smi) pr2+=sqrt((dd2-sma*sma)*(dd2-smi*smi));
          if(pr2*G4UniformRand()>pr1)      // --> "ENnv+Sigma0+Lambda" case
          {
            barPDG = 3212;     // Substitute Sigma0 for the second n
            barM   = mSigZ;
          }
          else
          {
            barPDG = 3122;     // Substitute Lambda for the second n
            barM   = mLamb;
		        }
        }
        if(hM>mLamb+mProt+mPi)
        {
          barPDG = 3122;         // Substitute Lambda for the second n
          barM   = mLamb;
        }
        else if(hM>mSigP+mProt)  // @@ Decay in 2
        {
          barPDG = 3222;         // Substitute Sigma- for the second n
          barM   = mSigP;
          tPDG   = 22;
          tM     = 0;
        }
	     }
      else if(hPDG==90000003)  // 3n
	     {
        tPDG   = 2112;         // Substitute n for pi-
        tM     = mNeut;
	     }
      G4bool fOK=true;
      G4LorentzVector nu4M(0.,0.,0.,nucM);
      G4LorentzVector ba4M(0.,0.,0.,barM);
      G4LorentzVector pi4M(0.,0.,0.,tM);
      G4double sum=nucM+barM+tM;
      if(fabs(hM-sum)<=eps)
	     {
        nu4M=h4Mom*(nucM/sum);
        ba4M=h4Mom*(barM/sum);
        pi4M=h4Mom*(tM/sum);
	     }
      if(hM<sum || !G4QHadron(h4Mom).DecayIn3(nu4M,ba4M,pi4M))
	     {
#ifdef pdebug
        G4int eA=theEnvironment.GetA();
        G4cerr<<"***G4QEnv::FSI:T="<<hPDG<<"("<<hM<<")-> N="<<nuQPDG<<"(M="<<nucM<<") + B="
              <<barPDG<<"("<<barM<<")+N/pi="<<tPDG<<"("<<tM<<")="<<sum<<", A="<<eA<<G4endl;
#endif
        //if(!eA)
        //{
          G4QHadron* theLast = theCurr;        // Prototype of the pointer to theLastHadron
          G4QHadron* qH = new G4QHadron(theCurr); // Copy of the Current Hadron
#ifdef pdebug
          G4cerr<<"***G4QE::FSI:#"<<ipo<<",4MQC="<<qH->Get4Momentum()<<qH->GetQC()<<G4endl;
#endif
          if(ipo+1<theQHadrons.size())         // If ipo<Last, swap CurHadr and theLastHadr
          {
            G4int nhd1=theQHadrons.size()-1;
            theLast = theQHadrons[nhd1];// Pointer to theLastHadron (ipo<L)
            G4LorentzVector l4M=theLast->Get4Momentum();
            G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
            if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of LastHadr
            else theCurr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
#ifdef pdebug
			         G4cerr<<"---Warning---G4QE::FSI:l#"<<nhd1<<",4M="<<l4M<<",PDG="<<lQP<<G4endl;
#endif
            theCurr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();              // exclude theLastHadron pointer from OUTPUT
          delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
          G4QContent cqQC=qH->GetQC();
          G4LorentzVector tqLV=qH->Get4Momentum();
          G4Quasmon* quasH = new G4Quasmon(cqQC,tqLV);//Create fakeQuasm
          if(!CheckGroundState(quasH,true))         // Try to correct by other hadrons
          {
#ifdef chdebug
            G4cerr<<"-W-G4QE::FSI:E="<<theEnvironment<<",Q="<<cqQC<<tqLV<<tqLV.m()<<G4endl;
#endif
            theQHadrons.push_back(qH);              // Fill as it is (delete equivalent)
          }
          else
          {
            delete qH;
            jpo--;
            nHadr=theQHadrons.size();
          }
          delete quasH;
          fOK=false;
		      //}
		      //else
		      //{
        //  G4cerr<<"***G4QEnv::FSI:NoRec(3)E="<<theEnvironment<<eA<<",PDG="<<hPDG<<G4endl;
	       //  throw G4QException("G4QEnv::FSI:ISO-dibaryon or 3n/3p DecayIn3 error");
		    //}
	     }
      if(fOK)
      {
        theQHadrons[ipo]->SetQPDG(nuQPDG);
        theQHadrons[ipo]->Set4Momentum(nu4M);
        G4QHadron* baH = new G4QHadron(barPDG,ba4M);
        theQHadrons.push_back(baH);               // (delete equivalent)
        G4QHadron* piH = new G4QHadron(tPDG,pi4M);
        theQHadrons.push_back(piH);               // (delete equivalent)
        nHadr=theQHadrons.size();
      }
	   }
    else if(hBN>1 && !sBN && (cBN<0 || cBN>hBN)) // "nN+mD- or nP+mD++ decay"
    {
#ifdef pdebug
      G4cout<<"G4QE::FSI:nNmD-/nPmD++ #"<<ipo<<",P="<<hPDG<<",B="<<hBN<<",C="<<cBN<<G4endl;
#endif
      G4double hM=h4Mom.m();
      G4QPDGCode nuQPDG=nQPDG; // "(n+m)*N,m*pi-" case === Default
      G4double nucM = mNeut;
      G4int  barPDG = 2112;
      G4double barM = mNeut;
      G4int    nN   = hBN-1;   // a#of baryons - 1
      G4int   tPDG  = -211;
      G4double tM   = mPi;
      G4int    nPi  = -cBN;    // a#of Pi-'s
      if( cBN>hBN)             // reinitialization for the "(n+m)*P,m*pi+" case
	     {
        nuQPDG = pQPDG;        // Substitute p for the first n
        nucM   = mProt;
        barPDG = 2212;         // Substitute p for the second n
        barM   = mProt;
        tPDG   = 211;          // Substitute pi+ for the first pi-
        nPi    = cBN-hBN;      // a#0f Pi+'s
      }
      if(nPi>1)   tM*=nPi;
      if(nN >1) barM*=nN;
      G4bool fOK=true;
      G4LorentzVector nu4M(0.,0.,0.,nucM);
      G4LorentzVector ba4M(0.,0.,0.,barM);
      G4LorentzVector pi4M(0.,0.,0.,tM);
      G4double sum=nucM+barM+tM;
      if(fabs(hM-sum)<=eps)
	     {
        nu4M=h4Mom*(nucM/sum);
        ba4M=h4Mom*(barM/sum);
        pi4M=h4Mom*(tM/sum);
	     }
      if(hM<sum || !G4QHadron(h4Mom).DecayIn3(nu4M,ba4M,pi4M))
	     {
#ifdef pdebug
        G4cerr<<"***G4QEnv::FSI:IsN M="<<hM<<","<<hPDG<<"->N="<<nuQPDG<<"(M="<<nucM<<")+"
              <<nN<<"*B="<<barPDG<<"(M="<<barM<<")+"<<nPi<<"*pi="<<tPDG<<"(M="<<tM<<")="
              <<nucM+barM+tM<<G4endl;
        G4cerr<<"***G4QEnv::FSI:IsN BaryN="<<hBN<<",Charge="<<cBN<<",Stran="<<sBN<<G4endl;
#endif
        if(!theEnvironment.GetA())           // Emergency recovery
        {
          G4QHadron* theLast = theCurr;      // Prototype of the pointer to the Last Hadron
          G4QHadron* qH = new G4QHadron(theCurr); // Copy of the Current Hadron
          if(ipo+1<theQHadrons.size())       // If ipo<Last, swap CurHadr and theLastHadr
          {
            theLast = theQHadrons[theQHadrons.size()-1];// Ptr to theLastHadron (ipo<Last)
            G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
            if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of LastHadr
            else theCurr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
            theCurr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();            // exclude theLastHadron pointer from OUTPUT
          delete theLast;//*!! When killing, DON'T forget to delete the last QHadron !!*
          G4QContent cqQC=qH->GetQC();
          G4LorentzVector cq4M=qH->Get4Momentum();
          G4Quasmon* quasH = new G4Quasmon(cqQC,cq4M);// Create fakeQuasmon for the Last
          if(!CheckGroundState(quasH,true))         // Try to correct by other hadrons
          {
            G4cerr<<"---Warning---G4QEnv::FSI:IN Failed, FillAsItIs: "<<cqQC<<cq4M<<G4endl;
            theQHadrons.push_back(qH);              // Fill as it is (delete equivalent)
          }
          else
          {
            delete qH;
            jpo--;
            nHadr=theQHadrons.size();
          }
          delete quasH;
          fOK=false;
		      }
        else
        {
          G4cerr<<"***G4QEnv::FSI:IN,NoRec(4) Env="<<theEnvironment<<",PDG="<<hPDG<<G4endl;
	         throw G4QException("G4QEnv::FSI:IN Multy ISO-dibaryon DecayIn3 did not succeed");
        }
	     }
      if(fOK)
      {
        theQHadrons[ipo]->SetQPDG(nuQPDG);
        theQHadrons[ipo]->Set4Momentum(nu4M);
        if(nN>1) ba4M=ba4M/nN;
        for(G4int ib=0; ib<nN; ib++)
        {
          G4QHadron* baH = new G4QHadron(barPDG,ba4M);
          theQHadrons.push_back(baH);               // (delete equivalent)
        }
        if(nPi>1) pi4M=pi4M/nPi;
        for(G4int im=0; im<nPi; im++)
        {
          G4QHadron* piH = new G4QHadron(tPDG,pi4M);
          theQHadrons.push_back(piH);               // (delete equivalent)
        }
        nHadr=theQHadrons.size();
      }
    }
#ifdef pdebug
    G4int           hNFrag= theQHadrons[ipo]->GetNFragments(); //Recover after swapping
    G4QContent      hQC   = theQHadrons[ipo]->GetQC();         // ...
    hPDG                  = theQHadrons[ipo]->GetPDGCode();    // ...
    h4Mom                 = theQHadrons[ipo]->Get4Momentum();  // ...
    ccs4M+=h4Mom;                                              // Calculate CurSum of Hadrs
    G4cout<<"G4QE::FSI:#"<<ipo<<": h="<<hPDG<<hQC<<",h4M="<<h4Mom<<h4Mom.m()<<",hNF="
          <<hNFrag<<G4endl;
#endif
    ipo=jpo;            // Take into account the roll back in case of the Last substitution
  }
#ifdef pdebug
  G4cout<<"G4QE::FSI: >>>CurrentControlSumOf4MomOfHadrons="<<ccs4M<<G4endl;
#endif
  nHadr=theQHadrons.size();
#ifdef chdebug
  // *** (1) Charge Control Sum Calculation for the Charge Conservation Check ***
  G4int ccContSum=0;                   // Intermediate ChargeControlSum 
  G4int cbContSum=0;                   // Intermediate BaryonNumberControlSum 
  if(nHadr)for(unsigned ic1=0; ic1<nHadr; ic1++) if(!(theQHadrons[ic1]->GetNFragments()))
  {
    ccContSum+=theQHadrons[ic1]->GetCharge();
    cbContSum+=theQHadrons[ic1]->GetBaryonNumber();
  }
  if(ccContSum-chContSum || cbContSum-bnContSum)
  {
    G4cerr<<"*G4QE::FSI:(1) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum<<G4endl;
    //throw G4QException("G4QEnvironment::FSInteract: (1) Charge is not conserved");
  }
  // ***
#endif
  G4double p2cut=250000.;        // 250000=(2*p_Ferm)**2
  if(envA>0) p2cut/=envA*envA;
  //G4double p2cut2=0.;          //cut for the alpha creation
  //
  G4int bfCountM=3;
  if(envA>10) bfCountM*=(envA-1)/3;
  G4bool bfAct = true;
  G4int bfCount= 0;
  G4LorentzVector tmp4Mom=tot4Mom;
  G4LorentzVector postp4M(0.,0.,0.,0.);
  G4int nPost=intQHadrons.size();
  if(nPost) for(G4int psp=0; psp<nPost; psp++)
    if(!(intQHadrons[psp]->GetNFragments())) postp4M+=intQHadrons[psp]->Get4Momentum();
  while(bfAct&&bfCount<bfCountM) // "Infinite" LOOP of the ThermoNuclearBackFusion
  {
    tot4Mom=tmp4Mom-postp4M;     // Prepare tot4Mom for the "En/Mom conservation" reduction
    bfAct=false;
    bfCount++;
    nHadr=theQHadrons.size();
    if(nHadr) for(unsigned hadron=0; hadron<theQHadrons.size(); hadron++)// BackFusion LOOP
    {
      G4QHadron* curHadr = theQHadrons[hadron]; // Get a pointer to the current Hadron
      G4int         hPDG = curHadr->GetPDGCode();
      G4QPDGCode    hQPDG(hPDG);
      G4double      hGSM = hQPDG.GetMass();     // Ground State Mass of the first fragment
#ifdef pdebug
      G4cout<<"G4QE::FSI:LOOP START,h#"<<hadron<<curHadr->Get4Momentum()<<hPDG<<G4endl;
#endif
      if(hPDG==89999003||hPDG==90002999)
      {
        G4cerr<<"---WARNING---G4QE::FSI:**nD-/pD++**(3),PDG="<<hPDG<<" CORRECTION"<<G4endl;
        G4LorentzVector h4Mom=curHadr->Get4Momentum();
        G4double      hM=h4Mom.m();
        G4QPDGCode fQPDG=nQPDG;
        G4double     fM =mNeut;
        G4int      sPDG =2112;
        G4double     sM =mNeut;
        G4int      tPDG =-211;
        G4double     tM =mPi;
        if(hPDG==90002999)
		      {
          fQPDG=pQPDG;
          fM   =mProt;
          sPDG =2212;
          sM   =mProt;
          tPDG =211;
        }
        G4bool fOK=true;
        G4LorentzVector f4M(0.,0.,0.,fM);
        G4LorentzVector s4M(0.,0.,0.,sM);
        G4LorentzVector t4M(0.,0.,0.,tM);
        G4double sum=fM+sM+tM;
        if(fabs(hM-sum)<=eps)
		      {
          f4M=h4Mom*(fM/sum);
          s4M=h4Mom*(sM/sum);
          t4M=h4Mom*(tM/sum);
		      }
        else if(hM<sum || !G4QHadron(h4Mom).DecayIn3(f4M,s4M,t4M))
	       {
          G4cerr<<"---WARNING---G4QE::FSI: Still trying, NDM="<<hM<<"->"<<fM<<"("<<fQPDG
                <<")+"<<sM<<"("<<sPDG<<")+"<<tM<<"("<<tPDG<<")="<<sum<<G4endl;
          if(!theEnvironment.GetA())
          {
            G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
            G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
            if(hadron+1<theQHadrons.size())        // If hadr<Last,swap CurHadr & LastHadr
            {
              theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr (nh<Last)
              G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
              if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of LastHadr
              else curHadr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
              curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
            }
            theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
            delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
            G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
            if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
            {
              G4cerr<<"---Warning---G4QE::FSI:NDel Failed LeaveAsItIs, 4m="<<h4Mom<<G4endl;
              theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
            }
            else
            {
              delete qH;
              nHadr=theQHadrons.size();
            }
            delete quasH;
            fOK=false;
		        }
          else
          {
            G4cerr<<"***G4QEnv::FSI: No ND recovery Env="<<theEnvironment<<G4endl;
	           throw G4QException("G4QEnv::FSI:ND DecayIn3 did not succeed");
          }
	       }
        if(fOK)
        {
          curHadr->SetQPDG(fQPDG);
          curHadr->Set4Momentum(f4M);
          G4QHadron* sH = new G4QHadron(sPDG,s4M);
          theQHadrons.push_back(sH);               // (delete equivalent)
          G4QHadron* tH = new G4QHadron(tPDG,t4M);
          theQHadrons.push_back(tH);               // (delete equivalent)
        }
        hPDG = curHadr->GetPDGCode();            // Change PDG Code of theFirstFragment
        hQPDG= G4QPDGCode(hPDG);
        hGSM = hQPDG.GetMass();                  // Change GroundStateMass of theFirstFragm
	     }
      nHadr=theQHadrons.size();
      if(hPDG==89001001||hPDG==89002000||hPDG==89000002)
      {
        G4cerr<<"---WARNING---G4QE::FSI:***(K+N)*** (2),PDG="<<hPDG<<" CORRECTION"<<G4endl;
        G4LorentzVector h4Mom=curHadr->Get4Momentum();
        G4double      hM=h4Mom.m();
        G4QPDGCode fQPDG=nQPDG;
        G4double     fM =mNeut;
        G4int      sPDG =311;
        G4double     sM =mK0;
        if(hPDG==89000002)
		      {
          fQPDG=pQPDG;
          fM   =mProt;
          sPDG =321;
          sM   =mK;
        }
        if(hPDG==89001001)
		      {
          if(hM<mK0+mProt || G4UniformRand()>.5)
          {
            sPDG =321;
            sM   =mK;
          }
          else
          {
            fQPDG=pQPDG;
            fM   =mProt;
          }
        }
        G4bool fOK=true;
        G4LorentzVector f4M(0.,0.,0.,fM);
        G4LorentzVector s4M(0.,0.,0.,sM);
        G4double sum=fM+sM;
        if(fabs(hM-sum)<=eps)
		      {
          f4M=h4Mom*(fM/sum);
          s4M=h4Mom*(sM/sum);
		      }
        else if(hM<sum || !G4QHadron(h4Mom).DecayIn2(f4M,s4M))
	       {
          G4cerr<<"---WARNING---G4QE::FSI: Still trying (2),NDM="<<hM<<"->"<<fM<<"("<<fQPDG
                <<")+"<<sM<<"("<<sPDG<<")="<<sum<<G4endl;
          if(!theEnvironment.GetA())
          {
            G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
            G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
            if(hadron+1<theQHadrons.size())        // If hadr<Last, swap CurHadr & LastHadr
            {
              theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr (nh<Last)
              G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
              if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of LastHadr
              else curHadr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
              curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
            }
            theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
            delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
            G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
            if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
            {
              G4cerr<<"---Warning---G4QE::FSI:KN Failed LeaveAsItIs 4m="<<h4Mom<<G4endl;
              theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
            }
            else
            {
              delete qH;
              nHadr=theQHadrons.size();
            }
            delete quasH;
            fOK=false;
		        }
          else
          {
            G4cerr<<"***G4QEnv::FSI: No KN recovery Env="<<theEnvironment<<G4endl;
	           throw G4QException("G4QEnv::FSI:KN DecayIn2 did not succeed");
          }
	       }
        if(fOK)
        {
          curHadr->SetQPDG(fQPDG);
          curHadr->Set4Momentum(f4M);
          G4QHadron* sH = new G4QHadron(sPDG,s4M);
          theQHadrons.push_back(sH);               // (delete equivalent)
        }
        hPDG = curHadr->GetPDGCode();            // Change PDG Code of theFirstFragment
        hQPDG= G4QPDGCode(hPDG);
        hGSM = hQPDG.GetMass();                  // Change GroundStateMass of theFirstFragm
	     }
      nHadr=theQHadrons.size();
      G4int           hS = curHadr->GetStrangeness();
      G4int           hF = curHadr->GetNFragments();
      G4LorentzVector h4m= curHadr->Get4Momentum();
      G4double hM        = h4m.m();             // Real Mass of the first fragment
      G4int hB           = curHadr->GetBaryonNumber();
      //////////////////////G4int hC           = curHadr->GetCharge();
#ifdef pdebug
      if(!hF&&(hPDG>80000000&&hPDG<90000000||hPDG==90000000||
               hPDG>90000000&&(hPDG%1000000>200000||hPDG%1000>300)))
        G4cerr<<"**G4QEnv::FSInteraction: PDG("<<hadron<<")="<<hPDG<<", M="<<hM<<G4endl;
#endif
#ifdef pdebug
      G4cout<<"G4QE::FSI:h="<<hPDG<<",S="<<hS<<",B="<<hB<<",#"<<hadron<<"<"<<nHadr<<G4endl;
#endif
	     //if(hadron&&!hF&&hB>0&&!hS&&(nHadr>3||hB<2)) // ThermoBackFus (VIMP for gamA TotCS)
	     //if(hadron&&!hF&&hB>0&&!hS&&(nHadr>2||hB<4)) // ThermoBackFus (VIMP for gamA TotCS)
	     if(hadron&&!hF&&hB>0&&!hS) // ThermoBackFusion cond. (VIMP for gamA TotCS)
	     //if(hadron&&!hF&&hB>0&&hB<4&&!hS) // ThermoBackFusion cond. (VIMP for gamA TotCS)
	     //if(hadron&&!hF&&hB>0&&!hS&&nHadr>2)//ThermoBackFusion MAX condition (VIMP for gamA)
	     //if(2>3)                         // Close the ThermoBackFusion (VIMP for gamA TotCS)
      {
#ifdef pdebug
        //if(nHadr==3)
          G4cout<<"G4QE::FSI: h="<<hPDG<<",B="<<hB<<",h#"<<hadron<<" < nH="<<nHadr<<G4endl;
#endif
        G4QContent hQC = curHadr->GetQC();
        if(hadron&&!hF&&hB>0) for(unsigned pt=0; pt<hadron; pt++)
	       {
          G4QHadron* backH = theQHadrons[pt];   // Get pointer to one of thePreviousHadrons
          G4int   bF = backH->GetNFragments();
          G4LorentzVector b4m= backH->Get4Momentum();
          G4double bM= b4m.m();                 // Real Mass of the second fragment
          G4QContent bQC = backH->GetQC();
          G4int bPDG=bQC.GetZNSPDGCode();
          G4QPDGCode bQPDG(bPDG);
          G4double bGSM=bQPDG.GetMass();        // Ground State Mass of the second fragment
          G4int   bB = backH->GetBaryonNumber();

          //////////////////G4int   bC = backH->GetCharge();
          G4QContent sQC=bQC+hQC;
          G4int sPDG=sQC.GetZNSPDGCode();
          G4QPDGCode sQPDG(sPDG);
          G4double tM=sQPDG.GetMass();
          G4LorentzVector s4M=h4m+b4m;
          G4double sM2=s4M.m2();
          G4double sM=sqrt(sM2);
          G4double dsM2=sM2+sM2;
          G4double rm=bM-hM;
          G4double sm=bM+hM;
          G4double pCM2=(sM2-rm*rm)*(sM2-sm*sm)/(dsM2+dsM2);
          G4int   bS = backH->GetStrangeness();
#ifdef pdebug
          //if(nHadr==3)
		        G4cout<<"G4QE::FSI:"<<pt<<",B="<<bB<<",S="<<bS<<",p="<<pCM2<<"<"<<p2cut<<",hB="
                <<hB<<",bM+hM="<<bM+hM<<">tM="<<tM<<",tQC="<<sQC<<G4endl;
#endif
          //if(!bF&&(bB==1||hB==1)&&bM+hM>tM+.001&&pCM2<p2cut)      // Only baryons == pcut
		        //if(!bF&&!bS&&(bB==1&&hB>0||hB==1&&bB>0)&&bM+hM>tM+.00001
          //   && (pCM2<p2cut&&nHadr>3||pCM2<p2cut2&&nHadr==3))
		        //if(!bF&&(bB==1||hB==1)&&bM+hM>tM+.001&&(pCM2<p2cut&&nHadr>3 ||
		        //   pCM2<p2cut2&&nHadr==3&&bPDG>90000000))
          //if(!bF&&bB<4&&bM+hM>tM+.001&&pCM2<p2cut)
          if(!bF&&!bS&&bB>0&&bM+hM>tM+.001&&pCM2<p2cut)
          //if(!bF&&bB<4&&bM+hM>tM+.001&&(pCM2<p2cut || bB+hB==4&&pCM2<p2cut2))
		        //if(!bF&&(bB==1||hB==1)&&(nHadr>3||bPDG>90000000)&&bM+hM>tM+.001&&pCM2<p2cut)
		        //if(!bF&&(bB==1&&!bC||hB==1&&!hC)&&bM+hM>tM+.001&&pCM2<p2cut)// Only n == pcut
		        //if(!bF&&(bB==1||hB==1)&&bM+hM>tM+.001&&sM-bM-hM<cut)  // Only baryons == ecut
		        //if(!bF&&bB&&bB<fL&&bM+hM>tM+.001&&sM-bM-hM<cut)    // Light fragments == ecut
	         {
#ifdef fdebug
            G4int bPDG = backH->GetPDGCode();
			         if(sPDG==89999003||sPDG==90002999||sPDG==89999002||sPDG==90001999)
              G4cout<<"G4QE::FSI:**nD-/pD++**,h="<<hPDG<<",hB="<<hB<<",b="<<bPDG<<",bB="
                    <<bB<<G4endl;
            //if(nHadr==3)
	           G4cout<<"G4QE::FSI:*FUSION*#"<<hadron<<"["<<hPDG<<"]"<<hM<<"+#"<<pt<<"["<<bPDG
                  <<"]"<<bM<<"="<<bM+hM<<", sM="<<sM<<">["<<sPDG<<"]"<<tM<<",p2="<<pCM2
                  <<"<"<<p2cut<<G4endl;
#endif
            bfAct=true;
            //@@Temporary decay in gamma
            G4bool three=false;
            G4QPDGCode fQPDG=sQPDG;
            G4QPDGCode rQPDG=gQPDG;
            G4QPDGCode hQPDG=gQPDG;
            G4LorentzVector f4Mom(0.,0.,0.,tM);
            G4LorentzVector g4Mom(0.,0.,0.,0.);
            G4LorentzVector t4Mom(0.,0.,0.,0.);
            if(sPDG==89999002)                               // A=1
		          {
              fQPDG=nQPDG;
              rQPDG=pimQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              g4Mom=G4LorentzVector(0.,0.,0.,mPi);
		          }
            else if(sPDG==90001999)
		          {
              fQPDG=pQPDG;
              rQPDG=pipQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mPi);
		          }
            else if(sPDG==90000002)                        // A=2
		          {
              fQPDG=nQPDG;
              rQPDG=nQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              g4Mom=f4Mom;
		          }
            else if(sPDG==90002000)
		          {
              fQPDG=pQPDG;
              rQPDG=pQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=f4Mom;
		          }
            else if(sPDG==92000000)
		          {
              fQPDG=lQPDG;
              rQPDG=lQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mLamb);
              g4Mom=f4Mom;
              if(sM>mSigZ+mSigZ)             // Sigma0+Sigma0 is possible
			           {                                  // @@ Only two particles PS is used
                G4double sma=mLamb+mLamb;        // Lambda+Lambda sum
                G4double pr1=0.;                 // Prototype to avoid sqrt(-)
                if(sM>sma) pr1=sqrt((sM2-sma*sma)*sM2); // Lamb+Lamb PS
                sma=mLamb+mSigZ;                 // Lambda+Sigma0 sum
                G4double smi=mSigZ-mLamb;        // Sigma0-Lambda difference
                G4double pr2=pr1;                // Prototype of +L +S0 PS
                if(sM>sma && sM>smi) pr2+=sqrt((sM2-sma*sma)*(sM2-smi*smi));
                sma=mSigZ+mSigZ;                 // Sigma0+Sigma0 sum
                G4double pr3=pr2;                // Prototype of +Sigma0+Sigma0 PS
                if(sM>sma) pr3+=sqrt((sM2-sma*sma)*sM2);
                G4double hhRND=pr3*G4UniformRand(); // Randomize PS
                if(hhRND>pr2)                    // --> "ENnv+Sigma0+Sigma0" case
                {                                //
                  fQPDG=s0QPDG;
                  f4Mom=G4LorentzVector(0.,0.,0.,mSigZ);
                  rQPDG=s0QPDG;
                  g4Mom=f4Mom;
                }                                //
                else if(hhRND>pr1)               // --> "ENnv+Sigma0+Lambda" case
                {                                //
                  fQPDG=s0QPDG;
                  f4Mom=G4LorentzVector(0.,0.,0.,mSigZ);
                }                                //
              }
              else if(sM>mSigZ+mLamb)            // Lambda+Sigma0 is possible
			           {                                  // @@ Only two particles PS is used
                G4double sma=mLamb+mLamb;        // Lambda+Lambda sum
                G4double pr1=0.;                 // Prototype to avoid sqrt(-)
                if(sM>sma) pr1=sqrt((sM2-sma*sma)*sM2); // Lamb+Lamb PS
                sma=mLamb+mSigZ;                 // Lambda+Sigma0 sum
                G4double smi=mSigZ-mLamb;        // Sigma0-Lambda difference
                G4double pr2=pr1;                // Prototype of +L +S0 PS
                if(sM>sma && sM>smi) pr2+=sqrt((sM2-sma*sma)*(sM2-smi*smi));
                if(pr2*G4UniformRand()>pr1)      // --> "ENnv+Sigma0+Lambda" case
                {                                //
                  fQPDG=s0QPDG;
                  f4Mom=G4LorentzVector(0.,0.,0.,mSigZ);
                }                                //
              }                                  //
		          }
            else if(sPDG==89999003)                       // A=2
		          {
              hQPDG=nQPDG;
              rQPDG=nQPDG;
              fQPDG=pimQPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mPi);
              three=true;
		          }
            else if(sPDG==90002999)
		          {
              hQPDG=pQPDG;
              rQPDG=pQPDG;
              fQPDG=pipQPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mPi);
              three=true;
		          }
            else if(sPDG==90000003)                        // A=3
		          {
              hQPDG=nQPDG;
              rQPDG=nQPDG;
              fQPDG=nQPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              three=true;
		          }
            else if(sPDG==90003000)
		          {
              hQPDG=pQPDG;
              rQPDG=pQPDG;
              fQPDG=pQPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mProt);
              three=true;
		          }
            else if(sPDG==90001003)                     // A=4
		          {
              rQPDG=nQPDG;
              fQPDG=tQPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mTrit);
		          }
            else if(sPDG==90003001)
		          {
              rQPDG=pQPDG;
              fQPDG=he3QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mHe3);
		          }
            else if(sPDG==90002003)                     // A=5
		          {
              rQPDG=nQPDG;
              fQPDG=aQPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mAlph);
		          }
            else if(sPDG==90003002)
		          {
              rQPDG=pQPDG;
              fQPDG=aQPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mAlph);
		          }
            else if(sPDG==90004002)                          // A=6
		          {
              hQPDG=pQPDG;
              rQPDG=pQPDG;
              fQPDG=aQPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mAlph);
              three=true;
		          }
            else if(sPDG==90002005)                        // A=7
		          {
              rQPDG=nQPDG;
              fQPDG=a6QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mHe6);
		          }
            else if(sPDG==90005002)
		          {
              rQPDG=pQPDG;
              fQPDG=be6QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mBe6);
		          }
            else if(sPDG==90004004)                        // A=8
		          {
              fQPDG=aQPDG;
              rQPDG=aQPDG;
              f4Mom=G4LorentzVector(0.,0.,0.,mAlph);
              g4Mom=f4Mom;
		          }
            //else if(sPDG==90006002)
		          //{
            //  hQPDG=pQPDG;
            //  rQPDG=pQPDG;
            //  fQPDG=be6QPDG;
            //  t4Mom=G4LorentzVector(0.,0.,0.,mProt);
            //  g4Mom=G4LorentzVector(0.,0.,0.,mProt);
            //  f4Mom=G4LorentzVector(0.,0.,0.,mBe6);
            //  three=true;
		          //}
            //else if(sPDG==90002006)
		          //{
            //  hQPDG=nQPDG;
            //  rQPDG=nQPDG;
            //  fQPDG=a6QPDG;
            //  t4Mom=G4LorentzVector(0.,0.,0.,mNeut);
            //  g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
            //  f4Mom=G4LorentzVector(0.,0.,0.,mHe6);
            //  three=true;
		          //}
            else if(sPDG==90002007)                      // A=9
		          {
              rQPDG=nQPDG;
              fQPDG=a8QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mNeut);
              f4Mom=G4LorentzVector(0.,0.,0.,mHe8);
		          }
            else if(sPDG==90005004)                      // A=9
		          {
              rQPDG=pQPDG;
              fQPDG=aQPDG;
              hQPDG=aQPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mAlph);
              t4Mom=G4LorentzVector(0.,0.,0.,mAlph);
              three=true;
		          }
            else if(sPDG==90007002)                      // A=9
		          {
              rQPDG=pQPDG;
              fQPDG=c8QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mC8);
		          }
            else if(sPDG==90008004)                      // A=12
		          {
              hQPDG=pQPDG;
              rQPDG=pQPDG;
              fQPDG=c10QPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mC10);
              three=true;
		          }
            else if(sPDG==90009006)                     // A=15
		          {
              rQPDG=pQPDG;
              fQPDG=o14QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mO14);
		          }
            else if(sPDG==90009007)                     // A=16
		          {
              rQPDG=pQPDG;
              fQPDG=o15QPDG;
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mO15);
		          }
            else if(sPDG==90010006)                     // A=16
		          {
              hQPDG=pQPDG;
              rQPDG=pQPDG;
              fQPDG=o14QPDG;
              t4Mom=G4LorentzVector(0.,0.,0.,mProt);
              g4Mom=G4LorentzVector(0.,0.,0.,mProt);
              f4Mom=G4LorentzVector(0.,0.,0.,mO14);
              three=true;
		          }
#ifdef pdebug
            G4cout<<"G4QE::FSI: "<<three<<",r="<<rQPDG<<",f="<<fQPDG<<",t="<<hQPDG<<G4endl;
#endif
            if(!three)
            {
              if(!G4QHadron(s4M).DecayIn2(f4Mom,g4Mom))
              {
                G4cerr<<"***G4QE::FSI:(2)*FUSION*,tM["<<sPDG<<"]="<<tM<<">sM="<<sM<<" of "
                      <<h4m<<hM<<hQC<<hGSM<<" & "<<b4m<<bM<<bQC<<bGSM<<G4endl;
                throw G4QException("***G4QEnvironment::FSInter:Fusion (1) DecIn2 error");
              }
              else
			           {
#ifdef pdebug
                G4cout<<"G4QE::FSI:*FUSION IS DONE*,fPDG="<<sPDG<<",PDG1="<<hPDG<<",PDG2="
                      <<bPDG<<G4endl;
#endif
                curHadr->SetQPDG(fQPDG);
                curHadr->Set4Momentum(f4Mom);
                backH->SetQPDG(rQPDG);
                backH->Set4Momentum(g4Mom);
#ifdef pdebug
                G4cout<<"G4QE::FSI:h="<<h4m<<",b="<<b4m<<",s="<<s4M<<G4endl;
                G4cout<<"G4QE::FSI:f="<<f4Mom<<",g="<<g4Mom<<",s="<<f4Mom+g4Mom<<G4endl;
#endif
              }
		          }
            else
            {
              if(!G4QHadron(s4M).DecayIn3(f4Mom,g4Mom,t4Mom))
              {
                G4cerr<<"***G4QE::FSI:(3)*FUSION*,tM["<<sPDG<<"]="<<tM<<">sM="<<sM<<" of "
                      <<h4m<<hM<<hQC<<hGSM<<" & "<<b4m<<bM<<bQC<<bGSM<<G4endl;
                throw G4QException("G4QEnvironment::FSInteract:Fusion(2) DecayIn3 error");
              }
              else
			           {
#ifdef pdebug
                G4cout<<"G4QE::FSI:DONE,n="<<nHadr<<",PDG="<<sPDG<<",1="<<hPDG<<",2="<<bPDG
                      <<G4endl;
#endif
                curHadr->SetQPDG(fQPDG);
                curHadr->Set4Momentum(f4Mom);
                backH->SetQPDG(rQPDG);
                backH->Set4Momentum(g4Mom);
                G4QHadron* newH = new G4QHadron(hQPDG.GetPDGCode(),t4Mom);
                theQHadrons.push_back(newH);      // (delete equivalent for newH)
                nHadr=theQHadrons.size();
#ifdef pdebug
                G4cout<<"G4QE::FSI:h="<<h4m<<",b="<<b4m<<G4endl;
                G4cout<<"G4QE::FSI:s="<<s4M<<" = Sum"<<f4Mom+g4Mom+t4Mom<<G4endl;
                G4cout<<"G4QE::FSI:*Products*,nH="<<nHadr<<",f="<<fQPDG<<f4Mom<<",b="
                      <<rQPDG<<g4Mom<<",new="<<hQPDG<<t4Mom<<",nH="<<nHadr<<",nD="
                      <<theQHadrons.size()<<G4endl;
#endif
              }
		          }
            tot4Mom+=b4m;                       // Instead of the fused hadron
            tot4Mom-=g4Mom;                     // subtract from the total the new hadron
            /////////////////////////////break; // Make fusion only for one (?)
            // Instead the curHadr parameters should be updated ______
            hQPDG=fQPDG;
            hPDG=hQPDG.GetPDGCode();
            hQC=fQPDG.GetQuarkContent();
            hS=hQC.GetStrangeness();
            hB=hQC.GetBaryonNumber();
            hGSM = hQPDG.GetMass();
		          h4m=f4Mom;
            hM=h4m.m();
            // End of Instead ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifdef pdebug
            G4cout<<"G4QE::FSI:cH4M="<<curHadr->Get4Momentum()<<G4endl;
#endif
		        } // End of the fusion check
	       } // End of the LOOP over previous hadrons
	     } // End of the FUSION check
#ifdef chdebug
      // *** (2) Charge Control Sum Calculation for the Charge Conservation Check ***
      ccContSum=0;                   // Intermediate ChargeControlSum 
      cbContSum=0;                   // Intermediate BaryonNumberControlSum 
      if(nHadr)for(unsigned ic2=0;ic2<nHadr;ic2++) if(!(theQHadrons[ic2]->GetNFragments()))
      {
        ccContSum+=theQHadrons[ic2]->GetCharge();
        cbContSum+=theQHadrons[ic2]->GetBaryonNumber();
      }
      unsigned pHadr=intQHadrons.size();
      if(pHadr)for(unsigned ic3=0;ic3<pHadr;ic3++) if(!(intQHadrons[ic3]->GetNFragments()))
      {
        ccContSum+=intQHadrons[ic3]->GetCharge();
        cbContSum+=intQHadrons[ic3]->GetBaryonNumber();
      }
      if(ccContSum-chContSum || cbContSum-bnContSum)
      {
        G4cerr<<"*G4QE::FSI:(2) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
              <<G4endl;
        //throw G4QException("G4QEnvironment::FSInteract: (2) Charge is not conserved");
      }
      // ***
#endif
      G4LorentzVector cH4Mom = curHadr->Get4Momentum(); // 4-mom of the current hadron
      tot4Mom-=cH4Mom;                          // Reduce theTotal 4mom by theCurrent 4mom
      if(hadron+1==nHadr)                       // The Last Hadron in the set
	     {
        G4double re =tot4Mom.e();
        G4double rpx=tot4Mom.px();
        G4double rpy=tot4Mom.py();
        G4double rpz=tot4Mom.pz();
        G4double dmo=rpx*rpx+rpy*rpy+rpz*rpz;
        G4double dem=re*re+dmo;
#ifdef pdebug
        G4cout<<"G4QE::FSI: Is Energy&Mom conserved? t4M="<<tot4Mom<<",d2="<<dem<<G4endl;
#endif
		      //if(2>3)                                 //@@En/Mom conservation check is closed
        if(dem>0.0001)                          // Energy or momentum is not conserved
	       {
#ifdef pdebug
          if(dem>.1)
          {
            G4cerr<<"---Warning---G4QE::FSI:E/Mcons4M="<<tot4Mom<<dem<<".Correct!"<<G4endl;
            G4LorentzVector cor4M(0.,0.,0.,mNeut);  // Prototype for the neutron
            if(dmo<0.0001 && re>900.)               // MomentumIsConserved - recoverMissing
			         {
              if(fabs(re-mNeut)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by neutron"<<G4endl;
                //cor4M=G4LorentzVector(0.,0.,0.,mNeut); // Made as a prototype
                G4QHadron* theH = new G4QHadron(90000001,G4LorentzVector(0.,0.,0.,mNeut));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mProt)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by proton"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mProt);
                G4QHadron* theH = new G4QHadron(90001000,G4LorentzVector(0.,0.,0.,mProt));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mDeut)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by deuteron"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mDeut);
                G4QHadron* theH = new G4QHadron(90001001,G4LorentzVector(0.,0.,0.,mDeut));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mTrit)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by tritium"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mTrit);
                G4QHadron* theH = new G4QHadron(90001002,G4LorentzVector(0.,0.,0.,mTrit));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mHe3)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by He3"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mHe3);
                G4QHadron* theH = new G4QHadron(90002001,G4LorentzVector(0.,0.,0.,mHe3));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mAlph)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by alpha"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mAlph);
                G4QHadron* theH = new G4QHadron(90002002,G4LorentzVector(0.,0.,0.,mAlph));
                theQHadrons.push_back(theH);  // (delete equivalent for the proton)
              }
			           else if(fabs(re-mNeut-mNeut)<.01)
			           {
                cor4M=G4LorentzVector(0.,0.,0.,mNeut+mNeut);
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by 2 neutrons"<<G4endl;
                G4QHadron* theH1 = new G4QHadron(90000001,G4LorentzVector(0.,0.,0.,mNeut));
                theQHadrons.push_back(theH1); // (delete equivalent for the proton)
                G4QHadron* theH2 = new G4QHadron(90000001,G4LorentzVector(0.,0.,0.,mNeut));
                theQHadrons.push_back(theH2); // (delete equivalent for the proton)
              }
			           else if(fabs(re-mProt-mProt)<.01)
			           {
                G4cout<<"...G4QE::FSI:E/M conservation is corrected by 2 protons"<<G4endl;
                cor4M=G4LorentzVector(0.,0.,0.,mProt+mProt);
                G4QHadron* theH1 = new G4QHadron(90001000,G4LorentzVector(0.,0.,0.,mProt));
                theQHadrons.push_back(theH1); // (delete equivalent for the proton)
                G4QHadron* theH2 = new G4QHadron(90001000,G4LorentzVector(0.,0.,0.,mProt));
                theQHadrons.push_back(theH2); // (delete equivalent for the proton)
              }
			           else throw G4QException("***G4QEnv::FSInteract: Increase theCorrectionEps");
			         }
            else
            {
              // @@ Temporary very bad correction: just to pass through @@
              curHadr->Set4Momentum(cH4Mom+tot4Mom); // modify 4-mom of the Last hadron
              G4cout<<"*!* G4QE::FSI:E/M dif is added to theLast="<<tot4Mom+cH4Mom<<G4endl;
              if(curHadr->GetBaryonNumber()>1)
              {
                G4QHadron* theNew  = new G4QHadron(curHadr); // Make NewHadr of theLastHadr
                theQHadrons.pop_back();       // theLastQHadron is excluded from theOUTPUT
                delete curHadr;//!!When kill,DON'T forget to delete theLastQH as an inst.!!
                EvaporateResidual(theNew);    // Try to evaporate Residual
              }
              //throw G4QException("***G4QEnv::FSInteract: Energy/Momentum conservation");
            }
            tot4Mom=tot4Mom-cor4M;
            G4cerr<<"---Warning---G4QE::FSI:En/MomCons.Error is corrected:"<<cor4M<<G4endl;
          }
#endif
          G4QHadron* prevHadr = theQHadrons[nHadr-2]; // GetPointer to Hadr prev to theLast
          G4LorentzVector pH4Mom = prevHadr->Get4Momentum(); // 4-mom of thePreviousHadron
          G4double cHM = curHadr->GetMass();  // Mass of the current hadron
          G4double pHM = prevHadr->GetMass(); // Mass of the current hadron
          tot4Mom+=cH4Mom+pH4Mom;
          G4double totRM=tot4Mom.m();
          if(cHM+pHM<=totRM)                  // *** Make the final correction ***
		        {
            if(!G4QHadron(tot4Mom).DecayIn2(pH4Mom,cH4Mom))
            {
              G4cerr<<"***G4QE::FSI:**Correction**tot4M="<<tot4Mom<<totRM<<">sM="<<cHM+cHM
                    <<G4endl;
#ifdef pdebug
              throw G4QException("***G4QEnvironment::FSInteract:CORRECTION DecIn2 error");
#endif
            }
#ifdef chdebug
            G4cout<<"---Warning---G4QE::FSI:***CORRECTION IS DONE*** d="<<dem<<G4endl;
#endif
            curHadr->Set4Momentum(cH4Mom);
            prevHadr->Set4Momentum(pH4Mom);
          }
          else
          {
            G4cerr<<"*!*G4QE::FSI:NoCor "<<cHM<<"+"<<pHM<<"="<<cHM+pHM<<">"<<totRM<<G4endl;
#ifdef pdebug
            throw G4QException("***G4QEnvironment::FSInteraction: TEMPORARY EXCEPTION");
#endif
          }
        }
#ifdef pdebug
        else G4cout<<"G4QE::FSI: Yes, it is. d="<<dem<<" for "<<nHadr<<" hadrons."<<G4endl;
#endif
      }
    } // End of the Back fusion LOOP
    // >| 2     | 2  | 2     | 2     | 2      | 2 - old    | 1. If gamma: add to sum4Mom
    //  |>0->sum| 3  | 3     | 3     | 3      | 3 - old    | 2. Compare BN with the Last
    //  | 5     |>5  | 4     | 4     | 4      | 4 - old    | 3. Swap if larger, del theLast
    //  | 0     | 0  |>0->sum| 5<-sum| 5->evap| 2 - new    | 4. If the Last: add the sum
    //  | 4     | 4  | 5     | ex    |        |(0 - gamma?)| 5. Decay/Eporate the Last
    //  | 3     | ex |                        | 3 - new
#ifdef chdebug
    // *** (3) Charge Control Sum Calculation for the Charge Conservation Check ***
    ccContSum=0;                              // Intermediate ChargeControlSum 
    cbContSum=0;                              // Intermediate BaryonNumberControlSum 
    if(nHadr)for(unsigned ic3=0; ic3<nHadr; ic3++) if(!(theQHadrons[ic3]->GetNFragments()))
    {
      ccContSum+=theQHadrons[ic3]->GetCharge();
      cbContSum+=theQHadrons[ic3]->GetBaryonNumber();
    }
    if(ccContSum-chContSum || cbContSum-bnContSum)
    {
      G4cerr<<"*G4QE::FSI:(3) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
            <<G4endl;
      //throw G4QException("G4QEnvironment::FSInteract: (3) Charge is not conserved");
    }
    // ***
#endif
    G4LorentzVector sum(0.,0.,0.,0.);
    G4int gamCount=0;
    nHadr=theQHadrons.size();
    G4bool frag=false;
    if(nHadr>2)for(unsigned f=0; f<theQHadrons.size(); f++) //Check that there's a fragment
    {
      G4int fBN=theQHadrons[f]->GetBaryonNumber(); // Baryon number of the fragment
#ifdef pdebug
      G4cout<<"G4QE::FSI:"<<f<<",PDG="<<theQHadrons[f]->GetPDGCode()<<",fBN="<<fBN<<G4endl;
#endif
      if(fBN>1) frag=true;                    // The fragment with A>1 is found
    }
#ifdef pdebug
    G4cout<<"G4QE::FSI:===Before Gamma Compression===, nH="<<nHadr<<",frag="<<frag<<G4endl;
#endif
	   if(nHadr>2 && frag) for(G4int h=nHadr-1; h>=0; h--)//Collect gammas & kill DecayedHadrs
    {
      G4QHadron* curHadr = theQHadrons[h];    // Get a pointer to the current Hadron
      G4int   hF = curHadr->GetNFragments();
      G4int hPDG = curHadr->GetPDGCode();
      if(hPDG==89999003||hPDG==90002999)
        G4cerr<<"---Warning---G4QEnv::FSI:nD-/pD++(1)="<<hPDG<<G4endl;
#ifdef pdebug
	     G4cout<<"G4QE::FSI: h#"<<h<<", hPDG="<<hPDG<<", hNFrag="<<hF<<G4endl;
#endif
	     if(hF||hPDG==22)                        // It should be compressed
	     {
        G4QHadron* theLast = theQHadrons[theQHadrons.size()-1];//Get Ptr to the Last Hadron
        if(hPDG==22)
        {
          sum+=curHadr->Get4Momentum();       // Add 4Mom of gamma to the "sum"
          gamCount++;
        }
        if(h<static_cast<G4int>(theQHadrons.size())-1) // Need swap with the Last
	       {
          curHadr->SetNFragments(0);
          curHadr->Set4Momentum(theLast->Get4Momentum());
          G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
          if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of LastHadr
          else curHadr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
#ifdef pdebug
	         G4cout<<"G4QE::FSI: Exchange with the last is done"<<G4endl;
#endif
	       }
        theQHadrons.pop_back();               // theLastQHadron is excluded from QHadrons
        delete theLast;//!!When kill,DON'T forget to delete theLastQHadron as an instance!!
        nHadr--;
#ifdef pdebug
	       G4cout<<"G4QE::FSI: The last is compessed"<<G4endl;
#endif
      }
    }
#ifdef pdebug
    G4cout<<"G4QE::FSI: nH="<<nHadr<<"="<<theQHadrons.size()<<", sum="<<sum<<G4endl;
#endif
#ifdef chdebug
    // *** (4) Charge Control Sum Calculation for the Charge Conservation Check ***
    ccContSum=0;                   // Intermediate ChargeControlSum 
    cbContSum=0;                   // Intermediate BaryonNumberControlSum 
    if(nHadr)for(unsigned ic4=0; ic4<nHadr; ic4++) if(!(theQHadrons[ic4]->GetNFragments()))
    {
        ccContSum+=theQHadrons[ic4]->GetCharge();
        cbContSum+=theQHadrons[ic4]->GetBaryonNumber();
	   }
    if(ccContSum-chContSum || cbContSum-bnContSum)
    {
      G4cerr<<"*G4QE::FSI:(4) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
            <<G4endl;
      //throw G4QException("G4QEnvironment::FSInteract: (4) Charge is not conserved");
    }
    // ***
#endif
    if(nHadr>1)for(unsigned hdr=0; hdr<theQHadrons.size()-1; hdr++)//Ord:theBigestIstheLast
    {
#ifdef pdebug
      G4cout<<"G4QE::FSI:ORD,h="<<hdr<<"<"<<nHadr<<",hPDG="<<theQHadrons[hdr]->GetPDGCode()
            <<G4endl;
#endif
      G4QHadron* curHadr = theQHadrons[hdr];  // Get a pointer to the current Hadron
      G4QHadron* theLast = theQHadrons[theQHadrons.size()-1]; //Get Ptr to the Last Hadron
      G4int hB           = curHadr->GetBaryonNumber();
      G4int lB           = theLast->GetBaryonNumber();
#ifdef pdebug
      G4cout<<"G4QE::FSI:hBN="<<hB<<"<lBN="<<lB<<",lstPDG="<<theLast->GetPDGCode()<<G4endl;
#endif
      if(lB<hB)                               // Must be swapped
	     {
        G4QPDGCode   hQPDG = curHadr->GetQPDG();
        G4LorentzVector h4m= curHadr->Get4Momentum();
        curHadr->Set4Momentum(theLast->Get4Momentum());
        G4QPDGCode lQP=theLast->GetQPDG();    // The QPDG of the last
        if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of LastHadr
        else curHadr->SetQC(theLast->GetQC());// CurHadrPDG instead of LastHadrPDG
        theLast->Set4Momentum(h4m);
        theLast->SetQPDG(hQPDG);
	     }
    }
    nHadr=theQHadrons.size();
#ifdef chdebug
    // *** (5) Charge Control Sum Calculation for the Charge Conservation Check ***
    ccContSum=0;                              // Intermediate ChargeControlSum 
    cbContSum=0;                              // Intermediate BaryonNumberControlSum 
    if(nHadr)for(unsigned ic5=0; ic5<nHadr; ic5++) if(!(theQHadrons[ic5]->GetNFragments()))
    {
      ccContSum+=theQHadrons[ic5]->GetCharge();
      cbContSum+=theQHadrons[ic5]->GetBaryonNumber();
    }
    if(ccContSum-chContSum || cbContSum-bnContSum)
    {
      G4cerr<<"*G4QE::FSI:(5) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
            <<G4endl;
      //throw G4QException("G4QEnvironment::FSInteract: (5) Charge is not conserved");
    }
    // ***
#endif
    if(gamCount)
    {
      G4QHadron* theLast = theQHadrons[nHadr-1];// Get a pointer to the Last Hadron
      if(theLast->GetBaryonNumber()>1)        // "Absorb photons & evaporate/decay" case
      {
        G4QHadron* theNew  = new G4QHadron(theLast); // Make New Hadron of the Last Hadron
#ifdef ffdebug
        G4cout<<"G4QE::FSI:BeforeLastSub,n="<<nHadr<<",PDG="<<theNew->GetPDGCode()<<G4endl;
#endif
        theQHadrons.pop_back();               // the last QHadron is excluded from OUTPUT
        delete theLast;//*!When kill,DON'T forget to delete theLastQHadron as an instance!*
        nHadr--;                              // TheLastHadron is only virtually exists now
        G4int newPDG=theNew->GetPDGCode();
        G4LorentzVector new4M=theNew->Get4Momentum(); // 4-mom of the fragment
#ifdef pdebug
        G4cout<<"G4QE::FSI:gSum4M="<<sum<<" is added to "<<new4M<<", PDG="<<newPDG<<G4endl;
#endif
        G4LorentzVector exRes4M=new4M+sum;    //Icrease 4Mom of theLast by "sum of gammas"
        G4QNucleus exResidN(exRes4M,newPDG);
        //G4double mGamEva=2700.;             // @@Threshold for the evaporation
        G4double mGamEva=1700.;               // @@Threshold for the evaporation
	       if(exResidN.SplitBaryon())
	       //if(2>3)                             //CloseTheFirstPriorityResN+gamSumEvaporation
	       {
          theNew->Set4Momentum(exRes4M);      // Icrease 4Mom of theLast by "sum" to Evapor
#ifdef ffdebug
          G4cout<<"G4QE::FSI:BeforeE(1),n="<<nHadr<<",nPDG="<<theNew->GetPDGCode()<<G4endl;
#endif
          EvaporateResidual(theNew);          // Try to evaporate the Nucl.(@@DecDib)(d.e.)
        }
        else if(theNew->GetPDGCode()==90002002&&exRes4M.m()>mHe3+mNeut&&G4UniformRand()>.5)
	       {
          theNew->Set4Momentum(exRes4M);      // Icrease 4Mom of theLast by "sum" to Evapor
          G4LorentzVector n4M(0.,0.,0.,mNeut);
          G4LorentzVector h4M(0.,0.,0.,mHe3);
          if(!theNew->DecayIn2(n4M,h4M))
          {
            G4cerr<<"***G4QE::FSI:GamSup, tM="<<exRes4M.m()<<"<n+He3="<<mNeut+mHe3<<G4endl;
            throw G4QException("***G4QEnvironment::FSInter:GamSUPPRES DecIn2(n+He3)error");
          }
#ifdef ffdebug
          G4cout<<"G4QE::FSI:Gamma Suppression succided, n="<<n4M<<", He3="<<h4M<<G4endl;
#endif
          theNew->Set4Momentum(n4M);
          theNew->SetQPDG(nQPDG);             // convert the alpha to the neutron
          theQHadrons.push_back(theNew);      // (delete equivalent for theHad=neutron)
          G4QHadron* theHe3 = new G4QHadron(90002001,h4M);// Make a New Hadr for the He3
          theQHadrons.push_back(theHe3);      // (delete equivalent for the proton)
        }
	       else if(nHadr)                        // Get LastHadrBefResNuc, absorb gam & decay
	       //else if(2>3)                        // Close the pair absorbtion of gamma
	       {
          if(nHadr>1)for(unsigned sh=0; sh<theQHadrons.size()-1; sh++)//Ord:MinE is TheLast
          {
            G4QHadron* curHadr = theQHadrons[sh];// Get a pointer to the current Hadron
            G4QHadron* thePrev = theQHadrons[theQHadrons.size()-1]; //GetPtr to theLastHadr
            G4LorentzVector h4M= curHadr->Get4Momentum();
            G4LorentzVector l4M= thePrev->Get4Momentum();
#ifdef ffdebug
            G4cout<<"G4QE::FSI:SO,h="<<sh<<"<"<<nHadr<<",PDG/LV="<<curHadr->GetPDGCode()
                  <<h4M<<G4endl;
#endif
            G4double hM=h4M.m();
            G4double hT=h4M.e()-hM;
            G4double lT=l4M.e()-l4M.m();
#ifdef ffdebug
            G4cout<<"G4QE::FSI:hT="<<hT<<"<T="<<lT<<".PDG="<<thePrev->GetPDGCode()<<G4endl;
#endif
            if(hM>mGamEva&&lT>hT)             // Must be swapped as the current is smaller
	           {
              G4QPDGCode   hQPDG = curHadr->GetQPDG();
              curHadr->Set4Momentum(l4M);
              G4QPDGCode lQP=thePrev->GetQPDG();    // The QPDG of the previous
              if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
              else curHadr->SetQC(thePrev->GetQC());// CurHadrPDG instead of PrevHadrPDG
              thePrev->Set4Momentum(h4M);
              thePrev->SetQPDG(hQPDG);
	           }
          }
          nHadr=theQHadrons.size();
          G4QHadron* thePrev = theQHadrons[nHadr-1]; // GetPtr to the BeforeResidNuclHadron
          if(thePrev->Get4Momentum().m()>mGamEva)
          {
            G4QHadron* theHad  = new G4QHadron(thePrev);// MakeNewHadr of theBeforeResNuclH
#ifdef ffdebug
            G4cout<<"G4QE::FSI:BeforeResidNucHadr nH="<<nHadr<<",hPDG="
                  <<theHad->GetPDGCode()<<G4endl;
#endif
            theQHadrons.pop_back();           // theLastQHadron excluded from OUTPUT
            delete thePrev;//*!When kill,DON'T forget to delete theLastQHadr asAnInstance!*
            G4LorentzVector n4M=theNew->Get4Momentum();// 4Mom of theLast (biggest nucleus)
            G4LorentzVector h4M=theHad->Get4Momentum();// 4Mom of the previous Hadron in HV
            G4LorentzVector dh4M=exRes4M+h4M; // 4Mom of LH+PH+sum(gam) for theDecay
            G4double dhM=dh4M.m();            // M of LH+PH+sum(gammas) for theDecay
            if(theHad->GetPDGCode()==90001001&&dhM>n4M.m()+mProt+mNeut&&G4UniformRand()>.5)
		          //if(2>3)                           // Close Possibility toSplitDeuteron
            {
              G4double nuM=n4M.m();
              h4M=G4LorentzVector(0.,0.,0.,mNeut);
              G4LorentzVector p4M(0.,0.,0.,mProt);
              G4double sum=nuM+mNeut+mProt;
              if(fabs(dhM-sum)<eps)
              {
                n4M=dh4M*(nuM/sum);
                h4M=dh4M*(mNeut/sum);
                p4M=dh4M*(mProt/sum);
              }
              else if(dhM<sum || !G4QHadron(dh4M).DecayIn3(n4M,h4M,p4M))
              {
                G4cerr<<"***G4QE::FSI:GamSupByD,M="<<dhM<<"<A+p+n="<<sum<<G4endl;
                throw G4QException("G4QEnviron::FSInt:Gamma SUPPRESSION by D DecIn3error");
              }
#ifdef ffdebug
              G4cout<<"G4QE::FSI:GamSuppression by d succided,h="<<h4M<<",A="<<n4M<<G4endl;
#endif
              theHad->Set4Momentum(h4M);
              theHad->SetQPDG(nQPDG);         // convert the deuteron to the neutron
              theQHadrons.push_back(theHad);  // (delete equivalent for theHad=neutron)
              G4QHadron* theProt = new G4QHadron(90001000,p4M);// Make NewHadr for Proton
              theQHadrons.push_back(theProt); // (delete equivalent for the proton)
              theNew->Set4Momentum(n4M);
              EvaporateResidual(theNew);      // TryToEvaporate theResNuc onceMore(del.eq.)
            }
            else
            {
              if(!G4QHadron(dh4M).DecayIn2(n4M,h4M))
              {
                G4cerr<<"*G4QE::FSI:GamSup,M="<<dh4M.m()<<"<A+h="<<n4M.m()+h4M.m()<<G4endl;
                throw G4QException("G4QEnviron::FSInt:GamSUPPRESSION (3) DecIn2 error");
              }
#ifdef ffdebug
              G4cout<<"G4QE::FSI:Gamma Suppression succided, h="<<h4M<<", A="<<n4M<<G4endl;
#endif
              theHad->Set4Momentum(h4M);
              theQHadrons.push_back(theHad);  // (delete equivalent for theHad)
              theNew->Set4Momentum(n4M);
              EvaporateResidual(theNew);      // Try to evaporate theResNuc (del.eq.)
            }
	         }
          else
	         {
            theNew->Set4Momentum(exRes4M);    // Icrease 4MomOfTheLast by "sum" for Evapor
#ifdef ffdebug
            G4cout<<"G4QE::FSI:BeforE(2),n="<<nHadr<<",PDG="<<theNew->GetPDGCode()<<G4endl;
#endif
            EvaporateResidual(theNew);  // Try to evaporate the Nucl.(@@DecDib)(delete eq.)
          }
        }
	       else                            // Absorb gammas to theResidNucleus and evaporateIt
	       {
          theNew->Set4Momentum(exRes4M);// Icrease 4Mom of the Last by the "sum" for Evap
          EvaporateResidual(theNew);    // Try to evaporate the Nucl.(@@DecDib)(delete eq.)
#ifdef ffdebug
          G4cout<<"G4QE::FSI:Bef.E(3),n="<<nHadr<<",PDG="<<newPDG<<",4M="<<exRes4M<<G4endl;
          unsigned nHN=theQHadrons.size();
          G4cout<<"G4QE::FSI:AfterEvaporation: nNew="<<nHN<<G4endl;
          if(nHN>nHadr)for(unsigned idp=nHadr; idp<nHN; idp++)
		        G4cout<<"G4QE::FSI: h#"<<idp<<", PDG="<<theQHadrons[idp]->GetPDGCode()<<G4endl;
#endif
        }
        //G4int onH=nHadr;
        nHadr=theQHadrons.size();
        //if(nHadr>onH) bfAct=true;
      } // End of "the last is the nucleus" case
    } // End of "There are gammas to assimilate"
  } // End of the While-LOOOP for the Back Fusion
  tot4Mom=tmp4Mom; // Recover tot4Mom after the "En/Mom conservation" reduction
  // Final attempt to alpha-decay the residual nucleus, suppressing the gamma ===========
  G4int gamcnt=0; // Counter of the residual gammas at this level
  nHadr=theQHadrons.size();
  unsigned maxB=nHadr-1;
#ifdef chdebug
  // *** (6) Charge Control Sum Calculation for the Charge Conservation Check ***
  ccContSum=0;                   // Intermediate ChargeControlSum 
  cbContSum=0;                   // Intermediate BaryonNumberControlSum 
  if(nHadr)for(unsigned ic6=0; ic6<nHadr; ic6++) if(!(theQHadrons[ic6]->GetNFragments()))
  {
    ccContSum+=theQHadrons[ic6]->GetCharge();
    cbContSum+=theQHadrons[ic6]->GetBaryonNumber();
  }
  if(ccContSum-chContSum || cbContSum-bnContSum)
  {
    G4cerr<<"*G4QE::FSI:(6) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
          <<G4endl;
    //throw G4QException("G4QEnvironment::FSInteract: (6) Charge is not conserved");
  }
  // ***
#endif
  lHadr=theQHadrons[maxB]->GetBaryonNumber();
  G4int tHadr=lHadr;                             // Total Baryon number
  if(nHadr>1)for(unsigned ipo=0; ipo<theQHadrons.size()-1; ipo++) // Find BiggestNuclFragm
  {
    G4int hPDG = theQHadrons[ipo]->GetPDGCode();
    if(hPDG==22) gamcnt++;
    else
    {
      G4int hBN  = theQHadrons[ipo]->GetBaryonNumber();
      tHadr+=hBN;
#ifdef pdebug
      G4cout<<"G4QE::FSI:h#"<<ipo<<":hPDG="<<hPDG<<",hBN="<<hBN<<",nH="<<theQHadrons.size()
            <<G4endl;
#endif
      if(hBN>lHadr)
      {
        lHadr=hBN;
        maxB=ipo;
      }                                           // the current biggest nuclear fragment
	   }
  }
#ifdef pdebug
  G4cout<<"G4QE::FSI:max#"<<maxB<<",lB="<<lHadr<<",tBN="<<tHadr<<",gam="<<gamcnt<<G4endl;
#endif
  nHadr=theQHadrons.size();
#ifdef chdebug
  // *** (7) Charge Control Sum Calculation for the Charge Conservation Check ***
  ccContSum=0;                   // Intermediate ChargeControlSum
  cbContSum=0;                   // Intermediate BaryonNumberControlSum
  if(nHadr)for(unsigned ic7=0; ic7<nHadr; ic7++) if(!(theQHadrons[ic7]->GetNFragments()))
  {
    ccContSum+=theQHadrons[ic7]->GetCharge();
    cbContSum+=theQHadrons[ic7]->GetBaryonNumber();
  }
  if(ccContSum-chContSum || cbContSum-bnContSum)
  {
    G4cerr<<"*G4QE::FSI:(7) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
          <<G4endl;
    //throw G4QException("G4QEnvironment::FSInteract: (7) Charge is not conserved");
  }
  // ***
#endif
  if(gamcnt&&tHadr>1)                           // Only if there are gammas one should act
  {
    if(maxB+1<nHadr)                            // If maxB<Last, swap theCurH and theLastH
    {
      G4QHadron* theCurr = theQHadrons[maxB];   // Pointer to the Current Hadron
      G4QHadron* theLast = theQHadrons[nHadr-1];// Pointer to the Last Hadron
      G4QHadron* curHadr = new G4QHadron(theCurr);//Remember theCurrentHadron to put on top
      G4QPDGCode lQP=theLast->GetQPDG();        // The QPDG of the last
      if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of PrevHadr
      else theCurr->SetQC(theLast->GetQC());    // CurHadrPDG instead of LastHadrPDG
      theCurr->Set4Momentum(theLast->Get4Momentum()); // ... continue substitution
      theQHadrons.pop_back();                   // Rnt to theLastHadron is excluded from HV
      delete theLast;//*!!When kill,DON'T forget to delete the last QHadron as an inst. !!*
      theQHadrons.push_back(curHadr);           // The current Hadron, which is the Biggest
    }
    nHadr=theQHadrons.size();                   // Must be the same
    // Now it is necessary to absorb the photon (photons) and try to radiate alpha or evap.
    G4LorentzVector gamSum(0.,0.,0.,0.);
    if(nHadr>1)for(unsigned gp=0; gp<nHadr-1; gp++)// Find Gumma, remember and kill
    {
      G4QHadron* theCurr = theQHadrons[gp];       // Pointer to the Current Hadron
      G4int hPDG=theCurr->GetPDGCode();
#ifdef pdebug
      G4cout<<"G4QE::FSI:gp#"<<gp<<", PDG="<<hPDG<<", is found"<<G4endl;
#endif
      if(hPDG==22)                                // Photon is foun on the "gp" position
	     {
        gamSum=gamSum+theCurr->Get4Momentum();    // Accumulate the 4Momenta of the photon
        unsigned nLast=nHadr-1;                   // position of theLastHadron (gp<nHadr-1)
        G4QHadron* theLast = theQHadrons[nLast];  // Pointer to the Last Hadron
        while(nLast>gp && theLast->GetPDGCode()==22) // "TheLast is a photon too" LOOP
								{
          gamSum=gamSum+theLast->Get4Momentum();  // Accumulate 4-momentum of theLastPhoton
          theQHadrons.pop_back();                 // Pnt to theLastHadr.is excluded from HV
          delete theLast;//*!!When kill,DON'T forget to delete theLastQHadron as an inst!!*
          nHadr=theQHadrons.size();
          nLast=nHadr-1;
          theLast = theQHadrons[nLast];
        }
        if(nLast>gp)
        {
          G4QPDGCode lQP=theLast->GetQPDG();      // The QPDG of the last
          if(lQP.GetPDGCode()!=10) theCurr->SetQPDG(lQP); //CurHadr instead of PrevHadr
          else theCurr->SetQC(theLast->GetQC());  // CurHadrPDG instead of LastHadrPDG
          theCurr->Set4Momentum(theLast->Get4Momentum()); // ... continue substitution
          theQHadrons.pop_back();                 // Pnt to theLastHadr.is excluded from HV
          delete theLast;//*!|When kill,DON'T forget to delete theLastQHadron as an inst!!*
          nHadr=theQHadrons.size();
#ifdef pdebug
          G4cout<<"G4QE::FSI:RepBy lPDG="<<lQP<<",nH="<<nHadr<<",gS="<<gamSum<<G4endl;
#endif
        }
	     }
    }
    // @@ Now it is necessary to try to emit alpha or evaporate the residual nucleus
    G4QHadron* theLast = theQHadrons[nHadr-1];   // Pointer to the Last Hadron
    if(theLast->GetPDGCode()==22)
	   {
      gamSum=gamSum+theLast->Get4Momentum();     // Accumulate 4-momentum of the LastPhoton
      theQHadrons.pop_back();                    // Pnt to theLastHadr.is excluded from HV
      delete theLast; //**!!When kill,DON'T forget to delete theLastQHadron as an inst.!!**
      nHadr=theQHadrons.size();
#ifdef pdebug
      G4cout<<"G4QE::FSI: The last photon is killed, nH="<<nHadr<<",gS="<<gamSum<<G4endl;
#endif
      theLast = theQHadrons[nHadr-1];
	   }
    G4int nEx=nHadr-2;                           // position to be exchanged with theLast
    while(theLast->GetBaryonNumber()<1 && nEx>=0)// theLastHadron must be a nucleus (A>0)
				{
      G4QHadron* theEx=theQHadrons[nEx];         // A hadron to be exchanged with theLast
      G4LorentzVector ex4Mom=theEx->Get4Momentum();
      G4QPDGCode exQPDG=theEx->GetQPDG();
      G4QContent exQC=theEx->GetQC();
      G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
      if(lQP.GetPDGCode()!=10) theEx->SetQPDG(lQP); //CurHadr instead of PrevHadr
      else theEx->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
      theEx->Set4Momentum(theLast->Get4Momentum());
      if(exQPDG.GetPDGCode()!=10) theLast->SetQPDG(exQPDG);
      else theLast->SetQC(exQC);                 // CurHadrPDG instead of LastHadrPDG
      theLast->Set4Momentum(ex4Mom);
      nEx--;
    }
    G4QHadron* curHadr = new G4QHadron(theLast); // Pnt to theCurrentHadron is theLastCopy
    theQHadrons.pop_back();                 // Pnt to theLastHadron is excluded from OUTPUT
    delete theLast;//*!! When kill,DON'T forget to delete theLas QHadron as an instance !!*
    G4int theLB= curHadr->GetBaryonNumber();
    G4LorentzVector tR4M=curHadr->Get4Momentum()+gamSum;
    G4double tRM=tR4M.m();                       // TotMass of theResidualNucleus to decay
    if(theLB>4)
	   {
	     G4QContent lrQC=curHadr->GetQC()-G4QContent(6,6,0,0,0,0);
      G4QNucleus lrN(lrQC);
      G4double lrM=lrN.GetMZNS();
      if(tRM>lrM+mAlph)
	     {
        G4LorentzVector lr4M(0.,0.,0.,lrM);
        G4LorentzVector al4M(0.,0.,0.,mAlph);
        if(!G4QHadron(tR4M).DecayIn2(lr4M,al4M))
        {
          curHadr->Set4Momentum(tR4M);
          EvaporateResidual(curHadr); // delete equivalent
#ifdef fdebug
          G4cout<<"G4QE::FSI: After Evap (1) nH="<<theQHadrons.size()<<G4endl;
#endif
	       }
        else
        {
		        delete curHadr;
          G4int APDG=lrN.GetPDG();
#ifdef pdebug
          G4cout<<"G4QE::FSI: Final A+alpha, A="<<APDG<<lr4M<<", a="<<al4M<<G4endl;
#endif
          G4QHadron* lrH = new G4QHadron(APDG,lr4M);
          theQHadrons.push_back(lrH);      // (delete equivalent for lrH)
          G4QHadron* alH = new G4QHadron(90002002,al4M);
          theQHadrons.push_back(alH);      // (delete equivalent for alH)
        }
      }
      else
      {
        curHadr->Set4Momentum(tR4M);
        EvaporateResidual(curHadr); // delete equivalent
#ifdef fdebug
        G4cout<<"G4QE::FSI: After Evap (2) nH="<<theQHadrons.size()<<G4endl;
#endif
	     }
    }
    else
    {
      curHadr->Set4Momentum(tR4M);
      EvaporateResidual(curHadr); // delete equivalent
#ifdef fdebug
      G4cout<<"G4QE::FSI: After Evap (5) nH="<<theQHadrons.size()<<G4endl;
#endif
	   }
  }
  //Now just fill the output theFravment vector (User is responsible to ClearAndDestroy it)
  nHadr=theQHadrons.size();
#ifdef chdebug
  // *** (8) Charge Control Sum Calculation for the Charge Conservation Check ***
  ccContSum=0;                   // Intermediate ChargeControlSum 
  cbContSum=0;                   // Intermediate BaryonNumberControlSum
  if(nHadr)for(unsigned ic8=0; ic8<nHadr; ic8++) if(!(theQHadrons[ic8]->GetNFragments()))
  {
    ccContSum+=theQHadrons[ic8]->GetCharge();
    cbContSum+=theQHadrons[ic8]->GetBaryonNumber();
  }
  if(ccContSum-chContSum || cbContSum-bnContSum)
  {
    G4cerr<<"*G4QE::FSI:(8) dC="<<ccContSum-chContSum<<",dB="<<cbContSum-bnContSum
          <<G4endl;
    //throw G4QException("G4QEnvironment::FSInteract: (8) Charge is not conserved");
  }
  // ***
#endif
  if(nHadr) for(unsigned hd=0; hd<theQHadrons.size(); hd++)
  {
    //G4QHadron* curHadr = new G4QHadron(theQHadrons[hd]);
    G4QHadron* curHadr = theQHadrons[hd];
    G4int hPDG=curHadr->GetPDGCode();
    if(hPDG==22 && fabs(curHadr->Get4Momentum().e())<.00001) // E=0, gamma in the OUTPUT
    {
      unsigned lin=theQHadrons.size()-1;
      G4QHadron* theLast = theQHadrons[lin];// Pointer to theLastHadron in theQHadrVector
      if(lin>hd)
      {
        G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
        if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
        else curHadr->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
        curHadr->Set4Momentum(theLast->Get4Momentum()); // ... continue substitution (4Mom)
      }
      //else break; // It's not necessary to delete: not copy to theFragments is enough
      delete theLast;//*!!When kill, DON'T forget to delete theLastQHadron asAnInstance!!
      theQHadrons.pop_back();              //pointer to theLastHadron is excluded from HV
      hPDG=curHadr->GetPDGCode();          // Redefine thePDGCode of theCurrentHadron
      if(lin==hd) break;
    }
#ifdef fdebug
    G4cout<<"G4QE::FSI: LOOP starts nH="<<nHadr<<", h#"<<hd<<", PDG="<<hPDG<<G4endl;
#endif
    if(hPDG==89999003||hPDG==90002999) G4cout<<"G4QEnv::FSI:nD-/pD++(0)="<<hPDG<<G4endl;
    if(hPDG==89999004||hPDG==90003999) G4cout<<"G4QEnv::FSI:nnD-/ppD++(0)="<<hPDG<<G4endl;
    //if(hPDG==89998003||hPDG==90002998)G4cout<<"G4QE::FSI:D-Pi-/D++Pi+PDG="<<hPDG<<G4endl;
    if(hPDG>100000000) G4cout<<"***G4QE::FSI: h#"<<hd<<", wrong PDGCode="<<hPDG<<G4endl;
#ifdef fdebug
    G4cout<<"G4QE::FSI:Copy is made with PDG="<<hPDG<<G4endl;//To compare with Original
#endif
    G4int hBN=curHadr->GetBaryonNumber();
    G4int hCG=curHadr->GetCharge();
    G4int hST=curHadr->GetStrangeness();
    G4int hNF=curHadr->GetNFragments();
    G4bool fOK=true;
    if(hNF==-1) curHadr->SetNFragments(0);
    else if(hPDG==91000000) curHadr->SetQPDG(G4QPDGCode(3122));// Move to theNextLoop
    else if(hPDG==90998003||hPDG==91002998)    // n,pi-,Sigma- OR p,pi+,Sigma+ (StrangeIso)
    {                                          // @@ can be converted here to B>2
#ifdef fdebug
      G4cout<<"G4QE::FSI: Pi+Nuc+Sigma state decay is found PDG="<<hPDG<<G4endl;
#endif
      G4LorentzVector r4M=curHadr->Get4Momentum(); // Real 4-mom of theIsoNucleus
      G4double reM=r4M.m();                   // Real mass of the IsoNucleus
      G4bool dub=false;
      G4int PDGnu=2112;
      G4double mNucl=mNeut;
      G4int PDGpi=-211;
      G4double mPion=mPi;
      G4int PDGsi=3112;
      G4double mSi=mSigM;
      G4double sum=mNucl+mPion+mSi;
      if(hPDG==90998003&&reM<sum)                      // Default -- values
	     {
        PDGsi=2112;
        mSi=mNeut;
        mPion=mPi+mPi;
        sum=mNucl+mPion+mSi;
        dub=true;
      }
      if(hPDG==91002998)                      // Change -- default values to +++ values
	     {
        mNucl=mProt;
        PDGnu=2212;
        PDGpi=211;
        PDGsi=3222;
        mSi  =mSigP;
        sum=mNucl+mPion+mSi;
        if(reM<sum)
	       {
          PDGsi=2212;
          mSi=mProt;
          sum=mNucl+mPion+mSi;
        }
      }
      G4LorentzVector n4M(0.,0.,0.,mNucl);
      G4LorentzVector p4M(0.,0.,0.,mPion);
      G4LorentzVector k4M(0.,0.,0.,mSi);
      if(fabs(reM-sum)<eps)
	     {
        //G4cout<<"G4QE::FSI:*TMP* PiDelta split PDG="<<hPDG<<G4endl;//To find out why?
        n4M=r4M*(mNucl/sum);
        p4M=r4M*(mPion/sum);
        k4M=r4M*(mSi/sum);
      }
      else if(reM<sum || !G4QHadron(r4M).DecayIn3(n4M,p4M,k4M))
      {
        G4cerr<<"---Warning---G4QE::FSI:Pi+N+Sigma recovery INPDG="<<hPDG<<","<<reM<<" < "
              <<mNucl<<"(PDG="<<PDGnu<<")+Pi="<<mPion<<")+Sigma="<<mSi<<"="<<sum<<G4endl;
        if(!theEnvironment.GetA())
        {
          G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
          G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
          if(hd+1<theQHadrons.size())            // If ipo<Last, swap CurHadr & LastHadr
          {
            theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr(ipo<Last)
            G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
            if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
            else curHadr->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
            curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
          delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
          G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
          if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
          {
            qH->SetNFragments(-1);              //@@ To avoid looping
            G4cerr<<"---Warning---G4QE::FSI:PiNSig Failed, LeaveAsItIs 4m="<<r4M<<G4endl;
            theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
          }
          else
          {
#ifdef fdebug
            for(unsigned hp=0; hp<theQHadrons.size(); hp++)
            {
              G4QHadron* cpHadr = new G4QHadron(theQHadrons[hp]);
              G4int hpPDG=cpHadr->GetPDGCode();
              G4LorentzVector hpLV=cpHadr->Get4Momentum();
		      G4cout<<"G4QE::FSI:h#"<<hp<<": hPDG="<<hpPDG<<", h4M="<<hpLV<<G4endl;
            }
#endif
            delete qH;
            nHadr=theQHadrons.size();
          }
          delete quasH;
          fOK=false;
		      }
        else
        {
          G4cerr<<"**G4QEnv::FSI: No Final PiNSig recovery, Env="<<theEnvironment<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter:PiNucSigma Final Decay Error");
        }
#ifdef fdebug
        G4cout<<"G4QEnv::FSI: PiNSi recover #"<<hd<<",PDG="<<curHadr->GetPDGCode()<<G4endl;
#endif
      }
      if(fOK)
      {
#ifdef fdebug
        G4cout<<"G4QE::FSI:PiNSigma==>"<<r4M<<"->N="<<PDGnu<<n4M<<"+Pi="<<PDGpi<<p4M
              <<"+ Sigma="<<PDGsi<<k4M<<G4endl;
#endif
        curHadr->Set4Momentum(n4M);
        curHadr->SetQPDG(G4QPDGCode(PDGnu));// At least one nucleun always exists
        if(dub)
		      {
          p4M/=2.;
          G4QHadron* Pid = new G4QHadron(PDGpi,p4M); // Create a Hadron for the pion
          theQHadrons.push_back(Pid);       // Fill pion (delete equivalent)
        }
        G4QHadron* Pi = new G4QHadron(PDGpi,p4M); // Create a Hadron for the pion
        theQHadrons.push_back(Pi);          // Fill pion (delete equivalent)
        G4QHadron* Si = new G4QHadron(PDGsi,k4M); // Create a Hadron for the Sigma
        theQHadrons.push_back(Si);          // Fill Sigma (delete equivalent)
      }
#ifdef fdebug
      G4cout<<"G4QE::FSI:*TMP* PiNSigma end up PDG="<<hPDG<<G4endl;//To find out why?
#endif
    }
    else if(hPDG==89998003||hPDG==90002998)   // Isonucleus (pi- + DEL- or pi+ + DEL++)
	   {                                         // @@ can be converted here to B>1
      //G4cout<<"G4QE::FSI:*TMP* PiDelta decay PDG="<<hPDG<<G4endl;//To find out why?
      G4double mNucl=mNeut;
      G4int PDGnu=2112;
      G4int PDGpi=-211;
      if(hPDG==90002998)                      // Change DEL- default values to DEL++ values
	     {
        mNucl=mProt;
        PDGnu=2212;
        PDGpi=211;
      }
      //G4double nucM=mNucl*hBN;
      G4LorentzVector r4M=curHadr->Get4Momentum(); // Real 4-mom of theIsoNucleus
      G4double reM=r4M.m();                   // Real mass of the IsoNucleus
      G4LorentzVector n4M(0.,0.,0.,mNucl);
      G4LorentzVector p4M(0.,0.,0.,mPi);
      G4LorentzVector k4M(0.,0.,0.,mPi);
      G4double sum=mNucl+mPi+mPi;
      if(fabs(reM-sum)<eps)
	     {
        //G4cout<<"G4QE::FSI:*TMP* PiDelta split PDG="<<hPDG<<G4endl;//To find out why?
        n4M=r4M*(mNucl/sum);
        p4M=r4M*(mPi/sum);
        k4M=r4M*(mPi/sum);
      }
      else if(reM<sum || !G4QHadron(r4M).DecayIn3(n4M,p4M,k4M))
      {
        G4cerr<<"---Warning---G4QE::FSI: Isonuc+Pi recovery INPDG="<<hPDG<<","<<reM<<" < "
              <<mNucl<<"(PDG="<<PDGnu<<") + 2*"<<mPi<<"="<<sum<<G4endl;
        if(!theEnvironment.GetA())
        {
          G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
          G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
          if(hd+1<theQHadrons.size())            // If ipo<Last, swap CurHadr & LastHadr
          {
            theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr(ipo<Last)
            G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
            if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
            else curHadr->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
            curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
          delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
          G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
          if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
          {
            qH->SetNFragments(-1);              //@@ To avoid looping
            G4cerr<<"---Warning---G4QE::FSI:IsoN+Pi Failed, LeaveAsItIs 4m="<<r4M<<G4endl;
            theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
          }
          else
          {
            delete qH;
            nHadr=theQHadrons.size();
          }
          delete quasH;
          fOK=false;
		      }
        else
        {
          G4cerr<<"**G4QEnv::FSI: No Final IsoN+Pi recovery, Env="<<theEnvironment<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter:IsoNucl+Pi FinalDecayError");
        }
#ifdef fdebug
        G4cout<<"G4QEnv::FSI: PiDel recover #"<<hd<<",PDG="<<curHadr->GetPDGCode()<<G4endl;
#endif
      }
      if(fOK)
      {
#ifdef fdebug
        G4cout<<"G4QE::FSI:IsoNuc+Pi==>"<<r4M<<"->N="<<PDGnu<<n4M<<"+Pi="<<PDGpi<<p4M
              <<"+ Pi="<<PDGpi<<k4M<<G4endl;
#endif
        curHadr->Set4Momentum(n4M);
        curHadr->SetQPDG(G4QPDGCode(PDGnu)); // At least one nucleun always exists
        G4QHadron* Pi1 = new G4QHadron(PDGpi,p4M); // Create a Hadron for the pion
        theQHadrons.push_back(Pi1);          // Fill pion (delete equivalent)
        G4QHadron* Pi2 = new G4QHadron(PDGpi,k4M); // Create a Hadron for the pion
        theQHadrons.push_back(Pi2);          // Fill pion (delete equivalent)
      }
#ifdef fdebug
      G4cout<<"G4QE::FSI:*TMP* PiDelta end up PDG="<<hPDG<<G4endl;//To find out why?
#endif
    }
    else if(hBN>0 && !hST && (hCG<0||hCG>hBN)) // Isonucleus (n*N+k*DEL- or n*P+k*DEL++)
	   {
      G4double mNucl=mNeut;
      G4int PDGnu=2112;
      G4int PDGpi=-211;
      G4int nPi=-hCG;                         // Prototype of the minimum number of pions
      if(hCG>0)                               // Change DEL- default values to DEL++ values
	     {
        mNucl=mProt;
        PDGnu=2212;
        PDGpi=211;
        nPi=hCG-hBN;
      }
      G4double nucM=mNucl*hBN;
      G4double pioM=mPi*nPi;
      G4LorentzVector r4M=curHadr->Get4Momentum(); // Real 4-mom of theIsoNucleus
      G4double reM=r4M.m();                   // Real mass of the IsoNucleus
      G4LorentzVector n4M(0.,0.,0.,nucM);
      G4LorentzVector k4M(0.,0.,0.,pioM);
      G4double sum=nucM+pioM;
      if(fabs(reM-sum)<eps)
	     {
        n4M=r4M*(nucM/sum);
        k4M=r4M*(pioM/sum);
      }
      else if(reM<sum || !G4QHadron(r4M).DecayIn2(n4M,k4M))
      {
#ifdef fdebug
        G4cerr<<"---Warning---G4QE::FSI: Isonucleus recovery INPDG="<<hPDG<<", M="<<reM
              <<" < "<<nucM<<"+"<<pioM<<"="<<sum<<G4endl;
#endif
        if(!theEnvironment.GetA())
        {
          G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
          G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
          G4QContent tQC=qH->GetQC();            // Quark content of the hadron
          G4LorentzVector t4M=qH->Get4Momentum();// 4Momentum of the hadron
          if(hd+1<theQHadrons.size())            // If ipo<Last, swap CurHadr & LastHadr
          {
            theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr(ipo<Last)
            G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
            if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
            else curHadr->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
            curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
          delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
          G4Quasmon* quasH = new G4Quasmon(tQC,t4M); // Fake Quasmon for the Recovery
          if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
          {
            G4int tPDG=qH->GetPDGCode();
            qH->SetNFragments(-1);              // @@ To avoid looping
            G4cerr<<"---Warning---G4QE::FSI:IsoN="<<tPDG<<tQC<<" FAsIs 4m="<<t4M<<G4endl;
            theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
          }
          else
          {
#ifdef fdebug
            for(unsigned hp=0; hp<theQHadrons.size(); hp++)
            {
              G4QHadron* cpHadr = new G4QHadron(theQHadrons[hp]);
              G4int hpPDG=cpHadr->GetPDGCode();
              G4LorentzVector hpLV=cpHadr->Get4Momentum();
		            G4cout<<"G4QE::FSI:h#"<<hp<<": hPDG="<<hpPDG<<", h4M="<<hpLV<<G4endl;
            }
#endif
            delete qH;                           // Temporary Hadron is used for recovery
            nHadr=theQHadrons.size();
          }
          delete quasH;                          // TemporaryQuasmon is used for recovery
          fOK=false;
		      }
        else
        {
          G4cerr<<"***G4QEnv::FSI: No FinalIsoNucRecovery, Env="<<theEnvironment<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter:IsoNucleus FinalDecayError");
        }
#ifdef fdebug
        G4cout<<"G4QEnv::FSI: Isonucleus recovery outPDG="<<curHadr->GetPDGCode()<<G4endl;
#endif
      }
      if(fOK)
      {
#ifdef fdebug
        G4cout<<"G4QE::FSI:IsoN==>"<<r4M<<"->N="<<PDGnu<<n4M<<"+Pi="<<PDGpi<<k4M<<G4endl;
#endif
		      if(hBN>1) n4M/=hBN;                    // Calculate the 4mom per one nucleon
        curHadr->Set4Momentum(n4M);
        curHadr->SetQPDG(G4QPDGCode(PDGnu));   // At least one nucleun always exists
        if(hBN>1) for(G4int ih=1; ih<hBN; ih++)
        {
          G4QHadron* Hi = new G4QHadron(PDGnu,n4M); // Create a Hadron for the baryon
          theQHadrons.push_back(Hi);    // (del.eq.- user is responsible for del)
          //theFragments->push_back(Hi);        // Fill nucleons (delete equivalent)
        }
        if(nPi>1) k4M/=nPi;
        for(G4int ip=0; ip<nPi; ip++)
        {
          G4QHadron* Hj = new G4QHadron(PDGpi,k4M); // Create a Hadron for the pion
          theQHadrons.push_back(Hj);          // Fill pion (delete equivalent)
        }
      }
#ifdef fdebug
      G4cout<<"G4QEnv::FSI: Isonucleus decay result h#="<<hd<<", outPDG="<<hPDG<<G4endl;
#endif
    }
	   else if(hBN>1 && (hBN==hCG&&!hST || !hCG&&!hST || !hCG&&hST==hBN)) //(n*P, n*N, or n*L)
	   {
      // *** Temporary Correction *** (To find out where is the error)
      if(hPDG==90000003 && fabs(curHadr->Get4Momentum().m()-mNeut-mNeut)<.1)
	     {
        hPDG=90000002;
        hBN=2;
        G4cout<<"--Warning--G4QEnv::FSI:3->2 neutrons conversion (***Check it***)"<<G4endl;
      }
      // End of the Temporary Correction
      if     (!hCG&&!hST)     hPDG=90000001;
      else if(hBN==hCG&&!hST) hPDG=90001000;
      else if(!hCG&&hST==hBN) hPDG=91000000;
      else throw G4QException("***G4QEnvironment::FSInteract: MultyDibaryon cant be here");
      G4LorentzVector newLV=(curHadr->Get4Momentum())/hBN;
      curHadr->Set4Momentum(newLV);
      curHadr->SetQPDG(G4QPDGCode(hPDG));
      for(G4int bd=1; bd<hBN; bd++)
      {
        G4QHadron* secHadr = new G4QHadron(curHadr);
        theQHadrons.push_back(secHadr);    // (del.eq.- user is responsible for del)
        //theFragments->push_back(secHadr);// (del.equiv. - user is responsible for that)
      }
    }
    else if(hST<0 && hBN>0) // AntistrangeNucleus (@@ see above, already done)
	   {
      G4LorentzVector r4M=curHadr->Get4Momentum(); // Real 4-mom of theAntiStrangeNucleus
      G4double reM=r4M.m();              // Real mass of the antiStrange nucleus
      G4QContent nQC=curHadr->GetQC();   // QCont of the Antistrange nucleus
      G4QNucleus  newN0(nQC-K0QC);
      G4int  RPDG=newN0.GetPDG();
      G4double mR=newN0.GetMZNS();       // Residual mass for K0
      G4double mKaon=mK0;                // Prototype is mass of K0
      G4int    kPDG =311;                // Prototype is PDG of K0
      G4QNucleus  newNp(nQC-KpQC);
      G4double mp=newNp.GetMZNS();       // Residual mass for K+
      if(mp+mK<mR+mK0)                   // Select K+ as the minimum mass of products
      {
        mR=mp;
        RPDG=newNp.GetPDG();
        mKaon=mK;
        kPDG=321;
      }
      G4double sum=mR+mKaon;
      if(sum>reM)                        // for GEANT4 (Can not decay in kaon and residual)
	     {
        if(kPDG==321)                    // *** Very seldom *** "K+->pi+ conversion"
		      {
          kPDG=211;
          mKaon=mPi;
		      }
        else                             // *** Very seldom *** "K0(S=-1)->pi- conversion"
		      {
          kPDG=111;
          mKaon=mPi0;
		      }
        sum=mR+mKaon;
      }
      G4LorentzVector n4M(0.,0.,0.,mR);
      G4LorentzVector k4M(0.,0.,0.,mKaon);
      if(fabs(reM-sum)<eps)
	     {
        n4M=r4M*(mR/sum);
        k4M=r4M*(mKaon/sum);
      }
      else if(reM<sum || !G4QHadron(r4M).DecayIn2(n4M,k4M))
      {
        G4cerr<<"---Warning---G4QE::FSI: Try to recover ASN="<<hPDG<<","<<reM<<"<"<<mR<<"+"
              <<mKaon<<"="<<sum<<G4endl;
        if(!theEnvironment.GetA())
        {
          G4QHadron* theLast = curHadr;          // Prototype of Pointer to theLastHadron
          G4QHadron* qH = new G4QHadron(curHadr);// Copy of the Current Hadron
          if(hd+1<theQHadrons.size())           // If ipo<Last, swap CurHadr & LastHadr
          {
            theLast = theQHadrons[theQHadrons.size()-1]; // Pointer to LastHadr(ipo<Last)
            G4QPDGCode lQP=theLast->GetQPDG();         // The QPDG of the last
            if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP); //CurHadr instead of PrevHadr
            else curHadr->SetQC(theLast->GetQC());       // CurHadrPDG instead of LastHadrPDG
            curHadr->Set4Momentum(theLast->Get4Momentum()); // ... 4Momentum substitution
          }
          theQHadrons.pop_back();        // exclude theLastHadron pointer from the OUTPUT
          delete theLast; //*!! When killing, DON'T forget to delete the last QHadron !!*
          G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());//Fake Quasmon
          if(!CheckGroundState(quasH,true))      // Try to correct by other hadrons
          {
            qH->SetNFragments(-1);              //@@ To avoid looping
            G4cerr<<"---Warning---G4QE::FSI:AntiStraN Failed LeaveAsItIs 4m="<<r4M<<G4endl;
            theQHadrons.push_back(qH);           // Leave as it is (delete equivalent)
          }
          else
          {
            delete qH;
            nHadr=theQHadrons.size();
          }
          delete quasH;
          fOK=false;
		      }
        else
        {
          G4cerr<<"***G4QEnv::FSI: No Final AntiSN recovery, E="<<theEnvironment<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter:AntistrangeNucleus decayError");
        }
      }
      if(fOK)
      {
#ifdef fdebug
        G4cout<<"G4QE::FSI:AntiSN==>"<<r4M<<"->A="<<RPDG<<n4M<<"+K="<<kPDG<<k4M<<G4endl;
#endif
        curHadr->Set4Momentum(k4M);                 // Kaon
        curHadr->SetQPDG(G4QPDGCode(kPDG));
        G4QHadron* theRes = new G4QHadron(RPDG,n4M);// Make a New Hadr for the residual
        EvaporateResidual(theRes);                  // Try to evaporate theResNuc (del.eq.)
      }
    }
    else if(hPDG>90500000&&hPDG!=91000000&&hPDG!=91000999&&hPDG!=90999001) // Only for G4
	// @@ Xi & Omega- can be added here ^^^, if show up below
	   {
#ifdef pdebug
      G4cerr<<"***G4QEnvironment::FSI:*G4* Hypernucleus PDG="<<hPDG<<" must decay"<<G4endl;
#endif
      G4int nL=curHadr->GetStrangeness();      // A number of Lambdas in the Hypernucleus
      G4int nB=curHadr->GetBaryonNumber();     // Total Baryon Number of the Hypernucleus
      G4int nC=curHadr->GetCharge();           // Charge of the Hypernucleus
      G4int nSM=0;                             // A#0f unavoidable Sigma-
      G4int nSP=0;                             // A#0f unavoidable Sigma+
      if(nC<0)                                 // Negative hypernucleus
	     {
        if(-nC<=nL)
		      {
          nSM=-nC;                             // Can be compensated by Sigma-
          nL+=nC;                              // Reduce the residual strangeness
        }
        else
		      {
          nSM=nL;                              // The maximum number of Sigma-
          nL=0;                                // Kill the residual strangeness
        }
      }
      else if(nC>nB-nL)                        // Extra positive hypernucleus
	     {
        if(nC<=nB)
		      {
          G4int dH=nB-nC;
          nSP=nL-dH;                           // Can be compensated by Sigma+
          nL=dH;                               // Reduce the residual strangeness
        }
        else
		      {
          nSP=nL;                              // The maximum number of Sigma+
          nL=0;                                // Kill the residual strangeness
        }
      }
      G4LorentzVector r4M=curHadr->Get4Momentum(); // Real 4-momentum of the hypernucleus
      G4double reM=r4M.m();                    // Real mass of the hypernucleus
      G4int rlPDG = hPDG-nL*1000000-nSP*1000999-nSM*999001;// Subtract Lamb/Sig from Nucl
      G4int    sPDG=3122;                      // Prototype for the Hyperon PDG (Lambda)
      G4double MLa=mLamb;                      // Prototype for one Hyperon decay
#ifdef pdebug
      G4cerr<<"G4QEnvironment::FSI:*G4* nS+="<<nSP<<", nS-="<<nSM<<", nL="<<nL<<G4endl;
#endif
      if(nSP||nSM)
	     {
        if(nL)
        {
          G4cerr<<"***G4QE::FSI: HypNPDG="<<hPDG<<": both Sigm&Lamb -> Improve it"<<G4endl;
          //throw G4QException("*G4QEnvironment::FSInter: Both Lambda&Sigma in Hypernucl");
          // @@ Correction, which does not conserv the charge !! (-> add decay in 3)
          if(nSP) nL+=nSP;
          else    nL+=nSM;
        }
        if(nSP)
		      {
          nL=nSP;
          sPDG=3222;
          MLa=mSigP;
        }
        else
		      {
          nL=nSM;
          sPDG=3112;
          MLa=mSigM;
        }
      }
#ifdef pdebug
      G4cerr<<"G4QEnvironment::FSI:*G4* mS="<<MLa<<", sPDG="<<sPDG<<", nL="<<nL<<G4endl;
#endif
      if(nL>1) MLa*=nL;
      G4double rlM=G4QNucleus(rlPDG).GetMZNS();// Mass of the new residualNonstrangeNucleus
      if(!nSP&&!nSM&&nL==1&&reM>rlM+mSigZ&&G4UniformRand()>.5) // Sigma0 @@ AddToHadroniz.?
	     {
        sPDG=3212;
        MLa=mSigZ;
      }
      G4int rnPDG = hPDG-nL*999999;            // Convert Lambdas to neutrons (for convInN)
      G4QNucleus rnN(rnPDG);                   // New nonstrange nucleus
      G4double rnM=rnN.GetMZNS();              // Mass of the new nonstrange nucleus
      // @@ Take into account that there can be Iso-Hypernucleus (Add PI+,R & Pi-,R decays)
      if(rlPDG==90000000)                      // Multy Hyperon (HyperNuc of only hyperons)
	     {
        if(nL>1) r4M=r4M/nL;                   // split the 4-mom for the MultyLambda
        for(G4int il=0; il<nL; il++)           // loop over Lambdas
        {
          G4QHadron* theLam = new G4QHadron(sPDG,r4M);// Make a New Hadr for the Hyperon
          theQHadrons.push_back(theLam);       // (del.eq.- user is responsible for del)
        }
      }
	     else if(reM>rlM+MLa-eps)                  // Lambda(or Sigma) can be split
	     {
        G4LorentzVector n4M(0.,0.,0.,rlM);
        G4LorentzVector h4M(0.,0.,0.,MLa);
        G4double sum=rlM+MLa;
        if(fabs(reM-sum)<eps)
	       {
          n4M=r4M*(rlM/sum);
          h4M=r4M*(MLa/sum);
        }
        else if(reM<sum || !G4QHadron(r4M).DecayIn2(n4M,h4M))
        {
          G4cerr<<"***G4QE::FSI:Hypern, M="<<reM<<"<A+n*L="<<sum<<",d="<<sum-reM<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter: Hypernuclear L-decay error");
        }
#ifdef pdebug
        G4cout<<"*G4QE::FSI:HypN "<<r4M<<"->A="<<rlPDG<<n4M<<", n*Lamb="<<nL<<h4M<<G4endl;
#endif
        curHadr->Set4Momentum(n4M);
        curHadr->SetQPDG(G4QPDGCode(rlPDG));   // converted hypernucleus
        if(rlPDG==90000002)                    // Additional action with curHadr change
	       {
          G4LorentzVector newLV=n4M/2.;
          curHadr->Set4Momentum(newLV);
          curHadr->SetQPDG(G4QPDGCode(90000001));
          G4QHadron* secHadr = new G4QHadron(curHadr);
          theQHadrons.push_back(secHadr);    // (del.eq.- user is responsible for del)
        }
        else if(rlPDG==90002000)               // Additional action with curHadr change
	       {
          G4LorentzVector newLV=n4M/2.;
          curHadr->Set4Momentum(newLV);
          curHadr->SetQPDG(G4QPDGCode(90001000));
          G4QHadron* secHadr = new G4QHadron(curHadr);
          theQHadrons.push_back(secHadr);    // (del.eq.- user is responsible for del)
        }
        // @@(?) Add multybaryon decays if necessary (Now it anyhow is made later)
#ifdef fdebug
        G4cout<<"*G4QE::FSI: resNucPDG="<<curHadr->GetPDGCode()<<G4endl;
#endif
        if(nL>1) h4M=h4M/nL;                   // split the lambda's 4-mom if necessary
        for(G4int il=0; il<nL; il++)           // loop over excessive
        {
          G4QHadron* theLamb = new G4QHadron(sPDG,h4M);// Make a New Hadr for the Hyperon
          theQHadrons.push_back(theLamb);      // (del.eq.- user is responsible for del)
        }
      }
	     else if(reM>rnM+mPi0-eps&&!nSP&&!nSM)    // Lambda->N only if Sigmas are absent
	     {
        G4int nPi=static_cast<G4int>((reM-rnM)/mPi0);
        if (nPi>nL) nPi=nL;
        G4double npiM=nPi*mPi0;
        G4LorentzVector n4M(0.,0.,0.,rnM);
        G4LorentzVector h4M(0.,0.,0.,npiM);
        G4double sum=rnM+npiM;
        if(fabs(reM-sum)<eps)
	       {
          n4M=r4M*(rnM/sum);
          h4M=r4M*(npiM/sum);
        }
        else if(reM<sum || !G4QHadron(r4M).DecayIn2(n4M,h4M))
        {
          G4cerr<<"***G4QE::FSI:Hypern, M="<<reM<<"<A+n*Pi0="<<sum<<",d="<<sum-reM<<G4endl;
          throw G4QException("***G4QEnvironment::FSInter: Hypernuclear decay error");
        }
        curHadr->Set4Momentum(n4M);
        curHadr->SetQPDG(G4QPDGCode(rnPDG));   // converted hypernucleus
#ifdef fdebug
        G4cout<<"*G4QE::FSI:HypN "<<r4M<<"->A="<<rnPDG<<n4M<<",n*Pi0="<<nPi<<h4M<<G4endl;
#endif
        if(nPi>1) h4M=h4M/nPi;                 // split the 4-mom if necessary
        for(G4int ihn=0; ihn<nPi; ihn++)       // loop over additional pions
        {
          G4QHadron* thePion = new G4QHadron(111,h4M);// Make a New Hadr for the pion
          theQHadrons.push_back(thePion);    // (del.eq.- user is responsible for del)
          //theFragments->push_back(thePion);    // (delete equivalent for the pion)
        }
        if(rnPDG==90000002)                    // Additional action with curHadr change
	       {
          G4LorentzVector newLV=n4M/2.;
          curHadr->Set4Momentum(newLV);
          curHadr->SetQPDG(G4QPDGCode(90000001));
          G4QHadron* secHadr = new G4QHadron(curHadr);
          theQHadrons.push_back(secHadr);    // (del.eq.- user is responsible for del)
          //theFragments->push_back(secHadr);    // (del.eq.- user is responsible for del)
        }
        else if(rnPDG==90002000)               // Additional action with curHadr change
	       {
          G4LorentzVector newLV=n4M/2.;
          curHadr->Set4Momentum(newLV);
          curHadr->SetQPDG(G4QPDGCode(90001000));
          G4QHadron* secHadr = new G4QHadron(curHadr);
          theQHadrons.push_back(secHadr);    // (del.eq.- user is responsible for del)
          //theFragments->push_back(secHadr);    // (del.eq.- user is responsible for del)
        }
        // @@ Add multybaryon decays if necessary
      }
      else // If this Excepton shows up (lowProbable appearance) => include gamma decay
	     {
        G4double d=rlM+MLa-reM;
		G4cerr<<"G4QE::FSI:R="<<rlM<<",S+="<<nSP<<",S-="<<nSM<<",L="<<nL<<",d="<<d<<G4endl;
        d=rnM+mPi0-reM;
		G4cerr<<"***G4QE::FSI: HypN="<<hPDG<<", M="<<reM<<"<"<<rnM+mPi0<<",d="<<d<<G4endl;
        throw G4QException("G4QEnvironment::FSInteract: Hypernuclear conversion");
      }
    }
    //unsigned nHd=theQHadrons.size()-1;
    //if(hd==nHd && !fOK)
	//{
    //  G4cerr<<"---Warning---G4QE::FSI: The Last #"<<hd<<" hadron must be filled"<<G4endl;
	//  fOK=true;
    //  curHadr = new G4QHadron(theQHadrons[hd]);
    //}
    //if(fOK) theFragments->push_back(curHadr);  //(del eq. - user is responsible for del)
    //else hd--;
    if(!fOK) hd--;
  } // 
#ifdef fdebug
  G4cout<<"G4QE::FSI:==>OUT,nH="<<theQHadrons.size()<<",nF="<<theFragments->size()<<G4endl;
#endif
  // *** (Final) Charge Control Sum Calculation for the Charge Conservation Check ***
  //nHadr=theFragments->size();
  nHadr=theQHadrons.size();
  G4int cfContSum=0;                   // Final ChargeControlSum 
  G4int bfContSum=0;                   // Final BaryonNumberControlSum 
  if(nHadr)for(unsigned f=0; f<nHadr; f++)
  {
    G4QHadron* curHadr = new G4QHadron(theQHadrons[f]);
#ifdef pdebug
    G4cout<<"G4QE::FSI:#"<<f<<curHadr->Get4Momentum()<<curHadr->GetPDGCode()<<G4endl;
#endif
    if(!(curHadr->GetNFragments()))
    {
      G4int chg=curHadr->GetCharge();
      cfContSum+=chg;
      G4int brn=curHadr->GetBaryonNumber();
      bfContSum+=brn;
      G4int str=curHadr->GetStrangeness();
      if(brn>1&&(!str&&(chg==brn||!chg) || !chg&&str==brn)) // Check for multibaryon(split)
	     {
        G4int             bPDG=90000001; // Prototype: multineutron
        if     (chg==brn) bPDG=90001000; // Multyproton
        else if(str==brn) bPDG=91000000; // Multilambda
        G4LorentzVector sp4m=curHadr->Get4Momentum()/brn; // Split 4-mom of the Multibaryon
        curHadr->Set4Momentum(sp4m);
        curHadr->SetQPDG(G4QPDGCode(bPDG)); // Substitute Multibaryon by a baryon
        for (G4int ib=1; ib<brn; ib++)
		      {
          G4QHadron* bH = new G4QHadron(bPDG, sp4m);
         theFragments->push_back(bH);     //(del eq. - user is responsible for deleting)
        }
      }
      theFragments->push_back(curHadr);  //(del eq. - user is responsible for deleting)
    }
  }
  if(cfContSum-chContSum || bfContSum-bnContSum)
  {
    G4cerr<<"*G4QE::FSI:(9) Ch="<<cfContSum-chContSum<<",Bn="<<bfContSum-bnContSum<<G4endl;
#ifdef chdebug
    throw G4QException("G4QEnvironment::FSInteract: (9) Charge is not conserved");
#endif
  }
  // ***
  return theFragments;
} // End of "FSInteraction"

//The public Quasmons duplication with delete responsibility of User (!)
G4QuasmonVector* G4QEnvironment::GetQuasmons()
{//              =============================
  G4int nQ=theQuasmons.size();
#ifdef pdebug
  G4cout<<"G4QEnvironment::GetQuasmons is called nQ="<<nQ<<G4endl;
#endif
  G4QuasmonVector* quasmons = new G4QuasmonVector;   // !! User is responsible to delet it
  if(nQ) for(G4int iq=0; iq<nQ; iq++)
  {
#ifdef pdebug
    G4cout<<"G4QEnv::GetQuasm:Q#"<<iq<<",QQPDG="<<theQuasmons[iq]->GetQPDG()<<",QQC="
          <<theQuasmons[iq]->GetQC()<<",M="<<theQuasmons[iq]->Get4Momentum().m()<<G4endl;
#endif
    G4Quasmon* curQ = new G4Quasmon(theQuasmons[iq]);
    quasmons->push_back(curQ);                 // (delete equivalent - user is responsible)
  }
#ifdef pdebug
  G4cout<<"G4QEnvironment::GetQuasmons ===OUT==="<<G4endl;
#endif
  return quasmons;
} // End of GetQuasmons

//The public QHadrons duplication with delete responsibility of User (!)
G4QHadronVector* G4QEnvironment::GetQHadrons()
{//              =============================
  G4int nH=theQHadrons.size();
#ifdef pdebug
  G4cout<<"G4QEnvironment::GetQHadrons is called nH="<<nH<<G4endl;
#endif
  G4QHadronVector* hadrons = new G4QHadronVector;  // !! User is responsible to delet it
  if(nH) for(G4int ih=0; ih<nH; ih++)
  {
#ifdef pdebug
    G4cout<<"G4QEnv::GetQHadrons:H#"<<ih<<",HQPDG="<<theQHadrons[ih]->GetQPDG()<<",HQC="
          <<theQHadrons[ih]->GetQC()<<",HM="<<theQHadrons[ih]->GetMass()<<G4endl;
#endif
    G4QHadron* curH = new G4QHadron(theQHadrons[ih]);
    hadrons->push_back(curH);                       // (del. equiv. - user is responsibile)
  }
#ifdef pdebug
  G4cout<<"G4QEnvironment::GetQHadrons ===OUT=== Copied nQH="<<hadrons->size()<<G4endl;
#endif
  return hadrons;
} // End of GetQHadrons

//Public QHadrons cleaning up after extraction (GetQHadrons) between Construct and Fragment
void G4QEnvironment::CleanUpQHadrons()
{//  =================================
  for_each(theQHadrons.begin(), theQHadrons.end(), DeleteQHadron());
  theQHadrons.clear();
} // End of CleanUpQHadrons

//The public FillQHadrons filling. It must be used only internally in CHIPS
void G4QEnvironment::FillQHadrons(G4QHadronVector* input)
{//                  ====================================
  G4int nH=input->size();
#ifdef pdebug
  G4cout<<"G4QEnvironment::FillQHadrons is called nH="<<nH<<G4endl;
#endif
  if(nH) for(G4int ih=0; ih<nH; ih++)
  {
#ifdef pdebug
    G4cout<<"G4QEnv::FillQHadrons:H#"<<ih<<",HQPDG="<<(*input)[ih]->GetQPDG()<<",HQC="
          <<(*input)[ih]->GetQC()<<",HM="<<(*input)[ih]->GetMass()<<G4endl;
#endif
    G4QHadron* curH = new G4QHadron((*input)[ih]);
    theQHadrons.push_back(curH);                        // (del. equiv. 
  }
#ifdef pdebug
  G4cout<<"G4QEnvironment::FillQHadrons ===OUT=== Filled nQH="<<theQHadrons.size()<<G4endl;
#endif
} // End of FillQHadrons

//Unavoidable decay of the Isonucleus in nP+(Pi+) or nN+(Pi-)
void G4QEnvironment::DecayIsonucleus(G4QHadron* qH)
{//  ============================================
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  //static const G4double mSigZ= G4QPDGCode(3212).GetMass();
  static const G4double mSigP= G4QPDGCode(3222).GetMass();
  static const G4double mSigM= G4QPDGCode(3112).GetMass();
  static const G4double eps  = 0.003;
  G4LorentzVector q4M = qH->Get4Momentum();      // Get 4-momentum of the Isonucleus
  G4double qM=q4M.m();                           // Real mass of the Isonucleus
  G4QContent qQC = qH->GetQC();                  // Get QuarcContent of the Isonucleus
  G4int qBN=qQC.GetBaryonNumber();               // Baryon number of the IsoNucleus
  G4int qC=qQC.GetCharge();                      // Charge of the IsoNucleus
  G4int qS=qQC.GetStrangeness();                 // Strangness of the IsoNucleus
#ifdef pdebug
  G4cout<<"G4QE::DecayIson:QC="<<qQC<<", M="<<qM<<",B="<<qBN<<",S="<<qS<<",C="<<qC<<G4endl;
#endif
  if(qS<0||qS>qBN)                               // *** Should not be here ***
  {
    G4cout<<"--Warning(Upgrade)--G4QEnv::DecIsonuc:FillAsIs,4M="<<q4M<<",QC="<<qQC<<G4endl;
    G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
   	if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH); // Cor or fill as it is
    else delete qH;  
    delete quasH;
    return;
  }
  G4int          qPN=qC-qBN;                     // Number of pions in the Isonucleus
  G4int          fPDG = 2212;                    // Prototype for nP+(Pi+) case
  G4int          sPDG = 211;
  G4int          tPDG = 3122;                    // @@ Sigma0 (?)
  G4double       fMass= mProt;
  G4double       sMass= mPi;
  G4double       tMass= mLamb;                   // @@ Sigma0 (?)
  // ========= Negative state ======
  if(qC<0)                                       // ====== Only Pi- can help
  {
    if(qS&&qBN==qS)                              // --- n*Lamb + k*(Pi-) State ---
    {
      sPDG = -211;
      if(-qC==qS && qS==1)                        // Only one Sigma- like (qBN=1)
      {
        if(fabs(qM-mSigM)<eps)
		      {
          theQHadrons.push_back(qH);              // Fill Sigma- as it is
          return;
        }
        else if(qM>mLamb+mPi)                     //(2) Sigma- => Lambda + Pi- decay
        {
          fPDG = 3122;
          fMass= mLamb;
        }
        else if(qM>mSigM)                         //(2) Sigma+ => Sigma+ + gamma decay
        {
          fPDG = 3112;
          fMass= mSigM;
          sPDG = 22;
          sMass= 0.;
        }
        else                                      //(2) Sigma- => Neutron + Pi- decay
        {
          fPDG = 2112;
          fMass= mNeut;
        }
        qPN  = 1;                                 // #of (Pi+ or gamma)'s = 1
      }
      else if(-qC==qS)                            //(2) a few Sigma- like
      {
        qPN  = 1;                                 // One separated Sigma-
        fPDG = 3112;
        sPDG = 3112;
        sMass= mSigM;
        qBN--;
        fMass= mSigM;
      }
      else if(-qC>qS)                             //(2) n*(Sigma-)+m*(Pi-)
      {
        qPN  = -qC-qS;                            // #of Pi-'s
        fPDG = 3112;
        fMass= mSigM;
      }
      else                                        //(2) n*(Sigma-)+m*Lambda (-qC<qS)
      {
        qBN += qC;                                // #of Lambda's
        fPDG = 3122;
        fMass= mLamb;
        qPN  = -qC;                               // #of Sigma+'s
        sPDG = 3112;
        sMass= mSigM;
      }
      qS   = 0;                                   // Only decays in two are above
    }
    else if(qS)                                   // ->n*Lamb+m*Neut+k*(Pi-) State (qS<qBN)
    {
      qBN -= qS;                                  // #of neutrons
      fPDG = 2112;
      fMass= mNeut;
      G4int nPin = -qC;                           // #of Pi-'s                    
      if(qS==nPin)                                //(2) m*Neut+n*Sigma-
      {
        qPN  = qS;                                // #of Sigma-
        sPDG = 3112;
        sMass= mSigM;
        qS   = 0;
      }
      else if(qS>nPin)                            //(3) m*P+n*(Sigma+)+k*Lambda
      {
        qS-=nPin;                                 // #of Lambdas
        qPN  = nPin;                              // #of Sigma+
        sPDG = 3112;
        sMass= mSigM;
      }
      else                                        //(3) m*N+n*(Sigma-)+k*(Pi-) (qS<nPin)
      {
        qPN  = nPin-qS;                           // #of Pi-
        sPDG = -211;
        tPDG = 3112;
        tMass= mSigM;
      }
    }
    else                                          //(2) n*N+m*(Pi-)   (qS=0)
	   {
      sPDG = -211;
      qPN  = -qC;
      fPDG = 2112;
      fMass= mNeut;
    }
  }
  else if(!qC)                                   // *** Should not be here ***
  {
    if(qS && qS<qBN)                             //(2) n*Lamb+m*N ***Should not be here***
    {
      qPN  = qS;
      fPDG = 2112;                               // mN+nL case
      sPDG = 3122;
      sMass= mLamb;
      qBN -= qS;
      fMass= mNeut;
      qS   = 0;
    }
    else if(qS>1 && qBN==qS)                     //(2) m*Lamb(m>1) ***Should not be here***
    {
      qPN  = 1;
      fPDG = 3122;
      sPDG = 3122;
      sMass= mLamb;
      qBN--;
      fMass= mLamb;
    }
    else if(!qS && qBN>1)                        //(2) n*Neut(n>1) ***Should not be here***
    {
      qPN  = 1;
      fPDG = 2112;
      sPDG = 2112;
      sMass= mNeut;
      qBN--;
      fMass= mNeut;
    }
    else G4cout<<"*?*G4QEnv::DecayIsonucleus: (1) QC="<<qQC<<G4endl;
  }
  else if(qC>0)                                  // n*Lamb+(m*P)+(k*Pi+)
  {
    if(qS && qS+qC==qBN)                         //(2) n*Lamb+m*P ***Should not be here***
    {
      qPN  = qS;
      qS   = 0;
      fPDG = 2212;
      sPDG = 3122;
      sMass= mLamb;
      qBN  = qC;
      fMass= mProt;
	   }
    else if(qS  && qC<qBN-qS)                     //(3)n*L+m*P+k*N ***Should not be here***
    {
      qPN  = qC;                                  // #of protons
      fPDG = 2112;                                // mP+nL case
      sPDG = 2212;
      sMass= mProt;
      qBN -= qS+qC;                               // #of neutrons
      fMass= mNeut;
    }
    else if(qS  && qBN==qS)                       // ---> n*L+m*Pi+ State
    {
      if(qC==qS && qS==1)                         // Only one Sigma+ like State
      {
        if(fabs(qM-mSigP)<eps)                    // Fill Sigma+ as it is
		      {
          theQHadrons.push_back(qH);
          return;
        }
        else if(qM>mLamb+mPi)                     //(2) Sigma+ => Lambda + Pi+ decay
        {
          fPDG = 3122;
          fMass= mLamb;
        }
        else if(qM>mNeut+mPi)                     //(2) Sigma+ => Neutron + Pi+ decay
        {
          fPDG = 2112;
          fMass= mNeut;
        }
        else if(qM>mSigP)                         //(2) Sigma+ => Sigma+ + gamma decay
        {
          fPDG = 3222;
          fMass= mSigP;
          sPDG = 22;
          sMass= 0.;
        }
        else                                      //(2) Sigma+ => Proton + gamma decay
        {
          fPDG = 2212;
          fMass= mProt;
          sPDG = 22;
          sMass= 0.;
        }
        qPN  = 1;                                 // #of (Pi+ or gamma)'s = 1
      }
      else if(qC==qS)                             //(2) a few Sigma+ like hyperons
      {
        qPN  = 1;
        fPDG = 3222;
        sPDG = 3222;
        sMass= mSigP;
        qBN--;
        fMass= mSigP;
      }
      else if(qC>qS)                              //(2) n*(Sigma+)+m*(Pi+)
      {
        qPN  = qC-qS;                             // #of Pi+'s
        fPDG = 3222;
        qBN  = qS;                                // #of Sigma+'s
        fMass= mSigP;
      }
      else                                        //(2) n*(Sigma+)+m*Lambda
      {
        qBN -= qC;                                // #of Lambda's
        fPDG = 3122;
        fMass= mLamb;
        qPN  = qC;                                // #of Sigma+'s
        sPDG = 3222;
        sMass= mSigP;
      }
      qS   = 0;                                   // All above are decays in 2
    }
    else if(qS && qC>qBN-qS)                      // n*Lamb+m*P+k*Pi+
    {
      qBN -= qS;                                  // #of protons
      G4int nPip = qC-qBN;                        // #of Pi+'s                    
      if(qS==nPip)                                //(2) m*P+n*Sigma+
      {
        qPN  = qS;                                // #of Sigma+
        sPDG = 3222;
        sMass= mSigP;
        qS   = 0;
      }
      else if(qS>nPip)                            //(3) m*P+n*(Sigma+)+k*Lambda
      {
        qS  -= nPip;                              // #of Lambdas
        qPN  = nPip;                              // #of Sigma+
        sPDG = 3222;
        sMass= mSigP;
      }
      else                                        //(3) m*P+n*(Sigma+)+k*(Pi+)
      {
        qPN  = nPip-qS;                           // #of Pi+
        tPDG = 3222;
        tMass= mSigP;
      }
    }
    if(qC<qBN)                                    //(2) n*P+m*N ***Should not be here***
    {
      fPDG = 2112;
      fMass= mNeut;
      qPN  = qC;
      sPDG = 2212;
      sMass= mProt;
    }
    else if(qBN==qC && qC>1)                     //(2) m*Prot(m>1) ***Should not be here***
    {
      qPN  = 1;
      fPDG = 2212;
      sPDG = 2212;
      sMass= mProt;
      qBN--;
      fMass= mProt;
    }
    else if(qC<=qBN||!qBN) G4cout<<"*?*G4QEnv::DecayIsonucleus: (2) QC="<<qQC<<G4endl;
    // !qS && qC>qBN                             //(2) Default condition n*P+m*(Pi+)
  }
  G4double tfM=qBN*fMass;
  G4double tsM=qPN*sMass;
  G4double ttM=0.;
  if(qS) ttM=qS*tMass;
  G4LorentzVector f4Mom(0.,0.,0.,tfM);
  G4LorentzVector s4Mom(0.,0.,0.,tsM);
  G4LorentzVector t4Mom(0.,0.,0.,ttM);
  G4double sum=tfM+tsM+ttM;
  if(fabs(qM-sum)<eps)
  {
    f4Mom=q4M*(tfM/sum);
    s4Mom=q4M*(tsM/sum);
    if(qS) t4Mom=q4M*(ttM/sum);
  }
  else if(!qS && (qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom)))
  {
#ifdef pdebug
    G4cerr<<"***G4QEnv::DecIsonuc:fPDG="<<fPDG<<"*"<<qBN<<"(fM="<<fMass<<")+sPDG="<<sPDG
          <<"*"<<qPN<<"(sM="<<sMass<<")"<<"="<<sum<<" > TotM="<<qM<<q4M<<qQC<<qS<<G4endl;
#endif
    G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	   if(!CheckGroundState(quasH,true))
    {
#ifdef pdebug
      G4cerr<<"*G4QEnv::DecIsonuc: Isonucleus's filled AsIs 4M="<<q4M<<qQC<<G4endl;
#endif
      theQHadrons.push_back(qH); // Corect or fill as it is
    }
    else delete qH;  
    delete quasH;
    return;
  }
  else if(qS && (qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom)))
  {
#ifdef pdebug
    G4cerr<<"***G4QEnv::DecIsonuc: "<<fPDG<<"*"<<qBN<<"("<<fMass<<")+"<<sPDG<<"*"<<qPN<<"("
          <<sMass<<")+Lamb*"<<qS<<"="<<sum<<" > TotM="<<qM<<q4M<<qQC<<G4endl;
#endif
    G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	   if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH);// Cor or fill as it is
    else delete qH;  
    delete quasH;
    return;
  }
#ifdef pdebug
  G4cout<<"G4QEnv::DecayIsonucleus: *DONE* n="<<qPN<<f4Mom<<fPDG<<", m="<<qPN<<s4Mom<<sPDG
        <<", l="<<qS<<t4Mom<<G4endl;
#endif
  delete qH;
  if(qBN)
  {
    f4Mom/=qBN;
    for(G4int ih=0; ih<qBN; ih++)
    {
      G4QHadron* Hi = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the hyperon
      theQHadrons.push_back(Hi);                 // Fill "Hi" (delete equivalent)
    }
  }
  if(qPN)
  {
    s4Mom/=qPN;
    for(G4int ip=0; ip<qPN; ip++)
    {
      G4QHadron* Hj = new G4QHadron(sPDG,s4Mom); // Create a Hadron for the meson
      theQHadrons.push_back(Hj);                 // Fill "Hj" (delete equivalent)
    }
  }
  if(qS)
  {
    t4Mom/=qS;
    for(G4int il=0; il<qS; il++)
    {
      G4QHadron* Hk = new G4QHadron(tPDG,t4Mom); // Create a Hadron for the lambda
      theQHadrons.push_back(Hk);                 // Fill "Hk" (delete equivalent)
    }
  }
} // End of DecayIsonucleus

//Decay of the excited Baryon in baryon & meson (gamma)
void G4QEnvironment::DecayBaryon(G4QHadron* qH)
{//  ============================================
  static const G4QPDGCode gQPDG(22);
  static const G4QPDGCode pizQPDG(111);
  static const G4QPDGCode pipQPDG(211);
  static const G4QPDGCode pimQPDG(-211);
  static const G4QPDGCode kmQPDG(-321);
  static const G4QPDGCode kzQPDG(-311);
  static const G4QPDGCode nQPDG(2112);
  static const G4QPDGCode pQPDG(2212);
  static const G4QPDGCode lQPDG(3122);
  static const G4QPDGCode laQPDG(3122);
  static const G4QPDGCode smQPDG(3112);
  static const G4QPDGCode szQPDG(3212);
  static const G4QPDGCode spQPDG(3222);
  static const G4QPDGCode kszQPDG(3322);
  static const G4QPDGCode ksmQPDG(3312);
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mPi0 = G4QPDGCode(111).GetMass();
  static const G4double mK   = G4QPDGCode(321).GetMass();
  static const G4double mK0  = G4QPDGCode(311).GetMass();
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mSigM= G4QPDGCode(3112).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mSigZ= G4QPDGCode(3212).GetMass();
  static const G4double mSigP= G4QPDGCode(3222).GetMass();
  //static const G4double mKsiM= G4QPDGCode(3312).GetMass();
  //static const G4double mKsi0= G4QPDGCode(3322).GetMass();
  //static const G4double mOmeg= G4QPDGCode(3334).GetMass();
  static const G4double mNPi0 = mPi0+ mNeut;
  static const G4double mNPi  = mPi + mNeut;
  static const G4double mPPi0 = mPi0+ mProt;
  static const G4double mPPi  = mPi + mProt;
  static const G4double mLPi0 = mPi0+ mLamb;
  static const G4double mLPi  = mPi + mLamb;
  static const G4double mSpPi = mPi + mSigP;
  static const G4double mSmPi = mPi + mSigP;
  static const G4double mPK   = mK  + mProt;
  static const G4double mPKZ  = mK0 + mProt;
  static const G4double mNKZ  = mK0 + mNeut;
  static const G4double mSpPi0= mPi0+ mSigP;
  static const G4double mSzPi0= mPi0+ mSigZ;
  static const G4double mSzPi = mPi + mSigZ;
  static const G4double eps  = 0.003;
  //static const G4QNucleus vacuum(90000000);
  G4int theLB= qH->GetBaryonNumber();          // Baryon number of the Baryon
  if(theLB!=1)
  {
    G4cerr<<"***G4QEnvironment::DecayBaryon: A!=1 -> fill as it is"<<G4endl;
#ifdef ppdebug
    throw G4QException("G4QEnv::DecayBaryon: Unknown Baryon with A!=1");
#endif
    theQHadrons.push_back(qH);                 // Fill AsIs (delete equivalent)
    return;
  }
  G4int theLC= qH->GetCharge();                // Chsrge of the Baryon
  G4int theLS= qH->GetStrangeness();           // Strangness of the Baryon
  //G4int          qPDG = qH->GetPDGCode();      // PDG Code of the decaying baryon
  G4LorentzVector q4M = qH->Get4Momentum();    // Get 4-momentum of the Baryon
  G4double         qM = q4M.m();               // Mass of the Baryon
#ifdef pdebug
  G4cout<<"G4QEnv::DecayBaryon: *Called* S="<<theLS<<",C="<<theLC<<",4M="<<q4M<<qM<<G4endl;
#endif
  // Select a chanel of the baryon decay
  G4QPDGCode     fQPDG = pQPDG;                 // Prototype for Proton
  G4double       fMass= mProt;
  G4QPDGCode     sQPDG = pizQPDG;                  // Prototype for Pi0
  G4double       sMass= mPi0;
  if(!theLS)                    // This is only for not strange baryons
  {
    if(!theLC)                  // Neutron like: n+gam, n+Pi0, p+Pi- are possible
				{
      if(qM<mNPi0)              // Only n+gamma decay is possible
						{
        if(fabs(qM-mNeut)<eps)
								{
#ifdef pdebug
          G4cout<<"G4QEnv::DecayBaryon: Fill Neutron AsIs"<<G4endl;
#endif
          theQHadrons.push_back(qH); // Fill AsIs (delete equivalent)
          return;
        }
        fQPDG=nQPDG;            // Baryon is neutron
        fMass=mNeut;
        sQPDG=gQPDG;            // Meson is gamma
        sMass=0.;
      }
      else if(qM>mPPi)          // Both n+pi0 (p=2/3) & p+Pi- (p=1/3) are possible
						{
        if(G4UniformRand()>.333333333) // Clebsh value for the Isobar decay
        {
          fQPDG=nQPDG;          // Baryon is neutron (meson is Pi0)
          fMass=mNeut;
        }
        else
        {
          sQPDG=pimQPDG;        // Meson is Pi- (baryon is proton)
          sMass=mPi;
        }
      }
						else                      // Only n+Pi0 decay is possible
      {
        fQPDG=nQPDG;            // Baryon is neutron
        fMass=mNeut;
      }
    }
    else if(theLC==1)           // Proton like: p+gam, p+Pi0, n+Pi+ are possible
				{
      if(qM<mPPi0)              // Only p+gamma decay is possible
						{
        if(fabs(qM-mProt)<eps)
								{
#ifdef pdebug
          G4cout<<"G4QEnv::DecayBaryon: Fill Proton AsIs"<<G4endl;
#endif
          theQHadrons.push_back(qH); // Fill AsIs (delete equivalent)
          return;
        }
        sQPDG=gQPDG;            // Meson is gamma (baryon is proton)
        sMass=0.;
      }
      else if(qM>mNPi)          // Both p+pi0 (p=2/3) & n+Pi+ (p=1/3) are possible
						{
        if(G4UniformRand()<.333333333) // Clebsh value for the Isobar decay
        {
          fQPDG=nQPDG;          // Baryon is neutron
          fMass=mNeut;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        // p+Pi0 is a default case
      }
      // p+Pi0 is a default case
    }
    else if(theLC==2)           // Delta++ like: only p+PiP is possible
				{
      if(qM>mPPi)               // Only p+gamma decay is possible
						{
        sQPDG=pipQPDG;          // Meson is Pi+ (baryon is proton)
        sMass=mPi;
      }
      else                      // @@ Can be aReason to search for anError in Fragmentation
						{
#ifdef pdebug
        G4cout<<"-Warning-G4QE::DecBary:*AsIs* DEL++ M="<<qM<<"<"<<mPPi<<G4endl;
#endif
        theQHadrons.push_back(qH);               // Fill AsIs (delete equivalent)
        return;
      }
    }
    else if(theLC==-1)          // Delta- like: only n+PiM is possible
				{
      if(qM+eps>mNPi)           // Only p+gamma decay is possible
						{
        fQPDG=nQPDG;            // Baryon is neutron
        fMass=mNeut;
        sQPDG=pimQPDG;          // Meson is Pi-
        sMass=mPi;
      }
      else                      // @@ Can be aReason to search for anError in Fragmentation
						{
#ifdef pdebug
        G4cout<<"-Warning-G4QE::DecBary:*AsIs* DEL++ M="<<qM<<"<"<<mNPi<<G4endl;
#endif
        theQHadrons.push_back(qH);               // Fill AsIs (delete equivalent)
        return;
      }
    }
    else 
    {
#ifdef pdebug
      G4cout<<"-Warning-G4QE::DecBary:*AsIs* UnknBaryon (S=0) QC="<<qH->GetQC()<<G4endl;
#endif
      theQHadrons.push_back(qH);                 // Fill AsIs (delete equivalent)
      return;
    }
  }
  else if(theLS==1)             // ======>>>> S=1 <<<<==========
  {
    if(!theLC)                  // -->> Lambda/Sz: L+g,L+Pi0,Sz+Pi0,Sm+Pip,Sp+Pim,p+Km,n+Kz
				{
      if(qM<mLPi0)              // Only Lambda+gamma decay is possible
						{
        if(fabs(qM-mLamb)<eps)
								{
#ifdef pdebug
          G4cout<<"G4QEnv::DecayBaryon: Fill Lambda AsIs"<<G4endl;
#endif
          theQHadrons.push_back(qH); // Fill AsIs (delete equivalent)
          return;
        }
        fQPDG=lQPDG;            // Baryon is Lambda
        fMass=mLamb;
        sQPDG=gQPDG;            // Meson is gamma
        sMass=0.;
      }
      else if(qM<mSzPi0)        // Only Lambda+Pi0 is possible
						{
        fQPDG=lQPDG;            // Baryon is Lambda
        fMass=mLamb;
      }
      else if(qM<mSpPi)         // Both Lambda+Pi0 & Sigma0+Pi0 are possible
						{
        if(G4UniformRand()>.6)  // @@ Relative probability (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
        }
        else
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
        }
      }
      else if(qM<mSmPi)         // Lambda+Pi0, Sigma0+Pi0, & SigmaP+PiM are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.4)               // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
        }
        else if(ra<.7)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
        }
        else
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
      }
      else if(qM<mPK)           // Lambda+Pi0, Sig0+Pi0, SigP+PiM, SigM+PiP are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.35)              // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
        }
        else if(ra<.6)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
        }
        else if(ra<.8)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=smQPDG;         // Baryon is SigmaM
          fMass=mSigM;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
      }
      else if(qM<mNKZ)          // Lambda+Pi0, Sig0+Pi0, SigP+PiM, SigM+PiP are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.3)               // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
        }
        else if(ra<.5)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
        }
        else if(ra<.7)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=smQPDG;         // Baryon is SigmaM
          fMass=mSigM;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else if(ra<.9)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else
        {
          fQPDG=pQPDG;          // Baryon is Proton
          fMass=mProt;
          sQPDG=kmQPDG;         // Meson is K-
          sMass=mK;
        }
      }
						else                      // Only n+Pi0 decay is possible
      {
        G4double ra=G4UniformRand();
        if(ra<.3)               // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
        }
        else if(ra<.5)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
        }
        else if(ra<.65)         // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=smQPDG;         // Baryon is SigmaM
          fMass=mSigM;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else if(ra<.8)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else if(ra<.9)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=pQPDG;          // Baryon is Proton
          fMass=mProt;
          sQPDG=kmQPDG;         // Meson is K-
          sMass=mK;
        }
        else
        {
          fQPDG=nQPDG;          // Baryon is Neutron
          fMass=mNeut;
          sQPDG=kzQPDG;         // Meson is K0
          sMass=mK0;
        }
      }
    }
    else if(theLC==1)           // SigmaP: SigP+gam,SigP+Pi0,Sp+PiP,L+Pi0,p+K0 are possible
				{
      if(qM<mLPi)               // Only SigmaPlus+gamma decay is possible
						{
        if(fabs(qM-mSigP)<eps)
								{
#ifdef pdebug
          G4cout<<"G4QEnv::DecayBaryon: Fill SigmaPlus AsIs"<<G4endl;
#endif
          theQHadrons.push_back(qH); // Fill AsIs (delete equivalent)
          return;
        }
        fQPDG=spQPDG;           // Baryon is SigmaP
        fMass=mSigP;
        sQPDG=gQPDG;            // Meson is gamma
        sMass=0.;
      }
      else if(qM<mSpPi0)        // Only Lambda+PiP is possible
						{
        fQPDG=lQPDG;            // Baryon is Lambda
        fMass=mLamb;
        sQPDG=pipQPDG;          // Meson is Pi+
        sMass=mPi;
      }
      else if(qM<mSzPi)         // Both Lambda+PiP & Sigma0+Pi0 are possible
						{
        if(G4UniformRand()<.6)  // @@ Relative probability (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
        }
      }
      else if(qM<mPKZ)          // Lambda+PiP, SigmaP+Pi0, & Sigma0+PiP are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.4)               // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else if(ra<.7)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
        }
        else
        {
          fQPDG=szQPDG;         // Baryon is SigmaZ
          fMass=mSigZ;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
      }
      else                      // Lambda+PiP, SigmaP+Pi0, Sigma0+PiP, p+K0 are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.35)              // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else if(ra<.6)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=spQPDG;         // Baryon is SigmaP
          fMass=mSigP;
        }
        else if(ra<.8)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is SigmaZ
          fMass=mSigZ;
          sQPDG=pipQPDG;        // Meson is Pi+
          sMass=mPi;
        }
        else
        {
          fQPDG=pQPDG;          // Baryon is Proton
          fMass=mProt;
          sQPDG=kzQPDG;         // Meson is K0
          sMass=mK0;
        }
      }
    }
    else if(theLC==-1)          // SigmaM: SigM+gam,SigM+Pi0,S0+PiM,L+Pi-,n+KM are possible
				{
      if(qM<mLPi)               // Only SigmaMinus + gamma decay is possible
						{
        if(fabs(qM-mSigM)<eps)
								{
#ifdef pdebug
          G4cout<<"G4QEnv::DecayBaryon: Fill SigmaMinus AsIs"<<G4endl;
#endif
          theQHadrons.push_back(qH); // Fill AsIs (delete equivalent)
          return;
        }
        fQPDG=smQPDG;           // Baryon is SigmaP
        fMass=mSigM;
        sQPDG=gQPDG;            // Meson is gamma
        sMass=0.;
      }
      else if(qM<mSzPi)         // Only Lambda+PiM is possible
						{
        fQPDG=lQPDG;            // Baryon is Lambda
        fMass=mLamb;
        sQPDG=pimQPDG;          // Meson is Pi-
        sMass=mPi;
      }
      else if(qM<mSzPi)         // Both Lambda+PiM & Sigma0+PiM are possible
						{
        if(G4UniformRand()<.6)  // @@ Relative probability (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
      }
      else if(qM<mPKZ)          // Lambda+PiM, Sigma0+PiM, & SigmaM+Pi0 are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.4)               // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else if(ra<.7)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else
        {
          fQPDG=smQPDG;         // Baryon is SigmaM
          fMass=mSigM;
        }
      }
      else                      // Lambda+PiM, Sig0+PiM, SigM+Pi0, n+KM are possible
						{
        G4double ra=G4UniformRand();
        if(ra<.35)              // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=lQPDG;          // Baryon is Lambda
          fMass=mLamb;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else if(ra<.6)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=szQPDG;         // Baryon is Sigma0
          fMass=mSigZ;
          sQPDG=pimQPDG;        // Meson is Pi-
          sMass=mPi;
        }
        else if(ra<.8)          // @@ Rel. probab. (take into account Phase Space)
        {
          fQPDG=smQPDG;         // Baryon is SigmaM
          fMass=mSigM;
        }
        else
        {
          fQPDG=nQPDG;          // Baryon is Proton
          fMass=mNeut;
          sQPDG=kmQPDG;         // Meson is K-
          sMass=mK;
        }
      }
    }
    else 
    {
      //KsiM: KsiM+Pi0=1456.29, Ksi0+Pi=1454.4, L+K=1609.36, Sig0+K=1686.32, SigM+K0=1695.1
      //KsiZ: Ksi0+Pi0=1449.81, KsiM+Pi=1460.9, L+K0=1613.3, Sig0+K0=1690.3, SigP+K=1683.05
      //Omeg: Omeg+Pi0=1807.43, Ksi0+K=1808.5, KsiM+K0=1818.96
      G4cout<<"-Warning-G4QE::DecBary:*AsIs* UnknBaryon(S>1)QC="<<qH->GetQC()<<G4endl;
      theQHadrons.push_back(qH);                 // Fill AsIs (delete equivalent)
      return;
    }
  }
  else 
  {
#ifdef pdebug
    G4cout<<"---Warning---G4QE::DecBary:*AsIso* UnknBaryon(AntiS) QC="<<qH->GetQC()<<G4endl;
#endif
    theQHadrons.push_back(qH);                 // Fill AsIs (delete equivalent)
    return;
  }
  G4LorentzVector f4Mom(0.,0.,0.,fMass);
  G4LorentzVector s4Mom(0.,0.,0.,sMass);
  G4double sum=fMass+sMass;
  if(fabs(qM-sum)<eps)
	 {
    f4Mom=q4M*(fMass/sum);
    s4Mom=q4M*(sMass/sum);
  }
  else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
  {
#ifdef pdebug
    G4cerr<<"---Warning---G4QE::DecBar:fPDG="<<fQPDG<<"(M="<<fMass<<")+sPDG="<<sQPDG<<"(M="
          <<sMass<<") > TotM="<<q4M.m()<<G4endl;
#endif
    if(!theEnvironment.GetA())
    {
      G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
	     if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH); // Cor or fill asItIs
      else delete qH;  
      delete quasH;
      return;
    }
    else
    {
      G4cerr<<"***G4QEnv::DecayBaryon: Can't Correct, *EmptyEnv*="<<theEnvironment<<G4endl;
      throw G4QException("G4QEnv::DecayBaryon: Baryon DecayIn2 error");
    }
  }
#ifdef pdebug
  G4cout<<"G4QEnv::DecayBaryon: *DONE* f4M="<<f4Mom<<",fPDG="<<fQPDG<<", s4M="<<s4Mom
        <<",sPDG="<<sQPDG<<G4endl;
#endif
  delete qH;
  //
  G4QHadron* H1 = new G4QHadron(fQPDG,f4Mom); // Create a Hadron for the 1-st baryon
  theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
  G4QHadron* H2 = new G4QHadron(sQPDG,s4Mom); // Create a Hadron for the 2-nd baryon
  theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
} // End of DecayBaryon

//Decay of the excited dibaryon in two baryons
void G4QEnvironment::DecayDibaryon(G4QHadron* qH)
{//  ============================================
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mSigM= G4QPDGCode(3112).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mSigP= G4QPDGCode(3222).GetMass();
  static const G4double mKsiM= G4QPDGCode(3312).GetMass();
  static const G4double mKsiZ= G4QPDGCode(3322).GetMass();
  static const G4double mDeut= G4QPDGCode(2112).GetNuclMass(1,1,0);
  static const G4double mPiN = mPi+mNeut;
  static const G4double mPiP = mPi+mProt;
  static const G4double dmPiN= mPiN+mPiN;
  static const G4double dmPiP= mPiP+mPiP;
  static const G4double nnPi = mNeut+mPiN;
  static const G4double ppPi = mProt+mPiP;
  static const G4double lnPi = mLamb+mPiN;
  static const G4double lpPi = mLamb+mPiP;
  static const G4double dNeut= mNeut+mNeut;
  static const G4double dProt= mProt+mProt;
  static const G4double dLamb= mLamb+mLamb;
  static const G4double dLaNe= mLamb+mNeut;
  static const G4double dLaPr= mLamb+mProt;
  static const G4double dSiPr= mSigP+mProt;
  static const G4double dSiNe= mSigM+mNeut;
  static const G4double dKsPr= mKsiZ+mProt;
  static const G4double dKsNe= mKsiM+mNeut;
  static const G4double eps  = 0.003;
  static const G4QNucleus vacuum(90000000);
#ifdef ppdebug
  G4cerr<<"***G4QEnvironment::DecayDibaryon: *** TEST OF EXCEPTION ***"<<G4endl;
  throw G4QException("G4QEnv::DecayDibaryon:Unknown DBary PDG code or small Mass of DB");
#endif
  G4bool four=false;                           // defFALSE for 4-particle decay of diDelta
  G4LorentzVector q4M = qH->Get4Momentum();    // Get 4-momentum of the Dibaryon
  G4int          qPDG = qH->GetPDGCode();      // PDG Code of the decaying dybaryon
  G4double         qM = q4M.m();
  G4double         rM = qM+eps;                // Just to avoid the computer accuracy
#ifdef pdebug
  G4cout<<"G4QEnv::DecayDibaryon: *Called* PDG="<<qPDG<<",4Mom="<<q4M<<", M="<<qM<<G4endl;
#endif
  // Select a chanel of the dibaryon decay (including Delta+Delta-> 4 particle decay
  G4int          fPDG = 2212;                  // Prototype for pp case
  G4int          sPDG = 2212;
  G4int          tPDG = 0;                     // Zero prototype to separate 3 from 2 
  G4double       fMass= mProt;
  G4double       sMass= mProt;
  G4double       tMass= mPi;
  if     (qPDG==90003998 && rM>=dmPiP)         // "diDelta++" case
  {
    sPDG = 211;
    sMass= mPi;
    four = true;
  }
  else if(qPDG==89998004 && rM>=dmPiN)         // "diDelta--" case
  {
    sPDG = -211;
    fPDG = 2112;
    sMass= mPi;
    fMass= mNeut;
    four = true;
  }
  else if(qPDG==90000002 && rM>=dNeut)         // "dineutron" case
  {
    fPDG = 2112;
    sPDG = 2112;
    fMass= mNeut;
    sMass= mNeut;    
  }
  else if(qPDG==90001001 && rM>=mDeut)         // "exited deutron" case
  {
    if(fabs(qM-mDeut)<eps)
	   {
      theQHadrons.push_back(qH);               // Fill as it is (delete equivalent)
      return;
	   }
    else if(mProt+mNeut<rM)
    {
      fPDG = 2112;
      fMass= mNeut;    
    }
    else
    {
      fPDG = 22;
      sPDG = 90001001;
      fMass= 0.;
      sMass= mDeut;    
    }
  }
  else if(qPDG==91000001 && rM>=dLaNe)         // "Lambda-neutron" case
  {
    fPDG = 2112;
    sPDG = 3122;
    fMass= mNeut;
    sMass= mLamb;    
  }
  else if(qPDG==91001000 && rM>=dLaPr)         // "Lambda-proton" case
  {
    sPDG = 3122;
    sMass= mLamb;    
  }
  else if(qPDG==89999003 && rM>=nnPi)         // "neutron/Delta-" case
  {
    fPDG = 2112;
    sPDG = 2112;
    tPDG = -211;
    fMass= mNeut;
    sMass= mNeut;    
  }
  else if(qPDG==90002999 && rM>=ppPi)         // "proton/Delta++" case
  {
    tPDG = 211;
  }
  else if(qPDG==90999002 && rM>=lnPi)         // "lambda/Delta-" case
  {
    fPDG = 2112;
    sPDG = 3122;
    tPDG = -211;
    fMass= mNeut;
    sMass= mLamb;    
  }
  else if(qPDG==91001999 && rM>=lpPi)         // "lambda/Delta+" case
  {
    sPDG = 3122;
    tPDG = 211;
    sMass= mLamb;    
  }
  else if(qPDG==90999002 && rM>=dSiNe)         // "Sigma-/neutron" case
  {
    fPDG = 2112;
    sPDG = 3112;
    fMass= mNeut;
    sMass= mSigM;    
  }
  else if(qPDG==91001999 && rM>=dSiPr)         // "Sigma+/proton" case
  {
    sPDG = 3222;
    sMass= mSigP;    
  }
  else if(qPDG==92000000 && rM>=dLamb)         // "diLambda" case
  {
    fPDG = 3122;
    sPDG = 3122;
    fMass= mLamb;
    sMass= mLamb;    
  }
  else if(qPDG==91999001 && rM>=dKsNe)         // "Ksi-/neutron" case
  {
    fPDG = 2112;
    sPDG = 3312;
    fMass= mNeut;
    sMass= mKsiM;    
  }
  else if(qPDG==92000999 && rM>=dKsPr)         // "Ksi0/proton" case
  {
    sPDG = 3322;
    sMass= mKsiZ;    
  }
  else if(qPDG!=90002000|| rM<dProt)           // Other possibilities (if not a default)
  {
    G4int qS = qH->GetStrangeness();
    G4int qB = qH->GetBaryonNumber();
    if(qB>0&&qS<0)                             // Antistrange diBarion
    {
      DecayAntiStrange(qH);
      return;
    }
    else
    {
#ifdef pdebug
      G4cerr<<"***G4QE::DecayDibaryon:PDG="<<qPDG<<",QC="<<qH->GetQC()<<",M="<<qM<<",Env="
            <<theEnvironment<<"nQ="<<theQuasmons.size()<<G4endl;
#endif
      if(!theEnvironment.GetA())
      {
        G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
	       if(!CheckGroundState(quasH,true))
        {
#ifdef pdebug
          G4cerr<<"***G4QE::DecDiB:GSCorFailed. FillAsItIs,n="<<theQHadrons.size()<<G4endl;
#endif
          theQHadrons.push_back(qH);        // Correction failed: fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayDiBaryon:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnvir::DecayDibar:Unknown PDG code or small Mass of DB");
      }
    }
  }
  G4LorentzVector f4Mom(0.,0.,0.,fMass);
  G4LorentzVector s4Mom(0.,0.,0.,sMass);
  G4LorentzVector t4Mom(0.,0.,0.,tMass);
  if(!tPDG&&!four)
  {
    G4double sum=fMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecDib:fPDG="<<fPDG<<"(M="<<fMass<<")+sPDG="<<sPDG<<"(M="
            <<sMass<<")="<<sum<<" >? TotM="<<q4M.m()<<q4M<<",Env="<<theEnvironment<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecDiBaryon: FillAsItIs diBaryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);      // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayDiBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("***G4QEnv::DecayDibaryon: DiBar DecayIn2 error");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecayDibaryon:(2) *DONE* f4M="<<f4Mom<<",fPDG="<<fPDG
          <<", s4M="<<s4Mom<<",sPDG="<<sPDG<<G4endl;
#endif
    delete qH;
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                 // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom); // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                 // Fill "H2" (delete equivalent)
  }
  else if(four)
  {
    q4M=q4M/2.;                                // Divided in 2 !!!
    qM/=2.;                                    // Divide the mass in 2 !
    G4double sum=fMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecDib:fPDG="<<fPDG<<"(M="<<fMass<<")+sPDG="<<sPDG<<"(M="
            <<sMass<<")"<<"="<<sum<<">tM="<<q4M.m()<<q4M<<",Env="<<theEnvironment<<G4endl;
      if(!theEnvironment.GetA())
      {
        // do not chande to q4M as it is devided by two !!
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,qH->Get4Momentum());
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecDibar:FillAsItIs DelDel QC/4M="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH); // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayDibaryon:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("***G4QEnv::DecDibaryon: Dibaryon DecayIn2 error");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecayDibaryon:(3) *DONE* f4M="<<f4Mom<<",fPDG="<<fPDG
          <<", s4M="<<s4Mom<<",sPDG="<<sPDG<<G4endl;
#endif
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                 // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom); // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                 // Fill "H2" (delete equivalent)
    // Now the second pair mus be decayed
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(!G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      // Should not be here as sum was already compared with qM above for the first delta
      G4cerr<<"***G4QE::DecDibar:fPDG="<<fPDG<<"(fM="<<fMass<<") + sPDG="<<sPDG<<"(sM="
            <<sMass<<")="<<sum<<" >? (DD2,Can't be here) TotM="<<q4M.m()<<q4M<<G4endl;
      throw G4QException("G4QEnv::DecayDibaryon: General DecayIn2 error");
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecayDibaryon:(4) *DONE* f4M="<<f4Mom<<",fPDG="<<fPDG
          <<", s4M="<<s4Mom<<",sPDG="<<sPDG<<G4endl;
#endif
    G4QHadron* H3 = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H3);                 // Fill "H1" (delete equivalent)
    G4QHadron* H4 = new G4QHadron(sPDG,s4Mom); // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H4);                 // Fill "H2" (delete equivalent)
    delete qH;
  }
  else
  {
    G4double sum=fMass+sMass+tMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
      t4Mom=q4M*(tMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecDib:fPDG="<<fPDG<<"(M="<<fMass<<")+sPDG="<<sPDG<<"(M="
            <<sMass<<")+tPDG="<<tPDG<<"(tM="<<tMass<<")="<<sum<<">TotM="<<q4M.m()<<G4endl;
      //if(theEnvironment==vacuum)
      if(!theEnvironment.GetA())
      {
        G4Quasmon* quasH = new G4Quasmon(qH->GetQC(),qH->Get4Momentum());
	       if(!CheckGroundState(quasH,true)) theQHadrons.push_back(qH); // Cor or fill asItIs
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QEnv::DecayDibaryon:Cann't Corr.*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayDibaryon: diBar DecayIn3 error");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecayDibaryon:(5) *DONE* f4M="<<f4Mom<<",fPDG="<<fPDG<<", s4M="<<s4Mom
          <<",sPDG="<<sPDG<<", t4M="<<t4Mom<<",tPDG="<<tPDG<<G4endl;
#endif
    //qH->SetNFragments(2);                    // Fill a#of fragments to decaying Dibaryon
    //theQHadrons.push_back(qH);               // Fill hadron with nf=2 (delete equivalent)
    // Instead
    delete qH;
    //
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                 // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom); // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                 // Fill "H2" (delete equivalent)
    G4QHadron* H3 = new G4QHadron(tPDG,t4Mom); // Create a Hadron for the meson
    theQHadrons.push_back(H3);                 // Fill "H3" (delete equivalent)
  }
} // End of DecayDibaryon

//Decay of the nuclear states with antistrangeness (K:/K0)
void G4QEnvironment::DecayAntiStrange(G4QHadron* qH)
{//  ===============================================
  static const G4double mK    = G4QPDGCode(321).GetMass();
  static const G4double mK0   = G4QPDGCode(311).GetMass();
  static const G4double eps   = 0.003;
  G4LorentzVector q4M = qH->Get4Momentum();    // Get 4-mom of the AntiStrangeNuclearState
  G4double         qM = q4M.m();               // Real mass of the AntiStrangeNuclearState
  G4QContent       qQC= qH->GetQC();           // PDGCode of theDecayingAntiStrangeNuclSt.
  G4int            qS = qH->GetStrangeness();  // Strangness of the AntiStrangeNuclearState
  G4int            qB = qH->GetBaryonNumber(); // BaryonNumber of the AntiStrangeNuclearSt.
  G4int            qP = qH->GetCharge();       // Charge of the AntiStranNuclState (a#of p)
#ifdef pdebug
  G4cout<<"G4QEnv::DecAntS:QC="<<qQC<<",S="<<qS<<",B="<<qB<<",C="<<qP<<",4M="<<q4M<<G4endl;
#endif
  G4int            qN = qB-qP-qS;              // a#of neuterons
  if(qS>=0 || qB<1)
  {
    G4cerr<<"G4QEnv::DecayAntiStrange:QC="<<qQC<<",S="<<qS<<",B="<<qB<<",4M="<<q4M<<G4endl;
    throw G4QException("G4QEnvironment::DecayAntiStrange: not an Anti Strange Nucleus");
  }
  G4int n1=1;         // prototype of a#of K0's
  G4double k1M=mK0;
  G4int k1PDG=311;    // K0 (as a prototype)
  G4int n2=0;         // prototype of a#of K+'s
  G4double k2M=mK;
  G4int k2PDG=321;    // K+
  G4int aS=-qS;       // -Strangness = antistrangeness
  G4int sH=aS/2;     // a small half of the antistrangeness
  G4int bH=aS-sH;    // a big half to take into account all the antistrangeness
  if(qP>0 && qP>qN)  // a#of protons > a#of neutrons
  {
    if(qP>=bH)                       // => "Enough protons in nucleus" case
    {
      if(qN>=sH)
      {
        n1=sH;
        n2=bH;
      }
      else
      {
        G4int dN=qP-qN;
        if(dN>=aS)
        {
          n1=0;
          n2=aS;
        }
        else
        {
          G4int sS=(aS-dN)/2;
          G4int bS=aS-dN-sS;
          sS+=dN;
          if(qP>=sS&&qN>=bS)
          {
            n1=bS;
            n2=sS;
          }
          else if(qP<sS)
          {
            G4int dS=aS-qP;
            n1=dS;
            n2=qP;
          }
          else
          {
            G4int dS=aS-qN;
            n1=qN;
            n2=dS;
          }
        }
      }
    }
  }
  else if(qN>=bH)
  {
    if(qP>=sH)
    {
      n2=sH;
      n1=bH;
    }
    else
    {
      G4int dN=qN-qP;
      if(dN>=aS)
      {
        n1=aS;
        n2=0;
      }
      else
      {
        G4int sS=(aS-dN)/2;
        G4int bS=aS-sS;
        if(qN>=bS&&qP>=sS)
        {
          n1=bS;
          n2=sS;
        }
        else if(qN<bS)
        {
          G4int dS=aS-qN;
          n1=qN;
          n2=dS;
        }
        else
        {
          G4int dS=aS-qP;
          n1=dS;
          n2=qP;
        }
      }
    }
  }
  G4int qPDG=90000000+(qP-n2)*1000+(qN-n1);     // PDG of the Residual Nucleus
  G4double nucM = G4QNucleus(qPDG).GetGSMass(); // Mass of the Residual Nucleus
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAnStran:nK0="<<n1<<",nK+="<<n2<<", nucM="<<nucM<<G4endl;
#endif
  G4int m1=0;                        // prototype of a#of K0's
  G4int m2=qP;                       // prototype of a#of K+'s
  if(qP>=-qS)   m2=-qS;              // Enough charge for K+'s
  else if(qP>0) m1=-qS-qP;           // Anti-Lambdas are partially compensated by neutrons
  G4int sPDG=90000000+(qP-m2)*1000+(qN-m1);     // PDG of the Residual Nucleus
  G4double mucM = G4QNucleus(sPDG).GetGSMass(); // Mass of the Residual Nucleus
  if(mucM+m1*mK+m2*mK0<nucM+n1*mK+n2*mK0)       // New is smaller
  {
    qPDG=sPDG;
    nucM=mucM;
    n1=m1;
    n2=m2;
  }
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAnStran:n1="<<n1<<",n2="<<n2<<", nM="<<nucM<<G4endl;
#endif
  if(!n1||!n2)                            // AntiKaons of only one sort are found
  {
    if(!n1)                               // No K0's only K+'s
	   {
      if(n2==1 && mK+nucM>qM+.0001)  // Mass limit: switch to K0
	     {
        k1M=mK0;
        n1=1;
        qPDG=90000000+qP*1000+qN-1;       // PDG of the Residual Nucleus
        nucM = G4QNucleus(qPDG).GetGSMass(); // Mass of the Residual Nucleus
      }
      else
      {
        k1M=mK;
        k1PDG=321;                        // Only K+'s (default K0's)
        n1=n2;                            // only n1 is used
	     }
	   }
    else                                  // No K+'s only K0's
    {
      if(n1==1 && mK0+nucM>qM+.0001) // Mass limit: switch to K+
	     {
        k1M=mK;
        k1PDG=321;                        // K+ instead of K0
        qPDG=90000000+(qP-1)*1000+qN;     // PDG of the Residual Nucleus
        nucM = G4QNucleus(qPDG).GetGSMass(); // Mass of the Residual Nucleus
      }
      else k1M=mK0;                      // Only anti-K0's (default k1PDG)
    }
#ifdef pdebug
    G4int naPDG=90000000+(qP-1)*1000+qN; // Prot PDG of the Alternative Residual Nucleus
    G4double naM=G4QNucleus(naPDG).GetGSMass(); // Prot Mass of the Alt. Residual Nucleus
    G4double kaM=mK;                     // Prot Mass of the Alternative kaon (K+)
    if(k1PDG==321)                       // Calculate alternative to K+
    {
      naPDG=90000000+qP*1000+qN-1;       // PDG of the Alternative Residual Nucleus
      naM=G4QNucleus(naPDG).GetGSMass(); // Mass of the Alt. Residual Nucleus
      kaM=mK0;                           // Prot Mass of the Alternative kaon (K0)
    }
    G4cout<<"G4QEnv::DecayAnStran:M="<<qM<<",kM="<<k1M<<"+nM="<<nucM<<"="<<k1M+nucM
          <<",m="<<kaM<<"+n="<<naM<<"="<<kaM+naM<<G4endl;
#endif
    G4double n1M=n1*k1M;
    G4LorentzVector f4Mom(0.,0.,0.,n1M);
    G4LorentzVector s4Mom(0.,0.,0.,nucM);
    G4double sum=nucM+n1M;
    if(sum>qM+eps && n1==1)              // Try to use another K if this is the only kaon
    {
      G4int naPDG=90000000+(qP-1)*1000+qN; // Prot PDG of the Alternative Residual Nucleus
      G4double naM=G4QNucleus(naPDG).GetGSMass(); // Prot Mass of the Alt. Residual Nucleus
      G4int akPDG=321;                   // Prototype PDGCode of the AlternativeKaon (K+)
      G4double kaM=mK;                   // Prototype Mass of the AlternativeKaon (K+)
      if(k1PDG==321)                     // Calculate alternative to the K+ meson
      {
        naPDG=90000000+qP*1000+qN-1;     // PDG of the Alternative Residual Nucleus
        naM=G4QNucleus(naPDG).GetGSMass(); // Mass of the Alt. Residual Nucleus
        akPDG=311;                       // PDG Code of the Alternative kaon (K0)
        kaM=mK0;                         // Mass of the Alternative kaon (K0)
      }
      G4double asum=naM+kaM;
      if(asum<sum)                       // Make a KSwap correction
      {
        nucM=naM;
        n1M=kaM;
        k1M=kaM;
        k1PDG=akPDG;
        qPDG=naPDG;
        f4Mom=G4LorentzVector(0.,0.,0.,n1M);
        s4Mom=G4LorentzVector(0.,0.,0.,nucM);
      }
    }
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(n1M/sum);
      s4Mom=q4M*(nucM/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
#ifdef pdebug
	     G4cerr<<"***G4QE::DecayAntiS:fPDG="<<n1<<"*"<<k1PDG<<"(M="<<k1M<<") + sPDG= "<<qPDG
            <<"(M="<<nucM<<" = "<<sum<<" > TotM="<<qM<<q4M<<G4endl;
#endif
      if(theEnvironment.GetA()==0)
	     {
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
		      if(!CheckGroundState(quasH,true))
        {
#ifdef chdebug
          G4cerr<<"---Warning---G4QE::DecAntiStran:Failed FillAsItIs h="<<qQC<<q4M<<G4endl;
#endif
          theQHadrons.push_back(qH);// @@ Can cause problems with ParticleConversion in G4
        }
        else delete qH;  
        delete quasH;
        return;
      }
	     else
      {
#ifdef pdebug
        G4cerr<<"---Warning---G4QE::DAS:AsItIsE="<<theEnvironment<<",h="<<qQC<<q4M<<",qM="
              <<qM<<" < sum="<<sum<<"=(F)"<<nucM<<"+(kK)"<<n1M<<G4endl;
#endif
        theQHadrons.push_back(qH);  // @@ Can cause problems with particle conversion in G4
        return;
        //throw G4QException("G4QE::DecayAntiStrange:AntStrangeNuc DecIn2 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecAntiS: nK+N "<<n1<<"*K="<<k1PDG<<f4Mom<<",N="<<qPDG<<s4Mom<<G4endl;
#endif
    delete qH;
    //
    f4Mom/=n1;
    for(G4int i1=0; i1<n1; i1++)
    {
      G4QHadron* H1 = new G4QHadron(k1PDG,f4Mom); // Create a Hadron for the Kaon
      theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
	   }
    G4QHadron* H2 = new G4QHadron(qPDG,s4Mom);   // Create a Hadron for the Nucleus
    //theQHadrons.push_back(H2);                 // Fill "H2" (delete equivalent)
    EvaporateResidual(H2);                       // Fill "H2" (delete equivalent)
#ifdef debug
    G4cout<<"G4QEnv::DecAntiS:*** After EvaporateResidual nH="<<theQHadrons.size()<<G4endl;
#endif
  }
  else
  {
    G4double n1M=n1*k1M;
    G4double n2M=n2*k2M;
    G4LorentzVector f4Mom(0.,0.,0.,n1M);
    G4LorentzVector s4Mom(0.,0.,0.,n2M);
    G4LorentzVector t4Mom(0.,0.,0.,nucM);
    G4double sum=nucM+n1M+n2M;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(n1M/sum);
      s4Mom=q4M*(n2M/sum);
      t4Mom=q4M*(nucM/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom))
    {
      G4cerr<<"---Warning---G4QE::DeAS:nPDG="<<qPDG<<"(M="<<nucM<<")+1="<<k1PDG<<"(M="<<k1M
            <<")+2="<<k2PDG<<"(M="<<k2M<<")="<<nucM+n1*k1M+n2*k2M<<">tM="<<qM<<q4M<<G4endl;
      if(theEnvironment.GetA()==0)
	     {
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
		      if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAntiS:NoCorDone,FillAsItIs h="<<q4M<<qQC<<G4endl;
          theQHadrons.push_back(qH); //@@ Can cause problems with particle conversion in G4
        }
        else delete qH;  
        delete quasH;
        return;
      }
	     else
      {
        G4cerr<<"---Warning---G4QE::DAS: NoCorrDone,FillAsItIs E="<<theEnvironment<<G4endl;
        theQHadrons.push_back(qH); // @@ Can cause problems with particle conversion in G4
        return;
        //throw G4QException("G4QEnv::DecayAntiStrange:AntiStrangeNucleus DecIn3 error");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecAntiS:*DONE* nPDG="<<qPDG<<",1="<<f4Mom<<",2="<<s4Mom<<",n="<<t4Mom
          <<G4endl;
#endif
    delete qH;
    //
    f4Mom/=n1;
    for(G4int i1=0; i1<n1; i1++)
    {
      G4QHadron* H1 = new G4QHadron(k1PDG,f4Mom); // Create a Hadron for the K0
      theQHadrons.push_back(H1);               // Fill "H1" (delete equivalent)
	   }
    s4Mom/=n2;
    for(G4int i2=0; i2<n2; i2++)
    {
      G4QHadron* H2 = new G4QHadron(k2PDG,s4Mom); // Create a Hadron for the K+
      theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
	   }
    G4QHadron* H3 = new G4QHadron(qPDG,t4Mom);    // Create a Hadron for the nucleus
    //theQHadrons.push_back(H3);                  // Fill "H3" (delete equivalent)
    EvaporateResidual(H3);                        // Fill "H3" (delete equivalent)
  }
#ifdef debug
  G4cout<<"G4QEnv::DecAntiS: ===> End of DecayAntiStrangness"<<G4endl;
#endif
} // End of DecayAntiStrange

//Decay of the excited 3p or 3n systems in three baryons
void G4QEnvironment::DecayMultyBaryon(G4QHadron* qH)
{//  ===============================================
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double eps=0.003;
  G4LorentzVector q4M = qH->Get4Momentum();       // Get 4-momentum of the MultyBaryon
  G4double         qM = q4M.m();                  // Mass of the Multybaryon
  G4int          qPDG = qH->GetPDGCode();         // PDG Code of the decaying multybar
  G4QContent      qQC = qH->GetQC();              // PDG Code of the decaying multibar
#ifdef pdebug
  G4cout<<"G4QEnv::DecayMultyBaryon: *Called* PDG="<<qPDG<<",4M="<<q4M<<qQC<<G4endl;
#endif
  G4int totS=qQC.GetStrangeness();  //  Total Strangeness       (L)                ^
  G4int totC=qQC.GetCharge();       //  Total Charge            (p)                ^
  G4int totBN=qQC.GetBaryonNumber();// Total Baryon Number      (A)                ^
  G4int totN=totBN-totS-totC;       // Total Number of Neutrons (n)                ^
  G4int          fPDG = 3122;       // Prototype for A lambdas case
  G4double       fMass= mLamb;
  if     (totN==totBN)              // "A-neutron" case
  {
    fPDG = 2112;
    fMass= mNeut;
  }
  else if(totC==totBN)              // "A-protons" case
  {
    fPDG = 2212;
    fMass= mProt;
  }
  else if(totS!=totBN)            // "Bad call" case
  {
    G4cerr<<"***G4QEnv::DecayMultyBaryon: PDG="<<qPDG<<G4endl;
    throw G4QException("***G4QEnv::DecayMultyBaryon: Can not decay this PDG Code");
  }
#ifdef pdebug
  else
  {
    G4cerr<<"**G4QEnv::DecayMultyBaryon: PDG="<<qPDG<<G4endl;
    throw G4QException("***G4QEnv::DecayMultyBaryon: Unknown PDG code of the MultiBaryon");
  }
#endif
  if(totBN==1)
  {
    theQHadrons.push_back(qH);
    //return;
  }
  else if(totBN==2)
  {
    G4LorentzVector f4Mom(0.,0.,0.,fMass);
    G4LorentzVector s4Mom(0.,0.,0.,fMass);
    G4double sum=fMass+fMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M/2.;
      s4Mom=f4Mom;
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QEnv::DecayMultyBar:fPDG="<<fPDG<<"(fM="<<fMass<<")*2="<<sum
            <<" > TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecMultyBar:FillAsItIsDiBar(I=1)="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);               // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QEnv::DecayMultyBaryon:Cann'tCor*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayMultyBaryon:diBaryon DecayIn2 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecMBar:*DONE* fPDG="<<fPDG<<",f="<<f4Mom<<",s="<<s4Mom<<G4endl;
#endif
    delete qH;
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);   // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                   // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(fPDG,s4Mom);   // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                   // Fill "H2" (delete equivalent)
  }
  else if(totBN==3)
  {
    G4LorentzVector f4Mom(0.,0.,0.,fMass);
    G4LorentzVector s4Mom(0.,0.,0.,fMass);
    G4LorentzVector t4Mom(0.,0.,0.,fMass);
    G4double sum=fMass+fMass+fMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M/3.;
      s4Mom=f4Mom;
      t4Mom=f4Mom;
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom))
    {
      G4cerr<<"---Warning---G4QEnv::DecayMultyBaryon: fPDG="<<fPDG<<"(fM="<<fMass<<")*3 = "
            <<3*fMass<<" >? TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QEnv::DecMultyBar:FillAsItIs 3Baryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);             // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayMultyBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayMultyBar:ThreeBaryon DecayIn3 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecMBar:*DONE*, fPDG="<<fPDG<<",f="<<f4Mom<<",s="<<s4Mom<<",t="
          <<t4Mom<<G4endl;
#endif
    delete qH;
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);   // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                   // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(fPDG,s4Mom);   // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                   // Fill "H2" (delete equivalent)
    G4QHadron* H3 = new G4QHadron(fPDG,t4Mom);   // Create a Hadron for the 3-d baryon
    theQHadrons.push_back(H3);                   // Fill "H3" (delete equivalent)
  }
  else
  {
    // @@It must be checked, that they are not under the mass shell
    // !! OK !! Checked by the warning print that they are mostly in the Ground State !!
    G4LorentzVector f4Mom=q4M/totBN; // @@ Too simple solution (split in two parts!)
#ifdef pdebug
    // Warning for the future development
    G4cout<<"**G4QE::DecMulBar:SplitMultiBar inEqParts M="<<totBN<<"*"<<f4Mom.m()<<G4endl;
    G4cout<<"G4QEnv::DecMBar:*DONE* fPDG="<<fPDG<<",f="<<f4Mom<<G4endl;
#endif
    delete qH;
    for(G4int h=0; h<totBN; h++)
    {
      G4QHadron* H1 = new G4QHadron(fPDG,f4Mom); // Create a Hadron for the baryon
      theQHadrons.push_back(H1);                 // Fill "H1" (delete equivalent)
	   }
  }
} // End of DecayMultyBaryon

//Decay of the excited alpha+2p or alpha+2n systems
void G4QEnvironment::DecayAlphaDiN(G4QHadron* qH)
{//  ============================================
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mAlph= G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double mHel6= G4QPDGCode(2112).GetNuclMass(2,4,0);
  static const G4double eps=0.003;
  G4LorentzVector q4M = qH->Get4Momentum();      // Get 4-momentum of the AlphaDibaryon
  G4double         qM = q4M.m();                 // Real mass of the AlphaDibaryon
  G4int          qPDG = qH->GetPDGCode();        // PDG Code of the decayin AlphaDybaryon
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAlphaDiN: *Called* PDG="<<qPDG<<",4M="<<q4M<<G4endl;
#endif
  G4int          fPDG = 2212;                    // Prototype for alpha+pp case
  G4double       fMass= mProt;
  G4int          sPDG = 90002002;
  G4double       sMass= mAlph;
  if     (qPDG==90002004)                        // "alpha+2neutrons" case
  {
    if(fabs(qM-mHel6)<eps)
	   {
      theQHadrons.push_back(qH);                 // Fill as it is (delete equivalent)
      return;
	   }
    else if(mNeut+mNeut+mAlph<qM)
    {
      fPDG = 2112;
      fMass= mNeut;
	   }
    else
    {
      G4cerr<<"***G4QEn::DecAlDiN:M(He6="<<mHel6<<")="<<qM<<"<"<<mNeut+mNeut+mAlph<<G4endl;
      throw G4QException("G4QEnv::DecayAlphaDiN: Cannot decay excited He6 with this mass");
    }
  }
  else if(qPDG!=90004002)                         // "Bad call" case
  {
    G4cerr<<"***G4QEnv::DecayAlphaDiN: PDG="<<qPDG<<G4endl;
    throw G4QException("G4QEnv::DecayAlphaDiN: Can not decay this PDG Code");
  }
  G4LorentzVector f4Mom(0.,0.,0.,fMass);
  G4LorentzVector s4Mom(0.,0.,0.,fMass);
  G4LorentzVector t4Mom(0.,0.,0.,sMass);
  G4double sum=fMass+fMass+sMass;
  if(fabs(qM-sum)<eps)
  {
    f4Mom=q4M*(fMass/sum);
    s4Mom=f4Mom;
    t4Mom=q4M*(sMass/sum);
  }
  else if(qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom))
  {
    G4int eA=theEnvironment.GetA();
    G4cerr<<"---Warning---G4QEnv::DecayAlphaDiN:fPDG="<<fPDG<<"(M="<<fMass<<")*2+mAlpha = "
          <<sum<<" >? TotM="<<qM<<q4M<<", d="<<sum-qM<<", envA="<<eA<<G4endl;
    //if(!eA)
    //{
      G4QContent qQC=qH->GetQC();
      G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	     if(!CheckGroundState(quasH,true))
      {
        G4cerr<<"---Warning---G4QEnv::DecayAlphaDiN:FillAsItIs AlphaNN="<<qQC<<q4M<<G4endl;
        theQHadrons.push_back(qH);      // Fill as it is
      }
      else delete qH;  
      delete quasH;
      return;
	   //}
    //else
    //{
    //  G4cerr<<"***G4QEnv::DecayAlphaDiN:CorrectionWithEmptyEnv="<<theEnvironment<<G4endl;
    //  throw G4QException("G4QEnv::DecayAlphaDiN: Alpha+N+N DecayIn3 error");
    //}
  }
#ifdef pdebug
  G4cout<<"G4QE::DecAl2N: fPDG="<<fPDG<<",f="<<f4Mom<<",s="<<s4Mom<<",t="<<t4Mom<<G4endl;
#endif
  delete qH;
  G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);    // Create a Hadron for the 1-st baryon
  theQHadrons.push_back(H1);                    // Fill "H1" (delete equivalent)
  G4QHadron* H2 = new G4QHadron(fPDG,s4Mom);    // Create a Hadron for the 2-nd baryon
  theQHadrons.push_back(H2);                    // Fill "H2" (delete equivalent)
  G4QHadron* H3 = new G4QHadron(sPDG,t4Mom);    // Create a Hadron for the alpha
  theQHadrons.push_back(H3);                    // Fill "H3" (delete equivalent)
} // End of DecayAlphaDiN

//Decay of the excited alpha+bayon state in alpha and baryons
void G4QEnvironment::DecayAlphaBar(G4QHadron* qH)
{//  ============================================
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mAlph= G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double mTrit= G4QPDGCode(2112).GetNuclMass(1,2,0);
  static const G4double mHe3 = G4QPDGCode(2112).GetNuclMass(2,1,0);
  static const G4double eps=0.003;
  G4LorentzVector q4M = qH->Get4Momentum();     // Get 4-momentum of the Alpha-Baryon
  G4double         qM = q4M.m();                // Mass of Alpha-Baryon
  G4int          qPDG = qH->GetPDGCode();       // PDG Code of the decayin Alpha-Baryon
  G4QContent      qQC = qH->GetQC();            // PDG Code of the decaying Alpha-Bar
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAlphaBar: *Called* PDG="<<qPDG<<",4M="<<q4M<<qQC<<G4endl;
#endif
  G4int totS=qQC.GetStrangeness();              //  Total Strangeness       (L)
  G4int totC=qQC.GetCharge();                   //  Total Charge            (p)
  G4int totBN=qQC.GetBaryonNumber();            // Total Baryon Number      (A)
  if((!totS&&!totC||totC==totBN||totS==totBN)&&totBN>1)
  {
    DecayMultyBaryon(qH);
    //return;
  }
  else if(qPDG==92001002||qPDG==92002001||qPDG==91003001||qPDG==91001003||qPDG==93001001)
  {
    theQHadrons.push_back(qH);
    //return;
  }
  else if(qPDG==92000003||qPDG==92003000||qPDG==93000002||qPDG==93002000)
  {
    G4int          fPDG = 3122;                 // 1st Prototype for 2L+3n case
    G4double       fMass= mLamb;
    G4int          sPDG = 2112;
    G4double       sMass= mNeut;
    if     (qPDG==92003000)                     // "2L+3p" case
    {
      sPDG = 2212;
      sMass= mProt;
    }
    else if(qPDG==93000002)                     // "2n+3L" case
    {
      fPDG = 2112;
      fMass= mNeut;
      sPDG = 3122;
      sMass= mLamb;
    }
    else if(qPDG==93002000)                     // "2p+3L" case
    {
      fPDG = 2212;
      fMass= mProt;
      sPDG = 3122;
      sMass= mLamb;
    }
    G4double tfM=fMass+fMass;
    G4double tsM=sMass+sMass+sMass;
    G4LorentzVector f4Mom(0.,0.,0.,tfM);
    G4LorentzVector s4Mom(0.,0.,0.,tsM);
    G4double sum=tfM+tsM;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(tfM/sum);
      s4Mom=q4M*(tsM/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecAlB:fPDG="<<fPDG<<"(M="<<fMass<<")*2="<<2*fMass<<",s="
            <<sPDG<<"(sM="<<sMass<<")*3="<<3*sMass<<"="<<sum<<">M="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAlphBar:FillAsItIsAlphaBaryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);            // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayAlphaBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayAlphaBar: DecayIn2 didn't succeed for 3/2");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecAB:*DONE*, fPDG="<<fPDG<<f4Mom<<",sPDG="<<sPDG<<s4Mom<<G4endl;
#endif
    delete qH;
    G4LorentzVector rf4Mom=f4Mom/2;
    G4QHadron* H1 = new G4QHadron(fPDG,rf4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    G4LorentzVector rs4Mom=s4Mom/3;
    G4QHadron* H2 = new G4QHadron(sPDG,rs4Mom); // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
    theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
    theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
  }
  else if(qPDG==90004001||qPDG==90001004)
  {
    G4int          fPDG = 90002001;             // Prototype for "He3+2p" case
    G4double       fMass= mHe3;
    G4int          sPDG = 2212;
    G4double       sMass= mProt;
    if     (qPDG==90001004)                     // "t+2n" case
    {
      fPDG = 90001002;
      fMass= mTrit;
      sPDG = 2112;
      sMass= mNeut;
    }
    G4LorentzVector f4Mom(0.,0.,0.,fMass);
    G4LorentzVector s4Mom(0.,0.,0.,sMass);
    G4LorentzVector t4Mom(0.,0.,0.,sMass);
    G4double sum=fMass+sMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
      t4Mom=s4Mom;
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn3(f4Mom, s4Mom, t4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecAlB:fPDG="<<fPDG<<",M="<<fMass<<",sPDG="<<sPDG<<",sM="
            <<sMass<<",2sM+fM="<<2*sMass+fMass<<" > TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAlphBar:FillAsItIsAlphaBaryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);      // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayAlphaBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayAlphaBar: t/nn,He3/pp DecayIn3 didn't");
      }
    }
#ifdef pdebug
    G4cout<<"G4QE::DecAlB:*DONE*, f="<<fPDG<<f4Mom<<", s="<<sPDG<<s4Mom<<t4Mom<<G4endl;
#endif
    delete qH;
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);   // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                   // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom);   // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                   // Fill "H2" (delete equivalent)
    G4QHadron* H3 = new G4QHadron(sPDG,t4Mom);   // Create a Hadron for the 3-d baryon
    theQHadrons.push_back(H3);                   // Fill "H3" (delete equivalent)
  }
  else if(qPDG==94000001||qPDG==94001000||qPDG==91000004||qPDG==91004000)
  {
    G4int          fPDG = 3122;                 // Prototype for "4L+n" case
    G4double       fMass= mLamb+mLamb;
    G4int          sPDG = 2112;
    G4double       sMass= mNeut;
    if     (qPDG==94001000)                     // "4L+p" case
    {
      sPDG = 2212;
      sMass= mProt;
    }
    else if(qPDG==91000004)                     // "4n+L" case
    {
      fPDG = 2112;
      fMass= mNeut+mNeut;
      sPDG = 3122;
      sMass= mLamb;
    }
    else if(qPDG==91004000)                     // "4p+L" case
    {
      fPDG = 2212;
      fMass= mProt+mProt;
      sPDG = 3122;
      sMass= mLamb;
    }
    G4LorentzVector f4Mom(0.,0.,0.,fMass+fMass);
    G4LorentzVector s4Mom(0.,0.,0.,sMass);
    G4double sum=fMass+fMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*((fMass+fMass)/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecAlphBar:fPDG="<<fPDG<<"(2*fM="<<fMass<<")*2="<<2*fMass
            <<",sPDG="<<sPDG<<"(sM="<<sMass<<" > TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAlphBar:FillAsItIsAlphaBaryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);            // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayAlphaBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayAlphaBar:QuintBaryon DecayIn2 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::Dec5B:*DONE*, fPDG="<<fPDG<<f4Mom<<",sPDG="<<sPDG<<s4Mom<<G4endl;
#endif
    delete qH;
    G4LorentzVector rf4Mom=f4Mom/4;
    G4QHadron* H1 = new G4QHadron(fPDG,rf4Mom); // Create a Hadron for the 1-st baryon
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    theQHadrons.push_back(H1);                  // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom);  // Create a Hadron for the 2-nd baryon
    theQHadrons.push_back(H2);                  // Fill "H2" (delete equivalent)
  }
  else if(qPDG==90003002||qPDG==90002003||qPDG==91002002)
  {
    G4int          fPDG = 90002002;             // Prototype for "alpha+n" case
    G4int          sPDG = 2112;
    G4double       fMass= mAlph;
    G4double       sMass= mNeut;
    if(qPDG==90003002)                          // "alpha+p" case
    {
      sPDG = 2212;
      sMass= mProt;    
    }
    else if(qPDG==9100202)                      // "alpha+l" case
    {
      sPDG = 3122;
      sMass= mLamb;    
    }
    else if(qPDG!=90002003)
    {
      theQHadrons.push_back(qH);              // Fill hadron as it is (delete equivalent)
      //EvaporateResidual(qH);                // Evaporate ResNuc (delete equivivalent) ??
      return;
    }
    G4double dM=fMass+sMass-qM;
    if(dM>0.&&dM<1.)
    {
#ifdef pdebug
      G4cerr<<"***Corrected***G4QEnv::DecayAlphaBar:fPDG="<<fPDG<<"(fM="<<fMass<<")+ sPDG="
            <<sPDG<<"(sM="<<sMass<<")="<<fMass+sMass<<" > TotM="<<qM<<q4M<<G4endl;
#endif
      G4double hdM=dM/2;
      fMass-=hdM;
      sMass-=hdM;
    }
    G4LorentzVector f4Mom(0.,0.,0.,fMass);
    G4LorentzVector s4Mom(0.,0.,0.,sMass);      // Mass is random since probab. time
    G4double sum=fMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QE::DecAlphaBar:fPDG="<<fPDG<<"(fM="<<fMass<<")+sPDG="<<sPDG
            <<"(sM="<<sMass<<")="<<fMass+sMass<<"="<<sum<<" > TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	       if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAlphBar:FillAsItIsAlphaBaryon="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);      // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QE::DecayAlphaBar:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("***G4QE::DecayAlphaBar:Alpha+Baryon DecIn2 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecAlBar:*DONE*a4M="<<f4Mom<<",s4M="<<s4Mom<<",sPDG="<<sPDG<<G4endl;
#endif
    delete qH;
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);      // Create a Hadron for the alpha
    theQHadrons.push_back(H1);                      // Fill "H1" (delete equivalent)
    G4QHadron* H2 = new G4QHadron(sPDG,s4Mom);      // Create a Hadron for the baryon
    theQHadrons.push_back(H2);                      // Fill "H2" (delete equivalent)
  }
  else G4cerr<<"---Warning---G4QEnv::DecayAlphaBar: Unknown PDG="<<qPDG<<G4endl;
} // End of DecayAlphaBar

//Decay of the excited alpha+alpha state in 2 alphas
void G4QEnvironment::DecayAlphaAlpha(G4QHadron* qH)
{//  ==============================================
  static const G4double mAlph= G4QPDGCode(2112).GetNuclMass(2,2,0);
  static const G4double aaGSM= G4QPDGCode(2112).GetNuclMass(4,4,0);
  static const G4double eps=0.003;
  G4int          qPDG = qH->GetPDGCode();         // PDG Code of the decayin dialpha
  if(qPDG!=90004004)
  {
    G4cerr<<"***G4QEnv::DecayAlphaAlpha: qPDG="<<qPDG<<G4endl;
    throw G4QException("***G4QEnv::DecayAlphaAlpha: Not Be8 state decais in 2 alphas");
  }
  G4LorentzVector q4M = qH->Get4Momentum();       // Get 4-momentum of the Dibaryon
  G4double qM=q4M.m();
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAlAl: *Called* PDG="<<qPDG<<",M="<<qM<<q4M<<">"<<aaGSM<<G4endl;
#endif
  //if(qM>aaGSM+.01)  // @@ Be8*->gamma+Be8 (as in evaporation)
  if(2>3)
  {
    G4int          fPDG = 22;
    G4int          sPDG = 90004004;
    G4double       fMass= 0.;
    G4double       sMass= aaGSM;
    G4LorentzVector f4Mom(0.,0.,0.,fMass);
    G4LorentzVector s4Mom(0.,0.,0.,sMass);          // Mass is random since probab. time
    G4double sum=fMass+sMass;
    if(fabs(qM-sum)<eps)
	   {
      f4Mom=q4M*(fMass/sum);
      s4Mom=q4M*(sMass/sum);
    }
    else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
    {
      G4cerr<<"---Warning---G4QEnv::DecayAlphaAlpha:gPDG="<<fPDG<<"(gM="<<fMass<<")+PDG="
            <<sPDG<<"(sM="<<sMass<<")="<<sum<<" > TotM="<<q4M.m()<<q4M<<G4endl;
      if(!theEnvironment.GetA())
      {
        G4QContent qQC=qH->GetQC();
        G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	    if(!CheckGroundState(quasH,true))
        {
          G4cerr<<"---Warning---G4QE::DecAlphAlph:FillAsItIsAlphaAlpha="<<qQC<<q4M<<G4endl;
          theQHadrons.push_back(qH);      // Fill as it is
        }
        else delete qH;  
        delete quasH;
        return;
      }
      else
      {
        G4cerr<<"***G4QEnv::DecayAlphAlph:Cann't Corr.*EmptyEnv*="<<theEnvironment<<G4endl;
        throw G4QException("G4QEnv::DecayAlphaAlpha: g+diAlph DecayIn2 didn't succeed");
      }
    }
#ifdef pdebug
    G4cout<<"G4QEnv::DecayAlphaAlpha: *DONE* gam4M="<<f4Mom<<", aa4M="<<s4Mom<<G4endl;
#endif
    G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);      // Create a Hadron for the 1-st alpha
    theQHadrons.push_back(H1);                      // Fill "H1" (delete equivalent)
    qH->Set4Momentum(s4Mom);
    q4M=s4Mom;
  }
  G4int          fPDG = 90002002;
  G4int          sPDG = 90002002;
  G4double       fMass= mAlph;
  G4LorentzVector f4Mom(0.,0.,0.,fMass);
  G4LorentzVector s4Mom(0.,0.,0.,fMass);
  G4double sum=fMass+fMass;
  if(fabs(qM-sum)<eps)
  {
    f4Mom=q4M*(fMass/sum);
    s4Mom=f4Mom;
  }
  else if(qM<sum || !G4QHadron(q4M).DecayIn2(f4Mom, s4Mom))
  {
    G4cerr<<"---Warning---G4QEnv::DecayAlphaAlpha:fPDG="<<fPDG<<"(fM="<<fMass<<")*2="<<sum
          <<" > TotM="<<q4M.m()<<q4M<<G4endl;
    if(!theEnvironment.GetA())
    {
      G4QContent qQC=qH->GetQC();
      G4Quasmon* quasH = new G4Quasmon(qQC,q4M);
	  if(!CheckGroundState(quasH,true))
      {
        G4cerr<<"---Warning---G4QE::DecAlphaAlpha:FillAsItIsAlphaAlpha="<<qQC<<q4M<<G4endl;
        theQHadrons.push_back(qH);      // Fill as it is
      }
      else delete qH;  
      delete quasH;
      return;
    }
    else
    {
      G4cerr<<"***G4QEnv::DecayAlphAlph:Cann't Correct*EmptyEnv*="<<theEnvironment<<G4endl;
      throw G4QException("G4QEnv::DecayAlphaAlpha: diAlpha DecayIn2 didn't succeed");
    }
  }
#ifdef pdebug
  G4cout<<"G4QEnv::DecayAlphaAlpha: *DONE* fal4M="<<f4Mom<<", sal4M="<<s4Mom<<G4endl;
#endif
  delete qH;
  G4QHadron* H1 = new G4QHadron(fPDG,f4Mom);      // Create a Hadron for the 1-st alpha
  theQHadrons.push_back(H1);                      // Fill "H1" (delete equivalent)
  G4QHadron* H2 = new G4QHadron(sPDG,s4Mom);      // Create a Hadron for the 2-nd alpha
  theQHadrons.push_back(H2);                      // Fill "H2" (delete equivalent)
} // End of DecayAlphaAlpha

// Check that it's possible to decay the TotalResidualNucleus in Quasmon+Environ & correct
// In case of correction the "quasm" is never deleted! If corFlag==true - correction.
G4bool G4QEnvironment::CheckGroundState(G4Quasmon* quasm, G4bool corFlag)
{ //   ==================================================================
  static const G4QPDGCode gamQPDG(22);
  static const G4QContent neutQC(2,1,0,0,0,0);
  static const G4QContent protQC(1,2,0,0,0,0);
  static const G4QContent lambQC(1,1,1,0,0,0);
  static const G4QContent pimQC(1,0,0,0,1,0);
  static const G4QContent pipQC(0,1,0,1,0,0);
  static const G4double mNeut= G4QPDGCode(2112).GetMass();
  static const G4double mProt= G4QPDGCode(2212).GetMass();
  static const G4double mLamb= G4QPDGCode(3122).GetMass();
  static const G4double mPi  = G4QPDGCode(211).GetMass();
  //static const G4double dmPi  = mPi+mPi;
  ///@@@///
  ///////////////corFlag=true;
  ///@@@///
  G4QContent valQ=quasm->GetQC();               // Quark content of the Quasmon
  G4int    resQPDG=valQ.GetSPDGCode();          // Reachable in a member function
  G4int    resB=valQ.GetBaryonNumber();         // Baryon number of the Quasmon
  G4int    resC=valQ.GetCharge();               // Charge of the Quasmon
  G4int    resS=valQ.GetStrangeness();          // Strangeness of the Quasmon
  if(resQPDG==10 && resB>0) resQPDG=valQ.GetZNSPDGCode();
  G4double resQMa=G4QPDGCode(resQPDG).GetMass();// GS Mass of the Residual Quasmon
  G4double resEMa=0.;                           // GS Mass of the EmptyResidualEnvironment
  G4bool   bsCond=false;                        // FragSeparatCondition for QuasmonInVacuum
  G4LorentzVector enva4M=G4LorentzVector(0.,0.,0.,0.); // Prototype of the environment Mass
  G4QContent reTQC=valQ;                        // Prototype QuarkContent of the ResidNucl
  G4LorentzVector reTLV=quasm->Get4Momentum();  // Prototyoe 4-Mom of the Residual Nucleus
  G4double reTM=reTLV.m();                      // Real mass of the Quasmon
  G4int envPDG=theEnvironment.GetPDG();
  if(resB>1 && (!resS && (resC==resB && reTM>resC*mProt || !resC && reTM>resB*mNeut)
	            || resS==resB && reTM>resS*mLamb) ) // Immediate Split(@@Decay) MultiBaryon
  {
#ifdef chdebug
    G4cout<<"G4QE::CGS:*MultyBar*E="<<envPDG<<",B="<<resB<<",C="<<resC<<",S"<<resS<<G4endl;
#endif
    if(envPDG!=90000000)
    {
      resEMa=theEnvironment.GetMZNS();          // GSMass of the Residual Environment
      enva4M=theEnvironment.Get4Momentum();     // 4-Mom of the Residual Environment
      G4LorentzVector toLV=reTLV+enva4M;        // Total 4-mom for decay
      enva4M=G4LorentzVector(0.,0.,0.,resEMa);  // GSM of the Environment
      reTLV=G4LorentzVector(0.,0.,0.,resQMa);   // GSM of the Quasmon
      if(G4QHadron(toLV).DecayIn2(reTLV,enva4M))
	     {
#ifdef pdebug
        G4cout<<"G4QE::CGS: fill EnvPDG="<<envPDG<<",4M="<<enva4M<<" and continue"<<G4endl;
#endif
        theQHadrons.push_back(new G4QHadron(envPDG,enva4M)); // (delete equivalent)
        theEnvironment=G4QNucleus(G4QContent(0,0,0,0,0,0), G4LorentzVector(0.,0.,0.,0.));
      }
      else G4cerr<<"*G4QE::CGS:tM="<<toLV.m()<<toLV<<"<q="<<resQMa<<"+EM="<<resEMa<<G4endl;
    }
    G4int  baPDG=3122;                          // Prototype for MultiLambda
    if(!resS) baPDG = (!resC) ? 2112 : 2212;    // MultiNeutron or MultiProton
#ifdef pdebug
    G4cout<<"G4QE::CGS: fill "<<resB<<" of "<<baPDG<<" with t4M="<<reTLV<<G4endl;
#endif
    reTLV/=resB;                                // Recalculate! Anyway go out...
    for(G4int ib=0; ib<resB; ib++) theQHadrons.push_back(new G4QHadron(baPDG,reTLV));
    return true;
  }
#ifdef cdebug
  if(resQPDG==89998005)
   G4cout<<"G4QE::CGS:Q="<<valQ<<resQPDG<<",GM="<<resQMa<<",4M="<<reTLV<<reTLV.m()<<G4endl;
#endif
  G4double resSMa=resQMa;                       // Prototype MinimalSplitMass of ResidNucl
  enva4M=theEnvironment.Get4Momentum();         // 4-Mom of the Residual Environment
  if(envPDG!=90000000 && fabs(enva4M.m2())>1.)  // => "Environment is not vacuum" case
  { // @@@@@@@@@@@@@@@@@@@ CALL SUBROUTINE ? @@@@@@@@@
    resEMa=theEnvironment.GetMZNS();            // GSMass of the Residual Environment
#ifdef cdebug
	   G4cout<<"G4QE::CGS:EnvironmentExists,gsM="<<resEMa<<",4M="<<enva4M<<enva4M.m()<<G4endl;
#endif
    reTQC+=theEnvironment.GetQCZNS();           // Quark content of the Residual Nucleus
    reTLV+=enva4M;                              // 4-Mom of Residual Nucleus
    //resSMa+=resEMa;                           // Minimal Split Mass of Residual Nucleus
    resSMa=G4QPDGCode(reTQC).GetMass();         // GS Mass of the Residual Quasmon+Environ
  }
  else                                          // Calculate BaryonSeparatCond for vacQuasm
  {
    G4double resQM=reTLV.m();                   // CM Mass of the Residual vacQuasmon
    G4int    baryn=valQ.GetBaryonNumber();      // Baryon Number of the Residual vacQuasmon
    if(baryn>1)                                 // => "Can split baryon?"
	   {
      if(valQ.GetN())                           // ===> "Can split neutron?"
	     {
        G4QContent resQC=valQ-neutQC;           // QC of Residual for the Neutron
        G4int    resPDG=resQC.GetSPDGCode();    // PDG of Residual for the Neutron
        if(resPDG==10&&resQC.GetBaryonNumber()>0) resPDG=resQC.GetZNSPDGCode();
        G4double resMas=G4QPDGCode(resPDG).GetMass(); // GS Mass of the Residual
        if(resMas+mNeut<resQM) bsCond=true;
	     }
      else if(valQ.GetP())                      // ===> "Can split proton?"
	     {
        G4QContent resQC=valQ-protQC;           // QC of Residual for the Proton
        G4int    resPDG=resQC.GetSPDGCode();    // PDG of Residual for the Proton
        if(resPDG==10&&resQC.GetBaryonNumber()>0) resPDG=resQC.GetZNSPDGCode();
        G4double resMas=G4QPDGCode(resPDG).GetMass(); // GS Mass of the Residual
        if(resMas+mProt<resQM) bsCond=true;
	     }
      else if(valQ.GetL())                      // ===> "Can split Lambda?"
	     {
        G4QContent resQC=valQ-lambQC;           // QC of Residual for the Lambda
        G4int    resPDG=resQC.GetSPDGCode();    // PDG of Residual for the Lambda
        if(resPDG==10&&resQC.GetBaryonNumber()>0) resPDG=resQC.GetZNSPDGCode();
        G4double resMas=G4QPDGCode(resPDG).GetMass(); // GS Mass of the Residual
        if(resMas+mLamb<resQM) bsCond=true;
	     }
	   }
  }
  G4double resTMa=reTLV.m();                    // CM Mass of the ResidualNucleus (Q+Env)
  //if(resTMa>resSMa && (resEMa || bsCond)) return true;// Why not ?? @@ (See G4Q the same)
  G4int nOfOUT = theQHadrons.size();            // Total #of QHadrons at this point
#ifdef cdebug
  G4int    reTPDG=reTQC.GetSPDGCode();
  if(reTPDG==10&&reTQC.GetBaryonNumber()>0) reTPDG=reTQC.GetZNSPDGCode();
  G4cout<<"G4QEnv::CheckGS:(tM="<<resTMa<<"<rQM+rEM="<<resSMa<<",d="<<resSMa-resTMa
        <<" || rEM="<<resEMa<<"=0 & "<<!bsCond<<"=1) & n="<<nOfOUT<<">0 & F="<<corFlag
        <<" then the correction must be done for PDG="<<reTPDG<<G4endl;
#endif
  if((resTMa<resSMa || !resEMa&&!bsCond) && nOfOUT>0 && corFlag) // *** CORRECTION ***
  {
    G4QHadron*  theLast = theQHadrons[nOfOUT-1];
    G4int cNf=theLast->GetNFragments();
    G4int crPDG=theLast->GetPDGCode();
#ifdef cdebug
	   G4cout<<"G4QE::CGS: **Correction** lNF="<<cNf<<",lPDG="<<crPDG<<",Q4M="<<reTLV<<G4endl;
#endif
    G4LorentzVector hadr4M = theLast->Get4Momentum();
    G4double  hadrMa=hadr4M.m();
    G4LorentzVector tmpTLV=reTLV+hadr4M;        // Tot (ResidNucl+LastHadron) 4-Mom
#ifdef cdebug
	   G4cout<<"G4QE::CGS:YES, s4M/M="<<tmpTLV<<tmpTLV.m()<<">rSM+hM="<<resSMa+hadrMa<<G4endl;
#endif
    G4double tmpTM=tmpTLV.m();
    if(tmpTM>resSMa+hadrMa &&!cNf && crPDG!=22) // Q(E)L contain QM(+EM)+lM ***Last CORR***
    {
      if(resEMa)                                // => "NonVacuumEnvironment exists" case
      {
        G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,resQMa); // GS Mass of Quasmon
        G4LorentzVector enva4M = G4LorentzVector(0.,0.,0.,resEMa); // GS Mass of ResidEnvir
        if(tmpTM>=resQMa+resEMa+hadrMa && G4QHadron(tmpTLV).DecayIn3(hadr4M,quas4M,enva4M))
        {
          //@@CHECK CoulBar (only for ResQuasmon in respect to ResEnv) and may be evaporate
#ifdef pdebug
          G4cout<<"G4QE::CGS: Modify the Last 4-momentum to "<<hadr4M<<G4endl;
#endif
          theLast->Set4Momentum(hadr4M);
          G4QHadron* quasH = new G4QHadron(valQ, quas4M);
          G4QContent theEQC=theEnvironment.GetQCZNS();
          G4QHadron* envaH = new G4QHadron(theEQC,enva4M);
#ifdef pdebug
          G4cout<<"G4QE::CGS: Fill Quasm "<<valQ<<quas4M<<" in any form"<<G4endl;
#endif
          // @@ Substitute by EvaporateResidual (if it is not used in the evaporateResid)
          if(resQPDG==92000000||resQPDG==90002000||resQPDG==90000002)DecayDibaryon(quasH);
          else if(resQPDG==93000000||resQPDG==90003000||resQPDG==90000003)
                                                                   DecayMultyBaryon(quasH);
          else if(resQPDG==90004002) DecayAlphaDiN(quasH);//Decay alph+2p(alph+2n isStable)
          else if(resQPDG==90002003||resQPDG==90003002) DecayAlphaBar(quasH); //DelEqu
          else if(resQPDG==90004004) DecayAlphaAlpha(quasH); //DelEqu
          else theQHadrons.push_back(quasH);    // Fill ResidQ as QHadron (delete equiv.)
#ifdef pdebug
          G4cout<<"G4QE::CGS: Fill envir "<<theEQC<<enva4M<<" in any form"<<G4endl;
#endif
          // @@ Substitute by EvaporateResidual (if it is not used in the evaporateResid)
          envaH->Set4Momentum(enva4M);
          if(envPDG==92000000||envPDG==90002000||envPDG==90000002) DecayDibaryon(envaH);
          else if(envPDG==93000000||envPDG==90003000||envPDG==90000003)
                                                                   DecayMultyBaryon(envaH);
          else if(envPDG==90002003||envPDG==90003002) DecayAlphaBar(envaH); //DelEqu
          else if(envPDG==90004002) DecayAlphaDiN(envaH);//Decay alph+2p(alph+2n IsStable)
          else if(envPDG==90004004) DecayAlphaAlpha(envaH); //DelEqu
          else theQHadrons.push_back(envaH);    // Fill 2nd Hadron (delete equivalent)
          // Kill environment as it is already included in the decay
          theEnvironment=G4QNucleus(G4QContent(0,0,0,0,0,0), G4LorentzVector(0.,0.,0.,0.));
		      }
        else
        {
#ifdef cdebug
          G4cout<<"***G4QEnv::CheckGroundState: Decay in Frag+ResQ+ResE error"<<G4endl;
#endif
          return false;
        }
	     }
      else                                      // => "Environ is vacuum" case (DecayIn2)
      {
        G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,resQMa); // GS Mass of Quasmon
        G4QHadron* quasH = new G4QHadron(valQ, quas4M);
        if(tmpTM>=resQMa+hadrMa && G4QHadron(tmpTLV).DecayIn2(hadr4M,quas4M))//DeIn2(noEnv)
        {
          //@@CHECK CoulBar (only for ResQuasmon in respect to ResEnv) & may be evaporate
          theLast->Set4Momentum(hadr4M);
#ifdef pdebug
          G4cout<<"G4QE::CGS: modify theLast 4M="<<hadr4M<<hadr4M.m()<<G4endl;
#endif
          quasH->Set4Momentum(quas4M);
#ifdef pdebug
          G4cout<<"G4QE::CGS: fill newQH "<<valQ<<quas4M<<quas4M.m()<<" inAnyForm"<<G4endl;
#endif
          if(resQPDG==92000000||resQPDG==90002000||resQPDG==90000002)DecayDibaryon(quasH);
          else if(resQPDG==93000000||resQPDG==90003000||resQPDG==90000003)
                                                                   DecayMultyBaryon(quasH);
          else if(resQPDG==90002003||resQPDG==90003002) DecayAlphaBar(quasH); //DelEqu
          else if(resQPDG==90004002) DecayAlphaDiN(quasH);//Decay alph+2p(alph+2n IsStable)
          else if(resQPDG==90004004) DecayAlphaAlpha(quasH); // DelEqu
          else theQHadrons.push_back(quasH);    // Fill ResidQuasmHadron (del. equiv.)
		      }
        else
        {
          delete quasH;                         // Delete "Quasmon Hadron"
#ifdef cdebug
          G4cerr<<"***G4QEnv::CheckGS: Decay in Fragm+ResQ did not succeeded"<<G4endl;
#endif
          return false;
        }
	     }
    }
    else                                        // *** Try Last+Previous CORRECTION ***
    {
#ifdef cdebug
	     G4cout<<"G4QEnv::CheckGS: the Last did not help, nH="<<nOfOUT<<G4endl;
#endif
      if(nOfOUT>1)                              // Cor with Last&Previous can be tryed
	     {
        G4QHadron*  thePrev = theQHadrons[nOfOUT-2]; // Get pointer to Prev before Last
        if(thePrev->GetNFragments()||thePrev->GetNFragments()) return false;// Dec H/g
        G4LorentzVector prev4M = thePrev->Get4Momentum();
        G4double  prevMa=prev4M.m();            // Mass of previous hadron
        tmpTLV+=prev4M;                         // Increase Total 4-Mom of TotalResidNucl
        G4int      totPDG=reTQC.GetSPDGCode();  // PDG Code of Total Residual Nucleus 
        if(totPDG==10&&reTQC.GetBaryonNumber()>0) totPDG=reTQC.GetZNSPDGCode();
        G4double   tQMa=G4QPDGCode(totPDG).GetMass(); // GS Mass of the Residual Nucleus
#ifdef cdebug
	       G4cout<<"G4QE::CGS:T3="<<tmpTLV<<tmpTLV.m()<<">t+h+p="<<tQMa+hadrMa+prevMa<<G4endl;
#endif
        if(tmpTLV.m()>tQMa+hadrMa+prevMa)
        {
          G4LorentzVector nuc4M = G4LorentzVector(0.,0.,0.,tQMa);// 4-Mom of ResidNucAtRest
          G4QHadron* nucH = new G4QHadron(reTQC, nuc4M);
          if(!G4QHadron(tmpTLV).DecayIn3(hadr4M,prev4M,nuc4M))
          {
            delete nucH;                        // Delete "Residual Nucleus Hadron"
            G4cerr<<"---Warning---G4QE::CGS:DecayIn ResNuc+LastH+PrevH Error"<<G4endl;
            return false;
          }
          else
          {
            theLast->Set4Momentum(hadr4M);
            thePrev->Set4Momentum(prev4M);
            nucH->Set4Momentum(nuc4M);
#ifdef cdebug
	           G4cout<<"G4QE::CGS:*SUCCESS**>CHECK, D4M="<<tmpTLV-hadr4M-prev4M-nuc4M<<G4endl;
#endif
#ifdef pdebug
            G4cout<<"G4QE::CGS: Fill nucleus "<<reTQC<<nuc4M<<" in any form"<<G4endl;
#endif
            if(totPDG==92000000||totPDG==90002000||totPDG==90000002) DecayDibaryon(nucH);
            else if(totPDG==93000000||totPDG==90003000||totPDG==90000003)
                                                                  DecayMultyBaryon(nucH);
            else if(totPDG==90004002) DecayAlphaDiN(nucH);// Dec alph+2p (alph+2n isStable)
            else if(totPDG==90002003||totPDG==90003002) DecayAlphaBar(nucH); //DelEqu
            else if(totPDG==90004004) DecayAlphaAlpha(nucH); //DelEqu
            else theQHadrons.push_back(nucH);             // Fill ResidNuclHadron (del.eq.)
	         }
		      }
        else                                  // ===> Try to use any hadron from the output
        {
#ifdef cdebug
		        G4cout<<"G4QE::CGS:Prev&Last didn't help,nH="<<nOfOUT<<">2, MQ="<<resQMa<<G4endl;
#endif
										G4int nphot=-1;                     // #of photons
          G4int npip=-1;                      // #of Pi+
          G4int npiz=-1;                      // #of Pi0
          G4int npim=-1;                      // #of Pi-
          G4double emaz=0.;                   // The maximum energy for pi0 (to sel high E)
          for(G4int id=nOfOUT-1; id>=0; id--) // Search for photons and pi+, and pi-
  			     {
            G4QHadron* curHadr = theQHadrons[id];
            G4int hPDG=curHadr->GetPDGCode();
            if(hPDG == 22) nphot=id;              // Get the earliest gamma
            else if(hPDG==211 && npip<1) npip=id; // Get the latest Pi+
            else if(hPDG==111)
            {
				          G4double piE=curHadr->Get4Momentum().e();
#ifdef chdebug
              G4cout<<"G4QE::CheckGroundState:"<<id<<",Epi0="<<piE<<">"<<emaz<<G4endl;
#endif
              if(piE>emaz)
              {
                npiz=id;
                emaz=piE;
              }
            }
            else if(hPDG==-211 && npim<1) npim=id; // Get the latest Pi-
          }
          if(nphot>=0)                 // Photon is found, try to use it to resolve PANIC
			       {
            G4QHadron* curHadr = theQHadrons[nphot];      // Pointer to the photon
            G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4-Mom of the Photon
            G4LorentzVector tt4M=ch4M+reTLV;// (resQMa=GSMass of the ResidQuasmon(+Env.))
            G4double ttM=tt4M.m();          // Mass of the Phot+ResidQm compaund system
            if(resQMa<ttM)                  // PANIC can be resolved with this Photon
			         {
              G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,resSMa);//GSMass of Quasm
              G4QHadron* rqH = new G4QHadron(reTQC,quas4M); // Prototype of outputResidQ
              if(!G4QHadron(tt4M).DecayIn2(ch4M,quas4M))
              {
                delete rqH;                 // Delete tmp "Residual Quasmon Hadron"
#ifdef cdebug
                G4cerr<<"***G4QEnv::CheckGS: Decay in Photon+ResQ tM="<<ttM<<G4endl;
#endif
              }
              else
              {
                curHadr->Set4Momentum(ch4M);// Change 4M of Photon (reduced by decay)
                rqH->Set4Momentum(quas4M);  // Fill 4M of the GS Residual Quasmon
                G4double hB=rqH->GetBaryonNumber();
                G4double hS=rqH->GetStrangeness();
                G4double hC=rqH->GetCharge();
#ifdef cdebug
                G4cout<<"G4QE::CGS:nPhot="<<nphot<<",ph4M="<<ch4M<<"+r4M="<<quas4M<<G4endl;
#endif
#ifdef pdebug
                G4cout<<"G4QE::CGS: Fill Resid "<<reTQC<<quas4M<<" in any form"<<G4endl;
#endif
                if(totPDG==92000000||totPDG==90002000||totPDG==90000002)DecayDibaryon(rqH);
                else if(hB>2 && (hB==hS || hB==hC || !hC&&!hS)) DecayMultyBaryon(rqH);
                else if(totPDG==90004002) DecayAlphaDiN(rqH);//Dec al+2p (al+2n isStable)
                else if(totPDG==90002003||totPDG==90003002) DecayAlphaBar(rqH); //DelEqu
                else if(totPDG==90004004) DecayAlphaAlpha(rqH); //DelEqu
                else if(hB>0 && (hC<0 || hC>hB)) DecayIsonucleus(rqH); //DelEqu
                else theQHadrons.push_back(rqH); // Fill ResidQuasmHadron (del eq)
                if(envPDG!=90000000) theEnvironment=G4QNucleus(G4QContent(0,0,0,0,0,0),
                                                          G4LorentzVector(0.,0.,0.,0.));
                return true;
		            }
            } // End of the KINEMATIC CHECK FOR THE PHOTON if
          } // End of nphot IF
#ifdef chdebug
          if(resQPDG==89998004)G4cout<<"G4QE::CGS:S="<<resS<<",B="<<resB<<",C="<<resC
                            <<",+#"<<npip<<",-#"<<npim<<",0#"<<npiz<<",E="<<envPDG<<G4endl;
#endif
          //if(npip>=0&&resQPDG==89998004 || npim>=0&&resQPDG==90003998)// D+D+pi->N+N+pi
          if(envPDG==90000000&&!resS&&resB>1&&(npip>=0&&resC==-2||npim>=0&&resC-resB==2))
			       {
            G4int npi=npip;               // (Delta-)+(Delta-)+k*n+(pi+)->(k+2)*n+(pi-)
            G4int piPD=-211;
            G4int nuPD=2112;
            G4double nuM=mNeut;
            if(resC!=-2)                  // (Delta++)+(Delta++)+k*p+(pi-)->(k+2)*p+(pi-)
            {
              npi=npim;
              piPD=211;
              nuPD=2212;
              nuM=mProt;
            }
            G4QPDGCode piQPDG(piPD);
            G4int rB=resB-1;
            G4double suB=rB*nuM;
            G4double suM=suB+nuM+mPi;
            G4QHadron* curHadr = theQHadrons[npi]; // Pointer to the pion of oposit sign
            G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4-Mom of the Pion
            G4LorentzVector tt4M=ch4M+reTLV;// (resQMa=GSMass of the ResidQuasmon(+Env.))
            G4double ttM=tt4M.m();          // Mass of the Pion+ResidQm compaund system
#ifdef cdebug
            if(resQPDG==89998005)
              G4cout<<"G4QE::CGS:Sm="<<suM<<"<Tot="<<ttM<<tt4M
                    <<",pi="<<ch4M<<",Q="<<reTLV.m()<<reTLV<<G4endl;
#endif
            if(suM<ttM)                    // PANIC can be resolved with this Pion
			         {
              G4LorentzVector fn4M = G4LorentzVector(0.,0.,0.,suB);//First nucleon(s)
              G4LorentzVector sn4M = G4LorentzVector(0.,0.,0.,nuM);//Second nucleon
              G4LorentzVector pi4M = G4LorentzVector(0.,0.,0.,mPi);//Pion
              if(!G4QHadron(tt4M).DecayIn3(fn4M,sn4M,pi4M))
              {
#ifdef cdebug
                if(resQPDG==89998005)
                  G4cerr<<"***G4QEnv::CheckGS:DecayIn3 2N+Pi,tM="<<ttM<<","<<suM<<G4endl;
#endif
              }
              else
              {
                if(rB>1) fn4M/=rB;
                for(G4int ib=0; ib<rB; ib++)
                {
                  G4QHadron* fnH = new G4QHadron(nuPD,fn4M);// First Nucleon(s)
#ifdef pdebug
                  G4cout<<"G4QE::CGS: fill Nucleon #"<<ib<<", "<<nuPD<<fn4M<<G4endl;
#endif
                  theQHadrons.push_back(fnH); // Fill First Nucleon(s) (del. equivalent)
                }
                G4QHadron* snH = new G4QHadron(nuPD,sn4M);// Second Nucleon
#ifdef pdebug
                G4cout<<"G4QE::CGS: fill the Last Nucleon, "<<nuPD<<sn4M<<G4endl;
#endif
                theQHadrons.push_back(snH); // Fill Second Nucleon (delete equivalent)
                curHadr->Set4Momentum(pi4M);// Change 4M of the Pion (reduced by decay)
                curHadr->SetQPDG(piQPDG);   // Change Charge of thePion
#ifdef cdebug
                if(resQPDG==89998005)
                  G4cout<<"G4QE::CGS:1="<<nuPD<<fn4M<<rB<<",2="<<sn4M<<",p="<<pi4M<<G4endl;
#endif
                return true;
		            }
            } // End of the KINEMATIC CHECK FOR THE PI+/PI- if
          } // End of npip/pin Isonucleus IF
          if(envPDG==90000000&&!resS&&resB>1&&npiz>=0&&(resC<-1||resC-resB>1))
			       {
#ifdef chdebug
            G4cerr<<"***G4QE::CGS:Pi0, rPDG="<<resQPDG<<",rC="<<resC<<",rB="<<resB<<G4endl;
#endif
            G4int npi=-resC;                // k*(Delta-)+m*n+pi0->(k+m)*k+(pi-)
            G4int piPD=-211;
            G4int nuPD=2112;
            G4double nuM=mNeut;
            if(resC!=-2)                    // k*(Delta++)+m*p+pi0->(k+m)*p+k*(pi+)
            {
              npi=resC-resB;
              piPD=211;
              nuPD=2212;
              nuM=mProt;
            }
            G4QPDGCode piQPDG(piPD);
            G4double suB=resB*nuM;          // Total mass of nucleons
            G4double suM=npi*mPi;           // Total mass of pions
            G4double sum=suB+suM;           // Total mass of secondaries
            G4QHadron* curHadr = theQHadrons[npiz]; // Pointer to pi0
            G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4-Mom of the Pion
            G4LorentzVector tt4M=ch4M+reTLV;// (resQMa=GSMass of the ResidQuasmon(+Env.))
            G4double ttM=tt4M.m();          // Mass of the Pion+ResidQm compaund system
#ifdef chdebug
            G4cout<<"G4QE::CGS:sum="<<sum<<"<"<<ttM<<tt4M<<",pi0="<<ch4M<<",Q="<<reTLV.m()
                  <<reTLV<<G4endl;
#endif
            if(sum<ttM)                     // PANIC can be resolved with this Pi0
			         {
              G4LorentzVector fn4M = G4LorentzVector(0.,0.,0.,suB); // Nucleon(s)
              G4LorentzVector pi4M = G4LorentzVector(0.,0.,0.,suM); // Pion(s)
              if(G4QHadron(tt4M).DecayIn2(fn4M,pi4M))
              {
                if(npi>1) pi4M/=npi;
                curHadr->Set4Momentum(pi4M);// Change 4M of the Pion (reduced by decay)
                curHadr->SetQPDG(piQPDG);   // Change Charge of thePion
                if(npi>1) for(G4int ip=1; ip<npi; ip++)
                {
                  G4QHadron* piH = new G4QHadron(piPD,pi4M);// Pion(s)
#ifdef pdebug
                  G4cout<<"G4QE::CGS: fill Pion #"<<ip<<", "<<piPD<<pi4M<<G4endl;
#endif
                  theQHadrons.push_back(piH); // Fill Pion(s) (delete equivalent)
                }
                if(resB>1) fn4M/=resB;
                for(G4int ib=0; ib<resB; ib++)
                {
                  G4QHadron* fnH = new G4QHadron(nuPD,fn4M);// Nucleon(s)
#ifdef pdebug
                  G4cout<<"G4QE::CGS: fill IsoNucleon #"<<ib<<", "<<nuPD<<fn4M<<G4endl;
#endif
                  theQHadrons.push_back(fnH); // Fill Nucleon(s) (delete equivalent)
                }
#ifdef chdebug
					           G4cout<<"G4QE::CGS: nucl="<<nuPD<<fn4M<<resB<<", pion="<<pi4M<<npi<<G4endl;
#endif

                return true;
		            }
#ifdef chdebug
              else G4cerr<<"***G4QEnv::CheckGS:DecayIn3:*Pi0* tM="<<ttM<<","<<sum<<G4endl;
#endif
            } // End of the KINEMATIC CHECK FOR THE PI0 if
										} // End of npiz Isonucleus IF
#ifdef cdebug
		        G4cout<<"G4QE::CGS: Photons can't help nP="<<nphot<<". TryChangeCharge."<<G4endl;
#endif
          // > Photons did not help, try to find an appropriate partner to join and decay
          G4int    reTBN=reTQC.GetBaryonNumber(); // Baryon number of theHadronicState
          G4int    reTCH=reTQC.GetCharge();       // Charge of theHadronicState
          G4bool isoN = reTCH-reTBN>0 || reTCH<0; // UnavoidableIsonucleus (Delta cond.)
          G4bool norN = reTCH<=reTBN || reTCH>=0; // "Regular nucleus" condition
          G4double nnM=resSMa;               // Fake prototype of the NormalNucleusMass
          G4QContent ipiQC=pipQC;            // Prototype of QCont for the Residual Pion+
          G4QContent nnQC=reTQC+pimQC;       // Prototype of theIsoReduceNucleus(Delta++)
          G4int nnPDG=nnQC.GetSPDGCode();    // Prot. PDGCode of the ResidNormalNucleus
          if((!nnPDG||nnPDG==10)&&nnQC.GetBaryonNumber()>0) nnPDG=nnQC.GetZNSPDGCode();
#ifdef cdebug
		        G4cout<<"G4QE::CGS: nnPDR="<<nnPDG<<". TryChangeCharge nOUT="<<nOfOUT<<",Iso="
                <<isoN<<",Nor="<<norN<<",C="<<reTCH<<",B="<<reTBN<<G4endl;
#endif
          if(isoN)                           // Calculations for the Isonuclear Residual
			       {
            if(reTCH<0)                      // "at least one Delta-" isostate (chngPort)
            {
              ipiQC=pimQC;                   // Change QCont for the Residual Pion-
              nnQC=reTQC+pipQC;              // Change QCont for theNormalNucleus(Delta-)
              nnPDG=nnQC.GetSPDGCode();      // Change PDGCode of theResidNormalNucleus
              if(nnPDG==10&&nnQC.GetBaryonNumber()>0) nnPDG=nnQC.GetZNSPDGCode();
			         }
            G4QPDGCode nnQPDG(nnPDG);        // Now can even have Q-code !
            if(nnPDG<80000000) nnM=nnQPDG.GetMass(); // Mass for the Fundamental Hadron
            else               nnM=nnQPDG.GetNuclMass(nnPDG); // Mass for the Nucleus
          }
          G4bool chx2g=true;
          G4bool force=false; // Force-flag to initiate gamma decays (ChEx=>"Pi0"->2gamma)
          while(chx2g)
          {
            if(force) chx2g=false;
            for(G4int hd=nOfOUT-1; hd>=0; hd--)// Try to use any hadron to resolve PANIC
  			       {
              G4QHadron* curHadr = theQHadrons[hd];
              G4int chNF=curHadr->GetNFragments();
              G4int chCH=curHadr->GetCharge();
              G4int chBN=curHadr->GetBaryonNumber();
              //G4int chS=curHadr->GetStrangeness();
              G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4Mom of the Current Hadron
#ifdef cdebug
			           G4cout<<"G4QE::CGS:#"<<hd<<",ch="<<chCH<<",b="<<chBN<<",4M="<<ch4M<<G4endl;
#endif
              if(!chNF)
              {
                G4LorentzVector tt4M=ch4M+reTLV;// resSMa=GSMass of the ResidQuasmon(+Env)
                G4double chM=ch4M.m();          // Mass of the CurrentHadron from theOUTPUT
                G4double ttM=tt4M.m();          // TotalMass of CurHadr+Residual compaund
                if(isoN)                        // "1 Delta Isonucleus" case
		              {
                  if(nnM+mPi+chM<ttM)           // PANIC can be resolved with thisCurHadron
			               {
#ifdef cdebug
                      G4cout<<"G4QE::CGS:CurH+ResQ+Pion t="<<tt4M<<ttM<<",cM="<<chM<<",rM="
                            <<nnM<<", d="<<ttM-chM-nnM-mPi<<G4endl;
#endif
                    ch4M = G4LorentzVector(0.,0.,0.,chM); // Mass of current Hadron
                    G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,nnM); // GSMass of RQ
                    G4LorentzVector ipi4M = G4LorentzVector(0.,0.,0.,mPi);// GSMass of Pion
                    if(G4QHadron(tt4M).DecayIn3(ch4M,ipi4M,quas4M))
                    {
                      curHadr->Set4Momentum(ch4M);// Change 4M of the Current Hadron
                      G4QHadron* rpH = new G4QHadron(ipiQC,ipi4M);// Prototype of ResidPion
#ifdef pdebug
                      G4cout<<"G4QE::CGS: fill Pion "<<ipiQC<<ipi4M<<G4endl;
#endif
                      theQHadrons.push_back(rpH); // Fill Resid Pion (delete equivalent)
                      G4QHadron* rqH = new G4QHadron(nnQC,quas4M);// Prototype of OutResidQ
#ifdef pdebug
                      G4cout<<"G4QE::CGS:Fill isoRes "<<nnQC<<quas4M<<" inAnyForm"<<G4endl;
#endif
                      G4double hB=rqH->GetBaryonNumber();
                      G4double hS=rqH->GetStrangeness();
                      G4double hC=rqH->GetCharge();
#ifdef cdebug
                      //if(resQPDG==89998004)
                        G4cout<<"G4QE::CGS:#"<<hd<<"is h="<<curHadr->GetPDGCode()<<ch4M
                              <<curHadr->Get4Momentum()<<" + rq="<<nnPDG<<quas4M<<" + pi="
                              <<ipiQC<<ipi4M<<G4endl;
#endif
                      if(nnPDG==92000000||nnPDG==90002000||nnPDG==90000002)
                                                                        DecayDibaryon(rqH);
                      else if(hB>2 && (hB==hS || hB==hC || !hC&&!hS))DecayMultyBaryon(rqH);
                      else if(nnPDG==90004002) DecayAlphaDiN(rqH);//Dec al+2p(al+2n isStab)
                      else if(nnPDG==90002003 || nnPDG==90003002) DecayAlphaBar(rqH);//DelE
                      else if(nnPDG==90004004) DecayAlphaAlpha(rqH); // Delete Equivalent
                      else if(hB>0 && (hC<0 || hC>hB)) DecayIsonucleus(rqH); //DelEqu
                      else theQHadrons.push_back(rqH); // Fill ResidQuasmHadron (del eq.)
                      if(envPDG!=90000000)theEnvironment=G4QNucleus(G4QContent(0,0,0,0,0,0)
                                                        ,G4LorentzVector(0.,0.,0.,0.));
                      return true;
		                  }
#ifdef cdebug
																				else G4cerr<<"***G4QE::CGS:DecIn3 CurH+ResQ+Pion dM="<<ttM-chM<<G4endl;
#endif
                  }
                  if(reTCH<0&&chCH>0 || reTCH>reTBN&&chCH<chBN) // Isoexchange can help
			               {
                    G4QContent chQC=curHadr->GetQC(); // QuarkCont of the CurrentHadron
                    if(reTCH<0)chQC+=pimQC;           // Add the negativPion QC to CurHadr
                    else       chQC+=pipQC;           // Add the positivePion QC to CurHadr
                    G4QPDGCode nnQPDG=G4QPDGCode(nnPDG);// New QPDG of the Residual
                    nnM=nnQPDG.GetMass();             // New Mass of the Residual
                    G4QPDGCode chQPDG=G4QPDGCode(chQC.GetSPDGCode());// New QPDG of CurHadr
                    chM=chQPDG.GetMass();             // New Mass of the CurHadron
                    if(force && nnPDG==111) nnM=0.;   // Decay of Pi0->2 gammas is possible
#ifdef cdebug
                    G4cout<<"G4QE::CGS:ChargeExchange,cx="<<chx2g<<",rC="<<reTCH<<",rB="
                          <<reTBN<<",rM="<<nnM<<",hC="<<chCH<<",hB="<<chBN<<",hM="<<chM
                          <<",rM+hB="<<nnM+chM<<" < "<<ttM<<G4endl;
#endif
                    if(nnM+chM<ttM)
                    {
                      G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,nnM);//GSMass of RQ
                      G4LorentzVector gam4M = G4LorentzVector(0.,0.,0.,0.);//4Mom of gamma1
                      ch4M = G4LorentzVector(0.,0.,0.,chM);//GSMass of ChrgExchanged Hadron
                      G4QHadron* rqH = new G4QHadron(nnQPDG,quas4M);//ChrgExResidualQuasmon
                      if(!nnM)                      // Decay ResidualVirtualQ: Pi0->2 gamma
                      {
                        if(!G4QHadron(tt4M).DecayIn3(ch4M,quas4M,gam4M))
                        {
                          delete rqH;               // Delete tmp "Residual Quasmon Hadron"
#ifdef cdebug
                          G4cerr<<"***G4QE::CGS:DecayIn3 CurH+2Gammas,d="<<ttM-chM<<G4endl;
#endif
                        }
                        else
                        {
                          if(chCH+reTCH-chQC.GetCharge()-nnQC.GetCharge())
						                      G4cerr<<"**G4QE::CGS:ChEx CH+2G i="<<reTCH<<"+h="<<chCH<<", f="
                                  <<nnQC.GetCharge()<<"+o="<<chQC.GetCharge()<<G4endl;
                          curHadr->Set4Momentum(ch4M);// Change 4M of the Current Hadron
                          curHadr->SetQPDG(chQPDG);   // Change QPDG of the Current Hadron
#ifdef cdebug
                          G4cout<<"G4QE::CGS:SubstituteH#"<<hd<<"->"<<chQPDG<<ch4M<<G4endl;
#endif
                          rqH->Set4Momentum(quas4M);  // Fill 4M of the GS Residual Quasmon
                          rqH->SetQPDG(gamQPDG);      // Change QPDG of the ResidualQuasmon
                          theQHadrons.push_back(rqH); // Fill Gamma 1 as QHadron (del. eq.)
#ifdef pdebug
                          G4cout<<"G4QE::CGS:Fill (SubRQ) Gamma 1,(22)4M="<<quas4M<<G4endl;
#endif
                          G4QHadron* gamH = new G4QHadron(gamQPDG, gam4M);
                          theQHadrons.push_back(gamH);// Fill Gamma 2 as QHadron (del. eq.)
#ifdef pdebug
                          G4cout<<"G4QE::CGS:Fill newMadeGamma 2, (22) 4M="<<gam4M<<G4endl;
#endif
                          if(envPDG!=90000000) theEnvironment=
                          G4QNucleus(G4QContent(0,0,0,0,0,0),G4LorentzVector(0.,0.,0.,0.));
                          return true;
		                      }
                      } // End of "the Normal decay without 2 gammas"
                      else                        // Normal decay (without "Pi0"->2 gammas)
                      {
                        if(!G4QHadron(tt4M).DecayIn2(ch4M,quas4M))
                        {
                          delete rqH;               // Delete tmp "Residual Quasmon Hadron"
#ifdef cdebug
                          G4cerr<<"**G4QE::CGS:DecayIn2 CurH+ResQ d="<<ttM-chM-nnM<<G4endl;
#endif
                        }
                        else
                        {
                          if(chCH+reTCH-chQC.GetCharge()-nnQC.GetCharge())
						                      G4cerr<<"**G4QE::CGS:ChEx CH+RQ i="<<reTCH<<"+h="<<chCH<<", f="
                                  <<nnQC.GetCharge()<<"+o="<<chQC.GetCharge()<<G4endl;
                          curHadr->Set4Momentum(ch4M);// Change 4M of the Current Hadron
                          curHadr->SetQPDG(chQPDG);   // Change QPDG of the Current Hadron
                          rqH->Set4Momentum(quas4M);  // Fill 4M of the GS Residual Quasmon
#ifdef cdebug
                          G4cout<<"G4QE::CGS:#"<<hd<<",h="<<ch4M<<"+rq="<<quas4M<<G4endl;
#endif
                          G4double hB=rqH->GetBaryonNumber();
                          G4double hS=rqH->GetStrangeness();
                          G4double hC=rqH->GetCharge();
#ifdef pdebug
                          G4cout<<"G4QE::CGS:FilFr "<<nnQPDG<<quas4M<<" inAnyForm"<<G4endl;
#endif
                          if(nnPDG==92000000||nnPDG==90002000||nnPDG==90000002)
                                                                        DecayDibaryon(rqH);
                          else if(hB>2 && (hB==hS||hB==hC||!hC&&!hS))DecayMultyBaryon(rqH);
                          else if(nnPDG==90004002) DecayAlphaDiN(rqH);//DecA+2p(A+2nIsStab)
                          else if(nnPDG==90002003 || nnPDG==90003002)DecayAlphaBar(rqH);
                          else if(nnPDG==90004004) DecayAlphaAlpha(rqH);//Delete Equivalent
                          else if(hB>0 && (hC<0 || hC>hB)) DecayIsonucleus(rqH); //DelEqu
                          else theQHadrons.push_back(rqH);//Fill ResidQuasmHadron (del eq.)
                          if(envPDG!=90000000) theEnvironment=
                          G4QNucleus(G4QContent(0,0,0,0,0,0),G4LorentzVector(0.,0.,0.,0.));
                          return true;
		                      }
                      } // End of "the Normal decay without 2 gammas"
                    }
                    else
																		  {
#ifdef cdebug
                      G4cout<<"**G4QE::CGS:rM+hB="<<nnM+chM<<">"<<ttM<<",B="<<chBN<<G4endl;
#endif
                      if(chBN>1)
                      {
                        G4QContent tcQC=chQC+nnQC; //QuarkCont for theTotalCompound nucleus
                        G4QPDGCode tcQPDG(tcQC);         // QPDG for the Total Compound
                        G4double   tcM=tcQPDG.GetMass(); // GS Mass of the TotalCompound
                        G4QHadron* tcH = new G4QHadron(tcQPDG,tt4M);// Hadron=TotalCompound
                        G4int tcS=tcQC.GetStrangeness();            // Total Strangeness
                        G4int tcC=tcQC.GetCharge();                 // Total Charge
                        G4int tcBN=tcQC.GetBaryonNumber();          // Total Baryon Number
                        if(tcBN==2|| !tcS&&!tcC||tcS==tcBN||tcC==tcBN)//dec DiBar or MulBar
                        {
                          if(tcBN==2) DecayDibaryon(tcH); // Decay Dibaryon
                          else     DecayMultyBaryon(tcH); // Decay Multibaryon
                          G4QHadron* theLast = theQHadrons[theQHadrons.size()-1];
                          curHadr->Set4Momentum(theLast->Get4Momentum());//4-Mom of CurHadr
                          G4QPDGCode lQP=theLast->GetQPDG();
                          if(lQP.GetPDGCode()!=10) curHadr->SetQPDG(lQP);
                          else curHadr->SetQC(theLast->GetQC());
                          theQHadrons.pop_back(); // theLastQHadron is excluded from OUTPUT
                          delete theLast;//*!!When kill, delete theLastQHadr asAnInstance!*
                        }
                        else if(tcM<ttM)// @@ Can't evaporate here, can only radiate gamma
																						  {
                          G4LorentzVector tc4M=G4LorentzVector(0.,0.,0.,tcM);//4M of TotCom
                          G4LorentzVector gc4M=G4LorentzVector(0.,0.,0.,0.); //4M of gamma
                          if(!G4QHadron(tt4M).DecayIn2(tc4M,gc4M))
                          {
                            delete tcH;                 // Delete tmp TotalCompoundHadron
                            curHadr->Set4Momentum(tt4M);// Change 4M of the Current Hadron
                            curHadr->SetQPDG(tcQPDG);   // Change QPDG of theCurrentHadron
#ifdef cdebug
                            G4cerr<<"**G4QE::CGS:DecayIn2 TotComp+gam d="<<ttM-tcM<<G4endl;
#endif
                          }
                          else
                          {
                            curHadr->Set4Momentum(gc4M);//Change 4Mom of the Current Hadron
                            curHadr->SetQPDG(gamQPDG);//Change PDG of theCurHadron to gamma
                            tcH->Set4Momentum(tc4M); // Fill 4-Mom of the GS Total Compound
                            G4double hB=tcH->GetBaryonNumber();
                            G4double hS=tcH->GetStrangeness();
                            G4double hC=tcH->GetCharge();
#ifdef cdebug
                            G4cout<<"G4QE::CGS:#"<<hd<<",ch="<<ch4M<<"+t4M="<<tc4M<<G4endl;
#endif
#ifdef pdebug
                            G4cout<<"G4QE::CGS:FilTC "<<tcQPDG<<tc4M<<" inAnyForm"<<G4endl;
#endif
                            if(nnPDG==90004002) DecayAlphaDiN(tcH);//Dec al+2p(al+2n=Stab)
                            else if(nnPDG==90002003||nnPDG==90003002)DecayAlphaBar(tcH);
                            else if(nnPDG==90004004) DecayAlphaAlpha(tcH); // alpha+alpha
                            else if(!hS && hB>0 && (hC<0 || hC>hB)) DecayIsonucleus(tcH);
                            else theQHadrons.push_back(tcH); // Fill Total Compound (delEq)
		                        }
                        }
																						  else // @@ Fill the TotalCompound instead of the CurrentHadron @@
																						  {
#ifdef cdebug
                          G4cout<<"G4QE::CGS:**CEF,M="<<tcM<<">"<<ttM<<",B="<<chBN<<G4endl;
#endif
                          delete tcH;                  // Delete tmp "TotalCompound"
                          curHadr->Set4Momentum(tt4M); // Change 4Mom of the Current Hadron
                          curHadr->SetQPDG(tcQPDG);    // Change QPDG of the Current Hadron
                        }
                        return true;
                      } // Check that the residual is a nucleus, not just a nucleon
                    } // Check if pion can still be radiated or must be absorbed
                  } // Check that the iso-exchange could help
                } // End of the IF: KINEMATIC CHECK FOR THE CURRENT HADRON(Isonuclear Case)
                else if(norN)
		              {
                  if(resSMa+chM<ttM)          // PANIC can be resolved with this CurHadron
			               {
                    G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,resSMa);// GSMass ofQ
                    G4QHadron* rqH = new G4QHadron(reTQC,quas4M);// Prototype OutputResidQ
                    if(!G4QHadron(tt4M).DecayIn2(ch4M,quas4M))
                    {
                      delete rqH;                   // Delete tmp "Residual Quasmon Hadron"
#ifdef cdebug
                      G4cerr<<"***G4QE::CheckGS:Decay in CurH+ResQ dM="<<ttM-chM<<G4endl;
#endif
                    }
                    else
                    {
                      curHadr->Set4Momentum(ch4M);  // Change 4M of the Current Hadron
                      rqH->Set4Momentum(quas4M);    // Fill 4M of the GS Residual Quasmon
                      G4double hB=rqH->GetBaryonNumber();
                      G4double hS=rqH->GetStrangeness();
                      G4double hC=rqH->GetCharge();
#ifdef cdebug
                      G4cout<<"G4QEnv::CheckGS:#"<<hd<<",ch4M="<<curHadr->GetPDGCode()
                            <<ch4M<<" + ResQ4M="<<totPDG<<quas4M<<G4endl;
#endif
#ifdef pdebug
                      G4cout<<"G4QE::CGS:Fill GSRes "<<reTQC<<quas4M<<" inAnyForm"<<G4endl;
#endif
                      if(totPDG==92000000||totPDG==90002000||totPDG==90000002)
                                                                     DecayDibaryon(rqH);
                      else if(hB>2 && (hB==hS || hB==hC || !hC&&!hS))DecayMultyBaryon(rqH);
                      else if(totPDG==90004002) DecayAlphaDiN(rqH);//Dec a+2p(a+2n isStab.)
                      else if(totPDG==90002003||totPDG==90003002) DecayAlphaBar(rqH);//DelE
                      else if(totPDG==90004004) DecayAlphaAlpha(rqH); //DelEqu
                      else if(hB>0 && (hC<0 || hC>hB)) DecayIsonucleus(rqH); //DelEqu
                      else theQHadrons.push_back(rqH); // Fill ResidQuasmHadron (del. eq.)
                      if(envPDG!=90000000)theEnvironment=G4QNucleus(G4QContent(0,0,0,0,0,0)
                                                            ,G4LorentzVector(0.,0.,0.,0.));
                      return true;
		                  }
                  } // End of the KINEMATIC CHECK FOR THE CURRENT HADRON if (NormNuclCase)
                } // End of IsoNucleus/NormalNucleus choice
              } // End of the NumberOfFragments=0 (NOT DECAYED PARTICLE) if
			         } // End of the LOOP over hadrons and all attempts to resolve PANIC
#ifdef cdebug
            G4cout<<"G4QEnv::CheckGS:***Any hadron from the OUTPUT did not help"<<G4endl;
#endif
            force=true;
          } // End of while for chx2g
          if(resB>1)
          {
            G4int hind=-1;
            if(npiz>-1) hind=npiz;
            else
												{
              if(resC+resC>resB && npim>-1) hind=npim;
              else                          hind=npip;
            }
            if(hind>-1)
												{
              G4QHadron* curHadr = theQHadrons[hind];
              G4int chNF=curHadr->GetNFragments();
              if(!chNF)
              {
                G4LorentzVector ch4M=curHadr->Get4Momentum(); // 4Mom of the Current Hadron
                G4LorentzVector tt4M=ch4M+reTLV; // resSMa=GSMass of the ResidQuasmon(+Env)
                G4double        ttM=tt4M.m();    // TotalMass of CurHadr+Residual compaund
                G4QContent      ttQC=valQ+curHadr->GetQC(); // total Quark Content
                G4int           ttPDG=ttQC.GetZNSPDGCode();
                G4QPDGCode      ttQPDG(ttPDG);
                G4double        ttGSM=ttQPDG.GetMass();
                if(ttM>ttGSM && ttQC.GetStrangeness()>0)//Hypernucleus can be L->N degraded
                {
                  ttPDG-=ttQC.GetStrangeness()*999999; // S Neutrons instead of S Lambdas
                  ttQPDG=G4QPDGCode(ttPDG);            // Update QPDGcode defining fragment
                  ttGSM=ttQPDG.GetMass();              // Update the degraded mass value
#ifdef pdebug
                  G4cout<<"G4QEnv::CheckGS:Hypernucleus degraded to QPDG="<<ttQPDG<<G4endl;
#endif
                }
                if(ttM>ttGSM)   // Decay of Pi0 in 2 gammas with the residual nucleus
                {
#ifdef cdebug
                  G4cout<<"G4QEnv::CheckGS: Decay in ResQ="<<ttQPDG<<" & 2 gammas"<<G4endl;
#endif
                  G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,ttGSM); // GSMass of RQ
                  G4LorentzVector fgam4M = G4LorentzVector(0.,0.,0.,0.); // 4Mom of gamma 1
                  G4LorentzVector sgam4M = G4LorentzVector(0.,0.,0.,0.); // 4Mom of gamma 2
                  if(!G4QHadron(tt4M).DecayIn3(quas4M,fgam4M,sgam4M))G4cerr<<"*3*"<<G4endl;
                  else
																		{
                    curHadr->Set4Momentum(fgam4M); // Change 4M of the Pion to Gamma
                    curHadr->SetQPDG(gamQPDG);      // Change QPDG of the Pion to Gamma
                    G4QHadron* sgH = new G4QHadron(gamQPDG,sgam4M); // Gamma 2
                    theQHadrons.push_back(sgH); // Fill Gamma 2 as QHadron (del. eq.)
                    G4QHadron* rqH = new G4QHadron(ttQPDG,quas4M); // GSResidualQuasmon
                    theQHadrons.push_back(rqH); // Fill GSResidQuasmon as QHadron (del.eq.)
                    return true;
                  }
                }
                else G4cerr<<"**G4QEnv::CheckGS:M="<<ttM<<" < GSM="<<ttGSM<<ttQPDG<<G4endl;
              }
            }
          }
          return false;
        } // End of the KINEMATIC LIMIT FOR THE L&P CORRECTION if/else (try any hadron)
	     } // End of the POSSIBILITY OF PREV+LAST (OR MORE) CORRECTION if
      else return false;
    } // End of the CORRECTION WITH THE LAST if/else
  } // End of the CORRECTION IS POSSIBLE if
  else return false;                     // Correction can not be done
  return true;                           // If correction was done successfully
} // End of "CheckGroundState"

// Try to decay the Total Residual Nucleus in Environ+Quasmon
void G4QEnvironment::CopyAndDeleteHadronVector(G4QHadronVector* HV)
{ // ==============================================================
  G4int nHadrons = HV->size();
  if(nHadrons)
  {
    for(G4int ih=0; ih<nHadrons; ih++) // LOOP over output QHadrons
    {
      //G4QHadron* inH = HV->operator[](ih);         // Pointer to the i-th QHadron
      G4QHadron* inH = (*HV)[ih];                  // Pointer to the i-th QHadron
      G4int hNF  = inH->GetNFragments();           // A#of secondary fragments
      if(!hNF)                                     // Fill only final hadrons
      {
#ifdef pdebug
	       G4cout<<"G4QEnv::Copy&DeleteHV:#"<<ih<<", hPDG="<<inH->GetPDGCode()<<G4endl;
#endif
        G4QHadron* curH = new G4QHadron(inH);      // will be deleted with allQHadronVector
        theQHadrons.push_back(curH);               // Fill hadron-copy (delete equivalent)
      }
    }
    for_each(HV->begin(), HV->end(), DeleteQHadron()); // Delete instances
    HV->clear();                                       // Delete pointers
  }
#ifdef pdebug
  else G4cerr<<"***G4QEnv::Kopy&DelHV: No hadrons in the QHadronVector"<<G4endl;
#endif
  delete HV;                                       // Delete the inputQHadronVector
} // End of "CopyAndDeleteHadronVector"

// Try to decay the Total Residual Nucleus in Environ+Quasmon
G4bool G4QEnvironment::DecayInEnvQ(G4Quasmon* quasm)
{ //   =============================================
  G4QContent valQ=quasm->GetQC();                 // Quark content of the Quasmon
  G4int    resQPDG=valQ.GetSPDGCode();            // Reachable in a member function
  if(resQPDG==10&&valQ.GetBaryonNumber()>0) resQPDG=valQ.GetZNSPDGCode();
  G4double resQMa=G4QPDGCode(resQPDG).GetMass();  // GS Mass of the Residual Quasmon
  G4LorentzVector enva4M=G4LorentzVector(0.,0.,0.,0.);
  G4LorentzVector reTLV=quasm->Get4Momentum();    // Prototyoe of the 4-Mom of the ResidNuc
  G4double resSMa=resQMa;                         // Prototype of MinSplitMass of ResidNucl
  G4int envPDG=theEnvironment.GetPDG();           // PDG Code of the Environment
  if(envPDG!=90000000)                            // => "Environment is not vacuum" case
  {
    G4double resEMa=theEnvironment.GetMZNS();     // GSMass of the Residual Environment
    enva4M=G4LorentzVector(0.,0.,0.,resEMa);      // 4-Mom of the Residual Environment
    reTLV+=enva4M;                                // 4-Mom of Residual Nucleus
    resSMa+=resEMa;                               // Minimal Split Mass of Residual Nucleus
    G4double resTMa=reTLV.m();                    // CM Mass of theResidNucleus (Quasm+Env)
	//#ifdef pdebug
    G4cout<<"G4QEnv::DecayInEnvQ: totM="<<reTLV<<resTMa<<" > rQM+rEM="<<resSMa<<G4endl;
	//#endif
    if(resTMa>resSMa)
    {
      G4LorentzVector quas4M = G4LorentzVector(0.,0.,0.,resQMa); // GS Mass of Quasmon
      G4QHadron* quasH = new G4QHadron(valQ, quas4M);
      G4QHadron* envaH = new G4QHadron(envPDG, enva4M);
      if(!G4QHadron(reTLV).DecayIn2(enva4M,quas4M))
      {
        delete quasH;                             // Delete "Quasmon Hadron"
        delete envaH;                             // Delete "Environment Hadron"
        G4cerr<<"---Warning---G4Q::DecInEnvQ:Decay in Environment+ResidualQuasmon"<<G4endl;
        return false;
      }
      else
      {
        quasH->Set4Momentum(quas4M);
        if(resQPDG==92000000||resQPDG==90002000||resQPDG==90000002) DecayDibaryon(quasH);
        else if(resQPDG==93000000||resQPDG==90003000||resQPDG==90000003)
                                                                   DecayMultyBaryon(quasH);
        else if(resQPDG==90004002) DecayAlphaDiN(quasH);//Decay alph+2p (alph+2n is stable)
        else if(resQPDG==90002003||resQPDG==90003002) DecayAlphaBar(quasH); //DelEqu
        else if(resQPDG==90004004) DecayAlphaAlpha(quasH); //DelEqu
        else theQHadrons.push_back(quasH);        // Fill ResidQuasmHadron (del.equivalent)
        envaH->Set4Momentum(enva4M);
        if(envPDG==92000000||envPDG==90002000||envPDG==90000002) DecayDibaryon(envaH);
        else if(envPDG==93000000||envPDG==90003000||envPDG==90000003)
                                                                   DecayMultyBaryon(envaH);
        else if(envPDG==90004002) DecayAlphaDiN(envaH);// Decay al+2p (a+2n is stable)
        else if(envPDG==90002003||envPDG==90003002) DecayAlphaBar(envaH); //DelEqu
        else if(envPDG==90004004) DecayAlphaAlpha(envaH); //DelEqu
        else theQHadrons.push_back(envaH);        // Fill Environment Hadron (del.Equiv.)
	     }
    }
    else return false;
  }
  else return false;                              // => "Environment is vacuum" case
  return true;
} // End of "DecayInEnvQ"

//General function makes Random Unit 3D-Vector
G4ThreeVector RndmDir()
{//  -------- =========
  G4double x = G4UniformRand(), y = G4UniformRand(), z = G4UniformRand();
  G4double r2= x*x+y*y+z*z;
  while(r2>1.||r2<.000001)
  {
    x = G4UniformRand(); y = G4UniformRand(); z = G4UniformRand();
    r2=x*x+y*y+z*z;
  }
  G4double r=sqrt(r2), quad=G4UniformRand();
  if(quad>0.5)
  {
    if(quad>0.75)
    {
      if(quad>0.875)    return G4ThreeVector(-x/r,-y/r,-z/r);
      else              return G4ThreeVector(-x/r,-y/r, z/r);
	   }
    else
    {
      if(quad>0.625)    return G4ThreeVector(-x/r, y/r,-z/r);
      else              return G4ThreeVector(-x/r, y/r, z/r);
	   }
  }
  else
  {
    if(quad>0.25)
    {
      if(quad>0.375)    return G4ThreeVector( x/r,-y/r,-z/r);
      else              return G4ThreeVector( x/r,-y/r, z/r);
	   }
    else if(quad>0.125) return G4ThreeVector( x/r, y/r,-z/r);
  }
  return                       G4ThreeVector( x/r, y/r, z/r);
} // End of RndmDir