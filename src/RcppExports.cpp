// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// parse_spf
List parse_spf(std::string spf_rec);
RcppExport SEXP _dnshelpers_parse_spf(SEXP spf_recSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type spf_rec(spf_recSEXP);
    rcpp_result_gen = Rcpp::wrap(parse_spf(spf_rec));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_dnshelpers_parse_spf", (DL_FUNC) &_dnshelpers_parse_spf, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_dnshelpers(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}