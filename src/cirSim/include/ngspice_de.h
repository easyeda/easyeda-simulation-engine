/***************************************************************************
 *   Copyright (C) 2025 by easyEDA                                         *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU Affero General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Affero General Public      *
 *   License along with this program. If not, see                          *
 *   <http://www.gnu.org/licenses/>.                                       *
 ***************************************************************************/

#pragma once


#include "pch.h"
#include <stddef.h>

#ifndef XSPICE
#define XSPICE 1
#endif
//还有需要优化的地方，由于不清楚ngspice引擎是否可以多开，因此没有把ngspcie的方法单独进行实例化
//这个还需后续优化。

enum NGANALYSISTYPE{
	NONE = 0,
    AC,
    DC,
    TR,
	OP,
};

enum DATATYPE{
	VOLT = 0,
	PHASE,
	GAIN,
	CURRENT,
};

enum SCOPETYPE{
	LMITATE = 0,
	DIGITAL
};

#define M_PI 3.14159265358979323846
struct ctl_mes
{
	std::string flag;
	std::string ifm;
};
struct DigitalNodeMes{
	std::string nodetype;
	double logicLevel;
	double logicVolt;
	double time;
	bool valid = true;
};

struct Scope
{
	std::string id;
	int scopeType;
	double hightLevel;
	double lowLevel;
};

struct SimData
{
	Scope scopeMes;
	std::vector<std::pair<double, double>> voltData;
	std::vector<std::pair<double, double>> gainData;
	std::vector<std::pair<double, double>> phaseData;
	std::vector<std::pair<double, double>> currentData;
	std::vector<std::pair<double, double>> digitalData;
};




struct NodeMes{
	std::string id;
	double value;
};
struct CurrentData{
	int count;
	std::string Name;
	std::vector<NodeMes> nodes;
};




struct vecvaluesall;
typedef vecvaluesall* pvecvaluesall;
class ngspice_de
{
public:

	static std::shared_mutex scop_mtx;                         // 锁，控制电路结果竞争访问
	std::atomic<bool> stopAllThreads;                          // 类的成员变量
	static std::vector<std::string> scopeName;                 // 示波器名称
	static NGANALYSISTYPE currentAnalysisType;                 // 分析类型
	static std::unordered_map<int,std::string> node_types;     //节点类型，用于区分数字与模拟
	static std::unordered_map<int,std::string> node_name;      //数字节点名称

	ngspice_de(std::string circuit);
	~ngspice_de();
	bool ngSpiceInt(char* myCircuit[]);               	//初始化
	char** ngStrProcess(std::string mes);               //电路处理
	void printCircuitArray(char** circuitArray);        //终端显示电路信息

	void deleteNgStrProcess(char** circuitArray);
	void ngSpiceStaticRunInt();                         //静态运行初始化


    //全局状态
	static std::string getGlobalStatus() 
	{ 
		std::lock_guard<std::shared_mutex> run_status(status);
		return globalStatus; 
	};
	static void setGlobalStatus(std::string state) 
	{
		 std::lock_guard<std::shared_mutex> lock(status); 
		globalStatus = state; // +1 为终止符
	};

	//错误信息写入与获取
	static std::vector<std::string> getErrorMessage() 
	{ 
		std::lock_guard<std::shared_mutex> run_status(outMesLock);
		return circuitError; 
	};
	static void setErrorMessage(std::string state) 
	{
		std::lock_guard<std::shared_mutex> lock(outMesLock);
		circuitError.push_back(state); 
	};

	//警告信息写入与获取
	static std::vector<std::string> getWarningMessage() 
	{ 
		std::lock_guard<std::shared_mutex> run_status(outMesLock);
		return circuitWarning; 
	};
	static void setWarningMessage(std::string state) 
	{
		std::lock_guard<std::shared_mutex> lock(outMesLock);
		circuitWarning.push_back(state); 
	};

	//添加示波器
	static void addScope( Scope scope);
	//删除示波器
	static void deleteScope(std::string v);
	//设置示波器信息
	static void setScopeData(double T, std::vector<std::pair<std::string,double>> V,DATATYPE type);
	static void setScopeData(DigitalNodeMes data,std::string name);
	static std::vector<std::pair<double, double>>& getTargetData(DATATYPE type,int location);
	//获取示波器信息
	static std::vector<std::string> getScopeData();
	//初始化示波器名称
	static void initScopeName();
;
	//获取静态示波器信息
	static std::vector<SimData> getCompleteScopeDatas()
	{
		std::lock_guard<std::shared_mutex> lk(ngspice_de::scop_mtx);
		return completeDatas;
	};

	void setSimType(const std::string ngtypestring,bool& confirmAnalysis);
	void setCurrentMes(std::string ngCurrentLine);
	double getUseDataAsDouble(std::string mes_node);
	static void handlingAcAnalysis(pvecvaluesall values);
	static void handlingTrOrOpAnalysis(pvecvaluesall values);
	static void handlingDCAnalysis(pvecvaluesall values);

	static double toLogicLevel(double voltage, double highLevel, double lowLevel);

private:
	ngspice_de* my_self;

	ctl_mes mes;                                     //控制信息
	static std::vector<SimData> completeDatas;       //存储的数值

	static std::vector<std::string> circuitError;    //电路计算出现的错误
	static std::vector<std::string> circuitWarning;  //电路计算出现的警告

	static int counter;                              //向量计数
	static std::vector<CurrentData> currentList;     // 电流引脚
	static double startValue;

	static std::unordered_map<std::string,bool> overlap_node_name;
	static bool initFlag;



	static std::string globalStatus;
	char** circuitArray;                          //电路

	static std::shared_mutex status;              //ngspcie引擎状态锁
	static std::shared_mutex outMesLock;           //错误信息控制锁
};


