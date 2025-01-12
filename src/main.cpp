#include <iostream>
#include "src/core/blockchain.h"
#include <filesystem>

int main() {
  Utils::logToCout = true;
  std::string blockchainPath = std::filesystem::current_path().string() + std::string("/blockchain");
  Blockchain blockchain(blockchainPath);
  /// Start the blockchain syncing engine.
  blockchain.start();
  std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(std::numeric_limits<int>::max()));
  return 0;
}