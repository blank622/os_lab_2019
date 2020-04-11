#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

typedef struct MinMax MinMax;

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
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
            if (seed <= 0)
              {
                printf("seed is a positive number\n");
                return 1;
              }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0)
            {
              printf("array size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum < 1)
            {
              printf("at least one process should be created, pnum >= 1\n");
              return 1;
            }
            break;
          case 3:
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
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  FILE *fpmin, *fpmax;
  int pipefd[pnum][2];

  for (unsigned int i = 0; i < pnum; i++)
  {
    if (pipe(pipefd[i]) == -1) exit(1);
  }

  for (unsigned int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        unsigned int begin = array_size / pnum * i;
        unsigned int end = array_size / pnum * (i + 1);
        if (with_files) {
          MinMax min_max = GetMinMax(array, begin, end);

          char min_file[10];
          sprintf(min_file, "min%d.txt", i);

          char max_file[10];
          sprintf(max_file, "max%d.txt", i);

          fpmin = fopen(min_file, "w");
          fprintf(fpmin, "%d", min_max.min);

          fpmax = fopen(max_file, "w");
          fprintf(fpmax, "%d", min_max.max);

          fclose(fpmin);
          fclose(fpmax);
          exit(0);
        }
        else
        {
          close(pipefd[i][0]);
          MinMax min_max = GetMinMax(array, begin, end);

          write(pipefd[i][1], &min_max.min, sizeof(int));
          write(pipefd[i][1], &min_max.max, sizeof(int));
          printf("pipe %d: min = %d\nmax = %d\n\n", i, min_max.min, min_max.max);

          close(pipefd[i][1]);
          exit(0);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    wait(0);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (unsigned int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      char min_file[10];
      sprintf(min_file, "min%d.txt", i);

      char max_file[10];
      sprintf(max_file, "max%d.txt", i);

      fpmin = fopen(min_file, "r");
      fscanf(fpmin, "%d", &min);

      fpmax = fopen(max_file, "r");
      fscanf(fpmax, "%d", &max);
      
      fclose(fpmin);
      fclose(fpmax);
    }
    else
    {
      close(pipefd[i][1]);
      read(pipefd[i][0], &min, sizeof(int));
      read(pipefd[i][0], &max, sizeof(int));
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
  return 0;
}
