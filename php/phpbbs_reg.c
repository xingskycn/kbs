#include "php_kbs_bbs.h"


PHP_FUNCTION(bbs_saveuserdata)
{
    char*   userid,
    *   realname,
    *   photo_url,
    *   address,
    * email,
    * phone,
    *   mobile_phone,
    * OICQ,
    * ICQ,
    * MSN,
    * homepage,
    * userface_url,
    * country,
    * province,
    * city,
    * graduate_school;
    int     userid_len,
    realname_len,
    photo_url_len,
    address_len,
    email_len,
    phone_len,
    mobile_phone_len,
    OICQ_len,
    ICQ_len,
    MSN_len,
    homepage_len,
    userface_url_len,
    country_len,
    province_len,
    city_len,
    graduate_school_len;
    long gender,
    year,
    month,
    day,
    userface_img,
    userface_width,
    userface_height,
    group,
    shengxiao,
    bloodtype,
    religion ,
    profession,
    married,
    education,
    character;
    zend_bool   bAuto;
    struct  userdata ud;
    int     usernum;

    int ac = ZEND_NUM_ARGS();


    if (ac != 32 || zend_parse_parameters(32 TSRMLS_CC, "sssllllssssssslslllsssllllllslsb", &userid,&userid_len,&realname,&realname_len,
                                          &address,&address_len,&gender,&year,&month,&day,&email,&email_len,&phone,&phone_len,&mobile_phone,&mobile_phone_len,
                                          &OICQ, &OICQ_len, &ICQ, &ICQ_len, &MSN, &MSN_len, &homepage, &homepage_len, &userface_img,
                                          &userface_url, &userface_url_len, &userface_width, &userface_height, &group, &country, &country_len,
                                          &province, &province_len, &city, &city_len, &shengxiao, &bloodtype, &religion, &profession,
                                          &married, &education, &graduate_school, &graduate_school_len, &character, &photo_url, &photo_url_len,&bAuto) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (userid_len > IDLEN)RETURN_LONG(2);

    usernum = searchuser(userid);
    if (0 == usernum)RETURN_LONG(3);

    if (userface_url_len!=0) {
        userface_img=-1;
        if ((userface_width<0) || (userface_width>120)) {
            RETURN_LONG(-1);
        }
        if ((userface_height<0) || (userface_height>120)) {
            RETURN_LONG(-2);
        }
    } else {
        userface_width=0;
        userface_height=0;
    }

    if (read_user_memo(userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);
    read_userdata(userid, &ud);
    strncpy(ud.realname, realname, NAMELEN);
    strncpy(ud.address, address, STRLEN);
    strncpy(ud.reg_email,email,STRLEN);
    strncpy(ud.OICQ,OICQ,STRLEN);
    strncpy(ud.ICQ,ICQ,STRLEN);
    strncpy(ud.MSN,MSN,STRLEN);
    strncpy(ud.homepage,homepage,STRLEN);
    strncpy(ud.userface_url,userface_url,STRLEN);
    strncpy(ud.country,country,STRLEN);
    strncpy(ud.province,province,STRLEN);
    strncpy(ud.city,city,STRLEN);
    strncpy(ud.graduateschool,graduate_school,STRLEN);
    strncpy(ud.telephone,phone,STRLEN);
    strncpy(ud.photo_url,photo_url,STRLEN);
    ud.photo_url[STRLEN-1]=0;
    ud.telephone[STRLEN-1]=0;
    ud.OICQ[STRLEN-1]=0;
    ud.ICQ[STRLEN-1]=0;
    ud.MSN[STRLEN-1]=0;
    ud.homepage[STRLEN-1]=0;
    ud.userface_url[STRLEN-1]=0;
    ud.country[STRLEN-1]=0;
    ud.province[STRLEN-1]=0;
    ud.city[STRLEN-1]=0;
    ud.graduateschool[STRLEN-1]=0;
    ud.realname[NAMELEN-1] = '\0';
    ud.address[STRLEN-1] = '\0';
    ud.reg_email[STRLEN-1] = '\0';


#ifdef HAVE_BIRTHDAY
    ud.birthyear=(year > 1900 && year < 2050)?(year-1900):0;
    ud.birthmonth=(month >=1 && month <=12)?month:0;
    ud.birthday=(day>=1 && day <=31)?day:0;
    if (gender==1)ud.gender='M';
    else
        ud.gender='F';
#endif
    ud.userface_img=userface_img;
    ud.userface_width=userface_width;
    ud.userface_height=userface_height;
    ud.group=group;
    ud.shengxiao=shengxiao;
    ud.bloodtype=bloodtype;
    ud.religion=religion;
    ud.profession=profession;
    ud.married=married;
    ud.education=education;
    ud.character=character;
    memcpy(&((getSession()->currentmemo)->ud), &ud, sizeof(ud));
    end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);
    write_userdata(userid, &ud);
    RETURN_LONG(0);

}

/**
 * Function: Create a registry form
 *  prototype:
 *
 * #ifdef HAVE_WFORUM
 *
 * int bbs_createregform(string userid ,string realname,string dept,string address,int gender,int year,int month,int day,
    string email,string phone,string mobile_phone,string OICQ, string ICQ, string MSN, string homepage, int userface_img,
 string userface_url,int userface_width, int userface_height, int group, string country ,string province, string city,
 int shengxiao, int bloodtype, int religion , int profession, int married, int education, string graduate_school,
 int character, bool bAuto)
 *
 * 或者 (wForum 用下面的方式调用表示重新填写注册单)
 * #endif
 *
 * int bbs_createregform(string userid ,string realname,string dept,string address,int gender,int year,int month,int day,
    string email,string phone,string mobile_phone,bool bAuto)
 *
 *
 *  bAuto : true -- 自动生成注册单,false -- 手工.
 *  @return the result
 *   0 -- success,
 *      1 -- 注册单尚未处理
 *      2 -- 参数错误
 *      3 -- 用户不存在
 *      4 -- 用户已经通过注册
 *      5 -- 不到时间
 *   10 -- system error
 *  @author binxun 2003.5
 */
PHP_FUNCTION(bbs_createregform)
{
    char*   userid,
    *   realname,
    *   dept,
    *   address,
    * email,
    * phone,
#ifdef HAVE_WFORUM
    * OICQ,
    * ICQ,
    * MSN,
    * homepage,
    * userface_url,
    * country,
    * province,
    * city,
    * graduate_school,
#endif
    *   mobile_phone;
    int     userid_len,
    realname_len,
    dept_len,
    address_len,
    email_len,
    phone_len,
    mobile_phone_len;
#ifdef HAVE_WFORUM
    int     OICQ_len,
    ICQ_len,
    MSN_len,
    homepage_len,
    userface_url_len,
    country_len,
    province_len,
    city_len,
    graduate_school_len;
    long userface_img,
    userface_width,
    userface_height,
    group,
    shengxiao,
    bloodtype,
    religion ,
    profession,
    married,
    education,
    character;
#endif
    long gender,
    year,
    month,
    day;
    zend_bool   bAuto;
    struct  userdata ud;
    struct  userec* uc;
    FILE*   fn;
    char    genbuf[STRLEN+1];
    char*   ptr;
    int     usernum;
    long    now;
#ifdef HAVE_WFORUM
    int     bReFill;
#endif
    int ac = ZEND_NUM_ARGS();


#ifdef HAVE_WFORUM
    bReFill = (ac == 12);
    if (ac != 32 || zend_parse_parameters(32 TSRMLS_CC, "ssssllllssssssslslllsssllllllslb", &userid,&userid_len,&realname,&realname_len,&dept,&dept_len,
                                          &address,&address_len,&gender,&year,&month,&day,&email,&email_len,&phone,&phone_len,&mobile_phone,&mobile_phone_len,
                                          &OICQ, &OICQ_len, &ICQ, &ICQ_len, &MSN, &MSN_len, &homepage, &homepage_len, &userface_img,
                                          &userface_url, &userface_url_len, &userface_width, &userface_height, &group, &country, &country_len,
                                          &province, &province_len, &city, &city_len, &shengxiao, &bloodtype, &religion, &profession,
                                          &married, &education, &graduate_school, &graduate_school_len, &character,&bAuto) == FAILURE)
#endif
        if (ac != 12 || zend_parse_parameters(12 TSRMLS_CC, "ssssllllsssb", &userid,&userid_len,&realname,&realname_len,&dept,&dept_len,
                                              &address,&address_len,&gender,&year,&month,&day,&email,&email_len,&phone,&phone_len,&mobile_phone,&mobile_phone_len,&bAuto) == FAILURE) {
            WRONG_PARAM_COUNT;
        }

    if (userid_len > IDLEN)RETURN_LONG(2);

    usernum = searchuser(userid);
    if (0 == usernum)RETURN_LONG(3);

#ifdef HAVE_WFORUM
    if (!bReFill) {
        if (userface_url_len!=0) {
            userface_img=-1;
            if ((userface_width<0) || (userface_width>120)) {
                RETURN_LONG(-1);
            }
            if ((userface_height<0) || (userface_height>120)) {
                RETURN_LONG(-2);
            }
        } else {
            userface_width=0;
            userface_height=0;
        }
    }
#endif

    //检查用户是否已经通过注册或者还不到时间(先放到这里,最好放到php里面)
    if (getuser(userid,&uc) == 0)RETURN_LONG(3);
    if (HAS_PERM(uc,PERM_LOGINOK))RETURN_LONG(4);

    if (!bAuto) {
        /* remed by roy 2003.7.17
        if(time(NULL) - uc->firstlogin < REGISTER_WAIT_TIME)RETURN_LONG(5);
        */
        //检查是否单子已经填过了
        if ((fn = fopen("new_register", "r")) != NULL) {
            while (fgets(genbuf, STRLEN, fn) != NULL) {
                if ((ptr = strchr(genbuf, '\n')) != NULL)
                    *ptr = '\0';
                if (strncmp(genbuf, "userid: ", 8) == 0 && strcmp(genbuf + 8, userid) == 0) {
                    fclose(fn);
                    RETURN_LONG(1);
                }
            }
            fclose(fn);
        }
    }
    if (read_user_memo(userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);
    read_userdata(userid, &ud);
    strncpy(ud.realname, realname, NAMELEN);
    strncpy(ud.address, address, STRLEN);
    strncpy(ud.reg_email,email,STRLEN);
    strncpy(ud.email,email,STRLEN);
    strncpy(ud.telephone,phone,STRLEN);
#ifdef HAVE_WFORUM
    if (!bReFill) {
        strncpy(ud.OICQ,OICQ,STRLEN);
        strncpy(ud.ICQ,ICQ,STRLEN);
        strncpy(ud.MSN,MSN,STRLEN);
        strncpy(ud.homepage,homepage,STRLEN);
        strncpy(ud.userface_url,userface_url,STRLEN);
        strncpy(ud.country,country,STRLEN);
        strncpy(ud.province,province,STRLEN);
        strncpy(ud.city,city,STRLEN);
        strncpy(ud.graduateschool,graduate_school,STRLEN);
        //strncpy(ud.telephone,phone,STRLEN);
        //ud.telephone[STRLEN-1]=0;
        ud.OICQ[STRLEN-1]=0;
        ud.ICQ[STRLEN-1]=0;
        ud.MSN[STRLEN-1]=0;
        ud.homepage[STRLEN-1]=0;
        ud.userface_url[STRLEN-1]=0;
        ud.country[STRLEN-1]=0;
        ud.province[STRLEN-1]=0;
        ud.city[STRLEN-1]=0;
        ud.graduateschool[STRLEN-1]=0;
        ud.userface_img=userface_img;
        ud.userface_width=userface_width;
        ud.userface_height=userface_height;
        ud.group=group;
        ud.shengxiao=shengxiao;
        ud.bloodtype=bloodtype;
        ud.religion=religion;
        ud.profession=profession;
        ud.married=married;
        ud.education=education;
        ud.character=character;
    }
#endif
    ud.realname[NAMELEN-1] = '\0';
    ud.address[STRLEN-1] = '\0';
    ud.reg_email[STRLEN-1] = '\0';
    ud.email[STRLEN-1] = '\0';
    ud.telephone[STRLEN-1]=0;

    if (strcmp(mobile_phone,"")) {
        ud.mobileregistered = true;
        strncpy(ud.mobilenumber,mobile_phone,MOBILE_NUMBER_LEN);
        ud.mobilenumber[MOBILE_NUMBER_LEN-1] = '\0';
    } else {
        ud.mobileregistered = false;
    }

#ifdef HAVE_BIRTHDAY
    ud.birthyear=(year > 1900 && year < 2050)?(year-1900):0;
    ud.birthmonth=(month >=1 && month <=12)?month:0;
    ud.birthday=(day>=1 && day <=31)?day:0;
    if (gender==1)ud.gender='M';
    else
        ud.gender='F';
#endif
    memcpy(&((getSession()->currentmemo)->ud), &ud, sizeof(ud));
    end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);
    write_userdata(userid, &ud);

    sprintf(genbuf,"%ld.%ld.%ld",year,month,day);
    if (bAuto) {
#ifdef NEWSMTH
        char buf[PATHLEN];
        sethomefile(buf, userid, "pre_register");
        fn = fopen(buf, "w");
#else
        fn = fopen("pre_register", "a");
#endif
    } else
        fn = fopen("new_register", "a");

    if (fn) {
        now = time(NULL);
        writew_lock(fileno(fn), 0, SEEK_SET, 0);
        fprintf(fn, "usernum: %d, %s", usernum, ctime(&now));
        fprintf(fn, "userid: %s\n", userid);
        fprintf(fn, "realname: %s\n", realname);
        fprintf(fn, "career: %s\n", dept);
        fprintf(fn, "addr: %s\n", address);
        fprintf(fn, "phone: %s\n", phone);
        fprintf(fn, "birth: %s\n", genbuf);
        fprintf(fn, "IP: %s\n", getSession()->fromhost);
        fprintf(fn, "----\n");
        un_lock(fileno(fn), 0, SEEK_SET, 0);
        fclose(fn);
        RETURN_LONG(0);
    } else
        RETURN_LONG(10);
}



/**
 * fill infomation of ID ,name, NO. dept, for tsinghua
 * prototype:
 * int bbs_fillIDInfo(string smthid,string name,string number,string dept);
 *
 *  @return the result
 *   0 -- success, -1 -- Invalid parameter
 *   -2 -- error
 *  @author binxun
 */
PHP_FUNCTION(bbs_fillidinfo)
{
    char* userid;
    int userid_len;
    char* realname;
    int realname_len;
    char* number;
    int number_len;
    char* dept;
    int dept_len;
    char genbuf[STRLEN];

    struct userdata ud;

    int ac = ZEND_NUM_ARGS();


    if (ac != 4 || zend_parse_parameters(4 TSRMLS_CC, "ssss", &userid, &userid_len,&realname,&realname_len,&number,&number_len,&dept,&dept_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (userid_len > IDLEN || realname_len > NAMELEN || dept_len > STRLEN)
        RETURN_LONG(-1);

    memset(&ud,0,sizeof(ud));
    if (read_user_memo(userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);

    if (read_userdata(userid,&ud) < 0)RETURN_LONG(-2);

    strncpy(ud.realname, realname, NAMELEN);
    strncpy(ud.address,dept,STRLEN);
    sprintf(genbuf,"%s#%s#%s#TH",realname,number,dept);
    if (strlen(genbuf) >= STRLEN - 16) //too long
        sprintf(genbuf,"%s#%s#TH",realname,number);//must < STRLEN - 16
    strncpy(ud.realemail,genbuf,STRLEN-16);

    memcpy(&((getSession()->currentmemo)->ud), &ud, sizeof(ud));
    end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);

    if (write_userdata(userid,&ud) < 0)RETURN_LONG(-2);

    bbslog("user","%s","new account from tsinghua www");

    RETURN_LONG(0);
}



/* bbsinfo.php, stiger */
PHP_FUNCTION(bbs_modify_info)
{
    char* username;
    int username_len;
    char* realname;
    int realname_len;
    char* address;
    int address_len;
    char* email;
    int email_len;

    struct userdata ud;
    struct userec newinfo;
    int unum;
    int ac = ZEND_NUM_ARGS();

    if (ac != 4 || zend_parse_parameters(4 TSRMLS_CC, "ssss", &username, &username_len,&realname,&realname_len,&address,&address_len,&email,&email_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    process_control_chars(username,NULL);
    process_control_chars(realname,NULL);
    process_control_chars(address,NULL);
    process_control_chars(email,NULL);

    if (username_len >= NAMELEN || realname_len >= NAMELEN || address_len >= STRLEN || email_len>= STRLEN)
        RETURN_LONG(-1);

    memset(&ud,0,sizeof(ud));
    if (read_user_memo(getCurrentUser()->userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);

    if (read_userdata(getCurrentUser()->userid,&ud) < 0)RETURN_LONG(-2);

    if (strcmp(ud.realname, realname)) {
        newbbslog(BBSLOG_USER, "ChangeName '%s' to1 '%s'", ud.realname, realname);
    }

    strncpy(ud.realname, realname, NAMELEN);
    strncpy(ud.address,address,STRLEN);
    strncpy(ud.email,email,STRLEN);

    memcpy(&((getSession()->currentmemo)->ud), &ud, sizeof(ud));
    end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);

    if (write_userdata(getCurrentUser()->userid,&ud) < 0)RETURN_LONG(-2);

    if ((unum = searchuser(getCurrentUser()->userid))==0)
        RETURN_LONG(-1);
    memcpy(&newinfo, getCurrentUser(), sizeof(struct userec));
    if (strcmp(newinfo.username, username)) {

        strcpy(newinfo.username, username);
        update_user(&newinfo, unum, 1);

        strcpy(getSession()->currentuinfo->username, username);
        UPDATE_UTMP_STR(username, (*(getSession()->currentuinfo)));
    }

    bbslog("user","%s","change user info from www");

    RETURN_LONG(0);
}




/**
 * Function: Create a new user id
 *  rototype:
 * int bbs_createNewID(string smthid,string passwd,string nickname);
 *
 *  @return the result
 *   0 -- success, 1 -- specail char or first char not alpha
 *   2 -- at least two chars 3 -- system name or bad name
 *   4 -- have been used 5 -- length > IDLEN
 *   6 -- passwd is too long > 39
 *  . 7 -- IP is baned
 *   10 -- system error
 *  @author binxun
 */
PHP_FUNCTION(bbs_createnewid)
{
    char* userid;
    int userid_len;
    char* passbuf;
    int passbuf_len;
    char* nickname;
    int nickname_len;
    char buf[1024];
    char tmpstr[30];
    struct stat lst;
    time_t lnow;
    struct userec newuser;
    int allocid;

    int ac = ZEND_NUM_ARGS();

    if (ac != 3 || zend_parse_parameters(3 TSRMLS_CC, "sss", &userid, &userid_len,&passbuf,&passbuf_len,&nickname,&nickname_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (userid_len > IDLEN)RETURN_LONG(5);
    if (passbuf_len > 39)RETURN_LONG(6);

    if (id_invalid(userid) == 1) RETURN_LONG(1);
    if (strlen(userid) < 2) RETURN_LONG(2);
    if (bad_user_id(userid)) RETURN_LONG(3);
    if (searchuser(userid)) RETURN_LONG(4);

#ifdef SECONDSITE
    RETURN_LONG(4);
#endif

    //if(check_ban_IP(getSession()->fromhost,buf) < 0)RETURN_LONG(7);

    lnow = time(NULL);
    sethomepath(buf,userid);
    //存在前人的目录,并且还在保存期限以内
    if (!stat(buf,&lst) && S_ISDIR(lst.st_mode) && (lnow-lst.st_ctime < SEC_DELETED_OLDHOME)) {
        //log?

        RETURN_LONG(10);
    }

    memset(&newuser,0,sizeof(newuser));
    strncpy(newuser.lasthost,getSession()->fromhost,IPLEN);
    newuser.lasthost[IPLEN-1]=0;
    strncpy(newuser.userid ,userid,IDLEN);
    strncpy(newuser.username,nickname,NAMELEN-1);
    newuser.username[NAMELEN-1] = '\0';

    newuser.firstlogin = newuser.lastlogin = time(NULL);

    setpasswd(passbuf,&newuser);

    newuser.userlevel = PERM_AUTOSET;
    newuser.userdefine[0] = -1;
    newuser.userdefine[1] = -1;
    SET_UNDEFINE(&newuser,DEF_NOTMSGFRIEND);
    SET_UNDEFINE(&newuser, DEF_SHOWREALUSERDATA);

    newuser.exittime = time(NULL) - 100;
    newuser.flags |= PAGER_FLAG;
    newuser.title = 0;

    //分配ID号
    allocid = getnewuserid2(newuser.userid);
    if (allocid > MAXUSERS || allocid <= 0) RETURN_LONG(10);

    //更新共享内存数据
    update_user(&newuser,allocid,1);

    if (!getuser(newuser.userid,&getCurrentUser()))RETURN_LONG(10);

    newbbslog(BBSLOG_USIES,"%s","new account from www");

    //检查是否有前人的信件
    sethomepath(tmpstr,userid);
    my_f_rm(tmpstr);
    /*
     sprintf(buf,"/bin/mv -f %s " BBSHOME "/homeback/%s",tmpstr,userid);
     system(buf);
    */
    setmailpath(tmpstr,userid);
    my_f_rm(tmpstr);
    /*
     sprintf(buf,"/bin/mv -f %s " BBSHOME "/mailback/%s",tmpstr,userid);
     system(buf);
    */

    //创建新目录
    sethomepath(tmpstr,userid);
    if (mkdir(tmpstr,0755) < 0) {
        bbslog("3error","create id %s home dir error:%s",userid,strerror(errno));
        RETURN_LONG(10);
    }

#ifdef NEWSMTH
    mail_file("SYSOP","etc/tonewuser",userid,"致新注册用户的信",0,NULL);
#elif defined(ZIXIA)
    mail_file(DELIVER,"etc/tonewuser",userid,"致新注册用户的信",0,NULL);
#endif

#ifdef NEW_COMERS
    {
        FILE *fout;
        char buf2[STRLEN],buf[STRLEN];
        sprintf(buf, "tmp/newcomer.%s",userid);
        if ((fout = fopen(buf, "w")) != NULL) {
            fprintf(fout, "大家好,\n\n");
            fprintf(fout, "我是 %s (%s), 来自 %s\n", userid,
                    nickname, SHOW_USERIP(getCurrentUser(), getSession()->fromhost));
            fprintf(fout, "今天%s初来此站报到, 请大家多多指教。\n",
#if 0 //TODO here
                    (ud.gender == 'M') ? "小弟" : "小女子");
#else
                    "小弟");
#endif
            fprintf(fout, "\n\n我是www注册用户~~~\n\n");
            fclose(fout);
            sprintf(buf2, "新手上路: %s", nickname);
            post_file(getCurrentUser(), "", buf, "newcomers", buf2, 0, 2, getSession());
            unlink(buf);
        }
    }
#endif

    RETURN_LONG(0);
}


/**
 * Function: check if the id is invalid
 *  rototype:
 * int bbs_is_invalid_id(string smthid);
 *
 *  @return the result
 *   0 -- valid ID
 *      1 -- specail char or first char not alpha
 *   2 -- at least two chars
 *      3 -- system name or bad name
 *   4 -- have been used
 *      5 -- length > IDLEN
 *  @author atppp
 */
PHP_FUNCTION(bbs_is_invalid_id)
{
    char* userid;
    int userid_len;
    int ac = ZEND_NUM_ARGS();

    if (ac != 1 || zend_parse_parameters(1 TSRMLS_CC, "s", &userid, &userid_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (userid_len > IDLEN)RETURN_LONG(5);

    if (id_invalid(userid) == 1) RETURN_LONG(1);
    if (strlen(userid) < 2) RETURN_LONG(2);
    if (searchuser(userid)) RETURN_LONG(4);
    if (bad_user_id(userid)) RETURN_LONG(3);
    RETURN_LONG(0);
}


#ifdef HAVE_ACTIVATION

PHP_FUNCTION(bbs_sendactivation)
{
    struct activation_info ai;
    char* userid;
    int   userid_len;
    struct userec *uc;

    int ac = ZEND_NUM_ARGS();

    if (ac != 1 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s" , &userid , &userid_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (getuser(userid,&uc)==0)
        RETURN_LONG(-1);
    getactivation(&ai, uc);
    RETURN_LONG(sendactivation(&ai, uc, getSession()));
}

PHP_FUNCTION(bbs_doactivation)
{
    struct activation_info ai;
    char* userid;
    int   userid_len;
    struct userec *uc;

    int ac = ZEND_NUM_ARGS();

    if (ac != 1 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "s" , &userid , &userid_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (getuser(userid,&uc)==0)
        RETURN_LONG(-1);
    getactivation(&ai, uc);
    doactivation(&ai, uc, getSession());
    RETURN_LONG(0);
}

/**
* set user's activation file
* function bbs_setactivation(string userid , string filebody)
* return  0 : seccess
*         -1: user not exist
*         -2: registered
*         -10:system error
**/
PHP_FUNCTION(bbs_setactivation)
{
    char* userid;
    int   userid_len;
    char* filebody;
    int   filebody_len;
    struct userec *uc;
    FILE *fn;
    char  afile[STRLEN];

    int ac = ZEND_NUM_ARGS();

    if (ac != 2 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "ss" , &userid , &userid_len , &filebody , &filebody_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (getuser(userid,&uc)==0)
        RETURN_LONG(-1);
    sethomefile(afile,uc->userid,"activation");
    if ((fn=fopen(afile,"w"))==NULL)
        RETURN_LONG(-10);
    fprintf(fn,"%s",filebody);
    fclose(fn);
    RETURN_LONG(0);
}

/*
** function bbs_getactivation(string userid,&string &activation)
*  return  0 :seccess;
*          -1:user not exist
*          -2:registered
*          -3:can not find activation file
*          -10:system error
*/
PHP_FUNCTION(bbs_getactivation)
{
    char* userid;
    int   userid_len;
    zval *activation;
    struct userec *uc;
    char  buf[200];
    struct activation_info ai;

    int ac = ZEND_NUM_ARGS();

    if (ac != 2 || zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sz" , &userid , &userid_len , &activation) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    if (!PZVAL_IS_REF(activation)) {
        zend_error(E_WARNING, "Parameter wasn't passed by reference");
        RETURN_FALSE;
    }
    if (getuser(userid,&uc)==0)
        RETURN_LONG(-1);
    if (!getactivation(&ai, uc)) RETURN_LONG(-1);
    buf[0] = ai.activated ? '1' : '0';
    memcpy(buf + 1, ai.activationcode, ACTIVATIONLEN);
    strcpy(buf + 1 + ACTIVATIONLEN, ai.reg_email);
    ZVAL_STRING(activation,buf,1);
    RETURN_LONG(0);
}
#endif /* HAVE_ACTIVATION */


#if defined(NEWSMTH) && defined(HAVE_ACTIVATION)

PHP_FUNCTION(bbs_getinvite_email)
{
    struct invite in;
    char *id;
    char *passwd;
    int id_len, passwd_len;

    if (zend_parse_parameters(2 TSRMLS_CC, "ss", &id, &id_len, &passwd, &passwd_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }

    if (id[0] == '/' || strchr(id,'.'))
        RETURN_LONG(0);

    memset(&in, 0, sizeof(in));
    if (get_invite(id, passwd, &in) <= 0) {
        RETURN_LONG(0);
    }

    RETURN_STRING(in.email, 1);

}

PHP_FUNCTION(bbs_cleaninvite)
{
    char *userid;
    char *inviteid;
    int userid_len, inviteid_len;
    int ret;

    if (zend_parse_parameters(2 TSRMLS_CC, "ss", &userid, &userid_len, &inviteid, &inviteid_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }

    ret = clean_invite(userid, inviteid);

    RETURN_LONG(ret);
}

PHP_FUNCTION(bbs_invite)
{
    char *email, *word;
    int email_len, word_len, ret;
    struct userdata ud;

    if (zend_parse_parameters(2 TSRMLS_CC, "ss", &email, &email_len, &word, &word_len) != SUCCESS) {
        WRONG_PARAM_COUNT;
    }

    if (!HAS_PERM(getCurrentUser(), PERM_POST)) {
        RETURN_LONG(-1);
    }
    
    if (read_userdata(getCurrentUser()->userid, &ud) < 0) {
        RETURN_LONG(-2);
    }

    if ((ud.lastinvite > time(NULL) - 86400) && !HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        RETURN_LONG(-3);
    }

    if (invalidaddr(email) || !strstr(email, "@") || !strstr(email, ".")) {
        RETURN_LONG(-4);
    }
    
    ret = send_invite(getCurrentUser(), getSession(), email, word);
    if(ret >= 0) {
        FILE *fout;
        char buf2[STRLEN], buf3[STRLEN];

        ud.lastinvite=time(NULL);
        write_userdata(getCurrentUser()->userid, &ud);

        sprintf(buf3, "tmp/sendinvite.%s",getCurrentUser()->userid);
        if ((fout = fopen(buf3, "w")) != NULL) {
            fprintf(fout, "我的 IP      : %s\n", getSession()->fromhost);
            fclose(fout);
            sprintf(buf2, "%s邀请%s (WEB)", getCurrentUser()->userid, email);
            post_file(getCurrentUser(), "", buf3, "Invite", buf2, 0, 2, getSession());
            unlink(buf3);
        }
    }
    else {
        RETURN_LONG(-5);
    }

    RETURN_LONG(0);
}


PHP_FUNCTION(bbs_autopass)
{
    char* userid;
    int userid_len;
    char *mobile;
    int mobile_len;

    const char *field[] = { "usernum", "userid", "realname", "career",
        "addr", "phone", "birth", "IP", NULL
    };
    const char *finfo[] = { "帐号位置", "申请代号", "真实姓名", "服务单位",
        "目前住址", "联络电话", "生    日", "注册 IP ", NULL
    };
    char buf[PATHLEN], fdata[9][STRLEN], *ptr;
    int n;
    FILE *fh, *fout;

    struct userdata ud;
    struct userec *user, *user_sysop;

    int ac = ZEND_NUM_ARGS();


    if (ac != 2 || zend_parse_parameters(2 TSRMLS_CC, "ss", &userid, &userid_len, &mobile, &mobile_len) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    if (userid_len > IDLEN)
        RETURN_LONG(-1);


    if (!getuser(userid, &user))
        RETURN_LONG(-2);

    if (!(user->flags & ACTIVATED_FLAG))
        RETURN_LONG(-3);

    if (user->userlevel & PERM_LOGINOK)
        RETURN_LONG(-4);

    memset(fdata, 0, sizeof(fdata));
    sethomefile(buf, user->userid, "pre_register");
    if (!(fh = fopen(buf, "r")))
        RETURN_LONG(-2);
    while (fgets(buf, STRLEN, fh)) {
        if ((ptr = (char *) strstr(buf, ": ")) != NULL) {
            *ptr = '\0';
            for (n = 0; field[n] != NULL; n++) {
                if (strcmp(buf, field[n]) == 0) {
                    strcpy(fdata[n], ptr + 2);
                    if ((ptr = (char *) strchr(fdata[n], '\n')) != NULL)
                        *ptr = '\0';
                }
            }
        } else {
            fclose(fh);
            memcpy(fdata[5], mobile, mobile_len);
            memset(&ud,0,sizeof(ud));
            if (read_user_memo(user->userid, &(getSession()->currentmemo)) <= 0) RETURN_LONG(-2);

            if (read_userdata(user->userid,&ud) < 0)RETURN_LONG(-2);

            sprintf(buf,"%s#%s@手机",fdata[3],fdata[5]);
            if (strlen(buf) >= STRLEN - 16) //too long
                sprintf(buf,"%s@手机",fdata[5]);//must < STRLEN - 16
            strncpy(ud.realemail,buf,STRLEN-16);

            memcpy(&((getSession()->currentmemo)->ud), &ud, sizeof(ud));
            end_mmapfile((getSession()->currentmemo), sizeof(struct usermemo), -1);

            if (write_userdata(user->userid,&ud) < 0)RETURN_LONG(-2);

            user->userlevel |= PERM_DEFAULT;

            sethomefile(buf, user->userid, "register");
            if ((fout = fopen(buf, "w")) != NULL) {
                for (n = 0; field[n] != NULL; n++)
                    fprintf(fout, "%s     : %s\n", finfo[n], fdata[n]);
                fprintf(fout, "您的昵称     : %s\n", user->username);
                fprintf(fout, "电子邮件信箱 : %s\n", ud.email);
                fprintf(fout, "真实 E-mail  : %s\n", ud.realemail);
                fprintf(fout, "注册日期     : %s\n", ctime(&(user->firstlogin)));
                fprintf(fout, "Approved: SYSOP\n");
                fclose(fout);
            }

            if ((fout = fopen("register.list", "a")) != NULL) {
                time_t now;

                for (n = 0; field[n] != NULL; n++)
                    fprintf(fout, "%s: %s\n", field[n], fdata[n]);
                now = time(NULL);
                fprintf(fout, "Date: %s\n", Ctime(now));
                fprintf(fout, "Approved: SYSOP\n");
                fprintf(fout, "----\n");
                fclose(fout);
            }

            getuser("SYSOP", &user_sysop);
            setCurrentUser(user_sysop);
            mail_file("SYSOP", "etc/s_fill", user->userid, "恭禧你，你已经完成注册。", BBSPOST_LINK, NULL);
            sprintf(buf, "自动处理程序[手机] 让 %s 通过身份确认.", user->userid);
            securityreport(buf, user, fdata, getSession());
            bbslog("user","%s","new account from mobile reg");
            

            sethomefile(buf, user->userid, "pre_register");
            unlink(buf);
            RETURN_LONG(0);
        }
    }
    RETURN_LONG(-2);
}

#endif /* defined(NEWSMTH) && defined(HAVE_ACTIVATION) */


