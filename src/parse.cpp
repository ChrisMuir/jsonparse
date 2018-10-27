#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


//' Parse JSON String
//'
//' Takes a JSON string as input, returns a list of key-value pairs
//'
//' @param json const char, JSON string to be parsed. Coming from R, this
//'  input should be a character vector of length 1.
//' @export
// [[Rcpp::export]]
List from_json(const char * json) {

  rapidjson::Document doc;
  doc.Parse(json);

  // Make sure there were no parse errors
  if(doc.HasParseError()) {
    Rcerr << "parse error for json string: "<< json << std::endl;
    stop("json parse error");
  }

  int json_len = doc.MemberCount();
  List out(json_len);
  CharacterVector names(json_len);

  int i = 0;
  for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {

    // Get current key
    names[i] = itr->name.GetString();

    // Get current value
    switch(itr->value.GetType()) {

      // bool
      case 2: {
        out[i] = itr->value.GetBool();
        break;
      }

      // string
      case 5: {
        out[i] = itr->value.GetString();
        break;
      }

      // numeric
      case 6: {
        if(itr->value.IsDouble()) {
          // double
          out[i] = itr->value.GetDouble();
        } else {
          // int
          out[i] = itr->value.GetInt();
        }
        break;
      }

      // some other data type not covered
      default: {
        stop("Uknown data type. Only able to parse int, double, string, bool");
      }
    }

    i++;
  }

  out.attr("names") = names;
  return out;
}
