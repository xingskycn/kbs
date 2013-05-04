#ifndef PHP_BBS_FILE_H
#define PHP_BBS_FILE_H

/* ���嵽һ�������ļ��Ĳ��� */

PHP_FUNCTION(bbs2_readfile);
PHP_FUNCTION(bbs2_readfile_text);
PHP_FUNCTION(bbs_file_output_attachment);
PHP_FUNCTION(bbs_printansifile);
#ifdef NFORUM
PHP_FUNCTION(bbs2_readfile_nforum);
PHP_FUNCTION(bbs_printansifile_noatt);
PHP_FUNCTION(bbs_printansifile_nforum);
#endif
PHP_FUNCTION(bbs_print_article);
PHP_FUNCTION(bbs_print_article_js);
PHP_FUNCTION(bbs_printoriginfile);
PHP_FUNCTION(bbs_originfile);
PHP_FUNCTION(bbs_decode_att_hash);
PHP_FUNCTION(bbs_parse_article);
PHP_FUNCTION(bbs_load_like);
PHP_FUNCTION(bbs_add_like);
PHP_FUNCTION(bbs_del_like);

#define PHP_BBS_FILE_EXPORT_FUNCTIONS_STD \
    PHP_FE(bbs2_readfile, NULL) \
    PHP_FE(bbs2_readfile_text, NULL) \
    PHP_FE(bbs_file_output_attachment, NULL) \
    PHP_FE(bbs_printansifile, NULL) \
    PHP_FE(bbs_print_article, NULL) \
    PHP_FE(bbs_print_article_js, NULL) \
    PHP_FE(bbs_printoriginfile, NULL) \
    PHP_FE(bbs_originfile, NULL) \
    PHP_FE(bbs_decode_att_hash, NULL) \
    PHP_FE(bbs_parse_article, NULL) \
    PHP_FE(bbs_load_like, NULL) \
    PHP_FE(bbs_add_like, NULL) \
    PHP_FE(bbs_del_like, NULL) \
     
#ifdef NFORUM
#define PHP_BBS_FILE_EXPORT_FUNCTIONS\
    PHP_BBS_FILE_EXPORT_FUNCTIONS_STD\
    PHP_FE(bbs2_readfile_nforum, NULL)\
    PHP_FE(bbs_printansifile_noatt, NULL)\
    PHP_FE(bbs_printansifile_nforum, NULL)
#else
#define PHP_BBS_FILE_EXPORT_FUNCTIONS PHP_BBS_FILE_EXPORT_FUNCTIONS_STD
#endif // NFORUM

#endif //PHP_BBS_FILE_H
