#ifndef PI_H
#define PI_H

#include <string>
#include "Messages/Manager/Tasks.hpp"
#include <chrono>

namespace manager
{
  struct ModifiedPi
  {
    int id;
    std::string field;
    std::string value;
  };

  class Pi
  {
  public:
    Pi();
    Pi(std::string ip, int port, std::string user, std::string pass, int priority, int id);
    Pi(const Pi& pi);
    ~Pi();

    void replaceTasks(std::vector<manager::Task> compelted, std::vector<manager::Task> tasks);
    std::string toString();
    static Pi fromString(std::string);
    bool operator==(const Pi&);
    bool operator!=(const Pi&);

    std::string getIpAddress() const ;
    int getPort() const;
    std::string getUsername() const;
    std::string getPassword() const;
    int getPriority() const;
    int getClientId() const;
    std::vector<manager::Task> getTasks() const;
    int getThreads() const;
    void decrementPriority();
    void changePriority(int priority);
    void changeThreads(int threads);
    int getAmountToSend() const;
    void updateAck();
    std::chrono::steady_clock::time_point getlastCom() const;
    Pi makeCopy();
    void setAddress(std::string ip);
    void setPort(int port);
    void setUsername(std::string user);
    void setPassword(std::string pass);
    void setPriority(int pr);
    void setClientId(int id);
    void setThreads(int thr);

  private:  
    /** Ip Address of the client */
    std::string m_ipAddress;
    /** port of the clinet */
    int m_port;
    /** username on the pi */
    std::string m_username;
    /** password for the username */
    std::string m_password;
    /** What priority in line */
    int m_priority;
    /** The clients Id*/
    int m_clientId;
    /** Amount of threads */
    int m_threads;
    /** Last Ack from pi*/
    std::chrono::steady_clock::time_point m_lastCom;
    /** Vector of current tasks*/
    std::vector<manager::Task> m_tasks;
  };
} // namespace manager

#endif
