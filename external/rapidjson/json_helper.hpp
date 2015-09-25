#ifndef JSON_HELPER_H__
#define JSON_HELPER_H__

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <sstream>
#include <fstream>

namespace json
{
	/** json ʹ�÷���
	 *  ʹ�÷�����
		std::string json_str = "{\"data\":{\"oms\":{\"mid\":\"001\",\"code\":\"600446\",\"volume\":100,\"price\":10.01}},\"tgw\":{\"appid\":20}}";
		std::cout << json_str << std::endl;
		std::cout << "-------------------------------------" << std::endl;
		rapidjson::Document doc;
		json::JsonParse(doc, json_str);

		if (json::CheckMember(doc, "data", json::MemberType::kObject) &&
		json::CheckMember(doc["data"], "oms", json::MemberType::kObject))
		{
			std::cout << "object data: " << json::GetMemberValueString(doc["data"], "oms") << std::endl;

			rapidjson::Value& val = doc["data"]["oms"];

			if (json::CheckMember(val, "code", json::MemberType::kString))
			{
				std::cout << "code: " << json::GetMemberValueString(val, "code") << std::endl;
			}
			std::cout << "mid: "<< json::GetMemberValueString(val, "mid") << std::endl;
			std::cout << "volume: " << json::GetMemberValueInt(val, "volume") << std::endl;
			std::cout << "price: " << json::GetMemberValueDouble(val, "price") << std::endl;

			std::cout << "object tgw: " << json::GetMemberValueString(doc, "tgw") << std::endl;
		}
	 */

	enum MemberType
	{
		kObject, kString, kInt, kDouble, kArray, kBool, kNumber
	};
	/**
	 *	��Ա���ͼ��
	 *  if(CheckMember(doc, member, MemberType::kObject))
	 */  
	template<class T>
	inline bool CheckMember(T& doc, const std::string& member, const MemberType type = kObject)
	{
		rapidjson::Value::ConstMemberIterator iter = doc.FindMember(member.c_str());
		if (iter != doc.MemberEnd())
		{
			switch (type)
			{
			case MemberType::kObject:
				if (iter->value.IsObject())
				{
					return true;
				}
				break;
			case MemberType::kString:
				if (iter->value.IsString())
				{
					return true;
				}
				break;
			case MemberType::kInt:
				if (iter->value.IsInt())
				{
					return true;
				}
				break;
			case MemberType::kDouble:
				if (iter->value.IsDouble())
				{
					return true;
				}
				break;
			case MemberType::kArray:
				if (iter->value.IsArray())
				{
					return true;
				}
				break;
			case MemberType::kBool:
				if (iter->value.IsBool())
				{
					return true;
				}
				break;
			case MemberType::kNumber:
				if (iter->value.IsNumber())
				{
					return true;
				}
			default:
				break;
			}
		}
		return false;
	}
	// ��ȡjson��Ա��ֵ����std::base_string�������ͷ���
	inline std::string GetMemberValueString(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, MemberType::kString))
		{
			return doc[member.c_str()].GetString();
		}
		else if (CheckMember(doc, member, MemberType::kInt))
		{
			std::ostringstream oss;
			oss << doc[member.c_str()].GetInt();
			return oss.str();
		}
		else if (CheckMember(doc, member, MemberType::kDouble))
		{
			std::ostringstream oss;
			oss << doc[member.c_str()].GetDouble();
			return oss.str();
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? "true" : "false";
		}
		else if (CheckMember(doc, member, MemberType::kObject) || CheckMember(doc, member, MemberType::kArray))
		{
			rapidjson::Value& valx = doc[member.c_str()];
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			valx.Accept(writer);
			return buffer.GetString();
		}

		return "";
	}
	// ��ȡjson(int����)��Ա��ֵ����int�������ͷ���
	inline int GetMemberValueInt(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, MemberType::kInt))
		{
			return doc[member.c_str()].GetInt();
		}
		else if (CheckMember(doc, member, MemberType::kString))
		{
			std::string str = doc[member.c_str()].GetString();
			return atoi(str.c_str());
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? 1 : 0;
		}

		return 0;
	}
	// ��ȡjson(double����)��Ա��ֵ����double�������ͷ���
	inline double GetMemberValueDouble(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, kDouble))
		{
			return doc[member.c_str()].GetDouble();
		}
		else if (CheckMember(doc, member, kInt))
		{
			return doc[member.c_str()].GetDouble();
		}
		else if (CheckMember(doc, member, MemberType::kString))
		{
			std::string str = doc[member.c_str()].GetString();
			return atof(str.c_str());
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? 1 : 0;
		}

		return 0.0;
	}

	/**
	 *	JsonParse / ParseFromString
	 *	��json�ַ�������Ϊjson����
	 */
	inline bool ParseFromString(rapidjson::Document& doc, const std::string& json_str)
	{
		doc.SetObject();
		doc.Parse<0>(json_str.c_str());
		if (doc.HasParseError())
		{
			return false;
		}
		return true;
	};

	/**
	 *	��ȡ�ļ�
	 */
	inline std::string ReadFile(const std::string& file_path)
	{
		std::ostringstream oss;
		std::ifstream ifile_stream(file_path, std::ios::binary);
		if (ifile_stream)
		{
			oss << ifile_stream.rdbuf();
		}
		return (oss.str());
	}
	/**
	 *	��rapidjson�е�object�������Ϊstring�ַ���
	 */
	template<class T>
	inline std::string JsonToString(T& doc)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		return buffer.GetString();
	}
	/**
	 *	json�ļ��������
	 */
	inline int ParseFromFile(rapidjson::Document& doc, const std::string& file_path)
	{
		if (!json::ParseFromString(doc, ReadFile(file_path)))
		{
			return -1;
		}
		return 0;
	}

}	// namespace json.


#endif // JSON_HELPER_H__
