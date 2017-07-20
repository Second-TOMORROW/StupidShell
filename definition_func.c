
//***************************************************************
//
//@file: definition_func.c
//
//@author: Second_TOMORROW
//
//@contact:second_tomorrow@163.com
//
//@date:July,2017
//
//@brief:
//implementation of all API functions
//
//***************************************************************



#include "stupidShell.h"


//current directory of StupidShell
char dirPromt[20];

//list of builtin commands
char *builtinStr[]={"cd","help","exit"};

int shm_id;
char *shm_buff;
int sp_shm_init(void)
{
	int key;

	key = ftok("StupidShell",0);
	shm_id = shmget(key,SHM_MEM_SIZE,IPC_CREAT|0666);
	shm_buff = (char *)shmat(shm_id,NULL,0);
	pool = (cmdPoolStruct *)shm_buff;
	
	return 0;
}


int sp_init()
{
	char *dirArr[10];
	char dir[200];
	int i;

	pool->cmdPtr=0;
	getcwd(dir,200);

	sp_splitStr(dirArr,dir,"/");

	//find current directory string
	for(i=0; ;i++)
	{
		if(NULL == dirArr[i])
			break;
	}

	strcpy( dirPromt , dirArr[i-1] );

	return 0;
}


int sp_cd(void)
{
	int i;
	char *dirArr[10];
	char dir[200];
	
	if( 0 != chdir(pool->cmdPool[0].cmd[1]) )
	{
		perror("chdir");
		return -1;
	}
	else
	{
		getcwd(dir,200);
		sp_splitStr(dirArr,dir,"/");

		//find current directory string
		for(i=0; ;i++)
		{
			if(NULL == dirArr[i])
				break;
		}

		strcpy( dirPromt , dirArr[i-1] );

		return 0;
	}
	
}


int sp_help(void)
{
	printf("StupidShell version 1.0 .\n");
	printf("last update : July,2017\n");
	printf("@author: Second_TOMORROW\n");
	printf("@contact:second_tomorrow@163.com\n");
	printf("Type 'exit' to exit .\n");

	return 0;
}


int sp_exit(void)
{
	printf("exiting...\n");

	shmdt(shm_buff);
	shmctl(shm_id,IPC_RMID,0);

	exit(EXIT_SUCCESS);
	return 0;
}


int sp_prompt()
{
	printf("\033[0;33m%s\033[0m",dirPromt);
	printf("\033[0;31m$\033[0m");

	return 0;
}


int sp_readLine(char * line)
{
	int position = 0;
	char c;

	while (1)
	{
		c = getchar();

		if (c == '\n')
		{
			line[position] = '\n';
			break;
		}
		else
		{
			line[position] = c;
		}
		position++;
	}

	return 0;
}



int sp_subString(char *resultStr, char *str , int origin , int end)
{
	int i,j=0;
	for(i=origin;i<=end;i++)
	{
		resultStr[j++]=str[i];
	}
	resultStr[j]='\0';

	return 0;
}


int sp_splitStr(char *resultArr[], char *str , char *split)
{
	int i,position = 0;
	char *token;

	token = strtok(str, split);
	while (token != NULL)
	{
		resultArr[position]=token;

		position++;
		token = strtok(NULL, split);
	}
	resultArr[position] = NULL;

	return 0;
}


int sp_analyCmd(char * line )
{
	int i,j=0;
	int lastPosition=0;

	for(i=0; '\n'!=line[i] ;i++)
	{
		if( '|'==line[i] || '<'==line[i] || '>'==line[i] )
		{
			sp_subString(pool->cmdPool[j].cmdStr,line,lastPosition,i-1);
			sp_splitStr(pool->cmdPool[j].cmd , pool->cmdPool[j].cmdStr, " ");
			pool->cmdPool[j].nextSign=line[i];
			lastPosition=i+1;
			j++;
		}
	}
	sp_subString(pool->cmdPool[j].cmdStr,line,lastPosition,i-1);
	sp_splitStr(pool->cmdPool[j].cmd , pool->cmdPool[j].cmdStr, " ");

	return 0;
}

int sp_builtinCmd(void)
{
	int i;
	
	for(i=0;i<3;i++)
	{
		if(0==strcmp(builtinStr[i],pool->cmdPool[0].cmd[0]) )
			break;
	}

	switch(i)
	{
	case 0:
		sp_cd();
		return 0;
		break;
	case 1:
		sp_help();
		return 0;
		break;
	case 2:
		sp_exit();
		return 0;
		break;
	default:
		return -1;
		break;
	}
	
}




int fd[2];
int sp_execute()
{
	int pid,localPtr;

	if('|'==pool->cmdPool[pool->cmdPtr].nextSign) 
	{	
		localPtr=pool->cmdPtr;
		pipe(fd);

		pid=fork();

		if(0==pid)//execute next command in child process
		{
			pool->cmdPtr++;

			dup2(fd[0],0);//redirect standard input to pipe(read)
			close(fd[0]);
			close(fd[1]);

			sp_execute();
			exit(EXIT_SUCCESS);
		}
		else//execute current command in current process 
		{
			signal(SIGCHLD, SIG_IGN);

			dup2(fd[1],1);//redirect standard output to pipe(write)
			close(fd[0]);
			close(fd[1]);

			if(0!=execvp(pool->cmdPool[localPtr].cmd[0],pool->cmdPool[localPtr].cmd))
				printf("No such command!\n");

			exit(EXIT_SUCCESS);
		}
	}
	else if('<'==pool->cmdPool[pool->cmdPtr].nextSign)
	{
		localPtr=pool->cmdPtr;
		char fileName[20];
		
		strcpy(fileName,pool->cmdPool[localPtr+1].cmd[0]);
		freopen(fileName,"r",stdin);//redirect stdin to fileName

		if('>'==pool->cmdPool[pool->cmdPtr+1].nextSign)
		{
			strcpy(fileName,pool->cmdPool[localPtr+2].cmd[0]);
			freopen(fileName,"w",stdout);//redirect stdout to fileName
		}
		if('|'==pool->cmdPool[pool->cmdPtr+1].nextSign)
		{
			pipe(fd);
			pid=fork();
			if(0==pid)//execute next command in child process
			{
				pool->cmdPtr+=2;

				dup2(fd[0],0);//redirect standard input to pipe(read)
				close(fd[0]);
				close(fd[1]);

				sp_execute();
				exit(EXIT_SUCCESS);
			}
			else//execute current command in current process
			{
				
				signal(SIGCHLD, SIG_IGN);

				dup2(fd[1],1);//redirect stdout to pipe(write)
				close(fd[0]);
				close(fd[1]);

				if(0!=execvp(pool->cmdPool[localPtr].cmd[0],pool->cmdPool[localPtr].cmd))
					printf("No such command!\n");

				exit(EXIT_SUCCESS);
			}
			
		}
		if(0!=execvp(pool->cmdPool[localPtr].cmd[0],pool->cmdPool[localPtr].cmd))
			printf("No such command!\n");

		exit(EXIT_SUCCESS);
		
	}
	else if('>'==pool->cmdPool[pool->cmdPtr].nextSign)
	{
		localPtr=pool->cmdPtr;
		char fileName[20];

		strcpy(fileName,pool->cmdPool[localPtr+1].cmd[0]);
		freopen(fileName,"w",stdout);//redirect stdout to fileName
		if(0!=execvp(pool->cmdPool[localPtr].cmd[0],pool->cmdPool[localPtr].cmd))
			printf("No such command!\n");
		
		exit(EXIT_SUCCESS);
	}
	else
	{
		localPtr=pool->cmdPtr;
		if(0!=execvp(pool->cmdPool[localPtr].cmd[0],pool->cmdPool[localPtr].cmd))
			printf("No such command!\n");

		exit(EXIT_SUCCESS);
	}

	return 0;

}


int sp_clearCmdPool(cmdPoolStruct * pool)
{
	int i,j;

	pool->cmdPtr=0;
	for(i=0;i<POOL_SZ;i++)
	{
		for(j=0;j<CMD_LENG;j++)
		{
			pool->cmdPool[i].cmd[j]=NULL;
			pool->cmdPool[i].cmdStr[0]='\0';
		}
		pool->cmdPool[i].nextSign=0;
	}
	
	return 0;
}
