#include <Rcpp.h>

using namespace Rcpp;

#include "config.h"
#include "spf_sys_config.h"
#include "spf.h"
#include "spf_dns.h"
#include "spf_internal.h"

static SPF_errcode_t
  SPF_record_stringify_data(SPF_data_t *data, SPF_data_t *data_end,
                            char **p_p, char *p_end,
                            int is_mod, int cidr_ok, int debug );

//' @export
// [[Rcpp::export]]
List parse_spf(std::string spf_rec) {

  List l;

  SPF_server_t *spf_server = NULL;
  SPF_request_t *spf_request = NULL;
  SPF_response_t *spf_response = NULL;
  SPF_record_t *spf_record = NULL;
  SPF_error_t *spf_error = NULL;
  SPF_errcode_t err;

  spf_server = SPF_server_new(SPF_DNS_CACHE, 0);

  spf_request = SPF_request_new(spf_server);
  spf_response = SPF_response_new(spf_request);

  err = SPF_record_compile(spf_server, spf_response, &spf_record, spf_rec.c_str());

  if (SPF_response_messages(spf_response) > 0) {
    for (int i=0; i<SPF_response_messages(spf_response); i++) {
      spf_error = SPF_response_message(spf_response, i);
      printf(
        "%s: %s%s\n",
        SPF_error_errorp(spf_error) ? "Error" : "Warning",
        ((SPF_error_errorp(spf_error) && (!err))
           ? "[UNRETURNED "
           : ""),
           SPF_error_message(spf_error)
      );
    }

    l = List::create();

  } else {

    SPF_record_print(spf_record);

    SPF_mech_t *mech;
    SPF_mod_t	*mod;
    SPF_data_t *data, *data_end;

    mech = spf_record->mech_first;
    for (int i=0; i<spf_record->num_mech; i++) {

      switch (mech->prefix_type) {
      case PREFIX_PASS: // +
        Rcout << "PASS" << " " ;
        break;
      case PREFIX_FAIL: // -
        Rcout << "FAIL" << " " ;
        break;
      case PREFIX_SOFTFAIL: // ~
        Rcout << "SOFTFAIL" << " " ;
        break;
      case PREFIX_NEUTRAL: // ?
        Rcout << "NEUTRAL" << " " ;
        break;
      case PREFIX_UNKNOWN: // not good
        Rcout << "AUGH1" << " " ;
        break;
      default: // not good
        Rcout << "AUGH2" << " " ;
      break;
      }

      Rcout << "Mech Type: " << (int)mech->mech_type << std::endl;

      switch (mech->mech_type) {
      case MECH_A: // a
        Rcout << "A" << std::endl;
        break;
      case MECH_MX: // mx
        Rcout << "MX" << std::endl;
        break;
      case MECH_PTR: // ptr
        Rcout << "PTR" << std::endl;
        break;
      case MECH_INCLUDE: // include
        Rcout << "INCLUDE" << std::endl;
        break;
      case MECH_IP4: // ip4 machinations
        Rcout << "IP4" << std::endl;
        // p_err = inet_ntop( AF_INET, SPF_mech_ip4_data( mech ),
        // 				 ip4_buf, sizeof( ip4_buf ) );
        // if ( p_err == NULL )
        // 	return SPF_E_INTERNAL_ERROR;
        // if ( mech->mech_len )
        // 	len = snprintf( p, p_end - p, "ip4:%s/%d",
        // 						ip4_buf, mech->mech_len );
        // else
        // 	len = snprintf( p, p_end - p, "ip4:%s", ip4_buf );
        break;
      case MECH_IP6: // ip6 machinations
        Rcout << "IP6" << std::endl;
        // p_err = inet_ntop( AF_INET6, SPF_mech_ip6_data( mech ),
        // 				 ip6_buf, sizeof( ip6_buf ) );
        // if ( p_err == NULL )
        // 	return SPF_E_INTERNAL_ERROR;
        // if ( mech->mech_len )
        // 	len = snprintf( p, p_end - p, "ip6:%s/%d",
        // 						ip6_buf, mech->mech_len );
        // else
        // 	len = snprintf( p, p_end - p, "ip6:%s", ip6_buf );
        break;
      case MECH_EXISTS: // exists
        Rcout << "EXISTS" << std::endl;
        break;
      case MECH_ALL: // all
        Rcout << "ALL" << std::endl;
        break;
      case MECH_REDIRECT: // redirect
        Rcout << "REDIRECT" << std::endl;
        break;
      default:
        break;
      }

      if (mech->mech_type != MECH_IP4  &&  mech->mech_type != MECH_IP6) {

        data = SPF_mech_data(mech);
        data_end = SPF_mech_end_data(mech);

        char buf[SPF_mech_data_len(mech)];
        char *p = &buf;

        bool cidr_ok = mech->mech_type == MECH_A || mech->mech_type == MECH_MX;
        SPF_record_stringify_data(
          data, data_end,
          &p, &buf[sizeof(buf)], FALSE, cidr_ok, 0
        );

        std::string s_mech_data((char *)buf, SPF_mech_data_len(mech));
        Rcout << s_mech_data << std::endl;

        //
        //         if (SPF_mech_data_len(mech) > 0
        //               /* We have an immediate string literal */
        //               && (data->dc.parm_type != PARM_CIDR
        //               /* Some data follows the CIDR */
        //               || SPF_data_next( data ) < data_end)
        //         ) {
        //           *p++ = ':';
        //         }

      }

      mech = SPF_mech_next(mech);

    }

    mod = spf_record->mod_first;
    for (int i = 0; i<spf_record->num_mod; i++) {

      std::string s_mod_name(SPF_mod_name(mod), mod->name_len);
      Rcout << s_mod_name << " => ";

      data = SPF_mod_data(mod);
      data_end = SPF_mod_end_data(mod);

      std::string s_mod_data((char *)data, data_end-data);
      Rcout << s_mod_data << std::endl;

      mod = SPF_mod_next(mod);

    }

    l = List::create(
      _["version"] = (unsigned int)spf_record->version,
      _["num_mech"] = (unsigned int)spf_record->num_mech,
      _["num_mod"] = (unsigned int)spf_record->num_mod
    );


    if (spf_record) {
      SPF_record_free(spf_record);
      spf_record = NULL;
    }

    return(l);

  }

}
