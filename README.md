# LCEDA 电路仿真工具

基于 NGspice 和 Simulide 的双引擎电路仿真工具，包含服务器模块和电路仿真模块。

## 📜 开源协议

本项目采用双许可证模式：

1. **原始 Simulide 代码**  
   - 版权归 Santiago González 所有
   - 采用 GNU Affero General Public License v3.0 (AGPLv3) 协议
   - 参见 [copyright.txt](copyright.txt)

2. **修改和新增代码**  
   - 版权归 EasyEDA & JLC Technology Group 所有
   - 采用 GNU General Public License v3.0 (GPLv3) 协议

### 使用要求
- 任何修改后的版本必须同样采用 AGPLv3 + GPLv3 双协议开源
- 如果作为网络服务提供，必须提供源代码（根据 AGPLv3 要求）
- 必须保留所有原始版权声明和许可声明

完整协议内容请查看 [LICENSE](LICENSE) 文件。

## ⚙️ 架构概述

### 双仿真引擎架构
1. **NGspice 引擎**
   - 提供精确的 SPICE 级电路仿真
   - 通过 sharedspice.h 接口集成
   - 使用 BSD 3-Clause 许可证

2. **Simulide 改造引擎**
   - 基于 Simulide 的事件驱动仿真核心
   - 多线程优化
   - 支持实时交互
   - 原始部分使用 AGPLv3，修改部分使用 GPLv3

## 📦 构建依赖

| 依赖项 | 版本 | 许可证 |
|--------|------|--------|
| CMake | 3.5+ | [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause) |
| C++ 编译器 | gcc (Rev5, Built by MSYS2 project)15.1.0 | - |

gcc工具链需在[MSYS2](https://www.msys2.org/)下载

## 🛠️ 构建步骤

### Windows 系统
```bash
# 创建构建目录
mkdir build
cd build

# 生成构建系统
cmake .. -G "MinGW Makefiles"

# 编译项目
cmake --build . --parallel 4

```
编译完成后，可执行文件将生成在 `bin/` 目录下。

## 📂 项目结构

```text
LCEDA_SIM/
├── src/
│   ├── cirSim/        # 电路仿真模块
│   │   ├── include/   # 头文件
│   │   └── src/       # 源代码
│   ├── server/        # 服务器模块
│   │   ├── include/   # 头文件
│   │   └── src/       # 源代码
│   └── public/        # 公共头文件
├── share/             # 共享资源
│   └── ngspice/       # ngspice 相关脚本
├── config             # 程序运行必备的配置文件（由CMake自动复制到运行目录）
├── external           # 第三方库（含源码或预编译文件）
├── image              # 程序图标
├── CMakeLists.txt     # 主构建配置
├── LICENSE            # 许可证文件
└── README.md          # 项目文档
```

## 🚀 使用说明

本LCEDA 电路仿真工具为后台服务程序，运行后无图形界面。
1. 启动仿真服务：
   ```bash
   ./bin/lceda-pro-sim-server
   ```
   （当通过LCEDA主程序启动时，主程序会自动呼起该服务；若直接运行可执行文件，服务将在后台启动）

2. 通过 WebSocket 客户端连接服务：
   - 服务启动后默认监听端口：`51115`
   - 支持 WebSocket 协议连接


## 🤝 贡献指南

欢迎提交 Pull Request。贡献者需同意：
1. 修改 Santiago González 的原始代码将采用 AGPLv3 协议授权
2. 新增代码将采用 GPLv3 协议授权
3. 所有贡献需包含适当的单元测试

## 🙏 致谢

- Simulide 项目 ([simulide.com](https://https://simulide.com/p/))
- ngspice 开发团队 ([ngspice 官方](http://ngspice.sourceforge.net/))
- 所有项目贡献者
