#include "Job.hpp"
#include <fstream>
#include <iostream>

manager::JobManager::JobManager(int id,
                                int size,
                                int pri,
                                int taskpb,
                                std::string gitUrl,
                                std::string database,
                                std::string cloneScript)
  : m_myId(id),
    m_priority(pri),
    m_taskPerBundle(taskpb),
    m_gitUrl(gitUrl),
    m_taskManager(id, size),
    m_resultManager(id),
    m_database(database),
    m_name(),
    m_toExec()
{
  m_name = gitUrl;
  int spot;
  while ((spot = m_name.find("/")) != std::string::npos)
  {
    m_name.erase(0, spot + 1);
  }
  m_name.erase(m_name.size() - 4);
  system(std::string("dir").c_str());
  system(std::string(cloneScript + " " + m_name + " " + m_gitUrl).c_str());
  m_taskManager.populateFields("Jobs/" + m_name + "/TaskLists.txt");
  system(std::string("mkdir " + m_database + std::to_string(m_myId)).c_str());
  std::ifstream fin("Jobs/" + m_name + "/" + m_name + "/Exec.txt");
  std::getline(fin, m_toExec);
}

manager::JobManager::~JobManager()
{
  // can remove folder
}

void manager::JobManager::addResults(std::vector<manager::Result> results)
{
  m_resultManager.addResult(results);
}

std::vector<manager::Task> manager::JobManager::getTasks(int amount)
{
  return m_taskManager.getNextTasks(amount);
}

std::string manager::JobManager::getName()
{
  return m_name;
}

std::string manager::JobManager::getExec()
{
  return m_toExec;
}

int manager::JobManager::getPriority()
{
  return m_priority;
}

int manager::JobManager::getTasksPerBundle()
{
  return m_taskPerBundle;
}
