/*
 * $Id: bbslib.h 10831 2011-08-14 16:10:38Z fancy $
 */
#if !defined BBSLIB_H
#define BBSLIB_H

#include "bbs.h"

typedef struct user_info uinfo_t;

typedef struct boardheader boardheader_t;
typedef struct friends friends_t;

void f_append(FILE *fp, char *buf);

int file_exist(char *file);

int del_mail(int ent, struct fileheader* fh, char* direct);

int send_msg(char *myuserid, int mypid, char *touserid, int topid, char msg[256]);

int isfriend(char *id);

//board manage
int del_post(int ent, struct fileheader *fileinfo, const struct boardheader *bh);

int www_user_login(struct userec *user, int useridx, int kick_multi, char *fromhost, char *fullfrom, struct user_info **ppuinfo, int *putmpent);
int www_user_logoff(struct userec *user, int useridx, struct user_info *puinfo, int userinfoidx);
int www_user_init(int useridx, char *userid, int key, struct userec **x, struct user_info **y, long compat_telnet);
int www_data_init();
void www_data_detach();
int is_BM(const struct boardheader *board,const struct userec *user);
char *unix_string(char *str);
void output_ansi_html(char *buf, size_t buflen, buffered_output_t *output,char* attachlink, int is_tex, char* preview_attach_dir);
#ifdef NFORUM
void output_ansi_html_noatt(char *buf, size_t buflen, buffered_output_t *output);
#endif
void print_font_style(unsigned int style, buffered_output_t * output);
void print_raw_ansi(char *buf, size_t buflen, buffered_output_t * output);
void generate_font_style(unsigned int *style, unsigned int *ansi_val, size_t len);

struct WWW_GUEST_S* www_get_guest_entry(int idx);

#define DEBUG_WAIT \
    do {\
        struct stat _dw_st; \
        while (stat("/tmp/start", &_dw_st) < 0) \
            sleep(1); \
    } while(0)

void output_ansi_text(char *buf, size_t buflen,
                      buffered_output_t * output, char* attachlink);
void output_ansi_javascript(char *buf, size_t buflen,
                            buffered_output_t * output, char* attachlink);

#endif                          /* BBSLIB_H */
