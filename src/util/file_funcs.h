

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
See https://github.com/joseluisquiroga/gateprog

gateprog is free software thanks to The Glory of Our Lord 
	Yashua Melej Hamashiaj.
Our Resurrected and Living, both in Body and Spirit, 
	Prince of Peace.

------------------------------------------------------------

file_funcs.h

file funcs.

--------------------------------------------------------------*/

#ifndef GP_FILE_FUNCS_H
#define GP_FILE_FUNCS_H

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#define _XOPEN_SOURCE 500	// used in <ftw.h> to define behaviour (see features.h)

#include <ftw.h>

#include "dbg_util.h"
#include "tools.h"
#include <unistd.h>

#define FILE_FN_CK(prm)		GP_CK(prm)
//define FILE_FN_CK_2(prm, comm)		DBG_CK_2(prm, comm)

#define GP_FILE_MAX_PATH_DEPTH 40
#define GP_PATH_MAX 500  // should be PATH_MAX as in limits.h

#define GP_LOCK_NAME	"lock.skl"
#define GP_SECS_DEAD_LOCK	10000

#define END_OF_SEC	0

typedef enum {
	flx_cannot_open,
	flx_cannot_calc_size,
	flx_cannot_fit_in_mem,
	flx_path_too_long
} fl_ex_cod_t;

class file_exception : public top_exception {
public:
	gp_string f_nm;
	file_exception(long the_id = 0, gp_string ff = "unknow_file") : top_exception(the_id)
	{
		f_nm = ff;
	}
};

void
read_file(gp_string f_nam, row<char>& f_data);

bool
write_file(gp_string& the_pth, row<char>& cnn, bool write_once);

bool
rename_file(gp_string& old_pth, gp_string& nw_pth);

// fnum = concurrent safe consec counter kept in file f_nam

gp_string
get_fstr(gp_string f_nam);

long
set_fstr(gp_string f_nam, gp_string the_val_str);

gp_string
get_nftw_flag_str(long ff);

gp_string
get_errno_str(long val_errno);

int
delete_dir_entry(const char *fpath, const struct stat *sb,
			int tflag, struct FTW *ftwbuf);

void
delete_directory(gp_string& dir_nm);

bool 
file_touch(gp_string the_pth);

int
get_file_write_lock(gp_string lk_dir);
	
void
drop_file_write_lock(int fd_lock);

bool
file_exists(gp_string th_pth);

bool
file_newer_than(gp_string the_pth, time_t tm1);

time_t
path_verify(gp_string the_pth);

bool
make_dir(gp_string the_pth, mode_t mod);

gp_string
path_get_running_path();

gp_string
path_to_absolute_path(gp_string pth);

bool
path_create(gp_string n_pth);

bool
path_begins_with(gp_string the_pth, gp_string the_beg);

bool
path_ends_with(gp_string& the_str, gp_string& the_suf);

void
path_delete(gp_string full_pth, gp_string up_to);

gp_string
path_get_directory(gp_string the_pth, bool add_last_dir_sep);

gp_string
path_get_name(gp_string the_pth);

bool
path_create_link(gp_string old_pth, gp_string new_pth);

gp_string
get_relative_path(gp_string pth1, gp_string pth2);

//gp_string
//path_slice(gp_string nam, row_long_t& sections);

#endif		// FILE_FUNCS_H


