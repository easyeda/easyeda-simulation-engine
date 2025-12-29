/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "component.h"
#include "e-element.h"

//class QDomElement;

class Chip : public Component, public eElement
{
    public:
        Chip( std::string type, std::string id );
        ~Chip();

        enum subcType_t{
            None=0,
            Logic,
            Board,
            Shield,
            Module
        };
        
        bool logicSymbol() { return m_isLS; }
        virtual void setLogicSymbol( bool ls );

        //virtual void setBackground( std::string bck ) override;

        std::string name() { return m_name; }
        virtual void setName( std::string name );

        subcType_t subcType() { return m_subcType; }

        std::string subcTypeStr() { return m_enumUids.at( (int)m_subcType ); }
        virtual void setSubcTypeStr( std::string s ){;}

        int pkgWidth() { return m_width; }

        void setBackData( std::vector<std::vector<int>>* d ) { m_backData = d; }

    protected:
        virtual Pin* addPin( std::string id, std::string type, std::string label,int pos){return NULL;}

        virtual Pin* updatePin( std::string id, std::string type, std::string label, int pos){return NULL;}

        void addNewPin( std::string id, std::string type, std::string label,int pos);

        virtual void initChip();
        virtual void initPackage(TiXmlElement*  root);

        //virtual void findHelp() override;

        //TiXmlDocument fileToDomDoc(const std::string& filePath, const std::string& context);
        
        //待定观察下它是否有用
        int m_width;       
        int m_height;
        
        bool m_isLS;
        bool m_initialized;

        subcType_t m_subcType;

        //QColor m_lsColor;
        //QColor m_icColor;

        std::string m_name;
        std::string m_pkgeFile;     // file containig package defs，芯片包所在位置
        std::string m_dataFile;

        std::vector<Pin*> m_unusedPins;

        std::vector<std::vector<int>>* m_backData;

        //QGraphicsTextItem m_label;
};

