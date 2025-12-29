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

#ifndef SIMMESSAGE_H
#define SIMMESSAGE_H

#include <string>
#include <nlohmann/json.hpp>
#include "common.h"
using json = nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;

class SimMessage {
public:
    // 核心层级字段
    std::string system;        // 系统层级：simulation/measurement/diagnostics
    std::string module;        // 模块层级：simulide/ngspice/session/server
    std::string component;     // 组件层级：execution/probes/components/maintenance
    std::string operation;     // 操作指令：start/stop/updateAttribute/addNode 等

    // 会话和参数
    std::string sessionId;      // 会话标识符
    json params;                // 动态参数（JSON对象）


    // WebSocket连接相关
    server* srv;
    websocketpp::connection_hdl handle;

        // 静态方法用于从JSON创建消息
    static SimMessage fromJson(const json& j, server* srv, websocketpp::connection_hdl handle);


        // 辅助方法
    std::string getParamAsString(const std::string& key, const std::string& defaultValue = "") const {
        return params.contains(key) ? params[key].get<std::string>() : defaultValue;
    }

    double getParamAsDouble(const std::string& key, double defaultValue = 0.0) const {
        if (!params.contains(key)) return defaultValue;
        if (params[key].is_number()) return params[key].get<double>();
        try {
            return std::stod(params[key].get<std::string>());
        } catch (...) {
            return defaultValue;
        }
    }
    double getUseDataAsDouble(std::string value) const {    
        try {
            return std::stod(value);
        } catch (const std::invalid_argument& e) {
        // 如果不是有效的数字，返回一个特殊值（例如 NaN）或抛出异常
            return 0;
        } catch (const std::out_of_range& e) {
            return 0;
        }
    }


private:
    std::string payload;

};

#endif // MESSAGE_H
