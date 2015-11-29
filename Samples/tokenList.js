var addon = require('../OpenVEIL'),
    kvConn = new addon.KeyVEILConnector(),
    token,
    fav,
    session,
    inData,
    outData,
    newSrc,
    origBuff,
    encBuff,
    newBuff;

kvConn.connect('http://localhost:8125', 'user1', '11111111');

for (i = 0; i < kvConn.tokenCount; i++) {
    token = kvConn.tokenByIndex(i);
    console.log();
    console.log("Token");
    console.log("  Name:            " + token.tokenName);
    console.log("  Type:            " + token.tokenType);
    console.log("  serialNumber:    " + token.serialNumber);
    console.log("  id:              " + token.id);
    console.log("  Enterprise name: " + token.enterpriseName);
    console.log("  Enterprise ID:   " + token.enterpriseId);
    console.log("  Member Name:     " + token.memberName);
    console.log("  Member ID:       " + token.memberId);
}

for (i = 0; i < kvConn.favoriteCount; i++) {
    fav = kvConn.favoriteByIndex(i);
    console.log();
    console.log("Favorite");
    console.log("  Name:         " + fav.favoriteName);
    console.log("  ID:           " + fav.favoriteId);
    console.log("  Enterprise:   " + fav.enterpriseId);
    console.log("  Token Serial: " + fav.tokenSerialNumber);
}
	
session = kvConn.tokenBySerialNumber('906845AEC554109D').openSession();

console.log("SESSION");
console.log("Is Valid:      " + session.isValid);
console.log("Is logged in:  " + session.isLoggedIn);

if (!session.isLoggedIn) {
    console.log("  login returned:  " + session.login('11111111'));
    console.log("  Is logged in:  " + session.isLoggedIn);
}

// String encryption to base 64
inData = '1234';
console.log("Original data: " + inData);
outData = kvConn.favoriteByName("Staff").encryptString(session, inData, true)
console.log("Encrypted data: " + outData)
newSrc = session.decryptString(outData)
console.log("Decrypted data: " + newSrc)

// buffer encryption
origBuff = new Buffer([1, 2, 3, 4]);
console.log("Original buffer data: " + origBuff.toString('hex'));
encBuff = kvConn.favoriteByName("Staff").encryptData(session, origBuff, true)
console.log("Encrypted data: " + encBuff.toString('hex'))
newBuff = session.decryptData(encBuff)
console.log("Decrypted data: " + newBuff.toString('hex'))


//
// Now try file encryption using the same encryption information
//
console.log("File encrypt returned " + kvConn.favoriteByName("Staff").encryptFile(session, 'tokenList.js', true, 'tokenList.js.ckm'))
console.log("File decrypt returned " + session.decryptFile('tokenList.js.ckm', 'tokenList2.js'))
