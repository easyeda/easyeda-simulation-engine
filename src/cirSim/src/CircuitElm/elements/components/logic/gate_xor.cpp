/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "gate_xor.h"
#include "itemlibrary.h"

#include "boolProp.h"

#define tr(str) simulideTr("XorGate",str)

Component* XorGate::construct( std::string type, std::string id )
{ return new XorGate( type, id ); }

LibraryItem* XorGate::libraryItem()
{
    return new LibraryItem(
        "Xor Gate" ,
        "Gates",
        "xorgate.png",
        "Xor Gate",
        XorGate::construct );
}

XorGate::XorGate( std::string type, std::string id )
       : Gate( type, id, 2 )
{
    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
    new boolProp<XorGate>("Invert_Inputs", "Invert Inputs","", this, &XorGate::invertInps, &XorGate::setInvertInps,propNoCopy) };
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

    addPropGroup( { "Timing", Gate::edgeProps(),0 } );

    // removeProperty("pd_n");
}
XorGate::~XorGate(){}

bool XorGate::calcOutput( int inputs ) { return (inputs == 1); }