#include <catch2/catch_test_macros.hpp>

#include <thread>

#include <chrono>
#include <ostream>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>

#include "io/null_stream.hpp"
#include "io/time.hpp"
#include "util/pick.hpp"

namespace lf {

    class nlog {
    public:

        struct end_t {};
        static const end_t end;

        enum level { TRACE, DEBUG, INFO, WARN, ERROR, MUTE };
        static const char* level_names[MUTE + 1];

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

        class formatter {
        public:
            virtual ~formatter() = default;
            virtual std::string format(const message& msg) const = 0;
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

        std::unique_ptr<formatter> format;
        std::vector<std::unique_ptr<sink>> sinks;

    };

    std::ostream& operator<<(std::ostream& s, const nlog::level& l);
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

    std::ostream& operator<<(std::ostream& s, const nlog::level& l) {
        return s << pick_indexed_with_last_default(l, nlog::level_names);
    }

    std::ostream& operator<<(std::ostream& s, const nlog::end_t&) {
        nlog::stream* lp = dynamic_cast<nlog::stream*>(&s);
        if (lp != nullptr) {
            lp->send();
        }
        return s;
    }

    const nlog::end_t nlog::end;
    const char* nlog::level_names[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "UNKNOWN" };

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

    void nlog::send(const message& msg) {
        message fmt = msg;
        if (format) {
            fmt.text = format->format(fmt);
        }
        for (auto& s: sinks) {
            s->write(fmt);
        }
    }

    class simple_log_formatter: public nlog::formatter {
    public:
        std::string format(const nlog::message& msg) const override;
    };

    std::string simple_log_formatter::format(const nlog::message& msg) const {
        std::ostringstream s;
        s << "[" << format_date_time(msg.timestamp) << "][" << std::setw(5) << std::setfill(' ') << msg.level << "] " << msg.text << std::endl;
        return s.str();
    }

    class log_formatting_sink: public nlog::sink {
        std::unique_ptr<nlog::formatter> formatter;
        std::unique_ptr<nlog::sink> sink;
    public:
        log_formatting_sink(std::unique_ptr<nlog::formatter> formatter, std::unique_ptr<nlog::sink> sink);
        ~log_formatting_sink() override = default;
        void write(const nlog::message& msg) override;
    };

    log_formatting_sink::log_formatting_sink(std::unique_ptr<nlog::formatter> formatter, std::unique_ptr<nlog::sink> sink): 
        formatter(std::move(formatter)), 
        sink(std::move(sink)) 
    {
    }

    void log_formatting_sink::write(const nlog::message& msg) {
        nlog::message fmt = msg;
        fmt.text = formatter->format(fmt);
        sink->write(fmt);
    }

    class log_stream_sink: public nlog::sink {
        std::ostream& stream;
    public:
        log_stream_sink(std::ostream& stream);
        ~log_stream_sink() override = default;
        void write(const nlog::message& msg) override;
    };

    log_stream_sink::log_stream_sink(std::ostream& stream): 
        stream(stream) 
    {
    }

    void log_stream_sink::write(const nlog::message& msg) {
        stream << msg.text;
    }

}

TEST_CASE("nlog", "[nlog]") {
    
    using lf::nlog;

    nlog.format = std::make_unique<lf::simple_log_formatter>();
    nlog.sinks.push_back(std::make_unique<lf::log_stream_sink>(std::cout));


    if (nlog.info()) {
        nlog::stream s = nlog();
        s << "test" << std::endl << "msg" << nlog::end;
        s << "another info" << nlog::end;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    nlog.warn() && nlog() << "warn msg" << nlog::end;
    nlog.error() && nlog() << "error msg" << nlog::end;


}