#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	int pid = fork();

	if (pid == 0)
	{
		/*execl(
			"parallel_min_max",
			"parallel_min_max",
			"--seed", "5",
			"--array_size", "20000000",
			"--pnum", "4",
			"-f",
			NULL
		);*/

		//execvp("./parallel_min_max", argv);

		// boooooooooooooooooooooooooooooring
		execvp("./sequential_min_max", argv);

		return 0;
	}

	if (pid != 0)
	{
		wait(NULL);
		printf("Execution finished\n");
	}
	
	return 0;
}
