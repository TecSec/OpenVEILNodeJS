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

#ifndef __OPENVEILADDON_H__
#define __OPENVEILADDON_H__

#include "OpenVEIL.h"
#include <node.h>
#include <node_object_wrap.h>

class OpenVEILAddon : public node::ObjectWrap
{
public:
	static void Init(v8::Handle<v8::Object> exports);

private:
	explicit OpenVEILAddon();
	~OpenVEILAddon();

	std::shared_ptr<IKeyVEILConnector> _value;
	int genericConnect(const tsAscii& url, const tsAscii& username, const tsAscii& password)
	{
		if (!isReady())
		{
			return connStatus_NoServer;
		}

		return _value->genericConnectToServer(url, username, password);
	}
	int kvConnect(const tsAscii& url, const tsAscii& username, const tsAscii& password)
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


  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GenericConnect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void KvConnect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void IsConnected(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SendJsonRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SendRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

};

#endif // __OPENVEILADDON_H__