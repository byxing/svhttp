#ifndef svhttp_client_ipp__
#define svhttp_client_ipp__

#include "http_client.hpp"
#include <iostream>
#include <stdio.h>

//#pragma comment(lib, "libcurl_imp.lib")

#ifndef nullptr
#define nullptr NULL
#endif
// 

namespace svhttp
{
	/**
	 *	http_global define & describe
	 */
	class http_global
	{
	protected:
		http_global(){ global_init(); };
		~http_global(){ global_clean(); };	
	public:
		template<class tmpl>
		friend tmpl& global_single();
		SVHTTP_DECL std::string get_version();
		
	private:
		SVHTTP_DECL bool global_init();
		SVHTTP_DECL void global_clean();
	};

	template<class tmpl>
	tmpl& global_single()
	{
		static tmpl tmpl_instance;
		return tmpl_instance;
	}

	bool http_global::global_init()
	{
		CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
		if (CURLE_OK != ret)
		{
			std::cerr << "init libcurl failed. " << std::endl;
			return false;
		}
		return true;
	};

	void http_global::global_clean()
	{ 
		curl_global_cleanup(); 
	}

	std::string http_global::get_version()
	{
		return curl_version();
	}


	/**
	 *	http_client describe
	 */
	size_t http_client::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		std::vector<char> *recv_buf = (std::vector<char>*)stream;
		size_t sizes = size * nmemb;

		// add data to the end of recv_buf
		// write data maybe called more than once in a single request
		recv_buf->insert(recv_buf->end(), (char*)ptr, (char*)ptr+sizes);

		return sizes;
	}

	// Callback function used by libcurl for collect header data
	size_t http_client::write_header_data(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		std::vector<char> *recv_buf = (std::vector<char>*)stream;
		size_t sizes = size * nmemb;

		// add data to the end of recv_buf
		// write data maybe called more than once in a single request
		recv_buf->insert(recv_buf->end(), (char*)ptr, (char*)ptr+sizes);

		return sizes;
	}

	http_client::http_client()
		:_headers(nullptr)
		,_curl(nullptr)
		,_timeout_connect(10*1000)
		,_timeout_read(30*1000)
	{
		global_single<http_global>();
		SVLOGGER_DBG << global_single<http_global>().get_version();

		_curl = curl_easy_init();
		if (nullptr == _curl)
		{
			SVLOGGER_ERR << "get a easy handle failed." ;
		}

		memset(_error_buffer, 0, sizeof(_error_buffer));
	}

	http_client::~http_client()
	{
		if (_curl)
		{
			curl_easy_cleanup(_curl);
			_curl = nullptr;
		}
		/* free the linked list for header data */
		 if (_headers)
		 {
			curl_slist_free_all(_headers);
			_headers = nullptr;
		 }
	}

	bool http_client::open(const std::string& url)
	{
		set_option(CURLOPT_URL, url.c_str());
 		
		if (!request_init())
		{
			return false;
		}

		CURLcode code = curl_easy_perform(_curl);

		// ÿ��ִ���굱������������������Ϊget<Ĭ��>
		// fixed bugs: post����󣬼����ø�curl�����������󶼱��post�ġ�
		set_option(CURLOPT_POST, 0);

		if (CURLE_OK != code)
		{
			return false;
		}

		return true;
	}

	bool http_client::request_init()
	{
		// ������������Ϣ����ѡ��
		if (!set_option(CURLOPT_ERRORBUFFER, _error_buffer))
		{
			return false;
		}
		// ��ӳ�ʱʱ������ѡ��
		if (!set_option(CURLOPT_TIMEOUT_MS, _timeout_read) )
		{
			return false;
		}
		if (!set_option(CURLOPT_CONNECTTIMEOUT_MS, _timeout_connect))
		{
			return false;
		}
		
		set_option(CURLOPT_NOSIGNAL, 1L);

		// set ssl ca
		if (_ssl_ca_file.empty()) 
		{
			set_option(CURLOPT_SSL_VERIFYPEER, 0L);
			set_option(CURLOPT_SSL_VERIFYHOST, 0L);
		} 
		else 
		{
			set_option(CURLOPT_SSL_VERIFYPEER, 1L);
			set_option(CURLOPT_SSL_VERIFYHOST, 2L);
			set_option(CURLOPT_CAINFO, _ssl_ca_file.c_str());
		}

		// set cookies.
		if (!_cookie_file.empty()) {
			//
			if (!set_option(CURLOPT_COOKIEFILE, _cookie_file.c_str())) 
			{
				return false;
			}
			//
			if (!set_option(CURLOPT_COOKIEJAR, _cookie_file.c_str())) 
			{
				return false;
			}
		}

		// ����֧��gzip
		if (_accept_encoding)
		{
			set_option(CURLOPT_ACCEPT_ENCODING, "");
		}

		// �����ʷ����
		_reponse_stream.clear();
		_reponse_header_stream.clear();

		return set_option(CURLOPT_WRITEFUNCTION, write_data)
			&& set_option(CURLOPT_WRITEDATA, &_reponse_stream)
			&& set_option(CURLOPT_HEADERFUNCTION, write_header_data)
			&& set_option(CURLOPT_HEADERDATA, &_reponse_header_stream);
	}

	// ����������Ϣͷ��Ϣ
	bool http_client::set_headers(request_header &request_opt)
	{
		// ���Ѿ����ڵ���Ϣͷȫ�������
		if (_headers)
        {
			curl_slist_free_all(_headers);
			_headers = nullptr;		// ****����
		}
		/**	
		 *	curl_slist_append��һ��������nullptr,��������һ���µ� _headers
		 *  _headers = curl_slist_append(nullptr, it->c_str());
		 */

		// get custom header data (if set)
		std::vector<std::string> headers=request_opt.option_all();
		if(!headers.empty())
		{
			// append custom headers one by one 
			for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
			{	
				_headers = curl_slist_append(_headers,it->c_str());
			}
			// set custom headers for curl 
			if (!set_option(CURLOPT_HTTPHEADER, _headers))
			{
				return false;	
			}
		}
		return true;
	}

	void http_client::set_cookies( const std::string& cookies_file )
	{
		_cookie_file = cookies_file;
	}

	void http_client::set_ssl_verification( const std::string& ssl_ca_file )
	{
		_ssl_ca_file = ssl_ca_file;
	}

	bool http_client::set_proxy( http_proxy &proxy )
	{
		set_option(CURLOPT_PROXYTYPE, proxy.get_proxy_type());
		if (!proxy.get_hostname().empty())
		{
			set_option(CURLOPT_PROXY, proxy.get_hostname().c_str()); 
			set_option(CURLOPT_PROXYPORT, proxy.get_port().c_str());
		}
		if (!proxy.get_username().empty() && !proxy.get_password().empty())
		{
			std::string str = proxy.get_username() + ":" + proxy.get_password();
			set_option(CURLOPT_PROXYUSERPWD, str.c_str());
		}

		return true;
	}

	bool http_client::set_proxy( std::string &single_proxy )
	{
		return set_option(CURLOPT_PROXY, single_proxy.c_str());
	}

	std::string http_client::get_version()
	{
		return global_single<http_global>().get_version();
	}

	bool http_client::set_post_fields( const std::string& post_str )
	{
		return set_option(CURLOPT_POST, 1)
			&& set_option(CURLOPT_POSTFIELDS, post_str.c_str())
			&& set_option(CURLOPT_POSTFIELDSIZE, post_str.length());
	}

}	//namespace svhttp


#endif	// svhttp_client_ipp__