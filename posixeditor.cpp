//
// Created by nick on 7/7/20.
//

#include "posixeditor.h"
#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/process/async.hpp>

namespace ba = boost::asio;
namespace bp = boost::process;

#include <iostream>
#define LOG(x) std::clog

#include <map>
#include <list>
#include <thread>

class ProcessManager { // ugh naming smell
    using error_code = boost::system::error_code;
private:
    ba::io_service _service;
    boost::optional<ba::io_service::work> _keep{_service};
    boost::process::group _group;
    std::thread io_thread;

    struct patchProcess : std::enable_shared_from_this<patchProcess> {
        using ptr = std::shared_ptr<patchProcess>;
        static ptr start(std::string command, std::vector<std::string> args, ProcessManager& mgr) {
            ptr p(new patchProcess(std::move(command), std::move(args), mgr));
            p->output_read_loop();
            return p;
        }

        boost::optional<std::string> getline() {
            std::lock_guard<std::mutex> lk(_mx);
            std::string s;
            if (has_newline(_output.data()) && std::getline(std::istream(&_output), s))
                return s;
            return boost::none;
        }

        void write(std::string message) {
            std::lock_guard<std::mutex> lk(_mx);
            _input_bufs.push_back({false, std::move(message)});

            if (_input_bufs.size() == 1)
                input_write_loop();
        }

        void close_stdin() {
            std::lock_guard<std::mutex> lk(_mx);
            if (_input_bufs.empty()) {
                _strand.post([this, self=shared_from_this()] { _stdin.close(); });
            } else {
                _input_bufs.push_back({true, {}});
            }
        }

        bool is_running() { return _process.running(); }

    private:
        patchProcess(std::string command, std::vector<std::string> args, ProcessManager& mgr)
                : _strand(mgr._service),
                  _stdout(mgr._service), _stdin(mgr._service),
                  _process(command, args, mgr._group, bp::std_out > _stdout, bp::std_in < _stdin, mgr._service)
        { }

        void output_read_loop() {
            ba::async_read_until(_stdout, pending_output, "\n", _strand.wrap([this, self=shared_from_this()](error_code ec, size_t /*transferred*/) {
                if (!ec) {
                    std::lock_guard<std::mutex> lk(_mx);
                    std::ostream(&_output) << &pending_output;
                    output_read_loop();
                }
            }));
        }

        void input_write_loop() { // assumes _mx locked
            if (!_input_bufs.empty()) {
                auto& msg = _input_bufs.front();
                if (msg.eof) {
                    _strand.post([this, self=shared_from_this()] { _stdin.close(); });
                } else {
                    ba::async_write(_stdin, ba::buffer(_input_bufs.front().pay_load),
                                    _strand.wrap([this, self=shared_from_this()](error_code ec, size_t /*transferred*/) {
                                        std::lock_guard<std::mutex> lk(_mx);
                                        _input_bufs.pop_front();
                                        if (!ec)
                                            input_write_loop();
                                    }));
                }
            }
        }

        ba::io_service::strand _strand; // thread-safe

        // strand-local
        bp::async_pipe _stdout, _stdin;
        bp::child _process;
        ba::streambuf pending_output;

        // mutex protected
        std::mutex mutable _mx;
        struct out_message { bool eof; std::string pay_load; };
        std::list<out_message> _input_bufs; // iterator stability again!
        ba::streambuf _output;

        // static helpers
        template <typename T>
        static bool has_newline(T buffer) {
            return std::find(buffers_begin(buffer), buffers_end(buffer), '\n') != buffers_end(buffer);
        }
    };

    using Map = std::map<std::string, patchProcess::ptr>; // iterator stability required!
    Map processList;

    void eventloop() {
        for(;;) try {
                if (!_service.run()) break;
            } catch(std::exception const& e) {
                LOG(error) << "Exception in handler: " << e.what() << "\n";
            }
    }
public:
    ProcessManager() : io_thread([this] { eventloop(); }) { }

    ~ProcessManager() {
        status(__FUNCTION__);
        _keep.reset();
        io_thread.join();
        status(__FUNCTION__);
    }

    void status(std::string const& caption = "Status") const {
        for (auto& p : processList) {
            LOG(info) << caption << ": '" << p.first << "' is " << (p.second->is_running()? "still running":"done") << "\n";
        }
    }

    patchProcess::ptr addNew(std::string name, std::string command, std::vector<std::string> args) {
        auto pit = processList.find(name);
        if (pit != processList.end()) {
            if (pit->second->is_running()) {
                LOG(error) << "Process already running ('" << name << "')\n";
                return {};
            }
            // TODO make sure process cleaned up etc.
        }
        LOG(info) << "Creating process  " << name << "\n";
        return processList[name] = patchProcess::start(std::move(command), std::move(args), *this);
    }
};

int PosixEditor::launch(const std::string &editor_name) {
    ProcessManager pm;
    auto editor_p = pm.addNew(editor_name, settings()["exec"], arguments());
    boost::optional<std::string> l;

    while ((l = editor_p->getline()) || editor_p->is_running()) {
        if (l.is_initialized()) {
            std::cout << "ls: " << std::quoted(*l) << std::endl;
            l.reset();
        }
    }
    return 0;
}

int PosixEditor::launch() {
    return launch(editor_name_);
}

PosixEditor::PosixEditor(std::string editor_name, std::string settings_file) :
Editor(editor_name, settings_file) {
    //std::cout << "Posix Editor Created" << '\n';
}
PosixEditor::~PosixEditor() noexcept {}

size_t PosixEditor::start_editor(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait) {
    return 0;
}

std::shared_ptr<PosixEditor> create_editor() {
    return std::make_shared<PosixEditor>(std::string("Editor"), std::string("editor.json"));
}
