#pragma once
#include<vector>
static auto vector_getter = [](void* vec, int idx, const char** outText)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*outText = vector.at(idx).c_str();
	return true;
};
