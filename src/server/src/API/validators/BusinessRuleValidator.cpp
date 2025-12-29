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

#include "BusinessRuleValidator.h"
using json = nlohmann::json;

// 参数规则结构体
struct ParamRule {
  bool required = false;
  std::vector<json::value_t> allowedTypes;
};
// 操作规则结构体
struct OperationRule {
  std::map<std::string, ParamRule> paramRules; // 参数验证规则
  bool requireSession = false;                 // 是否需要会话
  std::string permission;                      // 所需权限
};
// 全局验证规则表 - 初始化所有支持的校验规则
const std::map<std::string, OperationRule> VALIDATION_RULES = {
    {"updateAttribute",
     {{{"componentId", {true, {json::value_t::string}}},
       {"attribute", {true, {json::value_t::string}}},
       {"value",
        {true,
         {
             json::value_t::string,
             json::value_t::number_float,
             json::value_t::number_unsigned,
             json::value_t::number_integer,
         }}}},
      true,
      "update_permission"}},
    {"Start_Simulation", {{}, true, "start_permission"}},
    {"CloseServer", {{}, true, "admin_permission"}},

    // ngspice 规则
    {"Add_ProbeNode",
     {{{"ProbeNode", {true, {json::value_t::string}}},
       {"ProbeType",
        {true,
         {json::value_t::number_integer, json::value_t::number_unsigned}}},
       {"LowLevel",
        {true,
         {
             json::value_t::number_float,
             json::value_t::number_unsigned,
             json::value_t::number_integer,
         }}},
       {"HighLevel",
        {true,
         {
             json::value_t::number_float,
             json::value_t::number_unsigned,
             json::value_t::number_integer,
         }}}},
      true,
      ""}},
    {"Start_Ngspice",
     {{{"Netlist", {true, {json::value_t::string}}}}, true, ""}},

    // simulide 规则
    {"Start_Simulide",
     {{{"Netlist", {true, {json::value_t::string}}}}, true, "simulide_start"}},
    {"SetSpeed_Simulide",
     {{{"Speed",
        {true,
         {
             json::value_t::string,
             json::value_t::number_float,
             json::value_t::number_unsigned,
             json::value_t::number_integer,
         }}}},
      true,
      ""}},
    {"Stop_Simulide", {{{}}, true, ""}},
    {"Pause_Simulide", {{{}}, true, ""}},
    {"Resume_Simulide", {{{}}, true, ""}},
    {"Update_Simulide_Attr",
     {{{"updateCirID", {true, {json::value_t::string}}},
       {"attrInput", {true, {json::value_t::string}}},
       {"updateValue",
        {true,
         {
             json::value_t::boolean,
             json::value_t::string,
             json::value_t::number_float,
             json::value_t::number_unsigned,
             json::value_t::number_integer,
         }}}},
      true,
      ""}}};
bool BusinessRuleValidator::validate(const SimMessage &msg, Result *out) {
  // Layer 1: 基本结构校验
  if (msg.operation.empty()) {
    return fail(out, "MISSING_OPERATION", "Operation field is required");
  }

  // Layer 2: 表驱动规则校验
  auto ruleIt = VALIDATION_RULES.find(msg.operation);
  if (ruleIt != VALIDATION_RULES.end()) {
    const auto &rule = ruleIt->second;

    // 1. 会话验证
    // if (rule.requireSession && msg.sessionId.empty()) {
    //     return fail(out, "SESSION_REQUIRED",
    //                "Session ID required for this operation");
    // }

    // if (rule.requireSession &&
    // !SessionManager::isValidSession(msg.sessionId)) {
    //   return fail(out, "INVALID_SESSION", "Session is invalid");
    // }

    // 2. 参数验证
    for (const auto &[param, paramRule] : rule.paramRules) {
      // 必填检查
      if (paramRule.required && !msg.params.contains(param)) {
        return fail(out, "MISSING_PARAMETER", "Required parameter missing",
                    param);
      }

      // 类型检查
      if (msg.params.contains(param) && !paramRule.allowedTypes.empty()) {
        bool validType = false;
        const auto &value = msg.params[param];

        for (const auto &allowedType : paramRule.allowedTypes) {
          if (value.type() == allowedType) {
            validType = true;
            break;
          }
        }

        if (!validType) {
          return fail(out, "INVALID_TYPE", "Invalid data type for parameter",
                      param);
        }
      }
    }

    // 3. 权限验证
    // if (!rule.permission.empty() &&
    //     !UserPermissions::canPerform(msg.sessionId, rule.permission)) {
    //     return fail(out, "PERMISSION_DENIED",
    //                "Lack of permission for this operation");
    // }
  }
  // 未定义的操作支持
  else if (!msg.operation.empty()) {
    return fail(out, "UNSUPPORTED_OPERATION",
                "Unknown or unsupported operation type");
  }

  return true;
}
bool BusinessRuleValidator::fail(Result *out, const std::string &code,
                                 const std::string &msg,
                                 const std::string &field) {
  if (out) {
    out->errorCode = code;
    out->errorMessage = msg;
    out->invalidField = field;
  }
  return false;
}