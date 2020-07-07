#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/time.h>

#include <pthread.h>

struct FactArgs{
    int begin;
    int end;
    int mod;
};
long long result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* Factorial(void *args)
{
    struct FactArgs *fact_args = (struct FactArgs*)args;
    int fact = 1;
    int i;
    printf("begin = %d\nend = %d\n", fact_args->begin, fact_args->end);
    for(i = fact_args->begin; i <= fact_args->end; i++){
        fact *= i;
        fact %= fact_args->mod;
    }
    printf("fact = %d\n", fact);
    pthread_mutex_lock(&mutex);
    result *= fact;
    result %= fact_args->mod;
    pthread_mutex_unlock(&mutex);
    printf("result = %d\n", result);
    return NULL;
}

int main(int argc, char **argv) {
    int threads_num = -1;
    int mod = -1;
    int k = -1;
    int current_optind = optind ? optind : 1;
    while(true){
        static struct option options[] = {{"pnum", required_argument, 0, 0},
                                        {"mod", required_argument, 0, 0},
                                        {0, 0, 0, 0}};
                                        
        int option_index = 0;
        int c = getopt_long(argc, argv, "k:", options, &option_index);
    if (c == -1) break;
        switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                threads_num = atoi(optarg);
                break;
            case 1:
                mod = atoi(optarg);
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'k':
            k = atoi(optarg);
            if(k < 0){
                printf("k must be positive \n");
                return 1;
            }
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

    if (mod == -1 || k == -1 || threads_num == -1) {
        printf("Usage: %s -k \"num\" --pnum \"num\" --mod \"num\" \n",
            argv[0]);
        return 1;
    }
    
    pthread_t threads[threads_num];

    struct FactArgs args[threads_num];
    double block = (double) k / threads_num;
 
    int i;
    for (i = 0;i < threads_num;i++)
    {
        int begin = (int)(block * i)+1;
        int end = (int)(block * (i + 1));
	
	if (i == threads_num - 1) end = k;
	
        args[i].begin = begin;
        args[i].end = end;
        args[i].mod = mod;
    }
    for (i = 0; i < threads_num; i++) {
        if (pthread_create(&threads[i], NULL, Factorial, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    for ( i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Total: %lld\n", result);
    return 0;

}
