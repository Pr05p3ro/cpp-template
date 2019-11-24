#ifndef CPP_TEMPLATE_LOG_H
#define CPP_TEMPLATE_LOG_H

namespace game {

class Log {
public:
    static void error(const char *pMessage, ...);

    static void warn(const char *pMessage, ...);

    static void info(const char *pMessage, ...);

    static void debug(const char *pMessage, ...);
};

} // namespace game

#endif //CPP_TEMPLATE_LOG_H
