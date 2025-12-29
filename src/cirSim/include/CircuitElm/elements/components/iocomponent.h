/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "logicfamily.h"

class eElement;
class IoPin;

class IoComponent : public Component, public LogicFamily
{
    public:
        IoComponent( std::string type, std::string id );
        ~IoComponent();

        std::vector<comProperty*> inputProps();
        std::vector<comProperty*> outputProps();
        std::vector<comProperty*> outputType();
        std::vector<comProperty*> edgeProps();

        void initState();
        void runOutputs();
        void scheduleOutPuts( eElement* el );


        void setSupplyV( double v ) override;
        void setInpHighV( double volt ) override;
        void setInpLowV( double volt ) override;
        void setOutHighV( double volt ) override;
        void setOutLowV( double volt ) override;
        void setInputImp( double imp ) override;
        void setOutImp( double imp ) override;
        void setRiseTime( double time ) override;
        void setFallTime( double time ) override;
        void setInpPullups( double p ) override;
        void setOutPullups( double p ) override;

        double openImp() { return m_openImp; }
        void setOpenImp( double imp );

        bool invertOuts() { return m_invOutputs; }
        void setInvertOuts( bool invert );

        bool invertInps() { return m_invInputs; }
        virtual void setInvertInps( bool invert );

        int  numInps() { return m_inpPin.size(); }
        virtual void setNumInps( uint32_t pins, std::string label="I", int bit0=0, int id0=0 );

        int  numOuts() { return m_outPin.size(); }
        virtual void setNumOuts( uint32_t pins, std::string label="O", int bit0=0, int id0=0 );

        bool openCol() { return m_openCol; }
        void setOpenCol( bool op );

        void init( std::vector<std::string> pins );
        void initPin( IoPin* pin );

        virtual std::vector<Pin*> getPins() override;

        virtual void remove() override;

    protected:

        std::string getFamilyKeys(const std::unordered_map<std::string, logicFamily_t> &map); 

        IoPin* createPin( std::string data, std::string id );
        void setupPin( IoPin *pin, std::string data );
        void setNumPins( std::vector<IoPin*>* pinList, uint32_t pins, std::string label, int bit0, bool out, int id0 );
        void deletePins( std::vector<IoPin*>* pinList, uint32_t pins );

        uint32_t m_outValue;
        uint32_t m_nextOutVal;
        std::queue<uint32_t> m_outQueue;
        std::queue<uint64_t> m_timeQueue;
        //uint32_t m_nextOutDir;

        bool m_openCol;
        bool m_invOutputs;
        bool m_invInputs;

        double m_openImp;

        uint32_t m_width;
        uint32_t m_height;

        eElement* m_eElement;

        std::vector<IoPin*> m_inpPin;
        std::vector<IoPin*> m_outPin;
        std::vector<IoPin*> m_otherPin;
};

