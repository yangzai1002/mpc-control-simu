#pragma once

#include <iostream>

#include <iostream>
#include <string>
#include <string.h>
#include <memory>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

namespace avos {
namespace driver {

class SpiUtil {
public:
  SpiUtil();
  ~SpiUtil();
  void Init(uint8_t bits, uint32_t speed_hz, uint32_t mode);
  char Transfer(uint32_t const *tx, uint32_t const *rx, uint16_t len);

private:
  int fd_;
  uint32_t mode_;
  uint32_t speed_;
  uint8_t bits_;

};

} // namespace aovs
} // namespace driver