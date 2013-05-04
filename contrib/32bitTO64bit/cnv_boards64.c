/*******
.BOARDS 32bit->64bit 转化程序，修改自local/cnv_bh2.c
 ******/

#include "bbs.h"
#include "stdio.h"

struct boardheader_old {           /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char BM[BM_LEN];
    char title[STRLEN];
    unsigned level;
    unsigned int idseq;
    unsigned int clubnum; /*如果是俱乐部，这个是俱乐部序号*/
    unsigned int flag;
    union {
    unsigned int adv_club; /* 用于指定club对应关系,0表示没有,1表示序号*/
    unsigned int group_total; /*如果是二级目录，应该是目录的版面总数*/
    } board_data;
    int createtime;
/*    int toptitle; move to board status */
    int unused;
    char ann_path[128];
    int group; /*所属目录*/
    char title_level; /* 设定用户需要什么title可见这个版面*/
    char des[195]; /*版面描述,用于www的版面说明和search */
#ifdef FLOWBANNER
	int bannercount;
	char banners[MAXBANNER][BANNERSIZE];
#endif
};

struct boardheader_new {           /* This structure is used to hold data in */
    char filename[STRLEN];      /* the BOARDS files */
    char BM[BM_LEN];
    char title[STRLEN];
    unsigned level;
    unsigned int idseq;
    unsigned int clubnum; /*如果是俱乐部，这个是俱乐部序号*/
    unsigned int flag;
    union {
    unsigned int adv_club; /* 用于指定club对应关系,0表示没有,1表示序号*/
    unsigned int group_total; /*如果是二级目录，应该是目录的版面总数*/
    } board_data;
    time_t createtime;
/*    int toptitle; move to board status */
    int unused;
    char ann_path[128];
    int group; /*所属目录*/
    char title_level; /* 设定用户需要什么title可见这个版面*/
    char des[195]; /*版面描述,用于www的版面说明和search */
#ifdef FLOWBANNER
	int bannercount;
	char banners[MAXBANNER][BANNERSIZE];
#endif
};
int main(int argc, char *argv[])
{
    FILE *fp, *fp2, *fp3;
    struct boardheader_old bh[MAXBOARD];
    struct boardheader_new bhnew[MAXBOARD];
    char buf[128];
    int len;
    char *ptr = NULL;

    int i;

    if (argc != 3) {
        printf("usage: %s Old_BOARDS_FILE New_BOARDS_FILE\n", argv[0]);
        exit(0);
    }
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("open old board file %s failed!", argv[1]);
        exit(0);
    }

    if ((fp2 = fopen(argv[2], "w")) == NULL) {
        printf("cant create newboards file %s!", argv[2]);
        exit(0);
    }


    memset((void *) bh, 0, MAXBOARD * sizeof(struct boardheader_old));
    memset((void *) bhnew, 0, MAXBOARD * sizeof(struct boardheader_new));

    fread((void *) bh, sizeof(struct boardheader_old), MAXBOARD, fp);

    for (i = 0; i < MAXBOARD; i++) {
        memcpy(bhnew[i].filename, bh[i].filename, STRLEN);
        memcpy(bhnew[i].BM, bh[i].BM, BM_LEN);
        memcpy(bhnew[i].title, bh[i].title, STRLEN);
        bhnew[i].level = bh[i].level;
        bhnew[i].idseq = bh[i].idseq;
        bhnew[i].clubnum = bh[i].clubnum;
        bhnew[i].flag = bh[i].flag;
        bhnew[i].board_data.adv_club = bh[i].board_data.adv_club;
        bhnew[i].createtime = bh[i].createtime; //default
        bhnew[i].unused = bh[i].unused;     //none

        memcpy(bhnew[i].ann_path, bh[i].ann_path, 128);
        bhnew[i].group = bh[i].group;

        bhnew[i].title_level = bh[i].title_level;

        memcpy(bhnew[i].des, bh[i].des, 195);
#ifdef FLOWBANNER
        bhnew[i].bannercount = bh[i].bannercount;
        memcpy(bhnew[i].banners, bh[i].banners, sizeof(bh[i].banners));
#endif
        printf("Boards:%s OK!\n", bhnew[i].filename);
    }

    fwrite(bhnew, sizeof(struct boardheader_new), MAXBOARD, fp2);

    fclose(fp2);
    fclose(fp);
    return 0;
}

