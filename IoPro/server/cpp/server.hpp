#pragma ones

#include <cPMML.h>
#include <cstdio>
#include <cstring>
#include <hiredis/hiredis.h>
#include <iostream>
#include <math.h>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

#include "IoProject.grpc.pb.h"
#include "IoProject.pb.h"
#include <atomic>
#include <grpcpp/grpcpp.h>
#include <regex>
#include <unordered_map>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using namespace IoProjectGrpc;
using namespace std;

int cpulimit = 100;
// int numjobs = 1;
atomic<int> numjobs(0);
mutex r_mutex;
redisContext *redisConnection;
const std::string model_filepath("../../models/pmmlDecisionTree.pmml");
const cpmml::Model model(model_filepath);

void updateSys();
void RunServer();
int predictIo(string size, string read, string write, string randread,
              string randwrite);

class RouteGuideImpl final
    : public IoProjectGrpc::IoProjectGrpcRouteGuide::Service {
public:
  RouteGuideImpl(){};
  Status GetPrediction(ServerContext *context, const PredictRequest *request,
                       PredictReply *reply) override {
    cout << "new request from " << request->pid() << endl;
    string size = request->size();
    string read = request->read();
    string write = request->write();
    string randread = request->randread();
    string randwrite = request->randwrite();
    int result = predictIo(size, read, write, randread, randwrite);
    reply->set_prediction(google::protobuf::int32(result));
    numjobs++;
    return Status::OK;
  };
  Status CallFinish(ServerContext *context, const EmptyRequest *request,
                    EmptyReply *reply) override {
    numjobs--;
    return Status::OK;
  }
};
