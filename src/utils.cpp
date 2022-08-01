#include "utils.h"

std::mutex log_lock;
std::mutex debug_mutex;

void Utils::logToFile(std::string str) {
  log_lock.lock();
  std::ofstream log("log.txt", std::ios::app);
  log << str << std::endl;
  log.close();
  log_lock.unlock();
}

std::string Utils::uint256ToBytes(const uint256_t &i) {
  std::string ret(32, 0x00);
  std::string tmp;
  boost::multiprecision::export_bits(i, std::back_inserter(tmp), 8);
  for (uint16_t i = 0; i < tmp.size(); ++i) {
    ret[31-i] = tmp[tmp.size()-i-1];  // Replace bytes from tmp to ret to make it 32 bytes in size.
  }
  return ret;
}

std::string Utils::uint160ToBytes(const uint160_t &i) {
  std::string ret(20, 0x00);
  std::string tmp;
  boost::multiprecision::export_bits(i, std::back_inserter(tmp), 8);
  for (uint16_t i = 0; i < tmp.size(); ++i) {
    ret[19-i] = tmp[tmp.size()-i-1];  // Replace bytes from tmp to ret to make it 32 bytes in size.
  }
  return ret;
}

std::string Utils::uint64ToBytes(const uint64_t &i) {
  std::string ret(8, 0x00);
  ret[0] = i >> 56;
  ret[1] = i >> 48;
  ret[2] = i >> 40;
  ret[3] = i >> 32;
  ret[4] = i >> 24;
  ret[5] = i >> 16;
  ret[6] = i >> 8;
  ret[7] = i;
  return ret;
}

std::string Utils::uint32ToBytes(const uint32_t &i) {
  std::string ret(4, 0x00);
  ret[0] = i >> 24;
  ret[1] = i >> 16;
  ret[2] = i >> 8;
  ret[3] = i;
  return ret;
}

std::string Utils::uint8ToBytes(const uint8_t &i) {
  std::string ret(1, 0x00);
  ret[0] = i;
  return ret;
}

uint256_t Utils::bytesToUint256(const std::string &bytes) {
  if (bytes.size() != 32) throw; // Invalid size.
  uint256_t ret;
  boost::multiprecision::import_bits(ret, bytes.begin(), bytes.end(), 8);
  return ret;
}

uint160_t Utils::bytesToUint160(const std::string &bytes) {
  if (bytes.size() != 20) throw; // Invalid size.
  uint160_t ret;
  boost::multiprecision::import_bits(ret, bytes.begin(), bytes.end(), 8);
  return ret;
}

uint64_t Utils::bytesToUint64(const std::string &bytes) {
  if (bytes.size() != 8) throw; // Invalid size;
  uint64_t ret;
  ret |= uint64_t(bytes[0]) << 56;
  ret |= uint64_t(bytes[1]) << 48;
  ret |= uint64_t(bytes[2]) << 40;
  ret |= uint64_t(bytes[3]) << 32;
  ret |= uint64_t(bytes[4]) << 24;
  ret |= uint64_t(bytes[5]) << 16;
  ret |= uint64_t(bytes[6]) << 8;
  ret |= uint64_t(bytes[7]);
  return ret;
}

uint32_t Utils::bytesToUint32(const std::string &bytes) {
  if (bytes.size() != 4) throw; // Invalid size
  uint32_t ret;
  ret |= bytes[0] << 24;
  ret |= bytes[1] << 16;
  ret |= bytes[2] << 8;
  ret |= bytes[3];
  return ret;
}

uint8_t Utils::bytesToUint8(const std::string &bytes) {
  if (bytes.size() != 1) throw; // Invalid size
  uint8_t ret;
  ret |= bytes[0];
  return ret;
}

void Utils::LogPrint(std::string prefix, std::string function, std::string data) {
  debug_mutex.lock();
  std::ofstream log("debug.txt", std::ios::app);
  log << prefix << function << " - " << data << std::endl;
  log.close();
  debug_mutex.unlock();
}

void Utils::patchHex(std::string& str) {
  if (str[0] == '0' && str[1] == 'x') str = str.substr(2);
  for (auto &c : str) if (std::isupper(c)) c = std::tolower(c);
  return;
}

uint256_t Utils::hexToUint(std::string &hex) {
  patchHex(hex);
  return boost::lexical_cast<HexTo<uint256_t>>(hex);
}

std::string Utils::hexToBytes(std::string hex) {
  patchHex(hex);
  std::string ret;
  uint32_t index = 0;
  
  if(hex.size() % 2 != 0) {
    int h = fromHexChar(hex[index]);
    if (h != -1) {
      ret += uint8_t(h);
    } else {
      throw;
    }
    ++index;
  }

  while (index < hex.size()) {
    int h = fromHexChar(hex[index]);
    int l = fromHexChar(hex[index+1]);
    if (h != -1 && l != -1) {
      ret += uint8_t(h * 16 + l);
    } else {
      // TODO: Error handling, error handling everywhere;
      return "";
    }
    index += 2;
  }
  return ret;
}

int Utils::fromHexChar(char _i) noexcept
{
	if (_i >= '0' && _i <= '9')
		return _i - '0';
	if (_i >= 'a' && _i <= 'f')
		return _i - 'a' + 10;
	if (_i >= 'A' && _i <= 'F')
		return _i - 'A' + 10;
	return -1;
}

std::string Utils::bytesToHex(std::string bytes) { return dev::toHex(bytes); }

bool Utils::verifySignature(uint8_t const &v, uint256_t const &r, uint256_t const &s) {
  // s_max = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141
  static const uint256_t s_max("115792089237316195423570985008687907852837564279074904382605163141518161494337");
  static const uint256_t s_zero = 0;
  return (v <= 1 && r > s_zero && s > s_zero && r < s_max && s < s_max);
}

void Utils::sha3(const std::string &input, std::string &output) {
  output.resize(32);
  keccakUint8_256(reinterpret_cast<unsigned char*>(output.data()), reinterpret_cast<const unsigned char*>(input.data()), input.size());
}
