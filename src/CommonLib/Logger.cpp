#include "Logger.h"

namespace loggable
{
    namespace debugger
    {

        void Logger::init(const String &file)
        {
            try
            {
                log4cpp::PropertyConfigurator::configure(file);
            }
            catch (log4cpp::ConfigureFailure& f)
            {
                std::cout << "Config error:" << f.what() << std::endl;
                std::cout << "Add a log config to avoid this error msg..." << std::endl;
                return;
            }
            logger_ = &log4cpp::Category::getInstance(std::string("debugger"));
        }

        log4cpp::Category *Logger::getLogger()
        {
            assert(logger_ != NULL);
            return logger_;
        }
    }
}
