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

#include "SimMessageHandler.h"
#include "IMessageHandler.h"
std::unordered_map<std::string, size_t> lastReadPosMap;
std::mutex meterDataMutex; // 添加的互斥锁
enum class Operation {
  Start_Simulide,
  Resume_Simulide,
  Stop_Simulide,
  SetSpeed_Simulide,
  Pause_Simulide,
  Update_Simulide_Attr,
  Unknown
};
Operation hash_string(std::string const &inString) {
  static const std::unordered_map<std::string, Operation> string_to_enum = {
      {"Start_Simulide", Operation::Start_Simulide},
      {"Resume_Simulide", Operation::Resume_Simulide},
      {"Stop_Simulide", Operation::Stop_Simulide},
      {"SetSpeed_Simulide", Operation::SetSpeed_Simulide},
      {"Pause_Simulide", Operation::Pause_Simulide},
      {"Update_Simulide_Attr", Operation::Update_Simulide_Attr},
  };
  auto it = string_to_enum.find(inString);
  if (it != string_to_enum.end()) {
    return it->second;
  }
  return Operation::Unknown;
}
bool startsWithWire(const std::string &str) {
  return str.compare(0, 4, "Wire") == 0;
}
// 数据更新回调处理函数
static void dataUpdateCallback(const OutputData &data, server *s,
                               websocketpp::connection_hdl hdl) {
  nlohmann::json response;
  try {
    if (data.meterData) {
      for (const auto &Data : *data.meterData) {
        if (!startsWithWire(Data->id)) {
          auto &pos = lastReadPosMap[Data->id];
          response[Data->id] = Data->datas.getDelta(pos);
        } else {
          auto &pos = lastReadPosMap[Data->id];
          auto allData = Data->datas.getDelta(pos);
          if (!allData.empty()) {
            response[Data->id] = allData.back();
          }
        }
      }
    }
    if (data.statusData) {
      for (const auto &Data : *data.statusData) {
        auto &pos = lastReadPosMap[Data->id];
        auto allData = Data->datas.getDelta(pos);
        if (!allData.empty()) {
          response[Data->id] = allData.back();
        }
      }
    }
    if (!response.empty()) {
      s->send(hdl, response.dump(), websocketpp::frame::opcode::text);
    }
  } catch (const std::exception &e) {
    std::cerr << "回调处理错误1: " << e.what() << std::endl;
  }
}
void simulate_and_update_data(Circuit *cir, server *s,
                              websocketpp::connection_hdl hdl) {
  // 注册回调函数 (使用lambda捕获需要的参数)
  auto callback = [s, hdl](const OutputData &data) {
    dataUpdateCallback(data, s, hdl);
  };
  cir->m_simulator->registerDataCallback(callback);
  while (true) {
    std::this_thread::sleep_for(std::chrono::microseconds(20));
  }
}

void simulate_and_send_data(Circuit *cir, server *s,
                            websocketpp::connection_hdl hdl) {
  nlohmann::json response;
  std::vector<std::vector<double>> sendData(2);
  while (true) {
    try {
      std::lock_guard<std::mutex> lock(cir->m_simulator->m_meterMutex);
      response.clear();
      OutputData scopeDatas = cir->m_simulator->getOutputData();
      if (scopeDatas.meterData) {
        for (const auto &Data : *scopeDatas.meterData) {
          if (!startsWithWire(Data->id)) {
            auto &pos = lastReadPosMap[Data->id];
            auto alldata = Data->datas.getDelta(pos);
            if (!alldata.empty()) {
              response[Data->id] = alldata;
            }
          } else {
            auto &pos = lastReadPosMap[Data->id];
            auto allData = Data->datas.getDelta(pos);
            if (!allData.empty()) {
              response[Data->id] = allData.back();
            }
          }
        }
      }
      if (scopeDatas.statusData) {
        for (const auto &Data : *scopeDatas.statusData) {
          auto &pos = lastReadPosMap[Data->id];
          auto allData = Data->datas.getDelta(pos);
          if (!allData.empty()) {
            response[Data->id] = allData.back();
          }
        }
      }
      if (!response.empty()) {
        s->send(hdl, response.dump(), websocketpp::frame::opcode::text);
      }
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

void SimMessageHandler::handleMessage(const SimMessage &message) {
  nlohmann::json response;
  switch (hash_string(message.operation)) {
  case Operation::Start_Simulide: {
    std::string Netlist = message.params["Netlist"].get<std::string>();
    ItemLibrary *Factor = new ItemLibrary();
    std::cout << "Start Sim" << std::endl;
    cir = new Circuit(Netlist);
    cir->m_simulator->startSim();
    std::cout << "StartSim_Ok" << std::endl;
    std::thread(simulate_and_send_data, cir, message.srv, message.handle)
        .detach();
    break;
  }
  case Operation::Resume_Simulide:
    cir->m_simulator->resumeSim();
    break;
  case Operation::Stop_Simulide:
    cir->m_simulator->stopSim();
    break;
  case Operation::SetSpeed_Simulide: {
    std::string Speed = message.params["Speed"].get<std::string>();
    try {
      int speed = std::stoi(Speed);
      cir->m_simulator->setSpeed(speed);
    } catch (const std::exception &e) {
      std::cerr << "速度设置错误: " << e.what() << std::endl;
      cir->m_simulator->setSpeed(8); // 使用默认值
    }
    break;
  }
  case Operation::Pause_Simulide:
    cir->m_simulator->pauseSim();
    break;
  case Operation::Update_Simulide_Attr: {
    if (cir != nullptr) {
      std::string updateCirID =
          message.params["updateCirID"].get<std::string>();
      std::string attrInput = message.params["attrInput"].get<std::string>();
      std::string updateValueStr;
      if (message.params["updateValue"].is_boolean()) {
        updateValueStr = message.params["updateValue"].dump();
      } else {
        updateValueStr = message.params["updateValue"];
      }
      std::vector<std::string> data{updateCirID, attrInput, updateValueStr};
      auto result = cir->upDateCmp(data);
      response["updateCmpResult"] = result;
    } else {
      response["updateCmpResult"] = false;
      response["error"] = "Circuit not initialized";
    }
    message.srv->send(message.handle, response.dump(),
                      websocketpp::frame::opcode::text);
    break;
  }
  default:
    break;
  }
}
