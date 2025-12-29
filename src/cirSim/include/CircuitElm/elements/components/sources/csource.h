/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "e-resistor.h"
#include "component.h"

class LibraryItem;

//受控源
class Csource : public Component, public eResistor
{
    public:
        Csource( std::string type, std::string id );
        ~Csource();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void updateStep() override;

        double gain() { return m_gain; }
        void setGain( double g );

        double volt() { return m_volt; }
        void setVolt( double volt );

        double outCurrent() { return m_curr; }
        void setOutCurrent( double c );

        bool currSource() { return m_currSource; }
        void setCurrSource( bool c );

        bool currControl() { return m_currControl; }
        void setCurrControl( bool c );

        bool controlPins() {return m_controlPins; }
        void setControlPins( bool set );

        void setVoltage( double v );

        virtual bool setLinkedTo( Linkable* li ) override;
        virtual void setLinkedValue( double v, int i=0 ) override;

    private:
        void udtProperties();

        double m_volt;
        double m_curr;
        double m_gain;

        double m_lastCurr;

        bool m_controlPins;     //控制信号是否通过引脚输入
        bool m_currSource;      //受控源是电流源还是电压源
        bool m_currControl;     //控制信号是电流还是电压
}; 

