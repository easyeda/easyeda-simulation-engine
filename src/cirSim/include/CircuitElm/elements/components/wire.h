/***************************************************************************
 *    Modified (C) 2025 by EasyEDA & JLC Technology Group                     *
 *    chensiyu@sz-jlc.com                                                  *
 *                                                                         *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU Affero General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Affero General Public      *
 *   License along with this program. If not, see                          *
 *   <http://www.gnu.org/licenses/>.                                       *
 ***************************************************************************/

#pragma once

#include "e-resistor.h"
#include "component.h"

class LibraryItem;
class IoPin;

class Wire : public Component, public eResistor
{
    public:
        Wire( std::string type, std::string id );
        ~Wire();

        static Component* construct( std::string type, std::string id );
        static LibraryItem* libraryItem();

        virtual void updateStep() override;
        virtual double getElementData() override;

    protected:
    std::string m_unit;
    double  m_current;
    bool m_switchPins;
    IoPin* m_outPin;

};
