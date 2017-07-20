//***************************************************************
//
//@file: main.c
//
//@author: Second_TOMORROW
//
//@contact:second_tomorrow@163.com
//
//@date:July,2017
//
//@brief:
//main function of StupidShell
//
//***************************************************************


#include "stupidShell.h"

cmdPoolStruct * pool;

int main()
{
	char line[LINE_MAX];
	int pid;
	
	
	sp_shm_init();
	sp_init();
	while(1)
	{
		sp_prompt();

		sp_readLine(line);

		sp_analyCmd(line);


		 //if it is successfully to execute builtin command ,
		//clear command pool and restart to read new command
		if(0==sp_builtinCmd())           
		{
			sp_clearCmdPool(pool);
			continue;
		}
		//otherwise, fork a new process to execute as external command
		else
		{
			pid = fork();
			if(0==pid)
			{
				
				sp_execute();
			}

			waitpid(pid,NULL,0);
		}


		 //It is necessary to wait for long enough time before clear command pool
		//Because ,it doesn't use method of process synchronization to ensure
		//that clearing command is after all the command having been executed in other descendant process 
		sleep(1); 
		sp_clearCmdPool(pool);
	}
	

	return 0;

} 

