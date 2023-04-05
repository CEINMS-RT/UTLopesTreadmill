#include <UTLOPESTreadmillTwinCAT.hpp>
#include <iostream>
#include <mapTools.hpp>

const std::vector<std::string>& LOPESTreadmillTwinCAT::getXLDDofName(){
	_varNames = mapToKeys(_xld);
	return _varNames;
}


LOPESTreadmillTwinCAT::LOPESTreadmillTwinCAT() : record_(false), _outDirectory("output"), threadStop_(false)
{

}

LOPESTreadmillTwinCAT::~LOPESTreadmillTwinCAT()
{

}

void LOPESTreadmillTwinCAT::init(const std::string& subjectName, const std::string& executionName, const PluginConfig& config)
{

	varNameVect_.insert({"l_ground_force_vy", 0});
	varNameVect_.insert({"ground_force_vy", 0});

	_conversionMap.insert({"l_ground_force_vy", "synergy_speed_control_gait_detector.Output.GRF_l_Z"});
	_conversionMap.insert({"ground_force_vy", "synergy_speed_control_gait_detector.Output.GRF_r_Z"});


	timeStamp_ = rtb::getTime();

	std::string port = "352"; // Hardcoded for now

	client_ = std::make_shared<tcAdsClient>(std::atoi(port.c_str()));

	for(auto& pair : varNameVect_){
		std::string varName = _conversionMap.at(pair.first);
		pair.second = client_->getVariableHandle(&varName[0], (int) varName.size());
	}

	thread_ = std::make_shared<std::thread>(&LOPESTreadmillTwinCAT::Thread, this);

	_xld.insert({"l_ground_force_vy", 0});
	_xld.insert({"ground_force_vy", 0});
}

void LOPESTreadmillTwinCAT::Thread()
{
	std::map<std::string, double> dataLocal;
	double timeLocal;

	{
		std::lock_guard<std::mutex> lock(mtxXLD_);
		dataLocal = _xld;
	}


	while (!threadStop_)
	{
		timeLocal = rtb::getTime();
		for(auto& pair : dataLocal){
			client_->read(varNameVect_.at(pair.first), &dataLocal.at(pair.first), sizeof(double));
		}
		{
			std::lock_guard<std::mutex> lock(mtxXLD_);
			_xld = dataLocal;
		}
		{
			std::lock_guard<std::mutex> lock(mtxTime_);
			timeStamp_ = timeLocal;
		}
	}
}

const std::map<std::string, double>& LOPESTreadmillTwinCAT::getDataMapXLD()
{
	std::lock_guard<std::mutex> lock(mtxXLD_);
	dataXLDSafe_ = _xld;
	return dataXLDSafe_;
}


void LOPESTreadmillTwinCAT::stop()
{
	threadStop_ = true;
	thread_->join();
	// delete Thread_;
	// if (record_)
	// {
	// 	logger_->stop();
	// 	delete logger_;
	// }
	for (const auto& valuePair : varNameVect_)
		client_->releaseVariableHandle(valuePair.second);
	client_->disconnect();
	// delete client_;

}


void LOPESTreadmillTwinCAT::kill(){}


PLUGIN_STATUS_CODES LOPESTreadmillTwinCAT::pluginExecutionStatus(){
	return PLUGIN_STATUS_CODES::OPERATION_NORMAL;
}

void LOPESTreadmillTwinCAT::start(){}

void LOPESTreadmillTwinCAT::iterationStart(){}
void LOPESTreadmillTwinCAT::iterationEnd(){}

#ifdef UNIX
extern "C" void* create() {
    return new LOPESTreadmillTwinCAT;
}

extern "C" void destroy(void* p) {
    delete (LOPESTreadmillTwinCAT*)p;
}

#endif
#ifdef WIN32
extern "C" __declspec (dllexport) void* __cdecl create() {
	return new LOPESTreadmillTwinCAT;
}

extern "C" __declspec (dllexport) void __cdecl destroy(void* p) {
	delete (LOPESTreadmillTwinCAT*)p;
}
#endif