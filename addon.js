var addon = require('bindings')('OpenVEIL');

console.log(addon);
var obj = new addon.KeyVEILConnector();

console.log(obj);

console.log( obj.isConnected );
obj.disconnect();