#include "server.hpp"

int main() {
  redisConnection = redisConnect("127.0.0.1", 6379);

  thread t1(updateSys);
  RunServer();
  return 0;
}

int predictIo(string size, string read, string write, string randread,
              string randwrite) {
  int ret = 0;
  string command = to_string(ceil(stoi(size) / 10240) * 10240) + ":" +
                   randread + ":" + randwrite + ":" + read + ":" + write + ":" +
                   to_string(ceil(cpulimit / 5) * 5) + ":" +
                   to_string(numjobs >= 1 ? 2 : 1);
  r_mutex.lock();
  redisReply *reply =
      (redisReply *)redisCommand(redisConnection, "get %s", command.c_str());
  r_mutex.unlock();
  if (reply == nullptr) {
    // redisFree(redisConnection);
    printf("error with reidis\n");
  } else if (reply->len == 0) {
    unordered_map<string, string> params = {
        {"x1", size},
        {"x2", randread},
        {"x3", randwrite},
        {"x4", read},
        {"x5", write},
        {"x6", to_string(cpulimit)},
        {"x7", to_string(numjobs >= 1 ? 2 : 1)},
    };
    ret = stoi(model.predict(params));
    r_mutex.lock();
    redisCommand(redisConnection, "set %s %s", command.c_str(),
                 to_string(ret).c_str());
    r_mutex.unlock();
  } else {
    printf("hit cache!\n");
    ret = atoi(reply->str);
  }
  cout << "suggest result: " << ret << endl;
  return ret;
}

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  RouteGuideImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

void updateSys() {
  // char *top_cmd = "top 1 -n 1 | head -n 4 | sed -n 4p | awk -F, "
  //                 "'{print $4}' | awk '{print $1 $2}'";
  char *top_cmd = "top -n 1 | head -n 3 | sed -n 3p | awk -F, "
                  "'{print $4}' | awk '{print $1 $2}'";
  char buffer[128];
  FILE *top_pipe;
  // char *io_cmd = "sed '/^$/!h;$!d;g' /tmp/io.txt | awk '{print $NF}'";
  // FILE *io_pipe;
  while (1) {
    top_pipe = popen(top_cmd, "r");
    std::string top_info = "";
    std::string io_info = "";
    if (!top_pipe)
      continue;
    // throw std::runtime_error("popen() failed!");
    try {
      fgets(buffer, 128, top_pipe);
      buffer[strlen(buffer) - 1] = 0;
      top_info += buffer;
    } catch (...) {
      pclose(top_pipe);
      throw;
    }
    pclose(top_pipe);
    // io_pipe = popen(io_cmd, "r");
    // if (!io_pipe)
    //   continue;
    // // throw std::runtime_error("io popen() failed!");
    // try {
    //   fgets(buffer, 128, io_pipe);
    //   buffer[strlen(buffer) - 1] = 0;
    //   io_info += buffer;
    // } catch (...) {
    //   pclose(io_pipe);
    //   throw;
    // }
    // pclose(io_pipe);

    std::smatch m;
    std::regex match("(([1-9]\\d*\\.\\d*)|(0\\.\\d*[1-9]\\d*))");
    std::regex_search(top_info, m, match);

    if (m.size() == 0) {
      cpulimit = 0;
    } else {
      cpulimit = std::stof(m.begin()->str());
    }
    // int temp = std::stof(io_info);
    // if (temp >= 80) {
    //   numjobs = 2;
    // } else {
    //   numjobs = 1;
    // }

    // std::cout << "in_info:" << numjobs << std::endl;
    // std::cout << "cpu:" << cpulimit << std::endl;
  }
  pclose(top_pipe);
  // pclose(io_pipe);
}
