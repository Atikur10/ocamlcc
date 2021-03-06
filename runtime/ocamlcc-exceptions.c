/*************************************************************************/
/*                                                                       */
/*                               OCamlCC                                 */
/*                                                                       */
/*                    Michel Mauny, Benoit Vaugon                        */
/*                          ENSTA ParisTech                              */
/*                                                                       */
/*    This file is distributed under the terms of the CeCILL license.    */
/*    See file ../LICENSE-en.                                            */
/*                                                                       */
/*************************************************************************/

#include <stdlib.h>
#include <fail.h>

typedef struct caml__roots_block *caml__roots_block_struct_ptr;

#ifdef OCAMLCC_EXCEPTION_SETJMP

#define OCAMLCC_EXCEPTION_STACK_INIT_SIZE 256
struct longjmp_buffer *ocamlcc_exception_stack;
long ocamlcc_exception_stack_size = OCAMLCC_EXCEPTION_STACK_INIT_SIZE;
long ocamlcc_exception_stack_offset = 0;

void ocamlcc_exception_init_fun(void) {
  ocamlcc_exception_stack = (struct longjmp_buffer *)
    malloc(OCAMLCC_EXCEPTION_STACK_INIT_SIZE * sizeof(struct longjmp_buffer));
  if (ocamlcc_exception_stack == NULL) caml_raise_stack_overflow();
}

struct longjmp_buffer *ocamlcc_exception_pushtrap_fun(void) {
  if (ocamlcc_exception_stack_offset == ocamlcc_exception_stack_size) {
    long new_size = ocamlcc_exception_stack_size * 2;
    struct longjmp_buffer *new_stack = (struct longjmp_buffer *)
      realloc(ocamlcc_exception_stack,
              new_size * sizeof(struct longjmp_buffer));
    if (new_stack == NULL) caml_raise_stack_overflow();
    ocamlcc_exception_stack = new_stack;
    ocamlcc_exception_stack_size = new_size;
  }
  ocamlcc_exception_stack_offset ++;
  return &ocamlcc_exception_stack[ocamlcc_exception_stack_offset - 1];
}

struct longjmp_buffer *ocamlcc_exception_poptrap_fun(void) {
  CAMLassert(ocamlcc_exception_stack_offset > 0);
  ocamlcc_exception_stack_offset --;
  if (ocamlcc_exception_stack_offset < ocamlcc_exception_stack_size / 4) {
    long new_size = ocamlcc_exception_stack_size / 2;
    if (new_size >= OCAMLCC_EXCEPTION_STACK_INIT_SIZE) {
      ocamlcc_exception_stack = (struct longjmp_buffer *)
        realloc(ocamlcc_exception_stack,
                new_size * sizeof(struct longjmp_buffer));
      ocamlcc_exception_stack_size = new_size;
    }
  }
  return &ocamlcc_exception_stack[ocamlcc_exception_stack_offset - 1];
}

/***/

#define ocamlcc_exception_init() \
  ocamlcc_exception_init_fun()

#define ocamlcc_raise(exn) {                    \
  Unlock_exn();                                 \
  caml_exn_bucket = exn;                        \
  longjmp(caml_external_raise->buf, 1);         \
}

#define ocamlcc_pushtrap(restore_exn, lab, ukid) ;                      \
  volatile long ocamlcc_save_sp_offset_##ukid;                          \
  volatile caml__roots_block_struct_ptr ocamlcc_initial_local_roots_##ukid; \
  ocamlcc_initial_local_roots_##ukid = caml_local_roots;                \
  SaveSp(ocamlcc_save_sp_offset_##ukid);                                \
  caml_external_raise = ocamlcc_exception_pushtrap_fun();               \
  if (setjmp(caml_external_raise->buf)) {                               \
    caml_external_raise = ocamlcc_exception_poptrap_fun();              \
    caml_local_roots = ocamlcc_initial_local_roots_##ukid;              \
    RestoreSp(ocamlcc_save_sp_offset_##ukid);                           \
    restore_exn;                                                        \
    goto lab;                                                           \
  }

#define ocamlcc_poptrap(frame_sz) {                                     \
  ocamlcc_check_something_to_do(frame_sz);                              \
  caml_external_raise = ocamlcc_exception_poptrap_fun();                \
}

#define ocamlcc_saved_poptrap(to_save, frame_sz) {                      \
  ocamlcc_saved_check_something_to_do(to_save, frame_sz);               \
  caml_external_raise = ocamlcc_exception_poptrap_fun();                \
}

#define ocamlcc_catch(lab, restore_exn)         \
  lab:

#else /* OCAMLCC_EXCEPTION_SETJMP */

#ifndef __cplusplus
  #error - Incompatible code: compiler should support C++ try-catch mechanism
#endif

#define ocamlcc_exception_init()

#define ocamlcc_raise(exn) throw(exn)

#define ocamlcc_pushtrap(restore_exn, lab, ukid) {                      \
  volatile long ocamlcc_save_sp_offset_0_cpp_0;                         \
  volatile caml__roots_block_struct_ptr ocamlcc_initial_local_roots_0_cpp_0; \
  ocamlcc_initial_local_roots_0_cpp_0 = caml_local_roots;               \
  SaveSp(ocamlcc_save_sp_offset_0_cpp_0);                               \
  try {

#define ocamlcc_poptrap(frame_sz)                                       \
  ocamlcc_check_something_to_do(frame_sz);

#define ocamlcc_saved_poptrap(to_save, frame_sz)                        \
  ocamlcc_saved_check_something_to_do(to_save, frame_sz);

#define ocamlcc_catch(lab, restore_exn) ;                               \
  } catch(value exn) {                                                  \
    caml_local_roots = ocamlcc_initial_local_roots_0_cpp_0;             \
    RestoreSp(ocamlcc_save_sp_offset_0_cpp_0);                          \
    restore_exn;                                                        \
  }                                                                     \
}

#endif /* OCAMLCC_EXCEPTION_SETJMP */
