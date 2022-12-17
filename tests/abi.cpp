#include "tests.h"
#include "../src/utils/utils.h"
#include "../src/contract/abi.h"
#include <boost/filesystem.hpp>
#include <fstream>

void Tests::testABIDecoder() {
  // uint256[3]
  {
    std::string hexABI = "0x000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000002017594d841303970000000000000000000000000000000000000000000000000000027cae776d7500000000000000000000000000000000000000000000000000016201a9fce5dd";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::uint256Arr
    };

    ABI::Decoder decoder(types, ABI);

    auto vector = decoder.get<std::vector<uint256_t>>(0);

    assert(vector[0] == uint256_t(2312415123141231511));
    assert(vector[1] == uint256_t(2734526262645));
    assert(vector[2] == uint256_t(389234263123421));
  }

  // Address[3]
  {
    std::string hexABI = "0x000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000030000000000000000000000005b38da6a701c568545dcfcb03fcb875f56beddc4000000000000000000000000ab8483f64d9c6d1ecf9b849ae677dd3315835cb20000000000000000000000004b20993bc481177ec7e8f571cecae8a9e22c02db";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::addressArr
    };

    ABI::Decoder decoder(types, ABI);
    auto vector = decoder.get<std::vector<Address>>(0);

    assert(vector[0] == Address("0x5B38Da6a701c568545dCfcB03FcB875f56beddC4", true));
    assert(vector[1] == Address("0xAb8483F64d9C6d1EcF9b849Ae677dD3315835cb2", true));
    assert(vector[2] == Address("0x4B20993Bc481177ec7E8f571ceCaE8A9e22C02db", true));
  }

  // Boolean[3]
  {
    std::string hexABI = "0x00000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::booleanArr
    };

    ABI::Decoder decoder(types, ABI);

    auto vector = decoder.get<std::vector<bool>>(0);

    assert(vector[0] == true);
    assert(vector[1] == false);
    assert(vector[2] == true);
  }

  // Bytes
  {
    std::string hexABI = "0x000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000040adf1f1a00000000000000000000000000000000000000000000000000000000";
    std::string ABI = Utils::hexToBytes(hexABI);
    std::vector<ABI::Types> types = {
      ABI::Types::bytes
    };

    ABI::Decoder decoder(types, ABI);
    auto bytes = decoder.get<std::string>(0);

    assert(bytes == Utils::hexToBytes("0x0adf1f1a"));
  }

  // Bytes[]
  {
    std::string hexABI = "0x00000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000000040adf1f1a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004fffadcba0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000060113ffedc23100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002aaaa000000000000000000000000000000000000000000000000000000000000";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::bytesArr
    };

    ABI::Decoder decoder(types, ABI);

    auto bytesArr = decoder.get<std::vector<std::string>>(0);

    assert(bytesArr[0] == Utils::hexToBytes("0x0adf1f1a"));
    assert(bytesArr[1] == Utils::hexToBytes("0xfffadcba"));
    assert(bytesArr[2] == Utils::hexToBytes("0x0113ffedc231"));
    assert(bytesArr[3] == Utils::hexToBytes("0xaaaa"));

  }

  // String
  {
    std::string hexABI = "0x0000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000e5468697320697320612074657374000000000000000000000000000000000000";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::string
    };

    ABI::Decoder decoder(types, ABI);
    auto string = decoder.get<std::string>(0);

    assert(string == "This is a test");
  }

  // String[]
  {
    std::string hexABI = "0x00000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001400000000000000000000000000000000000000000000000000000000000000016546869732069732074686520666972737420746573740000000000000000000000000000000000000000000000000000000000000000000000000000000000175468697320697320746865207365636f6e642074657374000000000000000000000000000000000000000000000000000000000000000000000000000000001654686973206973207468652074686972642074657374000000000000000000000000000000000000000000000000000000000000000000000000000000000016546869732069732074686520666f727468207465737400000000000000000000";
    std::string ABI = Utils::hexToBytes(hexABI);

    std::vector<ABI::Types> types = {
      ABI::Types::stringArr
    };

    ABI::Decoder decoder(types, ABI);

    auto stringArr = decoder.get<std::vector<std::string>>(0);

    assert(stringArr[0] == "This is the first test");
    assert(stringArr[1] == "This is the second test");
    assert(stringArr[2] == "This is the third test");
    assert(stringArr[3] == "This is the forth test");
  }

  // String[], Bytes[]
  {
    std::string hexABI = "0x000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000001e00000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001400000000000000000000000000000000000000000000000000000000000000016546869732069732074686520666972737420746573740000000000000000000000000000000000000000000000000000000000000000000000000000000000175468697320697320746865207365636f6e642074657374000000000000000000000000000000000000000000000000000000000000000000000000000000001654686973206973207468652074686972642074657374000000000000000000000000000000000000000000000000000000000000000000000000000000000016546869732069732074686520666f7274682074657374000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000000040adf1f1a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004fffadcba0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000060113ffedc23100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002aaaa000000000000000000000000000000000000000000000000000000000000";
    std::string ABI = Utils::hexToBytes(hexABI);
    std::vector<ABI::Types> types = {
      ABI::Types::stringArr,
      ABI::Types::bytesArr
    };

    ABI::Decoder decoder(types, ABI);

    auto stringArr = decoder.get<std::vector<std::string>>(0);
    auto bytesArr = decoder.get<std::vector<std::string>>(1);

    assert(stringArr[0] == "This is the first test");
    assert(stringArr[1] == "This is the second test");
    assert(stringArr[2] == "This is the third test");
    assert(stringArr[3] == "This is the forth test");
    assert(bytesArr[0] == Utils::hexToBytes("0x0adf1f1a"));
    assert(bytesArr[1] == Utils::hexToBytes("0xfffadcba"));
    assert(bytesArr[2] == Utils::hexToBytes("0x0113ffedc231"));
    assert(bytesArr[3] == Utils::hexToBytes("0xaaaa"));

  }

  std::cout << __func__ << " OK" << std::endl;
}

void Tests::testABIJSONDecoder() {
  const std::string addToAddressListExpected = Utils::hexToBytes(
    "0x5fd673e8000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000020000000000000000000000002e913a79206280b3882860b3ef4df8204a62c8b10000000000000000000000002e913a79206280b3882860b3ef4df8204a62c8b1"
  );
  const std::string addToStringListExpected = Utils::hexToBytes(
    "0xece4955100000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000052616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161616161610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004f626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262620000000000000000000000000000000000"
  );
  const std::string addToStringListFourExpected = Utils::hexToBytes(
    "0xece4955100000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000016000000000000000000000000000000000000000000000000000000000000001c0000000000000000000000000000000000000000000000000000000000000002461616161616161616161616161616161616161616161616161616161616161616161616100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005862626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262000000000000000000000000000000000000000000000000000000000000000000000000000000286363636363636363636363636363636363636363636363636363636363636363636363636363636300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000046464646400000000000000000000000000000000000000000000000000000000"
  );
  const std::string addToBytesListExpected = Utils::hexToBytes(
    "0x8ab94fd600000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000002aaaa0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bbbb000000000000000000000000000000000000000000000000000000000000"
  );
  const std::string addToBytesListFourExpected = Utils::hexToBytes(
    "0x8ab94fd600000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000000090aaaaaaaaaaaaaaaaa0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001fbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb0000000000000000000000000000000000000000000000000000000000000000020ccc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000120ddddddddddddddddddddddddddddddddddd0000000000000000000000000000"
  );
  const std::string testMultipleByteArrayExpected = Utils::hexToBytes(
    "0x70afa559000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000001200000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000002aaaa0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bbbb0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000002cccc0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002dddd000000000000000000000000000000000000000000000000000000000000"
  );
  const std::string addMultipleToByteListExpected = Utils::hexToBytes(
    "0xf953151e000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000046aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bbbb000000000000000000000000000000000000000000000000000000000000"
  );
  const std::string addMultipleToStringListExpected = Utils::hexToBytes(
    "0x4aee7a8d000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000003616161000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036262620000000000000000000000000000000000000000000000000000000000"
  );
  const std::string testAlmostAllExpected = Utils::hexToBytes(
    "0x1608f4b100000000000000000000000000000000000000000000000000007614cf69b633000000000000000000000000c4ea73d428ab6589c36905d0f0b01f3051740ff800000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000002e0000000000000000000000000000000000000000000000000000000000000046000000000000000000000000000000000000000000000000000000000000004e00000000000000000000000000000000000000000000000000000000000000002aaaa000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000016d4c6f72656d20697073756d20646f6c6f722073697420616d65742c20636f6e73656374657475722061646970697363696e6720656c69742e2050656c6c656e746573717565206567657420706f72747469746f7220746f72746f722c2065742074696e636964756e74206e6962682e2041656e65616e2065726174207175616d2c206d6178696d757320696420677261766964612073697420616d65742c2072686f6e63757320736564206e756c6c612e20437572616269747572206d6178696d75732074656c6c7573206469616d2c2076656c2076756c7075746174652073617069656e206d6178696d75732076697461652e204475697320636f6e73656374657475722c2066656c69732061742065666669636974757220636f6e73656374657475722c20746f72746f72206e69736c20626962656e64756d206d61757269732c20656765742076656e656e61746973206175677565206a7573746f206574206f72636953696d706c6520000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000120000000000000000000000000000000000000000000000000000000000000000561616161610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000054626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262626262000000000000000000000000000000000000000000000000000000000000000000000000000000000000000363636300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000c4ea73d428ab6589c36905d0f0b01f3051740ff8000000000000000000000000c4ea73d428ab6589c36905d0f0b01f3051740ff8000000000000000000000000c4ea73d428ab6589c36905d0f0b01f3051740ff80000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000e000000000000000000000000000000000000000000000000000000000000000020aaa00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000020bbb0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000021aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa00000000000000000000000000000000000000000000000000000000000000"
  );

  boost::filesystem::path contractPath = (boost::filesystem::current_path().parent_path().string() + "/tests/ArrayTest.json");

  std::ifstream contractFile(contractPath.string());
  json contractJson = json::parse(contractFile);

  ABI::JSONEncoder contract(contractJson);

  std::string addToAddressList = contract("addToAddressListArr",
    json::array({
      json::array({
        "0x2E913a79206280B3882860B3eF4dF8204a62C8B1",
        "0x2E913a79206280B3882860B3eF4dF8204a62C8B1"
      })
    }));

 std::string addToStringList = contract("addToStringListArr",
    json::array({
      json::array({
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      })
    }));

  std::string addToStringListFour = contract("addToStringListArr",
    json::array({
      json::array({
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "cccccccccccccccccccccccccccccccccccccccc",
        "dddd"
      })
    }));

  std::string addToBytesList = contract("addToByteListArr",
    json::array({
      json::array({
        "0xaaaa",
        "0xbbbb"
      })
    }));

  std::string addToBytesListFour = contract("addToByteListArr",
    json::array({
      json::array({
        "0xaaaaaaaaaaaaaaaaa",
        "0xbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "0xccc",
        "0xddddddddddddddddddddddddddddddddddd"
      })
    }));

  std::string testMultipleByteArray = contract("testMultipleByteArray",
    json::array({
      json::array({
        "0xaaaa",
        "0xbbbb"
      }),
      json::array({
        "0xcccc",
        "0xdddd"
      })
    }));

  std::string addMultipleToByteList = contract("addMultipleToByteList",
    json::array({
      "0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
      "0xbbbb"
    }));

  std::string addMultipleToStringList = contract("addMultipleToStringList",
    json::array({
      "aaa",
      "bbb"
    }));

  // This should do it lmao.
  // testAlmostAll(uint256 item1, address item2, bool item3, bytes item4, string item5, string[] item6, address[] item7, bytes[] item8)
  std::string testAlmostAll = contract("testAlmostAll",
    json::array({
      "129831751235123",
      "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8",
      "1",
      "0xaaaa",
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque eget porttitor tortor, et tincidunt nibh. Aenean erat quam, maximus id gravida sit amet, rhoncus sed nulla. Curabitur maximus tellus diam, vel vulputate sapien maximus vitae. Duis consectetur, felis at efficitur consectetur, tortor nisl bibendum mauris, eget venenatis augue justo et orciSimple ",
      json::array({
        "aaaaa",
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "ccc"
      }),
      json::array({
        "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8",
        "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8",
        "0xc4ea73d428ab6589c36905d0f0b01f3051740ff8"
      }),
      json::array({
        "0xaaa",
        "0xbbb",
        "0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      })
    }));

  assert(addToAddressList == addToAddressListExpected);
  assert(addToStringList == addToStringListExpected);
  assert(addToStringListFour == addToStringListFourExpected);
  assert(addToBytesList == addToBytesListExpected);
  assert(addToBytesListFour == addToBytesListFourExpected);
  assert(testMultipleByteArray == testMultipleByteArrayExpected);
  std::cout << Utils::bytesToHex(addMultipleToByteList) << std::endl;

  std::cout << " " << std::endl;

  std::cout << Utils::bytesToHex(addMultipleToByteListExpected) << std::endl;
  assert(addMultipleToByteList == addMultipleToByteListExpected);
  assert(addMultipleToStringList == addMultipleToStringListExpected);
  assert(testAlmostAll == testAlmostAllExpected);

  std::cout << __func__ << " OK" << std::endl;
  return;
}
