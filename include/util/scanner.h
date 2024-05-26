#pragma once

#include "Windows.h"
#include <vector>

class Handle
{
public:
	Handle() : m_ptr(nullptr) {}
	explicit Handle(std::uintptr_t ptr) : m_ptr(reinterpret_cast<void*>(ptr)) {}

	template <typename T>
	std::enable_if_t<std::is_pointer_v<T>, T> As();

	template <typename T>
	std::enable_if_t<std::is_lvalue_reference_v<T>, T> As();

	template <typename T>
	std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> As();

	template <typename T>
	Handle Add(T offset)
	{
		return Handle(As<std::uintptr_t>() + offset);
	}

	template <typename T>
	Handle Sub(T offset)
	{
		return Handle(As<std::uintptr_t>() - offset);
	}

	Handle Rip()
	{
		return Add(As<std::int32_t&>()).Add(4);
	}

private:
	void* m_ptr;
};

template <typename T>
inline std::enable_if_t<std::is_pointer_v<T>, T> Handle::As()
{
	return static_cast<T>(m_ptr);
}

template <typename T>
inline std::enable_if_t<std::is_lvalue_reference_v<T>, T> Handle::As()
{
	return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(m_ptr);
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> Handle::As()
{
	return reinterpret_cast<std::uintptr_t>(m_ptr);
}

class scanner
{
	uintptr_t module_address;
	static std::vector<int> ConvPatternToByte(const char* pattern);
public:
	Handle scan(const char* signature);

	scanner(const char* module_name);
	scanner(uintptr_t module_addr);
};