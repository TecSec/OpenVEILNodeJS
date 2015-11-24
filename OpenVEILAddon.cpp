//	Copyright (c) 2015, TecSec, Inc.
//
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//	
//		* Redistributions of source code must retain the above copyright
//		  notice, this list of conditions and the following disclaimer.
//		* Redistributions in binary form must reproduce the above copyright
//		  notice, this list of conditions and the following disclaimer in the
//		  documentation and/or other materials provided with the distribution.
//		* Neither the name of TecSec nor the names of the contributors may be
//		  used to endorse or promote products derived from this software 
//		  without specific prior written permission.
//		 
//	ALTERNATIVELY, provided that this notice is retained in full, this product
//	may be distributed under the terms of the GNU General Public License (GPL),
//	in which case the provisions of the GPL apply INSTEAD OF those given above.
//		 
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED.  IN NO EVENT SHALL TECSEC BE LIABLE FOR ANY 
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "OpenVEILAddon.h"

using namespace v8;

Persistent<Function> OpenVEILAddon::constructor;

OpenVEILAddon::OpenVEILAddon() {
	_value = ::ServiceLocator()->try_get_instance<IKeyVEILConnector>("/KeyVEILConnector");
}

OpenVEILAddon::~OpenVEILAddon() {
}

void OpenVEILAddon::Init(Handle<Object> exports) {
	Isolate* isolate = Isolate::GetCurrent();

	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
	tpl->SetClassName(String::NewFromUtf8(isolate, "OpenVEIL"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	NODE_SET_PROTOTYPE_METHOD(tpl, "genericConnect", GenericConnect);
	NODE_SET_PROTOTYPE_METHOD(tpl, "kvConnect", KvConnect);
	NODE_SET_PROTOTYPE_METHOD(tpl, "disconnect", Disconnect);
	NODE_SET_PROTOTYPE_METHOD(tpl, "isConnected", IsConnected);
	NODE_SET_PROTOTYPE_METHOD(tpl, "sendJsonRequest", SendJsonRequest);
	NODE_SET_PROTOTYPE_METHOD(tpl, "sendRequest", SendRequest);

	constructor.Reset(isolate, tpl->GetFunction());
	exports->Set(String::NewFromUtf8(isolate, "OpenVEIL"),
		tpl->GetFunction());
}

void OpenVEILAddon::New(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new MyObject(...)`
		//double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
		OpenVEILAddon* obj = new OpenVEILAddon(/*value*/);
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	}
	else {
		// Invoked as plain function `MyObject(...)`, turn into construct call.
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		Local<Function> cons = Local<Function>::New(isolate, constructor);
		args.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

static tsAscii StringToTsAscii(v8::Local<v8::String>& string)
{
	tsAscii tmp;
	const int length = string->Utf8Length() + 1;  // Add one for trailing zero byte.
	tmp.resize(length);
	string->WriteOneByte((uint8_t*)tmp.rawData(), /* start */ 0, length);
	return tmp;
}

void OpenVEILAddon::GenericConnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());

	if (args.Length() < 3)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	int retVal = obj->genericConnect(StringToTsAscii(args[0]->ToString()), StringToTsAscii(args[1]->ToString()), StringToTsAscii(args[2]->ToString()));
	args.GetReturnValue().Set(Number::New(isolate, retVal));
}
void OpenVEILAddon::KvConnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());

	if (args.Length() < 3)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	int retVal = obj->kvConnect(StringToTsAscii(args[0]->ToString()), StringToTsAscii(args[1]->ToString()), StringToTsAscii(args[2]->ToString()));
	args.GetReturnValue().Set(Number::New(isolate, retVal));
}
void OpenVEILAddon::Disconnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());
	obj->disconnect();
}
void OpenVEILAddon::IsConnected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());
	args.GetReturnValue().Set(Boolean::New(isolate, obj->isConnected()));
}
void OpenVEILAddon::SendJsonRequest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());

	if (args.Length() < 3)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	tsAscii outData;
	int status = 0;
	bool retVal = obj->sendJsonRequest(StringToTsAscii(args[0]->ToString()), StringToTsAscii(args[1]->ToString()), StringToTsAscii(args[2]->ToString()), outData, status);

	Local<Object> outObj = Object::New(isolate);
	outObj->Set(String::NewFromUtf8(isolate, "status"), Number::New(isolate, status));
	outObj->Set(String::NewFromUtf8(isolate, "retVal"), Boolean::New(isolate, retVal));
	outObj->Set(String::NewFromUtf8(isolate, "outData"), String::NewFromUtf8(isolate, outData.c_str()));

	args.GetReturnValue().Set(outObj);
}
void OpenVEILAddon::SendRequest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	OpenVEILAddon* obj = ObjectWrap::Unwrap<OpenVEILAddon>(args.Holder());

	if (args.Length() < 3)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	tsAscii outData;
	int status = 0;
	bool retVal = obj->sendRequest(StringToTsAscii(args[0]->ToString()), StringToTsAscii(args[1]->ToString()), StringToTsAscii(args[2]->ToString()), outData, status);

	Local<Object> outObj = Object::New(isolate);
	outObj->Set(String::NewFromUtf8(isolate, "status"), Number::New(isolate, status));
	outObj->Set(String::NewFromUtf8(isolate, "retVal"), Boolean::New(isolate, retVal));
	outObj->Set(String::NewFromUtf8(isolate, "outData"), String::NewFromUtf8(isolate, outData.c_str()));

	args.GetReturnValue().Set(outObj);
}

