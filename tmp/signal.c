// Source - https://stackoverflow.com/a/4217070
// Posted by icyrock.com, modified by community. See post 'Timeline' for change history
// Retrieved 2025-12-06, License - CC BY-SA 4.0

#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

void     INThandler(int);

int  main(void)
{
     signal(SIGINT, INThandler);
     while (1)
          puts("still running");
     return 0;
}

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          exit(0);
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}

