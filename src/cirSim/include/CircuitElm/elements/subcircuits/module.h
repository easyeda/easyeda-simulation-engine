/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "shield.h"

class ModuleSubc : public ShieldSubc
{
    public:
        ModuleSubc( std::string type, std::string id );
        ~ModuleSubc();
        
        double zVal() { return 0; }               //------------------------------------后续需要修改
        void setZVal( double v);

    protected:
        virtual void renameTunnels() override;
};
