#pragma once 


#include <PluginInterface.hpp>
#include <tcAdsClient.h>
// #include "OpenSimFileLogger.h"
#include <GetTime.h>
#include <thread>
#include <mutex>

#define M_PI 3.14159265359

class DYNLIBEXPORT LOPESTreadmillTwinCAT : public PluginInterface {
public:
	LOPESTreadmillTwinCAT();
	~LOPESTreadmillTwinCAT();

	void init(const std::string& subjectName, const std::string& executionName, const PluginConfig& config);
	void start();
	void iterationStart();
	void iterationEnd();
	void kill();	
	PLUGIN_STATUS_CODES pluginExecutionStatus();

	// const double& getEMGTime();
	const std::vector<std::string>& getXLDDofName();
	const std::map<std::string, double>& getDataMapXLD();
	

	void reset(){}

	/**
	* Get the time stamp of the EMG capture.
	*/
	const double& getXLDTime()
	{
		std::lock_guard<std::mutex> lock(mtxTime_);
		timeStampSafe_ = timeStamp_;
		return  timeStampSafe_;
	}

	void stop();

	void setDirectory(const std::string& outDirectory, const std::string& inDirectory = std::string()) 
	{
		_outDirectory = outDirectory;
		_inDirectory = inDirectory;
	}

	void setVerbose(int verbose)
	{
	}

	void setRecord(bool record)
	{
		record_ = record;
	}

protected:

	std::string _outDirectory;
	std::string _inDirectory;

	void Thread();

	// double timeStampSafe_;
	std::shared_ptr<tcAdsClient> client_;
	std::map<std::string, unsigned long> varNameVect_;
	// OpenSimFileLogger<double>* logger_;
	bool record_;
	// const std::vector<std::string> __dofName;
	// std::set<std::string> nameSet_;
	// ExecutionEmgXml* _executionXLDXml;
	// std::vector<std::string> nameVect_; //!< Vector of channel names
	std::map<std::string, double> _xld, dataXLDSafe_;
	std::map<std::string, std::string> _conversionMap;

	std::mutex mtxXLD_;
	std::mutex mtxTime_;
	double timeStamp_, timeStampSafe_; // Not safe at all

	std::vector<std::string> _varNames;


	std::shared_ptr<std::thread> thread_;
	bool threadStop_;
};
