#include "ClientManager.hpp"

manager::ClientManager::ClientManager(
  std::shared_ptr<TCPSenderClient> pServerClient,
  std::string database)
  : m_pSenderClient(pServerClient),
    m_database(database),
    m_executeManager(database),
    m_piManager(database),
    m_resultsManager(database)
{
  // What to do ?
}

manager::ClientManager::~ClientManager()
{
  // Delete files?
}

void manager::ClientManager::update(const msg::Update pMsg)
{
  // It would be nice if i could move these to m_piManager and m_piManager
  // New clients
  auto clients = pMsg.newclients();
  for (const auto& client : clients)
  {
    manager::Pi pi(client.ipaddress(),
                   client.port(),
                   client.username(),
                   client.password(),
                   client.priority(),
                   client.clientid());
    m_piManager.addPi(pi);
  }
  // lost pis
  auto lostPis = pMsg.lostclients();
  for (const auto& pi : lostPis)
  {
    m_piManager.removePi(pi);
  }
  // modified pis
  auto modified = pMsg.modifiedpis();
  for (const auto& mod : modified)
  {
    m_piManager.modifyPi(mod.id(), mod.field(), mod.value());
  }
  // new jobs
  auto jobs = pMsg.newjobs();
  for (const auto& job : jobs)
  {
    m_executeManager.addJob(manager::Job(job.id(),
                                         job.size(),
                                         job.priority(),
                                         job.taskperbundle(),
                                         job.giturl(),
                                         m_database));
  }
  // lost jobs
  auto lostJobs = pMsg.lostclients();
  for (const auto& job : lostJobs)
  {
    m_executeManager.removeJob(job);
  }
  // modified jobs
  auto modifiedJobs = pMsg.modifiedjobs();
  for (const auto& job : modifiedJobs)
  {
    m_executeManager.modifyJob(job.id(), job.field(), job.value());
  }
  // Results
  auto results = pMsg.results();
  for (const auto& result : results)
  {
    std::vector<Result> toImport;
    auto jobResults = result.results();
    int id(-1);
    for (const auto& jobr : jobResults)
    {
      auto task = jobr.task();
      id = task.jobid();
      toImport.emplace_back(Task(task.jobid(), task.pagenumber(), task.id(), task.toexecute()), jobr.result());
    }
    
    m_resultsManager.addResults(id, toImport);
  }
}

void manager::ClientManager::execute(msg::TaskMsg* pMsg)
{
  m_executeManager.addTasks(pMsg);
}