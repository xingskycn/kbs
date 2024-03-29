/*

match.c

Author: Tatu Ylonen <ylo@cs.hut.fi>

Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
                   All rights reserved

Created: Thu Jun 22 01:17:50 1995 ylo

Simple pattern matching, with '*' and '?' as wildcards.

*/

#include "includes.h"
#include "ssh.h"
#include "xmalloc.h"

/* Returns true if the given string matches the pattern (which may contain
   ? and * as wildcards), and zero if it does not match. */

int match_pattern(const char *s, const char *pattern)
{
    while (1) {
        /* If at end of pattern, accept if also at end of string. */
        if (!*pattern)
            return !*s;

        /* Process '*'. */
        if (*pattern == '*') {
            /* Skip the asterisk. */
            pattern++;

            /* If at end of pattern, accept immediately. */
            if (!*pattern)
                return 1;

            /* If next character in pattern is known, optimize. */
            if (*pattern != '?' && *pattern != '*') {
                /* Look instances of the next character in pattern, and try
                   to match starting from those. */
                for (; *s; s++)
                    if (*s == *pattern && match_pattern(s + 1, pattern + 1))
                        return 1;
                /* Failed. */
                return 0;
            }

            /* Move ahead one character at a time and try to match at each
               position. */
            for (; *s; s++)
                if (match_pattern(s, pattern))
                    return 1;
            /* Failed. */
            return 0;
        }

        /* There must be at least one more character in the string.  If we are
           at the end, fail. */
        if (!*s)
            return 0;

        /* Check if the next character of the string is acceptable. */
        if (*pattern != '?' && *pattern != *s)
            return 0;

        /* Move to the next character, both in string and in pattern. */
        s++;
        pattern++;
    }
 /*NOTREACHED*/}

/* Check that host name matches the pattern. If the pattern only contains
   numbers and periods, and wildcards compare it against the ip address
   otherwise assume it is host name */
int match_host(const char *host, const char *ip, const char *pattern)
{
    int is_ip_pattern;
    const char *p;

    /* if the pattern does not contain any alpha characters then
       assume that it is a IP address (with possible wildcards),
       otherwise assume it is a hostname */
    if (ip)
        is_ip_pattern = 1;
    else
        is_ip_pattern = 0;

    for (p = pattern; *p; p++)
        if (!(isdigit(*p) || *p == '.' || *p == '?' || *p == '*')) {
            is_ip_pattern = 0;
            break;
        }
    if (is_ip_pattern) {
        return match_pattern(ip, pattern);
    }
    return match_pattern(host, pattern);
}
