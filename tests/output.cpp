#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <memory>
#include <sstream>
#include <string>

#include "config.h"
#include "main.cpp"

using namespace std::literals::string_literals;

TEST_CASE("Test key_from_hex_chars", "[output]")
{
  REQUIRE(key_from_hex_chars(""s) == std::vector<std::uint8_t> {});
  REQUIRE(key_from_hex_chars("hello"s) == std::nullopt);
  REQUIRE(key_from_hex_chars("12z"s) == std::nullopt);
  REQUIRE(key_from_hex_chars("0xabcd"s) == std::nullopt);
  REQUIRE(key_from_hex_chars("ab cd"s) == std::nullopt);
  REQUIRE(key_from_hex_chars("a"s) == std::vector<std::uint8_t> {0x0a});
  REQUIRE(key_from_hex_chars("0123456789abcdef"s) ==
          std::vector<std::uint8_t> {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef});
  REQUIRE(key_from_hex_chars("0123456789ABCDEF"s) ==
          std::vector<std::uint8_t> {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef});
}

/**
 * Compare the output of stenc given device responses
 *
 * These tests check the representation and interpretation of raw device data
 * and that the program output accurately reports the meaning of the data.
 */
TEST_CASE("Test SCSI inquiry output", "[output]")
{
  const uint8_t response[] {
    0x01, 0x80, 0x00, 0x02, 0x5b, 0x00, 0x00, 0x02,
    0x41, 0x43, 0x4d, 0x45, 0x20, 0x20, 0x20, 0x20,
    0x55, 0x6c, 0x74, 0x72, 0x69, 0x75, 0x6d, 0x2d,
    0x31, 0x30, 0x30, 0x30, 0x20, 0x20, 0x20, 0x20,
    0x31, 0x32, 0x33, 0x34, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };
  // note: fixed width strings in output
  const std::string expected_output {"\
Vendor:                  ACME    \n\
Product ID:              Ultrium-1000    \n\
Product Revision:        1234\n"s};
  std::ostringstream oss;
  print_device_inquiry(oss, reinterpret_cast<const scsi::inquiry_data&>(response));
  REQUIRE(oss.str() == expected_output);
}

TEST_CASE("SCSI get device encryption status output 1", "[output]")
{
  const uint8_t page[] {
    0x00, 0x20, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };
  const std::string expected_output {"\
Drive Encryption:        off\n\
Drive Output:            Not decrypting\n\
                         Raw encrypted data not outputted\n\
Drive Input:             Not encrypting\n\
Key Instance Counter:    0\n"s};
  std::ostringstream oss;
  print_device_status(oss, reinterpret_cast<const scsi::page_des&>(page), true);
  REQUIRE(oss.str() == expected_output);
}

TEST_CASE("SCSI get device encryption status output 2", "[output]")
{
  const uint8_t page[] {
    0x00, 0x20, 0x00, 0x24, 0x42, 0x02, 0x02, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x48, 0x65, 0x6c, 0x6c,
    0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21,
  };
  const std::string expected_output {"\
Drive Encryption:        on\n\
Drive Output:            Decrypting\n\
                         Unencrypted data not outputted\n\
Drive Input:             Encrypting\n\
Key Instance Counter:    1\n\
Encryption Algorithm:    1\n\
Drive Key Desc.(uKAD):   Hello world!\n"s};
  std::ostringstream oss;
  print_device_status(oss, reinterpret_cast<const scsi::page_des&>(page), true);
  REQUIRE(oss.str() == expected_output);
}

TEST_CASE("Test SCSI get next block encryption status output 1", "[output]")
{
  const uint8_t page[] {
    0x00, 0x21, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  };
  const std::string expected_output {"\
Volume Encryption:       Not encrypted\n"s};
  std::ostringstream oss;
  print_volume_status(oss, reinterpret_cast<const scsi::page_nbes&>(page));
  REQUIRE(oss.str() == expected_output);
}

TEST_CASE("Test SCSI get next block encryption status output 2", "[output]")
{
  const uint8_t page[] {
    0x00, 0x21, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x0c, 0x48, 0x65, 0x6c, 0x6c,
    0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21,
  };
  const std::string expected_output {"\
Volume Encryption:       Encrypted and able to decrypt\n\
Volume Algorithm:        1\n"s};
  std::ostringstream oss;
  print_volume_status(oss, reinterpret_cast<const scsi::page_nbes&>(page));
  REQUIRE(oss.str() == expected_output);
}