////////////////////////////////////////////////
//!  configuration_file_handler.h
//!
//!
//!
//!  @date Apr 6, 2009
//!  @author: Rene Staritzbichler
//!  @example:
////////////////////////////////////////////////

#ifndef CONFIGURATION_FILE_HANDLER_H_
#define CONFIGURATION_FILE_HANDLER_H_

#include "../../tara/include/readwrite/stream_operator.h"
#include "../../tara/include/string/io_string_functions.h"
#include "../../tara/include/readwrite/stream_functions.h"


namespace snmp
{

	inline
	SnmpController
	ReadConfigurationFile( std::istream &STREAM)
	{
		size_t
			nr_alerts,
			nr_sys_calls;
		double
			standard_sleep_time,
			alert_sleep_time,
			time_till_first_sys_call_execution,
			time_between_further_sys_call_executions,
			threshold;
		short
			device_id;
		std::string
			ip,
			oid,
			community,
			condition,
			line,
			log_file,
			output_file,
			message_when_no_value;
		int
                        message_interval;

		JumpOverComments( STREAM);
		STREAM >> standard_sleep_time;
		std::cout << "standard_sleep_time: " << standard_sleep_time << std::endl;
		JumpOverComments( STREAM);
		STREAM >> alert_sleep_time;
		std::cout << "alert sleeping time: " << alert_sleep_time << std::endl;
		JumpOverComments( STREAM);
		STREAM >> log_file;
		std::cout << "log file: " << log_file << std::endl;
		JumpOverComments( STREAM);
		STREAM >> nr_alerts;
		std::cout << "number of alerts: " << nr_alerts << std::endl << std::endl;

		SnmpController controller( standard_sleep_time, alert_sleep_time/*, nr_alerts*/, log_file);


		for( size_t i = 0; i < nr_alerts; ++i)
		{
		  std::cout << "snmp get alert: " << i << std::endl;
			JumpOverComments( STREAM);
			STREAM >> ip;
			std::cout << "IP: " << ip << std::endl;
			JumpOverComments( STREAM);
			STREAM >> oid;
			std::cout << "OID: " << oid << std::endl;
			JumpOverComments( STREAM);
			STREAM >> community;
			std::cout << "Community: " << community << std::endl;
			JumpOverComments( STREAM);
			STREAM >> output_file;
			std::cout << "Output file: " << output_file << std::endl;
			JumpOverComments( STREAM);
			STREAM >> threshold;
			std::cout << "Treshold: " << threshold << std::endl;
			JumpOverComments( STREAM);
			STREAM >> condition;
			std::cout << "Condition: " << condition << std::endl;
			JumpOverComments( STREAM);
			STREAM >> time_till_first_sys_call_execution;
			std::cout << "Delay: " << time_till_first_sys_call_execution << std::endl;
			JumpOverComments( STREAM);
			STREAM >> time_between_further_sys_call_executions;
			std::cout << "Delay: " << time_between_further_sys_call_executions << std::endl;
			JumpOverComments( STREAM);
			std::getline(STREAM, line);
			mystr::TrimString( line);
			message_when_no_value = line;
			std::cout << "Message when snmpget value cannot be read: <" << message_when_no_value << ">" << std::endl;
			JumpOverComments( STREAM);
			STREAM >> message_interval;
			std::cout << "message interval: " << message_interval << "s" << std::endl;
			JumpOverComments( STREAM);
			STREAM >> nr_sys_calls;
			std::cout << "Nr sys calls: " << nr_sys_calls << std::endl;

			std::vector< std::string> calls;

			for( size_t j = 0; j < nr_sys_calls; ++j)
			{
				JumpOverComments( STREAM);
				std::cout << j << " call" << std::endl;
				std::getline( STREAM, line);
				std::cout << "line: <" << line << ">" << std::endl;
				mystr::TrimString( line);
				// strip off new lines
				if( line.length() > 5)
				{
					calls.push_back( line);
				}
				std::cout << "call done" << std::endl;
			}
			std::cout << "add get control unit" << std::endl;
			controller.AddSnmpGetControlUnit( ip, oid, community, output_file, threshold, condition, time_till_first_sys_call_execution, time_between_further_sys_call_executions, calls, message_when_no_value, message_interval);
			std::cout << "get control unit added" << std::endl << std::endl;
		}

		JumpOverComments( STREAM);
		STREAM >> nr_alerts;

		for( size_t i = 0; i < nr_alerts; ++i)
		{
		  std::cout << "snmp trap alert: " << i << std::endl;
			JumpOverComments( STREAM);
			STREAM >> device_id;
			std::cout << "device: " << device_id << std::endl;
			JumpOverComments( STREAM);
			STREAM >> time_till_first_sys_call_execution;
			std::cout << "reaction delay: " << time_till_first_sys_call_execution << std::endl;
			JumpOverComments( STREAM);
			STREAM >> time_between_further_sys_call_executions;
			std::cout << "Delay: " << time_between_further_sys_call_executions << std::endl;
			JumpOverComments( STREAM);
			STREAM >> nr_sys_calls;
			std::cout << "nr sys-calls: " << nr_sys_calls << std::endl;

			std::vector< std::string> calls( nr_sys_calls);

			for( size_t j = 0; j < nr_sys_calls; ++j)
			{
				JumpOverComments( STREAM);
				std::getline( STREAM, line);
				std::cout << "line: " << line << std::endl;
				mystr::TrimString( line);
				// strip off new lines
				if( line.length() > 5)
				{
					calls[ j] = line;
				}
				std::cout << "call done" << std::endl;
			}
			std::cout << "add trap control unit" << std::endl;
			controller.AddSnmpTrapControlUnit( device_id, time_till_first_sys_call_execution, time_between_further_sys_call_executions, calls);
			std::cout << "trap control unit added" << std::endl;
		}

		return controller;
	} // end ReadConfigurationFile()



	std::ostream &WriteHelp( std::ostream &STREAM)
	{
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "========================= SNMP SYSTEM CONTROLLER ==============================" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "USAGE:" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "snmp_system_controller.exe OPTION (VALUE)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "OPTIONS: " << std::endl;
		STREAM << "" << std::endl;
		STREAM << "(no_options):                writes this help" << std::endl;
		STREAM << "manual:                      writes a documentation" << std::endl;
		STREAM << "start (config_file_name):    starts the daemon and reads the configuration file (default: /root/admin/controlling/snmp.config)" << std::endl;
		STREAM << "stop:                        terminates daemon" << std::endl;
		STREAM << "failure DEVICE_ID:           sends a message to the daemon that a failure trap was caught from device (DEVICE_ID is a \'short\' and has to be identical to the one in the configuration file)" << std::endl;
		STREAM << "restored DEVICE_ID:          sends a message to the daemon that a restored trap was caught from device (see above)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		return STREAM;
	}  // end WriteHelp()



	std::ostream &WriteManual( std::ostream &STREAM)
	{
		STREAM << "===============================================================================" << std::endl;
		STREAM << "========================= SNMP SYSTEM CONTROLLER MANUAL =======================" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "program for handling snmp device alerts (traps)," << std::endl;
		STREAM << "monitoring device properties," << std::endl;
		STREAM << "and executing shutdowns or notifications" << std::endl;
		STREAM << "of a flexible number of devices" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "author: rene staritzbichler" << std::endl;
		STREAM << "date: 16.4.2009" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "max-planck institute for biophysics" << std::endl;
		STREAM << "frankfurt, germany" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "============================ SHORT DESCRIPTION   ==============================" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "SnmpSystemController is written in a rather compressed form in c++/c." << std::endl;
		STREAM << "it's hierarchy is kept rather flat and is far from an ideal c++ design," << std::endl;
		STREAM << "which would e.g. separate functionality for sending and receiving messages to different classes." << std::endl;
		STREAM << "(it's more a side-product than a main project)" << std::endl;
		STREAM << "it also comprises several levels of functionality into one executable" << std::endl;
		STREAM << "calling the executable with the start option creates a daemon that on" << std::endl;
		STREAM << "one hand actively monitors devices and on the other is acting as a receiver" << std::endl;
		STREAM << "waiting for messages send by another instance of the same executable that" << std::endl;
		STREAM << "then acts as the sender." << std::endl;
		STREAM << "by calling the executable with the failure or restored option a second instance" << std::endl;
		STREAM << "is created that sends a message to the daemon." << std::endl;
		STREAM << "" << std::endl;
		STREAM << "Please note for snmpget based controls: " << std::endl;
		STREAM << "in order to get a correct information from a device the device must be alive and so must be the connection." << std::endl;
		STREAM << "The program considers receiving no information being as serious as crossing the threshold," << std::endl;
		STREAM << "e.g. if the connection is not back within the given time the system calls are executed." << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "==========================  PREREQUISITES =====================================" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "a) snmptrapd" << std::endl;
		STREAM << "b) MIB file " << std::endl;
		STREAM << "c) wireshark (might not be necessary)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "a) we need the snmptrapd to be installed properly to capture snmp traps" << std::endl;
		STREAM << "(a nice step by step explanation about the setup of the snmptrapd on SUSE SLES10 can be found here: http://www.frank4dd.com/howto/nagios/windows-reboot-monitoring-nagios.htm)" << std::endl;
		STREAM << "we use snmptrapd from Net-SNMP" << std::endl;
		STREAM << "which has to be configured such that it filters for the crucial traps of the correct devices" << std::endl;


		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "the first thing to be configured correctly is the file /etc/init.d/snmptrapd (open with editor" << std::endl;
		STREAM << "add the following to the OPTIONS: " << std::endl;
		STREAM << "-c /etc/snmp/snmptrapd.conf -M /usr/share/snmp/mibs -m ALL" << std::endl;
		STREAM << "the first ensures that the correct conf file is read" << std::endl;
		STREAM << "the second determines the location of the MIBS" << std::endl;
		STREAM << "the third ensures that ALL MIBS in that dir are read" << std::endl;
		STREAM << "otherwise all MIBS that are supposed to be used have to explicilty named (colon separated list):" << std::endl;
		STREAM << "-c /etc/snmp/snmptrapd.conf -M /usr/share/snmp/mibs -m MG-SNMP-UPS-MIB   (skip .txt) or:" << std::endl;
		STREAM << "-c /etc/snmp/snmptrapd.conf -M /usr/share/snmp/mibs -m +MG-SNMP-UPS-MIB  (skip .txt)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "restart the daemon after all changes in this and the conf file" << std::endl;
		STREAM << "/etc/init.d/snmptrapd restart" << std::endl;
		STREAM << "" << std::endl;

//		STREAM << "" << std::endl;
//		STREAM << "" << std::endl;

		STREAM << "" << std::endl;
		STREAM << "===============  EXAMPLE OF SNMPTRAPD CONFIGURATION FILE:  ===============" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "(/etc/snmp/snmptrapd.conf)" << std::endl;
		STREAM << "# --------------------------------------------------------------------------- #" << std::endl;
		STREAM << "# snmptrapd.conf:                                                             #" << std::endl;
		STREAM << "#    configuration file for configuring the ucd-snmp snmptrapd agent.         #" << std::endl;
		STREAM << "# ----------------------------------------------------------------------------#" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "# first, we define the access control - the last entry defines the snmp-community of the traps to receive (usually one can change that in the device)" << std::endl;
		STREAM << "authCommunity log,execute,net public" << std::endl;
		STREAM << "authCommunity log,execute,net tara" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "# next , the trap handlers: traphandle OID script (possible OID are defined in the MIB)" << std::endl;
		STREAM << "traphandle      MG-SNMP-UPS-MIB::upsmgUtilityFailure	    /root/admin/controlling/snmptrap_utilityfailure.sh" << std::endl;
		STREAM << "traphandle      MG-SNMP-UPS-MIB::upsmgUtilityRestored	/root/admin/controlling/snmptrap_utilityrestored.sh" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "# END of snmptrapd.conf ---------------------------------------------------- #" << std::endl;		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "=====  EXAMPLE OF SCRIPT EXECUTED BY SNMPTRAPD (snmptrap_utilityrestored.sh):  =====" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "#!/bin/sh" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "TESTLOG=/tmp/snmptraps" << std::endl;
		STREAM << "vars=" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "read host" << std::endl;
		STREAM << "read ip" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "while read oid val; do" << std::endl;
		STREAM << "  if [ \"$vars\" = \"\" ]; then" << std::endl;
		STREAM << "    vars=\"$oid = $val\"" << std::endl;
		STREAM << "  else" << std::endl;
		STREAM << "    vars=\"$vars, $oid = $val\"" << std::endl;
		STREAM << "  fi" << std::endl;
		STREAM << "done" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "if [ -w $TESTLOG ]; then" << std::endl;
		STREAM << "  touch $TESTLOG" << std::endl;
		STREAM << "fi" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "if [ \"$host\" = \"IP_OF_YOUR_DEVICE\" ]; then" << std::endl;
		STREAM << "    echo \"received a UtilityRestored signal from: $host !!!\" | mail -s \"UtilityRestored trap received\" root" << std::endl;
		STREAM << "    /usr/bin/snmp_system_controller.exe restored 1001" << std::endl;
		STREAM << "fi" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "echo trap: $1 $host  $ip  $vars >> $TESTLOG" << std::endl;
		STREAM << "===============  EXAMPLE END  ================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "you may have to check the output into the log file to see the actual format stored in '$host'" << std::endl;
		STREAM << "because this may differ on different machines!!!" << std::endl;
		STREAM << "after switching to a new operating system, I would recommend this script:" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "=====  EXAMPLE OF SCRIPT EXECUTED BY SNMPTRAPD (snmptrap_utilityfailure.sh):  =====" << std::endl;
		STREAM << "#!/bin/sh" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "TESTLOG=/tmp/snmptraps" << std::endl;
		STREAM << "vars=" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "read host" << std::endl;
		STREAM << "read ip" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "check=\"false\"" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "while read oid val; do" << std::endl;
		STREAM << "  if [ \"$vars\" = \"\" ]; then" << std::endl;
		STREAM << "    vars=\"$oid = $val\"" << std::endl;
		STREAM << "  else" << std::endl;
		STREAM << "    vars=\"$vars, $oid = $val\"" << std::endl;
		STREAM << "    if [ \"$val\" = \"192.168.42.235\" ]; then" << std::endl;
		STREAM << "        check=\"true\"" << std::endl;
		STREAM << "    fi" << std::endl;
		STREAM << "  fi" << std::endl;
		STREAM << "done" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "if [ -w $TESTLOG ]; then" << std::endl;
		STREAM << "  touch $TESTLOG" << std::endl;
		STREAM << "fi" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "echo $check" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "if [ \"$check\" = \"true\" ]; then" << std::endl;
		STREAM << "    /usr/bin/snmp_system_controller.exe failure 1001" << std::endl;
		STREAM << "    echo \"email alert ..." >> $TESTLOG\" << std::endl;
		STREAM << "    echo \"pacific received a UtilityFailure signal from UPS: $host !!!\" | mail -s \"snmptrapd: UPS UtilityFailure trap received\" cluster-admin" << std::endl;
		STREAM << "    echo \" ... sent\" >> $TESTLOG" << std::endl;
		STREAM << "    echo \"snmp_system_controller notified about failure\" >> $TESTLOG" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "fi" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "echo fail-trap: $1 x $host x $ip x $vars >> $TESTLOG" << std::endl;
		STREAM << "===============  EXAMPLE END  ================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "after any change in the configuration file restart the snmptrapd:" << std::endl;
		STREAM << "/etc/init.d/snmptrapd (start|stop|restart)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "to access the properties of the device use snmpget" << std::endl;
		STREAM << "example of a snmpget:" << std::endl;
		STREAM << "snmpget -v1 -c COMMUNITY IP OID" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "b) the MIB (Management Information Base) contains all the snmp information the device can provide" << std::endl;
		STREAM << "including both the messages is sends (traps)" << std::endl;
		STREAM << "as well as the information one can access via snmpget" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "snmp trap as such contain simply a couple of sequences of numbers, seperated by dots" << std::endl;
		STREAM << "which are called OID   (similar structure as IP adresses)" << std::endl;
		STREAM << "the MIB file allows to translate these sequences into readable strings" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "a useful freeware for understanding the MIB files is the Mibbrowser (http://tl1.ireasoning.com/mibbrowser.shtml)" << std::endl;
		STREAM << "otherwise they are hard to read" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "the MIB files are stored in a common directory" << std::endl;
		STREAM << "under SUSE SLES 10 it is /usr/share/snmp/mibs/" << std::endl;
		STREAM << "the MIB you get for a device has to be copied in there" << std::endl;
		STREAM << "and the naming has to follow the conventions of the other files in that directory" << std::endl;
		STREAM << "for SLES the name has to be STRING.txt, where STRING is the first string in the file" << std::endl;
		STREAM << "(has to be written identical!!)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "there are different ways to tell the snmptrapd about the new MIB" << std::endl;
		STREAM << "one is to modify the starting script of the snmptrapd in order to recognize new MIBs" << std::endl;
		STREAM << "under SUSE it is /etc/init.d/snmptrapd" << std::endl;
		STREAM << "there has to be the name of the MIB in the following way: " << std::endl;
		STREAM << "startproc $SNMPTRAPD  -m +MG-SNMP-UPS-MIB -Lf /var/log/net-snmpd.log" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "restart the snmptrapd after moving it to the correct directory and editing the start-script" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "if the MIB is included correctly the traps are readable as strings (by snmptrapd or wireshark or..)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "a useful tool when trying to figure out available OIDs is snmpwalk" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "snmpwalk  -v1  -m PowerNet-MIB  -c public  192.168.42.234  environmentalMonitor" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "snmpwalk -v1 (or: -v 2c)  (optional: -m MIB_NAME) -c COMMUNITY  IP_ADRESS  MIBTREENODE( e.g. system)" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "c) wireshark allows monitoring the network traffic" << std::endl;
		STREAM << "by filtering for snmp traps one can easily capture the traps send by the device" << std::endl;
		STREAM << "(hopefully it allows sending test-traps)" << std::endl;
		STREAM << "alternatively one can filter for the IPs of sender and receiver of the traps" << std::endl;
		STREAM << "download it from: http://www.wireshark.org/" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "run as root, interfaces are the eth0, eth1,.." << std::endl;
		STREAM << "start: capture->options" << std::endl;
		STREAM << "filter example: 'ip.src == IP and ip.dst == IP and snmp'" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "========================= SNMP SYSTEM CONTROLLER SET UP =======================" << std::endl;
		STREAM << "===============================================================================" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "after the snmptrapd is set up correctly the only crucial ingredient is the" << std::endl;
		STREAM << "configuration file that allows to control the program" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "=========  EXAMPLE CONFIGURATION FILE OF SNMP_SYSTEM_CONTROLLER: =========" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "### configuration file for the snmp_system_controller.exe" << std::endl;
		STREAM << "# author: rene staritzbichler" << std::endl;
		STREAM << "# date:  06.04.2009" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "# standard time intervall for sending snmpget's in seconds:" << std::endl;
		STREAM << "30" << std::endl;
		STREAM << "# sleep intervall between message checks:" << std::endl;
		STREAM << "3" << std::endl;
		STREAM << "# logfile:" << std::endl;
		STREAM << "/var/log/snmpcontroller.log" << std::endl;
		STREAM << "# number of controls:" << std::endl;
		STREAM << "1" << std::endl;
		STREAM << "#### for each device: ####" << std::endl;
		STREAM << "# IP:" << std::endl;
		STREAM << "192.168.42.235" << std::endl;
		STREAM << "# MIB-OID:" << std::endl;
		STREAM << "MG-SNMP-UPS-MIB::upsmgBatteryLevel.0" << std::endl;
		STREAM << "# comunity" << std::endl;
		STREAM << "tara" << std::endl;
		STREAM << "# output file:" << std::endl;
		STREAM << "/tmp/snmpget.txt" << std::endl;
		STREAM << "# Expected value / threshold:" << std::endl;
		STREAM << "100" << std::endl;
		STREAM << "# control condition( 'EQUAL', 'NONEQUAL', 'LARGER', 'SMALLER'):" << std::endl;
		STREAM << "SMALLER" << std::endl;
		STREAM << "# time until first execution of sys calls [s]:" << std::endl;
		STREAM << "10" << std::endl;
		STREAM << "# time period after which execution is repeated" << std::endl;
		STREAM << "120" << std::endl;
		STREAM << "# syscall if snmpget value cannot be read (disk full)" << std::endl;
		STREAM << "echo \"just a test\" | mail -s \"just testing snmp controller\" cluster-admin" << std::endl;
		STREAM << "# interval of messages for missing snmpget value in sec" << std::endl;
		STREAM << "50" << std::endl;
		STREAM << "# number of syscalls:" << std::endl;
		STREAM << "1 " << std::endl;
		STREAM << "## syscalls:" << std::endl;
		STREAM << "echo \"battery charge below threshold\" | mail me -s \"=>TEST<=: CRUCIAL WARNING from pacific: UPS\"&" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "### number of trap responses:" << std::endl;
		STREAM << "1" << std::endl;
		STREAM << "# device id (identical to signal send by snmptrapd)" << std::endl;
		STREAM << "1001" << std::endl;
		STREAM << "# time until first execution of sys calls [s]:" << std::endl;
		STREAM << "10" << std::endl;
		STREAM << "# time period after which execution is repeated" << std::endl;
		STREAM << "120" << std::endl;
		STREAM << "# number of syscalls:" << std::endl;
		STREAM << "1" << std::endl;
		STREAM << "##syscalls:" << std::endl;
		STREAM << "echo \"utility failure ups\" | mail me -s \"=>TEST<=: ups \"&" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		STREAM << "" << std::endl;
		return STREAM;
	}  // end WriteManual()


} // end namespace snmp




#endif /* CONFIGURATION_FILE_HANDLER_H_ */
