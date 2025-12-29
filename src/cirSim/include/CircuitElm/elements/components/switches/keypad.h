/***************************************************************************
 *   Copyright (C) 2018 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-element.h"
#include "push_base.h"

class LibraryItem;

//小键盘
class KeyPad : public Component, public eElement
{
    public:
        KeyPad( std::string type, std::string id );
        ~KeyPad();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        int rows() { return m_rows; }
        void setRows( int rows );
        
        int cols() { return m_cols; }
        void setCols( int cols );
        
        std::string keyLabels() { return m_keyLabels; }
        void setKeyLabels( std::string keyLabels );
        
        virtual void stamp() override;
        virtual void remove() override;

    protected:
        void setupButtons();

        int m_rows;
        int m_cols;
        
        std::string m_keyLabels;
    };


