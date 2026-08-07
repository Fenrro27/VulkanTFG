#include "GEVulkanRuntime.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <process.h>
#elif defined(__APPLE__)
#include <dirent.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace GEVulkanRuntime {

namespace {

	const char* const PAYLOAD_MAGIC = "GETFGPAYL";
	const char* const PAYLOAD_FOOTER = "GTRAILER!";
	constexpr std::streamoff FOOTER_SIZE = 17; // 9 de magic + 8 de offset

	struct PayloadEntry {
		std::string name;
		std::vector<char> data;
	};

	std::string dirName(const std::string& path) {
		size_t pos = path.find_last_of("/\\");
		if (pos == std::string::npos) return ".";
		if (pos == 0) return "/";
		return path.substr(0, pos);
	}

	std::string getExecutablePath() {
#ifdef _WIN32
		wchar_t buf[MAX_PATH];
		DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
		if (n == 0 || n >= MAX_PATH) return "";
		int len = WideCharToMultiByte(CP_UTF8, 0, buf, (int)n, nullptr, 0, nullptr, nullptr);
		std::string path(len, 0);
		WideCharToMultiByte(CP_UTF8, 0, buf, (int)n, &path[0], len, nullptr, nullptr);
		return path;
#elif defined(__APPLE__)
		uint32_t size = 0;
		_NSGetExecutablePath(nullptr, &size);
		std::vector<char> buf(size + 1, 0);
		if (_NSGetExecutablePath(buf.data(), &size) != 0) return "";
		return std::string(buf.data());
#else
		char buf[PATH_MAX];
		ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
		if (n <= 0) return "";
		buf[n] = '\0';
		return std::string(buf);
#endif
	}

	void setEnvVar(const std::string& name, const std::string& value) {
#ifdef _WIN32
		SetEnvironmentVariableA(name.c_str(), value.c_str());
#else
		setenv(name.c_str(), value.c_str(), 1);
#endif
	}

	void changeDir(const std::string& dir) {
#ifdef _WIN32
		_chdir(dir.c_str());
#else
		chdir(dir.c_str());
#endif
	}

	// Detecta si el sistema ya tiene ICDs Vulkan instalados (hardware o software
	// del sistema). Si el usuario ya definio VK_DRIVER_FILES / VK_ICD_FILENAMES
	// se respeta su eleccion y se considera que hay driver.
	bool hasSystemIcd() {
		const char* override = std::getenv("VK_DRIVER_FILES");
		if (override && override[0] != '\0') return true;
		override = std::getenv("VK_ICD_FILENAMES");
		if (override && override[0] != '\0') return true;
#ifdef _WIN32
		const char* keys[] = {
			"SOFTWARE\\Khronos\\Vulkan\\Drivers",
			"SOFTWARE\\WOW6432Node\\Khronos\\Vulkan\\Drivers"
		};
		for (const char* k : keys) {
			HKEY hKey = nullptr;
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, k, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				DWORD values = 0;
				RegQueryInfoKeyA(hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &values, nullptr, nullptr, nullptr, nullptr);
				RegCloseKey(hKey);
				if (values > 0) return true;
			}
		}
		return false;
#else
		std::vector<std::string> dirs;
		const char* xdgConf = std::getenv("XDG_CONFIG_HOME");
		const char* xdgData = std::getenv("XDG_DATA_HOME");
		const char* home = std::getenv("HOME");
		if (xdgConf && xdgConf[0]) dirs.push_back(std::string(xdgConf) + "/vulkan/icd.d");
		else if (home) dirs.push_back(std::string(home) + "/.config/vulkan/icd.d");
		if (xdgData && xdgData[0]) dirs.push_back(std::string(xdgData) + "/vulkan/icd.d");
		else if (home) dirs.push_back(std::string(home) + "/.local/share/vulkan/icd.d");
		dirs.push_back("/usr/local/share/vulkan/icd.d");
		dirs.push_back("/usr/share/vulkan/icd.d");
		dirs.push_back("/etc/vulkan/icd.d");

		for (const auto& d : dirs) {
			DIR* dp = opendir(d.c_str());
			if (!dp) continue;
			bool any = false;
			struct dirent* e;
			while ((e = readdir(dp)) != nullptr) {
				if (e->d_name[0] == '.') continue;
				std::string name = e->d_name;
				if (name.size() > 5 && name.substr(name.size() - 5) == ".json") {
					any = true;
					break;
				}
			}
			closedir(dp);
			if (any) return true;
		}
		return false;
#endif
	}

	bool readPayload(const std::string& exePath, std::vector<PayloadEntry>& out) {
		std::ifstream f(exePath, std::ios::binary);
		if (!f) return false;
		f.seekg(0, std::ios::end);
		std::streamoff size = f.tellg();
		if (size < FOOTER_SIZE) return false;
		f.seekg(size - FOOTER_SIZE);
		char footer[FOOTER_SIZE];
		f.read(footer, FOOTER_SIZE);
		if (std::memcmp(footer, PAYLOAD_FOOTER, 9) != 0) return false;
		uint64_t start = 0;
		std::memcpy(&start, footer + 9, 8);
		if (start >= (uint64_t)size) return false;
		f.seekg((std::streamoff)start);
		char magic[9] = { 0 };
		f.read(magic, 8);
		if (std::memcmp(magic, PAYLOAD_MAGIC, 8) != 0) return false;
		uint64_t count = 0;
		f.read((char*)&count, 8);
		if (count > 4096) return false;
		for (uint64_t i = 0; i < count; i++) {
			uint32_t nameLen = 0;
			f.read((char*)&nameLen, 4);
			if (nameLen == 0 || nameLen > 4096) return false;
			std::string name(nameLen, 0);
			f.read(&name[0], nameLen);
			uint64_t dataLen = 0;
			f.read((char*)&dataLen, 8);
			if (dataLen > (uint64_t)size) return false;
			PayloadEntry e;
			e.name = name;
			e.data.resize((size_t)dataLen);
			if (dataLen > 0) f.read(e.data.data(), dataLen);
			out.push_back(std::move(e));
		}
		return true;
	}

	void extractPayload(const std::string& exePath, const std::string& baseDir, const std::vector<PayloadEntry>& payload) {
		std::string stamp = baseDir + "/extracted.timestamp";
		bool need = true;
		try {
			if (fs::exists(stamp)) {
				need = fs::last_write_time(stamp) < fs::last_write_time(exePath);
			}
		}
		catch (...) {
			need = true;
		}
		if (!need) return;
		for (const auto& e : payload) {
			std::string target = baseDir + "/" + e.name;
			try {
				fs::create_directories(fs::path(target).parent_path());
			}
			catch (...) {
				continue;
			}
			std::ofstream of(target, std::ios::binary | std::ios::trunc);
			if (!of) continue;
			if (!e.data.empty()) of.write(e.data.data(), (std::streamsize)e.data.size());
		}
		std::ofstream of(stamp, std::ios::binary | std::ios::trunc);
	}

#ifdef _WIN32
	std::string baseName(const std::string& path) {
		size_t pos = path.find_last_of("/\\");
		if (pos == std::string::npos) return path;
		return path.substr(pos + 1);
	}

	// Reescribe el "library_path" del json ICD empaquetado con la ruta absoluta
	// donde quedo extraido (el loader no resuelve rutas relativas de forma
	// fiable en Windows).
	std::string fixWindowsIcd(const std::string& baseDir) {
		std::string json = baseDir + "\\vulkan\\vk_swiftshader_icd.json";
		if (!fs::exists(json)) return "";
		std::ifstream in(json, std::ios::binary);
		std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		size_t lp = text.find("library_path");
		if (lp == std::string::npos) return json;
		size_t q1 = text.find('"', lp);
		if (q1 == std::string::npos) return json;
		size_t q2 = text.find('"', q1 + 1);
		if (q2 == std::string::npos) return json;
		std::string rel = text.substr(q1 + 1, q2 - q1 - 1);
		std::string absPath = baseDir + "vulkan\\" + baseName(rel);
		text.replace(q1 + 1, rel.size(), absPath);
		std::ofstream out(json, std::ios::binary | std::ios::trunc);
		out << text;
		std::cout << "[GEVulkanRuntime] ICD de software -> " << absPath << std::endl;
		return json;
	}
#endif

} // namespace

//
// FUNCION: GEVulkanRuntime::setup()
//
// PROPOSITO: Prepara el entorno Vulkan antes de crear la instancia:
//            - cambia al directorio de trabajo con los recursos
//            - activa el driver Vulkan empaquetado si el sistema no tiene ninguno
//
void setup() {
	std::string exe = getExecutablePath();
	if (exe.empty()) return;
	std::string exeDir = dirName(exe);

	try {
#ifdef _WIN32
		std::string base;
		const char* localAppData = std::getenv("LOCALAPPDATA");
		base = localAppData ? std::string(localAppData) : exeDir;
		base += "\\ProjectTFG";

		std::vector<PayloadEntry> payload;
		bool ok = readPayload(exe, payload);
		if (ok) {
			extractPayload(exe, base, payload);
			changeDir(base.c_str());
			std::cout << "[GEVulkanRuntime] Recursos extraidos de la aplicacion a " << base << std::endl;
		}

		if (!hasSystemIcd() && ok) {
			std::string icd = fixWindowsIcd(base);
			if (fs::exists(icd)) {
				setEnvVar("VK_DRIVER_FILES", icd);
				setEnvVar("VK_ICD_FILENAMES", icd);
				std::cout << "[GEVulkanRuntime] Sin driver Vulkan en el sistema. Usando SwiftShader empaquetado." << std::endl;
			}
		}
#elif defined(__APPLE__)
		std::string resources = exeDir + "/../Resources";
		if (fs::is_directory(resources)) {
			changeDir(resources.c_str());
		}
		std::string icd = resources + "/vulkan/icd.d/MoltenVK_icd.json";
		std::string icdAlt = exeDir + "/MoltenVK_icd.json";
		std::string icdPath = fs::exists(icd) ? icd : (fs::exists(icdAlt) ? icdAlt : "");
		if (!icdPath.empty()) {
			setEnvVar("VK_DRIVER_FILES", icdPath);
			setEnvVar("VK_ICD_FILENAMES", icdPath);
			std::cout << "[GEVulkanRuntime] MoltenVK empaquetado activado: " << icdPath << std::endl;
		}
#else
		changeDir(exeDir.c_str());
		if (!hasSystemIcd()) {
			std::string icd = exeDir + "/vulkan/lvp_icd.x86_64.json";
			if (fs::exists(icd)) {
				setEnvVar("VK_DRIVER_FILES", icd);
				setEnvVar("VK_ICD_FILENAMES", icd);
				std::cout << "[GEVulkanRuntime] Sin driver Vulkan en el sistema. Usando lavapipe empaquetado." << std::endl;
			}
		}
#endif
	}
	catch (...) {
		// El empaquetado jamas debe impedir el arranque de la aplicacion
	}
}

} // namespace GEVulkanRuntime