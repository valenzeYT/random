#include "../include/interpreter.h"
#include <vector>
#include "../include/module_registry.h"
#include <cstdint>
#include <random>
#include <stdexcept>

namespace {
std::mt19937_64& randomEngine() {
    static std::mt19937_64 eng(std::random_device{}());
    return eng;
}
} // namespace

namespace random_lib {

void seed(std::uint64_t value) { randomEngine().seed(value); }

int randint(int minVal, int maxVal) {
    if (minVal > maxVal) {
        throw std::runtime_error("random.int[] expects min <= max");
    }
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(randomEngine());
}

double randfloat(double minVal, double maxVal) {
    if (minVal > maxVal) {
        throw std::runtime_error("random.float[] expects min <= max");
    }
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(randomEngine());
}

int randindex(int maxExclusive) {
    if (maxExclusive <= 0) {
        throw std::runtime_error("random.choice[] expects non-empty list");
    }
    std::uniform_int_distribution<int> dist(0, maxExclusive - 1);
    return dist(randomEngine());
}

} // namespace random_lib

extern "C" __declspec(dllexport)
void register_module() {
    module_registry::registerModule("random", [](Interpreter& interp) {
                    interp.registerModuleFunction("random", "seed", [&interp](const std::vector<Value>& args) -> Value {
                        interp.expectArity(args, 1, "random.seed");
                        random_lib::seed(static_cast<std::uint64_t>(interp.expectNumber(args[0], "random.seed expects number")));
                        return Value::fromNumber(0.0);
                    });
                    interp.registerModuleFunction("random", "int", [&interp](const std::vector<Value>& args) -> Value {
                        interp.expectArity(args, 2, "random.int");
                        int minVal = static_cast<int>(interp.expectNumber(args[0], "random.int expects number args"));
                        int maxVal = static_cast<int>(interp.expectNumber(args[1], "random.int expects number args"));
                        return Value::fromNumber(static_cast<double>(random_lib::randint(minVal, maxVal)));
                    });
                    interp.registerModuleFunction("random", "float", [&interp](const std::vector<Value>& args) -> Value {
                        interp.expectArity(args, 2, "random.float");
                        double minVal = interp.expectNumber(args[0], "random.float expects number args");
                        double maxVal = interp.expectNumber(args[1], "random.float expects number args");
                        return Value::fromNumber(random_lib::randfloat(minVal, maxVal));
                    });
                    interp.registerModuleFunction("random", "choice", [&interp](const std::vector<Value>& args) -> Value {
                        interp.expectArity(args, 1, "random.choice");
                        if (args[0].type != ValueType::LIST) {
                            throw std::runtime_error("random.choice expects list");
                        }
                        int idx = random_lib::randindex(static_cast<int>(args[0].list.size()));
                        return args[0].list[static_cast<size_t>(idx)];
                    });

    });
}
