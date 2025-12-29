/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "mux.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "connector.h"
#include "iopin.h"

#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("Mux",str)

Component* Mux::construct( std::string type, std::string id )
{ return new Mux( type, id ); }

LibraryItem* Mux::libraryItem()
{
    return new LibraryItem(
        "Mux",
        "Converters" ,
        "mux.png",
        "Mux",
        Mux::construct );
}

Mux::Mux( std::string type, std::string id )
   : LogicComponent( type, id )
{
    m_width  = 4;
    m_height = 10;
    m_tristate = true;

    setNumInps( 8,"D" );
    
    init({          // Inputs:
            "ID03S0",
            "ID02S1",
            "ID01S2",
                   // Outputs:
            "OR02Y",
            "OR03!Y"
        });

    //m_inpPin[9]->setLength( 6 );
    //m_inpPin[10]->setLength( 4 );

    createOePin( "IU03OE ", id+"-in11");

    m_addrBits = 0;
    setAddrBits( 3 );

    addPropGroup( { "Main", {
new intProp<Mux>("Address_Bits" , "Address Size" ,"_bits", this
                , &Mux::addrBits, &Mux::setAddrBits, propNoCopy )
    }, groupNoCopy } );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<Mux>("Invert_Inputs", "Invert Inputs","", this
                , &Mux::invertInps, &Mux::setInvertInps, propNoCopy) 
    };
    std::vector<comProperty*> outputProps = IoComponent::outputProps();

    //将三类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());


    addPropGroup({ "Electric",allProps,0 });

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );
}
Mux::~Mux(){}

void Mux::stamp()
{
    for( int i=0; i<11; ++i ) m_inpPin[i]->changeCallBack( this );
    LogicComponent::stamp();
}

void Mux::voltChanged()
{
    LogicComponent::updateOutEnabled();

    int address = 0;

    for( int i=8; i<11; ++i )
        if( m_inpPin[i]->getInpState() ) address += pow( 2, i-8 );

    bool out = m_inpPin[address]->getInpState();
    m_nextOutVal = out? 1:2;
    scheduleOutPuts( this );
}

void Mux::setAddrBits( int bits )
{
    if( m_addrBits == bits ) return;

    if     ( bits < 1 ) bits = 1;
    else if( bits > 3 ) bits = 3;

    int channels = pow( 2, bits );
    m_addrBits = bits;

    m_height = channels+2;
    int bit0 = 8;
    int w = m_width*8/2;
    int h = m_height*8/2;

    if (Simulator::self()->isRunning())
        Simulator::self()->stopSim();

    for( int i=0; i<3; ++i )
    {
        Pin* pin = m_inpPin[bit0+i];
        if(i >= bits)
            pin->removeConnector();
        //if( i < bits )
        //{
        //    pin->setVisible( true );
        //    pin->setY( h+8 );
        //}else{
        //    pin->removeConnector();
        //    pin->setVisible( false );
        //}
        //if( i < 2 ) m_outPin[i]->setY( -h+i*8+16 ); // Outputs
    }
    for( int i=0; i<8; ++i )
    {
        Pin* pin = m_inpPin[i];
        if (i >= channels)
        {
            pin->removeConnector();
        }
        //if( i < channels )
        //{
        //    pin->setVisible( true );
        //    pin->setY( i*8-(bits+bits/3)*8 );
        //}else{
        //    pin->removeConnector();
        //    pin->setVisible( false );
        //}   
    }
    //m_oePin->setY( -h-8 ); // OE

    //m_area = QRect(-w-1,-h-8-1, w*2+2, h*2+16+2 );
    //Circuit::self()->update();
}