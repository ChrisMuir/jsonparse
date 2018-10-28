#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


// Parse an array object, return an SEXP that contains the objects from the
// array.
SEXP parse_array(rapidjson::Value::ConstArray& array) {
  int array_len = array.Size();
  int data_type = array[0].GetType();
  if(data_type == 2) {
    data_type = 1;
  }
  bool list_out = false;

  // Check to see if the array has different data types, which means return
  // will be a list. Exception to this rule is for bool types...false has
  // data type of 1, and true has data type of 2. So, effectively, data types
  // 1 and 2 are identical (since they're both bool).
  int curr_dtype;
  for(int i = 1; i < array_len; ++i) {
    curr_dtype = array[i].GetType();
    if(curr_dtype == 2) {
      curr_dtype = 1;
    }
    if(array[i].GetType() != data_type) {
      list_out = true;
      break;
    }
  }

  // TODO: Add support for list vectors.
  if(list_out) {
    // Do stuff
  }

  // Get current value
  switch(data_type) {

    // bool
    case 1: {
      LogicalVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        out[i] = array[i].GetBool();
      }
      return out;
    }

    // string
    case 5: {
      CharacterVector out(array_len);
      for(int i = 0; i < array_len; ++i) {
        out[i] = array[i].GetString();
      }
      return out;
    }

    // numeric
    case 6: {
      if(array[0].IsDouble()) {
        // double
        NumericVector out(array_len);
        for(int i = 0; i < array_len; ++i) {
          out[i] = array[i].GetDouble();
        }
        return out;
      } else {
        // int
        IntegerVector out(array_len);
        for(int i = 0; i < array_len; ++i) {
          out[i] = array[i].GetInt();
        }
        return out;
      }
    }

  }

  return R_NilValue;
}


List parse_value(rapidjson::Value& val) {
  int json_len = val.Size();
  List out(json_len);
  CharacterVector names(json_len);

  int i = 0;
  for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {

    // Get current key
    names[i] = itr->name.GetString();

    // Get current value
    switch(itr->value.GetType()) {

      // bool - false
      case 1: {
        out[i] = itr->value.GetBool();
        break;
      }

      // bool - true
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

      // array
      case 4: {
        rapidjson::Value::ConstArray curr_array = itr->value.GetArray();
        out[i] = parse_array(curr_array);
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


List parse_document(rapidjson::Document& doc) {
  int json_len = doc.Size();
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

      // array
      case 4: {
        rapidjson::Value::ConstArray curr_array = itr->value.GetArray();
        out[i] = parse_array(curr_array);
        break;
      }

        // some other data type not covered
      default: {
        stop("Uknown data type. Only able to parse int, double, string, bool");
      }
    }

    // Bump i
    i++;
  }

  out.attr("names") = names;
  return out;
}


//' Parse JSON String
//'
//' Takes a JSON string as input, returns an R list of key-value pairs
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

  // If input is not an array, pass doc through parse_document(), and return
  // the result.
  if(!doc.IsArray()) {
    return parse_document(doc);
  }

  // Else if input is an array, pass each element of doc through parse_value(),
  // saving the results of each iteration in an Rcpp::List.
  rapidjson::Value curr_val;
  int json_len = doc.Size();
  List out(json_len);
  CharacterVector names(json_len);

  for(int i = 0; i < json_len; ++i) {
    curr_val = doc[i];
    out[i] = parse_value(curr_val);
  }

  out.attr("names") = names;
  return out;
}
