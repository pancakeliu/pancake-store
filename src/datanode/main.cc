
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>

int main(const int argc, char **argv) {
    seastar::app_template app;

    app.add_options()
        ("version,v", boost::program_options::value<seastar::sstring>(), "pancake store version")
        ("rpc-port", boost::program_options::value<uint16_t>()->default_value(8888), "datanode rpc server listen port")
        ("http-port", boost::program_options::value<uint16_t>()->default_value(9999), "datanode http server listen port");

    return app.run(argc, argv, [] {
        return seastar::make_ready_future<>();
    });
}