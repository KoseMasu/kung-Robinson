#include "concurrency.h"

DATA DataObj[MAX_OBJ];

static void *
worker(void *arg)
{
  /* Growing Phase */
  for (int i = 0; i < MAX_OBJ; i++) {
		lock(i);
  }

  for (int i = 0; i < MAX_OBJ; i++) {
		task_alpha(i);
		task_beta(i);
  }
	
	/* Shrinking Phase */
  for (int i = 0; i < MAX_OBJ; i++) {
		unlock(i);
	}

  return NULL;
}

extern int
main(int argc, char *argv[])
{
  int i, nthread = 4;
  struct timeval begin, end;
	pthread_t *thread;
	
	if (argc == 2) nthread = atoi(argv[1]);
	thread = (pthread_t *)calloc(nthread, sizeof(pthread_t));
	if (!thread) ERR;
	
	gettimeofday(&begin, NULL);
  for (i = 0; i < nthread; i++) {
    int ret = pthread_create(&thread[i], NULL, worker, NULL);
		if (ret < 0) ERR;
	}
  for (i = 0; i < nthread; i++) {
		int ret = pthread_join(thread[i], NULL);
		if (ret < 0) ERR;
	}
  gettimeofday(&end, NULL);
  print_result(begin, end, nthread);
	free(thread);
	
  return 0;
}
