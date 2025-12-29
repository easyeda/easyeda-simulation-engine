/***************************************************************************
 *   Copyright (C) 2010 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */


#include "gate_and.h"
#include "itemlibrary.h"

#include "intProp.h"
#include "boolProp.h"

//#define tr(str) simulideTr("AndGate",str)

Component* AndGate::construct( std::string type, std::string id )
{ return new AndGate( type, id ); }

LibraryItem* AndGate::libraryItem()
{
    return new LibraryItem(
        "And Gate" ,
        "Gates",
        "andgate.png",
        "And Gate",
        AndGate::construct );
}

AndGate::AndGate( std::string type, std::string id )
       : Gate( type, id, 2 )
{
    std::vector<comProperty*> inputProps = IoComponent::inputProps();
    std::vector<comProperty*> customProps = {
        new intProp <AndGate>("Num_Inputs", "Input Size",""
                             , this, &AndGate::numInps, &AndGate::setNumInputs, propNoCopy,"uint" ),

        new boolProp<AndGate>("Invert_Inputs", "Invert Inputs",""
                             , this, &AndGate::invertInps, &AndGate::setInvertInps, propNoCopy )
    };
    std::vector<comProperty*> outputProps = Gate::outputProps();
    std::vector<comProperty*> outputType = IoComponent::outputType();
    //将四类列表和并
    std::vector<comProperty*> allProps;
    allProps.reserve(inputProps.size() + customProps.size() + outputProps.size() + outputType.size()); // 预留足够的空间
    allProps.insert(allProps.end(), inputProps.begin(), inputProps.end());
    allProps.insert(allProps.end(), customProps.begin(), customProps.end());
    allProps.insert(allProps.end(), outputProps.begin(), outputProps.end());
    allProps.insert(allProps.end(), outputType.begin(), outputType.end());

    addPropGroup({ "Electric",allProps,0 });

    addPropGroup( { "Timing", IoComponent::edgeProps(),0 } );

}
AndGate::~AndGate(){}

