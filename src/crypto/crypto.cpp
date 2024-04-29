#include "crypto/crypto.hpp"
#include "crypto/secret.hpp"
#include "crypto/keys.hpp"
#include "crypto/hashes.hpp"

#include "util/bstream.h"

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <iostream>

// Filename of the game's executable
constexpr char* EXE_FILENAME = "rdr2.exe";

namespace rdr3 {
	namespace crypto {
		CKeys RDR2_KEYS;
	}
}

namespace {
	rdr3::crypto::CSecretStore _secrets;
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

	if (!_secrets.TryLoad()) {
		return false;
	}

	return RDR2_KEYS.LoadKeys(&_secrets);
}
