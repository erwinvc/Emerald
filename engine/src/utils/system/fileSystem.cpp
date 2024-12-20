#include "eepch.h"
#include "fileSystem.h"
#include <format>
#include <shobjidl.h>  
#include <shlobj.h> 
#include <shellapi.h>

namespace emerald {
	std::filesystem::path FileSystem::openFileDialog(const std::vector<FilterSpec>& filters) {
		std::filesystem::path filePath;

		// Initialize COM library
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr)) {
			IFileDialog* pFileDialog = NULL;

			// Create the file open dialog object
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
			if (SUCCEEDED(hr)) {
				// Set the file type filter
				std::vector<COMDLG_FILTERSPEC> fileTypes;
				for (const auto& filter : filters) {
					fileTypes.push_back({ filter.name, filter.spec });
				}
				pFileDialog->SetFileTypes(static_cast<UINT>(fileTypes.size()), fileTypes.data());

				// Show the dialog
				hr = pFileDialog->Show(NULL);
				if (SUCCEEDED(hr)) {
					// Get the file the user selected
					IShellItem* pItem;
					hr = pFileDialog->GetResult(&pItem);
					if (SUCCEEDED(hr)) {
						// Convert the file path to a std::filesystem::path
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr)) {
							filePath = std::filesystem::path(pszFilePath);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileDialog->Release();
			}
			CoUninitialize();
		}
		return filePath;
	}

	std::filesystem::path FileSystem::saveFileDialog(const std::vector<FilterSpec>& filters) {
		std::filesystem::path filePath;

		// Initialize COM library
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr)) {
			IFileDialog* pFileDialog = NULL;

			// Create the file save dialog object
			hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
			if (SUCCEEDED(hr)) {
				// Set the file type filter
				std::vector<COMDLG_FILTERSPEC> fileTypes;
				for (const auto& filter : filters) {
					fileTypes.push_back({ filter.name, filter.spec });
				}
				pFileDialog->SetFileTypes(static_cast<UINT>(fileTypes.size()), fileTypes.data());

				// Show the dialog
				hr = pFileDialog->Show(NULL);
				if (SUCCEEDED(hr)) {
					// Get the file path the user selected or typed
					IShellItem* pItem;
					hr = pFileDialog->GetResult(&pItem);
					if (SUCCEEDED(hr)) {
						// Convert the file path to a std::filesystem::path
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr)) {
							filePath = std::filesystem::path(pszFilePath);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileDialog->Release();
			}
			CoUninitialize();
		}
		return filePath;
	}

	std::filesystem::path FileSystem::openFolderDialog(LPCWSTR title) {
			std::filesystem::path folderPath;

			// Initialize COM library
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileDialog* pFileDialog = NULL;

				// Create the file open dialog object
				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
				if (SUCCEEDED(hr)) {
					// Set the dialog to pick folders
					DWORD dwOptions;
					pFileDialog->GetOptions(&dwOptions);
					pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
					pFileDialog->SetTitle(title);
					// Show the dialog
					hr = pFileDialog->Show(NULL);
					if (SUCCEEDED(hr)) {
						// Get the folder the user selected
						IShellItem* pItem;
						hr = pFileDialog->GetResult(&pItem);
						if (SUCCEEDED(hr)) {
							// Convert the folder path to a string
							PWSTR pszFolderPath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
							if (SUCCEEDED(hr)) {
								folderPath = std::filesystem::path(pszFolderPath);
								Log::info("{}", folderPath.string().c_str());
								CoTaskMemFree(pszFolderPath);
							}
							pItem->Release();
						}
					}
					pFileDialog->Release();
				}
				CoUninitialize();
			}
			return folderPath;
		}

	std::string FileSystem::readFile(const std::string& path) {
		std::ifstream stream(path);
		std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();
		return str;
	}

	bool FileSystem::doesFileExist(const std::filesystem::path& path) {
		auto absolutePath = std::filesystem::absolute(path);
		return std::filesystem::exists(absolutePath);
	}

	void FileSystem::saveJsonToFile(const nlohmann::json& jsonOb, const std::string& name) {
		std::string file = std::format("{}.json", name);
		std::ofstream i(file);
		i << std::setw(4) << jsonOb;
		i.close();
	}

	nlohmann::json FileSystem::loadJsonFromFile(const std::string& name) {
		std::string file = std::format("{}.json", name);
		std::ifstream i(file);
		if (i.fail()) {
			Log::error("Failed to open json file");
			return NULL;
		}
		nlohmann::json jsonOb = nlohmann::json::parse(i);
		i.close();
		return jsonOb;
	}

	void FileSystem::saveStringToFile(const std::string& str, const std::string& name) {
		std::string file = std::format("{}.txt", name);
		std::ofstream i(file);
		i << str;
		i.close();
	}

	void FileSystem::createFile(const std::string& file) {
		std::ofstream i(file);
		i.close();
	}

	bool FileSystem::createFileIfDoesntExist(const std::string& file) {
		bool exists = doesFileExist(file);
		if (!exists) createFile(file);
		return !exists;
	}

	std::string FileSystem::getShortFilename(const std::string& filename) {
		const char* lastSlash = strrchr(filename.c_str(), '/');
		if (lastSlash == nullptr) {
			lastSlash = strrchr(filename.c_str(), '\\');
		}
		std::string shortFilename = lastSlash != nullptr ? lastSlash + 1 : filename;
		return shortFilename;
	}

	void FileSystem::copyFile(const std::string& source, const std::string& dest) {
		if (doesFileExist(source)) {
			std::ifstream src(source.c_str(), std::ios::binary);
			std::ofstream dst(dest.c_str(), std::ios::binary);
			dst << src.rdbuf();
			src.close();
			dst.close();
		}
	}

	std::filesystem::path FileSystem::getAppDataPath() {
		WCHAR path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
			return std::filesystem::path(path);
		} else {
			throw std::runtime_error("Failed to get AppData path.");
		}
	}

	void FileSystem::openFolderAndSelectItem(const std::filesystem::path &path) {
		std::wstring argument = L"/select,\"" + path.wstring() + L"\"";
		ShellExecuteW(NULL, L"open", L"explorer.exe", argument.c_str(), NULL, SW_SHOWNORMAL);
	}
}
