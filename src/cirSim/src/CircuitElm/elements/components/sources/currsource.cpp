/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "currsource.h"
#include "itemlibrary.h"
//#include "propdialog.h"
#include "simulator.h"
//#include "custombutton.h"
#include "pin.h"

#include "doubleProp.h"

#define tr(str) simulideTr("CurrSource",str)

Component* CurrSource::construct( std::string type, std::string id )
{ return new CurrSource( type, id ); }

LibraryItem* CurrSource::libraryItem()
{
    return new LibraryItem(
        "Current Source",
        "Sources",
        "cursource.png",
        "Current Source",
        CurrSource::construct );
}

CurrSource::CurrSource( std::string type, std::string id )
          : VarSource( type, id )
{
    m_pin.resize(1);
    m_pin[0] = m_outPin = new Pin(id+"-outPin", 0, this );

    m_unit = "A";

    addPropGroup( { "Main", {
new doubleProp<CurrSource>("Value_Amp","Current Value","A", this, &CurrSource::getVal,   &CurrSource::setVal ),
new doubleProp<CurrSource>("MaxValue" ,"Max. Current" ,"A", this, &CurrSource::maxValue, &CurrSource::setMaxValue )
    }, 0} );

    //setShowProp("MaxValue");
    setPropStr( "MaxValue", "1 A" );
    //dialChanged( 0 );
}
CurrSource::~CurrSource(){}

void CurrSource::stamp()
{
    m_outPin->createCurrent();
}
void CurrSource::updateStep()
{
    if( m_changed ) 
    {
        //double current = m_button->isChecked() ? m_outValue : 0;   ---------------------------------需要针对按钮修改代码，脱离按钮控制。下面那个只是一个暂时替代
        double current = m_outValue;
        m_outPin->stampCurrent( current );
        //if( m_propDialog ) m_propDialog->updtValues();
        m_changed = false;
}   }

