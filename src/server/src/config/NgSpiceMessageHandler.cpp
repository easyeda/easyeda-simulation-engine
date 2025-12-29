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

#include "NgSpiceMessageHandler.h"
#include "IMessageHandler.h"
#include <random> // For std::fixed and std::setprecision

void ngSpiceThreadStaticInit(ngspice_de *cir) { cir->ngSpiceStaticRunInt(); }
bool anyVectorExceeds(const std::vector<SimData> &trends,
                      size_t threshold = 100000) {
  for (const auto &simData : trends) {
    // 检查每一个向量
    if (simData.voltData.size() > threshold ||
        simData.gainData.size() > threshold ||
        simData.phaseData.size() > threshold ||
        simData.currentData.size() > threshold ||
        simData.digitalData.size() > threshold) {
      return true;
    }
  }
  return false;
}
std::string generateRandomVoltage(double minVoltage, double maxVoltage) {
  // 创建随机数生成器
  std::random_device rd;  // 获取一个随机数种子
  std::mt19937 gen(rd()); // 初始化 Mersenne Twister 随机数生成器
  std::uniform_real_distribution<> dis(minVoltage,
                                       maxVoltage); // 定义均匀分布范围

  // 生成随机电压值
  double randomVoltage = dis(gen);

  // 格式化电压值为字符串
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(2) << randomVoltage;
  return "alter v5 = " + stream.str() + "V";
}
// 定义 std::pair<double, double> 的序列化函数
void to_json(nlohmann::json &j, const std::pair<double, double> &p) {
  j = nlohmann::json{{"first", p.first}, {"second", p.second}};
}

void to_json(nlohmann::json &j, const SimData &data) {
  std::vector<std::pair<double, double>> voltDataCopy;
  if (!data.voltData.empty()) {
    voltDataCopy = data.voltData; // 优先拷贝电压数据（如果不空）
  } else if (!data.currentData.empty()) {
    voltDataCopy = data.currentData; // 如果电压为空，但电流不空，则拷贝电流
  }
  j = nlohmann::json{
      {"id", data.scopeMes.id},      {"currentData", data.currentData},
      {"voltData", data.voltData},   {"gainData", data.gainData},
      {"phaseData", data.phaseData}, {"digitalData", data.digitalData},
  };
}

void NgSpiceMessageHandler::handleMessage(const SimMessage &message) {
  if (message.operation == "Add_ProbeNode") {
    std::string ProbeNode = message.params["ProbeNode"].get<std::string>();
    int ProbeType = message.params["ProbeType"].get<int>();
    double lowLevel = message.params["LowLevel"].get<double>();
    double hightLevel = message.params["HighLevel"].get<double>();

    if (message.getUseDataAsDouble(ProbeNode)) {
      ProbeNode = "V(" + ProbeNode + ")";
    } else {
      ProbeNode[0] = std::tolower(ProbeNode[0]);
    }
    Scope myScopes{ProbeNode, ProbeType, hightLevel, lowLevel};
    cir1->addScope(myScopes);
    volt = ProbeNode;
  }
#include <future>
#include <thread>
  if (message.operation == "Start_Ngspice") {
    std::string str = message.params["Netlist"].get<std::string>();
    cir1 = new ngspice_de(str);

    // 使用 promise 和 future 来实现超时控制
    std::promise<bool> sim_promise;
    std::future<bool> sim_future = sim_promise.get_future();

    // 启动仿真任务
    std::thread sim_thread([&]() {
      cir1->ngSpiceStaticRunInt();
      sim_promise.set_value(true); // 标记仿真完成
    });

    // 设置超时检查
    nlohmann::json response;
    bool timed_out = false;

    if (sim_future.wait_for(std::chrono::seconds(10)) !=
        std::future_status::ready) {
      // 超时处理
      timed_out = true;

      // 添加超时错误到错误消息
      std::vector<std::string> errorMes;
      errorMes.push_back("The drawing failed. Simulation timed out.");
      std::vector<SimData> empty_trends;
      response["ngspiceErrorMes"] = errorMes;

      // 发送错误响应
      message.srv->send(message.handle, response.dump(),
                        websocketpp::frame::opcode::text);
      sim_thread.detach();
      std::cout << "Simulation timed out after 10 seconds" << std::endl;
    }

    // 如果未超时但在这里，仿真已完成
    if (!timed_out) {
      // 获取仿真结果
      std::vector<SimData> trends = cir1->getCompleteScopeDatas();

      // 合并错误消息
      std::vector<std::string> errorMes = cir1->getErrorMessage();
      std::vector<std::string> warningMes = cir1->getWarningMessage();

      // 检查数据量是否过大
      if (anyVectorExceeds(trends, 100000)) {
        errorMes.push_back(
            "The drawing failed. The data volume is too large (>100,000 "
            "points). Please adjust the simulation step size or reduce the "
            "number of detection points");
      }

      // 构建响应
      response["ngspiceWaveVector"] = trends;
      if (!errorMes.empty()) {
        response["ngspiceErrorMes"] = errorMes;
      }
      if (!warningMes.empty()) {
        response["ngspiceWarningMes"] = warningMes;
      }

      message.srv->send(message.handle, response.dump(),
                        websocketpp::frame::opcode::text);
      std::cout << "Data sent and cleared" << std::endl;
    }

    // 清理资源
    if (sim_thread.joinable()) {
      sim_thread.join(); // 确保线程安全退出
    }
    std::cout << "ngspice exit" << std::endl;
    delete cir1;
    cir1 = nullptr;
  }
}
