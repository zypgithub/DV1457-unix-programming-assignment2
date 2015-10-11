/*************************************************************************
	> File Name: stringop.h
	> Author: 
	> Mail: 
	> Created Time: Sun 11 Oct 2015 01:29:59 PM CEST
 ************************************************************************/

#ifndef _STRINGOP_H
#define _STRINGOP_H

int get_file_type(char *, char *);
            
int split_filename_path(char *, char *, char *);

int parse_url(char *, char *, char *);

int get_substr(char *, char *, char);

int find_label(char *, char *, char *);

#endif
