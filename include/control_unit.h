////////////////////////////////////////////////
//!  control_unit.h
//!
//!
//!
//!  @date Apr 6, 2009
//!  @author: Rene Staritzbichler
//!  @example:
////////////////////////////////////////////////

#ifndef CONTROL_UNIT_H_
#define CONTROL_UNIT_H_

#include <exception>


#include "../../tara/include/readwrite/stream_operator.h"
#include "../../tara/include/string/io_string_functions.h"
#include "../../tara/include/math/double_functions.h"


namespace snmp
{

	class SnmpTrapControlUnit
	{
	protected:
		bool                        m_IsActiveAlert;
		double                      m_TimeBeforeFirstSysCallExecution;
		double                      m_TimeBetweenFurtherSysCallExecutions;
		time_t                      m_AlertTime;
		std::vector< std::string>   m_SystemCalls;
		bool 						m_IsFirstAlert;
	public:

		SnmpTrapControlUnit
		(
				const double &TIME_BEFORE_FIRST_SYS_CALL_EXECUTION,
				const double &TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS,
				const std::vector< std::string> &SYS_CALLS
		)
		: m_IsActiveAlert(false),
		m_TimeBeforeFirstSysCallExecution( TIME_BEFORE_FIRST_SYS_CALL_EXECUTION),
		m_TimeBetweenFurtherSysCallExecutions( TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS),
		m_AlertTime(),
		m_SystemCalls( SYS_CALLS),
		m_IsFirstAlert( true)
		{}

		SnmpTrapControlUnit( const SnmpTrapControlUnit &ORIGINAL)
		: m_IsActiveAlert(ORIGINAL.m_IsActiveAlert),
		m_TimeBeforeFirstSysCallExecution( ORIGINAL.m_TimeBeforeFirstSysCallExecution),
		m_TimeBetweenFurtherSysCallExecutions( ORIGINAL.m_TimeBetweenFurtherSysCallExecutions),
		m_AlertTime( ORIGINAL.m_AlertTime),
		m_SystemCalls( ORIGINAL.m_SystemCalls),
		m_IsFirstAlert( ORIGINAL.m_IsFirstAlert)
		{}

		void SetAlert()
		{
			m_IsActiveAlert = true;
			time( &m_AlertTime);
		}

		bool IsAlertActive()
		{
			return m_IsActiveAlert;
		}

		void UnsetAlert()
		{
			m_IsActiveAlert = false;
			m_IsFirstAlert = true;
		}

		void EvaluateSnmpTrapAlert(  const std::string &LOGFILE)
		{
			std::ofstream write;
			write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
			if( !m_IsActiveAlert)
			{
//				write << "why to evaluate inactive alert? check!!" << std::endl;
				return;
			}
			time_t now;
			std::time( &now);
			if( ( m_IsFirstAlert && std::difftime( now, m_AlertTime) > m_TimeBeforeFirstSysCallExecution) || ( !m_IsFirstAlert && std::difftime( now, m_AlertTime) > m_TimeBetweenFurtherSysCallExecutions))
			{
				write << "====> SYS CALLS NOW:  " << std::ctime( &now) << std::endl;
				for( std::vector< std::string>::const_iterator itr = m_SystemCalls.begin(); itr != m_SystemCalls.end(); ++itr)
				{
					write << *itr << std::endl;
					std::system( itr->c_str());
				}
				std::time( &m_AlertTime);
				m_IsFirstAlert = false;
			}
			write.close();
			write.clear();
		}

		std::ostream &Write( std::ostream &STREAM) const
		{
//			STREAM << GetClassName() << std::endl;
			STREAM << "is_active_alert: ";
			STREAM.setf( std::ios_base::boolalpha);
			STREAM << m_IsActiveAlert << std::endl;
			STREAM << "is_first_alert: ";
			STREAM.setf( std::ios_base::boolalpha);
			STREAM << m_IsFirstAlert << std::endl;
			STREAM << "time_before_first_execution: " << m_TimeBeforeFirstSysCallExecution << std::endl;
			STREAM << "time_between_further_execution: " << m_TimeBetweenFurtherSysCallExecutions << std::endl;
			STREAM << "nr_sys_calls: " << m_SystemCalls.size() << std::endl;
			for( std::vector< std::string>::const_iterator itr = m_SystemCalls.begin(); itr != m_SystemCalls.end(); ++itr)
		    {
		      STREAM << *itr << std::endl;
		    }
			return STREAM;
		}

	};


	class SnmpGetControlUnit
	{
	protected:
		///////////////
		//	DATA     //
		///////////////
//		typedef bool ( SnmpGetControlUnit::*FunctionPointer)( const double &X, const double &Y) const;   // function pointer
		typedef bool ( *FunctionPointer)( const double &X, const double &Y);   // function pointer

		std::string   			                         m_IP;
		std::string			  		         m_OID;
		std::string						 m_Community;
		std::string						 m_InAndOutputFile;
		double				  			 m_Threshold;
		FunctionPointer					         m_ConditionalFunction;
		double				   			 m_TimeBeforeFirstSysCallExecution;
		double				   			 m_TimeBetweenFurtherSysCallExecutions;
		time_t				   			 m_AlertTime;
		std::vector< std::string>		                  m_SystemCalls;
		std::string						 m_UnavailableMessage;
		int                                                      m_MessageInterval;
		bool							 m_IsInAlarmState;
		bool 				   		         m_IsFirstAlert;
		time_t                                                   m_MessageTimer;

	public:
		//////////////////////////////////
		//  CONSTRUCTION & DESTRUCTION  //
		//////////////////////////////////

		//! default constructor
		SnmpGetControlUnit
		()
		:  m_IP(),
		m_OID(),
		m_Community(),
		m_InAndOutputFile(),
		m_Threshold(),
		m_ConditionalFunction(),
		m_TimeBeforeFirstSysCallExecution(),
		m_TimeBetweenFurtherSysCallExecutions(),
		m_SystemCalls(),
		m_UnavailableMessage(),
		m_IsInAlarmState(),
		    m_MessageInterval(),
		    m_IsFirstAlert( true),
		    m_MessageTimer()
		  { /*std::cout << __FUNCTION__ << " default" << std::endl;*/}

		//! construct from data
		SnmpGetControlUnit
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
				const std::string &MESSAGE_IF_INFO_UNAVAILABLE,
				const int &INTERVAL
		)
		:  m_IP( IP),
		m_OID( OID),
		m_Community( COMMUNITY),
		m_InAndOutputFile( OUTPUT_FILE),
		m_Threshold( THRESHOLD),
		m_ConditionalFunction( GetFunctionPointer( CONDITION)),  // pick function only once
		m_TimeBeforeFirstSysCallExecution( TIME_BEFORE_FIRST_SYS_CALL_EXECUTION),
		m_TimeBetweenFurtherSysCallExecutions( TIME_BETWEEN_FURTHER_SYS_CALL_EXECUTIONS),
		m_AlertTime(),
		m_SystemCalls( CALLS),
		m_UnavailableMessage( MESSAGE_IF_INFO_UNAVAILABLE),
		    m_MessageInterval( INTERVAL),
		m_IsInAlarmState( false),
		m_IsFirstAlert( true),
		    m_MessageTimer()
		  {
		      time( &m_MessageTimer);
		      /*std::cout << __FUNCTION__ << " from data" << std::endl;*/
		  }

		//! copy constructor
		SnmpGetControlUnit( const SnmpGetControlUnit &ORIGINAL)
		: m_IP( ORIGINAL.m_IP),
		m_OID( ORIGINAL.m_OID),
		m_Community( ORIGINAL.m_Community),
		m_InAndOutputFile( ORIGINAL.m_InAndOutputFile),
		m_Threshold( ORIGINAL.m_Threshold),
		m_ConditionalFunction( ORIGINAL.m_ConditionalFunction),
		m_TimeBeforeFirstSysCallExecution( ORIGINAL.m_TimeBeforeFirstSysCallExecution),
		m_TimeBetweenFurtherSysCallExecutions( ORIGINAL.m_TimeBetweenFurtherSysCallExecutions),
		m_AlertTime( ORIGINAL.m_AlertTime),
		m_SystemCalls( ORIGINAL.m_SystemCalls),
		m_UnavailableMessage( ORIGINAL.m_UnavailableMessage),
		    m_MessageInterval( ORIGINAL.m_MessageInterval),
		m_IsInAlarmState( ORIGINAL.m_IsInAlarmState),
		m_IsFirstAlert( ORIGINAL.m_IsFirstAlert),
		    m_MessageTimer( ORIGINAL.m_MessageTimer)
		  { /*std::cout << __FUNCTION__ << " copy" << std::endl;*/}

		//! virtual destructor
		virtual ~SnmpGetControlUnit(){}

		//! virtual copy constructor
		virtual SnmpGetControlUnit *Clone() const{ return new SnmpGetControlUnit( *this);}

		/////////////////////////
		//     OPERATORS       //
		/////////////////////////

		/////////////////////////
		//      FUNCTIONS      //
		/////////////////////////
		void SendSnmpGet() const
		{
			std::string
				call( "snmpget -v1 -c ");
			call += m_Community + " ";
			call += m_IP + " ";
			call += m_OID +  " ";
			call += "> " + m_InAndOutputFile;
//			std::cout << "system( " << call << ")" << std::endl;
			std::system( call.c_str());
		}

		void ReadAndEvaluateSnmpGetValue( bool &IS_AT_ALERT_LEVEL, const std::string &LOGFILE)
		{
			bool
				is_there_trouble( false);
			std::ofstream
				write;
			std::string
				line,
				copy;
			std::fstream
				read( m_InAndOutputFile.c_str(), std::ios::binary|std::ios::in);
			size_t
				locator;
			time_t
				now;
			double
				value( std::numeric_limits< double>::max());
			// go to trouble state if file with value cannot be opened or is empty
			if( !read.is_open())
			{
			    time( &now);
				write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
				write << "====> " << m_InAndOutputFile << " not opened! " << ctime( &now) << std::endl;
				write.close();
				write.clear();
				is_there_trouble = true;
			}
			else
			{
				std::getline( read, line);
				if( line.length() == 0)
				{
				    time( &now);
					write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
					write << "====> " << m_InAndOutputFile << " nothing found: <" << line << "> " << ctime( &now) << std::endl;
					write.close();
					write.clear();
					is_there_trouble = true;
				}
			}
			//			std::cout << "answer on snmpget: <" << line << ">" << std::endl;
			read.close();
			read.clear();
			if( !is_there_trouble)
			{
				std::vector< std::string>
					vec( mystr::SplitString( line));
				if( mystr::IsNumerical( vec.back()))
				{
					value = mystr::ConvertStringToNumericalValue< double>( vec.back());
				}
				else  // if no numerical value is found, go to alarm state!
				{
					is_there_trouble = true;
					time( &now);
					write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
					write << "====> " << vec.back() << " is not numeric! " << ctime( &now) << std::endl;
					write.close();
					write.clear();
				}
			}
			//	std::cout << "value: " << value << std::endl;

			// send message if trap information could not be read
			if( is_there_trouble)
			{
			        time( &now);
				if( difftime( now, m_MessageTimer) > m_MessageInterval)
				{
				    std::system( m_UnavailableMessage.c_str());
				    time( &m_MessageTimer);
					write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
					write << "====> message send: " << ctime( &now) << std::endl;
					write.close();
					write.clear();
				}

			}

			// check for trouble
			if( (*m_ConditionalFunction)( value, m_Threshold))  // trouble message
			{

				write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
				time( &now);
				write << "alert condition fulfilled: " << value << " threshold: " << m_Threshold << " time: " << std::ctime( &now) <<std::endl;
				if( !m_IsInAlarmState)
				{
					IS_AT_ALERT_LEVEL = true;
					std::ctime( &m_AlertTime);
					m_IsInAlarmState = true;
					write << "===> Transition to alert state: " << std::ctime( &now) << std::endl;
				}
				if( ( m_IsFirstAlert && std::difftime( now, m_AlertTime) >= m_TimeBeforeFirstSysCallExecution) || ( !m_IsFirstAlert && std::difftime( now, m_AlertTime) > m_TimeBetweenFurtherSysCallExecutions))
				{
					write << "===> System calls executed: " << std::ctime( &now) << std::endl;
					for( std::vector< std::string>::const_iterator itr = m_SystemCalls.begin(); itr != m_SystemCalls.end(); ++itr)
					{
						copy = *itr;
						locator = copy.find( "$$VALUE$$");
						if( locator != std::string::npos)
						{
							write << "value inserted to system call string" << std::endl;
							copy.replace( locator, 9, mystr::NumericalValueToString< double>( value));
						}
						write << "call: <" << copy.c_str() << ">" << std::endl;
						std::system( copy.c_str());
					}
					time( &m_AlertTime);  // reset alert time after system calls for time delay of future calls
					write << "execution finished: " << std::ctime( &m_AlertTime) << std::endl;
					m_IsFirstAlert = false;
				}
				write.close();
				write.clear();
			}
			else // no trouble
			{
				if( m_IsInAlarmState) // relax if there was previously trouble
				{
					IS_AT_ALERT_LEVEL = false;
					m_IsInAlarmState = false;
					write.open( LOGFILE.c_str(), std::ios::out | std::ios::app);
					time( &now);
					write << "===> Transition to standard state: " << std::ctime( &now) << std::endl;
					write.close();
					write.clear();
					m_IsFirstAlert = true;
				}
			}

		}


		FunctionPointer GetFunctionPointer( const std::string &CONDITION) const
		{
			if( CONDITION == "LARGER")
			{
				return &math::IsLarger;
			}
			else if( CONDITION == "SMALLER")
			{
				return &math::IsSmaller;
			}
			else if( CONDITION == "NONEQUAL")
			{
				return &math::IsNonEqual;
			}
			else if( CONDITION == "EQUAL")
			{
				return &math::IsEqual;
			}
			else
			{
				throw std::exception();
			}
		}
		/////////////////////////
		//      Read/Write     //
		/////////////////////////

		virtual std::istream &Read( std::istream &STREAM)
		{
	       STREAM >> m_IP;
	       STREAM >> m_OID;
	       STREAM >> m_Community;
	       STREAM >> m_InAndOutputFile;
	       STREAM >> m_Threshold;
	       //	       STREAM >> m_ConditionalFunction;
	       STREAM >> m_TimeBeforeFirstSysCallExecution;
	       STREAM >> m_TimeBetweenFurtherSysCallExecutions;
	       STREAM >> m_SystemCalls;
	       STREAM >> m_IsInAlarmState;

			return STREAM;
		}

		virtual std::ostream &Write( std::ostream &STREAM) const
		{
			STREAM << GetClassName() << std::endl;
			STREAM << "IP: " << m_IP<< std::endl;
			STREAM << "OID: " << m_OID<< std::endl;
			STREAM << "community: " << m_Community<< std::endl;
			STREAM << "output_file: " << m_InAndOutputFile<< std::endl;
			STREAM << "threshold: " << m_Threshold<< std::endl;
//			STREAM << m_ConditionalFunction<< std::endl;
			STREAM << "reaction_delay: " << m_TimeBeforeFirstSysCallExecution<< std::endl;
			STREAM << "reaction_delay: " << m_TimeBetweenFurtherSysCallExecutions<< std::endl;
			STREAM << "system_calls: " << m_SystemCalls;
			STREAM << "is_in_alarm_state: ";
			STREAM.setf( std::ios_base::boolalpha);
			STREAM << m_IsInAlarmState << std::endl;
			return STREAM;
		}

		virtual std::string GetClassName() const
		{
			return mystr::GetClassName( __PRETTY_FUNCTION__);
		}

	}; // end class SnmpGetControlUnit
} // end namespace snmp




#endif /* CONTROL_UNIT_H_ */
