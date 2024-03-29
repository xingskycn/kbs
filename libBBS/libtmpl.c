#include "bbs.h"

/*
 * returns:
 * -1: some error
 * -2: tmpl version updated. needs term relogin and www restart
 * >=0: number of tmpls
 */
int orig_tmpl_init(char * nboard, int mode, struct a_template ** pptemp)
{
    /***********
     * mode 0: 用户看，不显示有斑竹权限的
     * mode 1: 斑竹管理
     ************/

    int fd;
    char tmpldir[STRLEN];
    struct s_template tmpl;
    struct s_content * cont;
    int templ_num;

    setbfile(tmpldir, nboard, TEMPLATE_DIR);

    if (pptemp == NULL) {
        return -1;
    }

    if (* pptemp == NULL) {
        * pptemp = (struct a_template *) malloc(sizeof(struct a_template) * MAX_TEMPLATE);
        if (* pptemp == NULL)
            return -1;
    }
    bzero(* pptemp, sizeof(struct a_template) * MAX_TEMPLATE);
    templ_num = 0;

    if ((fd = open(tmpldir, O_RDONLY)) == -1) {
        return 0;
    }
    while (read(fd, &tmpl, sizeof(struct s_template)) == sizeof(struct s_template)) {
        if (tmpl.version > TMPL_NOW_VERSION) {
            close(fd);
            return -2;
        }
        if (mode == 0 && (tmpl.flag & TMPL_BM_FLAG)) {
            lseek(fd, sizeof(struct s_content) * tmpl.content_num , SEEK_CUR);
            continue;
        }
        cont = (struct s_content *) malloc(sizeof(struct s_content) * tmpl.content_num);
        if (cont == NULL)
            break;
        bzero(cont, sizeof(struct s_content) * tmpl.content_num);
        if (read(fd, cont, sizeof(struct s_content)*tmpl.content_num) != sizeof(struct s_content)*tmpl.content_num)
            continue;
        (* pptemp)[templ_num].tmpl = (struct s_template *)malloc(sizeof(struct s_template));
        if ((* pptemp)[templ_num].tmpl == NULL) {
            free(cont);
            break;
        }
        bzero((* pptemp)[templ_num].tmpl , sizeof(struct s_template));
        memcpy((* pptemp)[templ_num].tmpl, &tmpl, sizeof(struct s_template));
        (* pptemp)[templ_num].cont = cont;
        templ_num ++;
        if (templ_num >= MAX_TEMPLATE)
            break;
    }
    close(fd);

    return templ_num;
}

int orig_tmpl_free(struct a_template ** pptemp, int temp_num)
{

    int i;

    for (i=0; i<temp_num; i++) {
        if ((* pptemp)[i].tmpl)
            free((* pptemp)[i].tmpl);
        if ((* pptemp)[i].cont)
            free((* pptemp)[i].cont);
    }
    free(* pptemp);
    * pptemp = NULL;
    return 0;
}

int orig_tmpl_save(struct a_template * ptemp, int temp_num, char *board)
{

    int i;
    FILE *fp;
    char tmpldir[STRLEN];

    setbfile(tmpldir, board, TEMPLATE_DIR);
    if ((fp = fopen(tmpldir, "w")) == NULL) {
        return -1;
    }
    for (i=0; i<temp_num; i++) {
        if (ptemp[i].tmpl == NULL)
            continue;
        fwrite(ptemp[i].tmpl, sizeof(struct s_template), 1, fp);
        fwrite(ptemp[i].cont, sizeof(struct s_content), ptemp[i].tmpl->content_num, fp);
    }
    fclose(fp);

    return 0;
}

int get_parameter_index_len(char *buf, int *index, int *fmtlen, int *def)
{
    int ret=0;

    char *ps = NULL, *pt = NULL;
    ps = strchr(buf, '#');
    if(ps != NULL)
    {
        if (fmtlen != NULL) {
            pt=ps+1;
            if (*pt == '-') {
                ret = -1;
                pt++;
            } else if (*pt == '+') {
                ret = 1;
                pt++;
            } else
                ret = 0;
            *fmtlen=atoi(pt);
        }
        *ps='\0';
    }
    else {
        if (fmtlen != NULL)
            *fmtlen=0;
    }

    if (!strcmp(buf, "USER")
      ||!strcmp(buf, "BOARD")
      ||!strcmp(buf, "BNAME")
      ||!strcmp(buf, "BMS")
      ||!strcmp(buf, "DATE")) {
        *def = 1;
        *index = 1;
    } else
        *index = atoi(buf);
    return ret;
}
