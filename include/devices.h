#pragma once
#include "kstdlib/maybe.h"
#include <kstdlib/list.h>
#include <stdint.h>

namespace devices {

class DeviceTreeParser;
class Node;
struct Property {
    const void *buffer;
    uint32_t len;
    const char* name;
};
class Node {
    kstd::List<Node> children_;
    kstd::List<Property> properties_;
    const char* name_;
public:
    Node(kstd::List<Node> children, kstd::List<Property> properties, const char* name);
    Node() = default;
    void dump(int level = 0);
    friend DeviceTreeParser;
};

class DeviceTreeParser {
    const uint32_t *current_;
    const char *dt_strings_;

    Node root_;

    void parse_nop();
    const char * parse_str();
    kstd::Maybe<Node> parse_node();
    kstd::Maybe<Property> parse_property();
public:
    DeviceTreeParser(const uint32_t *blob);
};

}
