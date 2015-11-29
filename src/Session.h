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

#ifndef __SESSION_H__
#define __SESSION_H__

#include "OpenVEIL.h"
#include <nan.h>
#include "Favorite.h"

// Used in the file encrypt and decrypt routines
class StatusClass : public IFileVEILOperationStatus, public tsmod::IObject
{
public:
	StatusClass() {}
	virtual bool Status(const tsAscii& taskName, int taskNumber, int ofTaskCount, int taskPercentageDone)
	{
		//if (g_doStatus)
		//{
		//	ts_out << "Task " << taskNumber << " of " << ofTaskCount << " " << taskName << " " << taskPercentageDone << "%" << endl;
		//}
		return true;
	}
	virtual void    FailureReason(const tsAscii&failureText)
	{
		//ERROR(failureText);
	}

private:
	virtual ~StatusClass() {}
};


class Session : public Nan::ObjectWrap
{
public:
	static NAN_MODULE_INIT(Init)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("Session").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		Nan::SetPrototypeMethod(tpl, "release", Release);
		Nan::SetPrototypeMethod(tpl, "close", Close);
		Nan::SetPrototypeMethod(tpl, "login", Login);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("isLoggedIn").ToLocalChecked(), GetIsLoggedIn);
		Nan::SetPrototypeMethod(tpl, "logout", Logout);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("profile").ToLocalChecked(), GetProfile);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("isLocked").ToLocalChecked(), GetIsLocked);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("retriesLeft").ToLocalChecked(), GetRetriesLeft);
		Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("isValid").ToLocalChecked(), GetIsValid);
		Nan::SetPrototypeMethod(tpl, "duplicate", Duplicate);
		Nan::SetPrototypeMethod(tpl, "encryptFileUsingFavorite", EncryptFileUsingFavorite);
		Nan::SetPrototypeMethod(tpl, "decryptFile", Decrypt_File);
		Nan::SetPrototypeMethod(tpl, "encryptDataUsingFavorite", EncryptDataUsingFavorite);
		Nan::SetPrototypeMethod(tpl, "decryptData", DecryptData);
		Nan::SetPrototypeMethod(tpl, "encryptStringUsingFavorite", EncryptStringUsingFavorite);
		Nan::SetPrototypeMethod(tpl, "decryptString", DecryptString);

		constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
		target->Set(Nan::New("Session").ToLocalChecked(), tpl->GetFunction());
	}
	explicit Session(std::shared_ptr<IKeyVEILSession> val) : _value(val) {}

	std::shared_ptr<IKeyVEILSession> handle() { return _value; }
	void handle(std::shared_ptr<IKeyVEILSession> val) { _value = val; }

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}

private:
	explicit Session();
	~Session();

	std::shared_ptr<IKeyVEILSession> _value;

	void release()
	{
		_value.reset();
	}

	void close()
	{
		if (!isReady())
			return;
		_value->Close();
	}
	LoginStatus login(const tsAscii& pin)
	{
		if (!isReady())
			return loginStatus_NoServer;
		return _value->Login(pin);
	}
	bool isLoggedIn()
	{
		if (!isReady())
			return false;
		return _value->IsLoggedIn();
	}
	bool logout()
	{
		if (!isReady())
			return true;
		return _value->Logout();
	}
	//bool GenerateWorkingKey(Asn1::CTS::CkmCombineParameters& params, std::function<bool(Asn1::CTS::CkmCombineParameters&, tsData&)> headerCallback, tsData &WorkingKey);
	//bool RegenerateWorkingKey(Asn1::CTS::CkmCombineParameters& params, tsData &WorkingKey);
	tsAscii getProfile()
	{
		if (!isReady())
			return "";
		return _value->GetProfile()->toJSON().ToString();
	}
	bool isLocked()
	{
		if (!isReady())
			return false;
		return _value->IsLocked();
	}
	size_t retriesLeft()
	{
		if (!isReady())
			return 0;
		return _value->retriesLeft();
	}
	bool isValid()
	{
		if (!isReady())
			return false;
		return _value->IsValid();
	}
	Session* duplicate()
	{
		if (!isReady())
			return nullptr;


		return new Session(_value->Duplicate());
	}
	bool encryptFileUsingFavorite(Favorite* fav, const tsAscii& sourceFile, bool compress, const tsAscii& encryptedFile)
	{
		if (!isReady())
			return false;
		return fav->encryptFile(this, sourceFile, compress, encryptedFile);
	}
	bool decryptFile(const tsAscii& encryptedFile, const tsAscii& decryptedFile)
	{
		if (!isReady())
			throw tsAscii("Session not ready");

		if (!InitializeCmsHeader())
			return false;

		std::shared_ptr<IFileVEILOperations> fileOps;
		std::shared_ptr<IFileVEILOperationStatus> status;

		if (xp_GetFileAttributes(encryptedFile) == XP_INVALID_FILE_ATTRIBUTES || xp_IsDirectory(encryptedFile))
		{
			throw (tsAscii() << "File -> " << encryptedFile << " <- does not exist Decrypt operation aborted");
		}

		status = ::ServiceLocator()->Finish<IFileVEILOperationStatus>(new StatusClass());

		if (!(fileOps = CreateFileVEILOperationsObject()) ||
			!(fileOps->SetStatusInterface(status)) ||
			!(fileOps->SetSession(handle())))
		{
			throw tsAscii("An error occurred while building the file decryptor.  The OpenVEIL installation may be damaged.");
		}

		if (!fileOps->DecryptFileAndStreams(encryptedFile, decryptedFile))
		{
			throw tsAscii("Decrypt failed.");
		}

		return true;
	}
	tsData encryptDataUsingFavorite(Favorite* fav, const tsData& sourceData, bool compress)
	{
		if (!isReady())
			throw tsAscii("Session not ready");
		return fav->encryptData(this, sourceData, compress);
	}
	tsData decryptData(const tsData& encryptedData)
	{
		if (!isReady())
			throw tsAscii("Session not ready");

		if (!InitializeCmsHeader())
			return tsData();

		std::shared_ptr<IFileVEILOperations> fileOps;
		std::shared_ptr<IFileVEILOperationStatus> status;
		tsData destData;

		status = ::ServiceLocator()->Finish<IFileVEILOperationStatus>(new StatusClass());

		if (!(fileOps = CreateFileVEILOperationsObject()) ||
			!(fileOps->SetStatusInterface(status)) ||
			!(fileOps->SetSession(handle())))
		{
			throw tsAscii("An error occurred while building the file decryptor.  The OpenVEIL installation may be damaged.");
		}

		if (!fileOps->DecryptCryptoData(encryptedData, destData))
		{
			throw tsAscii("Decrypt failed.");
		}

		return destData;
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
			Session* obj = new Session(/*value*/);
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
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		obj->release();
	}
	static NAN_METHOD(Close)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		obj->close();
	}
	static NAN_METHOD(Login)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}

		auto pin = Nan::To<v8::String>(info[0]).ToLocalChecked();
		switch (obj->login(*Nan::Utf8String(pin)))
		{
		case loginStatus_Connected:
			info.GetReturnValue().Set(Nan::New("Connected").ToLocalChecked());
			break;
		case loginStatus_NoServer:
			info.GetReturnValue().Set(Nan::New("NoServer").ToLocalChecked());
			break;
		case loginStatus_BadAuth:
			info.GetReturnValue().Set(Nan::New("BadAuth").ToLocalChecked());
			break;
		default:
			info.GetReturnValue().Set(Nan::New("unknown").ToLocalChecked());
			break;
		}
	}
	static NAN_GETTER(GetIsLoggedIn)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(obj->isLoggedIn());
	}
	static NAN_METHOD(Logout)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(obj->logout());
	}
	static NAN_GETTER(GetProfile)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(Nan::New<v8::String>(obj->getProfile().c_str()).ToLocalChecked());
	}
	static NAN_GETTER(GetIsLocked)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(obj->isLocked());
	}
	static NAN_GETTER(GetRetriesLeft)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(obj->retriesLeft());
	}
	static NAN_GETTER(GetIsValid)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());
		info.GetReturnValue().Set(obj->isValid());
	}
	static NAN_METHOD(Duplicate);
	static NAN_METHOD(EncryptFileUsingFavorite)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 4 || !info[0]->IsObject() || !info[1]->IsString() || !info[2]->IsBoolean() || !info[3]->IsString())
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}

		Favorite* fav = ObjectWrap::Unwrap<Favorite>(info[0]->ToObject());
		auto sourceFile = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
		auto destFile = Nan::To<v8::String>(info[3]).ToLocalChecked();
		try
		{
			bool retVal = fav->encryptFile(obj, *Nan::Utf8String(sourceFile), compress->BooleanValue(), *Nan::Utf8String(destFile));
			info.GetReturnValue().Set(retVal);
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
	static NAN_METHOD(Decrypt_File)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 2)
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}
		auto sourceFile = Nan::To<v8::String>(info[0]).ToLocalChecked();
		auto destFile = Nan::To<v8::String>(info[1]).ToLocalChecked();

		try
		{
			bool retVal = obj->decryptFile(*Nan::Utf8String(sourceFile), *Nan::Utf8String(destFile));
			info.GetReturnValue().Set(retVal);
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
	static NAN_METHOD(EncryptDataUsingFavorite)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 3 || !info[0]->IsObject() || (!info[1]->IsString() && !info[1]->IsObject()) || !info[2]->IsBoolean())
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}

		try
		{
			Favorite* fav = ObjectWrap::Unwrap<Favorite>(info[0]->ToObject());
			if (info[1]->IsObject())
			{
				auto sourceData = Nan::To<v8::Object>(info[1]).ToLocalChecked();
				auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();

				char* content = node::Buffer::Data(sourceData);
				int contentlength = node::Buffer::Length(sourceData);

				tsData retVal = fav->encryptData(obj, tsData((uint8_t*)content, contentlength), compress->BooleanValue());
				info.GetReturnValue().Set(Nan::CopyBuffer((char*)retVal.c_str(), retVal.size()).ToLocalChecked());
			}
			else
			{
				auto sourceData = Nan::To<v8::String>(info[1]).ToLocalChecked();
				auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
				tsData retVal = fav->encryptData(obj, tsAscii(*Nan::Utf8String(sourceData)).Base64ToData(), compress->BooleanValue());
				info.GetReturnValue().Set(Nan::New<v8::String>(retVal.ToBase64().c_str()).ToLocalChecked());
			}
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
	static NAN_METHOD(EncryptStringUsingFavorite)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsString() || !info[2]->IsBoolean())
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}

		Favorite* fav = ObjectWrap::Unwrap<Favorite>(info[0]->ToObject());
		auto sourceData = Nan::To<v8::String>(info[1]).ToLocalChecked();
		auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
		try
		{
			tsData retVal = fav->encryptData(obj, tsAscii(*Nan::Utf8String(sourceData)).ToUTF8Data(), compress->BooleanValue());
			info.GetReturnValue().Set(Nan::New<v8::String>(retVal.ToBase64().c_str()).ToLocalChecked());
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
	static NAN_METHOD(DecryptData)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}
		try
		{
			if (info[0]->IsObject())
			{
				auto sourceData = Nan::To<v8::Object>(info[0]).ToLocalChecked();

				char* content = node::Buffer::Data(sourceData);
				int contentlength = node::Buffer::Length(sourceData);

				tsData retVal = obj->decryptData(tsData((uint8_t*)content, contentlength));
				info.GetReturnValue().Set(Nan::CopyBuffer((char*)retVal.c_str(), retVal.size()).ToLocalChecked());
			}
			else
			{
				auto sourceData = Nan::To<v8::String>(info[0]).ToLocalChecked();

				tsData retVal = obj->decryptData(tsAscii(*Nan::Utf8String(sourceData)).Base64ToData());
				info.GetReturnValue().Set(Nan::New<v8::String>(retVal.ToBase64().c_str()).ToLocalChecked());
			}
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
	static NAN_METHOD(DecryptString)
	{
		Session* obj = ObjectWrap::Unwrap<Session>(info.This());

		if (info.Length() < 1)
		{
			Nan::ThrowTypeError("Wrong number or type of arguments");
			return;
		}
		auto sourceData = Nan::To<v8::String>(info[0]).ToLocalChecked();

		try
		{
			tsData retVal = obj->decryptData(tsAscii(*Nan::Utf8String(sourceData)).Base64ToData());
			info.GetReturnValue().Set(Nan::New<v8::String>(retVal.ToUtf8String().c_str()).ToLocalChecked());
		}
		catch (tsAscii& ex)
		{
			Nan::ThrowError(ex.c_str());
			return;
		}
	}
};


#endif // __SESSION_H__