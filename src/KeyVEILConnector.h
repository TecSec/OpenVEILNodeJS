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

#ifndef __KEYVEILCONNECTOR_H__
#define __KEYVEILCONNECTOR_H__

#include "OpenVEIL.h"
#include <nan.h>

class Token;

class KeyVEILConnector : public Nan::ObjectWrap
{
public:
	static NAN_MODULE_INIT(Init)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("KeyVEILConnector").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
		Nan::SetPrototypeMethod(tpl, "connect", Connect);
		Nan::SetPrototypeMethod(tpl, "disconnect", Disconnect);

		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("isConnected").ToLocalChecked(), IsConnected);
		Nan::SetPrototypeMethod(tpl, "sendJsonRequest", SendJsonRequest);
		Nan::SetPrototypeMethod(tpl, "sendRequest", SendRequest);


		Nan::SetPrototypeMethod(tpl, "refresh", Refresh);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("tokenCount").ToLocalChecked(), TokenCount);
		Nan::SetPrototypeMethod(tpl, "tokenByIndex", TokenByIndex);
		Nan::SetPrototypeMethod(tpl, "tokenByName", TokenByName);
		Nan::SetPrototypeMethod(tpl, "tokenBySerialNumber", TokenBySerialNumber);
		Nan::SetPrototypeMethod(tpl, "tokenById", TokenById);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("favoriteCount").ToLocalChecked(), FavoriteCount);
		Nan::SetPrototypeMethod(tpl, "favoriteByIndex", FavoriteByIndex);
		Nan::SetPrototypeMethod(tpl, "favoriteByName", FavoriteByName);
		Nan::SetPrototypeMethod(tpl, "favoriteById", FavoriteById);
		Nan::SetPrototypeMethod(tpl, "createFavorite_token", CreateFavorite_token);
		Nan::SetPrototypeMethod(tpl, "createFavorite_serial", CreateFavorite_serial);
		Nan::SetPrototypeMethod(tpl, "createFavorite_id", CreateFavorite_id);
		Nan::SetPrototypeMethod(tpl, "deleteFavorite", DeleteFavorite);
		Nan::SetPrototypeMethod(tpl, "updateFavoriteName", UpdateFavoriteName);
		Nan::SetPrototypeMethod(tpl, "updateFavorite", UpdateFavorite);
		Nan::SetPrototypeMethod(tpl, "tokenCountForEnterpriseId", TokenCountForEnterpriseId);
		Nan::SetPrototypeMethod(tpl, "tokenForEnterprise", TokenForEnterprise);
		Nan::SetPrototypeMethod(tpl, "favoriteCountForEnterprise", FavoriteCountForEnterprise);
		Nan::SetPrototypeMethod(tpl, "favoriteForEnterprise", FavoriteForEnterprise);



		constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
		target->Set(Nan::New("KeyVEILConnector").ToLocalChecked(), tpl->GetFunction());
	}

private:
	explicit KeyVEILConnector();
	~KeyVEILConnector();

	std::shared_ptr<IKeyVEILConnector> _value;
	ConnectionStatus connect(const tsAscii& url, const tsAscii& username, const tsAscii& password)
	{
		if (!isReady())
		{
			return connStatus_NoServer;
		}

		return _value->connect(url, username, password);
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
	bool sendJsonRequest(const tsAscii& verb, const tsAscii& cmd, const tsAscii& inData, tsAscii& outData, int& status)
	{
		if (isReady())
		{
			JSONObject inObj, outObj;

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
	bool sendRequest(const tsAscii& verb, const tsAscii& cmd, const tsAscii& inData, tsAscii& outData, int& status)
	{
		if (isReady())
		{
			tsData inObj, outObj;

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

	bool refresh()
	{
		if (!isReady())
			return false;
		return _value->refresh();
	}
	size_t tokenCount()
	{
		if (!isReady())
			return 0;
		return _value->tokenCount();
	}
	size_t favoriteCount()
	{
		if (!isReady())
			return 0;
		return _value->favoriteCount();
	}
	tsAscii createFavorite(Token* token, const tsData& headerData, const tsAscii& name);
	tsAscii createFavorite(const tsAscii& tokenId, const tsData& headerData, const tsAscii& name)
	{
		if (!isReady())
			return "";
		return ToString()(_value->CreateFavorite(ToGuid()(tokenId), headerData, name));
	}
	tsAscii createFavorite(const tsData& tokenSerial, const tsData& headerData, const tsAscii& name)
	{
		if (!isReady())
			return "";
		return ToString()(_value->CreateFavorite(tokenSerial, headerData, name));
	}
	bool deleteFavorite(const tsAscii& id)
	{
		if (!isReady())
			return false;
		return _value->DeleteFavorite(ToGuid()(id));
	}
	bool updateFavoriteName(const tsAscii& id, const tsAscii& name)
	{
		if (!isReady())
			return false;
		return _value->UpdateFavoriteName(ToGuid()(id), name);
	}
	bool updateFavorite(const tsAscii& id, const tsData& setTo)
	{
		if (!isReady())
			return false;
		return _value->UpdateFavorite(ToGuid()(id), setTo);
	}
	size_t tokenCountForEnterpriseId(const tsAscii& enterpriseId)
	{
		if (!isReady())
			return 0;
		return _value->tokenCountForEnterprise(ToGuid()(enterpriseId));
	}
	size_t favoriteCountForEnterprise(const tsAscii& enterpriseId)
	{
		if (!isReady())
			return 0;
		return _value->favoriteCountForEnterprise(ToGuid()(enterpriseId));
	}


	static NAN_METHOD(New)
	{
		if (info.IsConstructCall()) {
			// Invoked as constructor: `new MyObject(...)`
			//double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
			KeyVEILConnector* obj = new KeyVEILConnector(/*value*/);
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
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
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
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		obj->disconnect();
	}
	static NAN_GETTER(IsConnected)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		info.GetReturnValue().Set(obj->isConnected());
	}
	static NAN_METHOD(SendJsonRequest)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 3)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto verb = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto cmd = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto inData = Nan::To<v8::String>(info[2]).ToLocalChecked();

		tsAscii outData;
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
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 3)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto verb = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto cmd = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto inData = Nan::To<v8::String>(info[2]).ToLocalChecked();

		tsAscii outData;
		int status = 0;
		bool retVal = obj->sendRequest(*Nan::Utf8String(verb), *Nan::Utf8String(cmd), *Nan::Utf8String(inData), outData, status);

		v8::Local<v8::Object> outObj = Nan::New<v8::Object>();
		outObj->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));
		outObj->Set(Nan::New("retVal").ToLocalChecked(), Nan::New(retVal));
		outObj->Set(Nan::New("outData").ToLocalChecked(), Nan::New(outData.c_str()).ToLocalChecked());

		info.GetReturnValue().Set(outObj);
	}


	static NAN_METHOD(Refresh)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		info.GetReturnValue().Set(obj->refresh());
	}
	static NAN_GETTER(TokenCount)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		info.GetReturnValue().Set(obj->tokenCount());
	}
	static NAN_METHOD(TokenByIndex);
	static NAN_METHOD(TokenByName);
	static NAN_METHOD(TokenBySerialNumber);
	static NAN_METHOD(TokenById);
	static NAN_GETTER(FavoriteCount)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		info.GetReturnValue().Set(obj->favoriteCount());
	}
	static NAN_METHOD(FavoriteByIndex);
	static NAN_METHOD(FavoriteByName);
	static NAN_METHOD(FavoriteById);
	static NAN_METHOD(CreateFavorite_token);
	static NAN_METHOD(CreateFavorite_serial);
	static NAN_METHOD(CreateFavorite_id);
	static NAN_METHOD(DeleteFavorite)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
		info.GetReturnValue().Set(obj->deleteFavorite(*Nan::Utf8String(id)));
	}
	static NAN_METHOD(UpdateFavoriteName)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 2)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto name = Nan::To<v8::String>(info[1]).ToLocalChecked();
		info.GetReturnValue().Set(obj->updateFavoriteName(*Nan::Utf8String(id), *Nan::Utf8String(name)));
	}
	static NAN_METHOD(UpdateFavorite)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 2)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto data = Nan::To<v8::Object>(info[1]).ToLocalChecked();

		char* content = node::Buffer::Data(data);
		int contentlength = node::Buffer::Length(data);

		info.GetReturnValue().Set(obj->updateFavorite(*Nan::Utf8String(id), tsData((uint8_t*)content, contentlength)));
	}
	static NAN_METHOD(TokenCountForEnterpriseId)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
		info.GetReturnValue().Set(obj->tokenCountForEnterpriseId(*Nan::Utf8String(id)));
	}
	static NAN_METHOD(TokenForEnterprise);
	static NAN_METHOD(FavoriteCountForEnterprise)
	{
		KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}
		auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
		info.GetReturnValue().Set(obj->favoriteCountForEnterprise(*Nan::Utf8String(id)));
	}
	static NAN_METHOD(FavoriteForEnterprise);

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}
};

#endif // __OPENVEILADDON_H__