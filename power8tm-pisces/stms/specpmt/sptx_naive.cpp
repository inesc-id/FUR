#include <iostream>
#include "sptx_naive.h"

using namespace std;

thread_local SPLog *local_log = nullptr;
thread_local uint64_t thread_id = -1;
PLogMetadata plm;


SPPMT::SPPMT(){cout << __FILE__ << endl;}
void SPPMT::tm_system_init(){}
void SPPMT::tm_system_shutdown(){}
void SPPMT::tm_thread_init(){
  plm.createLocalLog();
}
void SPPMT::tm_thread_exit(){}

void SPPMT::tm_begin(){
  local_log->start_tx();
}

void SPPMT::tm_end(){
  local_log->commit_log(plm.getTimestamp());
  sfence();
}

PMT pmt;

Timer timer;
atomic<uint64_t> fence_cnt;
static void __attribute__ ((destructor)) end_benchmark()
{
  cout << "number of fence called : " << fence_cnt << endl;
}