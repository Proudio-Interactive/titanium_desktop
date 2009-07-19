/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include <Poco/Environment.h>
#include <Poco/Process.h>
#include "process_binding.h"
#include "process.h"
#include <signal.h>

namespace ti
{

	std::map<std::string,int> ProcessBinding::signals;
	ProcessBinding::ProcessBinding() : AccessorBoundObject("Process")
	{
		Logger::Get("Process")->Debug("Initializing Titanium.Process");
		
		//TODO doc me
		SetMethod("createProcess", &ProcessBinding::CreateProcess);
		SetMethod("createPipe", &ProcessBinding::CreatePipe);

#if defined(OS_OSX) || (OS_LINUX)
		signals["SIGHUP"] = SIGHUP;
		signals["SIGINT"] = SIGINT;
		signals["SIGQUIT"] = SIGQUIT;
		signals["SIGILL"] = SIGILL;
		signals["SIGTRAP"] = SIGTRAP;
		signals["SIGABRT"] = SIGABRT;
		signals["SIGFPE"] = SIGFPE;
		signals["SIGKILL"] = SIGKILL;
		signals["SIGBUS"] = SIGBUS;
		signals["SIGSEGV"] = SIGSEGV;
		signals["SIGSYS"] = SIGSYS;
		signals["SIGPIPE"] = SIGPIPE;
		signals["SIGALRM"] = SIGALRM;
		signals["SIGTERM"] = SIGTERM;
		signals["SIGURG"] = SIGURG;
		signals["SIGSTOP"] = SIGSTOP;
		signals["SIGTSTP"] = SIGTSTP;
		signals["SIGCHLD"] = SIGCHLD;
		signals["SIGTTIN"] = SIGTTIN;
		signals["SIGTTOU"] = SIGTTOU;
		signals["SIGIO"] = SIGIO;
		signals["SIGXCPU"] = SIGXCPU;
		signals["SIGXFSZ"] = SIGXFSZ;
		signals["SIGVTALRM"] = SIGVTALRM;
		signals["SIGPROF"] = SIGPROF;
		signals["SIGWINCH"] = SIGWINCH;
		signals["SIGUSR1"] = SIGUSR1;
		signals["SIGUSR2"] = SIGUSR2;
#elif defined(OS_WIN32)
		signals["SIGABRT"] = SIGABRT;
		signals["SIGFPE"] = SIGFPE;
		signals["SIGILL"] = SIGILL;
		signals["SIGINT"] = SIGINT;
		signals["SIGSEGV"] = SIGSEGV;
		signals["SIGTERM"] = SIGTERM;
#endif
#if defined(OS_OSX)
		signals["SIGEMT"] = SIGEMT;
		signals["SIGINFO"] = SIGINFO;
#endif
		std::map<std::string,int>::iterator iter;
		for (iter = signals.begin(); iter != signals.end(); iter++)
		{
			Set(iter->first.c_str(), Value::NewInt(iter->second));
		}
		
		Logger::Get("Process")->Debug("returning");
	}

	ProcessBinding::~ProcessBinding()
	{
	}

	void ProcessBinding::CreateProcess(const ValueList& args, SharedValue result)
	{
		args.VerifyException("createProcess", "o|l");
		SharedKObject temp = 0;
		SharedKList argList = 0;
		SharedKObject environment = 0;
		AutoPipe stdinPipe = 0;
		AutoPipe stdoutPipe = 0;
		AutoPipe stderrPipe = 0;

		if (args.at(0)->IsObject())
		{
			SharedKObject options = args.GetObject(0);
			argList = options->GetList("args", 0);
			if (argList.isNull())
				throw ValueException::FromString(
					"Titanium.Process option 'args' must be an array");

			environment = options->GetObject("env", 0);

			temp = options->GetObject("stdin");
			if (!temp.isNull())
				stdinPipe = temp.cast<Pipe>();

			temp = options->GetObject("stdout");
			if (!temp.isNull())
				stdoutPipe = temp.cast<Pipe>();

			temp = options->GetObject("stderr");
			if (!temp.isNull())
				stderrPipe = temp.cast<Pipe>();

		}
		else if (args.at(0)->IsList())
		{
			argList = args.at(0)->ToList();
			if (args.size() > 1)
				environment = args.GetObject(1);

			if (args.size() > 2)
			{
				temp = args.GetObject(2);
				if (!temp.isNull())
					stdinPipe = temp.cast<Pipe>();
			}

			if (args.size() > 3)
			{
				temp = args.GetObject(3);
				if (!temp.isNull())
					stdoutPipe = temp.cast<Pipe>();
			}

			if (args.size() > 4)
			{
				temp = args.GetObject(4);
				if (!temp.isNull())
					stderrPipe = temp.cast<Pipe>();
			}
		}

		if (argList.isNull())
		{
			throw ValueException::FromString(
				"Titanium.Process option argument 'args' was undefined");
		}

		if (argList->Size() == 0)
		{
			throw ValueException::FromString(
				"Titanium.Process option argument 'args' must have at least 1 element");
		}

		SharedKList argsClone = new StaticBoundList();
		for (size_t i = 0; i < argList->Size(); i++)
		{
			SharedValue arg = Value::Undefined;
			if (!argList->At(i)->IsString())
			{
				SharedString ss = argList->At(i)->DisplayString();
				arg = Value::NewString(ss);
			}
			else
			{
				arg = argList->At(i);
			}
			argsClone->Append(arg);
		}

		AutoProcess process = Process::CreateProcess();
		process->SetArguments(argsClone);

		if (!environment.isNull())
			process->SetEnvironment(environment);
		if (!stdinPipe.isNull())
			process->SetStdin(stdinPipe);
		if (!stdoutPipe.isNull())
			process->SetStdout(stdoutPipe);
		if (!stderrPipe.isNull())
			process->SetStderr(stderrPipe);

		// this is a callable object
		result->SetMethod(process);
	}

	void ProcessBinding::CreatePipe(const ValueList& args, SharedValue result)
	{
		result->SetObject(new Pipe());
	}
}
