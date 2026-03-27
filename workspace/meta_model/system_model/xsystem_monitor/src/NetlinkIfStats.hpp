#ifndef _NETLINK_IFSTATS_HPP_
#define _NETLINK_IFSTATS_HPP_
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class NetlinkIfStats {
public:
    NetlinkIfStats() {
        sock_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (sock_ < 0) {
            perror("socket");
        } else {
            struct sockaddr_nl addr {};
            addr.nl_family = AF_NETLINK;
            if (bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("bind");
                close(sock_);
                sock_ = -1;
            }
        }
    }

    ~NetlinkIfStats() {
        if (sock_ >= 0) {
            close(sock_);
        }
    }

    // 获取指定接口的统计数据
    bool getStats(const std::string &ifname, uint64_t &rxBytes, uint64_t &txBytes) {
        if (sock_ < 0)
            return false;

        char buffer[8192] = {0};

        struct nlmsghdr * nlh = (struct nlmsghdr *)buffer;
        struct ifinfomsg *ifi = (struct ifinfomsg *)(buffer + sizeof(*nlh));

        nlh->nlmsg_len = NLMSG_LENGTH(sizeof(*ifi));
        nlh->nlmsg_type = RTM_GETLINK;
        nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
        nlh->nlmsg_seq = 1;
        nlh->nlmsg_pid = getpid();

        ifi->ifi_family = AF_PACKET;

        struct sockaddr_nl kernel {};
        kernel.nl_family = AF_NETLINK;

        struct iovec  iov = {buffer, nlh->nlmsg_len};
        struct msghdr msg {};
        msg.msg_name = &kernel;
        msg.msg_namelen = sizeof(kernel);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        if (sendmsg(sock_, &msg, 0) < 0) {
            perror("sendmsg");
            return false;
        }

        int len = recv(sock_, buffer, sizeof(buffer), 0);
        if (len < 0) {
            perror("recv");
            return false;
        }

        return parseStats((struct nlmsghdr *)buffer, len, ifname, rxBytes, txBytes);
    }

private:
    int sock_ = -1;

    bool parseStats(struct nlmsghdr *nlh, int len, const std::string &ifname, uint64_t &rx, uint64_t &tx) {
        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_DONE)
                break;
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                std::cerr << "Netlink error.\n";
                return false;
            }

            struct ifinfomsg *ifi = (ifinfomsg *)NLMSG_DATA(nlh);
            int               attrlen = nlh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi));
            struct rtattr *   attr = IFLA_RTA(ifi);

            std::string               name;
            struct rtnl_link_stats64 *stats64 = nullptr;

            for (; RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen)) {
                if (attr->rta_type == IFLA_IFNAME) {
                    name = (char *)RTA_DATA(attr);
                } else if (attr->rta_type == IFLA_STATS64) {
                    stats64 = (struct rtnl_link_stats64 *)RTA_DATA(attr);
                }
            }

            if (name == ifname && stats64) {
                rx = stats64->rx_bytes;
                tx = stats64->tx_bytes;
                return true;
            }
        }
        return false;
    }
};

#endif
