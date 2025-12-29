/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "mech_contact.h"
#include "inductor.h"
#include "e-resistor.h"

class LibraryItem;
class eNode;

//继电器
class Relay : public MechContact
{
    public:
        Relay( std::string type, std::string id );
        ~Relay();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        double iTrig() { return m_trigCurrent; }
        void setITrig( double c ) { if( c > 0.0 ) m_trigCurrent = c; }

        double iRel() { return m_relCurrent; }
        void setIRel( double current ) { m_relCurrent = current; }

        virtual void initialize() override;
        virtual void stamp() override;
        virtual void voltChanged() override;

    protected:
        Inductor* m_inductor;

        eNode* m_internalEnode;

        double m_trigCurrent;
        double m_relCurrent;

        bool m_relayOn;
};

