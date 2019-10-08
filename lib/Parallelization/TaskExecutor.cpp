#include "mull/Parallelization/TaskExecutor.h"

#include <mach/mach_init.h>
#include <mach/task.h>

namespace mull {

std::vector<int> taskBatches(size_t itemsCount, size_t tasks) {
  assert(itemsCount >= tasks);

  std::vector<int> result;

  int n = itemsCount;
  int m = tasks;

  int q = 0;
  int r = 0;
  int start = 0;
  int end = 0;
  int k = 0;
  int s = 0;

  for (k = 0; k < m; k++) {
    q = (n - k) / m;
    r = (n - k) % m;

    start = end;
    end = end + q + (r != 0);

    s = s + end - start;
    result.push_back(end - start);
  }

  assert(s == n);
  return result;
}

void printTimeSummary(MetricsMeasure measure) {
  Logger::info() << ". Finished in " << measure.duration()
                 << MetricsMeasure::precision() << ".\n";
}

struct Memory {
  unsigned long bytes;
  unsigned long kbytes;
  unsigned long mbytes;
  unsigned long gbytes;
};

struct Memory convertMemory(vm_size_t memory) {
  struct Memory mem;
  unsigned long *m = &mem.bytes;
  for (int i = 0; i < 4; i++) {
    m[i] = memory % 1024;
    memory /= 1024;
  }

  return mem;
}

void printMemorySummary() {
  struct task_basic_info t_info {};
  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

  if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO,
                                (task_info_t)&t_info, &t_info_count)) {
    return;
  }

  Memory mem = convertMemory(t_info.resident_size);
  if (mem.gbytes) {
    Logger::info() << mem.gbytes << "Gb ";
  }

  Logger::info() << mem.mbytes << "Mb\n";
}

} // namespace mull