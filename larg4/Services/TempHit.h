/* 
 * File:   TempHit.h
 * Author: wenzel
 *
 * Created on October 22, 2018, 2:35 PM
 */

#ifndef TEMPHIT_H
#define TEMPHIT_H
#include <vector>
#include <iostream>

class TempHit {
private:
    int ID; ///< Geant4 copy ID 
    int trackID; ///< Geant4 supplied track ID
    int ParentID; //  Geant4 supplied Parent track ID
    bool isfirstinVolume; // Geant4  is first in Volume
    bool islastinVolume; // Geant4  is last in Volume
    float energyDeposited; ///< total energy deposited for this track ID and time
    float entryX; ///< Entry position X of particle
    float entryY; ///< Entry position Y of particle
    float entryZ; ///< Entry position Z of particle
    float entryT; ///< Entry time of particle
    float exitX; ///< Exit position X of particle
    float exitY; ///< Exit position Y of particle
    float exitZ; ///< Exit position Z of particle
    float exitT; ///< Exit time of particle
    float exitMomentumX; ///< Exit X-Momentum of particle
    float exitMomentumY; ///< Exit Y-Momentum of particle
    float exitMomentumZ; ///< Exit Z-Momentum of particle

public:
    // Default constructor

    TempHit() {
    }
    bool operator<(const TempHit& other) const;
    bool operator==(const TempHit& other) const;
    // Hide the following from Root    
#ifndef __GCCXML__

    TempHit(int iID,
            int itrackID,
            int iParentID, //  Geant4 supplied Parent track ID
            bool iisfirstinVolume, // Geant4  is first in Volume
            bool iislastinVolume, // Geant4  is last in Volume
            float ienergyDeposited,
            float ientryX,
            float ientryY,
            float ientryZ,
            float ientryT,
            float iexitX,
            float iexitY,
            float iexitZ,
            float iexitT,
            float iexitMomentumX,
            float iexitMomentumY,
            float iexitMomentumZ) :
    ID(iID),
    trackID(itrackID),
    ParentID(iParentID),
    isfirstinVolume(iisfirstinVolume),
    islastinVolume(iislastinVolume),
    energyDeposited(ienergyDeposited),
    entryX(ientryX),
    entryY(ientryY),
    entryZ(ientryZ),
    entryT(ientryT),
    exitX(iexitX),
    exitY(iexitY),
    exitZ(iexitZ),
    exitT(iexitT),
    exitMomentumX(iexitMomentumX),
    exitMomentumY(iexitMomentumY),
    exitMomentumZ(iexitMomentumZ) {
    }

    void SetExitMomentumZ(float iexitMomentumZ) {
        this->exitMomentumZ = iexitMomentumZ;
    }

    float GetExitMomentumZ() const {
        return exitMomentumZ;
    }

    void SetExitMomentumY(float iexitMomentumY) {
        this->exitMomentumY = iexitMomentumY;
    }

    float GetExitMomentumY() const {
        return exitMomentumY;
    }

    void SetExitMomentumX(float iexitMomentumX) {
        this->exitMomentumX = iexitMomentumX;
    }

    float GetExitMomentumX() const {
        return exitMomentumX;
    }

    void SetExitT(float iexitT) {
        this->exitT = iexitT;
    }

    float GetExitT() const {
        return exitT;
    }

    void SetExitZ(float iexitZ) {
        this->exitZ = iexitZ;
    }

    float GetExitZ() const {
        return exitZ;
    }

    void SetExitY(float iexitY) {
        this->exitY = iexitY;
    }

    float GetExitY() const {
        return exitY;
    }

    void SetExitX(float iexitX) {
        this->exitX = iexitX;
    }

    float GetExitX() const {
        return exitX;
    }

    void SetEntryT(float ientryT) {
        this->entryT = ientryT;
    }

    float GetEntryT() const {
        return entryT;
    }

    void SetEntryZ(float ientryZ) {
        this->entryZ = ientryZ;
    }

    float GetEntryZ() const {
        return entryZ;
    }

    void SetEntryY(float ientryY) {
        this->entryY = ientryY;
    }

    float GetEntryY() const {
        return entryY;
    }

    void SetEntryX(float ientryX) {
        this->entryX = ientryX;
    }

    float GetEntryX() const {
        return entryX;
    }

    void SetEnergyDeposited(float ienergyDeposited) {
        this->energyDeposited = ienergyDeposited;
    }

    float GetEnergyDeposited() const {
        return energyDeposited;
    }

    void SetTrackID(int itrackID) {
        this->trackID = itrackID;
    }

    int GetTrackID() const {
        return trackID;
    }

    void SetID(int iID) {
        this->ID = iID;
    }

    int GetID() const {
        return ID;
    }

    void Print() {
        std::cout << "TempHit: " << std::endl;
        std::cout << "copy ID: " << ID << " track ID: " << trackID << " Total energy (MeV) deposited: " << energyDeposited << std::endl;
        /*
                     int iParentID, //  Geant4 supplied Parent track ID
            bool iisfirstinVolume, // Geant4  is first in Volume
            bool iislastinVolume, // Geant4  is last in Vol/home/wenzel/trajectory/lArTest/include/TempHit.hh:15:18: error: ‘ int TempHit::ID’ is privateume
         */
        std::cout << "Begin position x,y,z (cm) time (ns) of particle: " << entryX << "  " << entryY << "  " << entryZ << "  " << entryT << std::endl;
        std::cout << "end position x,y,z (cm) time (ns) of particle: " << exitX << "  " << exitY << "  " << exitZ << "  " << exitT << std::endl;
        std::cout << "end momentum px,py,pz (MeV) of particle: " << exitMomentumX << "  " << exitMomentumY << "  " << exitMomentumZ << std::endl;
    }

    bool IsIslastinVolume() const {
        return islastinVolume;
    }

    bool IsIsfirstinVolume() const {
        return isfirstinVolume;
    }

    int GetParentID() const {
        return ParentID;
    }
    /*
        bool IsIslastinVolume() const;
        bool IsIsfirstinVolume() const;
         int GetParentID() const;
     */

#endif
};
/*class TempHitSortCriterion {
public:
    bool operator () (const TempHit& th1, const TempHit& th2) const {
        return th1.GetID()<th2.GetID()|| ((!(th2.GetID()<th1.GetID())) && (th1.GetTrackID()<th2.GetTrackID()));
    }
};
 */
typedef std::vector<TempHit> TempHitCollection;

inline bool TempHit::operator<(const TempHit& other) const {
    return ID < other.ID ||
            (!(other.ID < ID) && trackID < other.trackID) ||
            (!(other.ID < ID) && !(other.trackID < trackID) && exitT < other.exitT);
}

inline bool TempHit::operator==(const TempHit& other) const {
    return (other.trackID == trackID && other.ID == ID);
}

#endif /* TEMPHIT_H */

