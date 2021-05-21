//!  @file main.cpp The Combined API (CAPI) sample application.
//	modified by LNZ 19/11/19
//----------------------------------------------------------------------------
//
//  Copyright (C) 2017, Northern Digital Inc. All rights reserved.
//
//  All Northern Digital Inc. ("NDI") Media and/or Sample Code and/or Sample Code
//  Documentation (collectively referred to as "Sample Code") is licensed and provided "as
//  is" without warranty of any kind. The licensee, by use of the Sample Code, warrants to
//  NDI that the Sample Code is fit for the use and purpose for which the licensee intends to
//  use the Sample Code. NDI makes no warranties, express or implied, that the functions
//  contained in the Sample Code will meet the licensee's requirements or that the operation
//  of the programs contained therein will be error free. This warranty as expressed herein is
//  exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in
//  law, warranties, representations, and conditions of every kind pertaining in any way to
//  the Sample Code licensed and provided by NDI hereunder, including without limitation,
//  each warranty and/or condition of quality, merchantability, description, operation,
//  adequacy, suitability, fitness for particular purpose, title, interference with use or
//  enjoyment, and/or non infringement, whether express or implied by statute, common law,
//  usage of trade, course of dealing, custom, or otherwise. No NDI dealer, distributor, agent
//  or employee is authorized to make any modification or addition to this warranty.
//  In no event shall NDI nor any of its employees be liable for any direct, indirect,
//  incidental, special, exemplary, or consequential damages, sundry damages or any
//  damages whatsoever, including, but not limited to, procurement of substitute goods or
//  services, loss of use, data or profits, or business interruption, however caused. In no
//  event shall NDI's liability to the licensee exceed the amount paid by the licensee for the
//  Sample Code or any NDI products that accompany the Sample Code. The said limitations
//  and exclusions of liability shall apply whether or not any such damages are construed as
//  arising from a breach of a representation, warranty, guarantee, covenant, obligation,
//  condition or fundamental term or on any theory of liability, whether in contract, strict
//  liability, or tort (including negligence or otherwise) arising in any way out of the use of
//  the Sample Code even if advised of the possibility of such damage. In no event shall
//  NDI be liable for any claims, losses, damages, judgments, costs, awards, expenses or
//  liabilities of any kind whatsoever arising directly or indirectly from any injury to person
//  or property, arising from the Sample Code or any use thereof
//
//----------------------------------------------------------------------------

#ifdef _WIN32
	// For Windows Sleep(ms)
	#include <windows.h>
#else
	// For POSIX sleep(sec)
	#include <unistd.h>
#endif

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "CombinedApi.h"
#include "PortHandleInfo.h"
#include "ToolData.h"

static CombinedApi capi = CombinedApi();
HANDLE serialHandle;
LPCSTR comPort = "\\\\.\\COM1";

/**
 * @brief There's no standard cross platform sleep() method prior to C++11
 */
void sleepSeconds(unsigned numSeconds)
{
#ifdef _WIN32
	Sleep((DWORD)1000 * numSeconds); // Sleep(ms)
#else
	sleep(numSeconds); // sleep(sec)
#endif
}

/**
 * @brief Prints a debug message if a method call failed.
 * @details To use, pass the method name and the error code returned by the method.
 *          Eg: onErrorPrintDebugMessage("capi.initialize()", capi.initialize());
 *          If the call succeeds, this method does nothing.
 *          If the call fails, this method prints an error message to stdout.
 */
void onErrorPrintDebugMessage(std::string methodName, int errorCode)
{
	if (errorCode < 0)
	{
		std::cout << methodName << " failed: " << capi.errorToString(errorCode) << std::endl;
	}
}

/**
 * @brief Put the system into tracking mode and write data to COM1
 */
void getTrackingData(HANDLE serialHandle)
{	
	 //ptr_out will also be number of bytes to write! yay!
	std::uintptr_t i;
	std::uintptr_t f;
	DWORD dwBytesWritten = 0;

	// Start tracking, output a few frames of data, and stop tracking
	std::cout << std::endl << "Entering tracking mode and sending data to LabVIEW over COM1..." << std::endl;
	onErrorPrintDebugMessage("capi.startTracking()", capi.startTracking());
	while (true)
	{
		char comBuff[20000] = { 0 };
		char *ptr_out = comBuff;
		i = reinterpret_cast<std::uintptr_t>(ptr_out);
		std::vector<ToolData> toolData = capi.getTrackingDataBX();
		//std::cout << toolData.size() << std::endl;

		//compatibility with OptiTrack tracking program
		int frameNumber = 123;
		int numRigidBodies = toolData.size();
		int bodyID = 42;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float qx = 0.0f;
		float qy = 0.0f;
		float qz = 0.0f;
		float q0 = 1.0f;
		bool isTracked = 0;
		double timeStamp = 0.0f;

		//format into fixed length binary string that labview can read
		memcpy(ptr_out, &frameNumber, 4);
		ptr_out += 4;
		memcpy(ptr_out, &numRigidBodies, 4);
		ptr_out += 4;
		memcpy(ptr_out, &bodyID, 4);
		ptr_out += 4;
		for (int i = 0; i < toolData.size(); i++)
		{
			if (!toolData[i].transform.isMissing())
			{
				x = toolData[i].transform.tx;
				y = toolData[i].transform.ty;
				z = toolData[i].transform.tz;
				qx = toolData[i].transform.qx;
				qy = toolData[i].transform.qy;
				qz = toolData[i].transform.qz;
				q0 = toolData[i].transform.q0;
				isTracked = 1;
			}
			else {
				x = 0;
				y = 0;
				z = 0;
				qx = 0;
				qy = 0;
				qz = 0;
				q0 = 1;
				isTracked = 0;
			}
			memcpy(ptr_out, &x, 4);
			ptr_out += 4;
			memcpy(ptr_out, &y, 4);
			ptr_out += 4;
			memcpy(ptr_out, &z, 4);
			ptr_out += 4;
			memcpy(ptr_out, &qx, 4);
			ptr_out += 4;
			memcpy(ptr_out, &qy, 4);
			ptr_out += 4;
			memcpy(ptr_out, &qz, 4);
			ptr_out += 4;
			memcpy(ptr_out, &q0, 4);
			ptr_out += 4;
			memcpy(ptr_out, &isTracked, 2);
			ptr_out += 2;
		}

		//format into fixed length binary string that labview can read
		//memcpy(ptr_out, &frameNumber, 4);
		//ptr_out += 4;
		//memcpy(ptr_out, &numRigidBodies, 4);
		//ptr_out += 4;
		//for (int i = 0; i < toolData.size(); i++)
		//{
		//	memcpy(ptr_out, &bodyID, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &x, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &y, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &z, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &qx, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &qy, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &qz, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &q0, 4);
		//	ptr_out += 4;
		//	memcpy(ptr_out, &isTracked, 2);
		//	ptr_out += 2;
		//}
		memcpy(ptr_out, &timeStamp, 8);
		ptr_out += 8;

		f = reinterpret_cast<std::uintptr_t>(ptr_out);
		auto numBytes = f - i;
		//std::cout << "num rigid bodies:" << numRigidBodies << std::endl;
		//std::cout << "bytes written: " << numBytes << std::endl;
		if (!WriteFile(serialHandle, comBuff, numBytes, &dwBytesWritten, NULL)) {
			std::cerr << "Error! Could not write to COM1 :(" << std::endl;
		}

	}
	onErrorPrintDebugMessage("capi.stopTracking()", capi.stopTracking());
}

/**
 * @brief Initialize and enable loaded tools. This is the same regardless of tool type.
 */
int initializeAndEnableTools()
{
	// Initialize and enable tools
	std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
	for (int i = 0; i < portHandles.size(); i++)
	{
		std::cout << i << std::endl;
		onErrorPrintDebugMessage("capi.portHandleInitialize()", capi.portHandleInitialize(portHandles[i].getPortHandle()));
		onErrorPrintDebugMessage("capi.portHandleEnable()", capi.portHandleEnable(portHandles[i].getPortHandle()));
	}
	return portHandles.size();
}

/**
 * @brief   The entry point for the CAPIsample application.
 * @details The invocation of CAPIsample is expected to pass a few arguments: ./CAPIsample [hostname] [scu_hostname]
 *          arg(0) - (default)  The path to this application (ignore this)
 *          arg(1) - (required) The measurement device's hostname, IP address, or serial port.
 *          Eg: Connecting to device by IP address: "169.254.8.50"
 *          Eg: Connecting to device by zeroconf hostname: "P9-B0103.local"
 *          Eg: Connecting to serial port varies by OS: "COM10" (Win), "/dev/ttyUSB0" (Linux), "/dev/cu.usbserial-001014FA" (Mac)
 *          arg(2) - (optional) A System Control Unit (SCU) hostname, used to connect active tools.
 */
int main(int argc, char* argv[])
{
	// Validate the number of arguments received
	if (argc < 2 || argc > 3)
	{
		std::cout << "CAPIsample Ver " << capi.getVersion() << std::endl
			<< "usage: ./capisample <hostname> [<scu_hostname>]" << std::endl
			<< "where:" << std::endl
			<< "    <hostname>      (required) The measurement device's hostname, IP address, or serial port." << std::endl
			<< "    <scu_hostname>  (optional) A System Control Unit (SCU) hostname, used to connect active tools." << std::endl
			<< "example hostnames:" << std::endl
			<< "    Connecting to device by IP address: 169.254.8.50" << std::endl
			<< "    Connecting to device by hostname: P9-B0103.local" << std::endl
			<< "    Connecting to serial port varies by operating system:" << std::endl
			<< "        COM10 (Windows), /dev/ttyUSB0 (Linux), /dev/cu.usbserial-001014FA (Mac)" << std::endl;
		return -1;
	}

	std::cout << "Opening COM port" << std::endl;
	serialHandle = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serialHandle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cerr << "Error! COM port not found :(" << std::endl;
		}
	}

	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	if (!GetCommState(serialHandle, &serialParams)) {
		std::cerr << "Error! Could not get serial parameters :(" << std::endl;
	}

	serialParams.BaudRate = CBR_115200;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = NOPARITY;
	if (!SetCommState(serialHandle, &serialParams))
	{
		std::cerr << "Error! Could not set serial parameters :(" << std::endl;
	}

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 500;
	timeout.ReadTotalTimeoutConstant = 500;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 500;
	timeout.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(serialHandle, &timeout))
	{
		std::cerr << "Error! Could not set timeouts :(" << std::endl;
	}

	// Ignore argv[0], and assign the hostname and scu_hostname accordingly
	std::string hostname = std::string(argv[1]);
	std::string scu_hostname = (argc == 3) ? std::string(argv[2]) : "";

	// Attempt to connect to the device
	if (capi.connect(hostname) != 0)
	{
		std::cerr << "Connection Failed!" << std::endl;
	}
	std::cout << "Connected to Aurora!" << std::endl;
	sleepSeconds(1);
	onErrorPrintDebugMessage("capi.initialize()", capi.initialize());
	int numTools = initializeAndEnableTools();
	char comBuff[20000] = { 0 };
	char *ptr_out = comBuff;
	std::uintptr_t i = reinterpret_cast<std::uintptr_t>(ptr_out);
	memcpy(ptr_out, &numTools, 4);
	ptr_out += 4;
	std::uintptr_t f = reinterpret_cast<std::uintptr_t>(ptr_out);
	auto numBytes = f - i;

	std::cout << "Number of tools: " << numTools << std::endl;
	//char msg[] = "Ready to stream";
	//size_t len = strlen(msg);
	
	DWORD dwBytesWritten = 0;
	if (!WriteFile(serialHandle, comBuff, numBytes, &dwBytesWritten, NULL)) {
	//if (!WriteFile(serialHandle, msg, len, &dwBytesWritten, NULL)) {
		std::cerr << "Error! Could not write to COM1 :(" << std::endl;
	}
	else {
		std::cout << "COM1 initialized!" << std::endl;
	}
	sleepSeconds(1);
	getTrackingData(serialHandle);

	return 0;
}
