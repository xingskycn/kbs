EXTRA_DIST = fb2kstruct.h README
bin_PROGRAMS = convpasswd chlevel  convboards calc_effsize \
	gen_title strip_fh chfavbrd
EXTRA_PROGRAMS = 
LDADD=-lBBS -lsystem -L$(top_builddir)/libBBS -L$(top_builddir)/libsystem
AM_CFLAGS=-DINVISIBLE -D_DETAIL_UINFO_ -DSHOW_IDLE_TIME -I$(top_builddir)/src
CLEANFILES=@EXTRA_CLEAN@
