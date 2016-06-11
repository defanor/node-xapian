#include "node-xapian.h"

Persistent<FunctionTemplate> QueryParser::constructor_template;

void QueryParser::Init(Handle<Object> target) {
  constructor_template = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("QueryParser"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "parse_query", ParseQuery);

  target->Set(String::NewSymbol("QueryParser"), constructor_template->GetFunction());
}


static int kNewQueryParser[] = { eEnd };
Handle<Value> QueryParser::New(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  if (!checkArguments(kNewQueryParser, args, aOpt))
    return throwSignatureErr(kNewQueryParser);

  QueryParser* that;
  try {
    that = new QueryParser();
  } catch (const Xapian::Error& err) {
    return ThrowException(Exception::Error(String::New(err.get_msg().c_str())));
  }
  that->Wrap(args.This());
  return args.This();
}

static int kParseQuery[] = { eString, eEnd };
Handle<Value> QueryParser::ParseQuery(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  if (!checkArguments(kParseQuery, args, aOpt))
    return throwSignatureErr(kParseQuery);

  QueryParser* that = ObjectWrap::Unwrap<QueryParser>(args.This());
  // if (that->mBusy)
  //   return ThrowException(Exception::Error(kBusyMsg));

  Xapian::Query aQuery;
  try {
    aQuery = that->mQueryParser.parse_query(*String::Utf8Value(args[0]->ToString()));
  } catch (const Xapian::Error& err) {
    return ThrowException(Exception::Error(String::New(err.get_msg().c_str())));
  }

  Local<Value> aQueryParam[] = { External::New(&aQuery) };
  Handle<Object> aResult = Query::constructor_template->GetFunction()->NewInstance(1, aQueryParam);

  return scope.Close(aResult);
}
