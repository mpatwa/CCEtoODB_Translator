===========================================
Instructions for generating AES 128-bit key
===========================================

1. Open Command Prompt

2. SET OPENSSL_CONF=%SHARED_OBJ_DIR%\infra_oss\%SHARED_OBJ_TAG%\ssl\openssl.cnf

3. cd C:\Temp [ your favorite working directory ]

4. %SHARED_OBJ_DIR%\infra_oss\%SHARED_OBJ_TAG%\bin\openssl rand -out "cczkey1.bin" 16
This generates the .bin file which is the binary key. This should be put into source control. The 16 at the end is number of bytes for key length. If you need 256-bit length then change it to 32 and so on...

5. Create an XML configuration file for KeysPack utility - sample below,
<?xml version="1.0"?>
<KeysPackData>
  <reference uri="Mentor Graphics KeysPack Application"/>
  <VarName>key_data</VarName>
  <KeyData>
    <name>CczKey1</name>
    <format>binary</format>
    <ValueFile>cczkey1.bin</ValueFile>
  </KeyData>
</KeysPackData>

5. Name the XML configuration file as "KeysPack.xml" and place it in the current directory

6. %SHARED_OBJ_DIR%\infra_sec\%SHARED_OBJ_TAG%\bin\KeysPack.exe
This generates the header file "EncryptedKeyData.h"

7. If you want to add a new key to an existing one - generate a new key by following steps 1-4. Modify the KeysPack.xml configuration and add a new KeyData node with <name> and <ValueFile> pointing to the new bin file you generated now. Run KeysPack.exe on this configuration would generate a "EncryptedKeyData.h" which contains the old and new keys.