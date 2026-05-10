#include "kstdlib/debug.h"
#include "kstdlib/kio.h"
#include <stdint.h>
#include <devices/discovery.h>
#include <kstdlib/pair.h>
#include <kstdlib/maybe.h>

#define FDT_MAGIC 0xd00dfeed

#define FDT_NOP 0x00000004
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003

namespace devices {
using namespace kstd;

List<DeviceProbe*> DEVICE_PROBES;

struct fdt_header {
   uint32_t magic;
   uint32_t totalsize;
   uint32_t off_dt_struct;
   uint32_t off_dt_strings;
   uint32_t off_mem_rsvmap;
   uint32_t version;
   uint32_t last_comp_version;
   uint32_t boot_cpuid_phys;
   uint32_t size_dt_strings;
   uint32_t size_dt_struct;
};

struct property_desc {
    uint32_t len;
    uint32_t nameoff;
};

uintptr_t align_addr(uintptr_t addr) {
    addr += (4 - (addr % 4)) % 4;
    return addr;
}

uint32_t as_be_u32(uint32_t x) {
    uint32_t val = 0;
    val |= (x & 0xFF000000) >> 24;
    val |= (x & 0x00FF0000) >> 8;
    val |= (x & 0x0000FF00) << 8;
    val |= (x & 0x000000FF) << 24;
    return val;
}


DeviceTreeParser::DeviceTreeParser(const uint32_t *blob) {
    kprintf("starting parsing\n");
    const fdt_header *header = reinterpret_cast<const fdt_header*>(blob);
    ASSERT_MSG(as_be_u32(header->magic) == FDT_MAGIC, "Device Tree Blob header didn't contain correct magic");
    dt_strings_ = reinterpret_cast<const char*>(reinterpret_cast<const char*>(blob) + as_be_u32(header->off_dt_strings));
    current_ = reinterpret_cast<const uint32_t*>(reinterpret_cast<const char*>(blob) + as_be_u32(header->off_dt_struct));
    kprintf("starting to parse the dtb\n");
    root_ = parse_node().get();
    root_.dump();
}

void DeviceTreeParser::parse_nop() {
    while(*current_ == FDT_NOP) {
        current_++;
    }
}

const char* DeviceTreeParser::parse_str() {
    const char* str = reinterpret_cast<const char *>(current_);
    const char* curr = str;
    // TODO: this is def a disaster waiting to happen...
    // can overflow if this function is called in the wrong place...
    while(*(curr++) != '\0') {}
    current_ = reinterpret_cast<const uint32_t *>(align_addr(reinterpret_cast<uintptr_t>(curr)));
    return str;
}

Maybe<DTProperty> DeviceTreeParser::parse_property() {
    parse_nop();
    if (as_be_u32(*(current_++)) != FDT_PROP) {
        current_--;
        return Maybe<DTProperty>::nothing();
    }


    const property_desc *pd = reinterpret_cast<const property_desc *>(current_);
    const void *buffer = reinterpret_cast<const void*>(reinterpret_cast<const char*>(current_) + sizeof(property_desc));
    const char* name = dt_strings_ + as_be_u32(pd->nameoff);
    auto len = as_be_u32(pd->len);
    current_ = reinterpret_cast<const uint32_t *>(align_addr(reinterpret_cast<uintptr_t>(reinterpret_cast<const char *>(buffer) + len)));
    return Maybe<DTProperty>::just(DTProperty{.buffer = buffer, .len = len, .name = name});
}

Maybe<DTNode> DeviceTreeParser::parse_node() {
    parse_nop();
    if (as_be_u32(*(current_++)) != FDT_BEGIN_NODE) {
        current_--;
        return Maybe<DTNode>::nothing();
    }
    const char *name = parse_str();
    List<DTProperty> props;
    Maybe<DTProperty> currp = parse_property();
    while(!currp.is_nothing()) {
        props.push_back(currp.get());
        currp = parse_property();
    }
    List<DTNode> children;
    Maybe<DTNode> current = parse_node();

    while (!current.is_nothing()) {
        children.push_back(current.get());
        current = parse_node();
    }
    parse_nop();
    if (as_be_u32(*(current_++)) != FDT_END_NODE) {
        current_--;
        return Maybe<DTNode>::nothing();
    }

    auto node = Maybe<DTNode>::just(children, props, name);
    return node;

}
void DTNode::dump(int level) {
    char prefix[100];
    ASSERT(level < 100);
    for (int i = 0; i < level; ++i) prefix[i] = '-';
    prefix[level] = '\0';
    kprintf("%s Node: %s\n%s Properties:\n", prefix, name_, prefix);
    auto currp = properties_.head();
    while (currp) {
        kprintf("%s- %s\n", prefix, currp->name);
        currp = currp.next();
    }
    auto currn = children_.head();
    kprintf("%s Children:\n",prefix);
    if (!currn) {
        kprintf("%s- None\n", prefix);
    }
    while(currn) {
        currn->dump(level + 1);
        currn = currn.next();
    }

}

DTNode::DTNode(kstd::List<DTNode> children, kstd::List<DTProperty> properties, const char* name) : children_(children), properties_(properties), name_(name) {}
}
