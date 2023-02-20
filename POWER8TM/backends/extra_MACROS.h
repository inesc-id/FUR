#ifndef EXTRA_MACROS_H_GUARD_
#define EXTRA_MACROS_H_GUARD_

# define INACTIVE    0
# define ACTIVE      1
# define NON_DURABLE 2
# define first_2bits_zero 0x3ffffffffffffffff

# define UPDATE_TS_STATE(state){\
  long temp;\
  READ_TIMESTAMP(temp);\
  temp=temp & first_2bits_zero;\
  temp = (state<<62)|temp;\
  ts_state[local_thread_id].value=temp;\
}\

# define UPDATE_STATE(state){\
  long temp=state;\
  temp=temp<<2;\
  temp=temp>>2;\
  temp = (state<<62)|temp;\
  ts_state[local_thread_id].value=temp;\
}\

# define check_state(temp)({\
  (temp & (3l<<62))>>62;\
})\

#define abortMarker() \
{ \
  if ( order_ts[local_thread_id].value != -1 ) \
  { \
    commit_abort_marker(mylogpointer,order_ts[local_thread_id].value,mylogstart,mylogend);\
  } \
    UPDATE_STATE(INACTIVE); \
}; \
// end abortMarker

# define atomicInc()   __atomic_add_fetch(&global_order_ts, 1, __ATOMIC_RELEASE) 

# define WAIT(_cond) \
  while ( _cond ) \
  { \
    cpu_relax(); \
  } \
// end WAIT

# define CONTINUE_LOOP_IF(_cond, ...) \
  if ( _cond ) \
  { \
    __VA_ARGS__; \
    continue; \
  } \
// end CONTINUE_LOOP_IF

# define BREAK_LOOP_IF(_cond, ...) \
  if ( _cond ) \
  { \
    __VA_ARGS__; \
    break; \
  } \
// end BREAK_LOOP_IF


//Begin ROT
# define USE_ROT(){ \
	int rot_budget = ROT_RETRIES; \
	while(IS_LOCKED(single_global_lock)){ \
    cpu_relax(); \
  } \
	long start_time; \
	while(rot_budget > 0){ \
		rot_status = 1; \
		TM_buff_type TM_buff; \
    UPDATE_TS_STATE(ACTIVE);\
		rmb(); \
		if(IS_LOCKED(single_global_lock)){ \
			UPDATE_TS_STATE(INACTIVE); /* inactive rot*/ \
			rmb(); \
			while(IS_LOCKED(single_global_lock)) cpu_relax(); \
			continue; \
		} \
		/*BEGIN_ROT ------------------------------------------------*/ \
    \
    \		
    /*while(abs(mylogpointer-atomic_LOAD(log_replayer_start_ptr[local_thread_id]))>MIN_SPACE_LOG){\
			load_fence();\
		}*/\
    READ_TIMESTAMP(start_tx); \
		unsigned char tx_status = __TM_begin_rot(&TM_buff); \
		if (tx_status == _HTM_TBEGIN_STARTED) { \
      break; \
    } \
		else if(__TM_conflict(&TM_buff)){ \
      abortMarker();\
      READ_TIMESTAMP(end_tx); \
      stats_array[local_thread_id].abort_time += end_tx - start_tx;\
      \
      stats_array[local_thread_id].rot_conflict_aborts ++; \
			if(__TM_is_self_conflict(&TM_buff)){ stats_array[local_thread_id].rot_self_conflicts++; \
			}\
			else if(__TM_is_trans_conflict(&TM_buff)) stats_array[local_thread_id].rot_trans_conflicts++; \
			else if(__TM_is_nontrans_conflict(&TM_buff)) stats_array[local_thread_id].rot_nontrans_conflicts++; \
			else stats_array[local_thread_id].rot_other_conflicts++; \
      rot_status = 0; \
      rot_budget--; \
			int state = check_state(ts_state[local_thread_id].value); \
      if(state == ACTIVE) \
        UPDATE_STATE(INACTIVE);\
        rmb(); \
    } \
    else if (__TM_user_abort(&TM_buff)) { \
      abortMarker();\
      READ_TIMESTAMP(end_tx); \
      stats_array[local_thread_id].abort_time += end_tx - start_tx;\
      \
      stats_array[local_thread_id].rot_user_aborts ++; \
      rot_status = 0; \
      rot_budget--; \
    } \
    else if(__TM_capacity_abort(&TM_buff)){ \
      abortMarker();\
      READ_TIMESTAMP(end_tx); \
      stats_array[local_thread_id].abort_time += end_tx - start_tx;\
      \
			rot_status = 0; \
			stats_array[local_thread_id].rot_capacity_aborts ++; \
			if(__TM_is_persistent_abort(&TM_buff)) stats_array[local_thread_id].rot_persistent_aborts ++; \
        break; \
		} \
    else{ \
      abortMarker();\
      READ_TIMESTAMP(end_tx); \
      stats_array[local_thread_id].abort_time += end_tx - start_tx;\
			rot_status = 0; \
      rot_budget--; \
			stats_array[local_thread_id].rot_other_aborts ++; \
		} \
	} \
};\



#endif // EXTRA_MACROS_H_GUARD_