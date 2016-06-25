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

#ifndef __GENERICCONNECTOR_H__
#define __GENERICCONNECTOR_H__

#include "VEIL.h"
#include <nan.h>

class GenericConnector : public Nan::ObjectWrap
{
public:
	static NAN_MODULE_INIT(Init)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("GenericConnector").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
		Nan::SetPrototypeMethod(tpl, "connect", Connect);
		Nan::SetPrototypeMethod(tpl, "disconnect", Disconnect);

		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("isConnected").ToLocalChecked(), IsConnected);
		Nan::SetPrototypeMethod(tpl, "sendJsonRequest", SendJsonRequest);
		Nan::SetPrototypeMethod(tpl, "sendRequest", SendRequest);


		constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
		target->Set(Nan::New("GenericConnector").ToLocalChecked(), tpl->GetFunction());
	}

private:
	explicit GenericConnector();
	~GenericConnector();

	std::shared_ptr<IKeyVEILConnector> _value;
	ConnectionStatus connect(const tscrypto::tsCryptoString& url, const tscrypto::tsCryptoString& username, const tscrypto::tsCryptoString& password)
	{
		if (!isReady())
		{
			return connStatus_NoServer;
		}

		return _value->genericConnectToServer(url, username, password);
	}
	void disconnect()
	{
		if (isConnected())
		{
			_value->disconnect();
		}
	}
	bool isConnected()
	{
		if (isReady())
		{
			return _value->isConnected();
		}
		else
		{
			return false;
		}
	}
	bool sendJsonRequest(const tscrypto::tsCryptoString& verb, const tscrypto::tsCryptoString& cmd, const tscrypto::tsCryptoString& inData, tscrypto::tsCryptoString& outData, int& status)
	{
		if (isReady())
		{
			tscrypto::JSONObject inObj, outObj;

			if (inObj.FromJSON(inData.c_str()) <= 0)
				return false;

			bool retVal = _value->sendJsonRequest(verb, cmd, inObj, outObj, status);
			outData = outObj.ToJSON();
			return retVal;
		}
		else
		{
			return false;
		}
	}
	bool sendRequest(const tscrypto::tsCryptoString& verb, const tscrypto::tsCryptoString& cmd, const tscrypto::tsCryptoString& inData, tscrypto::tsCryptoString& outData, int& status)
	{
		if (isReady())
		{
			tscrypto::tsCryptoData inObj, outObj;

			inObj = inData.Base64ToData();

			bool retVal = _value->sendRequest(verb, cmd, inObj, outObj, status);
			outData = outObj.ToBase64();
			return retVal;
		}
		else
		{
			return false;
		}
	}
	bool isReady()
	{
		return !!_value;
	}



	static NAN_METHOD(New)
	{
		if (info.IsConstructCall()) {
			// Invoked as constructor: `new MyObject(...)`
			//double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
			GenericConnector* obj = new GenericConnector(/*value*/);
			obj->Wrap(info.This());
			info.GetReturnValue().Set(info.This());
		}
		else {
			// Invoked as plain function `MyObject(...)`, turn into construct call.
			const int argc = 1;
			v8::Local<v8::Value> argv[argc] = { info[0] };
			v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor());
			info.GetReturnValue().Set(cons->NewInstance(argc, argv));
		}
	}
	static NAN_METHOD(Connect)
	{
		GenericConnector* obj = ObjectWrap::Unwrap<GenericConnector>(info.This());
		if (info.Length() < 3)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto url = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto username = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto password = Nan::To<v8::String>(info[2]).ToLocalChecked();
		switch (obj->connect(*Nan::Utf8String(url), *Nan::Utf8String(username), *Nan::Utf8String(password)))
		{
		case connStatus_Connected:
			info.GetReturnValue().Set(Nan::New("Connected").ToLocalChecked());
			break;
		case connStatus_NoServer:
			info.GetReturnValue().Set(Nan::New("NoServer").ToLocalChecked());
			break;
		case connStatus_BadAuth:
			info.GetReturnValue().Set(Nan::New("BadAuth").ToLocalChecked());
			break;
		case connStatus_WrongProtocol:
			info.GetReturnValue().Set(Nan::New("WrongProtocol").ToLocalChecked());
			break;
		case connStatus_UrlBad:
			info.GetReturnValue().Set(Nan::New("UrlBad").ToLocalChecked());
			break;
		default:
			info.GetReturnValue().Set(Nan::New("unknown").ToLocalChecked());
			break;
		}
	}
	static NAN_METHOD(Disconnect)
	{
		GenericConnector* obj = ObjectWrap::Unwrap<GenericConnector>(info.This());
		obj->disconnect();
	}
	static NAN_GETTER(IsConnected)
	{
		GenericConnector* obj = ObjectWrap::Unwrap<GenericConnector>(info.This());
		info.GetReturnValue().Set(obj->isConnected());
	}
	static NAN_METHOD(SendJsonRequest)
	{
		GenericConnector* obj = ObjectWrap::Unwrap<GenericConnector>(info.This());
		if (info.Length() < 3)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto verb = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto cmd = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto inData = Nan::To<v8::String>(info[2]).ToLocalChecked();

		tscrypto::tsCryptoString outData;
		int status = 0;
		bool retVal = obj->sendJsonRequest(*Nan::Utf8String(verb), *Nan::Utf8String(cmd), *Nan::Utf8String(inData), outData, status);

		v8::Local<v8::Object> outObj = Nan::New<v8::Object>();
		outObj->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));
		outObj->Set(Nan::New("retVal").ToLocalChecked(), Nan::New(retVal));
		outObj->Set(Nan::New("outData").ToLocalChecked(), Nan::New(outData.c_str()).ToLocalChecked());

		info.GetReturnValue().Set(outObj);
	}
	static NAN_METHOD(SendRequest)
	{
		GenericConnector* obj = ObjectWrap::Unwrap<GenericConnector>(info.This());
		if (info.Length() < 3)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto verb = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto cmd = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto inData = Nan::To<v8::String>(info[2]).ToLocalChecked();

		tscrypto::tsCryptoString outData;
		int status = 0;
		bool retVal = obj->sendRequest(*Nan::Utf8String(verb), *Nan::Utf8String(cmd), *Nan::Utf8String(inData), outData, status);

		v8::Local<v8::Object> outObj = Nan::New<v8::Object>();
		outObj->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));
		outObj->Set(Nan::New("retVal").ToLocalChecked(), Nan::New(retVal));
		outObj->Set(Nan::New("outData").ToLocalChecked(), Nan::New(outData.c_str()).ToLocalChecked());

		info.GetReturnValue().Set(outObj);
	}
	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}
};

#endif // __GENERICCONNECTOR_H__