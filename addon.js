var addon = require('bindings')('OpenVEIL');

var obj = new addon.OpenVEIL();

console.log( obj.isConnected() );
obj.disconnect();