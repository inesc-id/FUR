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

# define check_state(temp) ({ \
  (temp & (3l<<62))>>62;\
}) \
// end check_state

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

#endif // EXTRA_MACROS_H_GUARD_