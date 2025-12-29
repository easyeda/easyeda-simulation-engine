/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "sevensegment_bcd.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "iopin.h"
//#include "ledbase.h"         //paint这个绘图的时候用到了这个头文件，目前不确定是否可以去除，暂定注释

#include "boolProp.h"

#define tr(str) simulideTr("SevenSegmentBCD",str)

Component* SevenSegmentBCD::construct( std::string type, std::string id )
{ return new SevenSegmentBCD( type, id ); }

LibraryItem* SevenSegmentBCD::libraryItem()
{
    return new LibraryItem(
        "7 Seg BCD",
        "Other Logic",
        "7segbcd.png",
        "7-Seg BCD", /// FIXME: only alphanumeric in Component type
        SevenSegmentBCD::construct );
}

SevenSegmentBCD::SevenSegmentBCD( std::string type, std::string id )
               : BcdBase( type, id )
{
    //m_graphical = true;

    m_width  = 4;
    m_height = 6;


    //IntPuts
    std::vector<std::string> pinList = {
        "ID041",
        "ID032",
        "ID024",
        "ID018",
        "IU04E",
        "IU01."
    };


    init( pinList );
    //for( uint32_t i=0; i<m_inpPin.size(); ++i )
    //{
    //    m_inpPin[i]->setX( m_inpPin[i]->x()-4);
    //    m_inpPin[i]->setSpace( 1 );
    //    m_inpPin[i]->setFontSize( 4 );
    //    m_inpPin[i]->setLabelColor( QColor( 250, 250, 200 ) );
    //}

    m_showEnablePin = false;
    m_enablePin = m_inpPin[4];
    m_enablePin->setInverted( true );
    //m_enablePin->setVisible( false );

    m_showDotPin = false;
    m_dotPin = m_inpPin[5];
    //m_dotPin->setVisible( false );

    //setLabelPos(-16,-40, 0);

    addPropGroup( { "Main", {
new boolProp<SevenSegmentBCD>("Show_Point_Pin", "Show Point Pin","", this
                      , &SevenSegmentBCD::isShowDotPin, &SevenSegmentBCD::setShowDotPin, propNoCopy ),
new boolProp<SevenSegmentBCD>("Show_Enable_Pin", "Show Enable Pin","", this
                      , &SevenSegmentBCD::isShowEnablePin, &SevenSegmentBCD::setShowEnablePin, propNoCopy )
    },groupNoCopy} );

    Simulator::self()->addToUpdateList( this );

    initialize();
}
SevenSegmentBCD::~SevenSegmentBCD(){}

void SevenSegmentBCD::updateStep()
{
    if( !m_changed ) return;
    m_changed = false;

    if( !Simulator::self()->isRunning() ) m_digit = 0;
    else if( !m_linkedTo ){
        if( m_enablePin->getInpState() ){
            BcdBase::voltChanged();
            if( m_dotPin->getInpState() ) m_digit |= 0x80;
        }
        else m_digit = 0;
    }

}

void SevenSegmentBCD::voltChanged() { m_changed = true; }

void SevenSegmentBCD::setShowEnablePin( bool show )
{
    if( m_showEnablePin == show ) return;
    m_showEnablePin = show;

    if( !show ) m_enablePin->removeConnector();
    //m_enablePin->setVisible( show );
}

void SevenSegmentBCD::setShowDotPin( bool show )
{
    if( m_showDotPin == show) return;
    m_showDotPin = show;

    if( !show ) m_dotPin->removeConnector();
    //m_dotPin->setVisible( show );
}


bool SevenSegmentBCD::setLinkedTo(Linkable *li)
{
    bool linked = Component::setLinkedTo( li );
    if( li && linked )
        for( uint32_t i=0; i<m_inpPin.size(); ++i ) m_inpPin[i]->removeConnector();

    // setHidden( (li && linked), false, false );

    return linked;
}

void SevenSegmentBCD::setLinkedValue(double v, int i)
{
    int vInt = v;
    vInt &= 0xFF;
    if( i == 0 ) m_digit = m_segments[vInt]; // Display value
    else         m_digit = vInt;           // 1 bit for each segment
    m_changed = true;
}


