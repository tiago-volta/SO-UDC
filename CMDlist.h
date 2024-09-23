//
// Created by pablojhd on 16/09/24.
//

#include <fcntl.h>

#ifndef CMDLIST_H
#define CMDLIST_H

#include "functions.h"


typedef struct {
    int fd;
    //char name[LENGTH_MAX];
    //char mode[LENGTH_MAX];
}OpenFile;


void ShowOpenFiles();

#endif //CMDLIST_H
