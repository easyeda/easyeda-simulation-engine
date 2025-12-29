/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef THERMISTORBASE_H
#define THERMISTORBASE_H

#include "varresbase.h"

class ThermistorBase : public VarResBase
{
    public:
        ThermistorBase( std::string type, std::string id );
        ~ThermistorBase();

        bool ptc()  { return m_ptc; }
        void setPtc( bool ptc );

        virtual void setMaxVal( double max ) override;
        virtual void setMinVal( double min ) override;

    protected:
        bool m_ptc;
};

#endif
