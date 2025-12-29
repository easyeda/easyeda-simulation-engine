/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "gate_or.h"
#include "itemlibrary.h"

#include "intProp.h"
#include "boolProp.h"

#define tr(str) simulideTr("OrGate",str)

Component* OrGate::construct( std::string type, std::string id )
{ return new OrGate( type, id ); }

LibraryItem* OrGate::libraryItem()
{
    return new LibraryItem(
        "Or Gate",
        "Gates",
        "orgate.png",
        "Or Gate",
        OrGate::construct );
}

OrGate::OrGate( std::string type, std::string id )
      : Gate( type, id, 2 )
{
    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
    new boolProp<OrGate>("Invert_Inputs", "Invert Inputs","", this
                        , &OrGate::invertInps, &OrGate::setInvertInps, propNoCopy),

    new intProp <OrGate>("Num_Inputs", "Input Size","_Inputs", this
                        , &OrGate::numInps, &OrGate::setNumInputs, propNoCopy,"uint"),
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

    addPropGroup( { "Timing", Gate::edgeProps(),0 } );

    // removeProperty("pd_n");

}
OrGate::~OrGate(){}

bool OrGate::calcOutput( int inputs ) { return (inputs > 0); }