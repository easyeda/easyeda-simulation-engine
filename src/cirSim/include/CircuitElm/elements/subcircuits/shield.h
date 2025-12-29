/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#pragma once

#include "board.h"

// 子电路板
class ShieldSubc : public BoardSubc
{
    public:
        ShieldSubc( std::string type, std::string id );
        ~ShieldSubc();

        std::string boardId() { return m_boardId; }
        void setBoardId( std::string id ) { m_boardId = id; }
        void setBoard( BoardSubc* board );

        void connectBoard();

        virtual void remove() override;

    protected:
        virtual void attachToBoard();
        virtual void renameTunnels();

        bool m_attached; // This is a shield which is attached to a board

        BoardSubc* m_board;  // A board this is attached to (this is a shield)
        std::string m_boardId;
};