/***************************************************************************
 *   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
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

#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <atomic>
#include <string>
#include <functional>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <ngspice_de.h>
#include "circuit.h"
#include"eventDispatcher.h"
#include "meter.h"
#include "simulator.h"
#include "itemlibrary.h"
// 全局变量
extern std::atomic<bool> running;
extern ngspice_de* cir1 ;
extern Circuit* cir;
extern std::mutex mtx;
extern std::stringstream gNgspiceOutput; // 用于收集 ngspice 输出的全局 stringstream
extern std::vector<int> myData;
extern std::mutex gMutex;
extern std::condition_variable gCondVar;
extern std::queue<std::pair<websocketpp::connection_hdl, std::string>> gMessageQueue;
// WebSocket 消息处理函数
extern std::vector<std::pair<double, double>> data_buffer;
extern std::string volt;






// 类型定义
typedef websocketpp::server<websocketpp::config::asio> server;
using json = nlohmann::json;
using websocketpp::lib::placeholders::_1; // 占位符
using websocketpp::lib::placeholders::_2; // 占位符
using websocketpp::lib::bind;
typedef server::message_ptr message_ptr;
using HandlerFunction = std::function<void(const json&, server*, websocketpp::connection_hdl)>;
using SimulationFunction = std::function<std::string(const std::string&)>;

#endif // COMMON_H