/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

// #pragma once

// //#include <QWidget>
// #include <vector>

// //#include "ui_memtable.h"

// class Component;
// //class QAction;

// class MemTable 
// {
//     //Q_OBJECT
    
//     public:
//         MemTable(int dataSize = 256, int wordBytes=1 );    // QWidget* parent=0,, 

//         void updateTable( std::vector<int>* data );
//         void setData( std::vector<int>* data, int wordBytes=1 );
//         void setValue( int address, int val );
//         void setCellBytes( int bytes );
//         void setAddrSelected( int addr ,bool jump );

//     //signals:
//     //    void dataChanged( int address, int val );

//     //public slots:
//     //    void on_table_itemChanged( QTableWidgetItem* item );
//     //    void on_table_cellClicked( int row, int col ) { cellClicked( row, col ); }
//     //    void on_table_itemEntered( QTableWidgetItem* item );
//     //    void on_context_menu_requested( const QPoint &pos );
//     //    void saveTable();
//     //    void loadTable();

//     private:
//         void resizeTable( int dataSize );
//         //void setCellValue( int address, int val );
//         void cellClicked( int row, int col );
//         std::string valToHex( int val, int bytes );
//         std::vector<int> toIntVector();

//         int m_updtCount;
//         int m_dataSize;
//         int m_wordBytes;
//         int m_cellBytes;
//         int m_byteRatio; // m_wordBytes/m_cellBytes
//         int m_addrBytes;

//         bool m_blocked;

//         bool m_canSaveLoad;

//         //QTableWidgetItem* m_hoverItem;
//         std::vector<int>* m_data;
// };

