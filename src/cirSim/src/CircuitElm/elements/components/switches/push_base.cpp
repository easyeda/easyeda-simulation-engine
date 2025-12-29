/***************************************************************************
 *   Copyright (C) 2016 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#include "push_base.h"
#include "circuit.h"
#include "custombutton.h"

namespace KeyValues {
    const std::string up = "Up";
    const std::string down = "Down";
}

PushBase::PushBase( std::string type, std::string id )
        : SwitchBase( type, id )
{}
PushBase::~PushBase(){}

void PushBase::onButtonPressed()
{
   m_closed = true;
   m_button->setChecked( true );
   m_changed = true;
}

void PushBase::onButtonReleased()
{
   m_closed = false;
   m_button->setChecked( false );
   m_changed = true;
}

void PushBase::updateState(std::string mes){
   if( mes == KeyValues::up){
      onButtonReleased();
   } else if(mes == KeyValues::down){
      onButtonPressed();
   }
}

