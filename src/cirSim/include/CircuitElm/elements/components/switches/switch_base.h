/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "mech_contact.h"

class CustomButton;


class SwitchBase : public MechContact
{
    public:
        SwitchBase( std::string type, std::string id );
        ~SwitchBase();

        virtual void updateStep() override;

        void rotateText( double angle );

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        std::string key() { return m_key; }
        void setKey( std::string key );
        
        CustomButton* button() { return m_button; }     //可能涉及到开关状态。。。。。。。。。。。。。。。。。。

        virtual void onbuttonclicked();
        virtual void keyEvent( std::string key, bool pressed ){;}

    protected:

        std::string m_key;
        
        CustomButton* m_button;        

};
