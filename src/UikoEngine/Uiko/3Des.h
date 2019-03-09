//////////////////////////////////////////////////////////////////////////
/*
    
*/
//////////////////////////////////////////////////////////////////////////

enum    {ENCRYPT,DECRYPT};
//enum    bool{false,true}; // if bool is not supported,use this or just replace with char
                            // and use 1 for true,0 for false;
//////////////////////////////////////////////////////////////////////////

// Type—ENCRYPT:加密,DECRYPT:解密
// 输出缓冲区(Out)的长度 >= ((datalen+7)/8)*8,即比datalen大的且是8的倍数的最小正整数
// In 可以= Out,此时加/解密后将覆盖输入缓冲区(In)的内容
// 当keylen>8时系统自动使用3次DES加/解密,否则使用标准DES加/解密.超过16字节后只取前16字节

/**************************************************************************
*  函数原型 :	Des_Go(char *Out,char *In,long datalen,const char *Key,int keylen,bool Type = ENCRYPT)
*  作    者 :	LIUBC
*  说    明 :	
*  输入参数 :	char *Out		        --- 输出缓冲区的长度
				char *In	            --- 输入缓冲区(In)的内容
				long datalen	        --- (datalen+7)/8)*8
                const char *Key         ---
                int keylen              --- keylen>8时系统自动使用3次DES加/解密
                bool Type = ENCRYPT     --- 
*  输出参数 :	
*  返 回 值 :		                    --- 
*  使用范例 :
					
* -----------------------------------------------------------------*/

bool Des_Go(char *Out,char *In,long datalen,const char *Key,int keylen,bool Type = ENCRYPT);

//////////////////////////////////////////////////////////////////////////

