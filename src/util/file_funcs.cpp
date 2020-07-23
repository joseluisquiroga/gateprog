

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

file_funcs.cpp  

file funcs.

--------------------------------------------------------------*/

#include <errno.h>
#include <fcntl.h>
#include <fstream>

#include "file_funcs.h"

void
read_file(gp_string f_nam, row<char>& f_data){
	const char* ff_nn = f_nam.c_str();
	std::ifstream istm;
	istm.open(ff_nn, std::ios::binary);
	if(! istm.good() || ! istm.is_open()){
		throw file_exception(flx_cannot_open, f_nam);
	}

	// get size of file:
	istm.seekg (0, std::ios::end);
	long file_sz = istm.tellg();
	istm.seekg (0, std::ios::beg);

	if(file_sz < 0){
		throw file_exception(flx_cannot_calc_size, f_nam);
	}

	GP_CK(sizeof(char) == 1);

	char* file_content = tpl_malloc<char>(file_sz + 1); // leave room for END_OF_SEC
	istm.read(file_content, file_sz);
	long num_read = istm.gcount();
	if(num_read != file_sz){
		tpl_free<char>(file_content, file_sz + 1);

		throw file_exception(flx_cannot_fit_in_mem, f_nam);
	}
	file_content[file_sz] = END_OF_SEC;

	s_row<char> tmp_rr;
	tmp_rr.init_data((gp_byte*)file_content, file_sz + 1);

	f_data.clear();
	tmp_rr.move_to(f_data);

	GP_CK(f_data.last() == END_OF_SEC);
	f_data.pop(); // read as writed but leave room for END_OF_SEC
}

bool
write_file(gp_string& the_pth, row<char>& dat, bool write_once){
	int fd = 0;
	
	if(write_once){
		if((fd = open(the_pth.c_str(), O_RDWR|O_CREAT|O_EXCL, 0444)) == -1){
			return false;
		}
	} else {
		if((fd = creat(the_pth.c_str(), 0777)) == -1){
			return false;
		}
	}

	const char* the_data = dat.get_c_array();
	long the_sz = dat.size();

	write(fd, the_data, the_sz);
	close(fd);

	return true;
}

gp_string
get_fstr(gp_string f_nam){
	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDONLY, 0744);
	if(fd == -1){
		return "-1";
	}

	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return "-2";
		} else {
			close(fd);
			return "-3";
		}
	}

	gp_string the_val_str = "";

	off_t pos0 = 0;
	off_t pos1 = lseek(fd, 0, SEEK_END);
	if((pos1 != -1) && (pos1 != 0)){
		pos0 = lseek(fd, 0, SEEK_SET);
		if(pos0 != 0){
			close(fd);
			return "-4";
		}

		char* pt_str = (char*)malloc(pos1 + 1);
		ssize_t nr = read(fd, pt_str, pos1);
		if(nr != pos1){
			close(fd);
			return "-5";
		}
		pt_str[pos1] = 0;

		the_val_str = pt_str;
		free(pt_str);
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return "-6";
	}

	close(fd);
	return the_val_str;
}

long
set_fstr(gp_string f_nam, gp_string the_val_str){
	int fd;
	struct flock fl;

	//SKELETON_CK(not_skl_path(f_nam));
	fd = open(f_nam.c_str(), O_RDWR|O_CREAT, 0744);
	if(fd == -1){
		return -1;
	}

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;	// the whole file

	if(fcntl(fd, F_SETLK, &fl) == -1) {
		if(errno == EACCES || errno == EAGAIN) {
			close(fd);
			return -2;
		} else {
			close(fd);
			return -3;
		}
	}

	off_t pos0 = lseek(fd, 0, SEEK_SET);
	if(pos0 != 0){
		close(fd);
		return -4;
	}

	pos0 = ftruncate(fd, 0);
	if(pos0 != 0){
		close(fd);
		return -5;
	}

	off_t pos2 = the_val_str.size();
	ssize_t nw = write(fd, the_val_str.c_str(), pos2);
	if(nw != pos2){
		close(fd);
		return -6;
	}

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if(fcntl(fd, F_SETLK, &fl) == -1){
		close(fd);
		return -7;
	}

	close(fd);
	return 0;
}

gp_string
get_nftw_flag_str(long ff){
	gp_string out_str = "?ERROR?";
	switch(ff){
	case FTW_F:		out_str = "FTW_F";		break;
	case FTW_D:		out_str = "FTW_D";		break;
	case FTW_DNR:		out_str = "FTW_DNR";		break;
	case FTW_NS:		out_str = "FTW_NS";		break;
	case FTW_SL:		out_str = "FTW_SL";		break;
	case FTW_SLN:		out_str = "FTW_SLN";		break;
	case FTW_DP:		out_str = "FTW_DP";		break;
	};
	return out_str;
}

gp_string
get_errno_str(long val_errno){
	gp_string out_str = "?ERROR?";
	switch(val_errno){
	case EACCES:		out_str = "EACCES";		break;
	case EBUSY:		out_str = "EBUSY";		break;
	case EFAULT:		out_str = "EFAULT";		break;
	case EINVAL:		out_str = "EINVAL";		break;
	case EISDIR:		out_str = "EISDIR";		break;
	case ELOOP:		out_str = "ELOOP";		break;
	case EMLINK:		out_str = "EMLINK";		break;
	case ENAMETOOLONG:	out_str = "ENAMETOOLONG";	break;
	case ENOENT:		out_str = "ENOENT";		break;
	case ENOMEM:		out_str = "ENOMEM";		break;
	case ENOSPC:		out_str = "ENOSPC";		break;
	case ENOTDIR:		out_str = "ENOTDIR";		break;
	case ENOTEMPTY:		out_str = "ENOTEMPTY";		break;
	case EEXIST:		out_str = "EEXIST";		break;
	case EPERM:		out_str = "EPERM";		break;
	case EROFS:		out_str = "EROFS";		break;
	case EXDEV:		out_str = "EXDEV";		break;
	case EIO:		out_str = "EIO";		break;
	};
	return out_str;
}

int
delete_dir_entry(const char *fpath, const struct stat *sb,
			int tflag, struct FTW *ftwbuf)
{
	GP_MARK_USED(sb);
	GP_MARK_USED(ftwbuf);

	switch (tflag) {
	case FTW_D:
	case FTW_DNR:
	case FTW_DP:
		{
			int rr1 = rmdir(fpath);
			GP_MARK_USED(rr1);
			GP_DBG_COND_COMM((rr1 != 0), os << "failed deleting dir \n" << fpath << "\n"
				<< " errno=" << errno << " == '" << get_errno_str(errno) << "'\n"
				<< " tflag=" << tflag << " " << get_nftw_flag_str(tflag));
			FILE_FN_CK((rr1 == 0) || (errno == ENOTEMPTY));
		}
		break;
	default:
		{
			int rr2 = unlink(fpath);
			GP_MARK_USED(rr2);
			GP_DBG_COND_COMM((rr2 != 0), os << "failed deleting file \n" << fpath << "\n" 
				<< " errno=" << errno << " == '" << get_errno_str(errno) << "'\n"
				<< " tflag=" << tflag << " " << get_nftw_flag_str(tflag));
			FILE_FN_CK((rr2 == 0) || (errno == ENOTEMPTY));
		}
		break;
	}
	return (0);
}

void
delete_directory(gp_string& dir_nm){
	long max_depth = GP_FILE_MAX_PATH_DEPTH;

	str_pos_t lst_pos = dir_nm.size() - 1;
	if(dir_nm[lst_pos] == '/'){
		dir_nm = dir_nm.substr(0, lst_pos);
	}

	nftw(dir_nm.c_str(), delete_dir_entry, max_depth, FTW_DEPTH | FTW_PHYS);
}

bool 
file_touch(gp_string the_pth){
	//SKELETON_CK(not_skl_path(the_pth));
	int ok1 = utimensat(AT_FDCWD, the_pth.c_str(), NULL_PT, 0);
	return (ok1 == 0);
}

bool
path_is_dead_lock(gp_string the_pth){
	//SKELETON_CK(not_skl_path(the_pth));
	gp_string posfix = GP_LOCK_NAME;
	long idx = the_pth.size() - posfix.size();
	if(idx < 0){ return false; }
	
	gp_string endstr = the_pth.substr(idx);
	if(endstr != posfix){ return false; }

	time_t tm1 = time(NULL_PT);
	if(tm1 == -1){ return false; }

	struct stat sf1;

	int resp1 = stat(the_pth.c_str(), &sf1);
	bool ok1 = (resp1 == 0);

	double dtm = difftime(tm1, sf1.st_mtime);

	bool is_dead = ok1 && (dtm >= GP_SECS_DEAD_LOCK);
	return is_dead;
}

int
get_file_write_lock(gp_string full_nm){
	int fd_lock = open(full_nm.c_str(), O_RDWR|O_CREAT|O_EXCL, 0444);
	return fd_lock;
}

void
drop_file_write_lock(int fd_lock){
	close(fd_lock);
}

bool
file_exists(gp_string th_pth){
	/*
	std::ifstream istm;
	//SKELETON_CK(not_skl_path(th_pth));
	istm.open(th_pth.c_str(), std::ios_base::in);
	return istm.good();
	*/
	const char* fname = th_pth.c_str();
	
	bool ff_exists = (access(fname, F_OK) != -1);
	return ff_exists;
}

bool
file_newer_than(gp_string the_pth, time_t tm1){
	//SKELETON_CK(not_skl_path(the_pth));
	struct stat sf1;

	int resp1 = stat(the_pth.c_str(), &sf1);
	bool ok1 = (resp1 == 0);

	double dtm = difftime(sf1.st_mtime, tm1);

	bool nwr_than = ok1 && (dtm > 0);
	return nwr_than;
}

time_t
path_verify(gp_string the_pth){
	time_t vrf_tm = -1;
	struct stat sf1;
	int ok1 = stat(the_pth.c_str(), &sf1);
	if(ok1 == 0){
		vrf_tm = sf1.st_mtime;
	} 
	return vrf_tm;
}

bool
make_dir(gp_string the_pth, mode_t mod){
	int resp = mkdir(the_pth.c_str(), mod);
	return (resp == 0);
}

gp_string
path_get_running_path(){
	char exepath[GP_PATH_MAX] = {0};
	readlink("/proc/self/exe", exepath, sizeof(exepath) - 1);
	gp_string the_pth = exepath;
	return the_pth;
}

gp_string
path_to_absolute_path(gp_string pth){
	if(pth.size() >= (GP_PATH_MAX - 1)){
		throw file_exception(flx_path_too_long, pth);
	}
	
	char rpath[GP_PATH_MAX];

	char* rr = realpath(pth.c_str(), rpath);
	if(rr == rpath){ 
		gp_string real_pth = rpath;
		return real_pth;
	}
	return pth;
}

bool
path_create(gp_string n_pth){
	//SKELETON_CK(not_skl_path(n_pth));
	
	int eos = (int)gp_string::npos;
	int pos1 = n_pth.find('/');
	bool path_ok = true;
	while((pos1 == eos) || (pos1 < (int)n_pth.size())){
		if(pos1 == eos){
			pos1 = (int)n_pth.size();
		}

		gp_string nm_dir = n_pth.substr(0, pos1);
		if(nm_dir.size() > 0){
			path_ok = make_dir(nm_dir, 0700);
			/*
			resp = mkdir(nm_dir.c_str(), 0700);
			//path_ok = ((resp == 0) || (errno == EEXIST));
			path_ok = (resp == 0);
			*/
		}

		if((pos1 + 1) < (int)n_pth.size()){
			pos1 = n_pth.find('/', pos1 + 1);
		} else {
			pos1 = (int)n_pth.size();
		}
	}

	return path_ok;
}

bool
path_begins_with(gp_string the_pth, gp_string the_beg){
	if(the_pth.size() < the_beg.size()){ 
		return false; 
	}

	gp_string pref_str = the_pth.substr(0, the_beg.size());
	if(pref_str != the_beg){ 
		return false; 
	}
	return true;
}

bool
path_ends_with(gp_string& the_str, gp_string& the_suf){
	if(the_str.size() < the_suf.size()){
		return false;
	}
	str_pos_t pos1 = the_str.size() - the_suf.size();
	gp_string sub_str = the_str.substr(pos1);
	if(sub_str == the_suf){
		return true;
	}
	return false;
}

gp_string
path_get_directory(gp_string the_pth, bool add_last_dir_sep){
	long pos = (long)the_pth.rfind('/');
	gp_string the_dir = the_pth.substr(0, pos);
	if(add_last_dir_sep){
		the_dir = the_dir + '/';
	}
	return the_dir;
}

gp_string
path_get_name(gp_string the_pth){
	long eos = (long)gp_string::npos;
	long pos = (long)the_pth.rfind('/');
	if(pos == eos){
		return the_pth;
	}
	gp_string the_nm = the_pth.substr(pos + 1);
	return the_nm;
}

void
path_delete(gp_string full_pth, gp_string up_to){
	//SKELETON_CK(not_skl_path(full_pth));
	gp_string sub_pth = full_pth;

	if(! path_begins_with(full_pth, up_to)){
		GP_CK(false);
		return; 
	}

	delete_directory(sub_pth);

	str_pos_t pos_sep = 0;
	for(	pos_sep = sub_pth.rfind('/');
		(pos_sep < sub_pth.size());
		pos_sep = sub_pth.rfind('/'))
	{
		sub_pth = sub_pth.substr(0, pos_sep);

		if(sub_pth == up_to){ break; }
		int resp = rmdir(sub_pth.c_str());
		if(resp != 0){ break; }
	}
}

bool
path_create_link(gp_string old_pth, gp_string nw_pth){
	int ok = symlink(old_pth.c_str(), nw_pth.c_str());
	GP_MARK_USED(ok);
	GP_DBG_COND_COMM((ok != 0), 
		os << "CANNOT create link old_pth=" << old_pth << " nw_pth=" << nw_pth << " err=" 
			<< get_errno_str(errno) << "\n";
	);
	return (ok == 0);
}

bool
rename_file(gp_string& old_pth, gp_string& nw_pth){
	int ok = rename(old_pth.c_str(), nw_pth.c_str());
	return (ok == 0);
}

gp_string
get_relative_path(gp_string pth1, gp_string pth2){
	//gp_string abs1 = path_to_absolute_path(pth1);
	//gp_string abs2 = path_to_absolute_path(pth2);
	gp_string abs1 = pth1;
	gp_string abs2 = pth2;
	
	size_t cm_pos = 0;
	size_t nx_pos = 0;
	while(abs1.compare(0, nx_pos, abs2, 0, nx_pos) == 0){
		cm_pos = nx_pos;
		nx_pos = abs1.find_first_of('/', nx_pos + 1);
		if(nx_pos == std::string::npos){
			break;
		}
	}
	
	gp_string rel = "..";
	size_t up_pos = cm_pos;
	while((up_pos = abs1.find_first_of('/', up_pos + 1)) != std::string::npos){
		rel += "/..";
	}
	
	gp_string rest = abs2.substr(cm_pos);
	rel += rest;
	
	return rel;
}

/*
gp_string
path_slice(gp_string nam, row_long_t& sections){
	char sep = '/';
	std::stringstream frm;
	long min_sec_sz = 1;
	long pos1 = 0;
	long pos2 = 0;

	long nm_sz = nam.size();
	for(long aa = 0; aa < sections.size(); aa++){
		long vv = sections[aa];
		if(vv < min_sec_sz){
			vv = min_sec_sz;
		}
		if(pos1 >= nm_sz){
			break;
		}
		pos2 = pos1 + vv;
		if(pos2 > nm_sz){
			pos2 = nm_sz;
		}

		gp_string sec1 = nam.substr(pos1, vv);
		pos1 = pos2;

		frm << sec1 << sep;
	}
	if(pos2 < nm_sz){
		gp_string sec2 = nam.substr(pos2, nm_sz);
		frm << sec2 << sep;
	}
	frm.flush();
	return frm.str();
}

*/
