#!/usr/bin/env python
#-*- coding: utf-8 -*-

try:
    import hashlib
    from Crypto.Cipher import DES3
except:
    print "Can't Load Module 'Crypto'. Please check you have installed one."
try:
    import pickle
except:
    print "Can't Load Module 'pickle'. Please check you have installed one."

class Coder:
    """Module encryption by method RC5. To encrypt you need to create a hash key
       by using function GetHash, then create an object RC5 by using
       GetCryptObject function by using a hash key as agrument or pass a key
       explicitly, then encrypt the string by using function EncodeIt, then you
       can save the encrypted data by using function SendFile. Decrypting can be
       implemented in two ways: reading encrypted data from file by using
       function GetFile, get formatted data by using function CheckData, then
       create an object RC5 by using function GetCryptObject, and then decrypt
       the data by using functionDecodeIt ; or Decrypt encrypted data that only
       knowing the encryption key, creating an object RC5 by using GetCryptObject
       function passing the key as an argument"""
    def __init__(self):
        self.one_char = False

    def GetFile(self, name):
        """Reads the encrypted key file.
            GetFile(NameOfFile)"""
        f = open(name, "rb")
        f.seek(0)
        result = pickle.load(f)
        f.close()
        return result

    def SendFile(self, name, EncMessage, HashKey):
        """Saves the encrypted key file.
            SendFile(NameOfFile: String, Data)"""
        data = self.SetData(EncMessage, HashKey)
        f = open(name, "wb")
        f.seek(0)
        p = pickle.Pickler(f)
        p.dump(data)
        f.close

    def CheckData(self, data):
        """Cuts encrypted keyfile to needed parts.
            CheckData(EncryptedFileData)"""
        st = []
        length = 16
        srez_data = data[0]
        srez = srez_data[-length:]
        ostatok = srez_data[:-length]
        st.append(ostatok)
        st.append(srez)
        return st

    def GetCryptObject(self, data):
        """Creates a RC5 object, with key as data.
            GetCryptObject(KEY)"""
        key = self.GetHash(data)
        crypt_obj = DES3.new(key, DES3.MODE_ECB)
        return crypt_obj

    def GetHash(self, data):
        """Returns MD5 Hash for data. GetHash(MessageToEncrypt)"""
        if data != None:
            Hash = hashlib.new("md5", data).digest()
        return Hash

    def DecodeIt(self, cr_ob, data):
        """Decodes an ancrypted message,
            DecodeIt(RC5_Object, EncryptedData)"""
        if data != None:
            result = cr_ob.decrypt(data)
            exit_result = self.GetData(result)
            return exit_result

    def EncodeIt(self, cr_ob, data):
        """Encodes an encrypted message.
            EncodeIt(RC5_Object, MessageToEncrypt)"""
        if data != None:
            exit_data = self.MultiplyData(data)
            result = cr_ob.encrypt(exit_data)
            return result

    def SetData(self, encoded, hashkey):
        """Makes a Data for saving"""
        st = []
        key = encoded + hashkey
        st.append(key)
        return st

    def GetData(self, data):
        if not self.one_char:
            if len(data)/8 != 1:
                inc = len(data)/8
                result = data[:inc]
            else:
                result = data
        else:
            result = data[:1]
            self.one_char = False
        return result

    def MultiplyData(self, data):
        if len(data) > 1:
            if len(data)%8 != 0:
                result = data*8
            else:
                result = data
        elif len(data)==1:
            self.one_char = True
            result = data*8
        return result
