#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <Poco/Process.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Windows.h>
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP
using namespace std;
using namespace Poco;
int main(int argc, char* argv[])
{
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%level %datetime{%H:%m:%s}: %msg");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, "false");
	LOG(INFO) << "Recreator started";
	LOG(INFO) << "argv[1]" << argv[1];
	LOG(INFO) << "argv[2]" << argv[2];
	LOG(INFO) << "argv[3]" << argv[3];
	LOG(INFO) << "argv[4]" << argv[4];
	bool flag = true; //if folder was uploaded
	bool flag2 = false; //remove rubbish or not
	//path where input video(folder) is located
	string working_path = argv[2];
	//replacing / with \\ in working path
	for (int i = 0; working_path[i] != '\0'; ++i) if (working_path[i] == '/') working_path[i] = '\\';
	int idx = 0;
	for (int i = 0; i < working_path.length(); ++i) if (working_path[i] == '\\') idx = i;
	working_path = working_path.substr(0, idx);
	//setting current path to executive file
	string current_path = argv[1];
	for (int i = 0; current_path[i] != '\0'; ++i) if (current_path[i] == '/') current_path[i] = '\\';	//setting remove rubbish-indicator
	if (argv[3][0] == '1') flag2 = true;
	LOG(INFO) << "Checking what was uploaded";
	//Checking what is given: video or photoes
	ifstream f;
	f.open(argv[2]);
	if (f)
	{
		LOG(INFO) << "Video";
		flag = false;//means that video was given

		//Creating folder, named "Frames" for photos, that FFMPEG will create
		string command = "mkdir " + working_path + "\\Frames";
		system(command.c_str());

		//Read frame-rate from argv[4]
		string f_r = argv[4];

		//FFMPEG splits video to frames and saves results as photos
		LOG(INFO) << "FFMPEG started";
		string temporary = working_path + "\\Frames\\image%d.jpg";
		try
		{
			Process::Args args1;
			args1.push_back("-i");
			args1.push_back(argv[2]);
			args1.push_back("-r");
			args1.push_back(f_r.c_str());
			args1.push_back(temporary.c_str());
			temporary = current_path + "\\Project_Programs\\ffmpeg.exe";
			ProcessHandle ph(Process::launch(temporary.c_str(), args1));
			int rc = ph.wait();
			LOG(INFO) << "FFMPEG finished";
		}
		catch (Poco::Exception &e)
		{
			LOG(INFO) << "POCO failed:" << endl << e.message();
		}
	}
	else LOG(INFO) << "Folder with photos";
	f.close();

	//VisualSFM turn photos into cloud point and creates some stuff
	//including cmp.ini, that will be an argument for CMPMVS
	LOG(INFO) << "VSFM started";
	string arg_str;
	try
	{
		Process::Args args2;
		args2.push_back("sfm+cmp");
		if (flag)
		{
			arg_str = argv[2];
			for (int i = 0; arg_str[i] != '\0'; ++i) if (arg_str[i] == '/') arg_str[i] = '\\';
		}
		else arg_str = working_path + "\\Frames";
		args2.push_back(arg_str.c_str());
		arg_str = working_path + "\\VSFM_result.nvm";
		args2.push_back(arg_str.c_str());
		arg_str = current_path + "\\Project_Programs\\VSFM\\VisualSFM.exe";
		ProcessHandle ph(Process::launch(arg_str.c_str(), args2));
		int rc = ph.wait();
		LOG(INFO) << "VSFM finished";
	}
	catch (Poco::Exception &e)
	{
		LOG(INFO) << "POCO failed:" << endl << e.message();
	}

	//Changing ini file
	try
	{
		LOG(INFO) << "Changing .ini file";
		ifstream f1;
		ofstream f2;
		arg_str = working_path + "\\VSFM_result.nvm.cmp\\00\\mvs.ini";
		f1.open(arg_str.c_str());
		arg_str = working_path + "\\VSFM_result.nvm.cmp\\00\\mvs_res.ini";
		f2.open(arg_str.c_str());
		for (int i = 1; i < 37; ++i)
		{
		string s;
		f1 >> s;
		if (i == 8) s = s.substr(0, 6) + '2';
		else if (i == 14) s = s.substr(0, 15) + "25";
		else if (i == 31)
		{
		f2 << s << endl;
		f2 << "doComputeDEMandOrtoPhoto=FALSE" << endl;
		f2 << "doGenerateVideoFrames=FALSE" << endl;
		f2 << "#EOF" << endl << endl << endl;
		break;
		}
		f2 << s << endl;
		}
		f1.close();
		f2.close();
		LOG(INFO) << "Changed";
	}
	catch (SystemException &e)
	{
		LOG(INFO) << "Failed to change ini file" << endl << e.message();
	}
	
	//CMPMVS creates some stuff, including textures(png type) and mesh(wrl type)
	arg_str = working_path + "\\VSFM_result.nvm.cmp\\00\\mvs_res.ini";
	LOG(INFO) << "CMPMVS started";
	try
	{
		Process::Args args3;
		args3.push_back(arg_str.c_str());
		for (int i = 0; arg_str[i] != '\0'; ++i) if (arg_str[i] == '/') arg_str[i] = '\\';
		arg_str = current_path + "\\Project_Programs\\CMPMVS\\CMPMVS.exe";
		for (int i = 0; i < arg_str.length(); ++i) if (arg_str[i] == '\\') idx = i;
		ProcessHandle ph = ProcessHandle(Process::launch(arg_str.c_str(), args3,
											arg_str.substr(0,idx).c_str()));
		int rc = ph.wait();
		LOG(INFO) << "CMPMVS finished";
	}
	catch (Poco::Exception &e)
	{
		LOG(INFO) << "POCO failed:" << endl << e.message();
	}
	
	//MeshLab resave simplified mesh like an obj type file
	LOG(INFO) << "MeshLab started";
	try
	{
		Process::Args args4;
		arg_str = "-i " + working_path + "\\VSFM_result.nvm.cmp\\00\\data\\_OUT\\simplified25\\meshTextured.wrl";
		args4.push_back(arg_str.c_str());
		arg_str = "-o " + working_path + "\\simple_mesh.obj";
		args4.push_back(arg_str.c_str());
		args4.push_back("-m vc vn fc wt");
		arg_str = current_path + "\\Project_Programs\\MeshLab\\meshlabserver.exe";
		ProcessHandle ph = ProcessHandle(Process::launch(arg_str.c_str(), args4));
		int rc = ph.wait();
		LOG(INFO) << "MeshLab finished";
	}
	catch (Poco::Exception &e)
	{
		LOG(INFO) << "POCO failed:" << endl << e.message();
	}
	
	//Shift textures into folder, that contains video
	try
	{
		LOG(INFO) << "Shhifting useful files to parent directory";
		arg_str = "copy " + working_path + "\\VSFM_result.nvm.cmp\\00\\data\\_OUT\\simplified25\\*.png "
			+ working_path + "\\*.png";
		system(arg_str.c_str());
		LOG(INFO) << "Shifted";
	}
	catch (SystemException &e)
	{
		LOG(INFO) << "Shifting failed:" << endl << e.message();
	}
	
	//Remove everything else, if necessary
	try
	{
		if(flag2)
		{
			LOG(INFO) << "Removing rubbish";
			if (!flag)
			{
				arg_str = "RMDIR /S /Q " + working_path + "\\Frames";
				system(arg_str.c_str());
			}
			arg_str = "RMDIR /S /Q " + working_path + "\\VSFM_result.nvm.cmp";
			system(arg_str.c_str());
			arg_str = "del " + working_path + "\\VSFM_result.nvm";
			system(arg_str.c_str());
			LOG(INFO) << "All rubbish removed";
		}
	}
	catch (SystemException &e)
	{
		LOG(INFO) << "Deleting files failed:" << endl << e.message();
	}
	LOG(INFO) << "Recreator finished";
	return 0;
}