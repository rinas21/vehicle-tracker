#include "Simulator.h"
#include "profiles/ProfileRegistry.h"

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Options {
    std::string profile = "vehicle";
    int durationSeconds = 12;
    std::string csvPath;
    bool quiet = false;
    bool noFaults = false;
    bool help = false;
    bool seedProvided = false;
    std::uint32_t seed = 0u;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n\n"
              << "Options:\n"
              << "  --profile <name>   vehicle | industrial_motor | water_pump "
                 "(default: vehicle)\n"
              << "  --duration <sec>   simulation duration in seconds "
                 "(default: 12)\n"
              << "  --csv <path>       CSV output path "
                 "(default: <profile>_simulation.csv)\n"
              << "  --seed <number>    override the profile's random seed\n"
              << "  --no-faults        disable all fault injections\n"
              << "  --quiet            suppress console samples, keep alerts off too\n"
              << "  --help             show this message\n\n"
              << "This program is a software simulation only. No hardware is used.\n";
}

std::string valueAfter(const std::string& argument, const std::string& option,
                       int& index, int argc, char** argv) {
    const std::string prefix = option + "=";
    if (argument.compare(0, prefix.size(), prefix) == 0) {
        return argument.substr(prefix.size());
    }
    if (index + 1 >= argc) {
        throw std::invalid_argument("Missing value for " + option);
    }
    return argv[++index];
}

Options parseOptions(int argc, char** argv) {
    Options options;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--help" || argument == "-h") {
            options.help = true;
        } else if (argument == "--quiet") {
            options.quiet = true;
        } else if (argument == "--no-faults") {
            options.noFaults = true;
        } else if (argument == "--profile" ||
                   argument.compare(0, 10, "--profile=") == 0) {
            options.profile = valueAfter(argument, "--profile", index, argc, argv);
        } else if (argument == "--duration" ||
                   argument.compare(0, 11, "--duration=") == 0) {
            const std::string value =
                valueAfter(argument, "--duration", index, argc, argv);
            options.durationSeconds = std::atoi(value.c_str());
            if (options.durationSeconds <= 0) {
                throw std::invalid_argument("Duration must be a positive integer");
            }
        } else if (argument == "--csv" || argument.compare(0, 6, "--csv=") == 0) {
            options.csvPath = valueAfter(argument, "--csv", index, argc, argv);
        } else if (argument == "--seed" || argument.compare(0, 7, "--seed=") == 0) {
            const std::string value =
                valueAfter(argument, "--seed", index, argc, argv);
            options.seed = static_cast<std::uint32_t>(std::strtoul(value.c_str(), nullptr, 0));
            options.seedProvided = true;
        } else {
            throw std::invalid_argument("Unknown option: " + argument);
        }
    }

    return options;
}

} // namespace

int main(int argc, char** argv) {
    using namespace vehiclesim;

    try {
        const Options options = parseOptions(argc, argv);

        if (options.help) {
            printUsage(argv[0]);
            return 0;
        }

        SystemProfile profile = makeProfile(options.profile);

        if (options.seedProvided) {
            profile.baseSeed = options.seed;
        }
        if (options.noFaults) {
            profile.faults.clear();
        }

        const std::string csvPath = options.csvPath.empty()
                                        ? profile.name + "_simulation.csv"
                                        : options.csvPath;

        if (!options.quiet) {
            std::cout << "Profile: " << profile.name << '\n'
                      << "Sensors: " << profile.sensors.size() << '\n'
                      << "Duration: " << options.durationSeconds << "s\n"
                      << "Faults: " << (profile.faults.empty() ? "disabled" : "enabled") << '\n'
                      << "CSV: " << csvPath << "\n\n";
        }

        Simulator simulator(profile, csvPath, !options.quiet);
        simulator.run(Milliseconds(options.durationSeconds * 1000));
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        printUsage(argv[0]);
        return 1;
    }
}
