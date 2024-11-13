// RemoveVsShell.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <iostream>

bool DeleteRegistryKeyRecursively(const HKEY hKeyRoot, const char* subKey)
{
	HKEY hKey;
	// 打开目标注册表项
	LONG result = RegOpenKeyExA(hKeyRoot, subKey, 0, KEY_READ | KEY_WRITE, &hKey);
	if (result != ERROR_SUCCESS)
	{
		std::cerr << "无法打开注册表项。错误代码：" << result << '\n';
		return false;
	}

	// 枚举并删除所有子项
	DWORD subKeyNameSize;
	while (true)
	{
		char subKeyName[256] = {};
		subKeyNameSize = sizeof(subKeyName);
		result = RegEnumKeyExA(hKey, 0, subKeyName, &subKeyNameSize, nullptr, nullptr, nullptr, nullptr);
		if (result == ERROR_NO_MORE_ITEMS)
		{
			break; // 没有更多子项，退出循环
		}
		if (result != ERROR_SUCCESS)
		{
			std::cerr << "枚举子项失败。错误代码：" << result << '\n';
			RegCloseKey(hKey);
			return false;
		}

		// 递归删除子项
		if (!DeleteRegistryKeyRecursively(hKey, subKeyName))
		{
			RegCloseKey(hKey);
			return false;
		}
	}

	// 关闭当前项并删除自己
	RegCloseKey(hKey);
	result = RegDeleteKeyA(hKeyRoot, subKey);
	if (result != ERROR_SUCCESS)
	{
		std::cerr << "删除注册表项失败。错误代码：" << result << '\n';
		return false;
	}
	return true;
}

int main()
{
	// 删除 Visual Studio 右键菜单项的路径
	const auto regPath = R"(Directory\Background\shell\AnyCode)";
	if (DeleteRegistryKeyRecursively(HKEY_CLASSES_ROOT, regPath))
	{
		std::cout << "成功移除右键菜单项 '使用 Visual Studio 打开'。" << '\n';
	}
	else
	{
		std::cout << "移除失败，可能未找到对应的注册表项或权限不足。" << '\n';
	}
	return 0;
}
