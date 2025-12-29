/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class IoPin;
class LibraryItem;
//class CustomButton;

class FixedVolt : public Component, public eElement
{
    public:
        FixedVolt( std::string type, std::string id );
        ~FixedVolt();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        void setSmall( bool s );
        bool isSmall() { return m_small; }

        virtual void stamp() override;
        virtual void updateStep() override;

        bool out();
        virtual void setOut( bool out );

        double volt() { return m_voltage; }
        void setVolt( double v );

        virtual void onbuttonclicked();

    protected:
        void updateOutput();

        double m_voltage;

        IoPin* m_outPin;
        bool m_small;


        //CustomButton* m_button;
        //QGraphicsProxyWidget* m_proxy;
};
