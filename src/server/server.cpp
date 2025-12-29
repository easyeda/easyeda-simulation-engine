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

#include "pch.h"
#include <boost/locale.hpp>
#include <tinyxml.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "BusinessRuleValidator.h"
#include "JsonParser.hpp"
#include "NgSpiceMessageHandler.h"
#include "ServerMessageHandler.h"
#include "SimMessage.hpp"
#include "SimMessageHandler.h"
#include "common.h"
#include "itemlibrary.h"
#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <pwd.h>
#endif

#include <boost/asio.hpp>
#include <thread>

#ifndef DISPLAY
#define DISPLAY 1
#endif

std::atomic<bool> running(true);
ngspice_de *cir1 = nullptr;
std::shared_ptr<server> echo_server_ptr;
Circuit *cir = nullptr;
std::string volt(" ");

// 获取格式化的当前时间
std::string get_current_time_string() {
  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) %
                1000;

  std::tm now_tm;
#ifdef _WIN32
  localtime_s(&now_tm, &now_time_t); // Windows 安全版本
#else
  localtime_r(&now_time_t, &now_tm); // Linux/macOS 安全版本
#endif

  std::ostringstream oss;
  oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0')
      << std::setw(3) << now_ms.count();

  return oss.str();
}
// 获取当前进程 ID
pid_t get_process_id() {
#ifdef _WIN32
  return GetCurrentProcessId(); // Windows
#else
  return getpid(); // Linux/macOS/POSIX
#endif
}
void on_close(server *s, websocketpp::connection_hdl hdl) {
  std::cout << std::endl
            << "[" << get_current_time_string() << "] "
            << "PID:" << get_process_id()
            << " TID:" << std::this_thread::get_id()
            << " - Connection closed, stopping server." << std::endl;
  running = false; // 设置运行标志为false，以通知线程停止
  exit(0);
}
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
  EventDispatcher dispatcher;
  std::string payload = msg->get_payload();
  json j = json::parse(payload);
  json response;
  SimMessage message = JsonParser<SimMessage>::parse(j, s, hdl);
  BusinessRuleValidator::Result validationResult;
  if (!BusinessRuleValidator::validate(message, &validationResult)) {
    // 验证失败，发送错误响应
    json error = {{"status", "error"},
                  {"errorCode", validationResult.errorCode},
                  {"message", validationResult.errorMessage},
                  {"field", validationResult.invalidField}};
    s->send(hdl, error.dump(), websocketpp::frame::opcode::text);
    return;
  }
  dispatcher.registerHandler(
      "ngspice",
      [&message](const json &j, server *s, websocketpp::connection_hdl hdl) {
        NgSpiceMessageHandler handler;
        handler.handleMessage(message);
      });
  dispatcher.registerHandler(
      "simulide",
      [&message](const json &j, server *s, websocketpp::connection_hdl hdl) {
        SimMessageHandler handler;
        handler.handleMessage(message);
      });
  dispatcher.registerHandler(
      "server",
      [&message](const json &j, server *s, websocketpp::connection_hdl hdl) {
        ServerMessageHandler handler;
        handler.handleMessage(message);
      });
  dispatcher.dispatch(message.module, j, s, hdl);
}
void signal_handler(int signal) {
  if (signal == SIGINT) {
    std::cout << "SIGINT received, stopping server." << std::endl;
    running = false;
    echo_server_ptr->stop();
  }
}
std::atomic<bool> activated(false); // 新增：激活状态标志

void run_http_server() {
  try {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(
        io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 17965));

    for (;;) {
      boost::asio::ip::tcp::socket socket(io_service);
      acceptor.accept(socket);
      try {
        // 读取请求
        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, "\r\n\r\n");
        // 解析请求
        std::istringstream request_stream(
            boost::asio::buffer_cast<const char *>(buffer.data()));
        std::string method, path, protocol;
        request_stream >> method >> path >> protocol;
        // 处理特定请求
        if (method == "GET" && path.find("/eda_simulation_server?type=test") !=
                                   std::string::npos) {
          // *** 关键修改：检查是否为首次请求 ***
          if (!activated.exchange(true)) {
            // 首次请求 - 返回成功
            std::string response_headers = "HTTP/1.1 200 OK\r\n"
                                           "Access-Control-Allow-Origin: *\r\n"
                                           "Content-Type: application/json\r\n";

            std::string response_body = "{\"status\":\"success\"}";
            response_headers +=
                "Content-Length: " + std::to_string(response_body.length()) +
                "\r\n\r\n";

            boost::asio::write(
                socket, boost::asio::buffer(response_headers + response_body));
            std::time_t now = std::time(nullptr);
            std::tm *t = std::localtime(&now);
            std::cout << (t->tm_year + 1900) << "-" << (t->tm_mon + 1) << "-"
                      << t->tm_mday << std::endl;
            std::cout << "Activation confirmed. Server is operational."
                      << std::endl; // 日志输出
          } else {
            // 后续请求 - 返回错误
            std::string json_body = "{\"status\":\"error\","
                                    "\"message\":"
                                    "\"该引擎已经被使用过一次，需要重新初始化引"
                                    "擎或者重新打开引擎\"}";
            std::string error_response = "HTTP/1.1 403 Forbidden\r\n"
                                         "Access-Control-Allow-Origin: *\r\n"
                                         "Content-Type: application/json\r\n"
                                         "Content-Length: " +
                                         std::to_string(json_body.length()) +
                                         "\r\n\r\n" + json_body;
            boost::asio::write(socket, boost::asio::buffer(error_response));
            std::cout << "This engine has already been used once. Please "
                         "restart the engine to reinitialize"
                      << std::endl; // 日志输出
            exit(0);
          }
        } else {
          std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
          boost::asio::write(socket, boost::asio::buffer(response));
        }

      }

      catch (boost::system::system_error &e) {
        // 👇 这里处理 EOF，不退出循环
        if (e.code() == boost::asio::error::eof) {
          std::cerr << "[Warning] Client disconnected (EOF)." << std::endl;
        } else {
          std::cerr << "[Error] " << e.what() << std::endl;
        }
        // 继续循环，保持服务器监听
      }
    }
  } catch (std::exception &e) {
    std::cerr << "HTTP Server Exception: " << e.what() << std::endl;
  }
}
int main(int argc, char *argv[]) {

#ifdef DISPLAY
#ifdef _WIN32
  HWND hwnd = GetForegroundWindow();
  if (hwnd) {
    ShowWindow(hwnd, SW_HIDE);
  }
#endif

  std::filesystem::path logDir;
  bool logDirCreated = false;
  std::string errorMessage;

#ifdef _WIN32
  wchar_t *docDir = nullptr;
  if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE,
                                     nullptr, &docDir))) {
    logDir = std::filesystem::path(docDir) / "lceda-pro-sim";
    logDirCreated = std::filesystem::create_directories(logDir);
    CoTaskMemFree(docDir);
  }
#else
  if (const char *home = getenv("HOME")) {
    logDir = std::filesystem::path(home) / "Documents/lceda-pro-sim";
    logDirCreated = std::filesystem::create_directories(logDir);
  }
#endif
  // 获取当前可执行文件路径
  std::filesystem::path exePath = std::filesystem::canonical(argv[0]);
  // 切换当前工作目录到可执行文件所在目录
  std::filesystem::current_path(exePath.parent_path());

  std::filesystem::path logFile =
      logDir.empty() ? "localLog_fallback.txt" : logDir / "localLog.txt";
  std::ofstream outFile(logFile, std::ios::out | std::ios::trunc);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open log file: " << logFile << std::endl;
    return 1;
  }
  std::streambuf *origCoutBuf = std::cout.rdbuf();
  std::streambuf *origCerrBuf = std::cerr.rdbuf();
  std::cout.rdbuf(outFile.rdbuf());
  std::cerr.rdbuf(outFile.rdbuf());
  // 日志初始化错误处理
  if (!errorMessage.empty()) {
    std::cerr << "[WARNING] " << errorMessage << std::endl;
  }
#endif

  // 启动HTTP服务器线程
  std::thread http_thread(run_http_server);
  http_thread.detach();

  int port = 51115;
  if (port == -1) {
    std::cerr << "ERROR:PORT_UNAVAILABLE" << std::endl;
#ifdef DISPLAY
    std::cout.rdbuf(origCoutBuf);
    std::cerr.rdbuf(origCerrBuf);
#endif
    return 1;
  }
  std::cout << "-----LC_PRO_SIM:1.12.3-----\nSTATE:READY\nPORT:" << port
            << std::endl;
  server echo_server;
#ifdef DISPLAY
  // 确保程序退出前恢复cout和cerr缓冲区
  struct StreamRestorer {
    std::streambuf *orig_cout;
    std::streambuf *orig_cerr;
    ~StreamRestorer() {
      std::cout.rdbuf(orig_cout);
      std::cerr.rdbuf(orig_cerr);
    }
  } restorer{origCoutBuf, origCerrBuf};
#endif
  try {
    // 设置日志设置
    echo_server.set_access_channels(websocketpp::log::alevel::none);
    echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
    // 初始化 ASIO
    echo_server.init_asio();
    echo_server.set_reuse_addr(true);
    // 注册我们的消息处理函数
    echo_server.set_message_handler(bind(&on_message, &echo_server, _1, _2));
    // 监听端口 9002s
    echo_server.listen(port);
    echo_server.set_close_handler(
        bind(&on_close, &echo_server, _1)); // 关闭调用这个函数
    // 开始服务器接受循环
    echo_server.start_accept();
    // 开始 ASIO io_service 运行循环
    echo_server.run();
  } catch (websocketpp::exception const &e) {
    std::cout << "WebSocket exception: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown exception" << std::endl;
  }
}
