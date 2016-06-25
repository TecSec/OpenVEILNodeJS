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

#include "KeyVEILConnector.h"
#include "Favorite.h"
#include "Token.h"

using namespace v8;
using namespace tscrypto;

static tscrypto::tsCryptoString StringToTsAscii(v8::Local<v8::String>& string)
{
	tscrypto::tsCryptoString tmp;
	const int length = string->Utf8Length() + 1;  // Add one for trailing zero byte.
	tmp.resize(length);
	string->WriteOneByte((uint8_t*)tmp.rawData(), /* start */ 0, length);
	return tmp;
}


KeyVEILConnector::KeyVEILConnector() {
	_value = ::ServiceLocator()->try_get_instance<IKeyVEILConnector>("/KeyVEILConnector");
}

KeyVEILConnector::~KeyVEILConnector() {
}

Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::TokenByIndex(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto index = Nan::To<v8::Int32>(info[0]).ToLocalChecked();

	std::shared_ptr<IToken> tokPtr = obj->_value->token(index->Int32Value());
	if (!tokPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Token::constructor());
	auto tokObj = cons->NewInstance();
	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);
	tok->handle(tokPtr);
	info.GetReturnValue().Set(tokObj);
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::TokenByName(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto name = Nan::To<v8::String>(info[0]).ToLocalChecked();

	std::shared_ptr<IToken> tokPtr = obj->_value->token(tscrypto::tsCryptoString(*Nan::Utf8String(name)));
	if (!tokPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Token::constructor());
	auto tokObj = cons->NewInstance();
	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);
	tok->handle(tokPtr);
	info.GetReturnValue().Set(tokObj);
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::TokenBySerialNumber(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto serial = Nan::To<v8::String>(info[0]).ToLocalChecked();

	std::shared_ptr<IToken> tokPtr = obj->_value->token(tscrypto::tsCryptoData(tscrypto::tsCryptoString(*Nan::Utf8String(serial)).HexToData()));
	if (!tokPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Token::constructor());
	auto tokObj = cons->NewInstance();
	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);
	tok->handle(tokPtr);
	info.GetReturnValue().Set(tokObj);
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::TokenById(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();

	std::shared_ptr<IToken> tokPtr = obj->_value->token(ToGuid()(tscrypto::tsCryptoString(*Nan::Utf8String(id))));
	if (!tokPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Token::constructor());
	auto tokObj = cons->NewInstance();
	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);
	tok->handle(tokPtr);
	info.GetReturnValue().Set(tokObj);
}

Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::FavoriteByIndex(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto index = Nan::To<v8::Int32>(info[0]).ToLocalChecked();

	std::shared_ptr<IFavorite> favPtr = obj->_value->favorite(index->Int32Value());
	if (!favPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Favorite::constructor());
	auto favObj = cons->NewInstance();
	Favorite* fav = ObjectWrap::Unwrap<Favorite>(favObj);
	fav->handle(favPtr);
	info.GetReturnValue().Set(favObj);
}

Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::FavoriteByName(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto name = Nan::To<v8::String>(info[0]).ToLocalChecked();

	std::shared_ptr<IFavorite> favPtr = obj->_value->favorite(tscrypto::tsCryptoString(*Nan::Utf8String(name)));
	if (!favPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Favorite::constructor());
	auto favObj = cons->NewInstance();
	Favorite* fav = ObjectWrap::Unwrap<Favorite>(favObj);
	fav->handle(favPtr);
	info.GetReturnValue().Set(favObj);
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::FavoriteById(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();

	std::shared_ptr<IFavorite> favPtr = obj->_value->favorite(ToGuid()(tscrypto::tsCryptoString(*Nan::Utf8String(id))));
	if (!favPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Favorite::constructor());
	auto favObj = cons->NewInstance();
	Favorite* fav = ObjectWrap::Unwrap<Favorite>(favObj);
	fav->handle(favPtr);
	info.GetReturnValue().Set(favObj);
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::TokenForEnterprise(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 2)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
	auto index = Nan::To<v8::Int32>(info[1]).ToLocalChecked();

	std::shared_ptr<IToken> tokPtr = obj->_value->tokenForEnterprise(ToGuid()(tscrypto::tsCryptoString(*Nan::Utf8String(id))), index->Int32Value());
	if (!tokPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Token::constructor());
	auto tokObj = cons->NewInstance();
	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);
	tok->handle(tokPtr);
	info.GetReturnValue().Set(tokObj);
}

Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::FavoriteForEnterprise(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 2)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
	auto index = Nan::To<v8::Int32>(info[1]).ToLocalChecked();

	std::shared_ptr<IFavorite> favPtr = obj->_value->favoriteForEnterprise(ToGuid()(tscrypto::tsCryptoString(*Nan::Utf8String(id))), index->Int32Value());
	if (!favPtr)
	{
		Nan::ThrowRangeError("Unable to retrieve that token");
		return;
	}

	auto cons = Nan::New<v8::Function>(Favorite::constructor());
	auto favObj = cons->NewInstance();
	Favorite* fav = ObjectWrap::Unwrap<Favorite>(favObj);
	fav->handle(favPtr);
	info.GetReturnValue().Set(favObj);
}

tscrypto::tsCryptoString KeyVEILConnector::createFavorite(Token* token, const tscrypto::tsCryptoData& headerData, const tscrypto::tsCryptoString& name)
{
	if (!isReady())
		return "";
	return ToString()(_value->CreateFavorite(token->handle(), headerData, name));
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::CreateFavorite_token(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsObject())
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto tokObj = Nan::To<v8::Object>(info[0]).ToLocalChecked();
	auto headerData = Nan::To<v8::Object>(info[1]).ToLocalChecked();
	auto name = Nan::To<v8::String>(info[2]).ToLocalChecked();

	Token* tok = ObjectWrap::Unwrap<Token>(tokObj);

	char* content = node::Buffer::Data(headerData);
	int contentlength = node::Buffer::Length(headerData);

	info.GetReturnValue().Set(Nan::New(obj->createFavorite(tok, tscrypto::tsCryptoData((uint8_t*)content, contentlength), *Nan::Utf8String(name)).c_str()).ToLocalChecked());
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::CreateFavorite_serial(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 3 || !info[1]->IsObject())
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto serial = Nan::To<v8::String>(info[0]).ToLocalChecked();
	auto headerData = Nan::To<v8::Object>(info[1]).ToLocalChecked();
	auto name = Nan::To<v8::String>(info[2]).ToLocalChecked();

	char* content = node::Buffer::Data(headerData);
	int contentlength = node::Buffer::Length(headerData);

	info.GetReturnValue().Set(Nan::New(obj->createFavorite(tscrypto::tsCryptoString(*Nan::Utf8String(serial)).HexToData(), tscrypto::tsCryptoData((uint8_t*)content, contentlength), *Nan::Utf8String(name)).c_str()).ToLocalChecked());
}
Nan::NAN_METHOD_RETURN_TYPE KeyVEILConnector::CreateFavorite_id(Nan::NAN_METHOD_ARGS_TYPE info)
{
	KeyVEILConnector* obj = ObjectWrap::Unwrap<KeyVEILConnector>(info.This());
	if (info.Length() < 3 || !info[1]->IsObject())
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}
	auto id = Nan::To<v8::String>(info[0]).ToLocalChecked();
	auto headerData = Nan::To<v8::Object>(info[1]).ToLocalChecked();
	auto name = Nan::To<v8::String>(info[2]).ToLocalChecked();

	char* content = node::Buffer::Data(headerData);
	int contentlength = node::Buffer::Length(headerData);

	info.GetReturnValue().Set(Nan::New(obj->createFavorite(tscrypto::tsCryptoString(*Nan::Utf8String(id)), tscrypto::tsCryptoData((uint8_t*)content, contentlength), *Nan::Utf8String(name)).c_str()).ToLocalChecked());
}
