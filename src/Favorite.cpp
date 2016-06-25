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

#include "Favorite.h"
#include "Session.h"
#include <node.h>

using namespace v8;
using namespace tscrypto;

Favorite::Favorite() {
}

Favorite::~Favorite() {
}

static tscrypto::tsCryptoString StringToTsAscii(v8::Local<v8::String>& string)
{
	tscrypto::tsCryptoString tmp;
	const int length = string->Utf8Length() + 1;  // Add one for trailing zero byte.
	tmp.resize(length);
	string->WriteOneByte((uint8_t*)tmp.rawData(), /* start */ 0, length);
	return tmp;
}

bool Favorite::encryptFile(Session* session, const tscrypto::tsCryptoString& sourceFile, bool compress, const tscrypto::tsCryptoString& encryptedFile)
{
	std::shared_ptr<IFileVEILOperations> fileOps;
	std::shared_ptr<ICmsHeader> header;
	std::shared_ptr<IFileVEILOperationStatus> status;
	tscrypto::tsCryptoString inputFile(sourceFile.c_str());
	tscrypto::tsCryptoString outputFile(encryptedFile.c_str());

	if (!isReady())
		throw tscrypto::tsCryptoString("Favorite not ready");

	if (!InitializeCmsHeader())
		return false;

	if (xp_GetFileAttributes(inputFile) == XP_INVALID_FILE_ATTRIBUTES || xp_IsDirectory(inputFile))
	{
		throw (tscrypto::tsCryptoString() << "File -> " << inputFile << " <- does not exist Encrypt operation aborted");
	}

	status = ::ServiceLocator()->Finish<IFileVEILOperationStatus>(new StatusClass());

	if (!(fileOps = CreateFileVEILOperationsObject()) ||
		!(fileOps->SetStatusInterface(status)) ||
		!(fileOps->SetSession(session->handle())))
	{
		throw tscrypto::tsCryptoString("An error occurred while building the file encryptor.  The CKM Runtime may be damaged.");
	}

	// Create output file name based on the input file name
	if (outputFile.size() == 0)
	{
		outputFile = inputFile;
		outputFile += ".ckm";
	}
	if (!(header = ::ServiceLocator()->get_instance<ICmsHeader>("/CmsHeader")) || !header->FromBytes(handle()->headerData()))
	{
		throw tscrypto::tsCryptoString("An error occurred while building the encryption header.");
	}

	// Indicate compression is desired.
	if (compress)
	{
		header->SetCompressionType(ct_zLib);
	}
	else
	{
		header->SetCompressionType(ct_None);
	}
	if (header->GetEncryptionAlgorithmID() == _TS_ALG_ID::TS_ALG_INVALID)
		header->SetEncryptionAlgorithmID(_TS_ALG_ID::TS_ALG_AES_GCM_256);

	if (!(fileOps->EncryptFileAndStreams(inputFile.c_str(), outputFile.c_str(), header, compress ? ct_zLib : ct_None,
		header->GetEncryptionAlgorithmID(), OIDtoID(header->GetDataHashOID().ToOIDString().c_str()),
		header->HasHeaderSigningPublicKey(), true,
		(Alg2Mode(header->GetEncryptionAlgorithmID()) == _SymmetricMode::CKM_SymMode_GCM ||
			Alg2Mode(header->GetEncryptionAlgorithmID()) == _SymmetricMode::CKM_SymMode_CCM) ?
		TS_FORMAT_CMS_ENC_AUTH : TS_FORMAT_CMS_CT_HASHED,
		false, header->GetPaddingType(), 5000000)))
	{
		throw tscrypto::tsCryptoString("The encryption failed.");
		return false;
	}

	return true;
}
tscrypto::tsCryptoData Favorite::encryptData(Session* session, const tscrypto::tsCryptoData& sourceData, bool compress)
{
	if (!isReady())
		throw tscrypto::tsCryptoString("Favorite not ready");

	tscrypto::tsCryptoData encData;

	if (sourceData.size() == 0)
	{
		return tscrypto::tsCryptoData();
	}

	if (!InitializeCmsHeader())
		return tscrypto::tsCryptoData();

	std::shared_ptr<IFileVEILOperations> fileOps;
	std::shared_ptr<IFileVEILOperationStatus> status;
	std::shared_ptr<ICmsHeader> header;

	if (!session->handle())
	{
		throw tscrypto::tsCryptoString("Session not ready");
	}

	status = ::ServiceLocator()->Finish<IFileVEILOperationStatus>(new StatusClass());

	if (!(fileOps = CreateFileVEILOperationsObject()) ||
		!(fileOps->SetStatusInterface(status)) ||
		!(fileOps->SetSession(session->handle())))
	{
		throw tscrypto::tsCryptoString("An error occurred while building the file encryptor.  The CKM Runtime may be damaged.");
	}
	if (!(header = ::ServiceLocator()->get_instance<ICmsHeader>("/CmsHeader")) || !header->FromBytes(handle()->headerData()))
	{
		throw tscrypto::tsCryptoString("An error occurred while building the encryption header.");
	}

	if (!header)
	{
		throw tscrypto::tsCryptoString("The favorite is invalid or incomplete.");
	}

	// Indicate compression is desired.
	if (compress)
	{
		header->SetCompressionType(ct_zLib);
	}
	else
	{
		header->SetCompressionType(ct_None);
	}
	if (header->GetEncryptionAlgorithmID() == _TS_ALG_ID::TS_ALG_INVALID)
		header->SetEncryptionAlgorithmID(_TS_ALG_ID::TS_ALG_AES_GCM_256);

	if (!(fileOps->EncryptCryptoData(sourceData, encData, header, compress ? ct_zLib : ct_None,
		header->GetEncryptionAlgorithmID(), OIDtoID(header->GetDataHashOID().ToOIDString().c_str()),
		header->HasHeaderSigningPublicKey(), true,
		(Alg2Mode(header->GetEncryptionAlgorithmID()) == _SymmetricMode::CKM_SymMode_GCM ||
			Alg2Mode(header->GetEncryptionAlgorithmID()) == _SymmetricMode::CKM_SymMode_CCM) ?
		TS_FORMAT_CMS_ENC_AUTH : TS_FORMAT_CMS_CT_HASHED,
		false, header->GetPaddingType(), 5000000)))
	{
		throw tscrypto::tsCryptoString("Encryption failed.");
	}

	return encData;
}

Nan::NAN_METHOD_RETURN_TYPE Favorite::Encrypt_File(Nan::NAN_METHOD_ARGS_TYPE info)
{
	Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());

	if (info.Length() < 4 || !info[0]->IsObject() || !info[1]->IsString() || !info[2]->IsBoolean() || !info[3]->IsString())
	{
		Nan::ThrowTypeError("Wrong number or type of arguments");
		return;
	}

	Session* session = ObjectWrap::Unwrap<Session>(info[0]->ToObject());
	auto sourceFile = Nan::To<v8::String>(info[1]).ToLocalChecked();
	auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
	auto destFile = Nan::To<v8::String>(info[3]).ToLocalChecked();
	try
	{
		bool retVal = obj->encryptFile(session, *Nan::Utf8String(sourceFile), compress->BooleanValue(), *Nan::Utf8String(destFile));
		info.GetReturnValue().Set(retVal);
	}
	catch (tscrypto::tsCryptoString& ex)
	{
		Nan::ThrowError(ex.c_str());
		return;
	}
}
Nan::NAN_METHOD_RETURN_TYPE Favorite::EncryptData(Nan::NAN_METHOD_ARGS_TYPE info)
{
	Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());

	if (info.Length() < 3 || !info[0]->IsObject() || (!info[1]->IsString() && !info[1]->IsObject()) || !info[2]->IsBoolean())
	{
		Nan::ThrowTypeError("Wrong number or type of arguments");
		return;
	}

	Session* session = ObjectWrap::Unwrap<Session>(info[0]->ToObject());
	try
	{
		if (info[1]->IsObject())
		{
			auto sourceData = Nan::To<v8::Object>(info[1]).ToLocalChecked();
			auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();

			char* content = node::Buffer::Data(sourceData);
			int contentlength = node::Buffer::Length(sourceData);

			tscrypto::tsCryptoData retVal = obj->encryptData(session, tscrypto::tsCryptoData((uint8_t*)content, contentlength), compress->BooleanValue());
			info.GetReturnValue().Set(Nan::CopyBuffer((char*)retVal.c_str(), retVal.size()).ToLocalChecked());
		}
		else
		{
			auto sourceData = Nan::To<v8::String>(info[1]).ToLocalChecked();
			auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
			tscrypto::tsCryptoData retVal = obj->encryptData(session, tscrypto::tsCryptoString(*Nan::Utf8String(sourceData)).Base64ToData(), compress->BooleanValue());
			info.GetReturnValue().Set(Nan::New(retVal.ToBase64().c_str()).ToLocalChecked());
		}
	}
	catch (tscrypto::tsCryptoString& ex)
	{
		Nan::ThrowError(ex.c_str());
		return;
	}
}
Nan::NAN_METHOD_RETURN_TYPE Favorite::EncryptString(Nan::NAN_METHOD_ARGS_TYPE info)
{
	Favorite* obj = ObjectWrap::Unwrap<Favorite>(info.This());

	if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsString() || !info[2]->IsBoolean())
	{
		Nan::ThrowTypeError("Wrong number or type of arguments");
		return;
	}

	Session* session = ObjectWrap::Unwrap<Session>(info[0]->ToObject());
	auto sourceFile = Nan::To<v8::String>(info[1]).ToLocalChecked();
	auto compress = Nan::To<v8::Boolean>(info[2]).ToLocalChecked();
	try
	{
		tscrypto::tsCryptoData retVal = obj->encryptData(session, tscrypto::tsCryptoString(*Nan::Utf8String(sourceFile)).ToUTF8Data(), compress->BooleanValue());
		info.GetReturnValue().Set(Nan::New<v8::String>(retVal.ToBase64().c_str()).ToLocalChecked());
	}
	catch (tscrypto::tsCryptoString& ex)
	{
		Nan::ThrowError(ex.c_str());
		return;
	}
}
