# Elaborato2024 Simulation Project

OMNeT++ and INET project implementing an automotive Ethernet simulation with
IEEE 802.1Q tagging, TSN traffic classes, Credit-Based Shaper transmission
selection, and a custom flow-aware network layer.

Build and run instructions, configuration details, and architecture
documentation are provided in the [repository README](../README.md).

## Quick Start

From this directory:

```bash
source /path/to/omnetpp-6.0.3/setenv
export INET_ROOT="/path/to/omnetpp-6.0.3/samples/inet4.5"

cd src
opp_makemake -f --deep \
  -KINET4_5_PROJ="$INET_ROOT" \
  -DINET_IMPORT \
  -I'$(INET4_5_PROJ)/src' \
  -L'$(INET4_5_PROJ)/src' \
  -l'INET$(D)'

cd ..
make -j4 MODE=release

cd simulations
../src/Elaborato2024 -u Cmdenv -n .:../src omnetpp.ini
```

## Structure

- `src/application/` contains the application, socket, network layer, and
  message definitions.
- `simulations/` contains the network topology, runtime configuration, and
  analysis files.
- `out/` contains generated build artifacts.

The `EthernetDataFrames_m.h` and `EthernetDataFrames_m.cc` files are generated
from `EthernetDataFrames.msg` by `opp_msgtool` and must not be edited manually.
