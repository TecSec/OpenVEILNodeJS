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

#ifndef __FAVORITE_H__
#define __FAVORITE_H__

#include "OpenVEIL.h"
#include "CmsHeader.h"
#include "FileVEILSupport.h"
#include <nan.h>

class Session;

class Favorite : public Nan::ObjectWrap
{
public:
	static NAN_MODULE_INIT(Init)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("Favorite").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		Nan::SetPrototypeMethod(tpl, "release", Release);
		Nan::SetPrototypeMethod(tpl, "encryptFile", Encrypt_File);
		Nan::SetPrototypeMethod(tpl, "encryptData", EncryptData);
		Nan::SetPrototypeMethod(tpl, "encryptString", EncryptString);

		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("favoriteId").ToLocalChecked(), GetFavoriteId, SetFavoriteId);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("enterpriseId").ToLocalChecked(), GetEnterpriseId, SetEnterpriseId);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("favoriteName").ToLocalChecked(), GetFavoriteName, SetFavoriteName);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("tokenSerialNumber").ToLocalChecked(), GetTokenSerialNumber, SetTokenSerialNumber);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("headerData").ToLocalChecked(), GetHeaderData, SetHeaderData);

		constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
		target->Set(Nan::New("Favorite").ToLocalChecked(), tpl->GetFunction());
	}

	explicit Favorite(std::shared_ptr<IFavorite> val) : _value(val) {}

	std::shared_ptr<IFavorite> handle() { return _value; }
	void handle(std::shared_ptr<IFavorite> val) { _value = val; }

	bool encryptFile(Session* session, const tsAscii& sourceFile, bool compress, const tsAscii& encryptedFile);
	tsData encryptData(Session* session, const tsData& sourceData, bool compress);

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}

private:
	explicit Favorite();
	~Favorite();

	std::shared_ptr<IFavorite> _value;

	void release()
	{
		_value.reset();
	}

	tsAscii getFavoriteId()
	{
		if (!isReady())
			return "";
		else
			return ToString()(_value->favoriteId());
	}
	void setFavoriteId(const tsAscii& setTo)
	{
		if (!isReady())
			return;
		_value->favoriteId(ToGuid()(setTo));
	}

	tsAscii getEnterpriseId()
	{
		if (!isReady())
			return "";
		else
			return ToString()(_value->enterpriseId());
	}
	void setEnterpriseId(const tsAscii& setTo)
	{
		if (!isReady())
			return;
		_value->enterpriseId(ToGuid()(setTo));
	}

	tsAscii getFavoriteName()
	{
		if (!isReady())
			return "";
		else
			return _value->favoriteName();
	}
	void setFavoriteName(const tsAscii& setTo)
	{
		if (!isReady())
			return;
		_value->favoriteName(setTo);
	}

	tsData getTokenSerialNumber()
	{
		if (!isReady())
			return "";
		else
			return _value->tokenSerialNumber();
	}
	void setTokenSerialNumber(const tsData& setTo)
	{
		if (!isReady())
			return;
		_value->tokenSerialNumber(setTo);
	}

	tsData headerData()
	{
		if (!isReady())
			return "";
		else
			return _value->headerData();
	}
	void headerData(const tsData& setTo)
	{
		if (!isReady())
			return;
		_value->headerData(setTo);
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
			Favorite* obj = new Favorite(/*value*/);
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
	static NAN_METHOD(Release)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		obj->release();
	}
	static NAN_GETTER(GetFavoriteId)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->getFavoriteId().c_str()).ToLocalChecked());
	}
	static NAN_SETTER(SetFavoriteId)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		auto id = Nan::To<v8::String>(value).ToLocalChecked();
		obj->setFavoriteId(*Nan::Utf8String(id));
	}
	static NAN_GETTER(GetEnterpriseId)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->getEnterpriseId().c_str()).ToLocalChecked());
	}
	static NAN_SETTER(SetEnterpriseId)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		auto id = Nan::To<v8::String>(value).ToLocalChecked();
		obj->setEnterpriseId(*Nan::Utf8String(id));
	}
	static NAN_GETTER(GetFavoriteName)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->getFavoriteName().c_str()).ToLocalChecked());
	}
	static NAN_SETTER(SetFavoriteName)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		auto name = Nan::To<v8::String>(value).ToLocalChecked();
		obj->setFavoriteName(*Nan::Utf8String(name));
	}
	static NAN_GETTER(GetTokenSerialNumber)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->getTokenSerialNumber().ToHexString().c_str()).ToLocalChecked());
	}
	static NAN_SETTER(SetTokenSerialNumber)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		auto serial = Nan::To<v8::String>(value).ToLocalChecked();
		obj->setTokenSerialNumber(tsAscii(*Nan::Utf8String(serial)).HexToData());
	}
	static NAN_GETTER(GetHeaderData)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->headerData().ToBase64().c_str()).ToLocalChecked());
	}
	static NAN_SETTER(SetHeaderData)
	{
		Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());
		auto data = Nan::To<v8::String>(value).ToLocalChecked();
		obj->headerData(tsAscii(*Nan::Utf8String(data)).HexToData());
	}
	static NAN_METHOD(Encrypt_File);
	static NAN_METHOD(EncryptData);
	static NAN_METHOD(EncryptString);

};

#endif // __FAVORITE_H__