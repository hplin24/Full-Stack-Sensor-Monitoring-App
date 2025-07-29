#pragma once

#include <string>

bool QueryWMI(const std::wstring& wsQuery, const std::wstring& wsProperty, std::wstring& result);
