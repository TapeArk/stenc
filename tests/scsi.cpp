#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "config.h"
#include "scsiencrypt.h"


using namespace std::literals::string_literals;

/**
 * Compare the SPOUT Set Data Encryption pages generated by stenc to an
 * expected output buffer based on the SCSI command spec.
 *
 * This checks that the program can correctly format command buffers that
 * reflect available input and program options.
 */
TEST_CASE("Disable encryption command", "[scsi]") {
  SCSIEncryptOptions opt;
  uint8_t buffer[1024] {};
  const uint8_t expected[] {
    0x00, 0x10, // page code
    0x00, 0x30, // page length
    0x40, // scope
    DEFAULT_CEEM << 6, // CEEM, CKOD, RDMC, et al.
    0x00, // encyption mode
    0x00, // decryption mode
    0x01, // algorithm index
    0x00, // key format
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved [8]
    0x00, 0x20, // key length
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  opt.cryptMode = CRYPTMODE_OFF;
  opt.algorithmIndex = 1;
  int pagelen = SCSIInitSDEPage(&opt, buffer);
  REQUIRE(pagelen == sizeof(expected));
  REQUIRE(memcmp(buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("Enable encryption command", "[scsi]") {
  SCSIEncryptOptions opt;
  uint8_t buffer[1024] {};
  const uint8_t expected[] {
    0x00, 0x10, // page code
    0x00, 0x30, // page length
    0x40, // scope
    DEFAULT_CEEM << 6, // CEEM, CKOD, RDMC, et al.
    0x02, // encyption mode
    0x02, // decryption mode
    0x01, // algorithm index
    0x00, // key format
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved [8]
    0x00, 0x20, // key length
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  };

  opt.cryptMode = CRYPTMODE_ON;
  opt.algorithmIndex = 1;
  opt.cryptoKey = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  };
  opt.keyName = ""s;

  int pagelen = SCSIInitSDEPage(&opt, buffer);
  REQUIRE(pagelen == sizeof(expected));
  REQUIRE(memcmp(buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("Enable encryption command with options", "[scsi]") {
  SCSIEncryptOptions opt;
  uint8_t buffer[1024] {};
  const uint8_t expected[] {
    0x00, 0x10, // page code
    0x00, 0x30, // page length
    0x40, // scope
    DEFAULT_CEEM << 6 | 0x24, // CEEM, CKOD, RDMC, et al.
    0x02, // encyption mode
    0x02, // decryption mode
    0x01, // algorithm index
    0x00, // key format
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved [8]
    0x00, 0x20, // key length
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  };

  opt.rdmc = 2;
  opt.CKOD = true;
  opt.cryptMode = CRYPTMODE_ON;
  opt.algorithmIndex = 1;
  opt.cryptoKey = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  };
  opt.keyName = ""s;

  int pagelen = SCSIInitSDEPage(&opt, buffer);
  REQUIRE(pagelen == sizeof(expected));
  REQUIRE(memcmp(buffer, expected, sizeof(expected)) == 0);
}

TEST_CASE("Enable encryption command with key name", "[scsi]") {
  SCSIEncryptOptions opt;
  uint8_t buffer[1024] {};
  const uint8_t expected[] {
    0x00, 0x10, // page code
    0x00, 0x40, // page length
    0x40, // scope
    DEFAULT_CEEM << 6, // CEEM, CKOD, RDMC, et al.
    0x02, // encyption mode
    0x02, // decryption mode
    0x01, // algorithm index
    0x00, // key format
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved [8]
    0x00, 0x20, // key length
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    // KAD
    0x00, // type
    0x00, // authenticated
    0x00, 0x0c, // length
    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21,
  };

  opt.cryptMode = CRYPTMODE_ON;
  opt.algorithmIndex = 1;
  opt.cryptoKey = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  };
  opt.keyName = "Hello world!"s;

  int pagelen = SCSIInitSDEPage(&opt, buffer);
  REQUIRE(pagelen == sizeof(expected));
  REQUIRE(memcmp(buffer, expected, sizeof(expected)) == 0);
}

/**
 * Check the representation of the SPIN Device Encryption Status page
 * matches the values from the raw buffer. Input buffers were observed
 * from device traffic.
 *
 * This checks the SSP_DES structure layout matches the spec, especially
 * with regard to byte ordering and bitfield positions.
 */
TEST_CASE("Interpret device encryption status page", "[scsi]") {
  const uint8_t buffer[] {
    0x00, 0x20, // page code
    0x00, 0x24, // length
    0x42, // nexus = 2h, key scope = 2h
    0x02, // encryption mode
    0x02, // decryption mode
    0x01, // algorithm index
    0x00, 0x00, 0x00, 0x01, // key instance counter
    0x18, // parameters control = 1, VCELB = 1, CEEMS = 0, RDMD = 0
    0x00, // KAD format
    0x00, 0x00, // ADSK count
    0x00, 0x00, 0x00, 0x00, // reserved[8]
    0x00, 0x00, 0x00, 0x00,
    // KAD descriptor
    0x00, // descriptor type
    0x01, // authenticated
    0x00, 0x0c, // length
    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21,
  };

  SSP_DES page(reinterpret_cast<const SSP_PAGE_BUFFER*>(buffer));
  REQUIRE(BSSHORT(page.des.pageCode) == 0x20);
  REQUIRE(BSSHORT(page.des.length) == 0x24);
  REQUIRE(page.des.nexusScope == 2);
  REQUIRE(page.des.keyScope == 2);
  REQUIRE(page.des.encryptionMode == 2);
  REQUIRE(page.des.decryptionMode == 2);
  REQUIRE(page.des.algorithmIndex == 1);
  REQUIRE(BSLONG(page.des.keyInstance) == 1);
  REQUIRE(page.des.parametersControl == 1);
  REQUIRE(page.des.VCELB == 1);
  REQUIRE(page.des.CEEMS == 0);
  REQUIRE(page.des.RDMD == 0);

  REQUIRE(page.kads.size() == 1);
  REQUIRE(page.kads[0].authenticated == 1);
  REQUIRE(BSSHORT(page.kads[0].descriptorLength) == std::strlen("Hello world!"));
  REQUIRE(memcmp(page.kads[0].descriptor, "Hello world!", BSSHORT(page.kads[0].descriptorLength)) == 0);
}

TEST_CASE("Interpret next block encryption status page", "[scsi]") {
  const uint8_t buffer[] {
    0x00, 0x21, // page code
    0x00, 0x1c, // length
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x05, // compression status = 0, encryption status = 5h
    0x01, // algorithm index
    0x00, // EMES = 0, RDMDS = 0
    0x00, // KAD format
    // KAD descriptor
    0x00, // descriptor type
    0x01, // authenticated
    0x00, 0x0c, // length
    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21,
  };

  SSP_NBES page(reinterpret_cast<const SSP_PAGE_BUFFER*>(buffer));
  REQUIRE(BSSHORT(page.nbes.pageCode) == 0x21);
  REQUIRE(BSSHORT(page.nbes.length) == 0x1c);
  REQUIRE(page.nbes.compressionStatus == 0);
  REQUIRE(page.nbes.encryptionStatus == 5);
  REQUIRE(page.nbes.algorithmIndex == 1);
  REQUIRE(page.nbes.EMES == 0);
  REQUIRE(page.nbes.RDMDS == 0);

  REQUIRE(page.kads.size() == 1);
  REQUIRE(page.kads[0].authenticated == 1);
  REQUIRE(BSSHORT(page.kads[0].descriptorLength) == std::strlen("Hello world!"));
  REQUIRE(memcmp(page.kads[0].descriptor, "Hello world!", BSSHORT(page.kads[0].descriptorLength)) == 0);
}
