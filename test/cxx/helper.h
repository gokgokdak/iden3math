#pragma once

#include <iomanip>

#define ONE_THOUSAND    1000
#define ONE_MILLION     1000000
#define TEN_MILLION     10000000

#define GTEST_LOG(msg) \
do { \
    auto* info = ::testing::UnitTest::GetInstance()->current_test_info(); \
    std::cout << "[ RUN      ] " << info->test_suite_name() << "." << info->name() << ": " << msg << std::endl; \
} while(0)

#define PERFORMANCE_TEST(rounds, code) \
size_t __perf_rounds = rounds; \
auto __perf_start = std::chrono::high_resolution_clock::now(); \
for (size_t _ = 0; _ < __perf_rounds; ++_) { \
    code \
} \
auto __perf_end = std::chrono::high_resolution_clock::now(); \
auto __perf_duration = std::chrono::duration_cast<std::chrono::microseconds>(__perf_end - __perf_start).count(); \
GTEST_LOG("Time cost total: " << __perf_duration << "us, each: " \
    << std::setiosflags(std::ios::fixed) << std::setprecision(8) \
    << (static_cast<double>(__perf_duration) / static_cast<double>(__perf_rounds)) << "us");

#define SCOPED_TRACE_POINT(p, base) SCOPED_TRACE(std::string(#p) + "(x=" + p.x.str(base) + ", y=" + p.y.str(base) + ")");

#define SCOPED_TRACE_BIGINT(num, base) SCOPED_TRACE(std::string(#num) + BigInt(num).str(base));
