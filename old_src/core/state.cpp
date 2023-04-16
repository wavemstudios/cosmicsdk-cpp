#include "state.h"
#include "chainTip.h"
#include "blockmanager.h"

State::State(std::shared_ptr<DBService> &dbServer, std::shared_ptr<VMCommClient> &grpcClient) : grpcClient(grpcClient), gen(Hash()) {
  this->loadState(dbServer);
}

bool State::loadState(std::shared_ptr<DBService> &dbServer) {
  stateLock.lock();
  auto accounts = dbServer->readBatch(DBPrefix::nativeAccounts);
  if (accounts.empty()) {
    Address dev("0x21B782f9BF82418A42d034517CB6Bf00b4C17612", true); // Ita's address
    Address dev2("0xb3Dc9ed7f450d188c9B5a44f679a1dDBb4Cbd6D2", true); // Supra's address
    Address dev3("0x12e7742c063Dff92dA0439430DFe8A05ce0d297e", true); // Ita's office
    Address dev4("0xaE33707325C17CD37331278ccb74d2Ba9bFa6c92", true); // Ita's laptop
    dbServer->put(dev.get(),Utils::uint256ToBytes(uint256_t("100000000000000000000")) + Utils::uint32ToBytes(0), DBPrefix::nativeAccounts);
    Utils::logToFile("Added balance to: " + dev.hex());
    dbServer->put(dev2.get(),Utils::uint256ToBytes(uint256_t("100000000000000000000")) + Utils::uint32ToBytes(0), DBPrefix::nativeAccounts);
    Utils::logToFile("Added balance to: " + dev2.hex());
    dbServer->put(dev3.get(),Utils::uint256ToBytes(uint256_t("100000000000000000000")) + Utils::uint32ToBytes(0), DBPrefix::nativeAccounts);
    Utils::logToFile("Added balance to: " + dev3.hex());
    dbServer->put(dev4.get(),Utils::uint256ToBytes(uint256_t("100000000000000000000")) + Utils::uint32ToBytes(0), DBPrefix::nativeAccounts);
    Utils::logToFile("Added balance to: " + dev4.hex());
    accounts = dbServer->readBatch(DBPrefix::nativeAccounts);
  }
  for (const DBEntry account : accounts) {
    Address address(account.key, false);
    this->nativeAccount[address].balance = Utils::bytesToUint256(account.value.substr(0,32));
    this->nativeAccount[address].nonce = Utils::bytesToUint32(account.value.substr(32,4));
  }
  stateLock.unlock();
  return true;
}

bool State::saveState(std::shared_ptr<DBService> &dbServer) {
  stateLock.lock();
  WriteBatchRequest accountsBatch;
  for (auto &account : this->nativeAccount) {
    accountsBatch.puts.emplace_back(
      account.first.get(),
      Utils::uint256ToBytes(account.second.balance) + Utils::uint32ToBytes(account.second.nonce)
    );
  }
  dbServer->writeBatch(accountsBatch, DBPrefix::nativeAccounts);
  stateLock.unlock();
  return true;
}

bool State::validateTransactionForBlock(const Tx::Base& tx) const {
  if (!tx.verified()) return false; // Ignore unverified txs
  bool ret = true;
  stateLock.lock_shared();
  if (!this->mempool.count(tx.hash())) { // Ignore if tx already exists in mempool
    if (this->nativeAccount.count(tx.from()) == 0) { // Account doesn't exist = zero balance = can't pay fees
      ret = false;
    } else {
      Account acc = this->nativeAccount.find(tx.from())->second;
      if (acc.balance < tx.value() || acc.nonce != tx.nonce()) { // Insufficient balance or invalid nonce
        ret = false;
      }
    }
  }
  stateLock.unlock_shared();
  return ret;
}

std::pair<int, std::string> State::validateTransactionForRPC(const Tx::Base& tx) const {
  // TODO: Handle error conditions to report at RPC level:
  // https://www.jsonrpc.org/specification#error_object
  // https://eips.ethereum.org/EIPS/eip-1474#error-codes
  int err = 0;
  std::string errMsg;
  if (!tx.verified()) {
    err = -32003;
    errMsg = "Transaction signature not verified when TX was constructed: " + Utils::bytesToHex(tx.rlpSerialize(true));
    return std::make_pair(err, errMsg);
  }

  stateLock.lock_shared();
  if (this->mempool.count(tx.hash())) { // Not really considered a failure
    errMsg = "Transaction already exists in mempool";
  } else if (this->nativeAccount.count(tx.from()) == 0) { // Account doesn't exist = zero balance = can't pay fees
    err = -32003;
    errMsg = "Insufficient balance - required: " + boost::lexical_cast<std::string>(tx.value()) + ", available: 0";
  } else {
    Account acc = this->nativeAccount.find(tx.from())->second;
    if (acc.balance < tx.value()) {
      err = -32002;
      errMsg = "Insufficient balance - required: "
      + boost::lexical_cast<std::string>(tx.value()) + ", available: "
      + boost::lexical_cast<std::string>(acc.balance);
    } else if (acc.nonce != tx.nonce()) {
      err = -32001;
      errMsg = "Invalid nonce";
    }
  }
  stateLock.unlock_shared();

  if (err != 0) {
    errMsg.insert(0, "Transaction rejected: ");
    Utils::LogPrint(Log::subnet, "validateTransactionForRPC: ", errMsg);
  } else {
    stateLock.lock();
    Hash txHash = tx.hash();
    this->mempool[txHash] = tx;
    stateLock.unlock();
  }
  return std::make_pair(err, errMsg);
}

bool State::processNewTransaction(const Tx::Base& tx) {
  bool isContractCall = false;
  Utils::LogPrint(Log::state, __func__,
    "tx.from(): " + tx.from().hex() +
    " tx.value(): " + boost::lexical_cast<std::string>(tx.value())
  );
  if (this->mempool.count(tx.hash()) != 0) this->mempool.erase(tx.hash()); // Remove tx from mempool if found there

  // Update balances and nonce.
  this->nativeAccount[tx.from()].balance -= tx.value();
  this->nativeAccount[tx.from()].balance -= uint256_t(tx.gasPrice() * tx.gas());
  this->nativeAccount[tx.to()].balance += tx.value();
  this->nativeAccount[tx.from()].nonce++;

  // TODO: Handle contract calls.
  return true;
}


// Check block header, validation and transactions within.
// Invalid transactions (such as invalid signatures, account having min balance for min fees), if included in a block, the block will be rejected.
bool State::validateNewBlock(const std::shared_ptr<const Block> &newBlock, const std::shared_ptr<const ChainHead>& chainHead, const std::shared_ptr<const BlockManager>& blockManager) const {
  // Check block previous hash.
  auto bestBlock = chainHead->latest();
  if (bestBlock->getBlockHash() != newBlock->prevBlockHash()) {
    Utils::LogPrint(Log::state, __func__, "Block previous hash does not match.");
    Utils::LogPrint(Log::state, __func__, "newBlock previous hash: " + newBlock->prevBlockHash().get());
    Utils::LogPrint(Log::state, __func__, "bestBlock hash: " + bestBlock->getBlockHash().get());
    return false;
  }

  if (newBlock->nHeight() != (1 + bestBlock->nHeight())) {
    Utils::LogPrint(Log::state, __func__, "Block height does not match.");
    Utils::LogPrint(Log::state, __func__, "newBlock height: " + std::to_string(newBlock->nHeight()));
    Utils::LogPrint(Log::state, __func__, "bestBlock height: " + std::to_string(bestBlock->nHeight()));
    return false;
  }

  if (!blockManager->validateBlock(newBlock)) {
    Utils::LogPrint(Log::state, __func__, "Block validation failed. validators does not match");
    return false;
  }

  for (const auto &tx : newBlock->transactions()) {
    if (!this->validateTransactionForBlock(tx.second)) {
      Utils::LogPrint(Log::state, __func__, "Block rejected due to invalid transaction");
      return false;
    }
  }

  Utils::LogPrint(Log::state, __func__, "Block " + Utils::bytesToHex(newBlock->getBlockHash().get()) + ", height " + boost::lexical_cast<std::string>(newBlock->nHeight()) + " validated.");
  return true;
}

void State::processNewBlock(const std::shared_ptr<const Block>&& newBlock, const std::shared_ptr<ChainHead>& chainHead, const std::shared_ptr<BlockManager>& blockManager) {
  // Check block previous hash.
  this->stateLock.lock();
  Utils::LogPrint(Log::state, __func__, "Processing new block " + Utils::bytesToHex(newBlock->getBlockHash().get()) + ", height " + boost::lexical_cast<std::string>(newBlock->nHeight()));
  auto bestBlock = chainHead->latest();

  for (const auto &tx : newBlock->transactions()) {
    this->processNewTransaction(tx.second);
  }
  // Append block to chainHead.
  blockManager->processBlock(newBlock);
  chainHead->push_back(std::move(newBlock));
  this->mempool.clear();
  this->stateLock.unlock();
}

const std::shared_ptr<const Block> State::createNewBlock(const std::shared_ptr<ChainHead>& chainHead, const std::shared_ptr<ChainTip> &chainTip, const std::shared_ptr<BlockManager> &blockManager) const {
  Utils::LogPrint(Log::state, __func__, "Creating new block.");
  auto bestBlockHash = chainTip->getPreference();
  std::shared_ptr<const Block> bestBlock;
  if (bestBlockHash.empty()) {
    Utils::LogPrint(Log::state, __func__, "No prefered block found");
    return nullptr;
  }
  Utils::LogPrint(Log::state, __func__, std::string("Got preference: ") + Utils::bytesToHex(bestBlockHash.get()));
  bestBlock = chainHead->getBlock(bestBlockHash);
  if (bestBlock == nullptr) { // Prefered block not found
    Utils::LogPrint(Log::state, __func__, "Prefered block does not exist");
    return nullptr;
  }
  Utils::LogPrint(Log::state, __func__, "Got best block.");

  auto newBestBlock = std::make_shared<Block>(
    bestBlock->getBlockHash(),
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count(),
    bestBlock->nHeight() + 1
  );

  stateLock.lock_shared();
  for (auto &tx : this->mempool) newBestBlock->appendTx(tx.second);
  stateLock.unlock_shared();

  auto validatorsMempool = blockManager->getMempoolCopy();
  auto validatorRandomList = blockManager->getRandomListCopy();

  // Order things up, first 4 transactions are randomHash, then 4 last transactions are random itself.
  std::vector<Tx::Validator> validatorTxs;
  for (auto const &tx : validatorsMempool) {

    Utils::logToFile(std::string("TX: ") + tx.second.hash().hex() + " FROM: " + tx.second.from().hex() +" TX TYPE: " + std::to_string(blockManager->getTransactionType(tx.second)));
  }

  // Reorder validator transactions, the mempool is a unordered map but it is required for the block
  // to have the validators transacations ordered
  // In the current code we are ordering as following:
  // First, append in order validator[1...4] (randomList) randomHash transactions
  // Then append in order validator[1...4] (randomList) randomSeed transactions

  while (validatorTxs.size() < blockManager->minValidators * 2) {
    for (auto const &tx : validatorsMempool) {
      if (validatorTxs.size() < blockManager->minValidators) { // Index the randomHash
        if (tx.second.from() == validatorRandomList[validatorTxs.size() + 1].get().get() && blockManager->getTransactionType(tx.second) == BlockManager::TransactionTypes::randomHash) { // skip [0] as it is us lol
          validatorTxs.push_back(tx.second);
          Utils::logToFile("Indexing validator hash tx");
        }
      } else { // Index the randomSeed
        if (tx.second.from() == validatorRandomList[validatorTxs.size() - blockManager->minValidators + 1].get().get() && blockManager->getTransactionType(tx.second) == BlockManager::TransactionTypes::randomSeed) {
          validatorTxs.push_back(tx.second);
          Utils::logToFile("Indexing validator seed tx");
        }
      }
      if (validatorTxs.size() == blockManager->minValidators * 2) break;
    }
  }

  // Append validators transactions
  for (auto const &i : validatorTxs) {
    newBestBlock->appendValidatorTx(i);
  }

  // Sign and finalize the block
  blockManager->finalizeBlock(newBestBlock);

  Utils::logToFile(std::string("createNewBlock Signature: ") + newBestBlock->signature().hex());

  Utils::LogPrint(Log::state, __func__, "New block created.");
  return newBestBlock;
}

uint256_t State::getNativeBalance(const Address& address) {
  uint256_t ret;
  this->stateLock.lock_shared();
  ret = this->nativeAccount[address].balance;
  this->stateLock.unlock_shared();
  return ret;
};

uint256_t State::getNativeNonce(const Address& address) {
  uint256_t ret;
  this->stateLock.lock_shared();
  ret = this->nativeAccount[address].nonce;
  this->stateLock.unlock_shared();
  return ret;
};

void State::addBalance(const Address &address) {
  this->stateLock.lock();
  this->nativeAccount[address].balance += 1000000000000000000;
  this->stateLock.unlock();
}