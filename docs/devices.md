# Devices

Currently the kenrel focuses soley on aarch64 based systems, which overwhelmingly use device tree for their device specification. 
We'll focus our device discovery solely on it.

The Device Tree Blob is given to the kernel by limine on entry. Each discoverable node contains a "compatible" property, which lists the types of driver each device is compatable with. We define the following wrapper class for Device Tree nodes.

```c++
class Property {
    const void* buffer_;
    enum PropertyType {
        /* Collection of DeviceTree Property types */
    };
public:
    Property(const void* buffer) : buffer_(buffer) {}
    /* collection of getter functions for various device node property types made safe by the fact we store the property type */
};
class Node {
    List<Node> children_;
    List<Property> properities_;
public:
    Node(const void* start);
};
```

Once we create the tree, for each node, we'll search our system for a driver that wants to handle it
