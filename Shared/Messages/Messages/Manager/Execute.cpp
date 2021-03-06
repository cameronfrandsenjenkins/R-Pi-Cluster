#include "Execute.hpp"

#include <array>
#include <memory>
#include <iostream>

namespace
{
#ifdef __unix||__APPLE__
  std::string exec(const char* cmd)
  {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        result += buffer.data();
    }
    return result;
  }
#else
  std::string exec(const char* cmd)
  {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) throw std::runtime_error("_popen() failed!");
    while (!feof(pipe.get()))
    {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        result += buffer.data();
    }
    return result;
  }

#endif
} // namespace

manager::Execute::Execute(int jobId,
                          std::string bldLoc,
                          Task task,
                          std::shared_ptr<std::mutex> pMutex,
                          std::shared_ptr<std::map<int, std::vector<Result>>> pResults)
  : QRunnable(), m_jobId(jobId), m_bldLoc(bldLoc), m_task(task), m_pMutex(pMutex), m_pResults(pResults)
{
}

manager::Execute::~Execute()
{
  // Anything?
}

void manager::Execute::run()
{
  auto result = exec(std::string(m_bldLoc + " " + m_task.toExecute).c_str());
  std::lock_guard<std::mutex> lock(*m_pMutex);
  (*m_pResults)[m_jobId].emplace_back(m_task, result);
  emit endTask(m_jobId);
}
