/*
 * log_op.c
 *
 *  Created on: Dec 4, 2013
 *      Author: reboot
 */

#include <glutil.h>
#include "config.h"
#include <t_glob.h>

#include "log_op.h"

#include <l_sb.h>
#include <m_general.h>
#include <omfp.h>
#include <memory_t.h>
#include <exec_t.h>
#include <str.h>
#include <l_error.h>
#include <m_string.h>
#include <m_lom.h>
#include <exech.h>
#include <x_f.h>
#include <log_io.h>
#include <misc.h>

#include <unistd.h>

char *_print_ptr = NULL, *_cl_print_ptr = NULL, *_print_ptr_post = NULL,
    *_print_ptr_pre = NULL;

char *
g_dgetf (char *str)
{
  if (!str)
    {
      return NULL;
    }
  if (!strncmp (str, "dirlog", 6))
    {
      return DIRLOG;
    }
  else if (!strncmp (str, "nukelog", 7))
    {
      return NUKELOG;
    }
  else if (!strncmp (str, "dupefile", 8))
    {
      return DUPEFILE;
    }
  else if (!strncmp (str, "lastonlog", 9))
    {
      return LASTONLOG;
    }
  else if (!strncmp (str, "oneliners", 9))
    {
      return ONELINERS;
    }
  else if (!strncmp (str, "imdb", 4))
    {
      return IMDBLOG;
    }
  else if (!strncmp (str, "oldimdb", 7))
    {
      return IMDBLOG_O;
    }
  else if (!strncmp (str, "game", 4))
    {
      return GAMELOG;
    }
  else if (!strncmp (str, "tvrage", 6))
    {
      return TVLOG;
    }
  else if (!strncmp (str, "ge1", 3))
    {
      return GE1LOG;
    }
  else if (!strncmp (str, "ge2", 3))
    {
      return GE2LOG;
    }
  else if (!strncmp (str, "ge3", 3))
    {
      return GE3LOG;
    }
  else if (!strncmp (str, "ge4", 3))
    {
      return GE4LOG;
    }
  else if (!strncmp (str, "sconf", 5))
    {
      return SCONFLOG;
    }
  else if (!strncmp (str, "gconf", 5))
    {
      return GCONFLOG;
    }
  else if (!strncmp (str, "altlog", 6))
    {
      return ALTLOG;
    }
  else if (!strncmp (str, "x", 1))
    {
      return XLOG;
    }
  return NULL;
}

int
data_backup_records (char *file)
{
  g_setjmp (0, "data_backup_records", NULL, NULL);
  int r;
  off_t r_sz;

  if (!file)
    {
      print_str ("ERROR: invalid log file\n");
      return -1;
    }

  if ((gfl & F_OPT_NOWRITE) || (gfl & F_OPT_NOBACKUP))
    {
      return 0;
    }

  if (file_exists (file))
    {
      if (gfl & F_OPT_VERBOSE3)
	{
	  print_str ("WARNING: BACKUP: %s: data file doesn't exist\n", file);
	}
      return 0;
    }

  if (!(r_sz = get_file_size (file)))
    {
      if ((gfl & F_OPT_VERBOSE))
	{
	  print_str ("WARNING: %s: refusing to backup 0-byte data file\n",
		     file);
	}
      return 0;
    }

  char buffer[PATH_MAX];

  snprintf (buffer, PATH_MAX, "%s.bk", file);

  if (gfl & F_OPT_VERBOSE2)
    {
      print_str ("NOTICE: %s: creating data backup: %s ..\n", file, buffer);
    }

  if ((r = (int) file_copy (file, buffer, "wb", F_FC_MSET_SRC)) < 1)
    {
      print_str ("ERROR: %s: [%d] failed to create backup %s\n", file, r,
		 buffer);
      return r;
    }
  if (gfl & F_OPT_VERBOSE)
    {
      print_str ("NOTICE: %s: created data backup: %s\n", file, buffer);
    }
  return 0;
}

static int
g_op_load_print_mech (__g_handle hdl, pmda print_mech, char *print_ptr)
{
  size_t pp_l = strlen (print_ptr);
  int r;

  if (!pp_l || print_ptr[0] == 0xA)
    {
      print_str ("ERROR: %s: empty print command\n", hdl->file);
      return 2010;
    }
  if (pp_l > MAX_EXEC_STR)
    {
      print_str ("ERROR: %s: print string too large\n", hdl->file);
      return 2004;
    }
  if ((r = g_compile_exech (print_mech, hdl, print_ptr)))
    {
      print_str ("ERROR: %s: [%d]: could not compile print string\n", hdl->file,
		 r);
      return 2009;
    }

  return 0;
}

static int
g_h_deepcp_mrr (void *source, void *dest, void *d_ptr)
{

  __g_match src_pmd = (__g_match) source;
  if ( src_pmd->flags & F_GM_IS_MOBJ)
    {
      pmda t_md = calloc(1, sizeof(mda));

      md_copy(src_pmd->next, t_md, sizeof(_g_match), g_h_deepcp_mrr);

      __g_match dest_pmd = (__g_match) d_ptr;

      dest_pmd->next = t_md;
    }

  return 0;
}

static void
g_determine_output (__g_handle hdl, uint64_t *gfl0, uint64_t f, __d_is_wb *w_d)
{
  if ((*gfl0 & f))
    {
#ifdef _G_SSYS_NET
      if (hdl->flags & F_GH_W_NSSYS)
	{
	  *w_d = g_omfp_q_nssys;
	}
      else
	{
#endif
	  if (*gfl0 & F_OPT_PRINTF)
	    {
	      if (hdl->flags & F_GH_PRINT0)
		{
		  *w_d = g_omfp_write0;
		}
	      else
		{
		  *w_d = g_omfp_write;
		}
	    }
	  else
	    {
	      *w_d = g_omfp_write;
	    }
#ifdef _G_SSYS_NET
	}
#endif
    }
  else
    {
#ifdef _G_SSYS_NET
      if (hdl->flags & F_GH_W_NSSYS)
	{
	  *w_d = g_omfp_q_nssys_nl;
	}
      else
	{
#endif
	  *w_d = g_omfp_write_nl;
#ifdef _G_SSYS_NET
	}
#endif
    }
}

static int
g_populate_htref (__g_handle hdl)
{
  p_md_obj ptr = md_first (&hdl->buffer);

  while (ptr)
    {
      _d_drt_h dtr =
	{ 0 };

      dtr.hdl = hdl;

      __g_proc_v fp_rval = hdl->g_proc1_lookup (ptr->ptr, hdl->ht_field,
						hdl->mv1_b, sizeof(hdl->mv1_b),
						&dtr);

      if (NULL == fp_rval)
	{
	  print_str ("ERROR: g_populate_htref: could not resolve: '%s'\n",
		     hdl->ht_field);
	  return 1;
	}

      char *r_v = fp_rval (ptr->ptr, hdl->ht_field, hdl->mv1_b,
			   sizeof(hdl->mv1_b), &dtr);

      if (NULL == r_v)
	{
	  goto fin;
	}

      size_t l = strlen (r_v) + 1;
      if (1 == l)
	{
	  goto fin;
	}

      pmda shell = (pmda) ht_get (hdl->ht_ref, (unsigned char*) r_v, l);

      if ( NULL == shell)
	{
	  shell = calloc (1, sizeof(mda));
	  md_init (shell, 4);
	  shell->flags |= F_MDA_REFPTR;
	  ht_set (hdl->ht_ref, (unsigned char*) r_v, l, shell, sizeof(pmda));
	}

      shell->lref_ptr = ptr;
      md_alloc (shell, 0);

      fin: ;

      ptr = ptr->next;

    }

  return 0;
}

int
g_proc_mr (__g_handle hdl)
{
  g_setjmp (0, "g_proc_mr", NULL, NULL);
  int r;

  if (!(gfl & F_OPT_PROCREV))
    {
      hdl->j_offset = 1;
      if (hdl->buffer.count)
	{
	  hdl->buffer.r_pos = md_first (&hdl->buffer);
	}
    }
  else
    {
      if (hdl->buffer.count)
	{
	  hdl->buffer.r_pos = md_last (&hdl->buffer);
	}
      hdl->j_offset = 2;
    }

  if (!(hdl->flags & F_GH_HASMATCHES) && _match_rr.count > 0)
    {
      if ((r = md_copy (&_match_rr, &hdl->_match_rr, sizeof(_g_match),
			g_h_deepcp_mrr)))
	{
	  print_str ("ERROR: %s: md_copy(_match_rr, handle) failed\n",
		     hdl->file);
	  return 2000;
	}
      if (hdl->_match_rr.offset)
	{
	  if ((gfl & F_OPT_VERBOSE4))
	    {
	      print_str ("NOTICE: %s: commit %llu matches to handle\n",
			 hdl->file, (ulint64_t) hdl->_match_rr.offset);
	    }
	  hdl->flags |= F_GH_HASMATCHES;
	}

    }

  if (gfl & F_OPT_HASMAXHIT)
    {
      hdl->max_hits = max_hits;
      hdl->flags |= F_GH_HASMAXHIT;
    }

  if (gfl & F_OPT_HASMAXRES)
    {
      hdl->max_results = max_results;
      hdl->flags |= F_GH_HASMAXRES;
    }

  if (gfl & F_OPT_IFIRSTRES)
    {
      hdl->flags |= F_GH_IFRES;
    }

  if (gfl & F_OPT_IFIRSTHIT)
    {
      hdl->flags |= F_GH_IFHIT;
    }

  if (!(gfl & F_OPT_IFRH_E))
    {
      hdl->ifrh_l0 = g_ipcbm;
    }
  else
    {
      hdl->ifrh_l1 = g_ipcbm;
    }

  if (gfl0 & F_OPT_PRINT0)
    {
      hdl->flags |= F_GH_PRINT0;
    }

  if ((gfl & F_OPT_HAS_G_REGEX) || (gfl & F_OPT_HAS_G_MATCH)
      || (gfl & F_OPT_HAS_G_FNAME))
    {
      if ((r = g_load_strm (hdl)))
	{
	  return r;
	}
    }

  if ((gfl & F_OPT_HAS_G_LOM))
    {
      if ((r = g_load_lom (hdl)))
	{
	  return r;
	}
    }

  if ((exec_v || exec_str))
    {
      if (!(hdl->flags & F_GH_HASEXC))
	{
	  hdl->exec_args.exc = exc;

	  if (!hdl->exec_args.exc)
	    {
	      print_str (
		  "ERROR: %s: no exec call pointer (this is probably a bug)\n",
		  hdl->file);
	      return 2002;
	    }
	  hdl->flags |= F_GH_HASEXC;
	}
      int r;
      if (exec_v && !hdl->exec_args.argv)
	{
	  hdl->exec_args.argv = exec_v;
	  hdl->exec_args.argc = exec_vc;

	  if (!hdl->exec_args.argc)
	    {
	      print_str ("ERROR: %s: no exec arguments\n", hdl->file);
	      return 2001;
	    }

	  if ((r = g_build_argv_c (hdl)))
	    {
	      print_str ("ERROR: %s: [%d]: failed building exec arguments\n",
			 hdl->file, r);
	      return 2005;
	    }

	  if ((r = find_absolute_path (hdl->exec_args.argv_c[0],
				       hdl->exec_args.exec_v_path)))
	    {
	      if (gfl & F_OPT_VERBOSE2)
		{
		  print_str (
		      "WARNING: %s: [%d]: exec unable to get absolute path\n",
		      hdl->file, r);
		}
	      snprintf (hdl->exec_args.exec_v_path, PATH_MAX, "%s",
			hdl->exec_args.argv_c[0]);
	    }
	}
      else if (!hdl->exec_args.mech.offset)
	{
	  if ((r = g_compile_exech (&hdl->exec_args.mech, hdl, exec_str)))
	    {
	      print_str ("ERROR: %s: [%d]: could not compile exec string\n",
			 hdl->file, r);
	      return 2008;
	    }
	}
    }

  if (gfl & F_OPT_MODE_RAWDUMP)
    {
      hdl->g_proc4 = g_omfp_raw;
      hdl->flags |= F_GH_PRINT;
#ifdef HAVE_ZLIB_H
      if ((hdl->flags & F_GH_IO_GZIP) && NULL == hdl->gz_fh1 &&
	  hdl->gz_fh1 = gzdopen(fd_out, hdl->w_mode)) == NULL)
	{
	  return 2015;
	}
#endif
    }
  else if (gfl & F_OPT_FORMAT_BATCH)
    {
      hdl->g_proc3 = hdl->g_proc3_batch;
    }
  else if ((gfl0 & F_OPT_PRINT) || (gfl0 & F_OPT_PRINTF))
    {
      if (!hdl->print_mech.offset && _print_ptr)
	{
	  if ((r = g_op_load_print_mech (hdl, &hdl->print_mech, _print_ptr)))
	    {
	      return r;
	    }
	}

      hdl->g_proc4 = g_omfp_eassemble;

      g_determine_output (hdl, &gfl0, F_OPT_PRINTF, &hdl->w_d);

      hdl->act_mech = &hdl->print_mech;

      hdl->flags |= F_GH_PRINT;
    }
  else if ((hdl->flags & F_GH_ISONLINE) && (gfl & F_OPT_FORMAT_COMP))
    {
      hdl->g_proc4 = g_omfp_ocomp;
      hdl->g_proc3 = hdl->g_proc3_extra;
      print_str (
	  "+-------------------------------------------------------------------------------------------------------------------------------------------\n"
	  "|                     USER/HOST/PID                       |    TIME ONLINE     |    TRANSFER RATE      |        STATUS                      \n"
	  "|---------------------------------------------------------|--------------------|-----------------------|------------------------------------\n");
    }
  else if (gfl & F_OPT_FORMAT_EXPORT)
    {
      hdl->g_proc3 = hdl->g_proc3_export;
    }

  if ((gfl0 & (F_OPT_POSTPRINT | F_OPT_POSTPRINTF)))
    {
      if (!hdl->post_print_mech.offset && _print_ptr_post)
	{
	  if ((r = g_op_load_print_mech (hdl, &hdl->post_print_mech,
					 _print_ptr_post)))
	    {
	      return r;
	    }
	  hdl->flags |= F_GH_POST_PRINT;
	}

      hdl->g_proc4_po = g_omfp_eassemble;

      g_determine_output (hdl, &gfl0, F_OPT_POSTPRINTF, &hdl->w_d_po);

    }

  if ((gfl0 & (F_OPT_PREPRINT | F_OPT_PREPRINTF)))
    {
      if (!hdl->pre_print_mech.offset && _print_ptr_pre)
	{
	  if ((r = g_op_load_print_mech (hdl, &hdl->pre_print_mech,
					 _print_ptr_pre)))
	    {
	      return r;
	    }
	  hdl->flags |= F_GH_PRE_PRINT;
	}

      hdl->g_proc4_pr = g_omfp_eassemble;

      g_determine_output (hdl, &gfl0, F_OPT_PREPRINTF, &hdl->w_d_pr);

    }

  hdl->t_rw = 1;

  if ( NULL == hdl->v_b0)
    {
      hdl->v_b0 = (void*) b_glob;
      hdl->v_b0_sz = sizeof(b_glob) - 4;
    }

  if (NULL == hdl->execv_wpid_fp)
    {
      hdl->execv_wpid_fp = l_waitpid_def;
    }

  if ((gfl0 & F_OPT_MAKEHT) && !(gfl0 & F_OPT_NOHT) && hdl->ht_ref == NULL)
    {
      if (0 == hdl->buffer.offset)
	{
	  print_str ("ERROR: %s: could not determine hashtable size\n",
		     hdl->file);
	  return 2089;
	}

      size_t max = (size_t) hdl->buffer.offset / 4;

      if (max == 0)
	{
	  max = 1;
	}

      hdl->ht_ref = ht_create (max);
      hdl->ht_field = ht_field;

      print_str ("DEBUG: %s: generating hashtable\n", hdl->file);

      if (g_populate_htref (hdl))
	{
	  return 2088;
	}
    }

  return 0;
}

int
determine_datatype (__g_handle hdl, char *file)
{
#ifndef _MAKE_SBIN
  if (!strncmp (file, DIRLOG, strlen (DIRLOG)))
    {
      pdt_set_dirlog (hdl);
    }
  else if (!strncmp (file, NUKELOG, strlen (NUKELOG)))
    {
      pdt_set_nukelog (hdl);
    }
  else if (!strncmp (file, DUPEFILE, strlen (DUPEFILE)))
    {
      pdt_set_dupefile (hdl);
    }
  else if (!strncmp (file, LASTONLOG, strlen (LASTONLOG)))
    {
      pdt_set_lastonlog (hdl);
    }
  else if (!strncmp (file, ONELINERS, strlen (ONELINERS)))
    {
      pdt_set_oneliners (hdl);
    }
  else if (!strncmp (file, IMDBLOG, strlen (IMDBLOG)))
    {
      pdt_set_imdb (hdl);
    }
  else if (!strncmp (file, IMDBLOG_O, strlen (IMDBLOG_O)))
    {
      pdt_set_imdb_o (hdl);
    }
  else if (!strncmp (file, GAMELOG, strlen (GAMELOG)))
    {
      pdt_set_game (hdl);
    }
  else if (!strncmp (file, TVLOG, strlen (TVLOG)))
    {
      pdt_set_tvrage (hdl);
    }
  else if (!strncmp (file, GE1LOG, strlen (GE1LOG)))
    {
      pdt_set_gen1 (hdl);
    }
  else if (!strncmp (file, GE2LOG, strlen (GE2LOG)))
    {
      pdt_set_gen2 (hdl);
    }
  else if (!strncmp (file, GE3LOG, strlen (GE3LOG)))
    {
      pdt_set_gen3 (hdl);
    }
  else if (!strncmp (file, GE4LOG, strlen (GE4LOG)))
    {
      pdt_set_gen4 (hdl);
    }
  else if (!strncmp (file, SCONFLOG, strlen (SCONFLOG)))
    {
      pdt_set_sconf (hdl);
    }
  else if (!strncmp (file, GCONFLOG, strlen (GCONFLOG)))
    {
      pdt_set_gconf (hdl);
    }
  else if (!strncmp (file, ALTLOG, strlen (ALTLOG)))
    {
      pdt_set_altlog (hdl);
    }
  else if (!strncmp (file, XLOG, strlen (XLOG)))
    {
      pdt_set_x (hdl);
    }
  else
    {
      return 1;
    }
#endif

  return 0;
}

int
d_gen_dump (char *arg)
{
  if (NULL == arg)
    {
      print_str ("ERROR: "MSG_GEN_MISSING_DTARG" (-q <log>)\n");
      return 1;
    }

  char *datafile = g_dgetf (arg);

  if (!datafile)
    {
      print_str (MSG_UNRECOGNIZED_DATA_TYPE, arg);
      return 2;
    }

  if (!strncmp (datafile, "stdin", 5))
    {
      gfl0 |= F_OPT_STDIN;
    }

  return g_print_stats (datafile, 0, 0);
}

int
rebuild (void *arg)
{
  g_setjmp (0, "rebuild", NULL, NULL);
  if (NULL == arg)
    {
      print_str ("ERROR: "MSG_GEN_MISSING_DTARG" (-e <log>)\n");
      return 1;
    }

  char *a_ptr = (char*) arg;
  char *datafile = g_dgetf (a_ptr);

  if (!datafile)
    {
      print_str (MSG_UNRECOGNIZED_DATA_TYPE, a_ptr);
      return 2;
    }

  if (g_fopen (datafile, "r", F_DL_FOPEN_BUFFER, &g_act_1))
    {
      return 3;
    }

  if (!g_act_1.buffer.count)
    {
      print_str (
	  "ERROR: data log rebuilding requires buffering, increase mem limit (or dump with --raw --nobuffer for huge files)\n");
      return 4;
    }

  int r;

  if ((r = rebuild_data_file (datafile, &g_act_1)))
    {
      print_str (MSG_GEN_DFRFAIL, datafile);
      return 5;
    }

  if (((gfl0 & F_OPT_STATS) || (gfl & F_OPT_VERBOSE4))
      && !(gfl0 & F_OPT_NOSTATS))
    {
#ifdef HAVE_ZLIB_H
      if (g_act_1.flags & F_GH_IO_GZIP)
	{
	  fprintf(stderr, MSG_GEN_WROTE2, datafile,
	      (double) get_file_size(datafile) / 1024.0,
	      (double) g_act_1.bw / 1024.0,
	      (long long unsigned int) g_act_1.rw);
	}
      else
	{
	  OPLOG_OUTPUT_NSTATS(datafile, g_act_1)
	}
#else
      OPLOG_OUTPUT_NSTATS(datafile, g_act_1)
#endif
    }

  /*if ((gfl & F_OPT_NOFQ) && !(g_act_1.flags & F_GH_APFILT))
   {
   return 6;
   }*/

  return 0;
}
