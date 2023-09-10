#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <signal.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

void  SIGINT_handler(int);   
void  SIGQUIT_handler(int); 
void  SIGINT_handler_child(int sig);
int child_pid;  

int main(void)
{
     int   i;
     pid_t pid = getpid();
     

     // if (signal(SIGINT, SIGINT_handler) == SIG_ERR) {
     //      printf("SIGINT install error\n");
     //      exit(1);
     // }
     // if (signal(SIGQUIT, SIGQUIT_handler) == SIG_ERR) {
     //      printf("SIGQUIT install error\n");
     //      exit(2);
     // }
     struct sigaction sa;
    sa.sa_handler = SIGINT_handler;
    
       
       

    
     child_pid=fork();
     printf("forked %d\n",child_pid);
     if(child_pid<0)
          exit(0);
     if(child_pid==0)
     {

          for (i = 0; ; i++) {        
          printf("in child\n");
          sleep(1);
     }
     }
     else{
          sigaction(SIGINT, &sa, NULL);
          wait(NULL);
          printf("out\n");
           int status;
          

        
           for (i = 0; ; i++) {        
          printf("in parent\n");
          sleep(1);
     }
     }
}

void  SIGINT_handler_child(int sig)
{
     //do nothing
     printf("From SIGINT: just got a %d (SIGINT ^C) signal\n", sig);
}
void  SIGINT_handler(int sig)
{

      if ((child_pid>0) && (kill(child_pid, 0) == 0)) {
        printf(PNK);
        printf("Received Ctrl+C. Sending SIGINT to the foreground process...\n");
        printf(COL_RESET);
        kill(child_pid, SIGINT);
    }
    else
    {

    }
     
}

// int main()
// {
//     long start=time(NULL);
//     int pid=fork();
//     long end=time(NULL);
//     printf("%ld %ld\n",start,end);
//     printf("%ld\n",end-start);
//     while(1)
//     {
//         printf("why ");
//     }
//         printf("hi %d\n",kill(getpid(),SIGINT));
    
// }

void  SIGQUIT_handler(int sig)
{
     signal(sig, SIG_IGN);
     printf("From SIGQUIT: just got a %d (SIGQUIT ^\\) signal"
                          " and is about to quit\n", sig);
     

     exit(3);
}


