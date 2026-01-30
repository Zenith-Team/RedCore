#pragma once

#include <cafe.h>

#include <source_location>
#include <string_view>

namespace red {
    
    namespace internal {
        struct LogFormat {
            const char* fmt;
            std::source_location loc;
            
            consteval LogFormat(const char* s, std::source_location l = std::source_location::current())
                : fmt(s)
                , loc(l)
            { }
        };
        
        constexpr std::string_view basename(std::string_view path) {
            std::size_t last = 0;
        
            for (std::size_t i = 0; i < path.size(); ++i) {
                if (path[i] == '/' || path[i] == '\\') {
                    last = i + 1;
                }
            }
        
            return path.substr(last);
        }
    }
    
    template <typename... Args>
    void print(internal::LogFormat format, Args&&... args) {
        const auto& loc = format.loc;
        const auto& fmt = format.fmt;
        
        OSReport("[%s:%d] ", internal::basename(loc.file_name()).data(), loc.line());
        OSReport(fmt, args...);
    }
}
