/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "buffer.h"
#include "itemlibrary.h"
#include "iopin.h"

#include "boolProp.h"



Component* Buffer::construct( std::string type, std::string id )
{ return new Buffer( type, id ); }

LibraryItem* Buffer::libraryItem()
{
    return new LibraryItem(
        "Buffer",
        "Gates",
        "buffer.png",
        "Buffer",
        Buffer::construct );
}

Buffer::Buffer( std::string type, std::string id )
      : Gate( type, id, 1 )
{
    //m_area = QRect(-8, -8, 16, 16 );
    
    setOePin( new IoPin(m_id+"-Pin_outEnable", 0, this, input ) );
    Buffer::setTristate( false );

    addPropGroup( { "Main", {},0} );

    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new boolProp<Buffer>("Invert_Inputs", "Invert Inputs","", this
                            , &Buffer::invertInps, &Buffer::setInvertInps, propNoCopy),
        new boolProp<Buffer>("Tristate", "Tristate","", this
                     , &Buffer::tristate, &Buffer::setTristate, propNoCopy)
    };
    std::vector<comProperty*> outputProps = IoComponent::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将四类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size() + outputType.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());

    addPropGroup({ "Electric",allProps,0 });

    addPropGroup({ "Timing" , Gate::edgeProps(),0 } );

    // removeProperty("pd_n");
}
Buffer::~Buffer(){}

void Buffer::setTristate( bool t )  // Activate or deactivate OE Pin
{
    LogicComponent::setTristate( t );
    //m_oePin->setLabelText( "" );
}
