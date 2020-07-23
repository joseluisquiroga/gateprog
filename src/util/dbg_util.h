

/*************************************************************

This file is part of gateprog.

gateprog is free software: you can redistribute it and/or modify
it under the terms of the version 3 of the GNU General Public 
License as published by the Free Software Foundation.

gateprog is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gateprog.  If not, see <http://www.gnu.org/licenses/>.

------------------------------------------------------------

Copyright (C) 2020. QUIROGA BELTRAN, Jose Luis.
Id (cedula): 79523732 de Bogota - Colombia.
See https://github.com/gateprog

gateprog is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

dbg_util.h

func to print a stack trace.

--------------------------------------------------------------*/

#ifndef GP_DBG_UTIL_H
#define GP_DBG_UTIL_H

#include <iostream>
#include <sstream>
#include <fstream>	// used for bj_ofstream
#include <string>

#define NULL_PT NULL
#define gp_null NULL

#define gp_byte uint8_t
#define gp_string std::string

typedef std::ostream gp_ostream;
typedef std::ostringstream gp_ostr_stream;
typedef std::ofstream gp_ofstream;
#define gp_eol std::endl
#define gp_out std::cout
#define gp_err std::cerr
#define gp_dbg std::cout

typedef gp_string::size_type str_pos_t;

template <bool> struct ILLEGAL_USE_OF_OBJECT;
template <> struct ILLEGAL_USE_OF_OBJECT<true>{};
#define OBJECT_COPY_ERROR ILLEGAL_USE_OF_OBJECT<false>()

#define gp_c_decl extern "C"

#define GP_STRINGIFY(x) #x
#define GP_TOSTRING(x) GP_STRINGIFY(x)

#define GP_MAX_STR_SZ 300

// -----------------

#define GP_MAX_CALL_STACK_SZ 100
#define GP_PRT_SIZE_T "%lu"

#define GP_ABORT_MSG(msg) gp_cstr("ABORTING. '" msg "' at " __FILE__ "(" GP_TOSTRING(__LINE__) ")")

#define GP_INFO_STR "Passed " __FILE__ "(" GP_TOSTRING(__LINE__) ")"

#ifdef __cplusplus
gp_c_decl {
#endif

#ifdef FULL_DEBUG
#	define GP_DBG(prm) prm
#else
#	define GP_DBG(prm) /**/ 
#endif

#define GP_MARK_USED(X)  ((void)(&(X)))

#ifdef __cplusplus
}
#endif

std::string gp_get_stack_trace( const std::string & file, int line );
void gp_ptr_call_stack_trace(FILE* out_fp);
void gp_abort_func(bool prt_stk = false);

#define GP_STACK_STR gp_get_stack_trace(__FILE__, __LINE__)

const char* gp_get_ptd_log_fnam();
bool gp_call_assert(char* out_fnam, bool is_assert, bool prt_stck, bool vv_ck, 
				const char* file, int line, const char* ck_str, const char* fmt, ...);

#define gp_abort(...) \
{ \
	fprintf(stderr, "\nABORTING.\n"); \
	gp_ptr_call_stack_trace(gp_null); \
	fprintf(stderr, __VA_ARGS__); \
	gp_abort_func(); \
} \

// end_define


//======================================================================
// top_exception

class top_exception {
private:
	top_exception&  operator = (top_exception& other){
		gp_abort("INVALID OPERATOR ON top_exception");
		return (*this);
	}
	
public:
	long		ex_id;
	gp_string 	ex_stk;
	gp_string 	ex_assrt;
	
	top_exception(long the_id = 0, gp_string assrt_str = ""){
		ex_id = the_id;
		ex_stk = GP_STACK_STR;
		ex_assrt = assrt_str;
	}
	
	~top_exception(){
	}
};

//======================================================================
// mem_exception

typedef enum {
	mex_memout_in_mem_alloc_1,
	mex_memout_in_mem_alloc_2,
	mex_memout_in_mem_sec_re_alloc_1,
	mex_memout_in_mem_re_alloc_1,
	mex_memout_in_mem_re_alloc_2
} mem_ex_cod_t;


class mem_exception : public top_exception {
public:
	mem_exception(long the_id = 0) : top_exception(the_id)
	{}
};


//======================================================================
// dbg macros

#define	GP_DBG_COND_COMM(cond, comm)	\
	GP_DBG( \
		if(cond){ \
			gp_ostream& os = gp_dbg; \
			comm; \
			os << '\n'; \
			os.flush(); \
		} \
	) \

//--end_of_def

#define GP_CK_2(prm, comms1) \
	GP_DBG_COND_COMM((! (prm)), \
		comms1; \
		os << '\n'; \
		os.flush(); \
		GP_CK(prm); \
	) \
	
//--end_of_def



#define GP_CODE(cod) cod
#define GP_DBG_CODE(cod) GP_DBG(cod)

#define GP_CK(vv) GP_DBG( \
	gp_call_assert(gp_null, true, true, vv, __FILE__, __LINE__, #vv, gp_null))

//	(! vv)?(gp_call_assert(gp_null, true, true, vv, __FILE__, __LINE__, #vv, __VA_ARGS__), 0):(0))
//	gp_call_assert(gp_null, true, true, vv, __FILE__, __LINE__, #vv, __VA_ARGS__))

#define GP_CK_PRT(vv, ...) GP_DBG( \
	(! vv)?(gp_call_assert(gp_null, true, true, vv, __FILE__, __LINE__, #vv, __VA_ARGS__), 0):(0))

#define GP_CK_LOG(vv, ...) GP_DBG( \
	gp_call_assert(gp_get_ptd_log_fnam(), true, true, vv, __FILE__, __LINE__, #vv, __VA_ARGS__))

#define GP_COND_LOG(cond, ...) GP_DBG( \
	gp_call_assert(gp_get_ptd_log_fnam(), false, false, cond, __FILE__, __LINE__, #cond, __VA_ARGS__))

#define GP_LOG(...) GP_COND_LOG(true, __VA_ARGS__)

#define GP_COND_PRT(cond, ...) GP_DBG( \
	gp_call_assert(gp_null, false, false, cond, __FILE__, __LINE__, #cond, __VA_ARGS__))

#define GP_PRT(...) GP_COND_PRT(true, __VA_ARGS__)

#define GP_PRINTF(...) GP_DBG(printf(__VA_ARGS__))

#define GP_PRT_STACK(cond, ...) GP_DBG( \
	gp_call_assert(gp_null, false, true, cond, __FILE__, __LINE__, #cond, __VA_ARGS__))

#define GP_LOG_STACK(cond, ...) GP_DBG( \
	gp_call_assert(gp_get_ptd_log_fnam(), false, true, cond, __FILE__, __LINE__, #cond, __VA_ARGS__))



#endif		// GP_DBG_UTIL_H


