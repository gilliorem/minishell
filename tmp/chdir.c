#include <unistd.h>
#include <stdio.h>
int main()
{
	char buf[1024];
	getcwd(buf, 1024);
	printf("%s\n", buf);
	chdir("..");
	getcwd(buf, 1024);
	printf("%s\n", buf);

}
