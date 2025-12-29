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
#include "ledsmd.h"

class LibraryItem;

//LED矩阵
class LedMatrix : public Component, public eElement
{
    public:
        LedMatrix( std::string type, std::string id );
        ~LedMatrix();

 static Component* construct( std::string type, std::string id );
 static LibraryItem* libraryItem();

        std::string colorStr();
        void setColorStr( std::string color );

        int  rows() { return m_rows; }
        void setRows( int rows );
        
        int  cols() { return m_cols; }
        void setCols( int cols );
        
        bool verticalPins() { return m_verticalPins; }
        //void setVerticalPins( bool v );    //设置垂直引脚暂时没搞懂有什么用
        
        double threshold() { return m_threshold; }
        void   setThreshold( double threshold );

        double maxCurrent() { return m_maxCurr; }
        void   setMaxCurrent( double current );
        
        double res() { return m_resist; }
        void  setRes( double resist );

        virtual void stamp() override;
        virtual void remove() override;

        //virtual void setHidden( bool hid, bool hidArea=false, bool hidLabel=false ) override;

        virtual std::vector<std::string>getEnumUids( std::string ) override;
        virtual std::vector<std::string>getEnumNames( std::string ) override;
        
    private:
        void setupMatrix( int rows, int cols );
        void createMatrix();
        void deleteMatrix();
        
        std::vector<std::vector<LedSmd*>> m_led;
        std::vector<Pin*> m_rowPin;
        std::vector<Pin*> m_colPin;
        
        bool m_verticalPins;

        double m_resist;
        double m_maxCurr;
        double m_threshold;
        
        int m_rows;
        int m_cols;
};

