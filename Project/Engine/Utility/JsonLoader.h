#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Debug.h"
#include <string>
#include <vector>
#include <variant>
#include <optional>

#include "json.hpp"

using json = nlohmann::json;

class JsonLoader {
private:
	struct Datum
	{
		std::variant<uint32_t, float, Vector2, Vector3, Vector4, std::string> datum;
		Datum() = default;

		Datum(uint32_t _uint) { datum = _uint; };
		Datum(float _f) { datum = _f; };
		Datum(const Vector2& _vec2) { datum = _vec2; };
		Datum(const Vector3& _vec3) { datum = _vec3; };
		Datum(const Vector4& _vec4) { datum = _vec4; };
        Datum(const std::string& _str) { datum = _str; };
        Datum(const Datum& _datum) { datum = _datum.datum; };

	};
	struct Data
	{
		// 変数名,値
		std::unordered_map<std::string, std::vector<Datum>> data;
	};
	// obj名,Data

public:
	JsonLoader(bool _autoSave = true);
	JsonLoader(const std::string& _directory, bool _autoSave = true);
	~JsonLoader();

	void LoadJson(const std::string& _filepath, bool _isMakeFile = false);
	void SaveJson();
	void MakeJsonFile();

	void SaveJson(const std::string& _groupName);

	void SetData(const std::string& _groupname, const std::string& _name, const Datum& _data, bool _isOverride = true);
	template<typename T>
	void SetData(const std::string& _groupname, const std::string& _name, const std::vector<T>& _data);

	Datum parseDatum(const json& j, const std::string& _groupName);
	json DatumToJson(const Datum& _datum);


	std::optional<Data> GetData(const std::string& _groupName);
	template<typename T>
	inline std::optional<T> GetDatum(const std::string& _groupName, const std::string& _variableName);
	template<typename T>
	inline std::optional<std::vector <T>>  GetDatumArray(std::string _groupName, std::string _variableName);

	void PrepareForSave();

private:
	json jsonData_;
	std::string folderPath_;
	std::string filePath_;
	bool autoSave_;

	std::unordered_map < std::string, Data > dataGroup_;

};


template<typename T>
inline void JsonLoader::SetData(const std::string& _groupname, const std::string& _name, const std::vector<T>& _data)
{
	dataGroup_[_groupname].data[_name].clear();

	Data& groupData = dataGroup_[_groupname];

	for (const auto& value : _data) {
		groupData.data[_name].emplace_back(value);
	}
}

template<typename T>
inline std::optional<T> JsonLoader::GetDatum(const std::string& _groupName, const std::string& _variableName)
{
	//objのデータを取得
	auto groupIt = dataGroup_.find(_groupName);
	if (groupIt != dataGroup_.end())
	{
		// 取得したデータの変数を取得
		auto it = groupIt->second.data.find(_variableName);
		if (it == groupIt->second.data.end())
			assert(false && "not found variaber");   // 見つからなければ止める

		const Datum& datum = it->second.front();

		if constexpr (std::is_same_v<T, uint32_t>)
		{
			if (auto value = std::get_if<uint32_t>(&datum.datum))
				return *value;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			if (auto value = std::get_if<float>(&datum.datum))
				return *value;
		}
		else if constexpr (std::is_same_v<T, Vector2>)
		{
			if (auto value = std::get_if<Vector2>(&datum.datum))
				return *value;
		}
		else if constexpr (std::is_same_v<T, Vector3>)
		{
			if (auto value = std::get_if<Vector3>(&datum.datum))
				return *value;
		}
		else if constexpr (std::is_same_v<T, Vector4>)
		{
			if (auto value = std::get_if<Vector4>(&datum.datum))
				return *value;
		}
	}
	Utils::Log("not found " + _variableName + "in" + _groupName);
	//assert(false && "not found variaber");   // 見つからなければ止める
	return T();
}

template<typename T>
inline std::optional<std::vector <T>> JsonLoader::GetDatumArray(std::string _groupName, std::string _variableName)
{
	//objのデータを取得
	auto groupIt = dataGroup_.find(_groupName);
	if (groupIt != dataGroup_.end())
	{
		// 取得したデータの変数を取得
		auto it = groupIt->second.data.find(_variableName);
		if (it == groupIt->second.data.end())
		{
			Utils::Log("not found " + _variableName + "in" + _groupName);
			return std::vector<T>();
			//assert(false && "not found variaber");   // 見つからなければ止める
		}

		std::vector<T> result;
		for( const Datum& datum : it->second)
		{
			if (std::holds_alternative<T>(datum.datum))
				result.push_back(std::get<T>(datum.datum));
		}
		return result;
	}
	Utils::Log("not found Group"  + _groupName);
	//assert(false && "not found variaber");   // 見つからなければ止める
	return std::vector<T>();
}