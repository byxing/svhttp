======

svhttp��һ������libcurlʵ�ֵ�HTTP��.

-
##### ���

��֧��HTTP(1.0/1.1)��HTTPS���ϵ����������̲߳������أ��첽��HTTP/SOCKS4/SOCKS5����֧�ֵ����ԣ������߿������ɵĻ�������⿪���������Ӧ�á�

-

##### ��������

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
```
-

##### ʹ��request_header����HTTP����

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	
	svhttp::request_header opts;
	opts.insert(std::string("Accept: application/javascript, */*;q=0.8"));
	opts.insert(std::string("Connection: Keep-Alive"));
	_client.set_headers(opts);
	
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
``` 

-

##### ���ô���

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	// ���ô���
	_client.set_proxy(std::string("127.0.0.1:808"));
	
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
```

������������������svhttpԴ�롣

-


##### ��¡����

ʹ�� git ��ȡ svhttp ��ĿԴ����:

$ git clone git://github.com/byxing/svhttp
$ cd svhttp
��¡��ģ����Ŀ:

$ git submodule init
$ git submodule update
���� svhttp ��Ŀ����:

$ cd svhttp
$ git pull -f

-

##### svhttpʹ��

[VS2012 �����������]
svhttp������
C/C++ -> ���� -> ���Ӱ���Ŀ¼
../../external/libcurl/include;
../../svhttp;
������ -> ���� -> ���ӿ�Ŀ¼
../../external/libcurl/build;
������ -> ���� -> ����������
libcurl_imp.lib;


svhttp֧��iconv��������ã�
C/C++ -> ���� -> ���Ӱ���Ŀ¼
../../external/libiconv/include;
������ -> ���� -> ���ӿ�Ŀ¼
../../external/libiconv/build/vc11;
������ -> ���� -> ����������
libiconvd.lib;

��svhttp��Ҫʹ��iconv�����֧�֣�����github��ģ����Ŀ���Ұ���������iconv�����֧�֣��粻��Ҫ֧�������˲�����

-

