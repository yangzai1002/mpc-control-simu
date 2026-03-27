#include "EventTriggeredRecorder.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace event_recorder;

/**
 * @brief 模拟故障数据生成
 */
ara::adsfi::FaultData generateFaultData(bool has_fault, const std::string& fault_code) {
    ara::adsfi::FaultData fault_data;

    if (has_fault) {
        ara::adsfi::FaultInfo info;
        info.code = fault_code;
        info.timestamp = std::chrono::duration<double>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        info.desc = "Test fault";
        info.from = "test_module";
        fault_data.fault_info.push_back(info);
        fault_data.fault_level = 3;
    }

    return fault_data;
}

/**
 * @brief 测试场景1：故障触发
 */
void testFaultTrigger() {
    std::cout << "\n========== Test 1: Fault Trigger ==========\n";

    // 创建配置
    EventTriggeredRecorder::Config config;
    config.topics = {"/test_topic1", "/test_topic2"};
    config.output_path = "/tmp/test_recorder/fault_trigger";
    config.buffer_size_kb = 256;
    config.duration_sec = 10;  // 10秒测试
    config.quota_mb = 1024;
    config.clean_threshold_mb = 800;
    config.fault_codes = {"202", "301"};

    // 创建录包器
    EventTriggeredRecorder recorder(config);
    recorder.start();

    std::cout << "Recorder started, recording for 30 seconds...\n";

    // 模拟10Hz调用
    auto start = std::chrono::steady_clock::now();
    int count = 0;

    while (true) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        if (elapsed >= 30) {
            break;  // 运行30秒
        }

        // 在第5秒和第25秒触发故障
        bool has_fault = (elapsed == 5 || elapsed == 25);
        std::string fault_code = (elapsed == 5) ? "202" : "301";

        auto fault_data = generateFaultData(has_fault, fault_code);
        recorder.updateFaultData(fault_data);

        if (has_fault) {
            std::cout << "  [" << elapsed << "s] Fault " << fault_code << " injected\n";
        }

        // 无手动触发
        recorder.updateManualTrigger(0.0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 10Hz
        count++;
    }

    std::cout << "Test completed. Total updates: " << count << "\n";
    std::cout << "Check files in: " << config.output_path << "\n";

    recorder.stop();
}

/**
 * @brief 测试场景2：手动触发
 */
void testManualTrigger() {
    std::cout << "\n========== Test 2: Manual Trigger ==========\n";

    EventTriggeredRecorder::Config config;
    config.topics = {"/test_topic1", "/test_topic2"};
    config.output_path = "/tmp/test_recorder/manual_trigger";
    config.buffer_size_kb = 256;
    config.duration_sec = 10;
    config.quota_mb = 1024;
    config.clean_threshold_mb = 800;
    config.fault_codes = {"202"};

    EventTriggeredRecorder recorder(config);
    recorder.start();

    std::cout << "Recorder started, testing manual trigger...\n";

    auto start = std::chrono::steady_clock::now();
    double manual_signal = 0.0;
    bool trigger_sent = false;

    while (true) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        if (elapsed >= 30) {
            break;
        }

        // 在第8秒触发手动录包（上升沿）
        if (elapsed >= 8 && elapsed < 13) {
            manual_signal = 1.0;  // 保持5秒
            if (!trigger_sent) {
                std::cout << "  [" << elapsed << "s] Manual trigger activated (rising edge)\n";
                trigger_sent = true;
            }
        } else {
            if (manual_signal == 1.0) {
                std::cout << "  [" << elapsed << "s] Manual trigger deactivated (falling edge)\n";
            }
            manual_signal = 0.0;
        }

        // 无故障
        auto fault_data = generateFaultData(false, "");
        recorder.updateFaultData(fault_data);
        recorder.updateManualTrigger(manual_signal);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Test completed. Check files in: " << config.output_path << "\n";
    recorder.stop();
}

/**
 * @brief 测试场景3：磁盘空间管理
 */
void testDiskSpaceManagement() {
    std::cout << "\n========== Test 3: Disk Space Management ==========\n";

    EventTriggeredRecorder::Config config;
    config.topics = {"/test_topic1"};
    config.output_path = "/tmp/test_recorder/disk_space";
    config.buffer_size_kb = 256;
    config.duration_sec = 5;  // 短周期
    config.quota_mb = 50;     // 小配额
    config.clean_threshold_mb = 40;
    config.fault_codes = {"202"};

    EventTriggeredRecorder recorder(config);
    recorder.start();

    std::cout << "Recorder started, generating multiple files...\n";
    std::cout << "Quota: " << config.quota_mb << " MB, Threshold: "
              << config.clean_threshold_mb << " MB\n";

    // 运行较长时间，触发清理
    auto start = std::chrono::steady_clock::now();

    while (true) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        if (elapsed >= 60) {  // 运行1分钟
            break;
        }

        // 每20秒触发一次，保证有些文件被保留
        bool has_fault = (elapsed % 20 == 0 && elapsed > 0);
        auto fault_data = generateFaultData(has_fault, "202");
        recorder.updateFaultData(fault_data);

        if (has_fault) {
            std::cout << "  [" << elapsed << "s] Fault triggered\n";
        }

        recorder.updateManualTrigger(0.0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Test completed. Check cleanup logs and files in: "
              << config.output_path << "\n";
    recorder.stop();
}

/**
 * @brief 测试场景4：去重机制
 */
void testDeduplication() {
    std::cout << "\n========== Test 4: Deduplication ==========\n";

    EventTriggeredRecorder::Config config;
    config.topics = {"/test_topic1"};
    config.output_path = "/tmp/test_recorder/deduplication";
    config.buffer_size_kb = 256;
    config.duration_sec = 15;  // 15秒一个文件
    config.quota_mb = 1024;
    config.clean_threshold_mb = 800;
    config.fault_codes = {"202"};

    EventTriggeredRecorder recorder(config);
    recorder.start();

    std::cout << "Recorder started, testing deduplication...\n";

    auto start = std::chrono::steady_clock::now();

    while (true) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        if (elapsed >= 40) {
            break;
        }

        // 持续发送故障码（从第5秒到第20秒）
        bool has_fault = (elapsed >= 5 && elapsed <= 20);
        auto fault_data = generateFaultData(has_fault, "202");
        recorder.updateFaultData(fault_data);

        if (elapsed == 5) {
            std::cout << "  [" << elapsed << "s] Fault 202 starts (should trigger once)\n";
        }
        if (elapsed == 20) {
            std::cout << "  [" << elapsed << "s] Fault 202 ends\n";
        }

        recorder.updateManualTrigger(0.0);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Test completed. Check logs for deduplication behavior.\n";
    std::cout << "Expected: Only trigger once at 5s, not repeatedly.\n";
    recorder.stop();
}

int main(int argc, char** argv) {
    std::cout << "Event Triggered Recorder - Test Suite\n";
    std::cout << "======================================\n";

    try {
        if (argc > 1) {
            std::string test = argv[1];
            if (test == "fault") {
                testFaultTrigger();
            } else if (test == "manual") {
                testManualTrigger();
            } else if (test == "disk") {
                testDiskSpaceManagement();
            } else if (test == "dedup") {
                testDeduplication();
            } else {
                std::cout << "Usage: " << argv[0] << " [fault|manual|disk|dedup]\n";
                return 1;
            }
        } else {
            // 运行所有测试
            testFaultTrigger();
            std::this_thread::sleep_for(std::chrono::seconds(2));

            testManualTrigger();
            std::this_thread::sleep_for(std::chrono::seconds(2));

            testDiskSpaceManagement();
            std::this_thread::sleep_for(std::chrono::seconds(2));

            testDeduplication();
        }

        std::cout << "\n======================================\n";
        std::cout << "All tests completed!\n";

    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
