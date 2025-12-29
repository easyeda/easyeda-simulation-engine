/***************************************************************************
 *   Modified (C) 2025 by easyEDAJLC Technology Group                      *
 *   ouzhifeng@sz-jlc.com                                                  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <iostream>
#include <unordered_map>
#include "common.h"


typedef websocketpp::server<websocketpp::config::asio> server;
using json = nlohmann::json;
using websocketpp::lib::placeholders::_1; // 占位符
using websocketpp::lib::placeholders::_2; // 占位符
using websocketpp::lib::bind;
typedef server::message_ptr message_ptr;
using HandlerFunction = std::function<void(const json&, server*, websocketpp::connection_hdl)>;
using SimulationFunction = std::function<std::string(const std::string&)>;
using HandlerFunction = std::function<void(const json&, server*, websocketpp::connection_hdl)>;



class EventDispatcher {
public:
    // 注册事件处理函数
    void registerHandler(const std::string& eventType, HandlerFunction handler) ;

    // 调用事件处理函数
    void dispatch(const std::string& eventType, const json& j, server* s, websocketpp::connection_hdl hdl) ;

private:
    std::unordered_map<std::string, HandlerFunction> handlers;
};
#endif // EVENT_DISPATCHER_H