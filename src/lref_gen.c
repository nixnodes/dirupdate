/*
 * lref_dirlog.c
 *
 *  Created on: Dec 4, 2013
 *      Author: reboot
 */

#include "lref_gen.h"

#include <l_sb.h>
#include <m_comp.h>
#include <lref.h>
#include <str.h>
#include <x_f.h>

#include <stdio.h>
#include <time.h>
#include <unistd.h>

int
ref_to_val_generic(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  if (!strncmp(match, "nukestr", 7))
    {
      if (NUKESTR)
        {
          snprintf(output, max_size, NUKESTR, "");
        }
    }
  else if (!strncmp(match, "procid", 6))
    {
      snprintf(output, max_size, "%d", getpid());
    }
  else if (!strncmp(match, "ipc", 3))
    {
      snprintf(output, max_size, "%.8X", (uint32_t) SHM_IPC);
    }
  else if (!strncmp(match, "usroot", 6))
    {
      snprintf(output, max_size, "%s/%s/%s", GLROOT, FTPDATA,
      DEFPATH_USERS);
      remove_repeating_chars(output, 0x2F);
    }
  else if (!strncmp(match, "logroot", 7))
    {
      snprintf(output, max_size, "%s/%s/%s", GLROOT, FTPDATA,
      DEFPATH_LOGS);
      remove_repeating_chars(output, 0x2F);
    }
  else if (!strncmp(match, "memlimit", 8))
    {
      snprintf(output, max_size, "%llu", db_max_size);
    }
  else if (!strncmp(match, "curtime", 7))
    {
      snprintf(output, max_size, "%d", (int32_t) time(NULL));
    }
  else if (!strncmp(match, "q:", 2))
    {
      return rtv_q(&match[2], output, max_size);
    }
  else if (!strncmp(match, "exe", 3))
    {
      if (self_get_path(output))
        {
          strcp_s(output, max_size, "UNKNOWN");
        }
    }
  else if (!strncmp(match, "glroot", 6))
    {
      strcp_s(output, max_size, GLROOT);
    }
  else if (!strncmp(match, "siteroot", 8))
    {
      strcp_s(output, max_size, SITEROOT);
    }
  else if (!strncmp(match, "siterootn", 9))
    {
      strcp_s(output, max_size, SITEROOT_N);
    }
  else if (!strncmp(match, "ftpdata", 7))
    {
      strcp_s(output, max_size, FTPDATA);
    }
  else if (!strncmp(match, "logfile", 7))
    {
      strcp_s(output, max_size, LOGFILE);
    }
  else if (!strncmp(match, "imdbfile", 8))
    {
      strcp_s(output, max_size, IMDBLOG);
    }
  else if (!strncmp(match, "gamefile", 8))
    {
      strcp_s(output, max_size, GAMELOG);
    }
  else if (!strncmp(match, "tvragefile", 10))
    {
      strcp_s(output, max_size, TVLOG);
    }
  else if (!strncmp(match, "spec1", 5))
    {
      strcp_s(output, max_size, b_spec1);
    }
  else if (!strncmp(match, "glconf", 6))
    {
      strcp_s(output, max_size, GLCONF_I);
    }
  else
    {
      return 1;
    }
  return 0;
}


char *
dt_rval_generic_nukestr(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  if (NUKESTR)
    {
      snprintf(output, max_size, NUKESTR, "");
    }
  return output;
}

char *
dt_rval_generic_procid(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, getpid());
  return output;
}

char *
dt_rval_generic_ipc(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, getpid());
  return output;
}

char *
dt_rval_generic_usroot(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, "%s/%s/%s", GLROOT, FTPDATA,
  DEFPATH_USERS);
  remove_repeating_chars(output, 0x2F);
  return output;
}

char *
dt_rval_generic_logroot(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, "%s/%s/%s", GLROOT, FTPDATA,
  DEFPATH_LOGS);
  remove_repeating_chars(output, 0x2F);
  return output;
}

char *
dt_rval_generic_memlimit(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, db_max_size);
  return output;
}

char *
dt_rval_generic_curtime(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  snprintf(output, max_size, ((__d_drt_h ) mppd)->direc, (int32_t) time(NULL));
  return output;
}

char *
dt_rval_q(void *arg, char *match, char *output, size_t max_size, void *mppd)
{
  if (rtv_q(&match[2], output, max_size))
    {
      output[0] = 0x0;
    }
  return output;
}

char *
dt_rval_generic_exe(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  if (self_get_path(output))
    {
      strcp_s(output, max_size, "UNKNOWN");
    }
  return output;
}

char *
dt_rval_generic_glroot(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return GLROOT;
}

char *
dt_rval_generic_siteroot(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return SITEROOT;
}

char *
dt_rval_generic_siterootn(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return SITEROOT_N;
}

char *
dt_rval_generic_ftpdata(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return FTPDATA;
}

char *
dt_rval_generic_imdbfile(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return IMDBLOG;
}

char *
dt_rval_generic_tvfile(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return TVLOG;
}

char *
dt_rval_generic_gamefile(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return GAMELOG;
}

char *
dt_rval_generic_spec1(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return b_spec1;
}

char *
dt_rval_generic_glconf(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return GLCONF_I;
}

char *
dt_rval_generic_logfile(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return LOGFILE;
}

char *
dt_rval_generic_newline(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return MSG_NL;
}

char *
dt_rval_generic_tab(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  return MSG_TAB;
}
#define MSG_GENERIC_BS         ":"



void *
ref_to_val_lk_generic(void *arg, char *match, char *output, size_t max_size,
    void *mppd)
{
  if (!strncmp(match, "nukestr", 7))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_nukestr, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "procid", 6))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_procid, (__d_drt_h ) mppd,
          "%d");
    }
  else if (!strncmp(match, "ipc", 3))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_ipc, (__d_drt_h ) mppd,
          "%d");
    }
  else if (!strncmp(match, "usroot", 6))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_usroot, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "logroot", 7))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_logroot, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "memlimit", 8))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_memlimit,
          (__d_drt_h ) mppd, "%llu");
    }
  else if (!strncmp(match, "curtime", 7))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_curtime, (__d_drt_h ) mppd,
          "%d");
    }
  else if (!strncmp(match, "q:", 2))
    {
      return as_ref_to_val_lk(match, dt_rval_q, (__d_drt_h ) mppd, "%s");
    }

  else if (!strncmp(match, MSG_GENERIC_BS, 1))
    {
      switch (match[1])
        {
      case 0x6E:
        return as_ref_to_val_lk(match, dt_rval_generic_newline,
            (__d_drt_h ) mppd, "%s");
      case 0x74:
        return as_ref_to_val_lk(match, dt_rval_generic_tab, (__d_drt_h ) mppd,
            "%s");
      case 0x5E:
        return as_ref_to_val_lk(match, dt_rval_generic_newline,
            (__d_drt_h ) mppd, "%s");
      case 0x54:
        return as_ref_to_val_lk(match, dt_rval_generic_tab, (__d_drt_h ) mppd,
            "%s");
        }

    }
  else if (!strncmp(match, "exe", 3))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_exe, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "glroot", 6))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_glroot, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "siteroot", 8))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_siteroot,
          (__d_drt_h ) mppd, "%s");
    }
  else if (!strncmp(match, "siterootn", 9))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_siterootn,
          (__d_drt_h ) mppd, "%s");
    }
  else if (!strncmp(match, "ftpdata", 7))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_ftpdata, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "logfile", 7))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_logfile, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "imdbfile", 8))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_imdbfile,
          (__d_drt_h ) mppd, "%s");
    }
  else if (!strncmp(match, "gamefile", 8))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_gamefile,
          (__d_drt_h ) mppd, "%s");
    }
  else if (!strncmp(match, "tvragefile", 10))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_tvfile, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "spec1", 5))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_spec1, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "glconf", 6))
    {
      return as_ref_to_val_lk(match, dt_rval_generic_glconf, (__d_drt_h ) mppd,
          "%s");
    }
  else if (!strncmp(match, "?", 1))
    {
      return ref_to_val_af(arg, match, output, max_size, (__d_drt_h ) mppd);
    }

  return NULL;
}
