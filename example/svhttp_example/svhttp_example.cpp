// svhttp_example.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
#include <iostream>

#define ENABLE_LIBICONV
#include "ip138.hpp"
#include "joker.hpp"

int main(int argc, char** argv)
{
	SVLOGGER_INIT_PATH(".");
	SVLOGGER_DBG << ip138().get_reponse_str();

	joker joker_;
	SVLOGGER_DBG << joker_.get_random_article_content();
	SVLOGGER_WARN << "press enter to continue...";
	while (getchar())
	{
		SVLOGGER_DBG << joker_.get_next_article_content();
		SVLOGGER_WARN << "press enter to continue...";
	}

	return 0;
}

