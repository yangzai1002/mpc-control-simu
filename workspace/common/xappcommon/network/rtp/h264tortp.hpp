#ifndef RTP_H264TORTP_H
#define RTP_H264TORTP_H

#include <cstdint>
#include <cstdio>
#include <cstring>

#define NAL_MAX 4000000
#define H264 96
#define G711 8

struct rtp_header_t {
    /* little-endian */
    /* byte 0 */
    uint8_t csrc_len : 4;  /* bit: 0~3 */
    uint8_t extension : 1; /* bit: 4 */
    uint8_t padding : 1;   /* bit: 5*/
    uint8_t version : 2;   /* bit: 6~7 */
    /* byte 1 */
    uint8_t payload_type : 7; /* bit: 0~6 */
    uint8_t marker : 1;       /* bit: 7 */
    /* bytes 2, 3 */
    uint16_t seq_no;
    /* bytes 4-7 */
    uint32_t timestamp;
    /* bytes 8-11 */
    uint32_t ssrc;
} __attribute__((packed)); /* 12 bytes */

#define RTP_OFFSET_SSRC offsetof(rtp_header_t, ssrc)

struct nalu_header_t {
    /* byte 0 */
    uint8_t type : 5;      /* bit: 0~4 */
    uint8_t nri : 2;       /* bit: 5~6 */
    uint8_t f : 1;         /* bit: 7 */
} __attribute__((packed)); /* 1 bytes */

struct fu_indicator_t {
    /* byte 0 */
    uint8_t type : 5;
    uint8_t nri : 2;
    uint8_t f : 1;
} __attribute__((packed)); /* 1 bytes */

struct fu_header_t {
    /* byte 0 */
    uint8_t type : 5;
    uint8_t r : 1;
    uint8_t e : 1;
    uint8_t s : 1;
} __attribute__((packed)); /* 1 bytes */

struct raw_header_t {
    std::uint16_t packet_len;
    std::uint32_t ralative_packet_ms;
} __attribute__((packed));

#endif
