﻿#include "stdafx.h"

memManager::memManager()
{
}

memManager::memManager(LPCSTR name)
{
	m_szWindowName = name;
}

memManager::~memManager()
{
	CloseHandle(m_hProc);
}

bool memManager::attach()
{
	if (m_hProc != nullptr)
		return 1;

	if (!this->findWindow())
		return 0;

	DWORD dwProcId;
	GetWindowThreadProcessId(m_hWndTarget, &dwProcId);
	if (!dwProcId)
		return 0;

	auto a  = PROCESS_ALL_ACCESS;
	m_hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 0, dwProcId);
	if (m_hProc == nullptr)
		return 0;
	initPtr();

	return 1;
}

MODULEENTRY32 memManager::getModule(const std::string& moduleName)
{
	DWORD dwProcId;
	GetWindowThreadProcessId(m_hWndTarget, &dwProcId);

	HANDLE        hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcId);
	MODULEENTRY32 mEntry;

	if (hModuleSnapshot == INVALID_HANDLE_VALUE)
		return MODULEENTRY32();

	mEntry.dwSize = sizeof(mEntry);

	if (!Module32First(hModuleSnapshot, &mEntry))
		CloseHandle(hModuleSnapshot);

	do
	{
		if (std::string(mEntry.szModule) == moduleName)
			return mEntry;
	}
	while (Module32Next(hModuleSnapshot, &mEntry));

	CloseHandle(hModuleSnapshot);
	return MODULEENTRY32();
}

bool memManager::findWindow()
{
	m_hWndTarget = FindWindow(nullptr, m_szWindowName);
	if (!m_hWndTarget)
		return 0;
	return 1;
}

HWND memManager::getWindow()
{
	return m_hWndTarget;
}

void memManager::setWindowName(LPCSTR str)
{
	m_szWindowName = str;
}

void memManager::initPtr()
{
	auto base = getModule();

	auto rip = [&](uintptr_t offset)
	{
		return readMem <int>((uintptr_t) base.modBaseAddr + offset + 3) + 7 + offset;
	};

	patternBatch patternMain(base);

	patternMain.add
		("World", "48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07", [&](uintptr_t offset)
		{
			ADDRESS_WORLD = rip(offset);
		});

	patternMain.add
		("Blip", "4C 8D 05 ? ? ? ? 0F B7 C1", [&](uintptr_t offset)
		{
			ADDRESS_BLIP = rip(offset);
		});

	patternMain.add
		("Ammo dec code", "41 2B D1 E8", [](uintptr_t offset)
		{
			ADDRESS_AMMO = offset;
		});

	patternMain.add
		("Magazine dec code", "41 2B C9 3B C8 0F", [](uintptr_t offset)
		{
			ADDRESS_MAGAZINE = offset;
		});

	patternMain.add
		("Aiming ped", "48 8B 0D ? ? ? ? 48 85 C9 74 0C 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 89 1D ? ? ? ?", [&](uintptr_t offset)
		{
			ADDRESS_AIMING_PED = rip(offset);
		});

	patternMain.add
		("Script Global", "4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11", [&](uintptr_t offset)
		{
			ADDRESS_GLOBAL = rip(offset);
		});

	patternMain.add
		("Player list", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8B CF", [&](uintptr_t offset)
		{
			ADDRESS_PLAYER_LIST = rip(offset);
		});

	patternMain.add
		("Replay interface", "48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8 ? ? ? ? 84 DB 75 13 48 8D 0D ? ? ? ?", [&](uintptr_t offset)
		{
			ADDRESS_REPLAY_INTERFACE = rip(offset);
		});

	patternMain.add
		("Unk model", "4C 8B 15 ? ? ? ? 49 8B 04 D2 44 39 40 08", [&](uintptr_t offset)
		{
			ADDRESS_UNK_MODEL = rip(offset);
		});

	patternMain.add
		("Frame flags zero writer dec code", "48 81 C1 ? ? ? ? E8 ? ? ? ? 48 8B 5C 24 ? 0F 28 74 24 ?", [](uintptr_t offset)
		{
			ADDRESS_FRAME_FLAGS = offset + 0x7;
		});

	patternMain.run();
}
