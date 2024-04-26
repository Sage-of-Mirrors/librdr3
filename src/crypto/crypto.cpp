#include "crypto/crypto.hpp"
#include "crypto/hashes.hpp"
#include "util/bstream.h"

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <iostream>

// Filename of the game's executable
constexpr char* EXE_FILENAME = "rdr2.exe";

// Number of bytes in the AES key
constexpr uint8_t AES_KEY_SIZE = 32;
// Number of bytes in a SHA1 digest
constexpr uint8_t SHA1_DIGEST_SIZE = 20;

// Offset of the game's AES key, as of version 1491.50
constexpr size_t LAST_KEY_POS = 0x0000000003C74978;
// Number of KEY_SIZE chunks to search for the key if it's not found at the expected location. 
constexpr uint32_t KEY_SEARCH_RANGE = 0x100000;

namespace rdr3 {
	namespace crypto {
		CKeys RDR2_KEYS;
	}
}

namespace {
	bool ReadPotentialAESKey(EVP_MD_CTX* evpCtx, bStream::CFileStream* stream, size_t pos, uint8_t* buffer) {
		EVP_DigestInit_ex(evpCtx, EVP_sha1(), nullptr);

		stream->seek(pos);
		stream->readBytesTo(buffer, AES_KEY_SIZE);

		EVP_DigestUpdate(evpCtx, buffer, AES_KEY_SIZE);

		unsigned int outLen = 0;
		unsigned char digest[SHA1_DIGEST_SIZE] { };
		EVP_DigestFinal_ex(evpCtx, digest, &outLen);

		return std::memcmp(RDR2_PC_AES_KEY_HASH, digest, SHA1_DIGEST_SIZE) == 0;
	}

	bool ExtractAESKey(std::filesystem::path gameExePath) {
		OpenSSL_add_all_digests();
		ERR_load_crypto_strings();

		EVP_MD_CTX* evpCtx = EVP_MD_CTX_create();

		uint8_t keyBuffer[AES_KEY_SIZE] { };
		bStream::CFileStream stream(gameExePath.u8string());

		// First, attempt to extract from the key's last known position.
		bool bKeyFound = ReadPotentialAESKey(evpCtx, &stream, LAST_KEY_POS, keyBuffer);

		// Key wasn't where we thought it would be... snoop above and below
		// where it should've been to see if an update shifted it somewhere.
		if (!bKeyFound) {
			for (uint32_t i = 1; i < KEY_SEARCH_RANGE; i++) {
				if (ReadPotentialAESKey(evpCtx, &stream, LAST_KEY_POS - (i * AES_KEY_SIZE), keyBuffer) ||
					ReadPotentialAESKey(evpCtx, &stream, LAST_KEY_POS + (i * AES_KEY_SIZE), keyBuffer)) {
					std::cout << "AES key was found above or below where it normally is." << std::endl
						      << "Please contact @SageOfMirrors to update its last known position." << std::endl;

					bKeyFound = true;
					break;
				}
			}
		}

		if (bKeyFound) {
			rdr3::crypto::RDR2_KEYS.SetAESKey(keyBuffer);
		}

		EVP_cleanup();
		return bKeyFound;
	}
}

bool rdr3::crypto::InitCrypto(std::filesystem::path gameRoot) {
	if (!std::filesystem::exists(gameRoot)) {
		std::cout << "Game root " << gameRoot << " does not exist. Crypto init failed." << std::endl;
		return false;
	}

	std::filesystem::path exePath = gameRoot / EXE_FILENAME;
	if (!std::filesystem::exists(exePath)) {
		std::cout << "Game root " << gameRoot << " does not contain the RDR2 executable! Crypto init failed. Try repairing your install." << std::endl;
		return false;
	}

	if (!RDR2_KEYS.TryLoad()) {
		std::cout << "Failed to load AES keys, attempting to extract fresh copies..." << std::endl;

		if (!ExtractAESKey(exePath)) {
			std::cout << "Failed to extract AES keys from the RDR2 executable. Crypto init failed." << std::endl;
			return false;
		}
	}

	return true;
}
