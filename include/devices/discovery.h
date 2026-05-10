#pragma once
#include "kstdlib/maybe.h"
#include <kstdlib/list.h>
#include <stdint.h>

#define REGISTER_DEVICE_PROBE(probe_class) \
    static struct probe_class##_registrar { \
        probe_class##_registrar() { \
            devices::DEVICE_PROBES.push_back(new probe_class()); \
        } \
    } probe_class##_registrar_instance;

namespace devices {

class DeviceTreeParser;
class DTNode;
struct DTProperty {
    const void *buffer;
    uint32_t len;
    const char* name;
};
class DTNode {
    kstd::List<DTNode> children_;
    kstd::List<DTProperty> properties_;
    const char* name_;
public:
    DTNode(kstd::List<DTNode> children, kstd::List<DTProperty> properties, const char* name);
    DTNode() = default;
    void dump(int level = 0);
    friend DeviceTreeParser;
};

class DeviceProbe {
public:
    virtual bool probe(const DTNode& node) = 0;
    virtual ~DeviceProbe() = default;
};

extern kstd::List<DeviceProbe*> DEVICE_PROBES;

class DeviceTreeParser {
    const uint32_t *current_;
    const char *dt_strings_;

    DTNode root_;

    void parse_nop();
    const char * parse_str();
    kstd::Maybe<DTNode> parse_node();
    kstd::Maybe<DTProperty> parse_property();
public:
    DeviceTreeParser(const uint32_t *blob);
};

}
