#include <napi.h>

class ActualClass {
 public:
  ActualClass(double value); //constructor
  double getValue(); //getter for the value
  double add(double toAdd); //adds the toAdd value to the value_
 private:
  double value_;
};

ActualClass::ActualClass(double value){
    this->value_ = value;
}

double ActualClass::getValue()
{
  return this->value_;
}

double ActualClass::add(double toAdd)
{
  this->value_ += toAdd;
  return this->value_;
}


class ClassExample : public Napi::ObjectWrap<ClassExample> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports); //Init function for setting the export key to JS
  ClassExample(const Napi::CallbackInfo& info); //Constructor to initialise

 private:
  static Napi::FunctionReference constructor; //reference to store the class definition that needs to be exported to JS
  Napi::Value GetValue(const Napi::CallbackInfo& info); //wrapped getValue function
  Napi::Value Add(const Napi::CallbackInfo& info); //wrapped add function
  ActualClass *actualClass_; //internal instance of actualclass used to perform actual operations.
};

Napi::FunctionReference ClassExample::constructor;

Napi::Object ClassExample::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "ClassExample", {
    InstanceMethod("add", &ClassExample::Add),
    InstanceMethod("getValue", &ClassExample::GetValue),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("ClassExample", func);
  return exports;
}

ClassExample::ClassExample(const Napi::CallbackInfo& info) : Napi::ObjectWrap<ClassExample>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  int length = info.Length();
  if (length != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
  }

  Napi::Number value = info[0].As<Napi::Number>();
  this->actualClass_ = new ActualClass(value.DoubleValue());
}

Napi::Value ClassExample::GetValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  double num = this->actualClass_->getValue();
  return Napi::Number::New(env, num);
}


Napi::Value ClassExample::Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (  info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
  }

  Napi::Number toAdd = info[0].As<Napi::Number>();
  double answer = this->actualClass_->add(toAdd.DoubleValue());

  return Napi::Number::New(info.Env(), answer);
}


Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  //return exports;
  return ClassExample::Init(env, exports);
}

NODE_API_MODULE(testaddon, InitAll)
