package elaborato2024.simulations;

import inet.networks.base.WiredNetworkBase;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth1G;
import inet.node.tsn.TsnDevice;
import inet.node.inet.StandardHost;
import inet.node.tsn.TsnSwitch;


network TestNet extends WiredNetworkBase
{
    parameters:
        @display("bgb=985.29865,607.5784");
        **.bitrate = default(100Mbps);
    submodules:

        LD2: TsnDevice {
            @display("p=258.5,105.468");
        }
        US2: TsnDevice {
            @display("p=258.5,175.78");
        }
        CM1: TsnDevice {
            @display("p=258.5,256.432");
        }
        LD1: TsnDevice {
            @display("p=258.5,329.846");
        }
        US1: TsnDevice {
            @display("p=258.5,399.12402");
        }
        S2: TsnDevice {
            @display("p=409.46402,163.37201");
        }
        CU: TsnDevice {
            @display("p=356.73,250.22801");
        }
        S1: TsnDevice {
            @display("p=409.46402,348.458");
        }
        HU: TsnDevice {
            @display("p=503.558,286.418");
        }
        RS1: TsnDevice {
            @display("p=645.216,199.56201");
        }
        RS2: TsnDevice {
            @display("p=645.216,299.86002");
        }
        S4: TsnDevice {
            @display("p=739.2775,148.35");
        }
        S3: TsnDevice {
            @display("p=739.2775,347.38626");
        }
        US3: TsnDevice {
            @display("p=864.1388,79.12");
        }
        ME: TsnDevice {
            @display("p=874.02875,175.5475");
        }
        RC: TsnDevice {
            @display("p=881.4463,255.90376");
        }
        TLM: TsnDevice {
            @display("p=874.02875,335.02377");
        }
        US4: TsnDevice {
            @display("p=843.1225,417.8525");
        }
        SW1: TsnSwitch {
            @display("p=442.552,249.194");
        }
        SW2: TsnSwitch {
            @display("p=718.63,249.194");
        }
    connections:
        LD2.ethg++ <--> Eth100M <--> SW1.ethg++;
        SW1.ethg++ <--> Eth100M <--> CU.ethg++;
        LD1.ethg++ <--> Eth100M <--> SW1.ethg++;
        ME.ethg++ <--> Eth100M <--> SW2.ethg++;
        SW2.ethg++ <--> Eth100M <--> SW1.ethg++;
        SW2.ethg++ <--> Eth100M <--> S3.ethg++;
        SW2.ethg++ <--> Eth100M <--> S4.ethg++;
        SW1.ethg++ <--> Eth100M <--> S1.ethg++;
        SW1.ethg++ <--> Eth100M <--> S2.ethg++;
        US1.ethg++ <--> Eth100M <--> SW1.ethg++;
        US2.ethg++ <--> Eth100M <--> SW1.ethg++;
        US4.ethg++ <--> Eth100M <--> SW2.ethg++;
        US3.ethg++ <--> Eth100M <--> SW2.ethg++;
        SW1.ethg++ <--> Eth100M <--> HU.ethg++;
        CM1.ethg++ <--> Eth100M <--> SW1.ethg++;
        SW2.ethg++ <--> Eth100M <--> RS1.ethg++;
        SW2.ethg++ <--> Eth100M <--> RS2.ethg++;
        TLM.ethg++ <--> Eth100M <--> SW2.ethg++;
        RC.ethg++ <--> Eth100M <--> SW2.ethg++;
}