/*
   Unix SMB/CIFS implementation.
   Define trace codes for kernel debuggind facility.
   Copyright (C) 2007 Apple Inc. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _SMBKDEBUG_H_
#define _SMBKDEBUG_H_

#if defined(WITH_PROFILE) && defined(WITH_KDEBUG_TRACE)
#include <sys/kdebug.h>
#include <sys/syscall.h>

#ifndef DBG_APP_SAMBA
/* This should come from <sys/kdebug.h>, but we might be building against old
 * definitions.
 */
#define DBG_APP_SAMBA 128
#endif

/* This is ugly because, sys/kdebug.h is really intended for use in the kernel.
 * So we have junk like a declaration of kernel_debug, but no definition of it.
 */

#undef KERNEL_DEBUG
#define KERNEL_DEBUG(x,a,b,c,d,e)	\
do {					\
    if (kdebug_enable)			\
        sys_kernel_debug((unsigned int)x, (unsigned int)a, (unsigned int)b, \
		     (unsigned int)c, (unsigned int)d, (unsigned int)e); \
} while(0)

#define KDEBUG_TRACE_START(traceid) \
    KERNEL_DEBUG(APPSDBG_CODE(DBG_APP_SAMBA, traceid) | DBG_FUNC_START, \
	    0,0,0,0,0)

#define KDEBUG_TRACE_END(traceid) \
    KERNEL_DEBUG(APPSDBG_CODE(DBG_APP_SAMBA, traceid) | DBG_FUNC_END, \
	    0,0,0,0,0)

static inline void sys_kernel_debug(int code, int a1, int a2,
			    int a3, int a4, int a5)
{
	syscall(SYS_kdebug_trace, code, a1, a2, a3, a4, a5);
}

#undef kernel_debug

/* At one time, these values were derived from the PR_VALUE profile point
 * constants. However, they need to match the values in
 * /usr/share/misc/trace.codes, which is maintained out of tree. So we now
 * hardcode the trace IDs to guarantee their stability.
 *
 * Do not change the values of these defines, just add new ones to the end.
 */

#define kdebug_smbd_idle		0
#define kdebug_syscall_opendir		1
#define kdebug_syscall_readdir		2
#define kdebug_syscall_seekdir		3
#define kdebug_syscall_telldir		4
#define kdebug_syscall_rewinddir	5
#define kdebug_syscall_mkdir		6
#define kdebug_syscall_rmdir		7
#define kdebug_syscall_closedir		8
#define kdebug_syscall_open		9
#define kdebug_syscall_close		10
#define kdebug_syscall_read		11
#define kdebug_syscall_pread		12
#define kdebug_syscall_write		13
#define kdebug_syscall_pwrite		14
#define kdebug_syscall_lseek		15
#define kdebug_syscall_sendfile		16
#define kdebug_syscall_rename		17
#define kdebug_syscall_fsync		18
#define kdebug_syscall_stat		19
#define kdebug_syscall_fstat		20
#define kdebug_syscall_lstat		21
#define kdebug_syscall_unlink		22
#define kdebug_syscall_chmod		23
#define kdebug_syscall_fchmod		24
#define kdebug_syscall_chown		25
#define kdebug_syscall_fchown		26
#define kdebug_syscall_chdir		27
#define kdebug_syscall_getwd		28
#define kdebug_syscall_utime		29
#define kdebug_syscall_ftruncate	30
#define kdebug_syscall_fcntl_lock	31
#define kdebug_syscall_kernel_flock	32
#define kdebug_syscall_fcntl_getlock	33
#define kdebug_syscall_readlink		34
#define kdebug_syscall_symlink		35
#define kdebug_syscall_link		36
#define kdebug_syscall_mknod		37
#define kdebug_syscall_realpath		38
#define kdebug_syscall_get_quota	39
#define kdebug_syscall_set_quota	40
#define kdebug_SMBmkdir		41
#define kdebug_SMBrmdir		42
#define kdebug_SMBopen		43
#define kdebug_SMBcreate	44
#define kdebug_SMBclose		45
#define kdebug_SMBflush		46
#define kdebug_SMBunlink	47
#define kdebug_SMBmv		48
#define kdebug_SMBgetatr	49
#define kdebug_SMBsetatr	50
#define kdebug_SMBread		51
#define kdebug_SMBwrite		52
#define kdebug_SMBlock		53
#define kdebug_SMBunlock	54
#define kdebug_SMBctemp		55
#define kdebug_SMBmknew		56
#define kdebug_SMBcheckpath	57
#define kdebug_SMBexit		58
#define kdebug_SMBlseek		59
#define kdebug_SMBlockread		60
#define kdebug_SMBwriteunlock		61
#define kdebug_SMBreadbraw		62
#define kdebug_SMBreadBmpx		63
#define kdebug_SMBreadBs		64
#define kdebug_SMBwritebraw		65
#define kdebug_SMBwriteBmpx		66
#define kdebug_SMBwriteBs		67
#define kdebug_SMBwritec		68
#define kdebug_SMBsetattrE		69
#define kdebug_SMBgetattrE		70
#define kdebug_SMBlockingX		71
#define kdebug_SMBtrans			72
#define kdebug_SMBtranss		73
#define kdebug_SMBioctl			74
#define kdebug_SMBioctls		75
#define kdebug_SMBcopy			76
#define kdebug_SMBmove			77
#define kdebug_SMBecho			78
#define kdebug_SMBwriteclose		79
#define kdebug_SMBopenX			80
#define kdebug_SMBreadX			81
#define kdebug_SMBwriteX		82
#define kdebug_SMBtrans2		83
#define kdebug_SMBtranss2		84
#define kdebug_SMBfindclose		85
#define kdebug_SMBfindnclose		86
#define kdebug_SMBtcon			87
#define kdebug_SMBtdis			88
#define kdebug_SMBnegprot		89
#define kdebug_SMBsesssetupX		90
#define kdebug_SMBulogoffX		91
#define kdebug_SMBtconX			92
#define kdebug_SMBdskattr		93
#define kdebug_SMBsearch		94
#define kdebug_SMBffirst		95
#define kdebug_SMBfunique		96
#define kdebug_SMBfclose		97
#define kdebug_SMBnttrans		98
#define kdebug_SMBnttranss		99
#define kdebug_SMBntcreateX		100
#define kdebug_SMBntcancel		101
#define kdebug_SMBntrename		102
#define kdebug_SMBsplopen		103
#define kdebug_SMBsplwr			104
#define kdebug_SMBsplclose		105
#define kdebug_SMBsplretq		106
#define kdebug_SMBsends			107
#define kdebug_SMBsendb			108
#define kdebug_SMBfwdname		109
#define kdebug_SMBcancelf		110
#define kdebug_SMBgetmac		111
#define kdebug_SMBsendstrt		112
#define kdebug_SMBsendend		113
#define kdebug_SMBsendtxt		114
#define kdebug_SMBinvalid		115
#define kdebug_pathworks_setdir		116
#define kdebug_Trans2_open		117
#define kdebug_Trans2_findfirst		118
#define kdebug_Trans2_findnext		119
#define kdebug_Trans2_qfsinfo		120
#define kdebug_Trans2_setfsinfo		121
#define kdebug_Trans2_qpathinfo		122
#define kdebug_Trans2_setpathinfo	123
#define kdebug_Trans2_qfileinfo		124
#define kdebug_Trans2_setfileinfo	125
#define kdebug_Trans2_fsctl		126
#define kdebug_Trans2_ioctl		127
#define kdebug_Trans2_findnotifyfirst	128
#define kdebug_Trans2_findnotifynext	129
#define kdebug_Trans2_mkdir		130
#define kdebug_Trans2_session_setup		131
#define kdebug_Trans2_get_dfs_referral		132
#define kdebug_Trans2_report_dfs_inconsistancy	133
#define kdebug_NT_transact_create		134
#define kdebug_NT_transact_ioctl		135
#define kdebug_NT_transact_set_security_desc	136
#define kdebug_NT_transact_notify_change	137
#define kdebug_NT_transact_rename		138
#define kdebug_NT_transact_query_security_desc	139
#define kdebug_NT_transact_get_user_quota	140
#define kdebug_NT_transact_set_user_quota	141
#define kdebug_get_nt_acl		142
#define kdebug_fget_nt_acl		143
#define kdebug_set_nt_acl		144
#define kdebug_fset_nt_acl		145
#define kdebug_chmod_acl		146
#define kdebug_fchmod_acl		147
#define kdebug_name_release		148
#define kdebug_name_refresh		149
#define kdebug_name_registration	150
#define kdebug_node_status		151
#define kdebug_name_query		152
#define kdebug_host_announce		153
#define kdebug_workgroup_announce	154
#define kdebug_local_master_announce	155
#define kdebug_master_browser_announce	156
#define kdebug_lm_host_announce		157
#define kdebug_get_backup_list		158
#define kdebug_reset_browser		159
#define kdebug_announce_request		160
#define kdebug_lm_announce_request	161
#define kdebug_domain_logon		162
#define kdebug_sync_browse_lists	163
#define kdebug_run_elections		164
#define kdebug_election			165

#define kdebug_syscall_lchown 166 /* added by jra in rev 23105 */
#define kdebug_syscall_ntimes 167 /* added by jra in rev 21714 */
#define kdebug_syscall_linux_setlease 168 /* added by jmcd in rev 21324 */

/* XXX jpeach added chflags in rev 21757 and didn't update the profiling */

#endif /* WITH_PROFILE && defined(WITH_KDEBUG_TRACE) */

#endif /* _SMBKDEBUG_H_ */
