/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-element.h"
#include "component.h"

class LibraryItem;
//class QPushButton;
//class QGraphicsProxyWidget;
class IoPin;

class Dht22 : public Component , public eElement
{
    public:
        Dht22( std::string type, std::string id );
        ~Dht22();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        void setModel( std::string model);
        std::string model();

        double tempInc() { return m_tempInc; }
        void setTempInc( double inc ) { m_tempInc = trim( inc ); }

        double humidInc() { return m_humiInc; }
        void setHumidInc( double inc ) { m_humiInc = trim( inc ); }

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

    //public slots:
    //    void onbuttonclicked();
    //    void upbuttonclicked();
    //    void downbuttonclicked();

    private:
        void calcData();
        double trim( double data );

        bool m_DHT22;
        bool m_lastIn;
        bool m_set;

        uint64_t m_lastTime;
        uint64_t m_start;
        uint64_t m_data;
        uint64_t m_bit;

        double m_temp;
        double m_humi;
        double m_tempInc;
        double m_humiInc;

        int m_outStep;
        int m_bitStep;

        //QFont m_font;

        //QPushButton* m_button;
        //QGraphicsProxyWidget* m_proxy;

        IoPin* m_inpin;
};

