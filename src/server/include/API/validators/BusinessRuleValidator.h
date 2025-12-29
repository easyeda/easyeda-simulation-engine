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
#include "SimMessage.hpp"  // 包含SimMessage定义
#include <string>
#include <nlohmann/json.hpp> // 包含nlohmann json库
class BusinessRuleValidator {
public:
    struct Result {
        std::string errorCode;      // 错误代码标识
        std::string errorMessage;   // 可读错误信息
        std::string invalidField = "";  // 无效字段名（如果有）
    };
    // 验证消息的业务规则
    // @param msg 要验证的消息对象
    // @param out 可选，用于返回验证失败详情
    // @return true验证通过，false验证失败
    static bool validate(const SimMessage& msg, Result* out = nullptr);
    
private:
    // 验证失败辅助方法
    static bool fail(Result* out, const std::string& code, 
                     const std::string& msg, const std::string& field = "");
};