////////////////////////////////////////////////
//!  snmp_trap.h
//!
//!
//!
//!  @date Apr 6, 2009
//!  @author: Rene Staritzbichler
//!  @example:
////////////////////////////////////////////////

#ifndef SNMP_TRAP_H_
#define SNMP_TRAP_H_

#include "../readwrite/stream_operator.h"
#include "../string/io_string_functions.h"



namespace snmp
{
	enum e_JobType { TRAP, TERMINATE};

	class SnmpTrap
	{
	protected:
		///////////////
		//	DATA     //
		///////////////
		long          m_ID;
		e_JobType     m_Type;
		std::string   m_IP;
		std::string   m_OID;
		std::string   m_Value;

	public:
		//////////////////////////////////
		//  CONSTRUCTION & DESTRUCTION  //
		//////////////////////////////////

		//! default constructor
		SnmpTrap()
		: PP(),
		m_Y()
		{}

		//! construct from data
		SnmpTrap( const data &DATA)
		: PP(),
		m_Y( DATA)
		{}

		//! copy constructor
		SnmpTrap( const SnmpTrap &ORIGINAL)
		: PP( ORIGINAL),
		m_Y( ORIGINAL.m_Y)
		{}

		//! virtual destructor
		virtual ~SnmpTrap(){}

		//! virtual copy constructor
		virtual SnmpTrap *Clone() const{ return new SnmpTrap( *this);}

		/////////////////////////
		//     OPERATORS       //
		/////////////////////////

		/////////////////////////
		//      FUNCTIONS      //
		/////////////////////////

		/////////////////////////
		//      Read/Write     //
		/////////////////////////

		virtual std::istream &Read( std::istream &STREAM)
		{
			return STREAM;
		}

		virtual std::ostream &Write( std::ostream &STREAM) const
		{
			return STREAM;
		}

		virtual std::string GetClassName() const
		{
			return mystr::GetClassName( __PRETTY_FUNCTION__);
		}

	}; // end class SnmpTrap
} // end namespace snmp




#endif /* SNMP_TRAP_H_ */
