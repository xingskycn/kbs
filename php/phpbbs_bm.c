#include "php_kbs_bbs.h"

struct usernode {
    char *userid;
    struct usernode *next;
};

struct clubarg {
    const struct boardheader *brd;
    int mode;
    struct usernode *ulheader, *ulcurrent;
};

/**
 * Checking whether a user is a BM of a board or not.
 * prototype:
 * int bbs_is_bm(int brdnum, int usernum);
 *
 * @return one if the user is BM,
 *         zero if not.
 * @author flyriver
 */
PHP_FUNCTION(bbs_is_bm)
{
    long brdnum;
    long usernum;
    const struct boardheader *bp = NULL;
    const struct userec *up = NULL;
    int ac = ZEND_NUM_ARGS();

    /*
     * getting arguments
     */
    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "ll", &brdnum, &usernum) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if ((bp = getboard(brdnum)) == NULL) {
        RETURN_LONG(0);
    }
    if ((up = getuserbynum(usernum)) == NULL) {
        RETURN_LONG(0);
    }
    RETURN_LONG(is_BM(bp, up));
}

/**
 * int bbs_bmmanage(string board,int id,int mode,int zhiding)
 * which is used to switch article's flag
 * $mode defined in funcs.php
 * $mode = 0: do nth;
 *         1: del;
 *         2: mark;
 *         3: digest;
 *         4: noreplay;
 *         5: zhiding;
 *       7-9: % X #
 *         6: undel  :: add by pig2532 on 2005.12.19 ::
 *        10: add article to new announce clipboard     ::   add by pig2532   ::
 *        11: add article to announce clipboard         ::    on 2006-03-04   ::
 *        12: import
 *        13: import without head and qmd
 *        14: censor
 * return 0 : success;
 *        -1: board is NOT exist
 *        -2: do NOT have permission
 *        -3: can NOT load dir file
 *        -4: can NOT find article
 *        -9: system error
 */
PHP_FUNCTION(bbs_bmmanage)
{
    char *board;
    int  board_len;
    long  id,mode,zhiding;
    const struct boardheader* bh;
    int ret;
    char dir[STRLEN];
    int ent;
    int fd, bid;
    int edit_top = 0, dir_mode;
    struct fileheader f;
    FILE *fp;

    /* if in DELETED mode, num is transfered instead of id at parameter "id" */
    int ac = ZEND_NUM_ARGS();
    if (ac != 4 || zend_parse_parameters(4 TSRMLS_CC, "slll", &board, &board_len, &id, &mode, &zhiding) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    bid = getbid(board, &bh);
    if (!bid) RETURN_LONG(-1);
    strcpy(board,bh->filename);
    if (!is_BM(bh, getCurrentUser()))
        RETURN_LONG(-2);

    if (mode == 6) { /* undel action, add by pig2532 */
        int find = 0;
        setbdir(DIR_MODE_DELETED, dir, board);
        fp = fopen(dir, "r");
        if (!fp) {
            RETURN_LONG(-9);    /* cannot open index file */
        }
        fseek(fp, sizeof(f) * (id - 1) , SEEK_SET);   /* here variable "id" is actually num */
        if (fread(&f, sizeof(f), 1, fp) > 0) {
            find = 1;
        }
        fclose(fp);
        if (find == 0) {
            RETURN_LONG(-4);    /* article index not found, maybe SYSOP cleared them */
        }
    } else if (zhiding) {
        int find = 0;
        ent = 1;
        setbdir(DIR_MODE_ZHIDING, dir, board);
        fd = open(dir, O_RDWR, 0644);
        if (fd < 0)
            RETURN_LONG(-3);
        while (1) {
            if (read(fd,&f, sizeof(struct fileheader)) <= 0)
                break;
            if (f.id==id) {
                find=1;
                break;
            }
            ent++;
        }
        close(fd);
        if (!find)
            RETURN_LONG(-4);
    } else {
        setbdir(DIR_MODE_NORMAL, dir, board);
        fd = open(dir, O_RDWR, 0644);
        if (fd < 0) RETURN_LONG(-3);
        if (!get_records_from_id(fd, id, &f, 1, &ent)) {
            close(fd);
            RETURN_LONG(-4);
        }
        close(fd);
    }

    if (mode == 6) { /* undel action, add by pig2532 */
        char buf[128];
        snprintf(buf, 100, "boards/%s/.DELETED", board);
        ret = do_undel_post(board, buf, id, &f, NULL, getSession());
        if (ret == 1) {
            ret = 0;
        } else {
            ret = -1;
        }
    } else if (zhiding && (mode == 1)) {
        ret = do_del_ding(board, bid, ent, &f, getSession());
        switch (ret) {
            case -1:
                RETURN_LONG(-4);    /* del failed */
                break;
            case -2:
                RETURN_LONG(-9);    /* null fileheader */
                break;
            case 0:
                RETURN_LONG(0);     /* success */
                break;
            default:
                RETURN_LONG(-9);
                break;
        }
    } else if (mode == 1) {
        ret = del_post(ent, &f, bh);
    } else {
        struct write_dir_arg dirarg;
        struct fileheader data;
        int flag = 0;
        data.accessed[0] = ~(f.accessed[0]);
        data.accessed[1] = ~(f.accessed[1]);
        init_write_dir_arg(&dirarg);
        if (mode == 2)
            flag = FILE_MARK_FLAG;
        else if (mode == 3)
            flag = FILE_DIGEST_FLAG;
        else if (mode == 4)
            flag = FILE_NOREPLY_FLAG;
        else if (mode == 5) {
            flag = FILE_DING_FLAG;
            data.accessed[0] = f.accessed[0]; // to reserve flags. hack! - atppp
        } else if (mode == 7)
            flag = FILE_PERCENT_FLAG;
        else if (mode == 8)
            flag = FILE_DELETE_FLAG;
        else if (mode == 9)
            flag = FILE_SIGN_FLAG;
        else if ((mode == 10) || (mode == 11)) {
            ret = ann_article_import((mode == 10) ? board : NULL, f.title, f.filename, getCurrentUser()->userid);
            if (ret < 0) {
                RETURN_LONG(-9);
            }
            flag = FILE_IMPORT_FLAG;
        } else if (mode == 12) {   /* import */
            a_Save(NULL, bh->filename, &f, true, NULL, 0, getCurrentUser()->userid);
            ret = 0;
        } else if (mode == 13) {   /* import without head and qmd */
            a_SeSave(NULL, bh->filename, &f, true, NULL, 0, 0, getCurrentUser()->userid);
            ret = 0;
        } else if (mode == 14) {
            flag = FILE_CENSOR_FLAG;
        } else
            RETURN_LONG(-3);

        if (mode==4 && is_top(&f, board)) { /* 修改置底文章的不可re */
            edit_top = 1;
            setbdir(DIR_MODE_ZHIDING, dir, board);
            dir_mode = DIR_MODE_ZHIDING;
            ent = -1;
            POSTFILE_BASENAME(f.filename)[0]='Z';
        } else {
            dir_mode = DIR_MODE_NORMAL;
            edit_top = 0;
        }
        dirarg.filename = dir;
        dirarg.ent = ent;
        if (change_post_flag(&dirarg, dir_mode, bh, &f, flag, &data,true,getSession())!=0)
            ret = 1;
        else
            ret = 0;
        free_write_dir_arg(&dirarg);
    }

    if (ret != 0)
        RETURN_LONG(-9);

    if (edit_top)
        board_update_toptitle(bid, true);

    RETURN_LONG(0);
}


/**
 * load deny users
 * function bbs_denyusers(char* board,array denyusers);
 *  @return the result
 *     0 : seccess
 *     -1: PHP error
 *     -2: board NOT exist
 *     -3: do NOT have permission
 *  @author: windinsn
 */
PHP_FUNCTION(bbs_denyusers)
{
    char *board;
    int  board_len;
    zval *element,*denyusers;

    const struct boardheader *brd;
    FILE *fp;
    char path[80], buf[256], buf2[100];
    char *id, *nick;
    int i;

    int ac = ZEND_NUM_ARGS();
    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "sa", &board, &board_len, &denyusers) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (array_init(denyusers) != SUCCESS)
        RETURN_LONG(-1);

    if (getbid(board, &brd) == 0)
        RETURN_LONG(-2);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-2);
    strcpy(board,brd->filename);
#ifdef MEMBER_MANAGER
    if (!check_board_member_manager(NULL, brd, BMP_DENY))
#else	
    if (!is_BM(brd, getCurrentUser()))
#endif
        RETURN_LONG(-3);

    sprintf(path, "boards/%s/deny_users", board);

    if ((fp=fopen(path,"r"))==NULL)
        RETURN_LONG(0);

    i = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        if (buf[0] == '\n') continue;
        MAKE_STD_ZVAL(element);
        array_init(element);

        id = strchr(buf, ' ');
        if (id != NULL)
            *id = '\0';

        strcpy(buf2, buf);
        add_assoc_string(element,"ID",buf2,1);
        strncpy(buf2, buf + 13, 30);
        buf2[30] = '\0';
        add_assoc_string(element,"EXP",buf2,1);

        nick = strrchr(buf + 13, '[');
        if (nick != NULL) {
            add_assoc_long(element,"FREETIME",atol(nick + 1));
            nick--;
            if (nick - buf > 43) {
                *nick = '\0';
                strcpy(buf2, buf + 43);
                add_assoc_string(element,"COMMENT",buf2,1);
            }
        }
        zend_hash_index_update(Z_ARRVAL_P(denyusers),i,(void*) &element, sizeof(zval*), NULL);
        i++;
    }
    fclose(fp);
    RETURN_LONG(0);
}

/**
 * add user to deny list
 * function bbs_denyadd(char* board,char* userid,char* exp,int denyday,int manual_deny);
 *  @return the result
 *     0 : success
 *     -1: board NOT exist
 *     -2: do NOT have permission
 *     -3: user NOT exist
 *     -4: already in list
 *     -5: time long error
 *     -6: need denymsg
 *     -7: user CAN'T post here
 *  @author: windinsn
 */
PHP_FUNCTION(bbs_denyadd)
{
    char *board,*userid,*exp;
    int  board_len,userid_len,exp_len;
    long  denyday,manual_deny;
    int autofree;
    const struct boardheader *brd;
    struct userec *lookupuser;
    char buf[256], denystr[32];
    struct tm *tmtime;
    time_t now,undenytime;
    char path[STRLEN];

    int ac = ZEND_NUM_ARGS();
    int id = 0;
    if (ac != 6 || zend_parse_parameters(6 TSRMLS_CC, "ssslll", &board, &board_len, &userid ,&userid_len ,&exp ,&exp_len ,&denyday ,&id ,&manual_deny) == FAILURE)
        if (ac != 5 || zend_parse_parameters(5 TSRMLS_CC, "sssll", &board, &board_len, &userid ,&userid_len ,&exp ,&exp_len ,&denyday ,&manual_deny) == FAILURE)
            WRONG_PARAM_COUNT;

    if (getbid(board, &brd) == 0)
        RETURN_LONG(-1);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-1);
    strcpy(board,brd->filename);
#ifdef MEMBER_MANAGER
    if (!check_board_member_manager(NULL, brd, BMP_DENY))
#else	
    if (!is_BM(brd, getCurrentUser()))
#endif
        RETURN_LONG(-2);
#ifdef RECORD_DENY_FILE
    //TODO: 想办法获得对应的fh
    struct fileheader fh;
    bzero(&fh, sizeof(struct fileheader));
    if (id) {
        int fd;
        setbdir(DIR_MODE_NORMAL, path, board);
        if ((fd = open(path, O_RDWR, 0644)) >= 0) {
            get_records_from_id(fd, id, &fh, 1, NULL);
            close(fd);
            strcpy(userid, fh.owner);
        }
    }
#endif
    if (getuser(userid,&lookupuser)==0)
        RETURN_LONG(-3);
    strcpy(userid,lookupuser->userid);
    if (!haspostperm(lookupuser, brd -> filename))
        RETURN_LONG(-7);
    if (deny_me(userid, board))
        RETURN_LONG(-4);

    process_control_chars(exp,NULL);

    if (!*exp)
        RETURN_LONG(-6);
    strnzhcpy(denystr, exp, 30);

    if (denyday < 1 || denyday > (HAS_PERM(getCurrentUser(), PERM_SYSOP)?70:14))
        RETURN_LONG(-5);

#ifdef MANUAL_DENY
    autofree = manual_deny ? false : true;
#else
    autofree = true;
#endif

#if 0
    brc_initial(getCurrentUser()->userid, board, getSession());
#endif

    now = time(0);
    undenytime = now + denyday * 24 * 60 * 60;
    tmtime = localtime(&undenytime);

    if (autofree)
        sprintf(buf, "%-12.12s %-30.30s%-12.12s %2d月%2d日解\x1b[%lum%lum", userid, denystr, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime, now);
    else
        sprintf(buf, "%-12.12s %-30.30s%-12.12s %2d月%2d日后\x1b[%lum%lum", userid, denystr, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime, now);

    setbfile(path, board, "deny_users");
    if (addtofile(path, buf) == 1) {
#ifdef RECORD_DENY_FILE
        deny_announce(userid,brd,denystr,denyday,getCurrentUser(),time(0),0,(fh.id)?&fh:NULL,0);
#else
        deny_announce(userid,brd,denystr,denyday,getCurrentUser(),time(0),0);
#endif
        deny_mailuser(userid,brd,denystr,denyday,getCurrentUser(),time(0),0,autofree);
/*
        struct userec *saveptr;
        int my_flag = 0;
        struct userec saveuser;
        FILE *fn;
        char buffer[STRLEN];

        gettmpfilename(path, "deny");
        fn = fopen(path, "w+");
        memcpy(&saveuser, getCurrentUser(), sizeof(struct userec));
        saveptr = getCurrentUser();
        getCurrentUser() = &saveuser;
        sprintf(buffer, "%s被取消在%s版的发文权限", userid, board);

        if ((HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) && !chk_BM_instr(brd->BM, getCurrentUser()->userid)) {
            strcpy(getCurrentUser()->userid, "SYSOP");
            strcpy(getCurrentUser()->username, NAME_SYSOP);
            my_flag = 0;
            fprintf(fn, "寄信人: SYSOP (%s) \n", NAME_SYSOP);
            fprintf(fn, "标  题: %s\n", buffer);
            fprintf(fn, "发信站: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "来  源: %s\n", NAME_BBS_ENGLISH);
            fprintf(fn, "\n");
            fprintf(fn, "由于您在 \x1b[4m%s\x1b[m 版 \x1b[4m%s\x1b[m，我很遗憾地通知您， \n", board, exp);
            fprintf(fn, "您被暂时取消在该版的发文权力 \x1b[4m%ld\x1b[m 天", denyday);
            if (!autofree)
                fprintf(fn, "，到期后请回复\n此信申请恢复权限。\n");
            fprintf(fn, "\n");
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, saveptr->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
        } else {
            my_flag = 1;
            fprintf(fn, "寄信人: %s \n", getCurrentUser()->userid);
            fprintf(fn, "标  题: %s\n", buffer);
            fprintf(fn, "发信站: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "来  源: %s \n", SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
            fprintf(fn, "\n");
            fprintf(fn, "由于您在 \x1b[4m%s\x1b[m 版 \x1b[4m%s\x1b[m，我很遗憾地通知您， \n", board, exp);
            fprintf(fn, "您被暂时取消在该版的发文权力 \x1b[4m%ld\x1b[m 天", denyday);
            if (!autofree)
                fprintf(fn, "，到期后请回复\n此信申请恢复权限。\n");
            fprintf(fn, "\n");
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", saveptr->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
        }
        fclose(fn);
#ifdef NEWSMTH
        mail_file(DELIVER, path, userid, buffer, 0, NULL);
#else
        mail_file(getCurrentUser()->userid, path, userid, buffer, 0, NULL);
#endif
        fn = fopen(path, "w+");
        fprintf(fn, "由于 \x1b[4m%s\x1b[m 在 \x1b[4m%s\x1b[m 版的 \x1b[4m%s\x1b[m 行为，\n", userid, board, exp);
        fprintf(fn, "被暂时取消在本版的发文权力 \x1b[4m%ld\x1b[m 天。\n", denyday);

        if (my_flag == 0) {
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, saveptr->userid);
        } else {
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", saveptr->userid);
        }
        fprintf(fn, "                              %s\n", ctime(&now));
        fclose(fn);
#ifdef NEWSMTH
        post_file(getCurrentUser(), "", path, board, buffer, 0, 1, getSession());
#else
        post_file(getCurrentUser(), "", path, board, buffer, 0, 2, getSession());
#endif
        getCurrentUser() = saveptr;

        //sprintf(buffer, "%s 被 %s 封禁本版POST权", userid, getCurrentUser()->userid);

        if (PERM_BOARDS & lookupuser->userlevel)
            sprintf(buffer, "%s 封某版" NAME_BM " %s 在 %s", getCurrentUser()->userid, userid, board);
        else
            sprintf(buffer, "%s 封 %s 在 %s", getCurrentUser()->userid, userid, board);
        post_file(getCurrentUser(), "", path, "denypost", buffer, 0, -1, getSession());
        unlink(path);
*/
        bmlog(getCurrentUser()->userid, board, 10, 1);
    }

    RETURN_LONG(0);
}

/**
 * modify denied user
 * function bbs_denymod(char* board,char* userid,char* exp,int denyday,int manual_deny);
 *  @return the result
 *     0 : success
 *     -1: board NOT exist
 *     -2: do NOT have permission
 *     -3: user NOT exist
 *     -4: user NOT in list
 *     -5: time long error
 *     -6: need denymsg
 *     -7: user CAN'T post here
 *  @author: jiangjun
 */
PHP_FUNCTION(bbs_denymod)
{
    char *board,*userid,*exp;
    int  board_len,userid_len,exp_len;
    long  denyday,manual_deny;
    int autofree,minday;
    const struct boardheader *brd;
    struct userec *lookupuser;
    char buf[256], denystr[32];
    struct tm *tmtime;
    time_t now,denytime,undenytime;
    char path[STRLEN];

    int ac = ZEND_NUM_ARGS();
    if (ac != 5 || zend_parse_parameters(5 TSRMLS_CC, "sssll", &board, &board_len, &userid ,&userid_len ,&exp ,&exp_len ,&denyday ,&manual_deny) == FAILURE)
        WRONG_PARAM_COUNT;

    if (getbid(board, &brd) == 0)
        RETURN_LONG(-1);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-1);
    strcpy(board,brd->filename);
#ifdef MEMBER_MANAGER
    if (!check_board_member_manager(NULL, brd, BMP_DENY))
#else	
    if (!is_BM(brd, getCurrentUser()))
#endif
        RETURN_LONG(-2);
    if (getuser(userid,&lookupuser)==0)
        RETURN_LONG(-3);
    strcpy(userid,lookupuser->userid);
    if (!haspostperm(lookupuser, brd -> filename))
        RETURN_LONG(-7);
    if (!deny_me(userid, board))
        RETURN_LONG(-4);
    setbfile(path, board, "deny_users");
    if (!seek_in_file(path, userid, buf))
        RETURN_LONG(-4);
    denytime = get_denied_time(buf);
    now = time(0);
    minday = (now - denytime) / 86400 + 1;

    process_control_chars(exp,NULL);

    if (!*exp)
        RETURN_LONG(-6);
    strnzhcpy(denystr, exp, 30);

    if (denyday < minday || denyday > (HAS_PERM(getCurrentUser(), PERM_SYSOP)?70:14))
        RETURN_LONG(-5);

#ifdef MANUAL_DENY
    autofree = manual_deny ? false : true;
#else
    autofree = true;
#endif

#if 0
    brc_initial(getCurrentUser()->userid, board, getSession());
#endif

    undenytime = denytime + denyday * 24 * 60 * 60;
    tmtime = localtime(&undenytime);

    if (autofree)
        sprintf(buf, "%-12.12s %-30.30s%-12.12s %2d月%2d日解\x1b[%lum%lum", userid, denystr, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime, denytime);
    else
        sprintf(buf, "%-12.12s %-30.30s%-12.12s %2d月%2d日后\x1b[%lum%lum", userid, denystr, getCurrentUser()->userid, tmtime->tm_mon + 1, tmtime->tm_mday, undenytime, denytime);

    if (replace_from_file_by_id(path, userid, buf)>=0) {
#ifdef RECORD_DENY_FILE
        deny_announce(userid,brd,denystr,denyday,getCurrentUser(),time(0),1,NULL,0);
#else
        deny_announce(userid,brd,denystr,denyday,getCurrentUser(),time(0),1);
#endif
        deny_mailuser(userid,brd,denystr,denyday,getCurrentUser(),time(0),1,autofree);
/*
        struct userec *saveptr;
        int my_flag = 0;
        struct userec saveuser;
        FILE *fn;
        char buffer[STRLEN];

        gettmpfilename(path, "deny");
        fn = fopen(path, "w+");
        memcpy(&saveuser, getCurrentUser(), sizeof(struct userec));
        saveptr = getCurrentUser();
        getCurrentUser() = &saveuser;
        sprintf(buffer, "修改%s在%s版的发文权限封禁", userid, board);

        if ((HAS_PERM(getCurrentUser(), PERM_SYSOP) || HAS_PERM(getCurrentUser(), PERM_OBOARDS)) && !chk_BM_instr(brd->BM, getCurrentUser()->userid)) {
            strcpy(getCurrentUser()->userid, "SYSOP");
            strcpy(getCurrentUser()->username, NAME_SYSOP);
            my_flag = 0;
            fprintf(fn, "寄信人: SYSOP (%s) \n", NAME_SYSOP);
            fprintf(fn, "标  题: %s\n", buffer);
            fprintf(fn, "发信站: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "来  源: %s\n", NAME_BBS_ENGLISH);
            fprintf(fn, "\n");
            fprintf(fn, "由于您在 \x1b[4m%s\x1b[m 版 \x1b[4m%s\x1b[m，我很遗憾地通知您， \n", board, exp);
            fprintf(fn, "您被暂时取消在该版的发文权力 \x1b[4m%ld\x1b[m 天", denyday);
            if (!autofree)
                fprintf(fn, "，到期后请回复\n此信申请恢复权限。\n");
            fprintf(fn, "\n");
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, saveptr->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
        } else {
            my_flag = 1;
            fprintf(fn, "寄信人: %s \n", getCurrentUser()->userid);
            fprintf(fn, "标  题: %s\n", buffer);
            fprintf(fn, "发信站: %s (%24.24s)\n", BBS_FULL_NAME, ctime(&now));
            fprintf(fn, "来  源: %s \n", SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
            fprintf(fn, "\n");
            fprintf(fn, "由于您在 \x1b[4m%s\x1b[m 版 \x1b[4m%s\x1b[m，我很遗憾地通知您， \n", board, exp);
            fprintf(fn, "您被暂时取消在该版的发文权力 \x1b[4m%ld\x1b[m 天", denyday);
            if (!autofree)
                fprintf(fn, "，到期后请回复\n此信申请恢复权限。\n");
            fprintf(fn, "\n");
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", saveptr->userid);
            fprintf(fn, "                              %s\n", ctime(&now));
        }
        fclose(fn);
#ifdef NEWSMTH
        mail_file(DELIVER, path, userid, buffer, 0, NULL);
#else
        mail_file(getCurrentUser()->userid, path, userid, buffer, 0, NULL);
#endif
        fn = fopen(path, "w+");
        fprintf(fn, "由于 \x1b[4m%s\x1b[m 在 \x1b[4m%s\x1b[m 版的 \x1b[4m%s\x1b[m 行为，\n", userid, board, exp);
        fprintf(fn, "被暂时取消在本版的发文权力 \x1b[4m%ld\x1b[m 天。\n", denyday);

        if (my_flag == 0) {
            fprintf(fn, "                            %s" NAME_SYSOP_GROUP DENY_NAME_SYSOP "\x1b[4m%s\x1b[m\n", NAME_BBS_CHINESE, saveptr->userid);
        } else {
            fprintf(fn, "                              " NAME_BM ":\x1b[4m%s\x1b[m\n", saveptr->userid);
        }
        fprintf(fn, "                              %s\n", ctime(&now));
        fclose(fn);
#ifdef NEWSMTH
        post_file(getCurrentUser(), "", path, board, buffer, 0, 1, getSession());
#else
        post_file(getCurrentUser(), "", path, board, buffer, 0, 2, getSession());
#endif
        getCurrentUser() = saveptr;

        //sprintf(buffer, "%s 被 %s 封禁本版POST权", userid, getCurrentUser()->userid);

        if (PERM_BOARDS & lookupuser->userlevel)
            sprintf(buffer, "%s 封某版" NAME_BM " %s 在 %s", getCurrentUser()->userid, userid, board);
        else
            sprintf(buffer, "%s 封 %s 在 %s", getCurrentUser()->userid, userid, board);
        post_file(getCurrentUser(), "", path, "denypost", buffer, 0, -1, getSession());
        unlink(path);
*/
        bmlog(getCurrentUser()->userid, board, 10, 1);
    }

    RETURN_LONG(0);
}

/**
 * remove user from deny list
 * function bbs_denydel(char* board,char* userid);
 *  @return the result
 *     0 : seccess
 *     -1: board NOT exist
 *     -2: do NOT have permission
 *     -3: NOT in list
 *  @author: windinsn
 */
PHP_FUNCTION(bbs_denydel)
{
    char *board,*userid;
    int  board_len,userid_len;
    const struct boardheader *brd;

    int ac = ZEND_NUM_ARGS();
    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "ss", &board, &board_len, &userid ,&userid_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (getbid(board, &brd) == 0)
        RETURN_LONG(-1);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-1);
    strcpy(board,brd->filename);
#ifdef MEMBER_MANAGER
    if (!check_board_member_manager(NULL, brd, BMP_DENY))
#else	
    if (!is_BM(brd, getCurrentUser()))
#endif
        RETURN_LONG(-2);

    if (deny_me(userid, board)) {
        deldeny(getCurrentUser(), board, userid, 0, 1, getSession());
        RETURN_LONG(0);
    } else
        RETURN_LONG(-3);
}



/**
 * function bbs_bm_get_manageable_bids(string userid)
 * @return a string
 *         "A": can manage all visible boards
 *         empty string:  not a bm
 *         .-delimited bid list: all manageable board id list
 * @author atppp
 *
 */
PHP_FUNCTION(bbs_bm_get_manageable_bids)
{
#define MAX_MANAGEABLE_BIDS_LENGTH 1024 // note most browser set 4k as maximum bytes per cookie - atppp
    char *userid;
    int userid_len;
    struct userec *user;
    char buf[MAX_MANAGEABLE_BIDS_LENGTH];
    int ac = ZEND_NUM_ARGS();
    if (ac == 0) {
        user = getCurrentUser();
        if (user == NULL) {
            RETURN_EMPTY_STRING();
        }
    } else {
        if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &userid ,&userid_len) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
        if (!getuser(userid, &user)) {
            RETURN_EMPTY_STRING();
        }
    }
    if (HAS_PERM(user, PERM_OBOARDS) || HAS_PERM(user, PERM_SYSOP)) {
        RETURN_STRING("A", 1);
    }
    if (!HAS_PERM(user, PERM_BOARDS)) {
        RETURN_EMPTY_STRING();
    }
    get_manageable_bids(user, buf, MAX_MANAGEABLE_BIDS_LENGTH);
    RETURN_STRING(buf, 1);
#undef MAX_MANAGEABLE_BIDS_LENGTH
}



/* club functions below, by pig2532 */

static int func_load_club_users(struct userec *user,void *varg)
{
    struct clubarg *clubflag = (struct clubarg *)varg;
    if (user->userid[0]&&get_user_club_perm(user,clubflag->brd,clubflag->mode)) {
        struct usernode *untmp;
        untmp = (struct usernode *)emalloc(sizeof(struct usernode));
        if (untmp != NULL) {
            untmp->userid = estrdup(user->userid);
            untmp->next = NULL;
            if (clubflag->ulheader == NULL) {
                clubflag->ulheader = clubflag->ulcurrent = untmp;
            } else {
                clubflag->ulcurrent->next = untmp;
                clubflag->ulcurrent = untmp;
            }
        }
        return COUNT;
    }
    return 0;
}

static int CompareNameCase(const void *v1,const void *v2)
{
    return strcasecmp((*((const char**)v1)),(*((const char**)v2)));
}

static void clubread_FreeAll(struct usernode *ulheader, char **userarray)
{
    struct usernode *ulnext;
    while (ulheader) {
        ulnext = ulheader->next;
        if (ulheader->userid)
            efree(ulheader->userid);
        efree(ulheader);
        ulheader = ulnext;
    }
    if (userarray)
        efree(userarray);
}

/* read club member, by pig2532
parameters:
    bname: board name
    clubmode: 0-read_prem_club, 1-write_prem_club
    start: read user list from
    num: how many user names to read
    userlist: array to save the name list
return:
    >=0: [success] record count total
    -1: system error
    -2: board not found
    -3: no BM perm
    -4: this is not a club
*/
PHP_FUNCTION(bbs_club_read)
{
    char *bname;
    long clubmode, start, num;
    zval *userlist;
    const struct boardheader *brd;
    int i, bname_len, count;
    struct clubarg clubflag;
    char **userarray, **t;
    struct usernode *ulheader, *ulcurrent;

    if (ZEND_NUM_ARGS() != 5 || zend_parse_parameters(5 TSRMLS_CC, "sllla", &bname, &bname_len, &clubmode, &start, &num, &userlist) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (start < 0)
        RETURN_LONG(0);
    if (array_init(userlist) != SUCCESS)
        RETURN_LONG(-1);
    if (getbid(bname, &brd) == 0)
        RETURN_LONG(-2);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-2);
    strcpy(bname, brd->filename);
    if (!is_BM(brd, getCurrentUser()))
        RETURN_LONG(-3);
    if (!(brd->flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))||!(brd->clubnum>0)||(brd->clubnum>MAXCLUB))
        RETURN_LONG(-4);
    if (!((brd->flag & BOARD_CLUB_READ) && (brd->flag & BOARD_CLUB_WRITE)))
        clubmode = brd->flag & BOARD_CLUB_WRITE;

    userarray = NULL;
    ulheader = ulcurrent = NULL;
    clubflag.brd = brd;
    clubflag.mode = clubmode;
    clubflag.ulheader = ulheader;
    clubflag.ulcurrent = ulcurrent;
    count = apply_users(func_load_club_users, &clubflag);
    ulheader = clubflag.ulheader;
    ulcurrent = clubflag.ulcurrent;
    if (start >= count) {
        clubread_FreeAll(ulheader, userarray);
        RETURN_LONG(0);
    }

    userarray = (char **)emalloc(count * sizeof(char *));
    if (userarray == NULL) {
        clubread_FreeAll(ulheader, userarray);
        RETURN_LONG(-1);
    }
    for (ulcurrent=ulheader,t=userarray;ulcurrent;ulcurrent=ulcurrent->next,t++)
        (*t)=ulcurrent->userid;
    qsort(userarray, count, sizeof(const char*), CompareNameCase);

    num = (num>count-start) ? (count-start) : num;
    t = userarray + start;
    for (i=0; i<num; i++) {
        add_next_index_string(userlist, *t, 1);
        t++;
    }

    clubread_FreeAll(ulheader, userarray);
    RETURN_LONG(count);
}

/* read club flag, by pig2532
parameters:
    bname: board name
return:
    -1: board not found
    0: not a club
    1: read perm club
    2: write perm club
    3: both perm club
*/
PHP_FUNCTION(bbs_club_flag)
{
    char *bname;
    int bname_len;
    const struct boardheader *brd;

    if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &bname, &bname_len) != SUCCESS)
        WRONG_PARAM_COUNT;

    if (getbid(bname, &brd) == 0)
        RETURN_LONG(-1);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-1);
    strcpy(bname, brd->filename);
    if (!(brd->flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))||!(brd->clubnum>0)||(brd->clubnum>MAXCLUB))
        RETURN_LONG(0);
    if (brd->flag & BOARD_CLUB_READ) {
        if (brd->flag & BOARD_CLUB_WRITE) {
            RETURN_LONG(3);
        } else {
            RETURN_LONG(1);
        }
    } else if (brd->flag & BOARD_CLUB_WRITE) {
        RETURN_LONG(2);
    } else {
        RETURN_LONG(0);
    }
}


/* modify user's club perm, by pig2532
parameters:
    bname: board name
    clubop: club operate string, start with '+' to add, '-' to remove
    mode: 0-read_perm, 1-write_perm
    info: operation comment
return:
    0: success
    -1: board not found
    -2: permission denied
    -3: this is not a club
*/
PHP_FUNCTION(bbs_club_write)
{
    char *bname, *clubop, *line, *info, *sp_com;
    int bname_len, clubop_len, info_len;
    long mode;
    struct userec *user;
    const struct boardheader *brd;

    if (ZEND_NUM_ARGS() != 4 || zend_parse_parameters(4 TSRMLS_CC, "ssls", &bname, &bname_len, &clubop, &clubop_len, &mode, &info, &info_len) != SUCCESS)
        WRONG_PARAM_COUNT;

    if (getbid(bname, &brd) == 0)
        RETURN_LONG(-1);
    if (!check_read_perm(getCurrentUser(), brd))
        RETURN_LONG(-1);
    strcpy(bname, brd->filename);
    if (!is_BM(brd, getCurrentUser()))
        RETURN_LONG(-2);
    if (!(brd->flag&(BOARD_CLUB_READ|BOARD_CLUB_WRITE))||!(brd->clubnum>0)||(brd->clubnum>MAXCLUB))
        RETURN_LONG(-3);
    if (!((brd->flag & BOARD_CLUB_READ) && (brd->flag & BOARD_CLUB_WRITE)))
        mode = brd->flag & BOARD_CLUB_WRITE;

    line = &clubop[1];
    switch (clubop[0]) {
        case 0:
        case 10:
        case 13:
        case '#':
            RETURN_LONG(0);
        case '-':
            trimstr(line);
            /* pig2532 Jan 2008, 模仿兔子的特定附加说明 */
            sp_com = strchr(line, 32);
            if (sp_com) {
                *sp_com++ = 0;
                trimstr(sp_com);
            }
            if (!getuser(line,&user)||!get_user_club_perm(user,brd,mode))
                RETURN_LONG(0);
            if (!del_user_club_perm(user,brd,mode)) {
                club_maintain_send_mail(user->userid, sp_com ? sp_com : info,1,mode,brd,getSession());
            }
            break;
        case '+':
            line++;
        default:
            line--;
            /* pig2532 Jan 2008, 模仿兔子的特定附加说明 */
            sp_com = strchr(line, 32);
            if (sp_com) {
                *sp_com++ = 0;
                trimstr(sp_com);
            }
            trimstr(line);
            if (!getuser(line,&user)||!strcmp(user->userid,"guest")||get_user_club_perm(user,brd,mode))
                RETURN_LONG(0);
            if (!set_user_club_perm(user,brd,mode)) {
                club_maintain_send_mail(user->userid,sp_com ? sp_com : info,0,mode,brd,getSession());
            }
            break;
    }
    RETURN_LONG(0);
}

/* club functions end */

/* threads functions, by pig2532
parameters:
    bid: board id
    gid: thread group id
    start: start article id
    operate:  0 - nothing
              1 - delete
              2 - mark
              3 - unmark
              4 - del X records
              5 - put to announce
              6 - set X flag
              7 - unset X flag
              8 - no reply
              9 - cancel no reply
              10 - make total
              11 - make total without quote
              12 - import
              13 - import without head and qmd
return:
    >=0: [success] article numbers
    -1: board not found
    -2: permission denied
    -3: system error
    -10: unknown operate
*/
PHP_FUNCTION(bbs_threads_bmfunc)
{
    long bid, gid, start, operate, force = 0;
    const struct boardheader *bp;
    char dirpath[STRLEN];
    int ret, haveprev=0, i, fd, ent, count;
    struct fileheader *articles, fh;

#ifdef NEWSMTH
    int goddelete = 0;
#endif
    if (ZEND_NUM_ARGS() != 4 || zend_parse_parameters(4 TSRMLS_CC, "llll", &bid, &gid, &start, &operate) == FAILURE) {
        if (ZEND_NUM_ARGS() != 5 || zend_parse_parameters(5 TSRMLS_CC, "lllll", &bid, &gid, &start, &operate, &force) == FAILURE) {
            WRONG_PARAM_COUNT;
        }
    }
#ifdef NEWSMTH
    if ((operate == 99)) {
        operate = 10;
        goddelete = 1;
    }
#endif
    if ((operate < 1) || (operate > 13)) {
        RETURN_LONG(-10);
    }
    bp = getboard(bid);
    if (bp == NULL) {
        RETURN_LONG(-1);
    }
#ifdef NEWSMTH
    if (!goddelete) {
#endif
    if (!is_BM(bp, getCurrentUser())) {
        RETURN_LONG(-2);
    }
#ifdef NEWSMTH
    }
#endif

    articles = NULL;
    setbdir(DIR_MODE_NORMAL, dirpath, bp->filename);
    if ((operate == 1) || (operate == 4) || ((operate >= 10) && (operate <= 13))) { /* delete or make total or import */
        ret = get_threads_from_gid(dirpath, gid, &articles, -1, start, &haveprev, operate, getCurrentUser());
    } else {
        if (operate == 5)   /* initialize announce clipboard first */
            ann_article_import(bp->filename, NULL, NULL, getCurrentUser()->userid);
        ret = get_threads_from_gid(dirpath, gid, NULL, -2, start, &haveprev, operate, getCurrentUser());
    }

    if (operate == 1) { /* del threads */
        fd = open(dirpath, O_RDWR, 0644);
        if (fd < 0) {
            free(articles);
            RETURN_LONG(-3);
        }
        count = 0;
        for (i=0; i<ret; i++) {
            if (force || !(articles[i].accessed[0] & (FILE_MARKED | FILE_PERCENT)))
                if (get_records_from_id(fd, articles[i].id, &fh, 1, &ent)) {
                    if (del_post(ent, &fh, bp) == 0)
                        count++;
                }
        }
        close(fd);
        ret = count;
    } else if (operate == 4) { /* del X articles in threads */
        fd = open(dirpath, O_RDWR, 0644);
        if (fd < 0) {
            free(articles);
            RETURN_LONG(-3);
        }
        count = 0;
        for (i=0; i<ret; i++) {
            if ((articles[i].accessed[1] & FILE_DEL) && (force || !(articles[i].accessed[0] & (FILE_MARKED | FILE_PERCENT))))
                if (get_records_from_id(fd, articles[i].id, &fh, 1, &ent)) {
                    if (del_post(ent, &fh, bp) == 0)
                        count++;
                }
        }
        close(fd);
        ret = count;
    } else if ((operate == 10) || (operate == 11)) /* make total */ {
        char title[STRLEN], *ptr, tmpf[PATHLEN];
#ifdef BOARD_SECURITY_LOG
        char buf[PATHLEN];
        FILE *fp = NULL;
#ifdef NEWSMTH
        if (!goddelete) {
#endif
        gettmpfilename(buf, "bm_func");
        fp = fopen(buf, "w");
#ifdef NEWSMTH
        }
#endif
#endif
        for (i=0; i<ret; i++) {
            a_SeSave(NULL, bp->filename, &articles[i], i>0, NULL, 0, operate==11, getCurrentUser()->userid);
#ifdef BOARD_SECURITY_LOG
            if (fp) {
                char date[8];
                strncpy(date, ctime((time_t *)&articles[i].posttime) + 4, 6);
                date[6] = '\0';
                fprintf(fp, "%8d %-12s %6s  %s%s\n", articles[i].id, articles[i].owner, date, articles[i].id==articles[i].groupid?"● ":"", articles[i].title);
            }
#endif
        }
        if (ret > 0) {
#ifdef BOARD_SECURITY_LOG
#ifdef NEWSMTH
            if (!goddelete) {
#endif
            if (fp)
                fclose(fp);
            board_security_report(buf, getCurrentUser(), "合集", bp->filename, &articles[0]);
#ifdef NEWSMTH
            }
#endif
#endif
            unsigned char accessed[2];
            strcpy(title, "[合集] ");
            if (strncmp(articles[0].title, "Re: ", 4) == 0)
                ptr = articles[0].title + 4;
            else
                ptr = articles[0].title;
            strncpy(title + 7, ptr, STRLEN - 7);
            title[STRLEN - 1] = '\0';
            sprintf(tmpf, "tmp/bm.%s", getCurrentUser()->userid);
            accessed[0] = 0;
            accessed[1] = FILE_READ;
#ifdef NEWSMTH
            if (post_file_alt(tmpf, getCurrentUser(), title, (goddelete?"GodDelete":bp->filename), (goddelete?bp->filename:NULL), 0x04, accessed))
#else
            if (post_file_alt(tmpf, getCurrentUser(), title, bp->filename, NULL, 0x04, accessed))
#endif
            {
                unlink(tmpf);
                sprintf(tmpf, "tmp/se.%s", getCurrentUser()->userid);
                unlink(tmpf);
                free(articles);
                RETURN_LONG(-3);
            }
        }
    } else if ((operate == 12) || (operate == 13)) { /* import */
        for (i=0; i<ret; i++) {
            a_SeSave(NULL, bp->filename, &articles[i], true, NULL, 0, operate==13, getCurrentUser()->userid);
        }
    }

    if (articles)
        free(articles);
    if (ret >= 0) {
        RETURN_LONG(ret);
    } else {
        RETURN_LONG(-3);
    }
}

PHP_FUNCTION(bbs_getdenyreason)
{
    int ac = ZEND_NUM_ARGS();
    char *board;
    long b_len, all;
    zval *element, *denyreasons;
    char reason[MAXDENYREASON][STRLEN];
    int i, count;

    if (ac!=3 || zend_parse_parameters(3 TSRMLS_CC, "sal", &board, &b_len, &denyreasons, &all) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    array_init(denyreasons);
    /* 先获取自定义封禁理由 */
    count = get_deny_reason(board, reason, MAXCUSTOMREASON);
    /* 再获得系统封禁理由 */
    if (all)
        count += get_deny_reason(NULL, &(reason[count]), MAXDENYREASON-count);
    for (i=0;i<count;i++) {
        MAKE_STD_ZVAL(element);
        array_init(element);
        add_assoc_string(element, "desc", reason[i], 1);
        zend_hash_index_update(Z_ARRVAL_P(denyreasons), i, (void *)&element, sizeof(zval *), NULL);
    }
    RETURN_LONG(count);
}

PHP_FUNCTION(bbs_setdenyreason)
{
    int ac = ZEND_NUM_ARGS();
    char *board, *str;
    long b_len, s_len;
    char reason[MAXCUSTOMREASON][STRLEN];
    char *p, *q;
    int count;

    if (ac!=2 || zend_parse_parameters(2 TSRMLS_CC, "ss", &board, &b_len, &str, &s_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (getbid(board, NULL)<=0)
        RETURN_LONG(-1);
    count = 0;
    p = str;
    q = p;
    while (*p!='\0') {
        if (*p == '\n') {
            *p = '\0';
            strcpy(reason[count], q);
            count++;
            p++;
            if (*p=='\0')
                break;
            if (*p == '\r') {
                p++;
                if (*p=='\0')
                    break;
            }
            if (count>=MAXCUSTOMREASON)
                break;
            q = p;
        } else
            p++;
    }
    save_deny_reason(board, reason, count);
    RETURN_LONG(0);
}
