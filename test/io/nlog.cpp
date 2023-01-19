#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <ostream>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>

#include "io/null_stream.hpp"
#include "io/time.hpp"

namespace lf {

    class log_stream;
    class log_sink;
    struct log_message;

    class nlog {
    public:

        struct end_t {};
        static const end_t end;

        enum level { TRACE, DEBUG, INFO, WARN, ERROR, MUTE };

        struct message {
            enum level level;
            std::chrono::system_clock::time_point timestamp;
            std::string text = "";
        };

        class stream: public std::ostream {
            nlog& ref;
            message msg;
            std::stringbuf buf;
        public:
            stream(nlog& ref, const message& msg);
            void send();
        };

        class sink {
        public:
            virtual ~sink() = default;
            virtual void write(const message& msg) = 0;
        };

        bool trace();
        bool debug();
        bool info();
        bool warn();
        bool error();
        bool with(level level);
        
        level min_level = INFO;
        static thread_local level cur_level;

        stream operator()();
        void send(const message& msg);

        std::vector<std::unique_ptr<sink>> sink;

    };

    std::ostream& operator<<(std::ostream& s, const nlog::end_t&);

    class nlog nlog;

    nlog::stream::stream(nlog& ref, const nlog::message& msg): std::ostream(msg.level >= ref.min_level ? static_cast<std::streambuf*>(&buf) : &nullout.nullbuf), ref(ref), msg(msg) {
    }

    void nlog::stream::send() {
        if (buf.view().empty()) {
            return;
        }
        msg.text = buf.str();
        ref.send(msg);
        buf.str("");
    }

    std::ostream& operator<<(std::ostream& s, const nlog::end_t&) {
        nlog::stream* lp = dynamic_cast<nlog::stream*>(&s);
        if (lp != nullptr) {
            lp->send();
        }
        return s;
    }

    const nlog::end_t nlog::end;
    thread_local nlog::level nlog::cur_level = nlog::INFO;

    bool nlog::trace() { return with(nlog::TRACE); }
    bool nlog::debug() { return with(nlog::DEBUG); }
    bool nlog::info() { return with(nlog::INFO); }
    bool nlog::warn() { return with(nlog::WARN); }
    bool nlog::error() { return with(nlog::ERROR); }
    bool nlog::with(nlog::level level) {
        if (level < min_level) {
            return false;
        }
        cur_level = level;
        return true;
    }

    nlog::stream nlog::operator()() {
        level msg_level = cur_level;
        cur_level = INFO;
        return nlog::stream(*this, nlog::message { .level = msg_level, .timestamp = std::chrono::system_clock::now() });
    }

    void nlog::send(message& msg) {

        for (auto& s: sink) {
            s->write(msg);
        }
    }

    class log_formatter {
    public:
        virtual std::string format(const nlog::message& msg) const = 0;
    };

    class simple_log_formatter: public log_formatter {
    public:
        std::string format(const nlog::message& msg) const override;
    };

    std::string simple_log_formatter::format(const nlog::message& msg) const {
        std::ostringstream s;
        s << "[" << format_date_time(msg.timestamp) << "][" << std::setw(5) << std::setfill(' ') << msg.level << "] " << msg.text << std::endl;
        return s.str();
    }

    class log_stream_sink: public nlog::sink {
        std::ostream& stream;
    public:
        log_stream_sink(std::ostream& stream);
        ~log_stream_sink() override = default;
        void write(const nlog::message& msg) override;
    };

    log_stream_sink::log_stream_sink(std::ostream& stream): stream(stream) {}
    void log_stream_sink::write(const nlog::message& msg) {
        stream << msg.text;
    }

}

TEST_CASE("nlog", "[nlog]") {
    
    using lf::nlog;

    nlog.sink.push_back(std::make_unique<lf::log_stream_sink>(std::cout));

    if (nlog.info()) {
        nlog::stream s = nlog();
        s << "test" << std::endl << "msg" << nlog::end << "another info" << nlog::end;
    }


}