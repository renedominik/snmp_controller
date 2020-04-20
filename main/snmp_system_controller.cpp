////////////////////////////////////////////////
//!  snmp_system_controller.cpp
//!
//!
//!
//!  @date Apr 6, 2009
//!  @author: Rene Staritzbichler
//!  @example:
////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <ctime>
#include <vector>
#include "../include/snmp_controller.h"
#include "../include/configuration_file_handler.h"


int main( int argc, char *argv[])
{
	assert( argc <= 3);

	snmp::SnmpController controller;

	if( argc > 1)
	{
		std::string argument( argv[1]);

		if( argument == "failure")
		{
			// send a device failure message to the daemon, set it to alert state
			assert( argc == 3);
			short device_id( mystr::ConvertStringToNumericalValue< short>( argv[ 2]));
			controller.InitializeConnection();
			controller.SendMessage( snmp::UTILITY_FAILURE, device_id);
			return 0;
		}
		else if( argument == "restored")
		{
			// send a device restored message to daemon, set from alert to normal state
			assert( argc == 3);
			short device_id( mystr::ConvertStringToNumericalValue< short>( argv[ 2]));
			controller.InitializeConnection();
			controller.SendMessage( snmp::UTILITY_RESTORED, device_id);
			return 0;
		}
		else if( argument == "manual")
		{
			snmp::WriteManual( std::cout);
		}
		else if( argument == "start")
		{
			std::cout << "start daemon" << std::endl;
			std::ifstream read;
			if( argc == 3)
			{
				std::cout << "read config: " << argv[2] << std::endl;
				read.open( argv[2]);
			}
			else
			{
				std::cout << "try to open standard config file: /root/admin/controlling/snmp_config.txt" << std::endl;
				read.open( "/root/admin/controlling/snmp_config.txt");
			}
			// read configuration
			if( read)
			{
				controller = snmp::ReadConfigurationFile( read);
			}
			else
			{
				std::cout << argv[2] << " config file not opened" << std::endl;
				return 1;
			}
			std::cout << "config read" << std::endl;
			std::cout << controller;

			std::cout << "daemonize ..." << std::endl;

			pid_t pid;

			// split process
		    if ((pid = fork()) < 0)
		    {
		        perror("fork() failed");
		        return 1;
		    }

		    // child process
		    if (pid == 0)
		    {
		        chdir("/");
		        setsid();  // set session id

		        close(STDIN_FILENO);
		        close(STDOUT_FILENO);
		        close(STDERR_FILENO);

//		        open("/dev/null", O_RDWR);
//		        dup(STDIN_FILENO);
//		        dup(STDIN_FILENO);

		        // set up messaging layer
				if( controller.InitializeConnection() == -1)
				{ exit(-1);}
				controller.Loop();
				controller.CutConnection();
		    }
		    else
		    {
				// parent process
				printf("Daemon has PID %i.\n", pid);
		    }
			return 0;
		}
		else if( argument == "stop")
		{
			// connect to daemon and send termination message
			controller.InitializeConnection();
			controller.SendMessage( snmp::TERMINATE);
			std::cout << "termination message sent to deamon" << std::endl;
			return 0;
		}
	}
	else
	{
		snmp::WriteHelp( std::cout);
		return 0;
	}
	return 0;
};
