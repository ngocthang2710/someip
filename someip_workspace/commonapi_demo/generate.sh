#!/bin/bash
CORE_GEN=~/someip_workspace/generators/commonapi-core-generator/commonapi-core-generator-linux-x86_64
SOMEIP_GEN=~/someip_workspace/generators/commonapi-someip-generator/commonapi-someip-generator-linux-x86_64

# Sinh code Core (Proxy, Stub, Types)
$CORE_GEN -sk ./fidl/Demo.fidl -d ./src-gen

# Sinh code Binding (Glue code kết nối vsomeip)
$SOMEIP_GEN ./fidl/Demo.fdepl -d ./src-gen
