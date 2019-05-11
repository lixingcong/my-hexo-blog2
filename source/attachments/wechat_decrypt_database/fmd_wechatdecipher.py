#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Time-stamp: < fmd_wechatdecipher.py 2016年03月27日 18:11:03 >
from os.path import isfile
from pysqlcipher import dbapi2 as sqlite
import hashlib
import sys
import re

# 指定文件名
ORIGINAL_FILE="EnMicroMsg.db"
DECRYPTED_FILE="EnMicroMsg-decrypted.db"
ENCRYPTED_FILE_NEW="EnMicroMsg-encrypted-NEW.db"

def decrypt( key ,operation):
	if operation==1:
		conn = sqlite.connect( ORIGINAL_FILE )
	else:
		conn = sqlite.connect( DECRYPTED_FILE )
	c = conn.cursor()		
	try:
		if operation==1:
			c.execute( "PRAGMA key = '" + key + "';" )
			c.execute( "PRAGMA cipher_use_hmac = OFF;" )
			c.execute( "PRAGMA cipher_page_size = 1024;" )
			c.execute( "PRAGMA kdf_iter = 4000;" )
			c.execute( "ATTACH DATABASE '"+DECRYPTED_FILE+"' AS wechatdecrypted KEY '';" )
			c.execute( "SELECT sqlcipher_export( 'wechatdecrypted' );" )
			c.execute( "DETACH DATABASE wechatdecrypted;" )
		else:
			c.execute( "ATTACH DATABASE '"+ENCRYPTED_FILE_NEW+"' AS wechatencrypted KEY '"+ key +"';" )
			c.execute( "PRAGMA wechatencrypted.cipher_use_hmac = OFF;" )
			c.execute( "PRAGMA wechatencrypted.cipher_page_size = 1024;" )
			c.execute( "PRAGMA wechatencrypted.kdf_iter = 4000;" )
			c.execute( "SELECT sqlcipher_export( 'wechatencrypted' );" )
			c.execute( "DETACH DATABASE wechatencrypted;" )
		c.close()
		status = 1
	except:
		c.close()
		status = 0
	return status
	
	
def generate_key():
	imei = input( "输入IMEI：\n" )
	uin = get_uin()
	print "IMEI: "+imei+" UIN: "+str( uin )
	key = hashlib.md5( str( imei ) + str( uin )).hexdigest()[ 0:7 ]
	print "key: "+key
	return key
	
def get_uin():
	f = open( 'system_config_prefs.xml', 'r' ).read()
	uin = re.findall( 'name="default_uin" value="([\-?[0-9]+)"', f )
	return uin[ 0 ] if uin else 0


def main():	
	if not ( isfile( ORIGINAL_FILE ) and isfile( "system_config_prefs.xml" )):
		print "'EnMicroMsg database file' or 'system_config_prefs.xml' not found!"
		sys.exit(1)
	print '-'*10
	key = generate_key()
	choose=raw_input("----------\nTo decrypt: 1, to encrypt: 0\nPlease choose an operation:")
	print '-'*10
	if choose == "1":
		status = decrypt( key,1 )
		if status == 1:
			print "解密成功：文件名："+DECRYPTED_FILE
		else:
			print "解密失败！！"
	else:
		status = decrypt( key,0 )
		if status == 1:
			print "加密成功：文件名："+ENCRYPTED_FILE_NEW
		else:
			print "加密失败"

if __name__ == '__main__':
	main()
