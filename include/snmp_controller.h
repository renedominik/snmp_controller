////////////////////////////////////////////////
//!  snmp_controller.h
//!
//!
//!
//!  @date Apr 6, 2009
//!  @author: Rene Staritzbichler
//!  @example:
////////////////////////////////////////////////

#ifndef SNMP_CONTROLLER_H_
#define SNMP_CONTROLLER_H_

#include <ctime>
#include <cstdlib>
#include <map>
#include <sstream>
#include <iostream>

#include "control_unit.h"
#include "../../tara/include/readwrite/stream_operator.h"
#include "../../tara/include/string/io_string_functions.h"
#include "../../tara/include/communication/messenger.h"


template<class T>
std::string ConvertValueToString(const T& VALUE)
{
	std::stringstream strm;
	strm << VALUE;
	return strm.str();
}


namespace snmp
{
	//! type of command send to snmp controller
	enum e_Command { UTILITY_FAILURE, UTILITY_RESTORED, TERMINATE};

	/////////////////////////////////////////////////////////////////////////////////
	//! class Message
	//! helper class containing information sent between subprocesses
	/////////////////////////////////////////////////////////////////////////////////
	class Message
	{
	public:
		long         m_MsgTyp;    //!< message type needed by msgrcv and msgsnd commands
		e_Command    m_Command;   //!< type of command
		short        m_Device;    //!< device id the command relates to

		//! default constructor
		Message()
		: m_MsgTyp( 1),
		m_Command(),
		m_Device()
		{}

		//! construct from data
		Message( const e_Command &COMMAND, short DEVICE)
		: m_MsgTyp( 1),
		m_Command( COMMAND),
		m_Device( DEVICE)
		{}


		Message( const e_Command &COMMAND)
		: m_MsgTyp( 1),
		m_Command( COMMAND),
		m_Device()
		{}
	};

	////////////////////////////////////////////////////////////////////////////////////////
	//!  class SnmpController
	//!  blown up class contain functionality for
	//!  receiving messages, sending messages from/to daemonized (forked) subprocess,
	//!  actively checking snmp devices
	//!  and reacting on snmp traps sent by the devices
	//! @author: Rene Staritzbichler
	//! @date: 15.4.2009
	/////////////////////////////////////////////////////////////////////////////////////////
	class SnmpController
	: public comm::Messenger
	{
	protected:
		///////////////
		//	DATA     //
		///////////////
		double      				 			m_StandardIntervallBetweenSnmpGets;      //!< time the program keeps on looping over the message-checking/sleep procedure
		bool						 			m_IsAtAlertLevel;						 //!< top level alert level, true if any control unit in alert state
		double 									m_Sleep;								 //!< sleep period between checking for traps
		std::vector< SnmpGetControlUnit>        m_GetControls;							 //!< control units for the snmpget commands, active control
		std::map< short, SnmpTrapControlUnit>   m_TrapControls;							 //!< control units for messages send by snmpgtrapd, passive control
		std::string				  		  	    m_LogFile;								 //!< writes all major events to logfile

	public:
		//////////////////////////////////
		//  CONSTRUCTION & DESTRUCTION  //
		//////////////////////////////////

		//! default constructor
		SnmpController()
		: m_StandardIntervallBetweenSnmpGets(),
		m_IsAtAlertLevel(),
		m_Sleep(),
		m_GetControls(),
		m_TrapControls(),
		m_LogFile()
		{}

		//! construct from data
		SnmpController( const double &STANDARD_TIME, const double &SLEEP_TIME, const std::string &LOGFILE = "/var/log/snmp_controller.log")
		: m_StandardIntervallBetweenSnmpGets( STANDARD_TIME),
		m_IsAtAlertLevel( false),
		m_Sleep( SLEEP_TIME),
		m_GetControls(),
		m_TrapControls(),
		m_LogFile( LOGFILE)
		{}

		//! copy constructor
		SnmpController( const SnmpController &ORIGINAL)
		: m_StandardIntervallBetweenSnmpGets( ORIGINAL.m_StandardIntervallBetweenSnmpGets),
		m_IsAtAlertLevel( ORIGINAL.m_IsAtAlertLevel),
		m_Sleep( ORIGINAL.m_Sleep),
		m_GetControls( ORIGINAL.m_GetControls),
		m_TrapControls( ORIGINAL.m_TrapControls),
		m_LogFile( ORIGINAL.m_LogFile)
		{}

		//! virtual destructor
		virtual ~SnmpController(){}

		//! virtual copy constructor
		virtual SnmpController *Clone() const{ return new SnmpController( *this);}


		/////////////////////////
		//      FUNCTIONS      //
		/////////////////////////

		//! return name of log file
		const std::string & GetLogFileName() const
		{
			return m_LogFile;
		}

		//! add control unit used for active monitoring of device properties
		void AddSnmpGetControlUnit
		(
				const std::string &IP,
				const std::string &OID,
				const std::string &COMMUNITY,
				const std::string &OUTPUT_FILE,
				const double &THRESHOLD,
				const std::string &CONDITION,
				const double &TIME_BEFORE_FIRST_SYS_CALL_EXECUTION,
				const double &TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS,
				const std::vector< std::string> &CALLS,
				const std::string &MESSAGE_NO_VALUE,
				const int &MESSAGE_INTERVAL
		)
		{
			m_GetControls.push_back( SnmpGetControlUnit( IP, OID, COMMUNITY, OUTPUT_FILE, THRESHOLD, CONDITION, TIME_BEFORE_FIRST_SYS_CALL_EXECUTION, TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS, CALLS, MESSAGE_NO_VALUE, MESSAGE_INTERVAL));
		}

		//! add control unit that handles traps send by the devices. the traps are caught by the snmptrapd and according to its config file the snmp_system_controller executable is called with essential information of the trap (major failure/restored)
		void AddSnmpTrapControlUnit
		(
				const short &DEVICE,
				const double &TIME_BEFORE_FIRST_SYS_CALL_EXECUTION,
				const double &TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS,
				const std::vector< std::string> &SYS_CALLS
		)
		{
			m_TrapControls.insert( std::make_pair( DEVICE, SnmpTrapControlUnit( TIME_BEFORE_FIRST_SYS_CALL_EXECUTION, TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS, SYS_CALLS)));
		}

		//! the main loop of the snmp system controller
		void Loop()
		{
			bool is_finished( false), shall_sleep( true);
			std::ofstream write;
			Message message;
			int check;
			time_t cycle_start, local;
			std::map< short, SnmpTrapControlUnit>::iterator trap_itr;

			// the main loop
			while( !is_finished)
			{
//				std::cout << "entering snmpget loop" << std::endl;
				// loop over controls
				for( std::vector< SnmpGetControlUnit>::iterator itr = m_GetControls.begin(); itr != m_GetControls.end(); ++itr)
				{
					// get device information
					itr->SendSnmpGet();
					// read and check value and react!
					itr->ReadAndEvaluateSnmpGetValue( m_IsAtAlertLevel, m_LogFile);
//						write.open( m_LogFile.c_str(), std::ios::out | std::ios::app);
//						time ( &local);
//						write << "get time ..." << std::ctime( &local) << std::endl;
//						write.close();
//						write.clear();
				}

				// sleep loop
				shall_sleep = true;
				time ( &cycle_start);
				time ( &local);
//				std::cout << "entering message-checking/sleeping loop" << std::endl;
				do
				{
//					std::cout << "check for messages: " << std::difftime( local, cycle_start) << "s after entering" << std::endl;
					check = msgrcv( m_MessageID, ( void*)&message, sizeof( Message) - sizeof( long), 0, IPC_NOWAIT); // only check once and continue

					if( check != -1)
					{
						write.open( m_LogFile.c_str(), std::ios::out | std::ios::app);

						time ( &local);

						switch( message.m_Command)
						{
						case snmp::TERMINATE:
							is_finished = true;
							write << "===> Terminate SnmpControllerDaemon: " << std::ctime( &local) << std::endl;
							break;
						case snmp::UTILITY_FAILURE:
							write << "utility failure signal received for: " << message.m_Device <<  std::endl;
							// set top level alert => no sleep and alternate sending snmpgets and checking for traps
							if( !m_IsAtAlertLevel)
							{
								m_IsAtAlertLevel = true;  // transition to alert-state
								write << "===> Transition to Alert state for device: " << message.m_Device << " at: " << std::ctime( &local) << std::endl;
							}

							// set control unit level to alert => start timer and eventually perform sys calls
							trap_itr = m_TrapControls.find( message.m_Device);
							assert( trap_itr != m_TrapControls.end());
							if( !trap_itr->second.IsAlertActive())
							{
								trap_itr->second.SetAlert();
							}
							trap_itr->second.EvaluateSnmpTrapAlert( m_LogFile);

							write << "===> Trap received for: " << message.m_Device << " at: " << std::ctime( &local) << std::endl;
							break;
						case snmp::UTILITY_RESTORED:
//							std::cout << "utility restored signal received for: " << message.m_Device << std::endl;

							// unset top level alert => sleeps next round
							if( m_IsAtAlertLevel)
							{
								m_IsAtAlertLevel = false;  // transition to alert-state
								write << "===> Transition to standard state" << std::ctime( &local) << std::endl;
							}

							// unset control unit alerts => no sys calls
							trap_itr = m_TrapControls.find( message.m_Device);
							assert( trap_itr != m_TrapControls.end());
							if( trap_itr->second.IsAlertActive())
							{
								trap_itr->second.UnsetAlert();
							}

							write << "===> Trap received: " << std::ctime( &local) << std::endl;
							break;
						}
						write.close();
						write.clear();
					}
					else if( !m_IsAtAlertLevel)
					{
						// actual sleep
						sleep( int( m_Sleep));
//						write.open( m_LogFile.c_str(), std::ios::out | std::ios::app);
//						time ( &local);
//						write << "nap time ... " << m_Sleep << "s at: " << std::ctime( &local) << std::endl;
//						write << "diff time:   " << std::difftime( local, cycle_start) << " of: " << m_StandardIntervallBetweenSnmpGets << "s" << std::endl;
//						write.close();
//						write.clear();
					}
					time ( &local);
				} while( !m_IsAtAlertLevel && check == -1 && std::difftime( local, cycle_start) <= m_StandardIntervallBetweenSnmpGets);

				if( m_IsAtAlertLevel)
				{
//					std::cout << "entering trap control loop" << std::endl;

					for( trap_itr = m_TrapControls.begin(); trap_itr != m_TrapControls.end(); ++trap_itr)
					{
						trap_itr->second.EvaluateSnmpTrapAlert( m_LogFile);
					}
				}
			}
		}

		//! sending messages to main/receiving process
		int SendMessage( const e_Command &COMMAND, const short DEVICE = std::numeric_limits< short>::max())
		{
			Message message( COMMAND, DEVICE);
			int check = msgsnd( m_MessageID, (void*) &message, sizeof( Message) - sizeof( long), 0);
			if( check == -1)
			{
				std::string message( "echo \"sending message (");
				if( COMMAND == UTILITY_FAILURE)
				{
					message += "UTILITY_FAILURE";
				}
				else if( COMMAND == UTILITY_RESTORED)
				{
					message += "UTILITY_RESTORED";
				}
				else if( COMMAND == TERMINATE)
				{
					message += "TERMINATE";
				}
				message += ") from device " + ConvertValueToString< short>( DEVICE) + "to daemon failed! | mail cluster-admin -s \"message from snmp_system_controller\"";

				std::system( message.c_str());
				std::cerr << "msgsnd failed" << std::endl;
				return -1;
			}
			return 0;
		}


		/////////////////////////
		//      Read/Write     //
		/////////////////////////

		virtual std::istream &Read( std::istream &STREAM)
		{
			return STREAM;
		}

		virtual std::ostream &Write( std::ostream &STREAM) const
		{
			STREAM << GetClassName() << std::endl;
			comm::Messenger::Write( STREAM);
			STREAM << "standard_time_between_snmp_gets: " << m_StandardIntervallBetweenSnmpGets << std::endl;
			STREAM << "sleep_time: " << m_Sleep << std::endl;
			STREAM << "is_at_alert_level: " << ((m_IsAtAlertLevel) ? "true":"false") << std::endl;
			STREAM << "log_file: " << m_LogFile << std::endl;
			STREAM << "nr_get_controls: " << m_GetControls.size() << std::endl;
			for( std::vector< SnmpGetControlUnit>::const_iterator itr = m_GetControls.begin(); itr != m_GetControls.end(); ++itr)
		    {
		      itr->Write( STREAM);
		    }
			STREAM << "nr_trap_controls: " << m_TrapControls.size() << std::endl;
			for( std::map< short, SnmpTrapControlUnit>::const_iterator itr = m_TrapControls.begin(); itr != m_TrapControls.end(); ++itr)
			{
				STREAM << "device_id: " << itr->first << " control_unit: ";
				itr->second.Write( STREAM);
				STREAM << std::endl;
			}
			return STREAM;
		}

		virtual std::string GetClassName() const
		{
			return mystr::GetClassName( __PRETTY_FUNCTION__);
		}

	}; // end class SnmpController
} // end namespace snmp




#endif /* SNMP_CONTROLLER_H_ */
