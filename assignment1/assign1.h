//
// Created by birdd on 9/20/2022.
//

//#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <sstream>
#include <string>
#include <map>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <signal.h>

#include"pfind.h"
#include "pcontrol.h"
#pragma once


#ifndef ASSIGN1_CPP_ASSIGN1_H
#define ASSIGN1_CPP_ASSIGN1_H
#define LINE_LENGTH 100     // Max # of character in an input line
#define MAX_ARGS 7          // Max number of arguments to a command
#define MAX_LENGTH 20       // Max # of characters in an arguements
#define MAX_PT_ENTRIES 32   // Max entries in the Process Table
#endif //ASSIGN1_CPP_ASSIGN1_H


int main(int argc,char *argv[]);


