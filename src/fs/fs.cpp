#include "fs/fs.hpp"
#include "fs/oodle.hpp"
#include "fs/fsdevice.hpp"
#include "fs/rpf8.hpp"

#include "crypto/crypto.hpp"

#include "util/util.hpp"

#include <pugixml.hpp>
#include <unordered_map>
#include <iostream>

constexpr char APPDATA_RPF_NAME[]  = "appdata0_update.rpf";
constexpr char MANIFEST_XML_NAME[] = "mountmanifest_tu.xml";

constexpr char MOUNTS_ROOT_NODE_NAME[] = "MountManifest";
constexpr char MOUNTS_NODE_NAME[]  = "Mounts";

constexpr char MOUNTPOINT_NODE_NAME[] = "MountPoint";
constexpr char PATH_NODE_NAME[] = "Path";

namespace {
	using MountPointNameHash = uint32_t;
	using MountPointDevices = shared_vector <rdr3::fs::CFSDevice>;

	bool bIsFSInitialized = false;
	std::unordered_map<MountPointNameHash, MountPointDevices> MountPoints;
}

bool LoadDevices(std::filesystem::path gameRoot) {
	std::filesystem::path appDataPath = gameRoot / APPDATA_RPF_NAME;
	if (!std::filesystem::exists(appDataPath)) {
		std::cout << "Cannot init FS without \"" << gameRoot << "\". Try repairing your install." << std::endl;
		return false;
	}

	std::shared_ptr<rdr3::fs::CFSDevice> appDataDevice = LoadRPF8(appDataPath);

	uint32_t manifestSize = 0;
	uint8_t* manifestData = appDataDevice->GetFileData(MANIFEST_XML_NAME, manifestSize);

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer(manifestData, size_t(manifestSize));
	if (result.status != pugi::status_ok) {
		std::cout << "Failed to load \"" << MANIFEST_XML_NAME << "\". Try repairing your install." << std::endl;
		return false;
	}

	pugi::xml_node mountNode = doc.child(MOUNTS_ROOT_NODE_NAME).child(MOUNTS_NODE_NAME);
	for (pugi::xml_node itemNode : mountNode.children()) {
		std::string mountName = itemNode.child(MOUNTPOINT_NODE_NAME).text().as_string();
		std::string deviceName = itemNode.child(PATH_NODE_NAME).text().as_string();

		if (deviceName[deviceName.length() - 1] == '/') {
			deviceName[deviceName.length() - 1] = '\\';
		}

		uint32_t mountNameHash = JenkinsHashString(mountName);
		if (auto iter = MountPoints.find(mountNameHash); iter == MountPoints.end()) {
			MountPoints.emplace(mountNameHash, shared_vector<rdr3::fs::CFSDevice>());
		}

		std::filesystem::path deviceFilePath = gameRoot / deviceName;
		if (!std::filesystem::exists(deviceFilePath)) {
			std::cout << "Attempted to mount device from \"" << deviceName << "\", but it was not found in \"" << gameRoot << "\"." << std::endl;
			continue;
		}

		// RPF file
		if (std::filesystem::is_regular_file(deviceFilePath)) {
			MountPoints[mountNameHash].push_back(LoadRPF8(deviceFilePath));
		}
		// Directory
		else if (std::filesystem::is_directory(deviceFilePath)) {
			std::cout << "Mounting manifest wants to mount directory \"" << deviceName << "\", but that's currently not supported. Skipping" << std::endl;
		}
		// Unknown type
		else {
			std::cout << "Mounting manifest wants to mount \"" << deviceName << "\", but it's neither a file nor a directory. Skipping." << std::endl;
		}
	}

	return true;
}

bool rdr3::fs::InitFS(std::filesystem::path gameRoot) {
	if (bIsFSInitialized) {
		std::cout << "FS already initialized!" << std::endl;
		return true;
	}

	if (!rdr3::crypto::InitCrypto(gameRoot) || !InitOodle(gameRoot)) {
		std::cout << "Failed to init FS." << std::endl;
		return false;
	}

	bIsFSInitialized = LoadDevices(gameRoot);
	return bIsFSInitialized;
}

std::string GetMountPointName(std::filesystem::path filePath) {
	std::string pathString = filePath.u8string();
	size_t colonOffset = pathString.find(':');

	return pathString.substr(0, colonOffset);
}

std::string GetRelativeFilePath(std::filesystem::path filePath) {
	std::string pathString = filePath.u8string();
	size_t colonOffset = pathString.find(':');

	// +2 to colonOffset to skip the ":/" from the mount point name.
	return pathString.substr(colonOffset + 2, pathString.length());
}

uint8_t* rdr3::fs::GetFileData(std::filesystem::path filePath, uint32_t& fileSize) {
	uint8_t* fileData = nullptr;
	if (!bIsFSInitialized) {
		return fileData;
	}

	uint32_t mountPointHash = JenkinsHashString(GetMountPointName(filePath));
	auto iter = MountPoints.find(mountPointHash);
	if (iter == MountPoints.end()) {
		return nullptr;
	}

	for (std::shared_ptr<CFSDevice> device : iter->second) {
		fileData = device->GetFileData(GetRelativeFilePath(filePath), fileSize);
		if (fileData != nullptr) {
			break;
		}
	}

	return fileData;
}
