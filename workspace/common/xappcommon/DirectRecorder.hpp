#ifndef DIRECT_RECORDER_HPP
#define DIRECT_RECORDER_HPP

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace app_common {
    /**
     * @brief File open mode for DirectRecorder
     */
    enum class DirectRecorderMode {
        CREATE_TRUNC_APPEND, // Create file, truncate if exists, then append
        APPEND_ONLY          // Open existing file for append only
    };

    /**
     * @brief A RAII-based file recorder using O_DIRECT for direct I/O
     *
     * This class provides a simple interface to:
     * - Open a file using system call open() with O_DIRECT flag
     * - Append string records to the file
     * - Read all records from the file
     *
     * Resource management follows RAII pattern - file is opened in constructor
     * and automatically closed in destructor.
     */
    class DirectRecorder {
    public:
        /**
         * @brief Construct a DirectRecorder and open the file with O_DIRECT
         *
         * @param filepath Path to the file to open
         * @param mode File open mode (CREATE_TRUNC_APPEND or APPEND_ONLY)
         * @throws std::runtime_error if file cannot be opened
         */
        explicit DirectRecorder(const std::string& filepath,
                                DirectRecorderMode mode = DirectRecorderMode::CREATE_TRUNC_APPEND)
            : _filepath(filepath), _mode(mode), _fd(-1) {
            open_file();
        }

        /**
         * @brief Destructor - closes the file automatically (RAII)
         */
        ~DirectRecorder() {
            close();
        }

        // Disable copy and move (mutex is not movable)
        DirectRecorder(const DirectRecorder&) = delete;

        DirectRecorder& operator=(const DirectRecorder&) = delete;

        DirectRecorder(DirectRecorder&&) = delete;

        DirectRecorder& operator=(DirectRecorder&&) = delete;

        /**
         * @brief Append a string record to the file (with newline)
         *
         * @param record The string record to append
         * @throws std::runtime_error if file is not open or write fails
         */
        void append(const std::string& record) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!is_open()) {
                throw std::runtime_error("DirectRecorder: file is not open");
            }

            std::string data = record + '\n';
            write_data(data);
        }

        /**
         * @brief Read all records from the file
         *
         * @return std::vector<std::string> Vector of all records (lines) in the file
         * @throws std::runtime_error if file cannot be read
         */
        std::vector<std::string> read_all() const {
            std::lock_guard<std::mutex> lock(_mutex);
            std::vector<std::string>    records;
            std::string                 content = read_content();

            size_t start = 0;
            size_t end = 0;
            while ((end = content.find('\n', start)) != std::string::npos) {
                records.push_back(content.substr(start, end - start));
                start = end + 1;
            }
            // Handle last line without newline
            if (start < content.size()) {
                records.push_back(content.substr(start));
            }
            return records;
        }

    private:
        /**
         * @brief Write data to file
         */
        void write_data(const std::string& data) {
            const char* ptr = data.c_str();
            size_t      len = data.size();
            size_t      written_total = 0;

            while (written_total < len) {
                ssize_t written = ::write(_fd, ptr + written_total, len - written_total);
                if (written < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    throw std::runtime_error("DirectRecorder: write failed: " + std::string(strerror(errno)));
                }
                written_total += written;
            }
        }

        /**
         * @brief Read the entire file content as a single string
         */
        std::string read_content() const {
            if (!is_open()) {
                throw std::runtime_error("DirectRecorder: file is not open");
            }

            struct stat st;
            if (::fstat(_fd, &st) < 0) {
                throw std::runtime_error("DirectRecorder: cannot stat file: " + _filepath + " - " + strerror(errno));
            }

            // Save current position and seek to beginning
            off_t current_pos = ::lseek(_fd, 0, SEEK_CUR);
            ::lseek(_fd, 0, SEEK_SET);

            std::string content;
            content.resize(st.st_size);

            ssize_t bytes_read = ::read(_fd, &content[0], st.st_size);

            // Restore position
            ::lseek(_fd, current_pos, SEEK_SET);

            if (bytes_read < 0) {
                throw std::runtime_error("DirectRecorder: read failed: " + std::string(strerror(errno)));
            }

            content.resize(bytes_read);
            return content;
        }

        /**
         * @brief Check if the file is currently open
         */
        bool is_open() const {
            return _fd >= 0;
        }

        /**
         * @brief Close the file
         */
        void close() {
            if (_fd >= 0) {
                ::close(_fd);
                _fd = -1;
            }
        }

        /**
         * @brief Open the file with O_DIRECT
         */
        void open_file() {
            int    flags = O_RDWR | O_DIRECT;
            mode_t file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // 0644

            switch (_mode) {
                case DirectRecorderMode::CREATE_TRUNC_APPEND:
                    flags |= O_CREAT | O_TRUNC;
                    break;
                case DirectRecorderMode::APPEND_ONLY:
                    flags |= O_APPEND | O_CREAT;
                    break;
            }

            _fd = ::open(_filepath.c_str(), flags, file_mode);
            if (_fd < 0) {
                throw std::runtime_error("DirectRecorder: cannot open file: " + _filepath + " - " + strerror(errno));
            }
        }

    private:
        std::string        _filepath;
        DirectRecorderMode _mode;
        int                _fd;
        mutable std::mutex _mutex;
    };

} // namespace app_common

#endif // DIRECT_RECORDER_HPP
