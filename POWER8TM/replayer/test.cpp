#include "replayer.hpp"

#include <cstdio>
#include <cstdlib>
#include <time.h>

#define TIMER_T                         struct timespec
#define TIMER_READ(time)                clock_gettime(CLOCK_MONOTONIC, &(time))
#define TIMER_DIFF_SECONDS(start, stop) \
    (((double)(stop.tv_sec)  + (double)(stop.tv_nsec / 1.0e9)) - \
     ((double)(start.tv_sec) + (double)(start.tv_nsec / 1.0e9)))
#define TIMER_DIFF_NSEC(start, stop) \
    (((double)(stop.tv_sec * 1.0e9)  + (double)(stop.tv_nsec)) - \
     ((double)(start.tv_sec * 1.0e9) + (double)(start.tv_nsec)))

static void test(const char *s)
{
  int nbGen = 0;
  int nbRep = 0;
  TIMER_T t1, t2;
  double t;

  TIMER_READ(t1);
  nbGen = rep::generate_log();
  TIMER_READ(t2);
  t = TIMER_DIFF_SECONDS(t1, t2);
  printf("[%s] Generated %i TXs in %fs (%fns/TX)\n", s, nbGen, t, t / nbGen * 1.0e9);

  TIMER_READ(t1);
  nbRep = rep::replay();
  TIMER_READ(t2);
  t = TIMER_DIFF_SECONDS(t1, t2);
  printf("[%s] Replayed %i TXs in %fs (%fns/TX)\n", s, nbRep, t, t / nbRep * 1.0e9);
}

int main(int argc, char** argv)
{
  rep::args_t a = { // TODO: get args from argv
    .nb_threads = 32,
    .size_thread_log = 2097152, // 2MB
    .size_metadata = 32 * 2097152 / 5,
    .heap_size = 2097152,
    .min_writes = 1,
    .max_writes = 5,
    .seed = 1234
  };
  
  printf(
    "---------------------------------\n" 
    "Naive log replayer implementation\n"
    "---------------------------------\n"
  );
  rep::setup_naive(a);
  test("naive");
  rep::destroy();


  printf(
    "--------------------------------------\n" 
    "Sequential log replayer implementation\n"
    "--------------------------------------\n"
  ); 
  rep::setup_seq_log(a);
  test("seq_log");
  rep::destroy();

  return EXIT_SUCCESS;
}
