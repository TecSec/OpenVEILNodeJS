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

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "OpenVEIL.h"
#include <nan.h>

class Token : public Nan::ObjectWrap
{
public:
	static NAN_MODULE_INIT(Init)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("Token").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		Nan::SetPrototypeMethod(tpl, "release", Release);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("tokenName").ToLocalChecked(), GetTokenName, SetTokenName);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("serialNumber").ToLocalChecked(), SerialNumber);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), Id);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("enterpriseName").ToLocalChecked(), EnterpriseName);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("memberName").ToLocalChecked(), MemberName);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("tokenType").ToLocalChecked(), TokenType);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("enterpriseId").ToLocalChecked(), EnterpriseId);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("memberId").ToLocalChecked(), MemberId);

		Nan::SetPrototypeMethod(tpl, "openSession", OpenSession);

		constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
		target->Set(Nan::New("Token").ToLocalChecked(), tpl->GetFunction());
	}

	explicit Token(std::shared_ptr<IToken> val) : _value(val) {}

	std::shared_ptr<IToken> handle() { return _value; }
	void handle(std::shared_ptr<IToken> val) { _value = val; }

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}

private:
	explicit Token();
	~Token();

	std::shared_ptr<IToken> _value;

	void release()
	{
		_value.reset();
	}

	tsAscii getTokenName()
	{
		if (!isReady())
			return "";
		return _value->tokenName();
	}
	void setTokenName(const tsAscii& setTo)
	{
		if (!isReady())
			return;
		_value->tokenName(setTo);
	}
	tsData serialNumber()
	{
		if (!isReady())
			return tsData();
		return _value->serialNumber();
	}
	tsAscii id()
	{
		if (!isReady())
			return "";
		return ToString()(_value->id());
	}
	tsAscii enterpriseName()
	{
		if (!isReady())
			return "";
		return _value->enterpriseName();
	}
	tsAscii memberName()
	{
		if (!isReady())
			return "";
		return _value->memberName();
	}
	tsAscii tokenType()
	{
		if (!isReady())
			return "";
		return _value->tokenType();
	}
	tsAscii enterpriseId()
	{
		if (!isReady())
			return "";
		return ToString()(_value->enterpriseId());
	}
	tsAscii memberId()
	{
		if (!isReady())
			return "";
		return ToString()(_value->memberId());
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
		  Token* obj = new Token(/*value*/);
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
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  obj->release();
  }

  static NAN_GETTER(GetTokenName)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->getTokenName().c_str()).ToLocalChecked());
  }
  static NAN_SETTER(SetTokenName)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  auto name = Nan::To<v8::String>(value).ToLocalChecked();
	  obj->setTokenName(*Nan::Utf8String(name));
  }
  static NAN_GETTER(SerialNumber)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->serialNumber().ToHexString().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(Id)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->id().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(EnterpriseName)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->enterpriseName().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(MemberName)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->memberName().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(TokenType)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->tokenType().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(EnterpriseId)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->enterpriseId().c_str()).ToLocalChecked());
  }
  static NAN_GETTER(MemberId)
  {
	  Token* obj = ObjectWrap::Unwrap<Token>(info.This());
	  info.GetReturnValue().Set(Nan::New(obj->memberId().c_str()).ToLocalChecked());
  }
  static NAN_METHOD(OpenSession);

};

#endif // __TOKEN_H__