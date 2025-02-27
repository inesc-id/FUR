#include <iostream>
#include "sptx_gc.h"

using namespace std;

thread_local SPGCLog *local_log = nullptr;
thread_local uint64_t thread_id = -1;
PLogMetadata plm;


SPGCPMT::SPGCPMT(){cout << __FILE__ << endl;}
void SPGCPMT::tm_system_init(){}
void SPGCPMT::tm_system_shutdown(){}
void SPGCPMT::tm_thread_init(){
  plm.createLocalLog();
}
void SPGCPMT::tm_thread_exit(){}

void SPGCPMT::tm_begin(){
  local_log->start_tx();
}

void SPGCPMT::tm_end(){
  local_log->commit_log(plm.getTimestamp());
  sfence();
}

/*=================================================*/
PMT pmt;
Timer timer;
atomic<uint64_t> fence_cnt;
bool outside_tx = false;

static void __attribute__ ((destructor)) end_benchmark()
{
  cout << "number of fence called : " << fence_cnt << endl;
}