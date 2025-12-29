/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef LDR_H
#define LDR_H

#include "varresbase.h"

class LibraryItem;

class Ldr : public VarResBase
{
    public:
        Ldr( std::string type, std::string id );
        ~Ldr();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        int r1() { return m_r1; }
        void setR1( int r1 );

        double gamma() { return m_gamma; }
        void setGamma( double ga );

        virtual void updateStep() override;
    private:
        int m_r1;

        double m_gamma;
};

#endif
