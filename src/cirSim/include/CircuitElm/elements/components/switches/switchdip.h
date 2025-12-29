/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

class LibraryItem;
class CustomButton;

//开关组
class SwitchDip : public Component, public eElement
{
    public:
        SwitchDip( std::string type, std::string id );
        ~SwitchDip();

        static Component* construct( std::string type, std::string id );
        static LibraryItem *libraryItem();

        int  size() { return m_size; }
        void setSize( int size );

        int  state() { return m_state; }
        void setState( int state );

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void remove() override;

        void createSwitches( int c );
        void deleteSwitches( int d );
        virtual void updateState(int index) override;

       void onbuttonclicked();

    private:
        std::vector<CustomButton*> m_buttons;

        int m_size;
        int m_state;
};


