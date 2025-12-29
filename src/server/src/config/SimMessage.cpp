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

#include "SimMessage.hpp"
#include <iostream>

SimMessage SimMessage::fromJson(const json &j, server *srv,
                                websocketpp::connection_hdl handle) {

  SimMessage message;
  message.srv = srv;
  message.handle = handle;
  try {
    // 解析核心层级字段
    message.system = j.value(
        "system", "simulation"); // 默认simulation ,便于后面复杂操作分层级使用
    if (j.contains("system")) {
      message.system = j["system"].get<std::string>();
    }
    if (j.contains("module")) {
      message.module = j["module"].get<std::string>();
    }
    if (j.contains("component")) {
      message.component = j["component"].get<std::string>();
    }
    if (j.contains("operation")) {
      message.operation = j["operation"].get<std::string>();
    }
    // 解析会话和参数
    if (j.contains("sessionId")) {
      message.sessionId = j["sessionId"].get<std::string>();
    }
    if (j.contains("params") && j["params"].is_object()) {
      message.params = j["params"];
    }

  } catch (json::parse_error &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
  } catch (nlohmann::json::type_error &e) {
    std::cerr << "Type error: " << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return message;
}
