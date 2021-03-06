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

#include <mlvalues.h>
#include <memory.h>
#include <stacks.h>
#include <callback.h>
#include <signals.h>

static void ocamlcc_bytecode_main(void);

value ocamlcc_main() {
  CAMLparam0();
  CAMLlocal1(result);
  DeclareLocalSp();
  ocamlcc_exception_init();
  ocamlcc_codeptrs_init();
  ocamlcc_pushtrap(result = Make_exception_result(caml_exn_bucket),
                   catch_label, 0_main_0);
  ocamlcc_bytecode_main();
  ocamlcc_poptrap(0);
  OffsetSp(0);
  result = Val_unit;
  goto end;
  ocamlcc_catch(catch_label, result = Make_exception_result(exn));
  OffsetSp(0);
 end:
  CAMLreturn(result);
}
