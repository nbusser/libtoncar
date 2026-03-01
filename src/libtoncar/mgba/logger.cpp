#include "mgba/logger.h"

#include <panic.h>
#include <registers/registers.h>
#include <sys/iosupport.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace toncar::mgba {

namespace {

class MgbaDebugRegister : Register<MgbaDebugRegister, uint16_t, 0x00FFF780> {
 public:
  static MgbaDebugRegister& Enable() { return Set(0xC0DE); }
  static MgbaDebugRegister& Disable() { return Set(0x0); }
  static bool IsEnabled() { return Get() == 0x1DEA; }
};

class MgbaDebugFlagsRegister : Register<MgbaDebugFlagsRegister, uint16_t, 0x00FFF700> {
 public:
  static void SetLevel(Logger::Level level) {
    // TODO: implement a commit-based Register to use .Or(kMask) instead.
    Set(static_cast<uint16_t>(level) | kMask);
  }

 private:
  static constexpr uint16_t kMask{0x100};
};

constexpr size_t kDebugStringMaxSize{0x100};

// TODO: create API for buffer like this
const auto kRegDebugString = reinterpret_cast<char*>(0x4FFF600);

template <Logger::Level level>
ssize_t MgbaConsoleWrite(struct _reent* r __attribute__((unused)),
                         void* fd __attribute__((unused)),
                         const char* ptr,
                         size_t len) {
  GBA_ASSERT(len <= kDebugStringMaxSize);
  strncpy(kRegDebugString, ptr, len);
  MgbaDebugFlagsRegister::SetLevel(level);
  return static_cast<ssize_t>(len);
}

template <Logger::Level level>
devoptab_t BuildDevoptTab(std::string_view name) {
  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage): controlled here
  return devoptab_t{.name = name.data(),
                    .structSize = 0,
                    .open_r = nullptr,
                    .close_r = nullptr,
                    .write_r = MgbaConsoleWrite<level>,
                    .read_r = nullptr,
                    .seek_r = nullptr,
                    .fstat_r = nullptr,
                    .stat_r = nullptr,
                    .link_r = nullptr,
                    .unlink_r = nullptr,
                    .chdir_r = nullptr,
                    .rename_r = nullptr,
                    .mkdir_r = nullptr,
                    .dirStateSize = 0,
                    .diropen_r = nullptr,
                    .dirreset_r = nullptr,
                    .dirnext_r = nullptr,
                    .dirclose_r = nullptr,
                    .statvfs_r = nullptr,
                    .ftruncate_r = nullptr,
                    .fsync_r = nullptr,
                    .deviceData = nullptr,
                    .chmod_r = nullptr,
                    .fchmod_r = nullptr,
                    .rmdir_r = nullptr,
                    .lstat_r = nullptr,
                    .utimes_r = nullptr};
}

}  // namespace

Logger::Logger()
    : dotab_stdout_{BuildDevoptTab<Logger::Level::Error>("mgba_stdout"),},
      dotab_stderr_{
          BuildDevoptTab<Logger::Level::Error>("mgba_stderr"),
      } {
  MgbaDebugRegister::Enable();
  GBA_ASSERT(MgbaDebugRegister::IsEnabled());
  // TODO: check if it should be cleared on dtor
  devoptab_list[STD_OUT] = &dotab_stdout_;
  devoptab_list[STD_ERR] = &dotab_stderr_;
}

Logger::~Logger() { MgbaDebugRegister::Disable(); }

// NOLINTNEXTLINE(cert-dcl50-cpp)
void Logger::Log(Level level, const char* ptr, ...) {
  EnsureInstanceExists();

  va_list args;
  va_start(args, ptr);
  static_cast<void>(vsnprintf(kRegDebugString, 0x100, ptr, args));
  va_end(args);
  MgbaDebugFlagsRegister::SetLevel(level);

  GBA_ASSERT(level != Level::Fatal);
}

}  // namespace toncar::mgba
