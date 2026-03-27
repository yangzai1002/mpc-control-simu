#ifndef ARCHIVE_UTILS_HPP
#define ARCHIVE_UTILS_HPP

#include <archive.h>
#include <archive_entry.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "log/log.h"

namespace app_common {
    class ArchiveUtils {
    public:
        ArchiveUtils() = delete;

    public:
        static std::int32_t extract(const std::string &filename, const std::string &dest_dir,
                                    bool clear_dest_dir = false) {
            archive *             a = nullptr;
            archive *             ext = nullptr;
            archive_entry *       entry = nullptr;
            std::int32_t          flags = 0;
            std::int32_t          r = 0;
            std::filesystem::path write_path;
            std::string           error;

            if (clear_dest_dir) {
                std::filesystem::remove_all(dest_dir);
            }

            // 解压选项
            flags = ARCHIVE_EXTRACT_TIME;  // 保留时间戳
            flags |= ARCHIVE_EXTRACT_PERM; // 保留权限
            flags |= ARCHIVE_EXTRACT_ACL;  // 保留 ACL
            flags |= ARCHIVE_EXTRACT_FFLAGS;

            a = archive_read_new();
            if (nullptr == a) {
                error = "cannot create archive for read";
                goto error1;
            }
            archive_read_support_format_all(a);
            archive_read_support_filter_all(a);

            if (0 != archive_read_open_filename(a, filename.c_str(), 10240)) {
                error = archive_error_string(a);
                goto error2;
            }

            ext = archive_write_disk_new();
            if (nullptr == ext) {
                error = "cannot create archive for write";
                goto error3;
            }
            archive_write_disk_set_options(ext, flags);
            archive_write_disk_set_standard_lookup(ext);

            while (true) {
                r = archive_read_next_header(a, &entry);
                if (r == ARCHIVE_EOF) {
                    break;
                } else if (r == ARCHIVE_WARN) {
                    AINFO << "[WARN] " << filename << ", " << archive_error_string(a);
                } else if (r < ARCHIVE_WARN) {
                    error = archive_error_string(a);
                    goto error4;
                }

                // 设置目标路径
                write_path = std::filesystem::path(dest_dir) / archive_entry_pathname(entry);
                archive_entry_set_pathname(entry, write_path.c_str());

                r = archive_write_header(ext, entry);
                if (r == ARCHIVE_WARN) {
                    AINFO << "[WARN] " << filename << ", " << archive_error_string(a);
                } else if (r < ARCHIVE_OK) {
                    error = archive_error_string(ext);
                    goto error4;
                }

                if (archive_entry_size(entry) > 0) {
                    const void *buff;
                    size_t      size;
                    la_int64_t  offset;

                    while (true) {
                        r = archive_read_data_block(a, &buff, &size, &offset);
                        if (r == ARCHIVE_EOF) {
                            break;
                        } else if (r == ARCHIVE_WARN) {
                            AINFO << "[WARN] " << filename << ", " << archive_error_string(a);
                        } else if (r < ARCHIVE_WARN) {
                            error = archive_error_string(a);
                            goto error4;
                        }

                        r = archive_write_data_block(ext, buff, size, offset);
                        if (r = ARCHIVE_WARN) {
                            AINFO << "[WARN] " << filename << ", " << archive_error_string(ext);
                        } else if (r < ARCHIVE_WARN) {
                            error = archive_error_string(ext);
                            goto error4;
                        }
                    }
                }
            }

            return 0;

        error4:
            archive_write_close(ext);
            archive_write_free(ext);
        error3:
            archive_read_close(a);
        error2:
            archive_read_free(a);
        error1:
            throw std::runtime_error(error);
        }
    };
} // namespace app_common
#endif
