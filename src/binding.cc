
#include <xapian.h>
#include "mime2text.h"

#include <stdlib.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include "node-xapian.h"

using namespace v8;
using namespace node;

Persistent<String> kBusyMsg;

void tryCallCatch(Handle<Function> fn, Handle<Object> context, int argc, Handle<Value>* argv) {
  TryCatch try_catch;

  fn->Call(context, argc, argv);

  if (try_catch.HasCaught())
    FatalException(try_catch);
}

void sendToThreadPool(FuncPool execute, FuncDone done, AsyncOpBase* data){
  uv_work_t* aReq = new uv_work_t;
  aReq->data = data;

  uv_queue_work(uv_default_loop(), aReq, execute, done);

  data->RefUv(aReq);
}

bool checkArguments(int signature[], const Arguments& args, int optionals[]) {
  int aArgN = 0, aOptN = 0;

  for (int aSigN=0; signature[aSigN] != eEnd; ++aSigN) {
    int aIsType;
    switch (abs(signature[aSigN])) {
    case eInt32:          aIsType = args[aArgN]->IsInt32();                                  break;
    case eUint32:         aIsType = args[aArgN]->IsUint32();                                 break;
    case eBoolean:        aIsType = args[aArgN]->IsBoolean();                                break;
    case eString:         aIsType = args[aArgN]->IsString();                                 break;
    case eArray:          aIsType = args[aArgN]->IsArray();                                  break;
    case eBuffer:
    case eObjectDatabase:
    case eObjectStem:
    case eObjectRSet:
    case eObjectDocument:
    case eObject:         aIsType = args[aArgN]->IsObject() && !args[aArgN]->IsFunction();   break;
    case eNull:           aIsType = args[aArgN]->IsNull();                                   break;
    case eFunction:       aIsType = args[aArgN]->IsFunction();                               break;
    default: { std::string aEx("incorrect signature member: "); aEx += (char)(signature[aSigN] + '0'); throw aEx; }
    }

    if (signature[aSigN] < 0) {
      optionals[aOptN] = aIsType ? aArgN++ : -1;
      ++aOptN;
    } else {
      if (!aIsType) return false;
      ++aArgN;
    }
  }

  return aArgN == args.Length();
}

static std::string generateSignatureString(int signature[]) {
  std::string aStr = "(";
  for (int aSigN=0; signature[aSigN] != eEnd; ++aSigN) {
    if (aSigN) aStr += ", ";
    if (signature[aSigN] < 0) aStr += "[";
    switch (abs(signature[aSigN])) {
    case eInt32:          aStr += "int32";    break;
    case eUint32:         aStr += "uint32";   break;
    case eBoolean:        aStr += "boolean";  break;
    case eString:         aStr += "string";   break;
    case eObject:         aStr += "object";   break;
    case eArray:          aStr += "array";    break;
    case eBuffer:         aStr += "Buffer";   break;
    case eObjectDatabase: aStr += "Database"; break;
    case eObjectStem:     aStr += "Stem";     break;
    case eObjectRSet:     aStr += "RSet";     break;
    case eObjectDocument: aStr += "Document"; break;
    case eNull:           aStr += "null";     break;
    case eFunction:       aStr += "function"; break;
    default:         throw "incorrect sig member"; 
    }
    if (signature[aSigN] < 0) aStr += "]";
  }
  aStr += ")";
  return aStr;
}

Handle<Value> throwSignatureErr(int signature[]) {
  std::string aStr("arguments are ");
  aStr += generateSignatureString(signature);
  return ThrowException(Exception::TypeError(String::New(aStr.c_str())));
}

Handle<Value> throwSignatureErr(int *signatures[], int sigN) {
  std::string aStr("arguments are ");
  for (int a = 0; a < sigN; a++) {
    if (a > 0)
      aStr += " or ";
    aStr += generateSignatureString(signatures[a]);
  }
  return ThrowException(Exception::TypeError(String::New(aStr.c_str())));
}

class Mime2Text : public ObjectWrap {
public:
  static void Init(Handle<Object> target);

  static Persistent<FunctionTemplate> constructor_template;

  Xapian::Mime2Text m2T;

protected:
  Mime2Text() : ObjectWrap(), m2T(), mBusy(false) {}

  ~Mime2Text() { }

  bool mBusy;

  friend struct AsyncOp<Mime2Text>;
  friend struct Main_data;

  static Handle<Value> New(const Arguments& args);

  static Handle<Value> Convert(const Arguments& args);
  static void Convert_pool(uv_work_t* req);
  static void Convert_done(uv_work_t* req, int);
  struct Convert_data : AsyncOp<Mime2Text> {
    Convert_data(Handle<Object> ob, Handle<Function> cb, Handle<String> fi, Handle<Value> ty)
      : AsyncOp<Mime2Text>(ob, cb), filename(fi), type(ty->IsString() ? ty : Handle<Value>()) {}
    String::Utf8Value filename;
    String::Utf8Value type;
    Xapian::Mime2Text::Fields fields;
  };
};

static Handle<Value> AssembleDocument(const Arguments& args);
static void Main_pool(uv_work_t* req);
static void Main_done(uv_work_t* req, int);
struct Main_data : public AsyncOpBase {
  Main_data(Handle<Function> cb, Xapian::Document* doc, TermGenerator* tg, String::Utf8Value** tl, Mime2Text* m2t, Handle<Value> p, Handle<Value> m)
    : AsyncOpBase(cb), document(doc), termgen(tg), textlist(tl), mime2text(m2t), path(p), mimetype(m) {
    termgen->Ref();
    mime2text->Ref();
  }
  ~Main_data() {
    if (textlist) {
      for (int a=0; textlist[a]; ++a)
        delete textlist[a];
      delete [] textlist;
    }
    termgen->Unref();
    mime2text->Unref();
  }
  Xapian::Document* document;
  TermGenerator* termgen;
  String::Utf8Value** textlist;
  Mime2Text* mime2text;
  String::Utf8Value path;
  String::Utf8Value mimetype;
  Xapian::Mime2Text::Fields fields;
};

extern "C"
void init (Handle<Object> target) {
  HandleScope scope;
  kBusyMsg = Persistent<String>::New(String::New("object busy with async op"));
  Database::Init(target);
  WritableDatabase::Init(target);
  TermGenerator::Init(target);
  Stem::Init(target);
  Enquire::Init(target);
  Query::Init(target);
  QueryParser::Init(target);
  Document::Init(target);
  Mime2Text::Init(target);
  target->Set(String::NewSymbol("assemble_document"), FunctionTemplate::New(AssembleDocument)->GetFunction());
}

NODE_MODULE(xapian_binding, init);

Persistent<FunctionTemplate> Mime2Text::constructor_template;

void Mime2Text::Init(Handle<Object> target) {
  constructor_template = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Mime2Text"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "convert", Convert);

  target->Set(String::NewSymbol("Mime2Text"), constructor_template->GetFunction());
}

Handle<Value> Mime2Text::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length())
    return ThrowException(Exception::TypeError(String::New("arguments are ()")));

  Mime2Text* that = new Mime2Text();
  that->Wrap(args.This());

  return args.This();
}

Handle<Value> Mime2Text::Convert(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 3 || !args[0]->IsString() || (!args[1]->IsString() && !args[1]->IsNull()) || !args[2]->IsFunction())
    return ThrowException(Exception::TypeError(String::New("arguments are (string, string|null, function)")));
  Convert_data* aData;
  try {
    aData = new Convert_data(args.This(), Local<Function>::Cast(args[2]), args[0]->ToString(), args[1]);
    aData->poolDone(); // concurrent access ok
  } catch (Local<Value> ex) {
    return ThrowException(ex);
  }

  uv_work_t* aReq = new uv_work_t;
  aReq->data = aData;
  uv_queue_work(uv_default_loop(), aReq, Convert_pool, Convert_done);

  return Undefined();
}

void Mime2Text::Convert_pool(uv_work_t* req) {
  Convert_data* aData = (Convert_data*) req->data;

  try {
  int aStatus = aData->object->m2T.convert(*aData->filename, aData->type.length() ? *aData->type : NULL, &aData->fields);
  if (aStatus != Xapian::Mime2Text::Status_OK) {
    std::string aMsg("Mime2Text::convert error: ");
    aMsg += (char) (aStatus + '0');
    throw Xapian::InternalError(aMsg);
  }
  } catch (const Xapian::Error& err) {
    aData->error = new Xapian::Error(err);
  }

  return;
}

void Mime2Text::Convert_done(uv_work_t* req, int) {
  HandleScope scope;

  Convert_data* aData = (Convert_data*) req->data;

  Handle<Value> argv[2];
  if (aData->error) {
    argv[0] = Exception::Error(String::New(aData->error->get_msg().c_str()));
  } else {
    argv[0] = Null();
    Local<Object> aO(Object::New());
    aO->Set(String::NewSymbol("title"   ), String::New(aData->fields.title.c_str()));
    aO->Set(String::NewSymbol("author"  ), String::New(aData->fields.author.c_str()));
    aO->Set(String::NewSymbol("keywords"), String::New(aData->fields.keywords.c_str()));
    aO->Set(String::NewSymbol("sample"  ), String::New(aData->fields.sample.c_str()));
    aO->Set(String::NewSymbol("body"    ), String::New(aData->fields.dump.c_str()));
    aO->Set(String::NewSymbol("md5"     ), String::New(aData->fields.md5.c_str()));
    aO->Set(String::NewSymbol("mimetype"), String::New(aData->fields.mimetype.c_str()));
    aO->Set(String::NewSymbol("command" ), String::New(aData->fields.command.c_str()));
    argv[1] = aO;
  }

  tryCallCatch(aData->callback, aData->object->handle_, aData->error ? 1 : 2, argv);

  delete aData;
  delete req;

  return;
}


/*
document input object: {
  // all members optional; at least one required
  id_term: string, // boolean term; if found in index, replace/delete that document
  data: string, // pass to Document::set_data()
  text: [ string/buffer, ... ], // pass to TermGenerator::index_text()
  file: { path: string, mime_t: string, ... }, // invoke format converter library, then index_text()
  terms: { term: wdfinc, ... }, // pass to Document::add_term()
  values: { slot: value, ... } // pass to Document::add_value()
}
*/

static Handle<Value> AssembleDocument(const Arguments& args) {
  HandleScope scope;

  TermGenerator* aTg;
  Mime2Text* aM2t;
  if (args.Length() < 4 || !(aTg = GetInstance<TermGenerator>(args[0])) || !(aM2t = GetInstance<Mime2Text>(args[1]))
   || !args[2]->IsObject() || !args[3]->IsFunction())
    return ThrowException(Exception::TypeError(String::New("arguments are (TermGenerator, Mime2Text, object, function)")));

  Local<Object> aO = args[2]->ToObject();
  Local<String> aKey;
  Local<Value> aVal, aPath, aMime;
  String::Utf8Value** aTextList = NULL;
  Xapian::Document aDoc;
  try {
    if (aO->Has(aKey = String::New("id_term"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsString())
        return ThrowException(Exception::TypeError(String::New("input object id_term not a string")));
      aDoc.add_boolean_term(*String::Utf8Value(aVal));
    }
    if (aO->Has(aKey = String::New("data"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsString())
        return ThrowException(Exception::TypeError(String::New("input object data not a string")));
      aDoc.set_data(*String::Utf8Value(aVal));
    }
    if (aO->Has(aKey = String::New("file"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsObject())
        return ThrowException(Exception::TypeError(String::New("input object file not an object")));
      Local<Object> aFile = aVal->ToObject();
      aPath = aFile->Get(String::New("path"));
      if (!aPath->IsString())
        return ThrowException(Exception::TypeError(String::New("input object file.path not a string")));
      if (aFile->Has(aKey = String::New("mime_t"))) {
        aMime = aFile->Get(aKey);
        if (!aMime->IsString())
          return ThrowException(Exception::TypeError(String::New("input object file.mime_t not a string")));
      }
    }
    if (aO->Has(aKey = String::New("terms"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsObject())
        return ThrowException(Exception::TypeError(String::New("input object terms not an object")));
      Local<Object> aTerms = aVal->ToObject();
      Local<Array> aNames = aTerms->GetPropertyNames();
      for (uint32_t a = 0; a < aNames->Length(); ++a) {
        aVal = aTerms->Get(aKey = aNames->Get(a)->ToString());
        if (aVal->IsUint32())
          aDoc.add_term(*String::Utf8Value(aKey), aVal->Uint32Value());
      }
    }
    if (aO->Has(aKey = String::New("values"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsObject())
        return ThrowException(Exception::TypeError(String::New("input object values not an object")));
      Local<Object> aValues = aVal->ToObject();
      Local<Array> aNames = aValues->GetPropertyNames();
      for (uint32_t a = 0; a < aNames->Length(); ++a) {
        aVal = aNames->Get(a);
        if (aVal->IsUint32()) {
          uint32_t aSlot = aVal->Uint32Value();
          aVal = aValues->Get(aSlot);
          if (aVal->IsString())
            aDoc.add_value(aSlot, *String::Utf8Value(aVal));
        }
      }
    }
    if (aO->Has(aKey = String::New("text"))) {
      aVal = aO->Get(aKey);
      if (!aVal->IsArray())
        return ThrowException(Exception::TypeError(String::New("input object text not an array")));
      Local<Array> aAry = Local<Array>::Cast(aVal);
      aTextList = new String::Utf8Value*[aAry->Length()+1];
      for (uint32_t a = 0; a < aAry->Length(); ++a)
        aTextList[a] = new String::Utf8Value(aAry->Get(a));
      aTextList[aAry->Length()] = NULL;
    }
    if (aVal.IsEmpty())
      return ThrowException(Exception::TypeError(String::New("input object has no relevant members")));
  } catch (const Xapian::Error& err) {
    return ThrowException(Exception::Error(String::New(err.get_msg().c_str())));
  }

  Main_data* aData = new Main_data(Local<Function>::Cast(args[3]), new Xapian::Document(aDoc), aTg, aTextList, aM2t, aPath, aMime);

  uv_work_t* aReq = new uv_work_t;
  aReq->data = aData;
  uv_queue_work(uv_default_loop(), aReq, Main_pool, Main_done);

  return Undefined();
}

static void Main_pool(uv_work_t* req) {
  Main_data* aData = (Main_data*) req->data;

  try {
  aData->termgen->mTg.set_document(*aData->document);
  for (int a = 0; aData->textlist && aData->textlist[a]; ++a) {
    aData->termgen->mTg.index_text(Xapian::Utf8Iterator(**aData->textlist[a], aData->textlist[a]->length()));
    aData->termgen->mTg.increase_termpos();
  }
  if (aData->path.length()) {
    int aStatus = aData->mime2text->m2T.convert(*aData->path, aData->mimetype.length() ? *aData->mimetype : NULL, &aData->fields);
    if (aStatus != Xapian::Mime2Text::Status_OK) {
      std::string aMsg("Mime2Text::convert error: ");
      aMsg += (char) (aStatus + '0');
      throw Xapian::InternalError(aMsg);
    }
    if (!aData->fields.title.empty()) {
      aData->termgen->mTg.index_text(aData->fields.title);
      aData->termgen->mTg.increase_termpos();
    }
    if (!aData->fields.author.empty()) {
      aData->termgen->mTg.index_text(aData->fields.author);
      aData->termgen->mTg.increase_termpos();
    }
    if (!aData->fields.keywords.empty()) {
      aData->termgen->mTg.index_text(aData->fields.keywords);
      aData->termgen->mTg.increase_termpos();
    }
    if (!aData->fields.dump.empty()) {
      aData->termgen->mTg.index_text(aData->fields.dump);
      aData->termgen->mTg.increase_termpos();
    }
  }
  } catch (const Xapian::Error& err) {
    aData->error = new Xapian::Error(err);
  }

  return;
}

static void Main_done(uv_work_t* req, int) {
  HandleScope scope;

  Main_data* aData = (Main_data*) req->data;

  Handle<Value> argv[2];
  if (aData->error) {
    argv[0] = Exception::Error(String::New(aData->error->get_msg().c_str()));
    delete aData->document;
  } else {
    argv[0] = Null();
    Local<Value> aDoc[] = { External::New(aData->document) };
    argv[1] = Document::constructor_template->GetFunction()->NewInstance(1, aDoc);
  }

  tryCallCatch(aData->callback, Context::GetCurrent()->Global(), aData->error ? 1 : 2, argv);

  delete aData;
  delete req;

  return;
}


