#include <R.h>
#include <Rdefines.h>

SEXP plusC(SEXP a, SEXP b) {
  PROTECT(a = AS_INTEGER(a));
  PROTECT(b = AS_INTEGER(b));
  int *pa = INTEGER_POINTER(a),
    *pb = INTEGER_POINTER(b);
  SEXP retval;
  PROTECT(retval = NEW_INTEGER(1));
  int *pretval = INTEGER_POINTER(retval); 
  pretval[0] = pa[0] + pb[0];
  UNPROTECT(3);
  return retval;
}
