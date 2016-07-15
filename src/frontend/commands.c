/* NG-SPICE -- An electrical circuit simulator
 *
 * Copyright (c) 1990 University of California
 * Copyright (c) 2000 Arno W. Peters
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation without fee, and without a written agreement is
 * hereby granted, provided that the above copyright notice, this
 * paragraph and the following three paragraphs appear in all copies.
 *
 * This software program and documentation are copyrighted by their
 * authors. The software program and documentation are supplied "as
 * is", without any accompanying services from the authors. The
 * authors do not warrant that the operation of the program will be
 * uninterrupted or error-free. The end-user understands that the
 * program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. THE AUTHORS SPECIFICALLY DISCLAIMS ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
 * SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE AUTHORS
 * HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS. */

/* Table of available commands.  Note that they're sorted so that the
 * commands that appear in the spiceinit file are at the top.  */

#include "ngspice/ngspice.h"
#include "ngspice/ftedefs.h"
#include "ngspice/cpdefs.h"

#include "ftehelp.h"
#include "commands.h"

#include "com_ahelp.h"
#include "com_ghelp.h"
#include "com_asciiplot.h"
#include "com_compose.h"
#include "com_display.h"
#include "com_hardcopy.h"
#include "com_help.h"
#include "com_let.h"
#include "com_plot.h"
#include "com_setscale.h"
#include "com_xgraph.h"
#include "com_gnuplot.h"
#include "com_state.h"
#include "com_chdir.h"
#include "com_echo.h"
#include "com_rehash.h"
#include "com_shell.h"
#include "com_shift.h"
#include "com_unset.h"
#include "fourier.h"
#include "newcoms.h"
#include "define.h"
#include "com_set.h"
#include "misccoms.h"
#include "com_commands.h"
#include "runcoms.h"
#include "com_alias.h"
#include "typesdef.h"
#include "postcoms.h"
#include "com_option.h"
#include "inp.h"
#include "com_dump.h"
#include "com_fft.h"
#include "spec.h"
#include "runcoms2.h"
#include "breakp.h"
#include "breakp2.h"
#include "aspice.h"
#include "com_history.h"
#include "com_cdump.h"
#include "linear.h"
#include "where.h"
#include "mw_coms.h"
#include "resource.h"
#include "diff.h"
#include "com_strcmp.h"

#include "arg.h"

#include "spiceif.h" /* for com_snload() and com_snsave() */

#include "com_dl.h"

#ifdef XSPICE
/* gtri - begin - wbk - add include files */
#include "ngspice/evtproto.h"
/* gtri - end - wbk - add include files */
#endif

/* Information about spice commands (struct comm). */

// char *co_comname;  /* The name of the command. */
// void (*co_func) (wordlist *wl);  /* The function that handles the command. */
// bool co_spiceonly;  /* These can't be used from nutmeg. */
// bool co_major; /* Is this a "major" command? */
// long co_cctypes[4]; /* Bitmasks for command completion. */
// unsigned int co_env; /* print help message on this environment mask */
// int co_minargs; /* minimum number of arguments required */
// int co_maxargs; /* maximum number of arguments allowed */
// void (*co_argfn) (wordlist *wl, struct comm *command); /* The fn that prompts the user. */
// char *co_help;  /* When these are printed, printf(string, av[0]) .. */

struct comm spcp_coms[] = {
    { "let", com_let, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      arg_let,
      "varname = expr : Assign vector variables." } ,
    { "reshape", com_reshape, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      arg_let,
      "vector ... [ shape ] : change the dimensions of a vector." } ,
    { "define", com_define, FALSE, TRUE,
      { 010000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[[func (args)] stuff] : Define a user-definable function." } ,
    { "set", com_set, FALSE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a variable." } ,


/* support for altering options in interactive mode,
   using either command 'option' or 'options'*/
    { "option", com_option, TRUE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a simulator option." } ,
    { "options", com_option, TRUE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a simulator option." } ,
    { "snsave", com_snsave, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 1, 1,
      NULL,
      "file : Save a snapshot." } ,
    { "snload", com_snload, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 2, 2,
      NULL,
      "file : Load a snapshot." } ,
    { "circbyline", com_circbyline, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "line : Enter a circuit line." } ,
    { "alias", com_alias, FALSE, FALSE,
      { 02, 04, 04, 04 }, E_ADVANCED, 0, LOTS,
      NULL,
      "[[word] alias] : Define an alias." } ,
    { "deftype", com_dftype, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, LOTS,
      NULL,
      "spec name pat ... : Redefine vector and plot types.\n" } ,
#ifdef TCL_MODULE
    { "bltplot", com_bltplot, FALSE, TRUE,
    { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
    arg_plot,
    "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
    { "plot", com_bltplot, FALSE, TRUE,
       { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
       arg_plot,
       "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
#else
    { "plot", com_plot, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
      arg_plot,
      "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
#endif
    { "display", com_display, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 0, LOTS,
      arg_display,
      ": Display vector status." } ,
    { "destroy", com_destroy, FALSE, FALSE,
      { 0400, 0400, 0400, 0400 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[plotname] ... : Throw away all the data in the plot." } ,
    { "setplot", com_splot, FALSE, TRUE,
      { 0400, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[plotname] : Change the current working plot." } ,
    { "setcirc", com_scirc, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[circuit name] : Change the current circuit." } ,
    { "setscale", com_setscale, FALSE, FALSE,
      { 040000, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[vecname] : Change default scale of current working plot." } ,
    { "transpose", com_transpose, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Perform matrix transposition on multi-D vectors." } ,
    { "xgraph", com_xgraph, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "file plotargs : Send plot to Xgraph-11." } ,
    { "gnuplot", com_gnuplot, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "file plotargs : Send plot to gnuplot." } ,
    { "wrdata", com_write_simple, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 2, NLOTS,
      NULL,
      "file plotargs : Send plot data to file." } ,
    { "wrs2p", com_write_sparam, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file : Send s-param data to file." } ,
    { "hardcopy", com_hardcopy, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file plotargs : Produce hardcopy plots." } ,
    { "asciiplot", com_asciiplot, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "plotargs : Produce ascii plots." } ,
    { "write", com_write, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file expr ... : Write data to a file." } ,
    { "compose", com_compose, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "var parm=val ... : Compose a vector." } ,
    { "unlet", com_unlet, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Undefine vectors." } ,
    { "print", com_print, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      arg_print,
      "[col] expr ... : Print vector values." } ,
#ifdef XSPICE
    { "eprint", EVTprint, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      NULL,
      "node node ... : Print event values." } ,
    { "eprvcd", EVTprintvcd, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      NULL,
      "node node ... : Print event values into vcd file." },
    { "edisplay", EVTdisplay, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 0, 0,
      NULL,
      "node node ... : Print all event nodes." },
    { "codemodel", com_codemodel, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      NULL,
      "library library ... : Loads the code model libraries." } ,
#endif
#ifdef DEVLIB
    { "use", com_use, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      NULL,
      "library library ... : Loads the device libraries." } ,
#endif
    { "load", com_load, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_BEGINNING | E_NOPLOTS, 1, LOTS,
      arg_load,
      "file ... : Load in data." } ,
    { "cross", com_cross, FALSE, TRUE,
      { 040000, 0, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "vecname number [ vector ... ] : Make a vector in a strange way." } ,
    { "undefine", com_undefine, FALSE, FALSE,
      { 010000, 010000, 010000, 010000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[func ...] : Undefine a user-definable function." } ,
    { "op", com_op, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.op line args] : Determine the operating point of the circuit." } ,
    { "tf", com_tf, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.tran line args] : Do a transient analysis." } ,
    { "tran", com_tran, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.tran line args] : Do a transient analysis." } ,
#ifdef WITH_PSS
/* SP: Steady State Analysis */
    { "pss", com_pss, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.pss line args] : Do a periodic state analysis." } ,
/* SP */
#endif
    { "ac", com_ac, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.ac line args] : Do an ac analysis." } ,
    { "dc", com_dc, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.dc line args] : Do a dc analysis." } ,
    { "pz", com_pz, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.pz line args] : Do a pole / zero analysis." } ,
    { "sens", com_sens, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.sens line args] : Do a sensitivity analysis." } ,
    { "disto", com_disto, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.disto line args] : Do an distortion analysis." } ,
    { "noise", com_noise, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.noise line args] : Do a noise analysis." } ,
    { "listing", com_listing, TRUE, TRUE,
      { 0100, 0100, 0100, 0100 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[logical] [physical] [deck] : Print the current circuit." } ,
    { "edit", com_edit, TRUE, TRUE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[filename] : Edit a spice deck and then load it in." } ,
    { "mc_source", com_mc_source, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
        NULL,
        ": Re-source the actual circuit deck for MC simulation." },
    { "dump", com_dump, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print a dump of the current circuit." } ,
    { "fft", com_fft, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "vector ... : Create a frequency domain plot with FFT." } ,
    { "psd", com_psd, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "vector ... : Create a power spetral density plot with FFT." } ,
    { "fourier", com_fourier, FALSE, TRUE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "fund_freq vector ... : Do a fourier analysis of some data." } ,
    { "spec", com_spec, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 4, LOTS,
      NULL,
      "start_freq stop_freq step_freq vector ... : Create a frequency domain plot." } ,
    { "meas", com_meas, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "various ... : User defined signal evaluation." } ,
    { "show", com_show, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "devices ... : parameters ... : Print out device summary." } ,
    { "showmod", com_showmod, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "models ... : parameters ... : Print out model summary." } ,
    { "sysinfo", com_sysinfo, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "Print out system info summary." } ,
    { "alter", com_alter, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "devspecs : parmname value : Alter device parameters." } ,
    { "altermod", com_altermod, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "devspecs : parmname value : Alter model parameters." } ,
    { "alterparam", com_alterparam, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "devspecs : parmname value : Alter .param parameters." },
    { "resume", com_resume, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Continue after a stop." } ,
    { "state", com_state, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "(unimplemented) : Print the state of the circuit." },
    { "stop", com_stop, TRUE, FALSE,
      { 04200, 04200, 04200, 04200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[stop args] : Set a breakpoint." } ,
    { "trace", com_trce, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Trace a node." } ,
    { "save", com_save, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Save a spice output." } ,
    { "iplot", com_iplot, TRUE, TRUE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Incrementally plot a node." } ,
    { "status", com_sttus, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print the current breakpoints and traces." } ,
    { "delete", com_delete, TRUE, FALSE,
      { 020, 020, 020, 020 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [break number ...] : Delete breakpoints and traces." } ,
    { "step", com_step, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[number] : Iterate number times, or one." } ,
    { "remcirc", com_remcirc, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Remove current citcuit." } ,
    { "reset", com_rset, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Terminate a simulation after a breakpoint (formerly 'end')." } ,
    { "run", com_run, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[rawfile] : Run the simulation as specified in the input file." } ,
    { "aspice", com_aspice, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, 2,
      NULL,
      "file [outfile] : Run a spice job asynchronously." } ,
    { "jobs", com_jobs, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Report on asynchronous spice jobs." } ,
    { "rspice", com_rspice, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[input file] : Run a spice job remotely." } ,
    { "bug", com_bug, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Report a %s bug." } ,
    { "where", com_where, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print last non-converging node or device" } ,
    { "newhelp", com_ahelp, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[command name] ... : help." },
    { "tutorial", com_ghelp, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_BEGINNING, 0, LOTS,
      NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "help", com_ghelp, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "oldhelp", com_help, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[command name] ... : Print help." } ,
      /* to remove circuits loaded */
    { "removecirc", com_removecirc, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[circuit name] : Remove the current circuit from memory." } ,
    { "quit", com_quit, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_BEGINNING, 0, 1,
      NULL,
      ": Quit %s." } ,
    { "source", com_source, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "file : Source a %s file." } ,
    { "shift", com_shift, FALSE, FALSE,
      { 020000, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      NULL,
      "[var] [number] : Shift argv or the named list var to the left." } ,
    { "unset", com_unset, FALSE, FALSE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Unset a variable." } ,
    { "unalias", com_unalias, FALSE, FALSE,
      { 02, 02, 02, 02 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "word ... : Undefine an alias." } ,
    { "history", com_history, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      NULL,
      "[-r] [number] : Print command history." } ,
    { "echo", com_echo, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, NLOTS,
      NULL,
      "[stuff ...] : Print stuff." } ,
    { "shell", com_shell, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[args] : Fork a shell, or execute the command." } ,
    { "rusage", com_rusage, FALSE, FALSE,
      { 02000, 02000, 02000, 02000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[resource ...] : Print current resource usage." } ,
    { "cd", com_chdir, FALSE, FALSE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[directory] : Change working directory." } ,
    { "version", com_version, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[number] : Print the version number." } ,
    { "diff", com_diff, FALSE, FALSE,
      { 0400, 0400, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "plotname plotname [vec ...] : 'diff' two plots." } ,
    { "rehash", com_rehash, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Rebuild the unix command database." } ,
    { "while", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "repeat", NULL, FALSE, FALSE,
      { 0, 0, 0, 0}, E_DEFHMASK, 0, 1,
      NULL,
      "[number] : Repeat number times, or forever." } ,
    { "dowhile", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "foreach", NULL, FALSE, FALSE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "variable value ... : Do once for each value." } ,
    { "if", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute if the condition is TRUE." } ,
    { "else", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Goes with if." } ,
    { "end", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": End a block." } ,
    { "break", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Break out of a block." } ,
    { "continue", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Continue a loop." } ,
    { "label", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      NULL,
      "word : Create someplace to go to." } ,
    { "goto", NULL, FALSE, FALSE,
      { 0100000, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      NULL,
      "word : Go to a label." } ,
    { "cdump", com_cdump, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Dump the current control structures." } ,
    { "mdump", com_mdump, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "outfile: Dump the current matrix." } ,
    { "mrdump", com_rdump, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "outfile: Dump the current RHS to file." } ,
    { "settype", com_stype, FALSE, FALSE,
      { 0200000, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "type vec ... : Change the type of a vector." } ,
    { "strcmp", com_strcmp, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, 3,
      NULL,
      "varname s1 s2 : Set $varname to strcmp(s1, s2)." } ,
    { "linearize", com_linearize, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      " [ vec ... ] : Convert plot into one with linear scale." } ,
    { "devhelp", com_devhelp, FALSE, FALSE,
      { 040000, 0400000, 040000, 040000 }, E_DEFHMASK, 0, 5 ,
      NULL,
      "devspecs : show device information." },
    { "inventory", com_inventory, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print circuit inventory" },
    { NULL, NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      NULL }
};


/* Bool fields:    spiceonly, major */
struct comm nutcp_coms[] = {
    { "let", com_let, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      arg_let,
      "varname = expr : Assign vector variables." } ,
    { "reshape", com_reshape, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      arg_let,
      "vector ... [ shape ] : change the dimensions of a vector." } ,
    { "define", com_define, FALSE, TRUE,
      { 010000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[[func (args)] stuff] : Define a user-definable function." } ,
    { "set", com_set, FALSE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a variable." } ,

#ifdef EXPERIMENTAL_CODE
/* PN support for altering options in interactive mode */
    { "option", com_option, TRUE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a simulator option." } ,
#endif

    { "alias", com_alias, FALSE, FALSE,
      { 02, 04, 04, 04 }, E_ADVANCED, 0, LOTS,
      NULL,
      "[[word] alias] : Define an alias." } ,
    { "deftype", com_dftype, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, LOTS,
      NULL,
      "spec name pat ... : Redefine vector and plot types.\n" } ,
    { "plot", com_plot, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
      arg_plot,
      "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
    { "display", com_display, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 0, LOTS,
      arg_display,
      ": Display vector status." } ,
    { "destroy", com_destroy, FALSE, FALSE,
      { 0400, 0400, 0400, 0400 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[plotname] ... : Throw away all the data in the plot." } ,
    { "setplot", com_splot, FALSE, TRUE,
      { 0400, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[plotname] : Change the current working plot." } ,
    { "setcirc", com_scirc, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[circuit name] : Change the current circuit." } ,
    { "setscale", com_setscale, FALSE, FALSE,
      { 040000, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[vecname] : Change default scale of current working plot." } ,
    { "transpose", com_transpose, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Perform matrix transposition on multi-D vectors." } ,
    { "xgraph", com_xgraph, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "file plotargs : Send plot to Xgraph-11." } ,
    { "gnuplot", com_gnuplot, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "file plotargs : Send plot to gnuplot." } ,
    { "wrdata", com_write_simple, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 2, NLOTS,
      NULL,
      "file plotargs : Send plot data to file." } ,
    { "wrs2p", com_write_sparam, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file : Send s-param data to file." } ,
    { "hardcopy", com_hardcopy, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file plotargs : Produce hardcopy plots." } ,
    { "asciiplot", com_asciiplot, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "plotargs : Produce ascii plots." } ,
    { "write", com_write, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "file expr ... : Write data to a file." } ,
    { "compose", com_compose, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "var parm=val ... : Compose a vector." } ,
    { "unlet", com_unlet, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Undefine vectors." } ,
    { "print", com_print, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      arg_print,
      "[col] expr ... : Print vector values." } ,
    { "load", com_load, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_BEGINNING | E_NOPLOTS, 1, LOTS,
      arg_load,
      "file ... : Load in data." } ,
    { "cross", com_cross, FALSE, TRUE,
      { 040000, 0, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "vecname number [ vector ... ] : Make a vector in a strange way." } ,
    { "undefine", com_undefine, FALSE, FALSE,
      { 010000, 010000, 010000, 010000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[func ...] : Undefine a user-definable function." } ,
    { "op", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.op line args] : Determine the operating point of the circuit." } ,
    { "tran", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.tran line args] : Do a transient analysis." } ,
#ifdef WITH_PSS
/* SP: Steady State Analysis */
    { "pss", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.pss line args] : Do a periodic steady state analysis." } ,
/* SP */
#endif
    { "ac", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.ac line args] : Do an ac analysis." } ,
    { "dc", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.dc line args] : Do a dc analysis." } ,
    { "pz", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.pz line args] : Do a pole / zero analysis." } ,
    { "sens", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.ac line args] : Do a sensitivity analysis." } ,
    { "disto", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.disto line args] : Do an distortion analysis." } ,
    { "noise", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[.noise line args] : Do a noise analysis." } ,
    { "listing", NULL, TRUE, TRUE,
      { 0100, 0100, 0100, 0100 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[logical] [physical] [deck] : Print the current circuit." } ,
    { "edit", NULL, TRUE, TRUE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[filename] : Edit a spice deck and then load it in." } ,
    { "dump", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print a dump of the current circuit." } ,
    { "fft", com_fft, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "vector ... : Create a frequency domain plot with FFT." } ,
    { "psd", com_psd, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "vector ... : Create a power spetral density plot with FFT." } ,
    { "fourier", com_fourier, FALSE, TRUE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "fund_freq vector ... : Do a fourier analysis of some data." } ,
    { "spec", com_spec, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 4, LOTS,
      NULL,
      "start_freq stop_freq step_freq vector ... : Create a frequency domain plot." } ,
    { "show", NULL, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "devspecs ... : parmspecs ... : Print out device parameters." } ,
    { "alter", NULL, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 3, LOTS,
      NULL,
      "devspecs : parmname value : Alter device parameters." } ,
    { "altermod", NULL, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 3, LOTS,
      NULL,
      "devspecs : parmname value : Alter model parameters." } ,
    { "resume", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Continue after a stop." } ,
    { "state", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "(unimplemented) : Print the state of the circuit." },
    { "stop", NULL, TRUE, FALSE,
      { 04200, 04200, 04200, 04200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[stop args] : Set a breakpoint." } ,
    { "trace", NULL, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Trace a node." } ,
    { "save", NULL, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Save a spice output." } ,
    { "iplot", NULL, TRUE, TRUE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [node ...] : Incrementally plot a node." } ,
    { "status", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print the current breakpoints and traces." } ,
    { "delete", NULL, TRUE, FALSE,
      { 020, 020, 020, 020 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[all] [break number ...] : Delete breakpoints and traces." } ,
    { "step", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[number] : Iterate number times, or one." } ,
    { "reset", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Terminate a simulation after a breakpoint (formerly 'end')." } ,
    { "run", NULL, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[rawfile] : Run the simulation as specified in the input file." } ,
    { "bug", com_bug, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Report a %s bug." } ,
    { "newhelp", com_ahelp, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[command name] ... : help." },
    { "tutorial", com_ghelp, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_BEGINNING, 0, LOTS,
      NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "help", com_ghelp, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "oldhelp", com_help, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[command name] ... : Print help." } ,
    { "quit", com_quit, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_BEGINNING, 0, 1,
      NULL,
      ": Quit %s." } ,
      /* to remove circuits loaded */
    { "removecirc", com_removecirc, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[circuit name] : Remove the current circuit from memory." } ,
    { "source", nutcom_source, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "file : Source a %s file." } ,
    { "shift", com_shift, FALSE, FALSE,
      { 020000, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      NULL,
      "[var] [number] : Shift argv or the named list var to the left." } ,
    { "unset", com_unset, FALSE, FALSE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "varname ... : Unset a variable." } ,
    { "unalias", com_unalias, FALSE, FALSE,
      { 02, 02, 02, 02 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "word ... : Undefine an alias." } ,
    { "history", com_history, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      NULL,
      "[-r] [number] : Print command history." } ,
    { "echo", com_echo, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[stuff ...] : Print stuff." } ,
    { "shell", com_shell, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[args] : Fork a shell, or execute the command." } ,
    { "rusage", com_rusage, FALSE, FALSE,
      { 02000, 02000, 02000, 02000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[resource ...] : Print current resource usage." } ,
    { "cd", com_chdir, FALSE, FALSE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      NULL,
      "[directory] : Change working directory." } ,
    { "version", com_version, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "[number] : Print the version number." } ,
    { "diff", com_diff, FALSE, FALSE,
      { 0400, 0400, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      "plotname plotname [vec ...] : 'diff' two plots." } ,
    { "rehash", com_rehash, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Rebuild the unix command database." } ,
    { "while", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "repeat", NULL, FALSE, FALSE,
      { 0, 0, 0, 0}, E_DEFHMASK, 0, 1,
      NULL,
      "[number] : Repeat number times, or forever." } ,
    { "dowhile", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "foreach", NULL, FALSE, FALSE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "variable value ... : Do once for each value." } ,
    { "if", NULL, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      NULL,
      "condition : Execute if the condition is TRUE." } ,
    { "else", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Goes with if." } ,
    { "end", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": End a block." } ,
    { "break", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Break out of a block." } ,
    { "continue", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Continue a loop." } ,
    { "label", NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      NULL,
      "word : Create someplace to go to." } ,
    { "goto", NULL, FALSE, FALSE,
      { 0100000, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      NULL,
      "word : Go to a label." } ,
    { "cdump", com_cdump, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Dump the current control structures." } ,
    { "settype", com_stype, FALSE, FALSE,
      { 0200000, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      NULL,
      "type vec ... : Change the type of a vector." } ,
    { "strcmp", com_strcmp, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, 3,
      NULL,
      "varname s1 s2 : Set $varname to strcmp(s1, s2)." } ,
    { "linearize", com_linearize, TRUE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      NULL,
      " [ vec ... ] : Convert plot into one with linear scale." } ,
    { "devhelp", NULL, FALSE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, 5 ,
      NULL,
      " devspecs : show device information." },
    { "inventory", NULL, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      NULL,
      ": Print circuit inventory" } ,
    { NULL, NULL, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      NULL,
      NULL }
};
