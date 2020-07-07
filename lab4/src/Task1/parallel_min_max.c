#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>   //Для signal

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* children;
int pnum;
int active_child_processes = 0;

void my_alarm(){
    int status;
    int i=0;
    for (; i<pnum; i++){
        int result = waitpid(children[i], &status, WNOHANG);
        if (result == 0){
            kill(children[i], SIGKILL);
            printf("%d is eliminated\n", children[i]);
        } else {
            printf("%d is finished\n", children[i]);
            active_child_processes--;
        }
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            if (seed <= 0) {
            printf("seed is a positive number\n");
            return 1;
            }
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            if (array_size <= 0) {
            printf("array_size is a positive number\n");
            return 1;
            }
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if (pnum < 1) {
            printf("at least 1 parallel process should be started\n");
            return 1;
            }
            // error handling
            break;
            case 3:
            timeout = atoi(optarg);
            // your code here
            if (timeout <= 0) {
            printf("timeout is a positive num\n");
            return 1;
            }
            // error handling
            break;
          case 4:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
//   GenerateArray(array, array_size, seed);
//   GenerateArray ниже
// int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  FILE *fpmin,*fpmax;
  int pipefd[pnum][2];    // общий pipe для процессов

  children = malloc(sizeof(pid_t)*pnum);

  int i = 0;
  for (; i < pnum; i++) {
    if (pipe(pipefd[i]) == -1) {
        exit(1);
    }
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      children[i] = child_pid;
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        // parallel somehow

        // разный seed для генерации различных массивов
        seed = seed * (i+1);    // допустим
        GenerateArray(array, array_size, seed);


        //if (i==3){
        //    while(true){ }
        //}
	sleep(15);

        if (with_files) {
          // use files here
          struct MinMax min_max = GetMinMax(array,0,array_size);

          // отдельные файлы для каждого дочернего процесса
          char min_file[10];
          sprintf(min_file, "min%d.txt", i+1);
          char max_file[10];
          sprintf(max_file, "max%d.txt", i+1);

          fpmin=fopen(min_file, "w");
          fprintf(fpmin,"%d",min_max.min);
          fpmax=fopen(max_file,"w");
          fprintf(fpmax,"%d",min_max.max);
          fclose(fpmin);
          fclose(fpmax);
          exit(0);
        } else {
          close(pipefd[i][0]); // закрываем сторону для чтения
          struct MinMax min_max = GetMinMax(array,0,array_size);
          write(pipefd[i][1], &min_max.min, sizeof(int));
          write(pipefd[i][1], &min_max.max, sizeof(int));
          printf("pipe %d : min = %d , max = %d\n", i+1, min_max.min, min_max.max);
          close(pipefd[i][1]);
          exit(0);
        }
        return 0;
      }

    }
    else {
      printf("Fork failed!\n");
      return 1;
    }
  }


if (timeout != -1){
    signal(SIGALRM, my_alarm);
    alarm(timeout);
    pause();
}

// получаем информацию об убитых процессах
while (active_child_processes > 0) {
    // your code here
    wait(0);
    active_child_processes -= 1;
}
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  i=0;
  for (; i < pnum; i++) {
    int min = INT_MAX;
    int max = 0;

    if (with_files) {
      // read from files
      char min_file[10];
      sprintf(min_file, "min%d.txt", i+1);
      char max_file[10];
      sprintf(max_file, "max%d.txt", i+1);

      fpmin=fopen(min_file,"r");
      fscanf(fpmin,"%d",&min);
      fpmax=fopen(max_file,"r");
      fscanf(fpmax,"%d",&max);
      fclose(fpmin);
      fclose(fpmax);
    } else {
      // read from pipes
      close(pipefd[i][1]);     // закрываем сторону для записи
      read(pipefd[i][0], &min, sizeof(int));
      read(pipefd[i][0], &max, sizeof(int));
    //   printf("%d pipe: min - %d , max - %d\n", i+1, min, max);
      close(pipefd[i][0]);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);

  //sleep(60);

  return 0;
}
