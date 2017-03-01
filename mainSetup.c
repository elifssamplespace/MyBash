#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
 
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */

int setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
           
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);  

    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */
   


    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }

	// printf(">>%s\n<<",inputBuffer);

    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */

        switch (inputBuffer[i]){
	    case ' ':
	     case '\t' :               /* argument separators */
		if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
		    ct++;
		}
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
		start = -1;
		break;

            case '\n':                 /* should be the final char examined */
		if (start != -1){
                    args[ct] = &inputBuffer[start];     
		    ct++;
		}
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
		break;

	    default :             /* some other character */
		if (start == -1)
		    start = i;
                if (inputBuffer[i] == '&' && i==length-2){
		    *background  = 1;
                    inputBuffer[i] = '\0';
		}
	} /* end of switch */
     }    /* end of for */
     // args[ct] = NULL; /* just in case the input line was > 80 */

  if(*background){
     args[ct-1] = NULL; /* just in case the input line was > 80 */
    }

// for (i = 0; i <= ct; i++)
// 		printf("args %d = %s\n",i,args[i]);

return ct;
} /* end of setup routine */
void addhistory(char ***history,char * args[],int argv[],int head,int size,int arg){
  // printf("check ");
  
char ** temp, **tail, **header; 
int headerarg;

 tail=history[size];
 header=history[1];
 headerarg=argv[1];
  


  if(tail!=NULL){
    // printf("free de ");

    for(int i=1;i<size;i++){
        
      history[i]=history[i+1];
      argv[i]=argv[i+1];      
    }
      history[size]=header;
      argv[size]=headerarg;


    for (int i = 1; i < argv[head]; i++){
        if(history[head][i]!=NULL)
          free(history[head][i]);
      }
        free(history[head]);
      }
  
  temp = (char**)malloc(sizeof(char *)*arg);
  argv[head]=arg;

  if(temp==NULL)
        printf("bellek ayrılamadı.\n");


    for(int i=0;i<arg;i++){
        temp[i]= (char *)malloc((MAX_LINE/2+1)*sizeof(char));
                if(temp[i]==NULL){
                      printf("bellek ayrılamadı.\n");
                }
                for (int j = 0; j <(MAX_LINE/2+1) ; j++){
                     if(args[i]!=NULL)
                    temp[i][j]=args[i][j];
                    
                    else {if(j==0)temp[i][j]='&';break;
                        /*  else temp[i][j]='\0'*/;}
                                 // printf("histargv[0][i]:%d %c\n",i,histargv[0][i][j]);
                }

    }

   history[head]=temp;
   



// for(int i=1;i<=size;i++){


//        if(history[i]!= NULL){

//         printf("History [%d]: ",i );
//         printf("arg: %d ",argv[i]);
      
      
//         for (int j = 0;j<argv[i]; j++){
//              printf("%s ",history[i][j]);
//         }
//             printf("\n");
//        }
//     }





}
int spelling_check(char * str){
  int check,digit=0;
  check=atoi(str);
  while(check>0){
    check=check/10;
    digit++;
  }

  printf("%d\n",digit);
  
  
  if(strlen(str)==digit)
    return 1;
  else 
    return 0;
}
 
int main(void)
{
        char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
        int background, arg,backcount, size =10, count=1, ncase, add_flag=1, read_flag=1 ,status; /* equals 1 if a command is followed by '&' */
        char *args[MAX_LINE/2 + 1]; /*command line arguments */
        char *dummy[MAX_LINE/2 + 1]; 
        pid_t childpid; pid_t * backchildarr;
        char buf[PATH_MAX]; //// directory
        char ***histargv;
        int *argv;
        int fd[2];


            struct sigaction action; /* control c action */
            action.sa_handler=SIG_IGN; /* assign signal ignore to action */
            action.sa_flags = 0;
          if ((status=sigemptyset(&action.sa_mask))==-1) {
                perror("Failed to initialize signal set"); exit(1);} // No blocking signal in action
           
          if ((status=sigaction(SIGINT, &action, NULL)) == -1) {  /// Ignore control ^c 
              perror("Failed to set signal handler for SIGINT"); exit(1);} 


        backchildarr=(pid_t *)malloc(sizeof(pid_t)*1);
        histargv=(char***)malloc(sizeof(char **)*(size+1));
        argv=(int*)malloc(sizeof(int)*(size+1));
         for(int i=0;i<=size;i++){histargv[i]=NULL;argv[i]=0;}

        while (1){
                    

                    /*setup() calls exit() when Control-D is entered */
                    if(read_flag){
			                background = 0;
                      printf("CSE333sh: %ld",(long)getpid() );
                      printf("\n"); 
                      arg=setup(inputBuffer, args, &background);
                    }
                    
                    if(args[0]==NULL)continue; //// if user press just enter, prevent 'cure dumpped'
         //       printf("%ld\n",(long)getpid() );
                  
                      if(strcmp(args[0],"cd") == 0){ncase=1; add_flag=1;}
                      else if(strcmp(args[0],"dir") == 0 && (arg==1 || (background && arg==2 ))){ncase=2; add_flag=1;} 
                      else if(strcmp(args[0],"clr") == 0 &&  (arg==1 || (background && arg==2 ))){ncase=3; add_flag=1;} //// Hataaaa diğerlerine de dir yap ve test et
                      else if(strcmp(args[0],"wait") == 0 &&  (arg==1 || (background && arg==2 ))){ncase=4; add_flag=1;}
                      else if(strcmp(args[0],"hist") == 0 &&  (arg==1 || (background && arg==2 ))){ncase=5; add_flag=1;}
                      else if(strcmp(args[0],"hist") == 0 && strcmp(args[1],"-set")==0 &&  (arg==3 || (background && arg==4 ))){ncase=9, add_flag=1;}
                      else if(strcmp(args[0],"exit") == 0 && (arg==1 || (background && arg==2 ))){ncase=6; add_flag=1;}
                      else if(strcmp(args[0],"!") == 0 && args[1]!= NULL && arg==2){ncase=8, add_flag=0;}
                      else{ncase=7; add_flag=1;}
                      

                    if(add_flag){
                      addhistory(histargv,args,argv,count,size,arg); 
                      if(count<size)               
                        count++; 
                    }

      /*Built in commands*/
                    if(ncase==1){ //// Cd

                      if(chdir(args[1])==0){
                         getcwd(buf,PATH_MAX);
                         setenv("PWD", buf, 1);
                         printf("PWD : %s\n", getenv("PWD"));
                      }
                      else{
                         if(errno==ENOENT){
                            chdir(getenv("HOME"));
                            getcwd(buf,PATH_MAX);
                            setenv("PWD", buf, 1);
                            printf("PWD : %s\n", getenv("PWD"));
                         }else if (errno==ENOTDIR)
                            fprintf(stderr, "Given Path is not directory"); 
                          else if(args[1]== NULL)
                            fprintf(stderr, "No given directory");
                         else
                            fprintf(stderr, "Path can not be changed\n");
                      }
                      read_flag=1;
                    }


                   else if(ncase==4){ ///// Waittt 
                      read_flag=1;
                      fprintf(stderr, "child processleri bekle"); ///// düzenleeee 
                      while(wait(NULL)>0);      

                    }

                  else if(ncase==3 ){ //// Clear
                        read_flag=1;
                        printf("args %d = %s\t",0,args[0]); 
                        system("clear");
                    }


                  else if(ncase==2){ //// Directory
                        read_flag=1;
                        if(getcwd(buf,PATH_MAX)!=NULL) //// error case leri koy 
                            fprintf(stderr, "Current Directory: %s\n", buf); 
                        else
                            fprintf(stderr, "Current directory is not readable"); 
                    }
                    else if(ncase==6){ //// error case i koy. olmadı dene!!!!!!
                        read_flag=1;

                        // for(int i=0;i<backcount;i++)
                        //   printf("background processes: %ld\n",backchildarr[i]);


                        for(int i=0;i<backcount;i++){
                          if(!waitpid(backchildarr[i],NULL,WNOHANG))
                              break;
                          if(i==backcount-1){
                             fprintf(stderr, "Shell will be terminated...\n");
                            sleep(3);
                            exit(0);}
                        }
                        fprintf(stderr, "Bacground processes are still running.The shell will not terminate before background processes are terminated\n ");
                        // if(x==0){
                        //     fprintf(stderr, "Bacground processes are still running.The shell will not terminate before background processes are terminated\n ");}
                        // else if(x>0)  {
                        //     fprintf(stderr, "Shell will be terminated...\n");
                        //     sleep(3);
                        //     exit(0);}
                        // else
                        //     fprintf(stderr, "Error!!!x= %d\n",x);
                    

                    }
                      else if(ncase==5){ /// Hist
                            for(int i=1;i<=size;i++){
                              if(histargv[i]!= NULL){

                                  printf("History [%d]: ",i );
                                  printf("arg: %d ",argv[i]);
      
      
                                  for (int j = 0;j<argv[i]; j++)
                                      printf("%s ",histargv[i][j]);
                                  
                                  printf("\n");
                              }
                            }
                            read_flag=1;
                        } /// Rerun history commands
                        else if(ncase==8){
                          int len, index,lastindex;
                          read_flag=0;
                          if(count==size){lastindex=count;}
                          else{lastindex=count-1;}

                           if(histargv[lastindex]==NULL && lastindex==size)
                              lastindex=count-1;
                           
                           if(histargv[lastindex]!=NULL ){
                              len=strspn(histargv[lastindex][0], args[1]);

                                  if(strlen(histargv[lastindex][0])<=3 && strlen(args[1])==len && len==strlen(histargv[lastindex][0])){
                                       // printf("1-argv[]: %d len: %d \n",argv[lastindex],len); 
                                        args[1]=NULL;
                                        for(int i=0;i<argv[lastindex];i++){
                                            args[i]=histargv[lastindex][i];
                                        }
                                        arg=argv[lastindex];
                                  }
                                  else if(len >=3 && strlen(args[1])==len && len!=strlen(histargv[lastindex][0])){
                                        // printf("2-argv[]: %d len: %d \n",argv[lastindex],len);
                                        args[1]=NULL;
                                        for(int i=0;i<argv[lastindex];i++){
                                            args[i]=histargv[lastindex][i];
                                        }
                                        arg=argv[lastindex];
                                  }else { ///// input ! 4dfdsg çalışmıyor
                                        index=atoi(args[1]);
                                        printf("3-argv[]: %d index: %d \n",argv[lastindex],index);
                                        if((0<index && index<size &&  spelling_check(args[1]) || (index == -1 && strlen(args[1])==2))){ //// Patlar
                                            args[1]=NULL;
                                            
                                            if(index== -1){
                                              for (int i = 0;i< argv[lastindex]; i++)
                                                  args[i]=histargv[lastindex][i];
                                                  arg=argv[lastindex];
                                            }
                                            else{
                                                for (int i = 0;i< argv[index]; i++)
                                                    args[i]=histargv[index][i];
                                                    arg=argv[index];
                                            }
                                            printf("\n");
                                        }
                                        else {printf("olmadı paşam"); read_flag=1;}
                                  }
                            }else{printf("History boş"); read_flag=1;}
                            
                           
                          for (int i = 0; i < arg; i++)
 		                         printf("args %d = %s\n",i,args[i]);

                            if(strcmp(args[arg-1],"&")==0){
				                        printf("check");
                                  args[arg-1]=NULL;
                                  background=1;}



                           
                          for (int i = 0; i < arg; i++)
 		                         printf("args %d = %s\n",i,args[i]);


                        }

                        else if(ncase==9){
                          read_flag=1;
                           if(spelling_check(args[2])){
      
                              int in=atoi(args[2]); 
                              char *** newHistory=(char***)malloc(sizeof(char **)*(in+1));
                              int * newArgv=(int*)malloc(sizeof(int)*(in+1));

                              int numOfArg;
                              if(count<size) numOfArg=count-1;
                              else numOfArg=count;

                              if(in<numOfArg){
        
                                  for(int i=in;i>0;i--){
                                      newHistory[i]=histargv[numOfArg];
                                      newArgv[i]=argv[numOfArg];
                                      numOfArg--;
                                  }

                                  for(int a=1;a<=numOfArg;a++){
                                    for (int i = 0; i < argv[a]; i++){
                                      if(histargv[a][i]!=NULL)
                                        free(histargv[a][i]);
                                      }
                                      free(histargv[a]);
                                  }
                                  count=in;
                                  histargv=newHistory;
                                  argv=newArgv;
                              }else{ 
                                  
                                  for(int i=1;i<=numOfArg;i++){
                                    newHistory[i]=histargv[i];
                                    newArgv[i]=argv[i];
                                  }
                                  histargv=newHistory;
                                  argv=newArgv;
                              }
                              size=in;
                              printf("History size has been updated");
                          }else printf("History size has not been updated");
	                       }

                  else{/*Systemmm Functionsss*/
                       read_flag=1;   
                   int * pipepos, pipeNum=0;
                          for(int i=0;i<arg;i++){
                            if(args[i]!=NULL){
                              if(i!=0){
                                if(strcmp(args[i],"|") == 0 && args[i+1]!=NULL && args[i-1]!=NULL && strcmp(args[i+1],"|") != 0 && strcmp(args[i-1],"|") != 0)
                                pipeNum++;
                              }
                            }
                          }
                            pipepos=(int *)malloc((pipeNum+2)* sizeof(int));
                              int pos=1;
                              for(int j=0;j<arg;j++){
                                if(args[j]!=NULL){
                                  if(j!=0){
                                    if(strcmp(args[j],"|") == 0 && args[j+1]!=NULL && args[j-1]!=NULL && strcmp(args[j+1],"|") != 0 && strcmp(args[j-1],"|") != 0){
                                       pipepos[pos]=j;
                                        pos++;
                                      }
                                  }
                                }  
                              }   
                            
                            pipepos[0]=-1;
                            if(background) pipepos[pipeNum+1]=arg-1;
                            else pipepos[pipeNum+1]=arg;

                            printf("pipenum: %d\n",pipeNum);
                            for(int i=0;i<(pipeNum+2);i++)
                              printf("pipepos %d: %d\n",i,pipepos[i]);

                              int terminal=1, start=0, end=0,child=0;
          

                              for(int i=0;i<=pipeNum;i++){
                                  if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
                                      perror("Failed to setup pipeline");
                                      return 1;
                                  }

                                  if(childpid>0){ ///// Parent çıkar
                                    break;
                                  }else {
                                      terminal=0;
                                      child=i;
                                    //   printf("\n\n\n\n");
                                     printf("pid %ld\t",(long)getpid() );
                                     printf("parent pid %ld\n",(long)getppid() );
                                        start=pipepos[pipeNum-i];
                                        end=pipepos[pipeNum+1-i];
                                         // printf("end: %d start: %d\n",end,start);
                                  }

                              }

                                if(terminal){

                                  if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
                                      perror("Failed to close extra pipe descriptors on ls");
                                      
                                  if (!background){
                                      int control = waitpid(childpid,NULL,NULL); ///// if it is not background, wait for last child
                                      if(control<0)fprintf(stderr, "interrupted by signal"); 
                                  }else{
                                    backcount++;
                                    printf("backcount: %d\n",backcount);
                                    if(backcount==1)
                                      backchildarr[0]=childpid;
                                    else{
                                      backchildarr=realloc(backchildarr,backcount * sizeof(pid_t));
                                      backchildarr[backcount-1]=childpid;
                                    }
                                   
                                  }

                                }else{
                                    if(child==0 && child!=pipeNum){
                                        printf("i:%d f0:%d f1:%d\n",child,fd[0],fd[1] );
                                        if (dup2(fd[0], STDIN_FILENO) == -1)               /* last command of pipe command string */
                                            perror("Failed to redirect stdin of sort");
                                        else if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
                                            perror("Failed to close extra pipe file descriptors on sort"); 
                                    }
                                    else if(child==0 && child==pipeNum){ /* System commands */
                                        printf("i:%d f0:%d f1:%d\n",child,fd[0],fd[1] );
                                        if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
                                            perror("Failed to close extra pipe descriptors on ls");
                                    }
                    else if (child == pipeNum) {      
                    printf("i:%d f0:%d f1:%d\n",child,fd[0],fd[1] );                           /* first command of pipe command string */
                          if (dup2(fd[1], STDOUT_FILENO) == -1) 
                            perror("Failed to redirect stdout of ls");
                          else if((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
                            perror("Failed to close extra pipe descriptors on ls");
                      }
                      else{
                        int write=fd[0]-1;
                        printf("i:%d f0:%d f1:%d\n",child,fd[0],write);
                        if (dup2(fd[0], STDIN_FILENO) == -1)               
                            perror("Failed to redirect stdin of sort");
                          else if(dup2(write, STDOUT_FILENO) == -1)
                            perror("Failed to redirect stdout of ls");
                          
                        else if  ((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
                            perror("Failed to close extra pipe file descriptors on sort"); 
  
                        }
                                      wait(NULL);
                        // printf("çocuğumu bekledim işimi yapıp çıkıcam \n" );
                                    
                                  int dum=0;
                                        for(int a=start+1;a<end;a++){
                                          dummy[dum]=args[a];
                                          dum++;
                                        }         
                    
                      if (dummy[0][0]=='/'){ ///// system commands with directory execl
                                      
                                 //    fprintf(stderr, "I am child %ld\n", (long)getpid()); 
                                    execl(dummy[0], dummy[1], dummy[2],  dummy[3], dummy[4], dummy[5], dummy[6], dummy[7], dummy[8], NULL);
                                    fprintf(stderr, "Command not found"); return 1;
                            
                                }else{ //// System commands without directory execvp
                                    fprintf(stderr, "I am child %ld\n", (long)getpid()); 
                              fprintf(stderr, "child:%d dummy[0]: %s\n",child, dummy[0] );

                                    execvp(dummy[0], &dummy[0]);
                                    fprintf(stderr, "Command not found"); 
                                    return 1;
//printf("control\n");
                                    }
                                  }

                 free(pipepos);   
                } ///// end of System functionsss

          } /// while ends
} /// Main Ends
