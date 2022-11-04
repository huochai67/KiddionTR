#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "StringHelper.hpp"

namespace kiddiontr {
	class TranslateService
	{
	public:
		TranslateService(const wchar_t* str);
		~TranslateService();

		std::wstring_view tr(const wchar_t* str);
		std::wstring_view tr(const std::wstring_view& str);

		void load_tr_file();
	private:
		void add_tofile(const std::wstring_view& str);
		size_t hash(const std::wstring_view& str);

		std::hash<std::wstring_view> hasher;

		std::filesystem::path m_path;

		std::vector<std::string> m_buffer;

		std::unordered_map<size_t, std::wstring> m_map;
	};
}