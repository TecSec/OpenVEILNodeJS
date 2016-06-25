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

#include <nan.h>
#include <node.h>
#include "KeyVEILConnector.h"
#include "GenericConnector.h"
#include "Favorite.h"
#include "Token.h"
#include "Session.h"

using namespace v8;
using namespace tscrypto;

// (function(exports) {
// 	function copyOwnFrom(target, source) {
// 		Object.getOwnPropertyNames(source).forEach(function(propName) {
// 			Object.defineProperty(target, propName,	Object.getOwnPropertyDescriptor(source, propName));
// 		});
// 		return target;
// 	}
// 	
// 	function Symbol(name, props) {
// 		this.name = name;
// 		if (props) {
// 			copyOwnFrom(this, props);
// 		}
// 		Object.freeze(this);
// 	}
//     /** We don’t want the mutable Object.prototype in the prototype chain */
//     Symbol.prototype = Object.create(null);
//     Symbol.prototype.constructor = Symbol;
//     /**
//       * Without Object.prototype in the prototype chain, we need toString()
//       * in order to display symbols.
//       */
//     Symbol.prototype.toString = function() {
// 		         return "|" + this.name + "|";
// 	};
//     Object.freeze(Symbol.prototype);
// 	
//     Enum = function(obj) {
// 		if (arguments.length == = 1 && obj != = null && typeof obj == = "object") {
// 			Object.keys(obj).forEach(function(name) {
// 				this[name] = new Symbol(name, obj[name]);
// 			}, this);
// 		}
// 		else {
// 			Array.prototype.forEach.call(arguments, function(name) {
// 				this[name] = new Symbol(name);
// 			}, this);
// 		}
// 		Object.freeze(this);
// 	}
// 	Enum.prototype.symbols = function() {
// 		return Object.keys(this).map(function(key) {
// 			return this[key];
// 		}, this);
// 	}
// 	Enum.prototype.contains = function(sym) {
// 		if (!sym instanceof Symbol) 
// 			return false;
// 		return this[sym.name] == = sym;
// 	}
//     exports.Enum = Enum;
//     exports.Symbol = Symbol;
// }(typeof exports === "undefined" ? this.enums = {} : exports));




NAN_MODULE_INIT(InitAll)
{
	//Local<Object> obj;
	//const char* k = "HEADERS_RECEIVED";
	//int v = 1;
	//obj->Set(v8::String::NewSymbol(k), v8::Int32::New(v), ReadOnly); // Use PropertyAttribute ReadOnly so that value won't be changed by javascript.

	KeyVEILConnector::Init(target);
	GenericConnector::Init(target);
	Favorite::Init(target);
	Token::Init(target);
	Session::Init(target);
	node::AtExit([](void*) {
		TerminateVEILSystem();
	});
}

NODE_MODULE(OpenVEIL, InitAll)
