/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef Thermistor_H
#define Thermistor_H

#include "thermistorbase.h"

class LibraryItem;

class Thermistor : public ThermistorBase
{
    public:
        Thermistor( std::string type, std::string id );
        ~Thermistor();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        int bVal() { return m_bVal; }
        void setBval( int bval );

        int r25() { return m_r25; }
        void setR25( int r25 );

        virtual void updateStep() override;

    private:
        int m_bVal;
        int m_r25;
};

#endif
