/***************************************************************************
 *   Copyright (C) 2024 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once
#include "pch.h"

struct logicFamily_t{
    double supply;
    double inpLHp;
    double inpHLp;
    double inpImp;
    double inpPul;
    double outHip;
    double outLop;
    double outImp;
    double outPul;
};



class LogicFamily
{
    public:
        LogicFamily();
        ~LogicFamily();

        double supplyV() { return m_supplyV; }
        virtual void setSupplyV( double v ){;}

        double inpHighV() { return m_inpHighV; }
        virtual void setInpHighV( double v );

        double inpLowV() { return m_inpLowV; }
        virtual void setInpLowV( double v );

        double inpPullups() { return m_inpPullups; }
        virtual void setInpPullups( double p ){;}

        double outHighV() { return m_outHighV; }
        virtual void setOutHighV( double v );

        double outLowV() { return m_outLowV; }
        virtual void setOutLowV( double v );

        double inputImp() { return m_inpImp; }
        virtual void setInputImp( double i ){;}

        double outImp() { return m_outImp; }
        virtual void setOutImp( double i ){;}

        double outPullups() { return m_outPullups; }
        virtual void setOutPullups( double p ){;}

        double propSize() { return m_delayMult; }
        void setPropSize( double g ) { m_delayMult = g; }

        double propDelay() { return m_delayBase*1e-12; }
        void setPropDelay( double pd );

        double riseTime() { return m_timeLH*1e-12; }
        virtual void setRiseTime( double time );

        double fallTime() { return m_timeHL*1e-12; }
        virtual void setFallTime( double time );

        std::string family() { return m_family; }
        void setFamily( std::string f );
        void setFamilyData( logicFamily_t lf );

    virtual std::vector<std::string> getEnumUids(const std::string &) {
        std::vector<std::string> keys;
        for (const auto &entry : m_families) {
            keys.push_back(entry.first); // 提取 key
        }
        return keys;
    }
    virtual std::vector<std::string> getEnumNames(const std::string &) {
        return getEnumUids(""); // 复用 getEnumUids()
    }

 static void getFamilies();

    protected:
        void updateData();

        double m_delayBase; // Propagation delay Base (Family delay)
        double m_delayMult; // Multiplier for total Propagation delay
        double m_timeLH;    // Time for Output voltage to switch from 10% to 90% (1 gate)
        double m_timeHL;    // Time for Output voltage to switch from 90% to 10% (1 gate)

        bool m_blocked;
        bool m_enableSupply;

        double m_supplyV;
        double m_inpHighV;
        double m_inpHighVp;
        double m_inpLowV;
        double m_inpLowVp;
        double m_outHighV;
        double m_outHighVp;
        double m_outLowV;
        double m_outLowVp;

        double m_inpImp;
        double m_outImp;

        double m_inpPullups;
        double m_outPullups;

        std::string m_family;

 static std::unordered_map<std::string, logicFamily_t> m_families;
};
