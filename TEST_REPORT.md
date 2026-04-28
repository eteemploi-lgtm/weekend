# Rapport de test CommuniCAN

## Environnement

- OS :
- Terminal :
- Compilateur :
- Dossier :
- Date :

## Test moteur

Commande :

```bash
make -f Makefile.diagnostic motor
```

Résultat :
- [ ] OK
- [ ] erreur compilation
- [ ] erreur linking
- [ ] erreur runtime

Première erreur :

```text

```

## Test power/contacteurs

Commande :

```bash
make -f Makefile.diagnostic power
```

Résultat :
- [ ] OK
- [ ] erreur compilation
- [ ] erreur linking
- [ ] erreur runtime

Première erreur :

```text

```

## Test gouvernes

Commande :

```bash
make -f Makefile.diagnostic gouv
```

Résultat :
- [ ] OK
- [ ] erreur compilation
- [ ] erreur linking
- [ ] erreur runtime

Première erreur :

```text

```

## Test capteurs

Commande :

```bash
make -f Makefile.diagnostic sensors
```

Résultat :
- [ ] OK
- [ ] erreur compilation
- [ ] erreur linking
- [ ] erreur runtime

Première erreur :

```text

```

## Test système

Commande :

```bash
make -f Makefile.diagnostic system
```

Résultat :
- [ ] OK
- [ ] erreur compilation
- [ ] erreur linking
- [ ] erreur runtime

Première erreur :

```text

```




cmake_minimum_required(VERSION 3.16)
project(CommuniCAN_Diagnostics LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LEGACY_INC "${CMAKE_CURRENT_SOURCE_DIR}/external/legacy_c/inc")
set(LEGACY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/external/legacy_c/src")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${LEGACY_INC}")

set(LEGACY_C_SOURCES
    ${LEGACY_SRC}/dataCommon.c
    ${LEGACY_SRC}/commDevice.c
    ${LEGACY_SRC}/moteur.c
    ${LEGACY_SRC}/contacteurs.c
    ${LEGACY_SRC}/gouvernes.c
    ${LEGACY_SRC}/DataValidation.c
    ${LEGACY_SRC}/ReadDataCan.c
)

set(COMMON_CPP_SOURCES
    src/adapters/legacy_context.cpp
    src/adapters/legacy_motor_adapter.cpp
    src/adapters/legacy_gouv_adapter.cpp
    src/adapters/legacy_power_adapter.cpp
    src/adapters/legacy_sensor_adapter.cpp
    src/managers/global_state.cpp
    src/services/motor_service.cpp
    src/services/gouv_service.cpp
    src/services/power_service.cpp
    src/services/sensor_service.cpp
    src/services/system_state_machine.cpp
    src/models/json_serialization.cpp
    src/diagnostics/legacy_context_probe.cpp
    src/diagnostics/motor_test_runner.cpp
    src/diagnostics/power_test_runner.cpp
    src/diagnostics/gouv_test_runner.cpp
    src/diagnostics/sensor_test_runner.cpp
)

add_executable(test_motor src/test_main_motor.cpp ${COMMON_CPP_SOURCES} ${LEGACY_C_SOURCES})
add_executable(test_power src/test_main_power.cpp ${COMMON_CPP_SOURCES} ${LEGACY_C_SOURCES})
add_executable(test_gouv src/test_main_gouv.cpp ${COMMON_CPP_SOURCES} ${LEGACY_C_SOURCES})
add_executable(test_sensors src/test_main_sensors.cpp ${COMMON_CPP_SOURCES} ${LEGACY_C_SOURCES})
add_executable(test_system src/test_main_system.cpp ${COMMON_CPP_SOURCES} ${LEGACY_C_SOURCES})

if (WIN32)
    target_link_libraries(test_motor ws2_32)
    target_link_libraries(test_power ws2_32)
    target_link_libraries(test_gouv ws2_32)
    target_link_libraries(test_sensors ws2_32)
    target_link_libraries(test_system ws2_32)
endif()




# CommuniCAN — diagnostic tests

## Objectif

Ce pack sert à tester demain par blocs :

1. moteur
2. contacteurs / power
3. gouvernes
4. capteurs
5. système complet

Le but n'est pas que tout passe immédiatement. Le but est d'obtenir des erreurs propres et isolées.

## Commandes recommandées

Depuis `server/` :

```bash
make -f Makefile.diagnostic motor
build_diag/test_motor.exe 1000
```

Puis :

```bash
make -f Makefile.diagnostic power
make -f Makefile.diagnostic gouv
make -f Makefile.diagnostic sensors
make -f Makefile.diagnostic system
```

Ne lance pas `all` au début.

## Erreurs probables

### undefined reference

Cause :
- fichier `.c` manquant dans `LEGACY_C`
- nom de fonction différent
- header C pas protégé pour C++

### conflicting declaration with C linkage

Cause :
- `extern "C"` mal placé

### stDataCommon inconnu

Cause :
- mauvais chemin include
- `dataCommon.h` pas trouvé
- nom réel différent

## À m'envoyer demain

Pour chaque test :

```text
TEST:
COMMANDE:
RESULTAT:
PREMIÈRE ERREUR:
CAPTURE:
```

Envoie seulement la première erreur bloquante.



#ifndef COMMUNICAN_GOUV_TEST_RUNNER_HPP
#define COMMUNICAN_GOUV_TEST_RUNNER_HPP
#include <cstdint>
#include "services/gouv_service.hpp"
#include "diagnostics/test_result.hpp"
class GouvTestRunner {
public:
    explicit GouvTestRunner(GouvService& gouv);
    TestResult runConfigureOnly();
    TestResult runHomingOnly();
    TestResult runOperationalOnly();
    TestResult runMoveRaw(std::int32_t rawPosition);
private:
    GouvService& gouv_;
};
#endif


#ifndef COMMUNICAN_LEGACY_CONTEXT_PROBE_HPP
#define COMMUNICAN_LEGACY_CONTEXT_PROBE_HPP
#include "adapters/legacy_context.hpp"
#include "diagnostics/test_result.hpp"
class LegacyContextProbe {
public:
    static TestResult run(LegacyContext& context);
};
#endif


#ifndef COMMUNICAN_MOTOR_TEST_RUNNER_HPP
#define COMMUNICAN_MOTOR_TEST_RUNNER_HPP
#include <cstdint>
#include "services/motor_service.hpp"
#include "diagnostics/test_result.hpp"
class MotorTestRunner {
public:
    explicit MotorTestRunner(MotorService& motor);
    TestResult runConfigureOnly();
    TestResult runStartStopOnly();
    TestResult runFullTest(std::int32_t rpm);
private:
    MotorService& motor_;
};
#endif


#ifndef COMMUNICAN_POWER_TEST_RUNNER_HPP
#define COMMUNICAN_POWER_TEST_RUNNER_HPP
#include "services/power_service.hpp"
#include "diagnostics/test_result.hpp"
class PowerTestRunner {
public:
    explicit PowerTestRunner(PowerService& power);
    TestResult runStartupOnly();
    TestResult runShutdownOnly();
    TestResult runStartupShutdown();
private:
    PowerService& power_;
};
#endif


#ifndef COMMUNICAN_SENSOR_TEST_RUNNER_HPP
#define COMMUNICAN_SENSOR_TEST_RUNNER_HPP
#include "services/sensor_service.hpp"
#include "diagnostics/test_result.hpp"
class SensorTestRunner {
public:
    explicit SensorTestRunner(SensorService& sensors);
    TestResult runRefreshOnly();
private:
    SensorService& sensors_;
};
#endif


#ifndef COMMUNICAN_TEST_LOGGER_HPP
#define COMMUNICAN_TEST_LOGGER_HPP
#include <iostream>
#include <string>
namespace diag {
inline void title(const std::string& m){ std::cout << "\n========== " << m << " ==========\n"; }
inline void step(const std::string& m){ std::cout << "[STEP] " << m << "\n"; }
inline void ok(const std::string& m){ std::cout << "[OK] " << m << "\n"; }
inline void fail(const std::string& m){ std::cerr << "[FAIL] " << m << "\n"; }
inline void warn(const std::string& m){ std::cout << "[WARN] " << m << "\n"; }
inline void info(const std::string& m){ std::cout << "[INFO] " << m << "\n"; }
}
#endif


#ifndef COMMUNICAN_TEST_RESULT_HPP
#define COMMUNICAN_TEST_RESULT_HPP
#include <string>
struct TestResult {
    bool ok = false;
    std::string message;
    static TestResult success(const std::string& msg){ return {true, msg}; }
    static TestResult failure(const std::string& msg){ return {false, msg}; }
};
#endif



#include <cstdlib>
#include "adapters/legacy_context.hpp"
#include "adapters/legacy_gouv_adapter.hpp"
#include "managers/global_state.hpp"
#include "services/gouv_service.hpp"
#include "diagnostics/gouv_test_runner.hpp"
#include "diagnostics/legacy_context_probe.hpp"
#include "diagnostics/test_logger.hpp"
#include "config/channels.hpp"

int main(int argc, char** argv) {
    int rawPosition = 0;
    if (argc >= 2) rawPosition = std::atoi(argv[1]);

    diag::title("CommuniCAN gouvernes diagnostic");
    LegacyContext legacy;
    GlobalState globalState;

    auto ctx = LegacyContextProbe::run(legacy);
    if (!ctx.ok) { diag::fail(ctx.message); return 1; }

    LegacyGouvAdapter gouvAdapter(legacy, CAN_CHANNEL_0);
    GouvService gouvService(gouvAdapter, globalState);
    GouvTestRunner runner(gouvService);

    auto r1 = runner.runConfigureOnly();
    if (!r1.ok) { diag::fail(r1.message); return 1; }

    auto r2 = runner.runHomingOnly();
    if (!r2.ok) { diag::fail(r2.message); return 1; }

    auto r3 = runner.runOperationalOnly();
    if (!r3.ok) { diag::fail(r3.message); return 1; }

    auto r4 = runner.runMoveRaw(rawPosition);
    if (!r4.ok) { diag::fail(r4.message); return 1; }

    diag::ok("Gouvernes diagnostic completed");
    return 0;
}


#include <cstdlib>
#include "adapters/legacy_context.hpp"
#include "adapters/legacy_motor_adapter.hpp"
#include "managers/global_state.hpp"
#include "services/motor_service.hpp"
#include "diagnostics/motor_test_runner.hpp"
#include "diagnostics/legacy_context_probe.hpp"
#include "diagnostics/test_logger.hpp"
#include "config/channels.hpp"

int main(int argc, char** argv) {
    int rpm = 1000;
    if (argc >= 2) rpm = std::atoi(argv[1]);

    diag::title("CommuniCAN motor diagnostic");
    LegacyContext legacy;
    GlobalState globalState;

    auto ctx = LegacyContextProbe::run(legacy);
    if (!ctx.ok) { diag::fail(ctx.message); return 1; }

    LegacyMotorAdapter motorAdapter(legacy, CAN_CHANNEL_1);
    MotorService motorService(motorAdapter, globalState);
    MotorTestRunner runner(motorService);

    auto result = runner.runFullTest(rpm);
    if (!result.ok) { diag::fail(result.message); return 1; }

    diag::ok(result.message);
    return 0;
}


#include "adapters/legacy_context.hpp"
#include "adapters/legacy_power_adapter.hpp"
#include "managers/global_state.hpp"
#include "services/power_service.hpp"
#include "diagnostics/power_test_runner.hpp"
#include "diagnostics/legacy_context_probe.hpp"
#include "diagnostics/test_logger.hpp"

int main() {
    diag::title("CommuniCAN power diagnostic");
    LegacyContext legacy;
    GlobalState globalState;

    auto ctx = LegacyContextProbe::run(legacy);
    if (!ctx.ok) { diag::fail(ctx.message); return 1; }

    LegacyPowerAdapter powerAdapter(legacy);
    PowerService powerService(powerAdapter, globalState);
    PowerTestRunner runner(powerService);

    auto result = runner.runStartupShutdown();
    if (!result.ok) { diag::fail(result.message); return 1; }

    diag::ok(result.message);
    return 0;
}




#include "adapters/legacy_context.hpp"
#include "adapters/legacy_sensor_adapter.hpp"
#include "managers/global_state.hpp"
#include "services/sensor_service.hpp"
#include "diagnostics/sensor_test_runner.hpp"
#include "diagnostics/legacy_context_probe.hpp"
#include "diagnostics/test_logger.hpp"

int main() {
    diag::title("CommuniCAN sensors diagnostic");
    LegacyContext legacy;
    GlobalState globalState;

    auto ctx = LegacyContextProbe::run(legacy);
    if (!ctx.ok) { diag::fail(ctx.message); return 1; }

    LegacySensorAdapter sensorAdapter(legacy);
    SensorService sensorService(sensorAdapter, globalState);
    SensorTestRunner runner(sensorService);

    auto result = runner.runRefreshOnly();
    if (!result.ok) { diag::fail(result.message); return 1; }

    diag::ok(result.message);
    return 0;
}



#include <cstdlib>

#include "adapters/legacy_context.hpp"
#include "adapters/legacy_motor_adapter.hpp"
#include "adapters/legacy_gouv_adapter.hpp"
#include "adapters/legacy_power_adapter.hpp"
#include "adapters/legacy_sensor_adapter.hpp"
#include "managers/global_state.hpp"
#include "services/motor_service.hpp"
#include "services/gouv_service.hpp"
#include "services/power_service.hpp"
#include "services/sensor_service.hpp"
#include "services/system_state_machine.hpp"
#include "models/json_serialization.hpp"
#include "diagnostics/test_logger.hpp"
#include "config/channels.hpp"

int main(int argc, char** argv) {
    int rpm = 1000;
    if (argc >= 2) rpm = std::atoi(argv[1]);

    diag::title("CommuniCAN full system diagnostic");

    LegacyContext legacy;
    GlobalState globalState;

    LegacyMotorAdapter motorAdapter(legacy, CAN_CHANNEL_1);
    LegacyGouvAdapter gouvAdapter(legacy, CAN_CHANNEL_0);
    LegacyPowerAdapter powerAdapter(legacy);
    LegacySensorAdapter sensorAdapter(legacy);

    MotorService motorService(motorAdapter, globalState);
    GouvService gouvService(gouvAdapter, globalState);
    PowerService powerService(powerAdapter, globalState);
    SensorService sensorService(sensorAdapter, globalState);

    SystemStateMachine machine(powerService, motorService, gouvService, sensorService, globalState);

    if (!machine.startup()) {
        diag::fail("System startup failed");
        std::cout << toJson(globalState.snapshot()) << "\n";
        return 1;
    }

    if (!machine.startRun(rpm)) {
        diag::fail("System run failed");
        std::cout << toJson(globalState.snapshot()) << "\n";
        return 1;
    }

    if (!machine.stop()) {
        diag::fail("System stop failed");
        std::cout << toJson(globalState.snapshot()) << "\n";
        return 1;
    }

    diag::ok("Full system diagnostic completed");
    std::cout << toJson(globalState.snapshot()) << "\n";
    return 0;
}




CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Iexternal/legacy_c/inc
BUILD := build_diag

LEGACY_C := \
	external/legacy_c/src/dataCommon.c \
	external/legacy_c/src/commDevice.c \
	external/legacy_c/src/moteur.c \
	external/legacy_c/src/contacteurs.c \
	external/legacy_c/src/gouvernes.c \
	external/legacy_c/src/DataValidation.c \
	external/legacy_c/src/ReadDataCan.c

COMMON_CPP := \
	src/adapters/legacy_context.cpp \
	src/adapters/legacy_motor_adapter.cpp \
	src/adapters/legacy_gouv_adapter.cpp \
	src/adapters/legacy_power_adapter.cpp \
	src/adapters/legacy_sensor_adapter.cpp \
	src/managers/global_state.cpp \
	src/services/motor_service.cpp \
	src/services/gouv_service.cpp \
	src/services/power_service.cpp \
	src/services/sensor_service.cpp \
	src/services/system_state_machine.cpp \
	src/models/json_serialization.cpp \
	src/diagnostics/legacy_context_probe.cpp \
	src/diagnostics/motor_test_runner.cpp \
	src/diagnostics/power_test_runner.cpp \
	src/diagnostics/gouv_test_runner.cpp \
	src/diagnostics/sensor_test_runner.cpp

.PHONY: all clean motor power gouv sensors system

all: motor power gouv sensors system

$(BUILD):
	mkdir $(BUILD)

motor: $(BUILD)
	$(CXX) $(CXXFLAGS) src/test_main_motor.cpp $(COMMON_CPP) $(LEGACY_C) -lws2_32 -o $(BUILD)/test_motor.exe

power: $(BUILD)
	$(CXX) $(CXXFLAGS) src/test_main_power.cpp $(COMMON_CPP) $(LEGACY_C) -lws2_32 -o $(BUILD)/test_power.exe

gouv: $(BUILD)
	$(CXX) $(CXXFLAGS) src/test_main_gouv.cpp $(COMMON_CPP) $(LEGACY_C) -lws2_32 -o $(BUILD)/test_gouv.exe

sensors: $(BUILD)
	$(CXX) $(CXXFLAGS) src/test_main_sensors.cpp $(COMMON_CPP) $(LEGACY_C) -lws2_32 -o $(BUILD)/test_sensors.exe

system: $(BUILD)
	$(CXX) $(CXXFLAGS) src/test_main_system.cpp $(COMMON_CPP) $(LEGACY_C) -lws2_32 -o $(BUILD)/test_system.exe

clean:
	rmdir /S /Q $(BUILD)




# CommuniCAN — diagnostic tests

## Objectif

Ce pack sert à tester demain par blocs :

1. moteur
2. contacteurs / power
3. gouvernes
4. capteurs
5. système complet

Le but n'est pas que tout passe immédiatement. Le but est d'obtenir des erreurs propres et isolées.

## Commandes recommandées

Depuis `server/` :

```bash
make -f Makefile.diagnostic motor
build_diag/test_motor.exe 1000
```

Puis :

```bash
make -f Makefile.diagnostic power
make -f Makefile.diagnostic gouv
make -f Makefile.diagnostic sensors
make -f Makefile.diagnostic system
```

Ne lance pas `all` au début.

## Erreurs probables

### undefined reference

Cause :
- fichier `.c` manquant dans `LEGACY_C`
- nom de fonction différent
- header C pas protégé pour C++

### conflicting declaration with C linkage

Cause :
- `extern "C"` mal placé

### stDataCommon inconnu

Cause :
- mauvais chemin include
- `dataCommon.h` pas trouvé
- nom réel différent

## À m'envoyer demain

Pour chaque test :

```text
TEST:
COMMANDE:
RESULTAT:
PREMIÈRE ERREUR:
CAPTURE:
```

Envoie seulement la première erreur bloquante.

