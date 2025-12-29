/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "push.h"
#include "pin.h"
#include "itemlibrary.h"

#include "stringProp.h"
#include "boolProp.h"
#include "intProp.h"

#define tr(str) simulideTr("Push",str)

Component* Push::construct( std::string type, std::string id )
{ return new Push( type, id ); }

LibraryItem* Push::libraryItem()
{
    return new LibraryItem(
        "Push",
        "Switches",
        "push.png",
        "Push",
        Push::construct);
}

Push::Push( std::string type, std::string id )
    : PushBase( type, id )
{
    SetupSwitches( 1, 1 );

    addPropGroup( { "Main", {
        new intProp <Push>("Poles"     , "Poles"          ,"_Poles", this, &Push::poles,  &Push::setPoles, propNoCopy,"uint" ),
    }, 0} );
}
Push::~Push(){}