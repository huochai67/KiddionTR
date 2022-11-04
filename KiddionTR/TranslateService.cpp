#include "TranslateService.hpp"

#include <Windows.h>
std::string ConvertWideToUtf8(const std::wstring& wstr)
{
	int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}

std::wstring ConvertUtf8ToWide(const std::string& str)
{
	int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
	return wstr;
}

kiddiontr::TranslateService::TranslateService(const wchar_t* str)
	:m_path(str)
{
	this->load_tr_file();
}

kiddiontr::TranslateService::~TranslateService()
{
	std::ofstream file(this->m_path, std::ios_base::app);

	if (!file.is_open()) {
		return;
	}

	for (auto& x : this->m_buffer)
		file << x;
	file.flush();
	file.close();
}

std::wstring_view kiddiontr::TranslateService::tr(const wchar_t* str)
{
	return this->tr(std::wstring_view{ str , wcslen(str) });
}

std::wstring_view kiddiontr::TranslateService::tr(const std::wstring_view& str)
{
	auto iter = this->m_map.find(this->hash(str));
	if (iter != this->m_map.end())
		return iter->second;
	else
	{
		this->m_map.insert({ this->hash(str), std::wstring(str)});
		this->add_tofile(str);
	}
	return str;
}

void kiddiontr::TranslateService::load_tr_file()
{
	std::fstream file(this->m_path, std::ios::ios_base::in);

	if (!file.is_open()) {
		return;
	}

	auto buffer = (char*)::malloc(0x100);

	while (file.getline(buffer, 0x100))
	{
		std::string view{ buffer, strlen(buffer)};
		auto index = view.find("====");
		if (index == std::wstring::npos)
			continue;
		auto t = std::string(buffer + index + 4, view.size() - index - 4);
		this->m_map.insert({this->hash(ConvertUtf8ToWide(view.substr(0, index))), ConvertUtf8ToWide( t)});
	}

	free(buffer);
	return;
}

void kiddiontr::TranslateService::add_tofile(const std::wstring_view& str)
{
	std::wstring tofile(str);
	tofile.append(L"====");
	tofile.append(str);
	tofile.append(L"\n");
	this->m_buffer.push_back(ConvertWideToUtf8(tofile));
}

size_t kiddiontr::TranslateService::hash(const std::wstring_view& str)
{
	return this->hasher(str);
}
