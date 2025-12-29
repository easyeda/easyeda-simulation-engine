/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"
#include "pin.h"

class ConnBase : public Component, public eElement
{
    public:
        ConnBase( std::string type, std::string id );
        ~ConnBase();

        int  size() { return m_size; }
        void setSize( int size );

        virtual void registerEnode( eNode*, int n=-1 ) override;

        void createPins( int c );
        void deletePins( int d );

        virtual void updatePins(){;}

    protected:
        virtual void updatePixmap(){;}

        int m_size;

        std::vector<Pin*> m_connPins;
        std::vector<Pin*> m_sockPins;

        Pin::pinType_t m_pinType;
};

