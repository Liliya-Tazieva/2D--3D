#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <Poco/Process.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP
using namespace std;
using namespace Poco;
int main(int argc, string argv[])
{
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%level %datetime{%H:%m:%s}: %msg");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToStandardOutput, "false");
	LOG(INFO) << "Recreator started";
	bool flag = true; //if folder was uploaded
	bool flag2 = false; //remove rubbish or not
	//finding directory from input argument
	int idx = 0;
	for (int i = 0; argv[0][i] != '\0'; ++i) if (argv[0][i] == '/') idx = i;
	//path to input video(folder)
	string working_path = argv[0].substr(0, idx);
	//replacing / with \\ in working path
	for (int i = 0; working_path[i] != '\0'; ++i) if (working_path[i] == '/') working_path[i] = '\\';
	idx = argv[0].find('\0', 0);
	if (argv[0][idx + 1] == '1') flag2 = true; //remove rubbish-indicator
	LOG(INFO) << "Checking what was uploaded";
	//Checking what is given: video or photoes
	ifstream f;
	f.open(argv[0].substr(0, idx));
	if (f)
	{
		LOG(INFO) << "Video";
		flag = false;//means that video was given
		
		//Creating folder, named "Frames" for photos, that FFMPEG will create
		string command = "mkdir " + working_path + "\\Frames";
		system(command.c_str());
		
		//Read frame-rate from argv[0]
		string f_r = argv[0].substr(idx + 3, argv[0].find('\0', idx + 3));

		//FFMPEG splits video to frames and saves results as photos
		LOG(INFO) << "FFMPEG started";
		string temporary = working_path + "\\Frames\\image%d.jpg";
		Process::Args args1;
		args1.push_back("-i");
		args1.push_back(argv[0].c_str());
		args1.push_back("-r");
		args1.push_back(f_r.c_str());
		args1.push_back(temporary.c_str());
		ProcessHandle ph(Process::launch("C:\\Project_Programs\\FFMPEG\\bin\\ffmpeg.exe", args1));
		int rc = ph.wait();
		LOG(INFO) << "FFMPEG finished";
	}
	else LOG(INFO) << "Folder with photos";
	f.close();
	/*
	//VisualSFM turn photos into cloud point and creates some stuff
	//including cmp.ini, that will be an argument for CMPMVS
	LOG(INFO) << "VSFM started";
	for (int i = 0; argv[0][i] != '\0'; ++i) if (argv[0][i] == '/') idx = i;
	Process::Args args2;
	args2.push_back("sfm+cmp");
	string arg_str;
	if (!flag) arg_str = working_path + "\\Frames";
	else arg_str = working_path + "\\" + argv[0].substr(idx + 1, argv[0].find('\0', idx));
	args2.push_back(arg_str.c_str());
	arg_str = working_path + "\\VSFM_result.nvm";
	args2.push_back(arg_str.c_str());
	ProcessHandle ph(Process::launch("C:\\Project_Programs\\VSFM\\VisualSFM.exe", args2));
	int rc = ph.wait();
	LOG(INFO) << "VSFM finished";
	*/
	string arg_str;
	//Changing ini file
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
	/*
	//CMPMVS creates some stuff, including textures(png type) and mesh(wrl type)
	LOG(INFO) << "CMPMVS started";
	Process::Args args3;
	args3.push_back(arg_str.c_str());
	ph = ProcessHandle(Process::launch("C:\\Project_Programs\\CMPMVS\\CMPMVS.exe", args3, "C:\\Project_Programs\\CMPMVS"));
	rc = ph.wait();
	LOG(INFO) << "CMPMVS finished";

	//MeshLab resave simplified mesh like an obj type file
	LOG(INFO) << "MeshLab started";
	Process::Args args4;
	arg_str = "-i " + working_path + "\\VSFM_result.nvm.cmp\\00\\data\\_OUT\\simplified10\\meshTextured.wrl";
	args4.push_back(arg_str.c_str());
	arg_str = "-o " + working_path + "\\simple_mesh.obj";
	args4.push_back(arg_str.c_str());
	args4.push_back("-m vc vn fc wt");
	ph = ProcessHandle(Process::launch("C:\\Project_Programs\\MeshLab\\meshlabserver.exe", args4));
	rc = ph.wait();
	LOG(INFO) << "MeshLab finished";

	//Shift textures into folder, that contains vodeo
	LOG(INFO) << "Shhifting useful files to parent directory";
	arg_str = "copy " + working_path + "\\VSFM_result.nvm.cmp\\00\\data\\_OUT\\simplified10\\*.png "
		+ working_path + "\\*.png";
	system(arg_str.c_str());
	LOG(INFO) << "Shifted";
	
	//Remove everything else, if necessary
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
	LOG(INFO) << "Recreator finished";
	*/return 0;
}