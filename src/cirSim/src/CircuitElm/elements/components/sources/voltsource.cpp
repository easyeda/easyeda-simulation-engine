/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "voltsource.h"
#include "itemlibrary.h"
#include "simulator.h"
//#include "custombutton.h"
#include "iopin.h"

#include "doubleProp.h"

#define tr(str) simulideTr("VoltSource",str)

Component* VoltSource::construct( std::string type, std::string id )
{ return new VoltSource( type, id ); }

LibraryItem* VoltSource::libraryItem()
{
    return new LibraryItem(
        "Voltage Source",
        "Sources",
        "voltsource.png",
        "Voltage Source",
        VoltSource::construct );
}

VoltSource::VoltSource( std::string type, std::string id )
          : VarSource( type, id )
{
    m_pin.resize(1);

    m_pin[0] = m_outPin = new IoPin(id+"-outPin", 0, this, source );

    m_outPin->setOutHighV( 0 );
    m_outPin->setOutLowV( 0 );

    m_unit = "V";
    addPropGroup({ "Main", {
new doubleProp<VoltSource>("Value_Volt", "Current Value","V", this, &VoltSource::getVal,   &VoltSource::setVal),
new doubleProp<VoltSource>("MaxValue"  , "Max. Voltage" ,"V", this, &VoltSource::maxValue, &VoltSource::setMaxValue),
new doubleProp<VoltSource>("MinValue"  , "Min. Voltage" ,"V", this, &VoltSource::minValue, &VoltSource::setMinValue)
    }, 0 });

    //setShowProp("MaxValue");
    //setPropStr( "MaxValue", "5 V" );
    //dialChanged( 0 );
}
VoltSource::~VoltSource() {}

void VoltSource::updateStep()
{
    if(m_changed)
    {
        m_outPin->setOutHighV( m_outValue );
        if (ctl)
        {
            m_outPin->setOutState(true);    //有按钮  ---------------代表更新电压值
            ctl = false;
        }

        //if( m_propDialog ) m_propDialog->updtValues();
        m_changed = false;
    }   
}
