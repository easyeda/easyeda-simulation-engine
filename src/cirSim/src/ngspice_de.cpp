/***************************************************************************
 *   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
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


#include <ngspice_de.h>
#include <sharedspice.h>
#include <utils.h>
#include <regex>

static int MySendChar(char* output, int i, void* userData);
static int MySendStat(char* state, int i, void* userData);
static int MyControlledExit(int exitStatus, NG_BOOL immediateUnload, NG_BOOL exitDueToQuit, int ident, void* userData);
static int MySendData(pvecvaluesall values, int numVecs, int ident, void* userData);
static int MySendInitData(pvecinfoall info, int ident, void* userData);
static int MyBGThreadRunning(NG_BOOL running, int ident, void* userData);
static double toLogicLevel(double voltage, double highLevel, double lowLevel);
#ifdef XSPICE
static int MySendEvtData(int nodeIndex, double time, double realValue, char* strValue, void* binData, int binSize, int mode, int ident, void* userData);
static int MySendInitEvtData(int nodeIndex, int maxNodeIndex, char* nodeName, char* nodeType, int ident, void* userData);
#endif

std::string ngspice_de::globalStatus;
int ngspice_de::counter = 0;
double prevPhase = 0;
double ngspice_de::startValue;

std::vector<SimData> ngspice_de::completeDatas;
std::shared_mutex  ngspice_de::scop_mtx;
std::shared_mutex  ngspice_de::status;
std::shared_mutex  ngspice_de::outMesLock;

std::vector<std::string> ngspice_de::scopeName;
std::vector<CurrentData> ngspice_de::currentList;
std::vector<std::string> ngspice_de::circuitError;
std::vector<std::string> ngspice_de::circuitWarning;

NGANALYSISTYPE ngspice_de::currentAnalysisType;

std::unordered_map<int,std::string> ngspice_de::node_types;
std::unordered_map<int,std::string> ngspice_de::node_name;   //数字节点名称
std::unordered_map<std::string,bool> ngspice_de::overlap_node_name;
bool ngspice_de::initFlag;


ngspice_de::ngspice_de(std::string circuit)
{
	circuitArray = nullptr;
	mes.flag = " ";
	mes.ifm  = " ";
	startValue = 0;
	initFlag = false;
	ngStrProcess(circuit);
	ngSpiceInt(circuitArray);
}

ngspice_de::~ngspice_de()
{
	deleteNgStrProcess(circuitArray);
}


bool ngspice_de::ngSpiceInt(char* myCircuit[])
{
	// 初始化 ngSpice
	int id = 0;
	int result = ngSpice_Init(MySendChar, MySendStat, MyControlledExit, MySendData, MySendInitData, MyBGThreadRunning, NULL);
	#ifdef XSPICE
    if (result == 0) {
        ngSpice_Init_Evt(MySendEvtData, MySendInitEvtData, NULL); // 注册 XSPICE 回调
    }
	#endif
	if (result == 0)
	{
		std::cout << "ngSpice init success." << std::endl;
	}
	else
	{
		std::cerr << "ngSpice init faild." << std::endl;
		return false;
	}
	std:: cout << *myCircuit << std::endl;
	result = ngSpice_Circ(myCircuit);
	if (result == 0)
	{
		std::cout << "circuit set to ngSpice is success." << std::endl;
	}
	else
	{
		std::cerr << "circuit set to ngSpice is faild" << std::endl;
		return false;
	}
	// 检查 ngSpice 是否正在运行
	NG_BOOL isRunning = ngSpice_running();
	if (isRunning)
	{
		std::cout << "ngSpice is running." << std::endl;
	}
	else
	{
		std::cout << "ngSpice is stoping ." << std::endl;
	}

	return true;
}
void ngspice_de::ngSpiceStaticRunInt()
{
	initScopeName();
	ngSpice_Command((char*)"run");
}

//新增检测节点
void ngspice_de::addScope(Scope scopeMes)
{
	std::shared_lock<std::shared_mutex> lk(ngspice_de::scop_mtx);
	int num = 0;
	for(num ; num < completeDatas.size();num++)
	{ 
		if( completeDatas[num].scopeMes.id == scopeMes.id &&
			completeDatas[num].scopeMes.scopeType == scopeMes.scopeType &&
			completeDatas[num].scopeMes.hightLevel == scopeMes.hightLevel &&
			completeDatas[num].scopeMes.lowLevel == scopeMes.lowLevel 
		) break;
	}

	if(num ==  completeDatas.size())
	{
		SimData completeData;
		//示波器信息
		completeData.scopeMes.scopeType = scopeMes.scopeType;
		completeData.scopeMes.id = scopeMes.id;
		completeData.scopeMes.hightLevel = scopeMes.hightLevel;
		completeData.scopeMes.lowLevel = scopeMes.lowLevel;

		//数据初始化
		if(currentAnalysisType == TR || currentAnalysisType == DC || currentAnalysisType == OP){
			completeData.voltData.reserve(100000);
			completeData.currentData.reserve(100000);
		}else if(currentAnalysisType == AC){
			completeData.gainData.reserve(100000);
			completeData.phaseData.reserve(100000);
		}
		completeData.digitalData.reserve(100000);
		completeDatas.push_back(completeData);
	}
}

void ngspice_de::deleteScope(std::string v)
{
	std::lock_guard<std::shared_mutex>  lk(ngspice_de::scop_mtx);
	completeDatas.erase(std::remove_if(completeDatas.begin(), completeDatas.end(),
							[&v](const SimData& item) {
								return item.scopeMes.id == v;
							}),
	completeDatas.end());
}

void ngspice_de::setScopeData( double T, std::vector<std::pair<std::string,double>> V,DATATYPE type)
{
	std::lock_guard<std::shared_mutex>  lk(ngspice_de::scop_mtx);
	int i = 0;
	double level = 0.5;
	for(i;i < completeDatas.size();i++){
		for( const auto& data: V){
			if(completeDatas[i].scopeMes.id == data.first){
				auto& targetData =getTargetData(type,i);
				if(completeDatas[i].scopeMes.scopeType == LMITATE ){
					if(!initFlag){
						targetData.clear();
						initFlag = true;
					}
					targetData.push_back(std::make_pair(T,data.second));
				} else if(completeDatas[i].scopeMes.scopeType == DIGITAL ){
					if(overlap_node_name[completeDatas[i].scopeMes.id]) continue;
					level = toLogicLevel(data.second,completeDatas[i].scopeMes.hightLevel,completeDatas[i].scopeMes.lowLevel);
					completeDatas[i].digitalData.push_back(std::make_pair(T,level));
				}
			}
		}

	}
	//std::cout << " Times is " << trendsDatas.back().first << std::endl;
	//std::cout << " V is " << trendsDatas.back().second << std::endl;
}

void ngspice_de::setScopeData(DigitalNodeMes data, std::string name)
{
	if(data.time < startValue) return;
	std::shared_lock<std::shared_mutex> lk(ngspice_de::scop_mtx);
	double level = 0.5;
	int i = 0;
	std::string vlotName = "V(" + name + ")";
	for(i;i < completeDatas.size();i++){
		if(completeDatas[i].scopeMes.id == vlotName){
			if(completeDatas[i].scopeMes.scopeType == DIGITAL){
				level = toLogicLevel(data.logicVolt,completeDatas[i].scopeMes.hightLevel,completeDatas[i].scopeMes.lowLevel);
				completeDatas[i].digitalData.push_back(std::make_pair(data.time,level));
			} else if(completeDatas[i].scopeMes.scopeType == LMITATE){
				if(overlap_node_name[completeDatas[i].scopeMes.id]) continue;
				completeDatas[i].voltData.push_back(std::make_pair(data.time,data.logicVolt));
			}
		}
	}
    
}

std::vector<std::pair<double, double>> &ngspice_de::getTargetData(DATATYPE type,int location)
{
        switch (type) {
        case VOLT:
            return completeDatas[location].voltData;
        case GAIN:
            return completeDatas[location].gainData;
        case PHASE:
            return completeDatas[location].phaseData;
		case CURRENT:
			 return completeDatas[location].currentData;
        default:
            throw std::invalid_argument("Invalid DATATYPE");
    }
}

std::vector<std::string> ngspice_de::getScopeData()
{
	std::lock_guard<std::shared_mutex> lk(ngspice_de::scop_mtx);
	std::vector<std::string> names;
	for( auto nodeName:completeDatas)
	{
		names.push_back(nodeName.scopeMes.id);
	}
	return names;
}

void ngspice_de:: initScopeName()
{
	scopeName = getScopeData();
}

//----------------------------------------------------------------------------------------------------------------//
//电路图处理

char** ngspice_de::ngStrProcess(std::string mes)
{
    // 分析命令正则匹配和处理
    const char* pattern = R"((\.(?:ac|tran|dc|noise|disto|pz|sens|hb|fft|four|mc|op|tf|sp|step|meas|plot|print)\b[^\n]*?)([+-]?\d*\.?\d+(?:e[+-]?\d+)?)([M])(\b[^\n]*))";
    std::regex re(pattern);
    std::string processed = std::regex_replace(mes, re, "$1$2E6$4");
    // 分行处理
    std::vector<std::string> lines;
    std::istringstream iss(processed);
    std::string tempLine;
    bool confirmAnalysis = false;
    while (std::getline(iss, tempLine)) {
        if (tempLine.empty()) continue;
        setSimType(tempLine, confirmAnalysis);
        setCurrentMes(tempLine);
        lines.push_back(tempLine);
    }
	// 添加 ".end" 标记
	// lines.push_back(".end");

	// 为行指针数组分配内存
	circuitArray = new char* [lines.size() + 1]; // 加1是为了NULL终止符

	// 填充指针数组
	for (size_t i = 0; i < lines.size(); i++)
	{
		circuitArray[i] = new char[lines[i].length() + 1];
		strcpy(circuitArray[i] ,lines[i].c_str());
		//std::cout << "circuitArray[" << i << "]: " << circuitArray[i] << std::endl;
	}
	// 最后一个指针设置为NULL
	circuitArray[lines.size()] = nullptr;
	printCircuitArray(circuitArray);
	return circuitArray;
}

//打印电路
void ngspice_de::printCircuitArray(char** circuitArray) {
	if (circuitArray == nullptr) {
		std::cout << "Circuit array is null." << std::endl;
		return;
	}
	std::cout << "Circuit Array:" << std::endl;
	for (int i = 0; circuitArray[i] != nullptr; ++i) {
		std::cout << circuitArray[i] << std::endl;
	}
}

void ngspice_de::deleteNgStrProcess(char** circuitArray)
{
	// 遍历数组，删除每个char*元素
	for (int i = 0; circuitArray[i] != nullptr; ++i)
	{
		delete[] circuitArray[i];
	}

	// 删除char**数组本身
	delete[] circuitArray;
}

void ngspice_de::setSimType(std::string ngtypestring,bool& confirmAnalysis)
{
	std::transform(ngtypestring.begin(),ngtypestring.end(),ngtypestring.begin(),::tolower);
	if((ngtypestring.find(".ac") != std::string::npos) && !confirmAnalysis){
		std::vector<std::string> mes = split(ngtypestring,' ');
		if(!mes[3].empty()){
			startValue = getValueOfNum(mes[3]);
		}
		ngspice_de::currentAnalysisType = AC;
		confirmAnalysis = true;
	}else if((ngtypestring.find(".dc") != std::string::npos) && !confirmAnalysis){
		ngspice_de::currentAnalysisType= DC;
		confirmAnalysis = true;
	}else if((ngtypestring.find(".tr") != std::string::npos) && !confirmAnalysis){
		std::vector<std::string> mes = split(ngtypestring,' ');
		if(!mes[3].empty()){
			startValue = getValueOfNum(mes[3]);
		}
		ngspice_de::currentAnalysisType = TR;
		confirmAnalysis = true;
	}else if(!confirmAnalysis){
		startValue = 0.0;
		ngspice_de::currentAnalysisType = NONE;
	}
}

void ngspice_de::setCurrentMes(std::string ngCurrentLine)
{
	if( ngCurrentLine.find("XAM") != std::string::npos){
		std::string mes1;
		std::string mes2;
		std::vector<std::string> mes =  split(ngCurrentLine,' ');
		if(getUseDataAsDouble(mes[1]))
        {
           mes1 = "V(" + mes[1] + ")";    
        }
		if(getUseDataAsDouble(mes[2]))
        {
           mes2 = "V(" + mes[2] + ")";    
        }
		CurrentData data = {
			0,
			mes[0],
			{
				{mes1,0},
				{mes2,0}
			}
		};
		currentList.push_back(data);
	}
}

double ngspice_de::getUseDataAsDouble(std::string mes_node) {    
	try {
		return std::stod(mes_node);
	} catch (const std::invalid_argument& e) {
	// 如果不是有效的数字，返回一个特殊值（例如 NaN）或抛出异常
		return 0;
	} catch (const std::out_of_range& e) {
		return 0;
	}
}

void ngspice_de::handlingAcAnalysis(pvecvaluesall values)
{
	bool FreValueSet = false;
	double FreValue = 0.0;

	std::string nodeName;
	std::vector<std::pair<std::string,double>> current_nodeGainMes;
	std::vector<std::pair<std::string,double>> current_nodePhaseMes;

	int sampleRate = 1;

	for (int i = 0; i < values->veccount; ++i) 
	{
		pvecvalues vecData = values->vecsa[i];
		if (vecData == NULL) {
			continue;
		}
		// Check for the 'frequence' vector and store its value
		if (vecData->name != nullptr && 
			std::strcmp(vecData->name, "frequency") == 0 && 
			vecData->creal >= startValue
		){
			FreValue = vecData->creal;
			FreValueSet = true;
		}

		// Check for the 'V' vector and store its value
		for(std::string voltName:ngspice_de::scopeName)
		{
			if (vecData->name != nullptr && std::string(vecData->name) == voltName) {
				if( vecData->is_complex){
					// double gainValue = 20 * log10(std::sqrt(vecData->creal * vecData->creal + vecData->cimag * vecData->cimag));
					double gainValue = 20 * log10(std::hypot(vecData->creal,vecData->cimag));
            		double phaseValue = std::atan2(vecData->cimag,vecData->creal)*(180.0/M_PI);
					prevPhase = phaseValue;
					nodeName = vecData->name;
					current_nodeGainMes.push_back( std::make_pair(nodeName,gainValue));
					current_nodePhaseMes.push_back( std::make_pair(nodeName,phaseValue));
				}
			}
		}


		if (FreValueSet) {
			ngspice_de::setScopeData(FreValue, current_nodeGainMes,GAIN);
			ngspice_de::setScopeData(FreValue, current_nodePhaseMes,PHASE);
			FreValue = false; 
		}
	}
}

void ngspice_de::handlingTrOrOpAnalysis(pvecvaluesall values){
	int currentCount = 0;
	bool timeValueSet = false;
	bool currentValueSet = false;
	double timeValue = 0.0;
	double voltageValue = 0.0;
	std::string voltageName;
	std::vector<std::pair<std::string,double>> current_voltMes;
	std::vector<std::pair<std::string,double>> current_currentMes;
	int sampleRate = 1;
	for (int i = 0; i < values->veccount; ++i) 
	{
		pvecvalues vecData = values->vecsa[i];
		auto it = std::find_if(
			node_name.begin(),
			node_name.end(),
			[&](const auto& pair){
				std::string vlotName = "V(" + pair.second + ")";
				return  vlotName == vecData->name;
		});
		if(it != node_name.end()){
			overlap_node_name[vecData->name] = true;
		}
		if (vecData == NULL) {
			continue;
		}
		// Check for the 'time' vector and store its value
		if (vecData->name != nullptr && 
			std::strcmp(vecData->name, "time") == 0 && 
			vecData->creal >= startValue
		){
			timeValue = vecData->creal;
			timeValueSet = true;
		}

		// Check for the 'V' vector and store its value
		for(std::string voltName:ngspice_de::scopeName)
		{
			if (vecData->name != nullptr && std::string(vecData->name) == voltName) {
				voltageValue = vecData->creal;
				voltageName = vecData->name;
				current_voltMes.push_back( std::make_pair(voltageName,voltageValue));
			}
		}

		for(auto& currentMes:currentList){
			for(auto& nodeMes:currentMes.nodes){
				if(vecData->name != nullptr && std::string(vecData->name) == nodeMes.id){
					nodeMes.value = vecData->creal;
					currentMes.count++;
				}
			}
			if(currentMes.count == 2){
				//这里涉及到一个电流正负的问题，与网表中左右节点有关
				double currentResult = (currentMes.nodes[0].value - currentMes.nodes[1].value)/1e-3;
				currentMes.Name[0] = std::tolower(currentMes.Name[0]);
				current_currentMes.push_back(std::make_pair(currentMes.Name,currentResult));
				currentMes.count = 0;
				currentCount++;
			}
		}
		if(currentCount = currentList.size() || currentList.empty())
			currentValueSet = true;

		if (timeValueSet && currentValueSet) {
			if(!current_voltMes.empty()){
				ngspice_de::setScopeData(timeValue, current_voltMes,VOLT);
			}
			if(!current_currentMes.empty()){
				ngspice_de::setScopeData(timeValue, current_currentMes,CURRENT);
			}
			timeValueSet = false; 
			currentValueSet = false;
			break;
		}
	}
}

void ngspice_de::handlingDCAnalysis(pvecvaluesall values)
{
	bool sweepValueSet = false;
	double sweepValue = 0.0;
	double voltageValue = 0.0;
	std::string voltageName;
	std::vector<std::pair<std::string,double>> current_voltMes;
	int sampleRate = 1;
	for (int i = 0; i < values->veccount; ++i) 
	{
		pvecvalues vecData = values->vecsa[i];
		if (vecData == NULL) {
			continue;
		}
		// Check for the 'time' vector and store its value
		if (vecData->name != nullptr && std::strcmp(vecData->name, "v-sweep") == 0) {
			sweepValue = vecData->creal;
			sweepValueSet = true;
		}

		// Check for the 'V' vector and store its value
		for(std::string voltName:ngspice_de::scopeName)
		{
			if (vecData->name != nullptr && std::string(vecData->name) == voltName) {
				voltageValue = vecData->creal;
				voltageName = vecData->name;
				current_voltMes.push_back( std::make_pair(voltageName,voltageValue));
			}
		}

		if (sweepValueSet) {
			ngspice_de::setScopeData(sweepValue, current_voltMes,VOLT);
			sweepValueSet = false; 
		}
	}
}

double ngspice_de::toLogicLevel(double voltage, double highLevel, double lowLevel)
{
    if(voltage >= highLevel) return 1.0;
    if(voltage <= lowLevel) return 0.0;
    return 0.5; 
}

//----------------------------------------------------------------------------------------------------------------------------------------//

static int MySendChar(char* output, int i, void* userData) {
    std::cout << "Send Char Message: " << output << std::endl;
    std::string outputStr(output);
    if (outputStr.empty()) {
        return -1;
    }

    ngspice_de::setGlobalStatus(output);
    std::string lowerOutput = toLower(outputStr);
    const std::string stderrMarker = "stderr";
    size_t errPos = lowerOutput.find(stderrMarker);

	while (errPos != std::string::npos) {
        size_t msgStart = lowerOutput.find_first_not_of(" :\t", errPos + stderrMarker.length());
        
        if (msgStart == std::string::npos) break;
        
        size_t msgEnd = lowerOutput.find('\n', msgStart);
        if (msgEnd == std::string::npos) msgEnd = outputStr.length();
        
        std::string rawMessage = outputStr.substr(msgStart, msgEnd - msgStart);
        std::string lowerMsg = toLower(rawMessage);
        
        if (lowerMsg.find("fatal error") == 0) {
            std::string cleanMsg = rawMessage.substr(12); 
            cleanMsg.erase(0, cleanMsg.find_first_not_of(" \t")); 
            ngspice_de::setErrorMessage(cleanMsg);
        } 
        else if (lowerMsg.find("error:") == 0) {
            std::string cleanMsg = rawMessage.substr(6); 
            cleanMsg.erase(0, cleanMsg.find_first_not_of(" \t"));
            ngspice_de::setErrorMessage(cleanMsg);
        }
        else if (lowerMsg.find("warning:") == 0) {
            std::string cleanMsg = rawMessage.substr(8); 
            cleanMsg.erase(0, cleanMsg.find_first_not_of(" \t"));
            ngspice_de::setWarningMessage(cleanMsg);
        }
        else if (lowerMsg.find("warning -") == 0) {
            std::string cleanMsg = rawMessage.substr(9); 
            cleanMsg.erase(0, cleanMsg.find_first_not_of(" \t"));
            ngspice_de::setWarningMessage(cleanMsg);
        }
        
        // 7. 查找下一个错误
        errPos = lowerOutput.find(stderrMarker, msgEnd);
    }

    return 0;  // 成功
}




static int MySendStat(char* state, int i, void* userData)
{

	std::cout << "Send Stat Message:" << state << std::endl;

	return 0; // 返回成功
}

static int MyControlledExit(int exitStatus, NG_BOOL immediateUnload, NG_BOOL exitDueToQuit, int ident, void* userData)
{
	// 处理退出前的清理工作
	if (exitDueToQuit)
	{
		std::cout << "ngSpice is exiting upon user request." << std::endl;
	}
	else
	{
		std::cerr << "ngSpice is exiting due to an internal error." << std::endl;
	}

	// 这里可以添加清理代码，比如关闭文件、释放内存等

	if (immediateUnload)
	{
		// 如果需要立即卸载 DLL，执行卸载操作
	}

	// 返回退出状态
	return exitStatus;
}




static int MySendInitData(pvecinfoall info, int ident, void* userData)
{
	// 确保 info 指针有效
	if (info == NULL)
	{
		return 1; // 返回错误代码
	}

	// 打印绘图信息
	std::cout << "Plot name: " << info->name << std::endl;
	std::cout << "Plot title: " << info->title << std::endl;
	std::cout << "Plot date: " << info->date << std::endl;
	std::cout << "Plot type: " << info->type << std::endl;
	std::cout << "Vector count: " << info->veccount << std::endl;

	// 遍历所有向量信息
	for (int i = 0; i < info->veccount; ++i)
	{
		pvecinfo vecData = info->vecs[i]; // 使用指针访问 vecinfo 结构体
		if (vecData == NULL)
		{
			continue; // 如果指针无效，跳过当前向量
		}

		// 打印向量信息
		// 打印向量信息
		std::cout << "Vector number: " << vecData->number << std::endl;
		std::cout << "Vector name: " << vecData->vecname << std::endl;
		std::cout << "Is real data: " << (vecData->is_real ? "Yes" : "No") << std::endl;

		// 如果需要，可以转换 pdvec 和 pdvecscale 指针，以访问实际的向量数据
		// dvec *actualVector = (dvec *)vecData->pdvec;
		// dvec *scaleVector = (dvec *)vecData->pdvecscale;
		// ... 这里可以添加更多关于 actualVector 和 scaleVector 的处理 ...
	}

	return 0; // 返回成功
}


int MyBGThreadRunning(NG_BOOL running, int ident, void* userData)
{
	std::ofstream logfile("",std::ios_base::app);

	if (running)
	{
		std::cout << "ngSpice background thread (ID: " << ident << ") is running." << std::endl;
		// 。。。。。。
	}
	else {
		std::cout << "ngSpice background thread (ID: " << ident << ") has stopped." << std::endl;
		// 。。。。。。。
	}


	// ...

	return 0; // 返回成功
}


static int MySendData(pvecvaluesall values, int numVecs, int ident, void* userData) {
	// ... (other code remains unchanged)
	// std::cout << "1 ci"<< std::endl;
	switch (ngspice_de::currentAnalysisType)
	{
		case AC:
			ngspice_de::handlingAcAnalysis(values);
		break;
		case OP:
		case TR:
			ngspice_de::handlingTrOrOpAnalysis(values);
		break;
		case DC:
			ngspice_de::handlingDCAnalysis(values);
		break;
	}
	return 0; // Return success
}


#ifdef XSPICE
static int MySendInitEvtData(int nodeIndex, int maxNodeIndex, char* nodeName, char* nodeType, int ident, void* userData) {
	ngspice_de::node_types[nodeIndex] = nodeType; 
	ngspice_de::node_name[nodeIndex] = nodeName;
    std::cout << "[XSPICE] Event Node Initialized: " 
              << "Index=" << nodeIndex << "/" << maxNodeIndex 
              << ", Name='" << (nodeName ? nodeName : "NULL") 
              << "', Type='" << (nodeType ? nodeType : "NULL") << "'" 
              << std::endl;
    
    // 可选：将数字节点名称存入自定义容器（如全局列表）
    // if (nodeName) {
    //     ngspice_de::addScope(nodeName); // 假设你已有 addScope 方法
    // }
    
    return 0; // 返回成功
}
static int MySendEvtData(int nodeIndex, double time, double realValue, char* strValue, void* binData, int binSize, int mode, int ident, void* userData) {
	 std::cout << "[XSPICE] Event Data: "
              << "Time=" << time << "s, NodeIndex=" << nodeIndex 
              << ", State='" << strValue << "', Voltage=" << realValue 
              << std::endl;
	
	double digitalVlot = 0.0;
	double digitaLevel = 0; 
	// 处理数字信号（使用 strValue）
	if (ngspice_de::node_types[nodeIndex] == "d") {

		if (strValue) {
			if (strstr(strValue, "1s")) digitaLevel = 1.0;  // "1s", "1r" 等表示高电平
			else if (strstr(strValue, "0s")) digitaLevel = 0.0;
			else if(strstr(strValue, "Us")) digitaLevel = 0.5; // 未知/亚稳态（如 "U"）
		}
		digitalVlot = realValue;
    } else {
        // 处理模拟信号（使用 realValue）
		digitalVlot = realValue;
    }
	DigitalNodeMes digitalData = {
		ngspice_de::node_types[nodeIndex],
		digitaLevel,
		digitalVlot,
		time
	};
    ngspice_de::setScopeData(digitalData, ngspice_de::node_name[nodeIndex]); // 复用 VOLT 类型或新增 DIGITAL
    return 0; // 返回成功
}
#endif
