#include <stdio.h>
#include <memory.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "spi/spi.h"

namespace avos {
namespace driver {

SpiUtil::SpiUtil() {
  fd_ = open("/dev/spidev0.0", O_RDWR);
}

SpiUtil::~SpiUtil() {
  close(fd_);
}

void SpiUtil::Init(uint8_t bits, uint32_t speed_hz, uint32_t mode) {
  bits_ = bits;
  speed_ = speed_hz;
  mode_ = mode;

  if (fd_ < 0) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't open device");
  }
  if (ioctl(fd_, SPI_IOC_WR_MODE, &mode_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't set spi mode");
  }
  if (ioctl(fd_, SPI_IOC_RD_MODE, &mode_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't get spi mode");
  }
  if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't set bits per word");
  }
  if (ioctl(fd_, SPI_IOC_RD_BITS_PER_WORD, &bits_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't get bits per word");
  }
  if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't set max speed hz");
  }
  if (ioctl(fd_, SPI_IOC_RD_MAX_SPEED_HZ, &speed_) == -1) {
    // abort();
    // ROS_ERROR("[SpiUtil] can't get max speed hz");
  }

  printf("spi mode: 0x%x\n", mode_);
  printf("bits per word: %d\n", bits_);
  printf("max speed: %d Hz (%d KHz)\n", speed_, speed_ / 1000);
}

char SpiUtil::Transfer(uint32_t const *tx, uint32_t const *rx, uint16_t len) {
  spi_ioc_transfer spi_tranfer;
  memset(&spi_tranfer, 0, sizeof(spi_ioc_transfer));
  spi_tranfer.tx_buf = (unsigned long)tx; //TBD
  spi_tranfer.rx_buf = (unsigned long)rx; //TBD
  spi_tranfer.len = len; //TBD
  spi_tranfer.speed_hz = speed_;
  spi_tranfer.bits_per_word = bits_;

  if (mode_ & SPI_TX_QUAD)       spi_tranfer.tx_nbits = 4;
  else if (mode_ & SPI_TX_DUAL)  spi_tranfer.tx_nbits = 2;

  if (mode_ & SPI_RX_QUAD)       spi_tranfer.rx_nbits = 4;
  else if (mode_ & SPI_RX_DUAL)  spi_tranfer.rx_nbits = 2;

  if (!(mode_ & SPI_LOOP)) {
    if (mode_ & (SPI_TX_QUAD | SPI_TX_DUAL))       spi_tranfer.rx_buf = 0;
    else if (mode_ & (SPI_RX_QUAD | SPI_RX_DUAL))  spi_tranfer.tx_buf = 0;
  }

  int ret = ioctl(fd_, SPI_IOC_MESSAGE(1), &spi_tranfer);
  if (ret < 1) {
    // ROS_ERROR("[SpiUtil] can't send/receive spi message");
    //  abort();
  }
  return static_cast<char>(ret);
}


} // namespace aovs
} // namespace driver