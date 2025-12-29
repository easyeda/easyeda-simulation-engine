/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "e-element.h"
#include "component.h"
#include "e-coil.h"

class LibraryItem;

//变压器
class Transformer : public Component, public eElement
{
        struct winding_t
        {
            std::string definition;
            std::string topology;
            std::string prefix;
            std::vector<Pin*>   pins;
            std::vector<eCoil*> coils;
            int nCoils;
            int start;
            int angle;
            int x;
        };

    public:
        Transformer( std::string type, std::string id );
        ~Transformer();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void runEvent() override;

        double baseInd() { return m_baseInd; }
        void setBaseInd( double i );

        std::string primary() { return m_primary.definition; }
        void setPrimary( std::string p );

        std::string secondary() { return m_secondary.definition; }
        void setSecondary( std::string s );

        double coupCoeff() { return m_coupCoeff; }
        void setCoupCoeff( double c );
    private:
        void setupInducts( winding_t* w );
        void createCoils();
        int  getLeght( winding_t* w );
        void modify( winding_t* w );
        void create( winding_t* w );

        int m_lenght;

        double m_baseInd;
        double m_coupCoeff;

        uint64_t m_reacStep;

        winding_t m_primary;
        winding_t m_secondary;

        std::vector<eCoil*> m_coils;

        std::vector<std::vector<double>> m_inva;
};

