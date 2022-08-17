#pragma once
#ifndef MEMMANAGER_H
#define MEMMANAGER_H

class memManager
{
	public:
		memManager();
		memManager(LPCSTR name);
		~memManager();
		bool          attach();
		MODULEENTRY32 getModule(const std::string& moduleName = "GTA5.exe");
		bool          findWindow();
		HWND          getWindow();
		void          setWindowName(LPCSTR str);
		void          initPtr();

		template <typename rT> inline rT readMem(DWORD_PTR address)
		{
			rT out;
			ReadProcessMemory(m_hProc, (LPVOID) address, &out, sizeof(rT), nullptr);
			return out;
		}

		template <typename rT> inline void readMem(DWORD_PTR address, rT* output, DWORD size = NULL, DWORD prot = NULL)
		{
			size = (size == NULL) ? sizeof(rT) : size;
			ReadProcessMemory(m_hProc, (LPVOID) address, output, size, NULL);
			return;
		}

		template <typename wT> inline void writeMem(DWORD_PTR address, wT* value, DWORD size = NULL, DWORD prot = NULL)
		{
			size = (size == NULL) ? sizeof(wT) : size;
			WriteProcessMemory(m_hProc, (LPVOID) address, value, size, NULL);
			return;
		}

		template <typename wT> inline void writeMem(DWORD_PTR address, wT value, DWORD size = NULL, DWORD prot = NULL)
		{
			size = (size == NULL) ? sizeof(wT) : size;
			WriteProcessMemory(m_hProc, (LPVOID) address, &value, size, NULL);
			return;
		}

	protected:
		LPCSTR m_szWindowName;
		HWND   m_hWndTarget;
		HANDLE m_hProc = nullptr;
};

extern memManager* g_pMemMan;

#endif
