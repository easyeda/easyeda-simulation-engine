/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef RESISTORDIP_H
#define RESISTORDIP_H

#include "component.h"
#include "e-element.h"

class LibraryItem;
class eResistor;
class Pin;

class ResistorDip : public Component, public eElement
{
    public:
        ResistorDip( std::string type, std::string id );
        ~ResistorDip();

 static Component* construct( std::string type, std::string id );
 static LibraryItem *libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;

        int size() { return m_size; }
        void setSize( int size );

        double getRes() { return m_resist; }
        void setRes( double resist );

        bool pullUp() { return m_pullUp; }
        void setPullUp( bool p );

        double puVolt() { return m_puVolt; }
        void setPuVolt( double pv ) { m_puVolt = pv; }

        void createResistors( int c );
        void deleteResistors( int d );

        virtual void remove() override;

    private:
        double m_resist;
        int m_size;

        bool m_pullUp;
        double m_puVolt;

        std::vector<Pin*> m_pin;
        std::vector<eResistor*> m_resistor;

        static eNode m_puEnode;
};

#endif
