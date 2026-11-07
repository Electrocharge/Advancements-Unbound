#pragma once
#include <dlfcn.h>

namespace mc {
class ClientInstance;
class Level;
namespace world { class Actor; }
}

namespace ll::service {
inline mc::Level* getLevel() {
    using GetLevelFn = void* (*)();
    static auto fn = reinterpret_cast<GetLevelFn>(dlsym(RTLD_DEFAULT, "ll_service_get_level"));
    return fn ? reinterpret_cast<mc::Level*>(fn()) : nullptr;
}
inline mc::ClientInstance* getClientInstance() {
    using GetClientFn = void* (*)();
    static auto fn = reinterpret_cast<GetClientFn>(dlsym(RTLD_DEFAULT, "ll_service_get_client_instance"));
    return fn ? reinterpret_cast<mc::ClientInstance*>(fn()) : nullptr;
}
inline mc::Level* getMultiPlayerLevel() { return getLevel(); }
inline void* getLocalPlayer() {
    using GetPlayerFn = void* (*)();
    static auto fn = reinterpret_cast<GetPlayerFn>(dlsym(RTLD_DEFAULT, "ll_service_get_local_player"));
    return fn ? fn() : nullptr;
}
}
