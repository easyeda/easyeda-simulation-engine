/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-led.h"
#include "component.h"
#include "e-node.h"

class eNode;

class LedBase : public Component, public eLed
{
    public:
        LedBase( std::string type, std::string id );
        ~LedBase();
        
        enum LedColor {
            yellow = 0,
            red,
            green,
            blue,
            orange,
            purple,
            white, 
        };

        std::string colorStr() {     
            const size_t index  = static_cast<size_t>(m_ledColor);
            if (index  >= COLOR_NAMES.size()) [[unlikely]] {
             return "Invalid";
            }
            return COLOR_NAMES[index ]; 
        }
        void setColorStr( std::string color );

        bool grounded() { return m_grounded; }
        void setGrounded( bool grounded );

        virtual void initialize() override;
        virtual void updateStep() override;

    protected:
        static constexpr std::array<const char*, 7> COLOR_NAMES = { 
            "Yellow", "Red", "Green", "Blue", "Orange", "Purple", "White"
        };
        bool   m_grounded;

 static int m_overBright;
        
        LedColor m_ledColor;

        static eNode m_gndEnode;
};

