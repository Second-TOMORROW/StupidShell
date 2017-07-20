//***************************************************************
//
//@file: stupidShell.h
//
//@author: Second_TOMORROW
//
//@contact:second_tomorrow@163.com
//
//@date:July,2017
//
//@brief:
//header file of StupidShell
//including declaration of variables and function
//
//***************************************************************



#ifndef __STUPIDSHELL_H__

#define __STUPIDSHELL_H__

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define POOL_SZ 10          //The size of the cmdPool
#define CMD_LENG 8          //The max_length of a simple cmd ( = the amount of parameters + 1 )
#define PARA_MAX 64         //The max_length of a parameter
#define LINE_MAX 5120       //The max length of a line from input
#define SHM_MEM_SIZE 10240  //The size of share memory


//a simple comand structure
typedef struct CMD_STRUCT
{
	char *cmd[CMD_LENG];
	char cmdStr[CMD_LENG*PARA_MAX];
	char nextSign; // '|' or'>' or '<'
}cmdStruct;



//command pool including N simple command and pointer
typedef struct CMD_POOL_STRUCT
{
	cmdStruct cmdPool[POOL_SZ];
	int cmdPtr;
}cmdPoolStruct;

extern cmdPoolStruct *pool;


//************************************************************************
//
//@brief initialize share memory
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_shm_init(void);



//************************************************************************
//
//@brief initialize prompt
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_init(void);



//************************************************************************
//
//@brief builtin command,change the work directory
//
//@params
//
//@return 0==successfull;-1==fail
//
//************************************************************************
int sp_cd(void);



//************************************************************************
//
//@brief builtin command,printf tips and version 
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_help(void);



//************************************************************************
//
//@brief builtin command,free resources and exit stupidShell
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_exit(void);



//************************************************************************
//
//@brief printf formatted prompt
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_prompt(void);



//************************************************************************
//
//@brief read a line from standard input
//
//@params line :a char pointer to the space of saving a line of input
//
//@return reserve
//
//************************************************************************
int sp_readLine(char *line);



//************************************************************************
//
//@brief copy a substring from a string to another char array
//
//@params resultStr:the pointer of destination array , str:the pointer of
//        source string , origin: the copy origin in source , end: the
//        end point of copy 
//
//@return reserve
//
//************************************************************************
int sp_subString(char *resultStr , char *str , int origin , int end);



//************************************************************************
//
//@brief split a string according to some char 
//
//@params resultArr[]:pointer array pointing to the substring splited,
//        str:pointer of source string ,split:pointer of spliting char
//
//@return reserve
//
//************************************************************************
int sp_splitStr(char *resultArr[] , char *str , char *split);



//************************************************************************
//
//@brief analyse and format the command
//
//@params line:command string
//
//@return reserve
//
//************************************************************************
int sp_analyCmd(char *line);



//************************************************************************
//
//@brief execute builtin command
//
//@params
//
//@return 0:it was a builtin command and exeuted successfully, -1:it was
//        not builtin command and executed fail
//
//************************************************************************
int sp_builtinCmd(void);



//************************************************************************
//
//@brief execute exiternal command with recursion 
//In this project, command including pipe and redirection is called
//complicated command such as ls<a.txt>b.txt , ls|grep a and so on.
//And complicated command will be split into simple command like ls , a.txt , b.txt .
//Strictly speaking , files like a.txt are not commands which can be executed .
//So , files will not be executed as command but redirected to standard input or output
//of last simple comand . What’s more , builtin commands don’t support pipe and redirection.
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_execute(void);



//************************************************************************
//
//@brief clear the command pool
//
//@params
//
//@return reserve
//
//************************************************************************
int sp_clearCmdPool(cmdPoolStruct * pool );



#endif
