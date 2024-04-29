#include "crypto/keys.hpp"
#include "crypto/secret.hpp"
#include "crypto/hashes.hpp"

constexpr uint32_t KEY_COUNT = 163;
constexpr char RAGE_IV_HASH[] = "5C8xGcF{L^AxqTQd!ofA";

rdr3::crypto::CKeys::CKeys() {

}

bool rdr3::crypto::CKeys::LoadKeys(CSecretStore* secrets) {
	CTfit2Key keyBuffer;

	for (size_t i = 0; i < KEY_COUNT; i++) {		
		if (secrets->Get(RDR2_PC_KEY_HASHES[i], &keyBuffer, sizeof(keyBuffer))) {
			mKeys.emplace(i, keyBuffer);
		}
	}

	if (secrets->Get(RDR2_PC_KEY_HASHES[163], &keyBuffer, sizeof(keyBuffer))) {
		mKeys.emplace(0xC0, keyBuffer);
	}

	if (secrets->Get(RDR2_PC_KEY_HASHES[164], &keyBuffer, sizeof(keyBuffer))) {
		mKeys.emplace(0xC5, keyBuffer);
	}

	if (secrets->Get(RDR2_PC_KEY_HASHES[165], &keyBuffer, sizeof(keyBuffer))) {
		mKeys.emplace(0xC6, keyBuffer);
	}

#define X(NAME, HASH)                                 \
    do                                                \
    {                                                 \
        if (!secrets->Get(HASH, &NAME, sizeof(NAME))) \
            return false;                             \
    } while (false)

#define Y(NAME) X(mContext.NAME, RDR2_PC_TFIT2_HASHES.NAME)

    Y(InitTables);

    for (size_t i = 0; i < 17; ++i)
    {
        Y(Rounds[i].Lookup);

        for (size_t j = 0; j < 16; ++j)
        {
            Y(Rounds[i].Blocks[j].Masks);
            Y(Rounds[i].Blocks[j].Xor);
        }
    }

    Y(EndMasks);
    Y(EndTables);
    Y(EndXor);

#undef Y

    X(mInitVector, RAGE_IV_HASH);

    return true;
}

rdr3::crypto::CTfit2CbcCipher rdr3::crypto::CKeys::GetCipher(uint16_t tag) {
    if (auto iter = mKeys.find(tag); iter != mKeys.end()) {
        return CTfit2CbcCipher(&iter->second, mInitVector, &mContext);
    }
}
