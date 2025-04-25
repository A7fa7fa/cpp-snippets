#include "../logger.h"


static bool test_log() {

    std::string filename = "testfile";
    Logger &log = Logger::instance();
    log.set_log_level(Logger::Level::WARN);
    log.info("some message");
    return true;

}

int main() {
    try {
        test_log();
        std::cout << "Test with no errors" << std::endl;
        return EXIT_SUCCESS;
    } catch(const std::exception& e) {
        std::cerr << "Test with errors" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
