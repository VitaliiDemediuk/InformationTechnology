// STl
#include <iostream>

// Local
#include "ServicesHolder.h"

// Core
#include "Database.h"
#include "StringUtils.h"
#include "CustomSaveLoadStrategy.h"
#include "SQLiteSaveLoadStrategy.h"
#include "MongoDbSaveLoadStrategy.h"

// boost
#include <boost/program_options.hpp>

// gRpc
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

struct CmdResult
{
    std::string server_address;
    std::unique_ptr<core::VirtualDatabase> db;
};

CmdResult processCmd(int argc, char** argv)
{
    CmdResult res;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("load-type", po::value<std::string>(), "set serializing type(custom, sqlite, mongodb)")
        ("server-ip", po::value<std::string>(), "set server ip")
        ("server-port", po::value<uint16_t>(), "set server port")
        ("database-name", po::value<std::string>(), "set database name")
        ("database-file", po::value<std::string>(), "set database file")
        ("database-ip", po::value<std::string>(), "set database ip")
        ("database-port", po::value<uint16_t>(), "set database ip");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    const bool haveServerIp = vm.contains("server-ip");
    const bool haveServerPort = vm.contains("server-port");

    if (haveServerIp && haveServerPort) {
        res.server_address = vm["server-ip"].as<std::string>() + ":" + std::to_string(vm["server-port"].as<uint16_t>());
    } else if (haveServerIp) {
        throw std::logic_error("Need set server port!");
    } else if (haveServerPort) {
        throw std::logic_error("Need set server ip!");
    } else {
        res.server_address = "localhost:50051";
    }

    const auto load_type = vm["load-type"].as<std::string>();

    std::unique_ptr<core::save_load::AbstractStrategy> saveLoadStrategy;
    if (load_type == "custom") {
        core::save_load::CustomFileInfo info{.filePath = core::utils::utf8ToWstring(vm["database-file"].as<std::string>())};
        saveLoadStrategy = std::make_unique<core::save_load::CustomFileStrategy>(std::move(info));
    } else if (load_type == "sqlite") {
        core::save_load::SQLiteInfo info{.filePath = core::utils::utf8ToWstring(vm["database-file"].as<std::string>())};
        saveLoadStrategy = std::make_unique<core::save_load::SQLiteStrategy>(std::move(info));
    } else if (load_type == "mongodb") {
        core::save_load::MongoDbInfo info{.dbName = core::utils::utf8ToWstring(vm["database-name"].as<std::string>()),
                                          .ipAddress = vm["database-ip"].as<std::string>(),
                                          .port = vm["database-port"].as<uint16_t>()};
        saveLoadStrategy = std::make_unique<core::save_load::MongoDbStrategy>(std::move(info));
    } else {
        throw std::logic_error("Invalid load-type!");
    }

    res.db = saveLoadStrategy->load();

    return res;
}

void RunServer(int argc, char** argv)
{
    auto [server_address, database] = processCmd(argc, argv);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    db_grpc_server::ServicesHolder servicesHolder{*database};
    servicesHolder.registerAll(builder);

    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv)
{
    RunServer(argc, argv);
    return 0;
}
