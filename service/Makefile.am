EXTRA_DIST = definekey.* ansihelp.txt
SUBDIRS=pip worker personaldns
noinst_HEADERS = tcplib.h
lib_LTLIBRARIES = libfriendtest.la libbbsnet.la libpandora.la libdict.la libquiz.la \
                  libkiller.la libkiller2.la libcalendar.la libcalc.la libansieditor.la \
                  libiquery.la libtt.la libtetris.la libwinmine.la libsnake.la libball.la
libfriendtest_la_SOURCES = friendtest.c
libbbsnet_la_SOURCES = bbsnet.c tcplib.c
libpandora_la_SOURCES = pandora.c tcplib.c
libdict_la_SOURCES = dict.c
libquiz_la_SOURCES = quiz.c
libkiller_la_SOURCES = killer.c
libkiller2_la_SOURCES = killer2.c
libcalendar_la_SOURCES = calendar.c
libcalc_la_SOURCES = calc.c
libansieditor_la_SOURCES = ansieditor.c
libiquery_la_SOURCES = iquery.c
libtt_la_SOURCES = tt.c
libtetris_la_SOURCES = tetris.c
libwinmine_la_SOURCES = winmine.c
libball_la_SOURCES = ball.c
libsnake_la_SOURCES = snake.c
libdir=${prefix}/service
AM_CFLAGS = -DBBSMAIN -I$(top_srcdir)/src
CLEANFILES=@EXTRA_CLEAN@
install-data-local:
	cp -f $(srcdir)/definekey.* $(srcdir)/ansihelp.txt $(libdir)
