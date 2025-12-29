/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "comp2pin.h"
#include "e-resistor.h"

class LibraryItem;

//电池
class Battery : public Comp2Pin, public eResistor
{
    public:
        Battery( std::string type, std::string id );
        ~Battery();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        double volt() { return m_volt; }
        void setVolt( double volt );

        virtual void setRes( double resist ) override;

        virtual void stamp() override;
        virtual void updateStep() override;

    private:
        double m_volt;

        bool m_connected;
};
